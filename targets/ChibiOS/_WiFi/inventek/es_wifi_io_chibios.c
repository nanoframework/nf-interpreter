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

// Genuine edge counter for the CMDDATA_RDY line, incremented from ISR context every time a
// RISING EDGE is detected on the line (via EXTI). This mirrors the original STM32Cube HAL
// reference driver (es_wifi_io_stm.c), which uses a real GPIO_MODE_IT_RISING interrupt +
// semaphore for this exact purpose. A polling approximation that only looks at the CURRENT
// level of the line cannot distinguish "still high from an earlier, already-consumed response"
// from "a genuinely new response has just become ready" - both look identical to a level check.
// Counting actual edges removes that ambiguity entirely.
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

        // prescaler - PCLK1 on this board runs undivided at the 80MHz SYSCLK (STM32_PPRE1_DIV1),
        // so SPI_CR1_BR_0 alone (/4) would clock SPI3 at 20MHz. Scan/connect testing on real
        // hardware showed intermittent data corruption (inconsistent access-point counts across
        // otherwise-identical scans, occasional runaway reads hitting the STUFFING_FOREVER safety
        // net) consistent with the SPI clock being too fast for reliable communication with this
        // module on this board - use /16 (5MHz) instead for a much larger timing margin.
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
        palEnableLineEvent(LINE_ISM43362_CMDTRDY, PAL_EVENT_MODE_RISING_EDGE);
        palSetLineCallback(LINE_ISM43362_CMDTRDY, CmdDataRdyEdgeCallback, NULL);

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

    // drain the reset prompt ("\x15\x15\r\n> ") - bounded by both a byte count and a wall-clock
    // timeout so a CMDDATA_RDY line that (for whatever reason - SPI/timing mismatch, module not
    // actually reset, etc.) never drops back to idle can't turn this into an infinite busy-loop
    // that starves the rest of the RTOS (and the debugger monitor thread with it).
    uint32_t resetDrainDeadline = GetTick() + 1000;

    while (WIFI_IS_CMDDATA_READY() && readSucceed)
    {
        // read in batches of 6 bytes (which are passed as 3 because we're using 16bit mode)
        if (spiReceive(spiDriver, 3, &rxBuffer[count]) == MSG_OK)
        {
            count += 2 * 3;
        }
        else
        {
            readSucceed = false;
        }

        // sanity check for buffer overflow because of repeated operations reading garbage data
        if (count > 200)
        {
            readSucceed = false;
            break;
        }

        if (GetTick() > resetDrainDeadline)
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
    spiStop(spiDriver);

    return ES_WIFI_STATUS_OK;
}

// Wait for CMD or DATA ready signal
// @param  timeout : timeout to wait for the signal to be asserted in mS
int wait_cmddata_rdy_high(int timeout)
{
    uint32_t ticksToEnd = GetTick() + (uint32_t)timeout;
    uint32_t lastWatchdogFeed = GetTick();

    while (WIFI_IS_CMDDATA_READY() == 0)
    {
        if (GetTick() > ticksToEnd)
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

// Wait for a genuinely NEW rising edge on CMDDATA_RDY (i.e. the module signaling that a fresh
// response has become ready) - NOT merely for the line to currently read high, which could just
// as easily be leftover from an earlier, already-fully-read response. Backed by a real EXTI
// interrupt edge counter (see CmdDataRdyEdgeCallback above), matching the original STM32Cube HAL
// reference driver's semaphore-based wait_cmddata_rdy_rising_event() semantics exactly.
// @param  timeout : timeout to wait for a new edge, in mS
int wait_cmddata_rdy_rising_event(int timeout)
{
    uint32_t startTick = GetTick();
    uint32_t ticksToEnd = startTick + (uint32_t)timeout;
    uint32_t lastWatchdogFeed = startTick;
    uint32_t startCount = cmddataRdyEdgeCount;

    while (cmddataRdyEdgeCount == startCount)
    {
        if (GetTick() > ticksToEnd)
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

    // wait for a genuinely NEW rising edge on CMDDATA_RDY (i.e. THIS command's response
    // becoming ready) - matches the original STM32Cube HAL reference driver's structure
    // exactly. Earlier attempts at guessing from the line's CURRENT level (e.g. "if already
    // high on entry, assume it's stale leftover data and drain it") were fundamentally
    // ambiguous - a fast module response and genuinely stale leftover data both look identical
    // to a level check. The edge-counter-backed wait removes that ambiguity: it only returns
    // once a NEW edge has actually been observed since this call started waiting.
    if (wait_cmddata_rdy_rising_event((int)timeout) < 0)
    {
        // CMDDATA_RDY never produced a new edge within the timeout - confirmed on real hardware
        // that once this happens (e.g. after a WiFi join that failed/hung on the module's own
        // side), the module can get stuck with CMDDATA_RDY sitting at a fixed level forever,
        // meaning every SUBSEQUENT command would time out identically too, with no recovery for
        // the rest of the session. Reset the module here (matching the existing STUFFING_FOREVER
        // recovery below) so the NEXT command at least starts from a known-good state instead of
        // being stuck permanently.
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
            data[0] = tmp[0];
            data[1] = tmp[1];
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
    if (wait_cmddata_rdy_high((int)timeout) < 0)
    {
        return ES_WIFI_ERROR_SPI_FAILED;
    }

    CS_SELECT;

    SPI_WIFI_DelayUs(15);

    if (len > 1)
    {
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
