//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "es_wifi.h"
#include "es_wifi_io_chibios.h"
#include <string.h>
#include "es_wifi_conf.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t __attribute__((section(".ram4"))) txBuffer[1024];
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t __attribute__((section(".ram4"))) rxBuffer[220];

SPIDriver *spiDriver;
SPIConfig spiConfiguration = {0};
static int volatile spi_rx_event = 0;
static int volatile spi_tx_event = 0;
static int volatile cmddata_rdy_rising_event = 0;

TX_EVENT_FLAGS_GROUP wifiEvents;

static int wait_cmddata_rdy_high(int timeout);
static void SPI_WIFI_DelayUs(uint32_t);

///////////////
// definitions
#define CS_SELECT   palClearPad(PAL_PORT(LINE_ISM43362_CS), PAL_PAD(LINE_ISM43362_CS))
#define CS_UNSELECT palSetPad(PAL_PORT(LINE_ISM43362_CS), PAL_PAD(LINE_ISM43362_CS))

//  Initialize SPI3
int8_t SPI_WIFI_Init(uint16_t mode)
{
    if (mode == ES_WIFI_INIT)
    {
        // config GPIOs
        // clear wakeup signal to make sure we can wake up the module
        palClearLine(LINE_ISM43362_WAKEUP);
        // clear BOOT0 pin to allow clean boot of the module
        palClearLine(LINE_ISM43362_BOOT0);
        // set RST pin to high to make sure our reset transition is clean
        palSetLine(LINE_ISM43362_RST);

        // configure SPI driver
        spiDriver = &SPID3;

        // prescaller
        spiConfiguration.cr1 = SPI_CR1_BR_0;

        // 16bits
        spiConfiguration.cr2 = SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
        spiConfiguration.slave = false;

        spiStart(spiDriver, &spiConfiguration);

        // disable CS
        CS_UNSELECT;

        // first call used for calibration
        SPI_WIFI_DelayUs(10);
    }

    return SPI_WIFI_ResetModule();
}

int8_t SPI_WIFI_ResetModule(void)
{
    uint8_t count = 0;
    bool readSucceed = true;

    // clear the buffer to the prompt size (6 chars)
    memset(rxBuffer, 0, 6);

    WIFI_RESET_MODULE();

    // wait for CMDDATA_RDY reading high
    if (wait_cmddata_rdy_high(0xFF) != ES_WIFI_STATUS_OK)
    {
        return ES_WIFI_STATUS_ERROR;
    }

    CS_SELECT;

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
            readSucceed = true;
        }
    }

    CS_UNSELECT;

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
    int ticksToEnd = HAL_GetTick() + timeout;
    int currentTicks;

    while (WIFI_IS_CMDDATA_READY() == 0)
    {
        currentTicks = (int)HAL_GetTick();

        if (currentTicks > ticksToEnd)
        {
            return ES_WIFI_STATUS_ERROR;
        }

        if (currentTicks % 1000)
        {
            // pass control to the OS every second
            tx_thread_sleep(TX_TICKS_PER_MILLISEC(1));
        }
    }

    return ES_WIFI_STATUS_OK;
}

// Wait for CMD or DATA ready signal to rise
// @param  timeout : timeout to wait for the signal to go high in mS
int wait_cmddata_rdy_rising_event(int timeout)
{

#ifdef SEM_WAIT
    return SEM_WAIT(cmddata_rdy_rising_sem, timeout);
#else

    int ticksToEnd = HAL_GetTick() + timeout;
    int currentTicks;

    while (cmddata_rdy_rising_event == 1 && !WIFI_IS_CMDDATA_READY())
    {
        currentTicks = (int)HAL_GetTick();

        if (currentTicks > ticksToEnd)
        {
            return -1;
        }

        if (currentTicks % 1000)
        {
            // pass control to the OS every second
            tx_thread_sleep(TX_TICKS_PER_MILLISEC(1));
        }
    }

    cmddata_rdy_rising_event = 0;

    return 0;

#endif
}

int16_t SPI_WIFI_ReceiveData(uint8_t *data, uint16_t len, uint32_t timeout)
{
    int16_t length = 0;
    uint8_t tmp[2];

    CS_UNSELECT;

    SPI_WIFI_DelayUs(3);

    if (wait_cmddata_rdy_rising_event(timeout) < 0)
    {
        return ES_WIFI_ERROR_WAITING_DRDY_FALLING;
    }

    // spiAcquireBus(spiDriver);

    CS_SELECT;
    SPI_WIFI_DelayUs(15);

    while (WIFI_IS_CMDDATA_READY())
    {
        if ((length < len) || (!len))
        {
            spi_rx_event = 1;

            if (spiReceive(spiDriver, 1, &tmp[0]) != HAL_RET_SUCCESS)
            {
                CS_UNSELECT;
                // spiReleaseBus(spiDriver);

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
                // spiReleaseBus(spiDriver);

                return ES_WIFI_ERROR_STUFFING_FOREVER;
            }
        }
        else
        {
            break;
        }
    }

    CS_UNSELECT;

    // spiReleaseBus(spiDriver);

    return length;
}

// Send wifi Data thru SPI
// @param  data : pointer to data
// @param  len : Data length
// @param  timeout : send timeout in mS
// @retval Length of sent data
int16_t SPI_WIFI_SendData(uint8_t *data, uint16_t len, uint32_t timeout)
{
    // uint8_t waitResult = TX_WAIT_ERROR;
    // uint32_t dummy;

    if (wait_cmddata_rdy_high(timeout) < 0)
    {
        return ES_WIFI_ERROR_SPI_FAILED;
    }

    // arm to detect rising event
    cmddata_rdy_rising_event = 1;
    // spiAcquireBus(spiDriver);

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

        spi_tx_event = 1;

        if (spiSend(spiDriver, len / 2, txBuffer) != HAL_RET_SUCCESS)
        {
            CS_UNSELECT;
            // spiReleaseBus(spiDriver);

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
    HAL_Delay(delay);
}

// Delay
// @param  Delay in us
// @retval None
void SPI_WIFI_DelayUs(uint32_t n)
{
    volatile uint32_t ct = 0;
    uint32_t loop_per_us = 0;
    static uint32_t cycle_per_loop = 0;

    // calibration happen on first call for a duration of 1 ms * nbcycle per loop
    // 10 cycle for STM32L4
    if (cycle_per_loop == 0)
    {
        uint32_t cycle_per_ms = (SystemCoreClock / 1000UL);
        uint32_t t = 0;

        ct = cycle_per_ms;
        t = HAL_GetTick();

        while (ct)
        {
            ct--;
        }

        cycle_per_loop = HAL_GetTick() - t;

        if (cycle_per_loop == 0)
        {
            cycle_per_loop = 1;
        }
    }

    loop_per_us = SystemCoreClock / 1000000UL / cycle_per_loop;
    ct = n * loop_per_us;

    while (ct)
    {
        ct--;
    }

    return;
}

// Interrupt handler for SPI RX and TX events
void SPI_WIFI_Callback(SPIDriver *spip)
{
    (void)spip;

    if (spi_rx_event)
    {
        tx_event_flags_set(&wifiEvents, WIFI_RX_EVENT_FLAG, TX_OR);
        spi_rx_event = 0;
    }
    else if (spi_tx_event)
    {
        tx_event_flags_set(&wifiEvents, WIFI_TX_EVENT_FLAG, TX_OR);
        spi_tx_event = 0;
    }
}
