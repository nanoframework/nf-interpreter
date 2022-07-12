//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_gecko_spi_driver.h"

#if defined(EUSART_PRESENT)
#error "Only USART type is supported. Drivar can't handle EUSART."
#endif

// Adds track for a new DMA request
void DmaRequestAdd(struct Gecko_SpiDriver *driver)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();

    if (driver->DmaRequestCount == 0)
    {
        sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
    }

    driver->DmaRequestCount++;

    CORE_EXIT_ATOMIC();
#else
    driver->DmaRequestCount++;
#endif
}

//  Substracts track of a DMA request
void DmaRequestRemove(struct Gecko_SpiDriver *driver)
{
    if (driver->DmaRequestCount == 0)
    {
        return;
    }

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    driver->DmaRequestCount--;
    if (driver->DmaRequestCount == 0)
    {
        sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
    }
    CORE_EXIT_ATOMIC();
#else
    driver->DmaRequestCount--;
#endif
}

// Callback for DMA transfer completed
bool RxDmaComplete(unsigned int channel, unsigned int sequenceNo, void *userParam)
{
    (void)channel;
    (void)sequenceNo;
    Gecko_SpiDriver *driver = (Gecko_SpiDriver *)userParam;

    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();

    driver->DmaRemaningCount = 0;

    driver->DmaCallback(driver, ECODE_OK, 0);

    CORE_EXIT_ATOMIC();

    DmaRequestRemove(driver);

    return true;
}

// Callback for DMA blocking transfers
void DmaBlockingComplete(struct Gecko_SpiDriver *driver, Ecode_t transferStatus, int itemsTransferred)
{
    (void)itemsTransferred;
    (void)transferStatus;

    driver->BlockingOpCompleted = true;
}

