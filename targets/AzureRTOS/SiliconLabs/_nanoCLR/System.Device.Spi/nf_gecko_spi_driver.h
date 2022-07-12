//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_GECKO_SPI_DRIVER_H
#define NF_GECKO_SPI_DRIVER_H

#include <stddef.h>

#include <tx_api.h>

#include <em_device.h>
#include <em_core.h>
#include <em_usart.h>
#include <em_ldma.h>
#include <dmadrv.h>

// flags for hardware events
extern TX_EVENT_FLAGS_GROUP nanoHardwareEvents;

typedef void (*SpiDmaCallback)(struct Gecko_SpiDriver *driver, Ecode_t transferStatus, int itemsTransferred);

#ifdef __cplusplus
extern "C"
{
#endif

    struct Gecko_SpiDriver
    {
        USART_TypeDef *Usart;
        USART_InitSync_TypeDef *Configuration;

        bool DataIs16bits;

        unsigned int TxDmaChannel;
        unsigned int RxDmaChannel;
        DMADRV_PeripheralSignal_t TxDmaSignal;
        DMADRV_PeripheralSignal_t RxDmaSignal;
        bool BlockingOpCompleted;
        int32_t DmaTransferCount;
        int32_t DmaRemaningCount;
        int32_t DmaRequestCount;
        uint32_t DmaDummyRxTxValue;
        SpiDmaCallback DmaCallback;
    };

    void SpiReceive(struct Gecko_SpiDriver *driver, void *buffer, int count, SpiDmaCallback callback);
    void SpiTransmit(struct Gecko_SpiDriver *driver, const void *buffer, int count, SpiDmaCallback callback);
    void SpiTransfer(
        struct Gecko_SpiDriver *driver,
        const void *txBuffer,
        void *rxBuffer,
        int count,
        SpiDmaCallback callback);
    void SpiTransmitBlocking(struct Gecko_SpiDriver *driver, const void *buffer, int count);
    void SpiReceiveBlocking(struct Gecko_SpiDriver *driver, void *buffer, int count);
    void SpiTransferBlocking(struct Gecko_SpiDriver *driver, const void *txBuffer, void *rxBuffer, int count);
    void StartTransferDma(
        struct Gecko_SpiDriver *driver,
        const void *txBuffer,
        void *rxBuffer,
        int count,
        SpiDmaCallback callback);
    void StartTransmitDma(struct Gecko_SpiDriver *driver, const void *buffer, int count, SpiDmaCallback callback);
    void StartReceiveDma(struct Gecko_SpiDriver *driver, void *buffer, int count, SpiDmaCallback callback);
    void WaitForTransferCompletion(struct Gecko_SpiDriver *driver);
    void DmaBlockingComplete(struct Gecko_SpiDriver *driver, Ecode_t transferStatus, int itemsTransferred);
    bool RxDmaComplete(unsigned int channel, unsigned int sequenceNo, void *userParam);
    void DmaRequestRemove(struct Gecko_SpiDriver *driver);
    void DmaRequestAdd(struct Gecko_SpiDriver *driver);
    void SpiDriverInit(struct Gecko_SpiDriver *driver);
    bool SpiStart(struct Gecko_SpiDriver *driver);
    void SpiRelease(struct Gecko_SpiDriver *driver);

#ifdef __cplusplus
}
#endif

#endif // NF_GECKO_SPI_DRIVER_H
