//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_gecko_spi_driver.h"

#if defined(EUSART_PRESENT)
#error "Only USART type is supported. Drivar can't driver EUSART."
#endif

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "em_device.h"
#include "em_gpio.h"
#include "em_core.h"
#include "em_usart.h"
#include "dmadrv.h"
#include "nf_gecko_spi_driver.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif
#include <string.h>

#if defined(DMA_PRESENT) && (DMA_COUNT == 1)
#define SPI_DMA_IRQ DMA_IRQn

#elif defined(LDMA_PRESENT) && (LDMA_COUNT == 1)
#define SPI_DMA_IRQ LDMA_IRQn

#else
#error "No valid SPIDRV DMA engine defined."
#endif

#define EMDRV_SPIDRV_USART_FRAMELENGTH_REGVALUE_OFFSET  (3U)
#define EMDRV_SPIDRV_EUSART_FRAMELENGTH_REGVALUE_OFFSET (6U)

// SPI Pins structure used when mapping from location to gpio port+pin.
typedef struct
{
    uint8_t mosiPort;
    uint8_t mosiPin;
    uint8_t misoPort;
    uint8_t misoPin;
    uint8_t clkPort;
    uint8_t clkPin;
    uint8_t csPort;
    uint8_t csPin;
} SPI_Pins_t;

static bool spidrvIsInitialized = false;

static Ecode_t NF_SpiDriver_InitUsart(NF_SpiDriver_Handle_t handle, NF_SpiDriver_Init_t *initData);

static void BlockingComplete(NF_SpiDriver_Handle_t handle, Ecode_t transferStatus, int itemsTransferred);

static Ecode_t ConfigGPIO(NF_SpiDriver_Handle_t handle, bool enable);

static bool RxDMAComplete(unsigned int channel, unsigned int sequenceNo, void *userParam);

static void StartReceiveDMA(NF_SpiDriver_Handle_t handle, void *buffer, int count, NF_SpiDriver_Callback_t callback);

static void StartTransferDMA(
    NF_SpiDriver_Handle_t handle,
    const void *txBuffer,
    void *rxBuffer,
    int count,
    NF_SpiDriver_Callback_t callback);

static void StartTransmitDMA(
    NF_SpiDriver_Handle_t handle,
    const void *buffer,
    int count,
    NF_SpiDriver_Callback_t callback);

static Ecode_t TransferApiPrologue(NF_SpiDriver_Handle_t handle, void *buffer, int count);

static Ecode_t TransferApiBlockingPrologue(NF_SpiDriver_Handle_t handle, void *buffer, int count);

static void WaitForTransferCompletion(NF_SpiDriver_Handle_t handle);

//
// Power management functions.
//
static void em1RequestAdd(NF_SpiDriver_Handle_t handle)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    if (handle->em1RequestCount == 0)
    {
        sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
    }
    handle->em1RequestCount++;
    CORE_EXIT_ATOMIC();
#else
    handle->em1RequestCount++;
#endif
}

static void em1RequestRemove(NF_SpiDriver_Handle_t handle)
{
    EFM_ASSERT(handle->em1RequestCount > 0);
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    handle->em1RequestCount--;
    if (handle->em1RequestCount == 0)
    {
        sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
    }
    CORE_EXIT_ATOMIC();
#else
    handle->em1RequestCount--;
#endif
}

static void emRequestInit(NF_SpiDriver_Handle_t handle)
{
    handle->em1RequestCount = 0;
}

static void emRequestDeinit(NF_SpiDriver_Handle_t handle)
{
    if (handle->em1RequestCount > 0)
    {
        handle->em1RequestCount = 1;
        em1RequestRemove(handle);
    }
}

//
//
//    Initialize an SPI driver instance.
//
// @param[out] handle  Pointer to an SPI driver handle; refer to @ref
//                     NF_SpiDriver_Handle_t.
// @param[in] initData Pointer to an initialization data structure;
//                     refer to @ref NF_SpiDriver_Init.
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
//    SPIDRV @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_Init(NF_SpiDriver_Handle_t handle, NF_SpiDriver_Init_t *initData)
{
    return NF_SpiDriver_InitUsart(handle, initData);
}

