//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "es_wifi.h"
#include "es_wifi_io_chibios.h"
#include <string.h>
#include "es_wifi_conf.h"
#include <hal.h>
#include <targetHAL_Watchdog.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t txBuffer[1024];
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t rxBuffer[220];

SPIDriver *spiDriver;
SPIConfig spiConfiguration = {0};

// ChibiOS port: WiFiMutex is declared extern in es_wifi_conf.h (LOCK_WIFI()/UNLOCK_WIFI()) and defined here
mutex_t WiFiMutex;

static int wait_cmddata_rdy_high(int timeout);
static void SPI_WIFI_DelayUs(uint32_t);

// Edge counter for CMDDATA_RDY, incremented from ISR on every rising edge (EXTI). Needed to tell
// a genuinely NEW response apart from the line simply still being high from an earlier one - a
// plain level check can't distinguish the two.
static volatile uint32_t cmddataRdyEdgeCount = 0;

static void CmdDataRdyEdgeCallback(void *arg)
{
    (void)arg;
    cmddataRdyEdgeCount++;
}

///////////////
// definitions
#define CS_SELECT   palClearPad(PAL_PORT(LINE_ISM43362_CS), PAL_PAD(LINE_ISM43362_CS))
#define CS_UNSELECT palSetPad(PAL_PORT(LINE_ISM43362_CS), PAL_PAD(LINE_ISM43362_CS))

// returns the current system tick count in milliseconds (ChibiOS equivalent of ST HAL's HAL_GetTick())
static uint32_t GetTick(void)
{
    return (uint32_t)TIME_I2MS(chVTGetSystemTimeX());
}

//  Initialize SPI3
int8_t SPI_WIFI_Init(uint16_t mode)
{
    ISM43362_DebugPrintf("[ISM43362] SPI_WIFI_Init: mode=%d\r\n", (int)mode);

    if (mode == ES_WIFI_INIT)
    {
        // one-time initialization of the mutex protecting access to the ES-WIFI module
        chMtxObjectInit(&WiFiMutex);

        // config GPIOs
        // clear wakeup signal to make sure we can wake up the module
        palClearLine(LINE_ISM43362_WAKEUP);
        // clear BOOT0 pin to allow clean boot of the module
        palClearLine(LINE_ISM43362_BOOT0);
        // set RST pin to high to make sure our reset transition is clean
        palSetLine(LINE_ISM43362_RST);

        // configure SPI driver
        spiDriver = &SPID3;

        // /16 (5MHz) - /4 (20MHz) caused intermittent SPI data corruption on real hardware
        // (bad scan counts, STUFFING_FOREVER errors)
        spiConfiguration.cr1 = SPI_CR1_BR_1 | SPI_CR1_BR_0;

        // 16bits
        spiConfiguration.cr2 = SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
        spiConfiguration.slave = false;

        spiStart(spiDriver, &spiConfiguration);

        // disable CS
        CS_UNSELECT;

        // capture every rising edge of CMDDATA_RDY from here on, via EXTI interrupt, so
        // wait_cmddata_rdy_rising_event() can reliably detect a genuinely NEW response becoming
        // ready rather than guessing from the line's current (possibly already-high) level
        cmddataRdyEdgeCount = 0;
        palSetLineCallback(LINE_ISM43362_CMDTRDY, CmdDataRdyEdgeCallback, NULL);
        palEnableLineEvent(LINE_ISM43362_CMDTRDY, PAL_EVENT_MODE_RISING_EDGE);

        // first call used for calibration
        SPI_WIFI_DelayUs(10);
    }

    ISM43362_DebugPrintf("[ISM43362] SPI_WIFI_Init: calling SPI_WIFI_ResetModule()...\r\n");

    int8_t resetResult = SPI_WIFI_ResetModule();

    ISM43362_DebugPrintf("[ISM43362] SPI_WIFI_Init: SPI_WIFI_ResetModule() returned %d\r\n", (int)resetResult);

    return resetResult;
}

