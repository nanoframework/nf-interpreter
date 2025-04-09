//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_GECKO_SPI_DRIVER_H
#define NF_GECKO_SPI_DRIVER_H

#include <stddef.h>

#include <tx_api.h>

#include <em_device.h>
#include <em_cmu.h>
#include <em_usart.h>

#include <ecode.h>
#include <sl_sleeptimer.h>
#include <sl_slist.h>
#include <dmadrv.h>
#include <sl_enum.h>

#include <platformHAL.h>

#if defined(DMA_PRESENT) && (DMA_COUNT == 1)
#define SPI_DMA_IRQ DMA_IRQn

#elif defined(LDMA_PRESENT) && (LDMA_COUNT == 1)
#define SPI_DMA_IRQ LDMA_IRQn

#else
#error "No valid SPIDRV DMA engine defined."
#endif

// flags for hardware events
extern TX_EVENT_FLAGS_GROUP nanoHardwareEvents;

#ifdef __cplusplus
extern "C"
{
#endif

    // clang-format off

#define ECODE_EMDRV_SPIDRV_OK                (ECODE_OK)                               ///< A successful return value.
#define ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE    (ECODE_EMDRV_SPIDRV_BASE | 0x00000001)   ///< An illegal SPI handle.
#define ECODE_EMDRV_SPIDRV_PARAM_ERROR       (ECODE_EMDRV_SPIDRV_BASE | 0x00000002)   ///< An illegal input parameter.
#define ECODE_EMDRV_SPIDRV_BUSY              (ECODE_EMDRV_SPIDRV_BASE | 0x00000003)   ///< The SPI port is busy.
#define ECODE_EMDRV_SPIDRV_TIMER_ALLOC_ERROR (ECODE_EMDRV_SPIDRV_BASE | 0x00000004)   ///< Unable to allocate timeout timer.
#define ECODE_EMDRV_SPIDRV_TIMEOUT           (ECODE_EMDRV_SPIDRV_BASE | 0x00000005)   ///< An SPI transfer timeout.
#define ECODE_EMDRV_SPIDRV_IDLE              (ECODE_EMDRV_SPIDRV_BASE | 0x00000006)   ///< No SPI transfer in progress.
#define ECODE_EMDRV_SPIDRV_ABORTED           (ECODE_EMDRV_SPIDRV_BASE | 0x00000007)   ///< An SPI transfer has been aborted.
#define ECODE_EMDRV_SPIDRV_MODE_ERROR        (ECODE_EMDRV_SPIDRV_BASE | 0x00000008)   ///< SPI master used slave API or vice versa.
#define ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR   (ECODE_EMDRV_SPIDRV_BASE | 0x00000009)   ///< Unable to allocate DMA channels.

    // clang-format on

    /// SPI driver instance type.
    SL_ENUM(NF_SpiDriver_Type_t){
        spidrvMaster = 0, ///< Act as an SPI master.
        spidrvSlave = 1   ///< Act as an SPI slave.
    };

    /// SPI bus bit order.
    SL_ENUM(NF_SpiDriver_BitOrder_t){
        spidrvBitOrderLsbFirst = 0, ///< LSB bit is transmitted first.
        spidrvBitOrderMsbFirst = 1  ///< MSB bit is transmitted first.
    };

    /// SPI clock mode (clock polarity and phase).
    SL_ENUM(NF_SpiDriver_ClockMode_t){
        spidrvClockMode0 = 0, ///< SPI mode 0: CLKPOL=0, CLKPHA=0.
        spidrvClockMode1 = 1, ///< SPI mode 1: CLKPOL=0, CLKPHA=1.
        spidrvClockMode2 = 2, ///< SPI mode 2: CLKPOL=1, CLKPHA=0.
        spidrvClockMode3 = 3  ///< SPI mode 3: CLKPOL=1, CLKPHA=1.
    };

    /// SPI master chip select (CS) control scheme.
    SL_ENUM(NF_SpiDriver_CsControl_t){
        spidrvCsControlAuto = 0,       ///< CS controlled by the SPI driver.
        spidrvCsControlApplication = 1 ///< CS controlled by the application.
    };

    /// SPI slave transfer start scheme.
    SL_ENUM(NF_SpiDriver_SlaveStart_t){
        spidrvSlaveStartImmediate = 0, ///< Transfer starts immediately.
        spidrvSlaveStartDelayed = 1    ///< Transfer starts when the bus is idle (CS deasserted).
    };

    /// Type of a USART peripheral
    SL_ENUM(NF_SpiDriver_PeripheralType_t){
        spidrvPeripheralTypeUsart = 0, ///< USART peripheral
    };

    SL_ENUM(NF_SpiDriver_State_t){spidrvStateIdle = 0, spidrvStateTransferring = 1};

    struct NF_SpiDriver_HandleData;

    /*
     * @brief
     *  NF_SpiDriver transfer completion callback function.
     *
     * @details
     *  Called when a transfer is complete. An
     *  application should check the transferStatus and itemsTransferred values.
     *
     * @param[in] handle
     *   The NF_SpiDriver device handle used to start the transfer.
     *
     * @param[in] transferStatus
     *   A number of bytes actually transferred.
     *
     * @param[in] itemsTransferred
     *   A number of bytes transferred.
     *
     * @return
     *   @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_TIMEOUT
     *   on timeout. Timeouts are only relevant for slave mode transfers.
     */
    typedef void (
        *NF_SpiDriver_Callback_t)(struct NF_SpiDriver_HandleData *handle, Ecode_t transferStatus, int itemsTransferred);

    /// An SPI driver instance initialization structure.
    /// Contains a number of SPIDRV configuration options.
    /// This structure is passed to @ref NF_SpiDriver_Init() when initializing a SPIDRV
    /// instance. Some common initialization data sets are predefined in
    /// @ref spidrv_init_structs
    typedef struct NF_SpiDriver_Init
    {
        void *port;                         ///< The USART used for SPI.
        uint8_t portLocationTx;             ///< A location number for the SPI Tx pin.
        uint8_t portLocationRx;             ///< A location number for the SPI Rx pin.
        uint8_t portLocationClk;            ///< A location number for the SPI Clk pin.
        uint8_t portLocationCs;             ///< A location number for the SPI Cs pin.
        uint32_t bitRate;                   ///< An SPI bitrate.
        uint32_t frameLength;               ///< An SPI framelength, valid numbers are 4..16
        uint32_t dummyTxValue;              ///< The value to transmit when using SPI receive API functions.
        NF_SpiDriver_Type_t type;           ///< An SPI type, master or slave.
        NF_SpiDriver_BitOrder_t bitOrder;   ///< A bit order on the SPI bus, MSB or LSB first.
        NF_SpiDriver_ClockMode_t clockMode; ///< SPI mode, CLKPOL/CLKPHASE setting.
        NF_SpiDriver_CsControl_t csControl; ///< A select master mode chip select (CS) control scheme.
        bool isHalfDuplex;                  ///< True if the SPI is half duplex.
    } NF_SpiDriver_Init_t;

    /// An SPI driver instance handle data structure.
    /// The handle is allocated by the application using the SPIDRV.
    /// Several concurrent driver instances can exist in an application. The application is
    /// neither supposed to write or read the contents of the handle.
    typedef struct NF_SpiDriver_HandleData
    {
        union {
            USART_TypeDef *usartPort;
            void *__reserved_space;
        } peripheral;
        NF_SpiDriver_Init_t *initData;
        unsigned int txDMACh;
        unsigned int rxDMACh;
        DMADRV_PeripheralSignal_t txDMASignal;
        DMADRV_PeripheralSignal_t rxDMASignal;
        NF_SpiDriver_Callback_t userCallback;
        uint32_t dummyRx;
        int transferCount;
        int remaining;
        GPIO_Port_TypeDef portCs;
        uint8_t pinCs;
        Ecode_t transferStatus;
        volatile NF_SpiDriver_State_t state;
        CMU_Clock_TypeDef usartClock;
        volatile bool blockingCompleted;
        int em1RequestCount;
        NF_SpiDriver_PeripheralType_t peripheralType;
        sl_sleeptimer_timer_handle_t timer;
        sl_slist_node_t node;
    } NF_SpiDriver_HandleData_t;

    /// An SPI driver instance handle.
    typedef NF_SpiDriver_HandleData_t *NF_SpiDriver_Handle_t;

    Ecode_t NF_SpiDriver_AbortTransfer(NF_SpiDriver_Handle_t handle);

    Ecode_t NF_SpiDriver_DeInit(NF_SpiDriver_Handle_t handle);

    Ecode_t NF_SpiDriver_GetBitrate(NF_SpiDriver_Handle_t handle, uint32_t *bitRate);

    Ecode_t NF_SpiDriver_GetFramelength(NF_SpiDriver_Handle_t handle, uint32_t *frameLength);

    Ecode_t NF_SpiDriver_GetTransferStatus(NF_SpiDriver_Handle_t handle, int *itemsTransferred, int *itemsRemaining);

    Ecode_t NF_SpiDriver_Init(NF_SpiDriver_Handle_t handle, NF_SpiDriver_Init_t *initData);

    Ecode_t NF_SpiDriver_MReceive(
        NF_SpiDriver_Handle_t handle,
        void *buffer,
        int count,
        NF_SpiDriver_Callback_t callback);

    Ecode_t NF_SpiDriver_MReceiveB(NF_SpiDriver_Handle_t handle, void *buffer, int count);

    Ecode_t NF_SpiDriver_MTransfer(
        NF_SpiDriver_Handle_t handle,
        const void *txBuffer,
        void *rxBuffer,
        int count,
        NF_SpiDriver_Callback_t callback);

    Ecode_t NF_SpiDriver_MTransferB(NF_SpiDriver_Handle_t handle, const void *txBuffer, void *rxBuffer, int count);

    Ecode_t NF_SpiDriver_MTransmit(
        NF_SpiDriver_Handle_t handle,
        const void *buffer,
        int count,
        NF_SpiDriver_Callback_t callback);

    Ecode_t NF_SpiDriver_MTransmitB(NF_SpiDriver_Handle_t handle, const void *buffer, int count);

    Ecode_t NF_SpiDriver_SetBitrate(NF_SpiDriver_Handle_t handle, uint32_t bitRate);

    Ecode_t NF_SpiDriver_SetFramelength(NF_SpiDriver_Handle_t handle, uint32_t frameLength);

#ifdef __cplusplus
}
#endif

#endif // NF_GECKO_SPI_DRIVER_H