//
//
//    Initialize an SPI driver usart instance.
//
// @param[out] handle  Pointer to an SPI driver handle; refer to @ref
//                     NF_SpiDriver_Handle_t.
//
// @param[in] initData Pointer to an initialization data structure;
//                     refer to @ref NF_SpiDriver_Init.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
//    SPIDRV @ref Ecode_t is returned.
//
static Ecode_t NF_SpiDriver_InitUsart(NF_SpiDriver_Handle_t handle, NF_SpiDriver_Init_t *initData)
{
    Ecode_t retVal;
    CORE_DECLARE_IRQ_STATE;
    USART_InitSync_TypeDef usartInit = USART_INITSYNC_DEFAULT;
#if defined(_SILICON_LABS_32B_SERIES_2)
    int8_t spiPortNum = -1;
#endif

    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    if (initData == NULL)
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    memset(handle, 0, sizeof(NF_SpiDriver_HandleData_t));
    emRequestInit(handle);

    if (0)
    {
#if defined(USART0)
    }
    else if ((USART_TypeDef *)initData->port == USART0)
    {
        handle->usartClock = cmuClock_USART0;
        handle->txDMASignal = dmadrvPeripheralSignal_USART0_TXBL;
#if defined(_SILICON_LABS_32B_SERIES_2)
        handle->rxDMASignal = dmadrvPeripheralSignal_USART0_RXDATAV;
        spiPortNum = 0;
#else
        handle->rxDMASignal = dmadrvPeripheralSignal_USART0_RXDATAV;
#endif
#endif
#if defined(USART1)
    }
    else if ((USART_TypeDef *)initData->port == USART1)
    {
        handle->usartClock = cmuClock_USART1;
        handle->txDMASignal = dmadrvPeripheralSignal_USART1_TXBL;
#if defined(_SILICON_LABS_32B_SERIES_2)
        handle->rxDMASignal = dmadrvPeripheralSignal_USART1_RXDATAV;
        spiPortNum = 1;
#else
        handle->rxDMASignal = dmadrvPeripheralSignal_USART1_RXDATAV;
#endif
#endif
#if defined(USART2)
    }
    else if ((USART_TypeDef *)initData->port == USART2)
    {
        handle->usartClock = cmuClock_USART2;
        handle->txDMASignal = dmadrvPeripheralSignal_USART2_TXBL;
#if defined(_SILICON_LABS_32B_SERIES_2)
        handle->rxDMASignal = dmadrvPeripheralSignal_USART2_RXDATAV;
        spiPortNum = 2;
#else
        handle->rxDMASignal = dmadrvPeripheralSignal_USART2_RXDATAV;
#endif
#endif
#if defined(USART3)
    }
    else if ((USART_TypeDef *)initData->port == USART3)
    {
        handle->usartClock = cmuClock_USART3;
        handle->txDMASignal = dmadrvPeripheralSignal_USART3_TXBL;
        handle->rxDMASignal = dmadrvPeripheralSignal_USART3_RXDATAV;
#if defined(_SILICON_LABS_32B_SERIES_2)
        spiPortNum = 3;
#endif
#endif
#if defined(USART4)
    }
    else if ((USART_TypeDef *)initData->port == USART4)
    {
        handle->usartClock = cmuClock_USART4;
        handle->txDMASignal = dmadrvPeripheralSignal_USART4_TXBL;
        handle->rxDMASignal = dmadrvPeripheralSignal_USART4_RXDATAV;
#if defined(_SILICON_LABS_32B_SERIES_2)
        spiPortNum = 4;
#endif
#endif
#if defined(USART5)
    }
    else if ((USART_TypeDef *)initData->port == USART5)
    {
        handle->usartClock = cmuClock_USART5;
        handle->txDMASignal = dmadrvPeripheralSignal_USART5_TXBL;
        handle->rxDMASignal = dmadrvPeripheralSignal_USART5_RXDATAV;
#if defined(_SILICON_LABS_32B_SERIES_2)
        spiPortNum = 5;
#endif
#endif
#if defined(USARTRF0)
    }
    else if ((USART_TypeDef *)initData->port == USARTRF0)
    {
        handle->usartClock = cmuClock_USARTRF0;
        handle->txDMASignal = dmadrvPeripheralSignal_USARTRF0_TXBL;
        handle->rxDMASignal = dmadrvPeripheralSignal_USARTRF0_RXDATAV;
#endif
#if defined(USARTRF1)
    }
    else if ((USART_TypeDef *)initData->port == USARTRF1)
    {
        handle->usartClock = cmuClock_USARTRF1;
        handle->txDMASignal = dmadrvPeripheralSignal_USARTRF1_TXBL;
        handle->rxDMASignal = dmadrvPeripheralSignal_USARTRF1_RXDATAV;
#endif
    }
    else
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    handle->peripheral.usartPort = (USART_TypeDef *)initData->port;
    handle->peripheralType = spidrvPeripheralTypeUsart;
    handle->initData = initData;

    if (initData->bitOrder == spidrvBitOrderMsbFirst)
    {
        usartInit.msbf = true;
    }

    if (initData->clockMode == spidrvClockMode0)
    {
        usartInit.clockMode = usartClockMode0;
    }
    else if (initData->clockMode == spidrvClockMode1)
    {
        usartInit.clockMode = usartClockMode1;
    }
    else if (initData->clockMode == spidrvClockMode2)
    {
        usartInit.clockMode = usartClockMode2;
    }
    else if (initData->clockMode == spidrvClockMode3)
    {
        usartInit.clockMode = usartClockMode3;
    }
    else
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    usartInit.master = true;
    usartInit.baudrate = initData->bitRate;

#if defined(_CMU_HFPERCLKEN0_MASK)
    CMU_ClockEnable(cmuClock_HFPER, true);
#endif
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(handle->usartClock, true);

    if ((initData->frameLength < 4U) || (initData->frameLength > 16U))
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }
    uint32_t databits = initData->frameLength - 4U + _USART_FRAME_DATABITS_FOUR;
    usartInit.databits = (USART_Databits_TypeDef)databits;

    USART_InitSync((USART_TypeDef *)initData->port, &usartInit);

    if (initData->csControl == spidrvCsControlAuto)
    {
        handle->peripheral.usartPort->CTRL |= USART_CTRL_AUTOCS;
    }

    if (initData->csControl == spidrvCsControlAuto)
    {
        // SPI 4 wire mode
#if defined(USART_ROUTEPEN_TXPEN)
        handle->peripheral.usartPort->ROUTELOC0 =
            (handle->peripheral.usartPort->ROUTELOC0 & ~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK |
                                                         _USART_ROUTELOC0_CLKLOC_MASK | _USART_ROUTELOC0_CSLOC_MASK)) |
            (initData->portLocationTx << _USART_ROUTELOC0_TXLOC_SHIFT) |
            (initData->portLocationRx << _USART_ROUTELOC0_RXLOC_SHIFT) |
            (initData->portLocationClk << _USART_ROUTELOC0_CLKLOC_SHIFT) |
            (initData->portLocationCs << _USART_ROUTELOC0_CSLOC_SHIFT);

        handle->peripheral.usartPort->ROUTEPEN =
            USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN;