int8_t SPI_WIFI_ResetModule(void)
{
    uint8_t count = 0;
    bool readSucceed = true;

    // clear the buffer to the prompt size (6 chars)
    memset(rxBuffer, 0, 6);

    ISM43362_DebugPrintf("[ISM43362] SPI_WIFI_ResetModule: toggling RST line...\r\n");

    WIFI_RESET_MODULE();

    // wait for CMDDATA_RDY reading high
    if (wait_cmddata_rdy_high(0xFF) != ES_WIFI_STATUS_OK)
    {
        ISM43362_DebugPrintf("[ISM43362] SPI_WIFI_ResetModule: timed out waiting for CMDDATA_RDY high\r\n");
        return ES_WIFI_STATUS_ERROR;
    }

    ISM43362_DebugPrintf("[ISM43362] SPI_WIFI_ResetModule: CMDDATA_RDY is high, draining prompt...\r\n");

    CS_SELECT;

    // drain the reset prompt ("\x15\x15\r\n> ") - bounded by count and a wall-clock timeout so a
    // stuck CMDDATA_RDY line can't turn this into an infinite busy-loop
    uint32_t drainStart = GetTick();

    while (WIFI_IS_CMDDATA_READY() && readSucceed)
    {
        // bail out before a batch write could overflow rxBuffer - tied to the buffer's actual
        // size (rather than a magic number) so this stays safe if either one changes later
        if ((size_t)count + 2 * 3 > sizeof(rxBuffer))
        {
            readSucceed = false;
            break;
        }

        // read in batches of 6 bytes (which are passed as 3 because we're using 16bit mode)
        if (spiReceive(spiDriver, 3, &rxBuffer[count]) == MSG_OK)
        {
            count += 2 * 3;
        }
        else
        {
            readSucceed = false;
        }

        if ((uint32_t)(GetTick() - drainStart) > 1000)
        {
            readSucceed = false;
            break;
        }
    }

    CS_UNSELECT;

    ISM43362_DebugPrintf(
        "[ISM43362] SPI_WIFI_ResetModule: drain done, count=%d, readSucceed=%d, bytes=%02X %02X %02X %02X %02X "
        "%02X\r\n",
        (int)count,
        (int)readSucceed,
        rxBuffer[0],
        rxBuffer[1],
        rxBuffer[2],
        rxBuffer[3],
        rxBuffer[4],
        rxBuffer[5]);

    if (!readSucceed || (rxBuffer[0] != 0x15) || (rxBuffer[1] != 0x15) || (rxBuffer[2] != '\r') ||
        (rxBuffer[3] != '\n') || (rxBuffer[4] != '>') || (rxBuffer[5] != ' '))
    {
        return ES_WIFI_STATUS_ERROR;
    }

    return ES_WIFI_STATUS_OK;
}

// DeInitialize the SPI
// @param  None
// @retval None
int8_t SPI_WIFI_DeInit(void)
{
    // stop delivering CMDDATA_RDY edges - otherwise CmdDataRdyEdgeCallback() keeps firing and
    // incrementing cmddataRdyEdgeCount after this deinit, and a later re-Init(ES_WIFI_RESET)
    // would see stale edges left over from before this call
    palDisableLineEvent(LINE_ISM43362_CMDTRDY);

    if (spiDriver != NULL)
    {
        spiStop(spiDriver);
    }

    return ES_WIFI_STATUS_OK;
}

// Wait for CMD or DATA ready signal
// @param  timeout : timeout to wait for the signal to be asserted in mS
int wait_cmddata_rdy_high(int timeout)
{
    uint32_t startTick = GetTick();
    uint32_t lastWatchdogFeed = startTick;

    while (WIFI_IS_CMDDATA_READY() == 0)
    {
        if ((uint32_t)(GetTick() - startTick) > (uint32_t)timeout)
        {
            return ES_WIFI_STATUS_ERROR;
        }

        // feed the watchdog periodically while polling - some operations (e.g. a full WiFi
        // scan) legitimately wait here far longer than the ~10s IWDG timeout would otherwise
        // allow, and this loop doesn't go through AT_ExecuteCommand() (where the normal
        // per-command watchdog feed happens)
        if ((uint32_t)(GetTick() - lastWatchdogFeed) > 500)
        {
            Watchdog_Reset();
            lastWatchdogFeed = GetTick();
        }

        // pass control to the OS
        chThdSleepMilliseconds(1);
    }

    return ES_WIFI_STATUS_OK;
}