void WaitForTransferCompletion(struct Gecko_SpiDriver *driver)
{
    // TODO replace with RTOS semaphore

    // if (CORE_IrqIsBlocked(SPI_DMA_IRQ))
    {
        // Poll for completion by calling IRQ handler.
        while (driver->BlockingOpCompleted == false)
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
    // else
    {
        while (driver->BlockingOpCompleted == false)
            ;
    }
}

void StartReceiveDma(struct Gecko_SpiDriver *driver, void *buffer, int count, SpiDmaCallback callback)
{
    void *rxPort, *txPort;
    DMADRV_DataSize_t size;

    driver->BlockingOpCompleted = false;
    driver->DmaTransferCount = count;
    driver->DmaCallback = callback;

    driver->Usart->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

    if (driver->DataIs16bits)
    {
        rxPort = (void *)&(driver->Usart->RXDOUBLE);
        txPort = (void *)&(driver->Usart->TXDOUBLE);
    }
    else
    {
        rxPort = (void *)&(driver->Usart->RXDATA);
        txPort = (void *)&(driver->Usart->TXDATA);
    }

    if (driver->DataIs16bits)
    {
        size = dmadrvDataSize2;
    }
    else
    {
        size = dmadrvDataSize1;
    }

    DmaRequestAdd(driver);

    // Start receive DMA.
    DMADRV_PeripheralMemory(
        driver->RxDmaChannel,
        driver->RxDmaSignal,
        (void *)buffer,
        rxPort,
        true,
        count,
        size,
        RxDmaComplete,
        driver);

    // Start transmit DMA.
    DMADRV_MemoryPeripheral(
        driver->TxDmaChannel,
        driver->TxDmaSignal,
        txPort,
        (void *)&(driver->DmaDummyRxTxValue),
        false,
        count,
        size,
        NULL,
        NULL);
}

void StartTransmitDma(struct Gecko_SpiDriver *driver, const void *buffer, int count, SpiDmaCallback callback)
{
    void *rxPort, *txPort;
    DMADRV_DataSize_t size;

    driver->BlockingOpCompleted = false;
    driver->DmaTransferCount = count;
    driver->DmaCallback = callback;

    driver->Usart->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

    if (driver->DataIs16bits)
    {
        rxPort = (void *)&(driver->Usart->RXDOUBLE);
        txPort = (void *)&(driver->Usart->TXDOUBLE);
    }
    else
    {
        rxPort = (void *)&(driver->Usart->RXDATA);
        txPort = (void *)&(driver->Usart->TXDATA);
    }

    if (driver->DataIs16bits)
    {
        size = dmadrvDataSize2;
    }
    else
    {
        size = dmadrvDataSize1;
    }

    DmaRequestAdd(driver);

    // Receive DMA runs only to get precise numbers for SPIDRV_GetTransferStatus()
    // Start receive DMA.
    DMADRV_PeripheralMemory(
        driver->RxDmaChannel,
        driver->RxDmaSignal,
        &(driver->DmaDummyRxTxValue),
        rxPort,
        false,
        count,
        size,
        RxDmaComplete,
        driver);

    // Start transmit DMA.
    DMADRV_MemoryPeripheral(
        driver->TxDmaChannel,
        driver->TxDmaSignal,
        txPort,
        (void *)buffer,
        true,
        count,
        size,
        NULL,
        NULL);
}

void StartTransferDma(
    struct Gecko_SpiDriver *driver,
    const void *txBuffer,
    void *rxBuffer,
    int count,
    SpiDmaCallback callback)
{
    void *rxPort, *txPort;
    DMADRV_DataSize_t size;

    driver->BlockingOpCompleted = false;
    driver->DmaTransferCount = count;
    driver->DmaCallback = callback;

    driver->Usart->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

    if (driver->DataIs16bits)
    {
        rxPort = (void *)&(driver->Usart->RXDOUBLE);
        txPort = (void *)&(driver->Usart->TXDOUBLE);
    }
    else
    {
        rxPort = (void *)&(driver->Usart->RXDATA);
        txPort = (void *)&(driver->Usart->TXDATA);
    }

    if (driver->DataIs16bits)
    {
        size = dmadrvDataSize2;
    }
    else
    {
        size = dmadrvDataSize1;
    }

    DmaRequestAdd(driver);

    // Start receive DMA.
    DMADRV_PeripheralMemory(
        driver->RxDmaChannel,
        driver->RxDmaSignal,
        rxBuffer,
        rxPort,
        true,
        count,
        size,
        RxDmaComplete,
        driver);

    // Start transmit DMA.
    DMADRV_MemoryPeripheral(
        driver->TxDmaChannel,
        driver->TxDmaSignal,
        txPort,
        (void *)txBuffer,
        true,
        count,
        size,
        NULL,
        NULL);
}

// Start a blocking transfer
void SpiTransferBlocking(struct Gecko_SpiDriver *driver, const void *txBuffer, void *rxBuffer, int count)
{
    StartTransferDma(driver, txBuffer, rxBuffer, count, DmaBlockingComplete);

    WaitForTransferCompletion(driver);
}

// Start a blocking receive transfer
void SpiReceiveBlocking(struct Gecko_SpiDriver *driver, void *buffer, int count)
{
    StartReceiveDma(driver, buffer, count, DmaBlockingComplete);

    WaitForTransferCompletion(driver);
}

// Start a blocking transmit transfer
void SpiTransmitBlocking(struct Gecko_SpiDriver *driver, const void *buffer, int count)
{
    StartTransmitDma(driver, buffer, count, DmaBlockingComplete);

    WaitForTransferCompletion(driver);
}

// Start a non-blocking transfer
void SpiTransfer(
    struct Gecko_SpiDriver *driver,
    const void *txBuffer,
    void *rxBuffer,
    int count,
    SpiDmaCallback callback)
{
    StartTransferDma(driver, txBuffer, rxBuffer, count, callback);
}

// Start a non-blocking transmit transfer
void SpiTransmit(struct Gecko_SpiDriver *driver, const void *buffer, int count, SpiDmaCallback callback)
{
    StartTransmitDma(driver, buffer, count, callback);
}

// Start a non-blocking receive transfer
void SpiReceive(struct Gecko_SpiDriver *driver, void *buffer, int count, SpiDmaCallback callback)
{
    StartReceiveDma(driver, buffer, count, callback);
}

// Initialize DMA for SPI transfers
bool SpiStart(struct Gecko_SpiDriver *driver)
{
    // set frame size
    driver->Usart->FRAME =
        (driver->Usart->FRAME & ~_USART_FRAME_DATABITS_MASK) | driver->DataIs16bits ? usartDatabits16 : usartDatabits8;

    USART_Enable(driver->Usart, usartEnable);

    return true;
}

void SpiRelease(struct Gecko_SpiDriver *driver)
{
    USART_Enable(driver->Usart, usartDisable);
}

void SpiDriverInit(struct Gecko_SpiDriver *driver)
{
    USART_InitSync(driver->Usart, driver->Configuration);
}