#elif defined(_GPIO_USART_ROUTEEN_MASK)
        GPIO->USARTROUTE[spiPortNum].ROUTEEN =
            GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_CLKPEN | GPIO_USART_ROUTEEN_CSPEN;

        GPIO->USARTROUTE[spiPortNum].TXROUTE = ((uint32_t)initData->portTx << _GPIO_USART_TXROUTE_PORT_SHIFT) |
                                               ((uint32_t)initData->pinTx << _GPIO_USART_TXROUTE_PIN_SHIFT);

        GPIO->USARTROUTE[spiPortNum].RXROUTE = ((uint32_t)initData->portRx << _GPIO_USART_RXROUTE_PORT_SHIFT) |
                                               ((uint32_t)initData->pinRx << _GPIO_USART_RXROUTE_PIN_SHIFT);

        GPIO->USARTROUTE[spiPortNum].CLKROUTE = ((uint32_t)initData->portClk << _GPIO_USART_CLKROUTE_PORT_SHIFT) |
                                                ((uint32_t)initData->pinClk << _GPIO_USART_CLKROUTE_PIN_SHIFT);

        GPIO->USARTROUTE[spiPortNum].CSROUTE = ((uint32_t)initData->portCs << _GPIO_USART_CSROUTE_PORT_SHIFT) |
                                               ((uint32_t)initData->pinCs << _GPIO_USART_CSROUTE_PIN_SHIFT);
#else
        handle->peripheral.usartPort->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN |
                                              USART_ROUTE_CSPEN |
                                              (initData->portLocation << _USART_ROUTE_LOCATION_SHIFT);
#endif
    }
    else
    {
        // SPI 3 wire mode
#if defined(USART_ROUTEPEN_TXPEN)
        handle->peripheral.usartPort->ROUTELOC0 =
            (handle->peripheral.usartPort->ROUTELOC0 &
             ~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK | _USART_ROUTELOC0_CLKLOC_MASK)) |
            (initData->portLocationTx << _USART_ROUTELOC0_TXLOC_SHIFT) |
            (initData->portLocationRx << _USART_ROUTELOC0_RXLOC_SHIFT) |
            (initData->portLocationClk << _USART_ROUTELOC0_CLKLOC_SHIFT);

        handle->peripheral.usartPort->ROUTEPEN = USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CLKPEN;
#elif defined(GPIO_USART_ROUTEEN_TXPEN)
        GPIO->USARTROUTE[spiPortNum].ROUTEEN =
            GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_CLKPEN;

        GPIO->USARTROUTE[spiPortNum].TXROUTE = ((uint32_t)initData->portTx << _GPIO_USART_TXROUTE_PORT_SHIFT) |
                                               ((uint32_t)initData->pinTx << _GPIO_USART_TXROUTE_PIN_SHIFT);

        GPIO->USARTROUTE[spiPortNum].RXROUTE = ((uint32_t)initData->portRx << _GPIO_USART_RXROUTE_PORT_SHIFT) |
                                               ((uint32_t)initData->pinRx << _GPIO_USART_RXROUTE_PIN_SHIFT);

        GPIO->USARTROUTE[spiPortNum].CLKROUTE = ((uint32_t)initData->portClk << _GPIO_USART_CLKROUTE_PORT_SHIFT) |
                                                ((uint32_t)initData->pinClk << _GPIO_USART_CLKROUTE_PIN_SHIFT);
#else
        handle->peripheral.usartPort->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN |
                                              (initData->portLocation << _USART_ROUTE_LOCATION_SHIFT);