// Wait for a genuinely NEW rising edge on CMDDATA_RDY (a fresh response becoming ready), not just
// for the line to currently read high (which could be stale leftover from an earlier response).
// Backed by the real EXTI edge counter above.
// @param  timeout : timeout to wait for a new edge, in mS
int wait_cmddata_rdy_rising_event(int timeout)
{
    uint32_t startTick = GetTick();
    uint32_t lastWatchdogFeed = startTick;
    uint32_t startCount = cmddataRdyEdgeCount;

    while (cmddataRdyEdgeCount == startCount)
    {
        if ((uint32_t)(GetTick() - startTick) > (uint32_t)timeout)
        {
            ISM43362_DebugPrintf(
                "[ISM43362] wait_cmddata_rdy_rising_event: TIMED OUT after %d ms waiting for a new edge\r\n",
                (int)(GetTick() - startTick));
            return -1;
        }

        // feed the watchdog periodically while polling - see comment in wait_cmddata_rdy_high()
        if ((uint32_t)(GetTick() - lastWatchdogFeed) > 500)
        {
            Watchdog_Reset();
            lastWatchdogFeed = GetTick();
        }

        // pass control to the OS
        chThdSleepMilliseconds(1);
    }

    return 0;
}

int16_t SPI_WIFI_ReceiveData(uint8_t *data, uint16_t len, uint32_t timeout)
{
    int16_t length = 0;
    uint8_t tmp[2];

    CS_UNSELECT;

    SPI_WIFI_DelayUs(3);

    // wait for a genuinely NEW rising edge on CMDDATA_RDY (this command's response becoming
    // ready) - a plain level check can't tell that apart from stale leftover data
    if (wait_cmddata_rdy_rising_event((int)timeout) < 0)
    {
        // module can get stuck with CMDDATA_RDY fixed forever (e.g. after a failed/hung join),
        // which would time out every subsequent command too - reset so the next command at
        // least starts from a known-good state
        SPI_WIFI_ResetModule();

        return ES_WIFI_ERROR_WAITING_DRDY_FALLING;
    }

    CS_SELECT;
    SPI_WIFI_DelayUs(15);

    while (WIFI_IS_CMDDATA_READY())
    {
        if ((length < len) || (!len))
        {
            if (spiReceive(spiDriver, 1, &tmp[0]) != MSG_OK)
            {
                CS_UNSELECT;

                return ES_WIFI_ERROR_SPI_FAILED;
            }

            // always drain both bytes of this 16bit-mode SPI unit, but only commit the second
            // one to the caller's buffer if it's actually within the requested len - otherwise,
            // for an odd len, this would write one byte past the end of the caller's buffer
            data[0] = tmp[0];
            if ((!len) || ((length + 1) < len))
            {
                data[1] = tmp[1];
            }
            length += 2;
            data += 2;

            if (length >= ES_WIFI_DATA_SIZE)
            {
                CS_UNSELECT;
                SPI_WIFI_ResetModule();

                return ES_WIFI_ERROR_STUFFING_FOREVER;
            }
        }
        else
        {
            break;
        }
    }

    CS_UNSELECT;

    return length;
}

// Send wifi Data thru SPI
// @param  data : pointer to data
// @param  len : Data length
// @param  timeout : send timeout in mS
// @retval Length of sent data
int16_t SPI_WIFI_SendData(uint8_t *data, uint16_t len, uint32_t timeout)
{
    if (wait_cmddata_rdy_high((int)timeout) != ES_WIFI_STATUS_OK)
    {
        return ES_WIFI_ERROR_SPI_FAILED;
    }

    CS_SELECT;

    SPI_WIFI_DelayUs(15);

    if (len > 0)
    {
        // txBuffer is a fixed-size buffer - reject anything that wouldn't fit instead of
        // overflowing it (the -1 accounts for the odd-length filler byte written below)
        if (len > sizeof(txBuffer) - 1)
        {
            CS_UNSELECT;

            return ES_WIFI_ERROR_SPI_FAILED;
        }

        memcpy(txBuffer, data, len);

        if (len % 2)
        {
            // buffer has odd length, append a filler char to bTx
            txBuffer[len] = 0x0A;

            // adjust length
            len++;
        }

        if (spiSend(spiDriver, len / 2, txBuffer) != MSG_OK)
        {
            CS_UNSELECT;

            return ES_WIFI_ERROR_SPI_FAILED;
        }
    }

    CS_UNSELECT;

    return len;
}

// Delay
// @param  Delay in ms
// @retval None
void SPI_WIFI_Delay(uint32_t delay)
{
    chThdSleepMilliseconds(delay);
}

// Delay
// @param  Delay in us
// @retval None
void SPI_WIFI_DelayUs(uint32_t n)
{
    // ChibiOS provides a calibrated busy-wait based on the core cycle counter, so there is no
    // need for the manual calibration loop used by the original ST HAL based implementation
    chSysPolledDelayX(US2RTC(STM32_SYSCLK, n));
}