#endif
    }

    if ((retVal = ConfigGPIO(handle, true)) != ECODE_EMDRV_SPIDRV_OK)
    {
        return retVal;
    }

    CORE_ENTER_ATOMIC();
    if (!spidrvIsInitialized)
    {
        spidrvIsInitialized = true;
        CORE_EXIT_ATOMIC();
    }
    else
    {
        CORE_EXIT_ATOMIC();
    }

    // Initialize DMA.
    DMADRV_Init();

    if (DMADRV_AllocateChannel(&handle->txDMACh, NULL) != ECODE_EMDRV_DMADRV_OK)
    {
        return ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR;
    }

    if (DMADRV_AllocateChannel(&handle->rxDMACh, NULL) != ECODE_EMDRV_DMADRV_OK)
    {
        return ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR;
    }

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Deinitialize an SPI driver instance.
//
//  This function should only be called with an initialized spidrv instance handle.
//
// @param[in] handle Pointer to an SPI driver handle.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
//    SPIDRV @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_DeInit(NF_SpiDriver_Handle_t handle)
{
    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    // Stop DMAs.
    DMADRV_StopTransfer(handle->rxDMACh);
    DMADRV_StopTransfer(handle->txDMACh);

    ConfigGPIO(handle, false);

    if (0)
    {
    }
    else if (handle->peripheralType == spidrvPeripheralTypeUsart)
    {
        USART_Reset(handle->peripheral.usartPort);
    }

    CMU_ClockEnable(handle->usartClock, false);

    DMADRV_FreeChannel(handle->txDMACh);
    DMADRV_FreeChannel(handle->rxDMACh);
    DMADRV_DeInit();
    emRequestDeinit(handle);

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Abort an ongoing SPI transfer.
// @param[in] handle Pointer to an SPI driver handle.
//
// @return
// @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_IDLE if
//    SPI is idle. On failure, an appropriate SPIDRV @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_AbortTransfer(NF_SpiDriver_Handle_t handle)
{
    CORE_DECLARE_IRQ_STATE;

    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    CORE_ENTER_ATOMIC();
    if (handle->state == spidrvStateIdle)
    {
        CORE_EXIT_ATOMIC();
        return ECODE_EMDRV_SPIDRV_IDLE;
    }

    // Stop DMA's.
    DMADRV_StopTransfer(handle->rxDMACh);
    DMADRV_StopTransfer(handle->txDMACh);
    DMADRV_TransferRemainingCount(handle->rxDMACh, &handle->remaining);
    handle->transferStatus = ECODE_EMDRV_SPIDRV_ABORTED;
    handle->state = spidrvStateIdle;
    handle->transferStatus = ECODE_EMDRV_SPIDRV_ABORTED;
    handle->blockingCompleted = true;
    // signal RTOS event
    tx_event_flags_set(&nanoHardwareEvents, NANO_HW_EVENTS_SPI_TRANSACTION_FLAG, TX_OR);

    em1RequestRemove(handle);

    if (handle->userCallback != NULL)
    {
        handle->userCallback(handle, ECODE_EMDRV_SPIDRV_ABORTED, handle->transferCount - handle->remaining);
    }
    CORE_EXIT_ATOMIC();

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Get current SPI bus bitrate.
//
// @param[in] handle Pointer to an SPI driver handle.
//
// @param[out] bitRate Current SPI bus bitrate.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
//    @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_GetBitrate(NF_SpiDriver_Handle_t handle, uint32_t *bitRate)
{
    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    if (bitRate == NULL)
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    if (0)
    {
    }
    else if (handle->peripheralType == spidrvPeripheralTypeUsart)
    {
        *bitRate = USART_BaudrateGet(handle->peripheral.usartPort);
    }

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Get current SPI framelength.
//
// @param[in] handle Pointer to an SPI driver handle.
//
// @param[out] frameLength Current SPI bus framelength.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
//    @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_GetFramelength(NF_SpiDriver_Handle_t handle, uint32_t *frameLength)
{
    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    if (frameLength == NULL)
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    *frameLength = handle->initData->frameLength;

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Get the status of an SPI transfer.
//
// @details
//    Returns status of an ongoing transfer. If no transfer is in progress,
//    the status of the last transfer is reported.
//
// @param[in] handle Pointer to an SPI driver handle.
//
// @param[out] itemsTransferred Number of items (frames) transferred.
//
// @param[out] itemsRemaining Number of items (frames) remaining.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
//    @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_GetTransferStatus(NF_SpiDriver_Handle_t handle, int *itemsTransferred, int *itemsRemaining)
{
    int remaining;

    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    if ((itemsTransferred == NULL) || (itemsRemaining == NULL))
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    CORE_ATOMIC_SECTION(
        if (handle->state == spidrvStateIdle) { remaining = handle->remaining; } else {
            DMADRV_TransferRemainingCount(handle->rxDMACh, &remaining);
        })

    // itemsTransferred = (handle->transferCount - remaining);
    *itemsRemaining = remaining;

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Start an SPI master receive transfer.
//
// @note
//    The MOSI wire will transmit @ref SPIDRV_Init.dummyTxValue.
//
// @param[in]  handle Pointer to an SPI driver handle.
//
// @param[out] buffer Receive data buffer.
//
// @param[in]  count Number of bytes in transfer.
//
// @param[in]  callback Transfer completion callback.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
//    @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_Receive(NF_SpiDriver_Handle_t handle, void *buffer, int count, NF_SpiDriver_Callback_t callback)
{
    Ecode_t retVal;

    if ((retVal = TransferApiPrologue(handle, buffer, count)) != ECODE_EMDRV_SPIDRV_OK)
    {
        return retVal;
    }

    StartReceiveDMA(handle, buffer, count, callback);

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Start an SPI master blocking receive transfer.
//
// @note
//    The MOSI wire will transmit @ref SPIDRV_Init.dummyTxValue.
//    @n This function is blocking and returns when the transfer is complete
//    or when @ref SPIDRV_AbortTransfer() is called.
//
// @param[in]  handle Pointer to an SPI driver handle.
//
// @param[out] buffer Receive data buffer.
//
// @param[in]  count Number of bytes in transfer.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
//    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
//    SPIDRV @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_ReceiveBlocking(NF_SpiDriver_Handle_t handle, void *buffer, int count)
{
    Ecode_t retVal;

    if ((retVal = TransferApiBlockingPrologue(handle, buffer, count)) != ECODE_EMDRV_SPIDRV_OK)
    {
        return retVal;
    }

    StartReceiveDMA(handle, buffer, count, BlockingComplete);

    WaitForTransferCompletion(handle);

    return handle->transferStatus;
}

//
//
//    Start an SPI master transfer.
//
// @param[in]  handle Pointer to an SPI driver handle.
//
// @param[in]  txBuffer Transmit data buffer.
//
// @param[out] rxBuffer Receive data buffer.
//
// @param[in]  count Number of bytes in transfer.
//
// @param[in]  callback Transfer completion callback.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
//    @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_Transfer(
    NF_SpiDriver_Handle_t handle,
    const void *txBuffer,
    void *rxBuffer,
    int count,
    NF_SpiDriver_Callback_t callback)
{
    Ecode_t retVal;

    if ((retVal = TransferApiPrologue(handle, (void *)txBuffer, count)) != ECODE_EMDRV_SPIDRV_OK)
    {
        return retVal;
    }

    if (rxBuffer == NULL)
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    StartTransferDMA(handle, txBuffer, rxBuffer, count, callback);

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Start an SPI master blocking transfer.
//
// @note
//    This function is blocking and returns when the transfer is complete
//    or when @ref SPIDRV_AbortTransfer() is called.
//
// @param[in]  handle Pointer to an SPI driver handle.
//
// @param[in]  txBuffer Transmit data buffer.
//
// @param[out] rxBuffer Receive data buffer.
//
// @param[in]  count Number of bytes in transfer.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
//    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
//    SPIDRV @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_TransferBlocking(NF_SpiDriver_Handle_t handle, const void *txBuffer, void *rxBuffer, int count)
{
    Ecode_t retVal;

    if ((retVal = TransferApiBlockingPrologue(handle, (void *)txBuffer, count)) != ECODE_EMDRV_SPIDRV_OK)
    {
        return retVal;
    }

    if (rxBuffer == NULL)
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    StartTransferDMA(handle, txBuffer, rxBuffer, count, BlockingComplete);

    WaitForTransferCompletion(handle);

    return handle->transferStatus;
}

//
//
//    Start an SPI master transmit transfer.
//
// @note
//    The data received on the MISO wire is discarded.
//
// @param[in] handle Pointer to an SPI driver handle.
//
// @param[in] buffer Transmit data buffer.
//
// @param[in] count Number of bytes in transfer.
//
// @param[in] callback Transfer completion callback.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
//    @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_Transmit(
    NF_SpiDriver_Handle_t handle,
    const void *buffer,
    int count,
    NF_SpiDriver_Callback_t callback)
{
    Ecode_t retVal;

    if ((retVal = TransferApiPrologue(handle, (void *)buffer, count)) != ECODE_EMDRV_SPIDRV_OK)
    {
        return retVal;
    }

    StartTransmitDMA(handle, buffer, count, callback);

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Start an SPI master blocking transmit transfer.
//
// @note
//    The data received on the MISO wire is discarded.
//    @n This function is blocking and returns when the transfer is complete.
//
// @param[in] handle Pointer to an SPI driver handle.
//
// @param[in] buffer Transmit data buffer.
//
// @param[in] count Number of bytes in transfer.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
//    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
//    SPIDRV @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_TransmitBlocking(NF_SpiDriver_Handle_t handle, const void *buffer, int count)
{
    Ecode_t retVal;

    if ((retVal = TransferApiBlockingPrologue(handle, (void *)buffer, count)) != ECODE_EMDRV_SPIDRV_OK)
    {
        return retVal;
    }

    StartTransmitDMA(handle, buffer, count, BlockingComplete);

    WaitForTransferCompletion(handle);

    return handle->transferStatus;
}

//
//
//    Set SPI bus bitrate.
//
// @param[in] handle Pointer to an SPI driver handle.
//
// @param[in] bitRate New SPI bus bitrate.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
//    @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_SetBitrate(NF_SpiDriver_Handle_t handle, uint32_t bitRate)
{
    CORE_DECLARE_IRQ_STATE;

    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    CORE_ENTER_ATOMIC();
    if (handle->state != spidrvStateIdle)
    {
        CORE_EXIT_ATOMIC();
        return ECODE_EMDRV_SPIDRV_BUSY;
    }

    handle->initData->bitRate = bitRate;

    if (0)
    {
    }
    else if (handle->peripheralType == spidrvPeripheralTypeUsart)
    {
        USART_BaudrateSyncSet(handle->peripheral.usartPort, 0, bitRate);
    }
    CORE_EXIT_ATOMIC();

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Set SPI framelength.
//
// @param[in] handle Pointer to an SPI driver handle.
//
// @param[in] frameLength New SPI bus framelength.
//
// @return
//    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
//    @ref Ecode_t is returned.
//
Ecode_t NF_SpiDriver_SetFramelength(NF_SpiDriver_Handle_t handle, uint32_t frameLength)
{
    CORE_DECLARE_IRQ_STATE;

    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    if (0)
    {
    }
    else if (handle->peripheralType == spidrvPeripheralTypeUsart)
    {
        frameLength -= EMDRV_SPIDRV_USART_FRAMELENGTH_REGVALUE_OFFSET;

        if ((frameLength < _USART_FRAME_DATABITS_FOUR) || (frameLength > _USART_FRAME_DATABITS_SIXTEEN))
        {
            return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
        }
    }

    CORE_ENTER_ATOMIC();
    if (handle->state != spidrvStateIdle)
    {
        CORE_EXIT_ATOMIC();
        return ECODE_EMDRV_SPIDRV_BUSY;
    }

    if (0)
    {
    }
    else if (handle->peripheralType == spidrvPeripheralTypeUsart)
    {
        handle->initData->frameLength = frameLength + EMDRV_SPIDRV_USART_FRAMELENGTH_REGVALUE_OFFSET;

        handle->peripheral.usartPort->FRAME = (handle->peripheral.usartPort->FRAME & ~_USART_FRAME_DATABITS_MASK) |
                                              (frameLength << _USART_FRAME_DATABITS_SHIFT);
    }
    CORE_EXIT_ATOMIC();

    return ECODE_EMDRV_SPIDRV_OK;
}

//
//
//    Transfer complete callback function used by blocking transfer API
//    functions. Called by DMA interrupt handler, timer timeout handler
//    or @ref SPIDRV_AbortTransfer() function.
//
static void BlockingComplete(NF_SpiDriver_Handle_t handle, Ecode_t transferStatus, int itemsTransferred)
{
    (void)itemsTransferred;

    handle->transferStatus = transferStatus;
    handle->blockingCompleted = true;
    // signal RTOS event
    tx_event_flags_set(&nanoHardwareEvents, NANO_HW_EVENTS_SPI_TRANSACTION_FLAG, TX_OR);
}

#if defined(_SILICON_LABS_32B_SERIES_1)
//
// Get SPI pins for Series 1 devices.
//
static Ecode_t GetSpiPins(NF_SpiDriver_Handle_t handle, SPI_Pins_t *pins)
{
    if (0)
    {
#if defined(USART0)
    }
    else if (handle->peripheral.usartPort == USART0)
    {
        pins->mosiPort = AF_USART0_TX_PORT(handle->initData->portLocationTx);
        pins->misoPort = AF_USART0_RX_PORT(handle->initData->portLocationRx);
        pins->clkPort = AF_USART0_CLK_PORT(handle->initData->portLocationClk);
        pins->csPort = AF_USART0_CS_PORT(handle->initData->portLocationCs);
        pins->mosiPin = AF_USART0_TX_PIN(handle->initData->portLocationTx);
        pins->misoPin = AF_USART0_RX_PIN(handle->initData->portLocationRx);
        pins->clkPin = AF_USART0_CLK_PIN(handle->initData->portLocationClk);
        pins->csPin = AF_USART0_CS_PIN(handle->initData->portLocationCs);
#endif
#if defined(USART1)
    }
    else if (handle->peripheral.usartPort == USART1)
    {
        pins->mosiPort = AF_USART1_TX_PORT(handle->initData->portLocationTx);
        pins->misoPort = AF_USART1_RX_PORT(handle->initData->portLocationRx);
        pins->clkPort = AF_USART1_CLK_PORT(handle->initData->portLocationClk);
        pins->csPort = AF_USART1_CS_PORT(handle->initData->portLocationCs);
        pins->mosiPin = AF_USART1_TX_PIN(handle->initData->portLocationTx);
        pins->misoPin = AF_USART1_RX_PIN(handle->initData->portLocationRx);
        pins->clkPin = AF_USART1_CLK_PIN(handle->initData->portLocationClk);
        pins->csPin = AF_USART1_CS_PIN(handle->initData->portLocationCs);
#endif
#if defined(USART2)
    }
    else if (handle->peripheral.usartPort == USART2)
    {
        pins->mosiPort = AF_USART2_TX_PORT(handle->initData->portLocationTx);
        pins->misoPort = AF_USART2_RX_PORT(handle->initData->portLocationRx);
        pins->clkPort = AF_USART2_CLK_PORT(handle->initData->portLocationClk);
        pins->csPort = AF_USART2_CS_PORT(handle->initData->portLocationCs);
        pins->mosiPin = AF_USART2_TX_PIN(handle->initData->portLocationTx);
        pins->misoPin = AF_USART2_RX_PIN(handle->initData->portLocationRx);
        pins->clkPin = AF_USART2_CLK_PIN(handle->initData->portLocationClk);
        pins->csPin = AF_USART2_CS_PIN(handle->initData->portLocationCs);
#endif
#if defined(USART3)
    }
    else if (handle->peripheral.usartPort == USART3)
    {
        pins->mosiPort = AF_USART3_TX_PORT(handle->initData->portLocationTx);
        pins->misoPort = AF_USART3_RX_PORT(handle->initData->portLocationRx);
        pins->clkPort = AF_USART3_CLK_PORT(handle->initData->portLocationClk);
        pins->csPort = AF_USART3_CS_PORT(handle->initData->portLocationCs);
        pins->mosiPin = AF_USART3_TX_PIN(handle->initData->portLocationTx);
        pins->misoPin = AF_USART3_RX_PIN(handle->initData->portLocationRx);
        pins->clkPin = AF_USART3_CLK_PIN(handle->initData->portLocationClk);
        pins->csPin = AF_USART3_CS_PIN(handle->initData->portLocationCs);
#endif
#if defined(USART4)
    }
    else if (handle->peripheral.usartPort == USART4)
    {
        pins->mosiPort = AF_USART4_TX_PORT(handle->initData->portLocationTx);
        pins->misoPort = AF_USART4_RX_PORT(handle->initData->portLocationRx);
        pins->clkPort = AF_USART4_CLK_PORT(handle->initData->portLocationClk);
        pins->csPort = AF_USART4_CS_PORT(handle->initData->portLocationCs);
        pins->mosiPin = AF_USART4_TX_PIN(handle->initData->portLocationTx);
        pins->misoPin = AF_USART4_RX_PIN(handle->initData->portLocationRx);
        pins->clkPin = AF_USART4_CLK_PIN(handle->initData->portLocationClk);
        pins->csPin = AF_USART4_CS_PIN(handle->initData->portLocationCs);
#endif
#if defined(USART5)
    }
    else if (handle->peripheral.usartPort == USART5)
    {
        pins->mosiPort = AF_USART5_TX_PORT(handle->initData->portLocationTx);
        pins->misoPort = AF_USART5_RX_PORT(handle->initData->portLocationRx);
        pins->clkPort = AF_USART5_CLK_PORT(handle->initData->portLocationClk);
        pins->csPort = AF_USART5_CS_PORT(handle->initData->portLocationCs);
        pins->mosiPin = AF_USART5_TX_PIN(handle->initData->portLocationTx);
        pins->misoPin = AF_USART5_RX_PIN(handle->initData->portLocationRx);
        pins->clkPin = AF_USART5_CLK_PIN(handle->initData->portLocationClk);
        pins->csPin = AF_USART5_CS_PIN(handle->initData->portLocationCs);
#endif
    }
    else
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }
    return ECODE_EMDRV_SPIDRV_OK;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
//
// Get SPI pins for Series 2 devices.
//
static Ecode_t GetSpiPins(NF_SpiDriver_Handle_t handle, SPI_Pins_t// pins)
{
    pins->mosiPort = handle->initData->portTx;
    pins->misoPort = handle->initData->portRx;
    pins->clkPort = handle->initData->portClk;
    pins->csPort = handle->initData->portCs;
    pins->mosiPin = handle->initData->pinTx;
    pins->misoPin = handle->initData->pinRx;
    pins->clkPin = handle->initData->pinClk;
    pins->csPin = handle->initData->pinCs;

    return ECODE_EMDRV_SPIDRV_OK;
}
#endif

//
// Configure/deconfigure SPI GPIO pins.
//
static Ecode_t ConfigGPIO(NF_SpiDriver_Handle_t handle, bool enable)
{
    SPI_Pins_t pins;
    Ecode_t ret;

    ret = GetSpiPins(handle, &pins);
    if (ret != ECODE_EMDRV_SPIDRV_OK)
    {
        return ret;
    }
    handle->portCs = (GPIO_Port_TypeDef)pins.csPort;
    handle->pinCs = pins.csPin;

    if (enable)
    {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.mosiPort, pins.mosiPin, gpioModePushPull, 0);
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.misoPort, pins.misoPin, gpioModeInput, 0);

        if ((handle->initData->clockMode == spidrvClockMode0) || (handle->initData->clockMode == spidrvClockMode1))
        {
            GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin, gpioModePushPull, 0);
        }
        else
        {
            GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin, gpioModePushPull, 1);
        }

        if (handle->initData->csControl == spidrvCsControlAuto)
        {
            GPIO_PinModeSet((GPIO_Port_TypeDef)handle->portCs, handle->pinCs, gpioModePushPull, 1);
        }
    }
    else
    {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.mosiPort, pins.mosiPin, gpioModeInputPull, 0);
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.misoPort, pins.misoPin, gpioModeInputPull, 0);

        if ((handle->initData->clockMode == spidrvClockMode0) || (handle->initData->clockMode == spidrvClockMode1))
        {
            GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin, gpioModeInputPull, 0);
        }
        else
        {
            GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin, gpioModeInputPull, 1);
        }

        if (handle->initData->csControl == spidrvCsControlAuto)
        {
            GPIO_PinModeSet((GPIO_Port_TypeDef)handle->portCs, handle->pinCs, gpioModeDisabled, 0);
        }
    }

    return ECODE_EMDRV_SPIDRV_OK;
}

//
// DMA transfer completion callback. Called by the DMA interrupt handler.
//
static bool RxDMAComplete(unsigned int channel,
                          unsigned int sequenceNo,
                          void *userParam)
{
    CORE_DECLARE_IRQ_STATE;
    NF_SpiDriver_Handle_t handle;
    (void)channel;
    (void)sequenceNo;

    CORE_ENTER_ATOMIC();

    handle = (NF_SpiDriver_Handle_t)userParam;

    handle->transferStatus = ECODE_EMDRV_SPIDRV_OK;
    handle->state = spidrvStateIdle;
    handle->remaining = 0;

    if (handle->userCallback != NULL)
    {
        handle->userCallback(handle, ECODE_EMDRV_SPIDRV_OK, handle->transferCount);
    }

    CORE_EXIT_ATOMIC();
    em1RequestRemove(handle);

    return true;
}

//
// Start an SPI receive DMA.
//
static void StartReceiveDMA(NF_SpiDriver_Handle_t handle,
                            void *buffer,
                            int count,
                            NF_SpiDriver_Callback_t callback)
{
    void *rxPort, *txPort;
    DMADRV_DataSize_t size;

    handle->blockingCompleted = false;
    handle->transferCount = count;
    handle->userCallback = callback;

    if (0)
    {
    }
    else if (handle->peripheralType == spidrvPeripheralTypeUsart)
    {
        handle->peripheral.usartPort->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

        if (handle->initData->frameLength > 9)
        {
            rxPort = (void *)&(handle->peripheral.usartPort->RXDOUBLE);
            txPort = (void *)&(handle->peripheral.usartPort->TXDOUBLE);
        }
        else if (handle->initData->frameLength == 9)
        {
            rxPort = (void *)&(handle->peripheral.usartPort->RXDATAX);
            txPort = (void *)&(handle->peripheral.usartPort->TXDATAX);
        }
        else
        {
            rxPort = (void *)&(handle->peripheral.usartPort->RXDATA);
            txPort = (void *)&(handle->peripheral.usartPort->TXDATA);
        }
    }
    else
    {
        return;
    }

    if (handle->initData->frameLength > 8)
    {
        size = dmadrvDataSize2;
    }
    else
    {
        size = dmadrvDataSize1;
    }

    em1RequestAdd(handle);

    // Start receive DMA.
    DMADRV_PeripheralMemory(
        handle->rxDMACh,
        handle->rxDMASignal,
        (void *)buffer,
        rxPort,
        true,
        count,
        size,
        RxDMAComplete,
        handle);

    // Start transmit DMA.
    DMADRV_MemoryPeripheral(
        handle->txDMACh,
        handle->txDMASignal,
        txPort,
        (void *)&(handle->initData->dummyTxValue),
        false,
        count,
        size,
        NULL,
        NULL);
}

//
// Start an SPI transmit/receive DMA.
//
static void StartTransferDMA(NF_SpiDriver_Handle_t handle,
                             const void *txBuffer,
                             void *rxBuffer,
                             int count,
                             NF_SpiDriver_Callback_t callback)
{
    void *rxPort, *txPort;
    DMADRV_DataSize_t size;

    handle->blockingCompleted = false;
    handle->transferCount = count;
    handle->userCallback = callback;

    if (0)
    {
    }
    else if (handle->peripheralType == spidrvPeripheralTypeUsart)
    {
        handle->peripheral.usartPort->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

        if (handle->initData->frameLength > 9)
        {
            rxPort = (void *)&(handle->peripheral.usartPort->RXDOUBLE);
            txPort = (void *)&(handle->peripheral.usartPort->TXDOUBLE);
        }
        else if (handle->initData->frameLength == 9)
        {
            rxPort = (void *)&(handle->peripheral.usartPort->RXDATAX);
            txPort = (void *)&(handle->peripheral.usartPort->TXDATAX);
        }
        else
        {
            rxPort = (void *)&(handle->peripheral.usartPort->RXDATA);
            txPort = (void *)&(handle->peripheral.usartPort->TXDATA);
        }
    }
    else
    {
        return;
    }

    if (handle->initData->frameLength > 8)
    {
        size = dmadrvDataSize2;
    }
    else
    {
        size = dmadrvDataSize1;
    }

    em1RequestAdd(handle);

    // Start receive DMA.
    DMADRV_PeripheralMemory(
        handle->rxDMACh,
        handle->rxDMASignal,
        rxBuffer,
        rxPort,
        true,
        count,
        size,
        RxDMAComplete,
        handle);

    // Start transmit DMA.
    DMADRV_MemoryPeripheral(
        handle->txDMACh,
        handle->txDMASignal,
        txPort,
        (void *)txBuffer,
        true,
        count,
        size,
        NULL,
        NULL);
}

//
// Start an SPI transmit DMA.
//
static void StartTransmitDMA(NF_SpiDriver_Handle_t handle,
                             const void *buffer,
                             int count,
                             NF_SpiDriver_Callback_t callback)
{
    void *rxPort, *txPort;
    DMADRV_DataSize_t size;

    handle->blockingCompleted = false;
    handle->transferCount = count;
    handle->userCallback = callback;

    if (0)
    {
    }
    else if (handle->peripheralType == spidrvPeripheralTypeUsart)
    {
        handle->peripheral.usartPort->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

        if (handle->initData->frameLength > 9)
        {
            rxPort = (void *)&(handle->peripheral.usartPort->RXDOUBLE);
            txPort = (void *)&(handle->peripheral.usartPort->TXDOUBLE);
        }
        else if (handle->initData->frameLength == 9)
        {
            rxPort = (void *)&(handle->peripheral.usartPort->RXDATAX);
            txPort = (void *)&(handle->peripheral.usartPort->TXDATAX);
        }
        else
        {
            rxPort = (void *)&(handle->peripheral.usartPort->RXDATA);
            txPort = (void *)&(handle->peripheral.usartPort->TXDATA);
        }
    }
    else
    {
        return;
    }

    if (handle->initData->frameLength > 8)
    {
        size = dmadrvDataSize2;
    }
    else
    {
        size = dmadrvDataSize1;
    }

    em1RequestAdd(handle);

    // Receive DMA runs only to get precise numbers for SPIDRV_GetTransferStatus()
    // Start receive DMA.
    DMADRV_PeripheralMemory(
        handle->rxDMACh,
        handle->rxDMASignal,
        &(handle->dummyRx),
        rxPort,
        false,
        count,
        size,
        RxDMAComplete,
        handle);

    // Start transmit DMA.
    DMADRV_MemoryPeripheral(
        handle->txDMACh,
        handle->txDMASignal,
        txPort,
        (void *)buffer,
        true,
        count,
        size,
        NULL,
        NULL);
}

//
// Parameter checking function for blocking transfer API functions.
//
static Ecode_t TransferApiBlockingPrologue(NF_SpiDriver_Handle_t handle,
                                           void *buffer,
                                           int count)
{
    CORE_DECLARE_IRQ_STATE;

    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    if ((buffer == NULL) || (count == 0) || (count > DMADRV_MAX_XFER_COUNT))
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    CORE_ENTER_ATOMIC();
    if (handle->state != spidrvStateIdle)
    {
        CORE_EXIT_ATOMIC();
        return ECODE_EMDRV_SPIDRV_BUSY;
    }
    handle->state = spidrvStateTransferring;
    CORE_EXIT_ATOMIC();

    return ECODE_EMDRV_SPIDRV_OK;
}

//
// Parameter checking function for non-blocking transfer API functions.
//
static Ecode_t TransferApiPrologue(NF_SpiDriver_Handle_t handle,
                                   void *buffer,
                                   int count)
{
    CORE_DECLARE_IRQ_STATE;

    if (handle == NULL)
    {
        return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
    }

    if ((buffer == NULL) || (count == 0) || (count > DMADRV_MAX_XFER_COUNT))
    {
        return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }

    CORE_ENTER_ATOMIC();
    if (handle->state != spidrvStateIdle)
    {
        CORE_EXIT_ATOMIC();
        return ECODE_EMDRV_SPIDRV_BUSY;
    }
    handle->state = spidrvStateTransferring;
    CORE_EXIT_ATOMIC();

    return ECODE_EMDRV_SPIDRV_OK;
}

//
// @brief Wait for transfer completion.
//
static void WaitForTransferCompletion(NF_SpiDriver_Handle_t handle)
{
    uint32_t dummy;

    if (CORE_IrqIsBlocked(SPI_DMA_IRQ))
    {
        // Poll for completion by calling IRQ handler.
        while (handle->blockingCompleted == false)
        {
#if defined(DMA_PRESENT) && (DMA_COUNT == 1)
            DMA_IRQHandler();
#elif defined(LDMA_PRESENT) && (LDMA_COUNT == 1)
            LDMA_IRQHandler();
#else
#error "No valid SPIDRV DMA engine defined."
#endif
        }
    }
    else
    {
        // wait forever for the SPI event
        tx_event_flags_get(
            &nanoHardwareEvents,
            NANO_HW_EVENTS_SPI_TRANSACTION_FLAG,
            TX_OR_CLEAR,
            &dummy,
            TX_WAIT_FOREVER);
    }
}
