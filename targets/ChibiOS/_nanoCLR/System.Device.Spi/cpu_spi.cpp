//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include <LaunchCLR.h>
#include <string.h>
#include <targetPAL.h>

#include <sys_dev_spi_native_target.h>
#include <hal.h>

// SPI DMA errors are reported through an extension field of the SPI driver, which is set by the SPI DMA error hook
// (see SPI_DRIVER_EXT_FIELDS in the target mcuconf.h). Targets that don't define it keep their previous behaviour.
#if defined(SPI_DRIVER_EXT_FIELDS)

static inline bool DmaErrorOccurred(SPIDriver *spip)
{
    return spip->dmaError;
}

static inline void ClearDmaError(SPIDriver *spip)
{
    spip->dmaError = false;
}

#else

static inline bool DmaErrorOccurred(SPIDriver *spip)
{
    (void)spip;
    return false;
}

static inline void ClearDmaError(SPIDriver *spip)
{
    (void)spip;
}

#endif

#if defined(RP_SPI_USE_SPI0) || defined(RP_SPI_USE_SPI1)

#if defined(RP_SPI_USE_SPI0)
NF_PAL_SPI SPI0_PAL;
#endif
#if defined(RP_SPI_USE_SPI1)
NF_PAL_SPI SPI1_PAL;
#endif

#define RP_SPI_PERI_CLK 125000000

// Tidy up after completing tranfer
// CAUTION: has to be called from thread context, never from an ISR!
// spiReleaseBus() unlocks a mutex which is owned by the thread that acquired the bus, so calling it
// from an ISR would operate on whatever thread happens to be running at that moment.
static void CompleteTranfer(NF_PAL_SPI *palSpi)
{
    spiUnselect(palSpi->Driver);

    if (palSpi->TransferFailed)
    {
        // DMA error: stop the driver
        spiStop(palSpi->Driver);
        ClearDmaError(palSpi->Driver);
    }

    spiReleaseBus(palSpi->Driver);
}

// Waits for the async transfer in progress to complete.
// The completion callback always runs when the transfer ends, so there is no timeout here: the bus must never be
// released while a transfer is still using it, no matter how long it takes (e.g. a large display update).
static void WaitAsyncTransfer(NF_PAL_SPI *palSpi)
{
    osalSysLock();

    if (!palSpi->AsyncTransferComplete)
    {
        // resumed by the completion callback
        (void)osalThreadSuspendS(&palSpi->AsyncWaiter);
    }

    osalSysUnlock();
}

// Completes the async transfer started by the calling thread, if any, and releases the bus.
// The bus is a mutex owned by the thread that acquired it, so only that thread can release it. Any other thread
// returns without doing anything and gets the bus through spiAcquireBus(), once the owner has released it.
static void CompleteAsyncTranfer(NF_PAL_SPI *palSpi)
{
    if (palSpi->AsyncOwner != chThdGetSelfX())
    {
        // no async transfer in progress, or it belongs to another thread
        return;
    }

    WaitAsyncTransfer(palSpi);

    palSpi->AsyncOwner = NULL;

    CompleteTranfer(palSpi);
}

// Ends the transfer in progress, from the SPI callback (ISR context)
static void FinishTransferFromIsr(NF_PAL_SPI *palSpi, bool failed)
{
    if (palSpi->ChipSelect >= 0)
    {
        CPU_GPIO_TogglePinState(palSpi->ChipSelect);
    }

    palSpi->TransferFailed = failed;

    if (palSpi->Callback)
    {
        palSpi->Callback(palSpi->BusIndex);
    }

    // flag the transfer as completed and resume the thread waiting for it
    osalSysLockFromISR();
    palSpi->AsyncTransferComplete = true;
    osalThreadResumeI(&palSpi->AsyncWaiter, MSG_OK);
    osalSysUnlockFromISR();
}

static void SpiCallback(SPIDriver *spip)
{
    (void)spip;

    NATIVE_INTERRUPT_START

    NF_PAL_SPI *palSpi = NULL;

#if defined(RP_SPI_USE_SPI0)
    if (spip == &SPID0)
    {
        palSpi = &SPI0_PAL;
    }
#endif
#if defined(RP_SPI_USE_SPI1)
    if (spip == &SPID1)
    {
        palSpi = &SPI1_PAL;
    }
#endif

    if (palSpi == NULL)
    {
        NATIVE_INTERRUPT_END
        return;
    }

    if (DmaErrorOccurred(spip))
    {
        // DMA error: the driver ends the transfer and has to be reported as failed and prevent
        // prevent the next phase of a sequential transfer from starting
        palSpi->SequentialTxRx = false;

        FinishTransferFromIsr(palSpi, true);
    }
    else if (palSpi->SequentialTxRx)
    {
        palSpi->SequentialTxRx = false;

        osalSysLockFromISR();
        spiStartReceiveI(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
        osalSysUnlockFromISR();
    }
    else
    {
        FinishTransferFromIsr(palSpi, false);
    }

    NATIVE_INTERRUPT_END
};

static void ComputeBaudRate(int32_t requestedFrequency, int32_t &actualFrequency, uint32_t &sspcpsr, uint32_t &sspcr0)
{
    if (requestedFrequency <= 0 || requestedFrequency > RP_SPI_PERI_CLK / 2)
    {
        requestedFrequency = RP_SPI_PERI_CLK / 2;
    }

    uint32_t bestCpsdvsr = 2;
    uint32_t bestScr = 0;
    int32_t bestFreq = RP_SPI_PERI_CLK / 2;

    for (uint32_t cpsdvsr = 2; cpsdvsr <= 254; cpsdvsr += 2)
    {
        uint32_t scr = (RP_SPI_PERI_CLK / (cpsdvsr * requestedFrequency)) - 1;
        if (scr > 255)
        {
            scr = 255;
        }

        int32_t freq = RP_SPI_PERI_CLK / (cpsdvsr * (1 + scr));

        if (freq <= requestedFrequency && freq > bestFreq)
        {
            bestFreq = freq;
            bestCpsdvsr = cpsdvsr;
            bestScr = scr;
        }
        else if (bestFreq > requestedFrequency && freq <= requestedFrequency)
        {
            bestFreq = freq;
            bestCpsdvsr = cpsdvsr;
            bestScr = scr;
        }

        if (freq <= requestedFrequency)
        {
            break;
        }
    }

    actualFrequency = bestFreq;
    sspcpsr = bestCpsdvsr;
    sspcr0 = (bestScr << 8);
}

NF_PAL_SPI *GetNfPalfromBusIndex(uint8_t busIndex)
{
    switch (busIndex)
    {
#if defined(RP_SPI_USE_SPI0)
        case 0:
            return &SPI0_PAL;
#endif
#if defined(RP_SPI_USE_SPI1)
        case 1:
            return &SPI1_PAL;
#endif
        default:
            return NULL;
    }
}

void GetSPIConfig(SPI_DEVICE_CONFIGURATION &config, SPI_WRITE_READ_SETTINGS &wrc, SPIConfig *llConfig)
{
    int32_t actualFrequency;
    uint32_t sspcpsr;
    uint32_t sspcr0;

    ComputeBaudRate(config.Clock_RateHz, actualFrequency, sspcpsr, sspcr0);

    if (wrc.Bits16ReadWrite)
    {
        sspcr0 |= (16 - 1);
    }
    else
    {
        sspcr0 |= (8 - 1);
    }

    switch (config.Spi_Mode)
    {
        case SpiMode_Mode1:
            sspcr0 |= (1 << 7);
            break;
        case SpiMode_Mode2:
            sspcr0 |= (1 << 6);
            break;
        case SpiMode_Mode3:
            sspcr0 |= (1 << 6) | (1 << 7);
            break;
        default:
            break;
    }

    llConfig->SSPCR0 = sspcr0;
    llConfig->SSPCPSR = sspcpsr;
    llConfig->end_cb = SpiCallback;
}

HRESULT CPU_SPI_nWrite_nRead(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &wrc,
    uint8_t *writeBuffer,
    int32_t writeSize,
    uint8_t *readBuffer,
    int32_t readSize)
{
    NANOCLR_HEADER();
    {
        NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;
        bool sync = (wrc.callback == 0);

        // the driver is cleared when the bus is uninitialized
        SPIDriver *driver = palSpi->Driver;

        if (driver == NULL)
        {
            // the bus has been uninitialized
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // complete a previous async transfer from this thread, which still holds the bus
        // (otherwise this thread would deadlock trying to acquire a bus that it already owns)
        CompleteAsyncTranfer(palSpi);

        // acquire the bus before touching the PAL struct: it's shared by all the devices on this bus and it's also
        // used by the completion callback of a transfer that can still be in progress
        spiAcquireBus(driver);

        if (palSpi->Driver != driver)
        {
            spiReleaseBus(driver);

            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // clear the error state of the previous transfer
        palSpi->TransferFailed = false;
        ClearDmaError(palSpi->Driver);

        palSpi->BufferIs16bits = wrc.Bits16ReadWrite;
        palSpi->Callback = wrc.callback;
        palSpi->WriteSize = 0;
        palSpi->WriteBuffer = NULL;
        palSpi->ReadSize = 0;
        palSpi->ReadBuffer = NULL;

        if (writeBuffer != NULL)
        {
            palSpi->WriteSize = writeSize;
            palSpi->WriteBuffer = writeBuffer;
        }

        if (readBuffer != NULL)
        {
            palSpi->ReadSize = readSize;
            palSpi->ReadBuffer = readBuffer;
        }

        palSpi->BusIndex = sdev.Spi_Bus;

        GetSPIConfig(sdev, wrc, &palSpi->Configuration);

        if (sync)
        {
            palSpi->Configuration.end_cb = NULL;
        }

        spiStart(palSpi->Driver, &palSpi->Configuration);
        spiSelect(palSpi->Driver);

        palSpi->ChipSelect = wrc.DeviceChipSelect;
        if (wrc.DeviceChipSelect >= 0)
        {
            CPU_GPIO_SetPinState(wrc.DeviceChipSelect, (GpioPinValue)wrc.ChipSelectActiveState);
        }

        if (sync)
        {
            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                if (wrc.fullDuplex)
                {
                    spiExchange(
                        palSpi->Driver,
                        palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                        palSpi->WriteBuffer,
                        palSpi->ReadBuffer);
                }
                else
                {
                    spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);

                    // receive, unless the send failed
                    if (!DmaErrorOccurred(palSpi->Driver))
                    {
                        spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                    }
                }
            }
            else
            {
                if (palSpi->ReadSize != 0)
                {
                    spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }

            // a DMA error fails the transfer
            bool failed = DmaErrorOccurred(palSpi->Driver);
            palSpi->TransferFailed = failed;

            CompleteTranfer(palSpi);

            if (wrc.DeviceChipSelect >= 0)
            {
                CPU_GPIO_SetPinState(wrc.DeviceChipSelect, (GpioPinValue)!wrc.ChipSelectActiveState);
            }

            if (failed)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_IO);
            }
        }
        else
        {
            // the bus stays held by this thread until the transfer completes (see CompleteAsyncTranfer)
            palSpi->AsyncTransferComplete = false;
            palSpi->AsyncOwner = chThdGetSelfX();

            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                if (wrc.fullDuplex)
                {
                    palSpi->SequentialTxRx = false;
                    spiStartExchange(
                        palSpi->Driver,
                        palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                        palSpi->WriteBuffer,
                        palSpi->ReadBuffer);
                }
                else
                {
                    palSpi->SequentialTxRx = true;
                    spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }
            else
            {
                palSpi->SequentialTxRx = false;

                if (palSpi->ReadSize != 0)
                {
                    spiStartReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }

            NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
        }
    }

    NANOCLR_NOCLEANUP();
}

SPI_OP_STATUS CPU_SPI_OP_Status(uint8_t busIndex, uint32_t deviceHandle)
{
    (void)busIndex;

    NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;

    if (palSpi->AsyncOwner != NULL)
    {
        // async transfer in progress, or completed and still holding the bus
        if (!palSpi->AsyncTransferComplete)
        {
            return SPI_OP_RUNNING;
        }

        // get the result before the bus is released
        bool failed = palSpi->TransferFailed;

        // completed: release the bus (only happens if the calling thread is the one that owns it)
        CompleteAsyncTranfer(palSpi);

        return failed ? SPI_OP_FAILED : SPI_OP_COMPLETE;
    }

    switch (palSpi->Driver->state)
    {
        default:
        case SPI_UNINIT:
        case SPI_STOP:
        case SPI_READY:
            return SPI_OP_READY;
        case SPI_ACTIVE:
            return SPI_OP_RUNNING;
        case SPI_COMPLETE:
            return SPI_OP_COMPLETE;
    }
}

// Waits for an async transfer started by the calling thread to complete and releases the bus
void CPU_SPI_Wait_Busy(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION &sdev)
{
    (void)sdev;

    NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;

    if (palSpi != NULL)
    {
        CompleteAsyncTranfer(palSpi);
    }
}

// Aborts the async transfer started by the calling thread, if any, and releases the bus.
// DMA is stopped before the bus is released, so the transfer buffers are no longer in use when this returns.
void CPU_SPI_Abort(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION &sdev)
{
    (void)sdev;

    NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;
    bool aborted = false;

    if (palSpi == NULL || palSpi->AsyncOwner != chThdGetSelfX())
    {
        // no async transfer in progress, or it belongs to another thread
        return;
    }

    osalSysLock();

    if (!palSpi->AsyncTransferComplete)
    {
        // stop both DMA channels
        dmaChannelDisableX(palSpi->Driver->dmatx);
        dmaChannelDisableX(palSpi->Driver->dmarx);

        // back to ready, as spiAbortI() would do
        palSpi->Driver->state = SPI_READY;

        // the transfer is over: report it as failed
        palSpi->SequentialTxRx = false;
        palSpi->TransferFailed = true;
        palSpi->AsyncTransferComplete = true;

        aborted = true;
    }

    osalSysUnlock();

    if (aborted && palSpi->ChipSelect >= 0)
    {
        // the completion callback won't run, so CS has to be de-asserted here
        CPU_GPIO_TogglePinState(palSpi->ChipSelect);
    }

    // release the bus (and reset the driver)
    CompleteAsyncTranfer(palSpi);
}

bool CPU_SPI_Initialize(uint8_t busIndex, const SPI_DEVICE_CONFIGURATION &spiDeviceConfig)
{
    switch (busIndex)
    {
#if defined(RP_SPI_USE_SPI0)
        case 0:
            if (SPI0_PAL.Driver == NULL)
            {
                ConfigPins_SPI0(spiDeviceConfig);
                SPI0_PAL.Driver = &SPID0;
                SPI0_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
#if defined(RP_SPI_USE_SPI1)
        case 1:
            if (SPI1_PAL.Driver == NULL)
            {
                ConfigPins_SPI1(spiDeviceConfig);
                SPI1_PAL.Driver = &SPID1;
                SPI1_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
        default:
            return false;
    }

    return true;
}

bool CPU_SPI_Uninitialize(uint8_t busIndex)
{
    NF_PAL_SPI *palSpi = GetNfPalfromBusIndex(busIndex);

    if (palSpi == NULL)
    {
        // the requested SPI bus is not valid
        return false;
    }

    if (palSpi->Driver == NULL)
    {
        // this bus is not initialized, nothing to do here
        return true;
    }

    SPIDriver *driver = palSpi->Driver;

    // complete an async transfer from this thread, which still holds the bus
    CompleteAsyncTranfer(palSpi);

    // acquire the bus before stopping it: it can be in use by a transfer from another thread or by another (native)
    // driver sharing it, and stopping it in the middle of a transaction would corrupt that transaction
    spiAcquireBus(driver);

    spiStop(driver);

    palSpi->Driver = NULL;

    spiReleaseBus(driver);

    return true;
}

uint32_t CPU_SPI_PortsMap()
{
    uint32_t map = 0;

#if defined(RP_SPI_USE_SPI0)
    map |= 0x01;
#endif
#if defined(RP_SPI_USE_SPI1)
    map |= 0x02;
#endif

    return map;
}

HRESULT CPU_SPI_Add_Device(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, uint32_t &handle)
{
    if (spiDeviceConfig.BusConfiguration == SpiBusConfiguration_Simplex)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    handle = (uint32_t)GetNfPalfromBusIndex(spiDeviceConfig.Spi_Bus);

    return S_OK;
}

void CPU_SPI_GetPins(uint32_t busIndex, GPIO_PIN &clk, GPIO_PIN &miso, GPIO_PIN &mosi)
{
    (void)busIndex;

    clk = (GPIO_PIN)-1;
    miso = (GPIO_PIN)-1;
    mosi = (GPIO_PIN)-1;
}

HRESULT CPU_SPI_MinClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    *frequency = RP_SPI_PERI_CLK / (254 * 256);

    return S_OK;
}

HRESULT CPU_SPI_MaxClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    *frequency = RP_SPI_PERI_CLK / 2;

    return S_OK;
}

uint32_t CPU_SPI_ChipSelectLineCount(uint32_t busIndex)
{
    (void)busIndex;

    return MAX_SPI_DEVICES;
}

#else

/////////////////////////////////////////////////////
// SPI PAL strucs declared in win_dev_spi_native.h //
/////////////////////////////////////////////////////
#if STM32_SPI_USE_SPI1
NF_PAL_SPI SPI1_PAL;
#endif
#if STM32_SPI_USE_SPI2
NF_PAL_SPI SPI2_PAL;
#endif
#if STM32_SPI_USE_SPI3
NF_PAL_SPI SPI3_PAL;
#endif
#if STM32_SPI_USE_SPI4
NF_PAL_SPI SPI4_PAL;
#endif
#if STM32_SPI_USE_SPI5
NF_PAL_SPI SPI5_PAL;
#endif
#if STM32_SPI_USE_SPI6
NF_PAL_SPI SPI6_PAL;
#endif

// Invalidate the cache over the read buffer, if any, so the content that DMA has transfered is the one being read
// (only required for Cortex-M7)
// It's safe to call this from an ISR: these are cache maintenance operations, no OS calls involved.
static void InvalidateReadBuffer(NF_PAL_SPI *palSpi)
{
    // event occurred
    if (palSpi->ReadSize > 0)
    {
        // because this was a Read transaction, need to copy from DMA buffer to managed buffer
        int readSize = palSpi->ReadSize;

        // Adjust read size for data width of 16bits
        if (palSpi->BufferIs16bits)
        {
            readSize *= 2;
        }

        // invalidate cache over read buffer to ensure that content from DMA is read
        // (only required for Cortex-M7)
        // get the pointer to the read buffer as UINT16 because it's really an UINT16 (2 bytes)
        cacheBufferInvalidate(palSpi->ReadBuffer, readSize);
    }
}

// Tidy up after completing tranfer
// CAUTION: has to be called from thread context, never from an ISR!
// spiReleaseBus() unlocks a mutex which is owned by the thread that acquired the bus, so calling it
// from an ISR would operate on whatever thread happens to be running at that moment.
static void CompleteTranfer(NF_PAL_SPI *palSpi)
{
    if (palSpi->TransferFailed)
    {
        // DMA error: the driver is left active, stop the transfer to get it back to ready...
        (void)spiStopTransfer(palSpi->Driver, NULL);
    }

    spiUnselect(palSpi->Driver);

    if (palSpi->TransferFailed)
    {
        // ... and stop the driver, which releases its DMA streams and disables the peripheral
        spiStop(palSpi->Driver);
        ClearDmaError(palSpi->Driver);
    }

    spiReleaseBus(palSpi->Driver);
}

// Waits for the async transfer in progress to complete.
// The completion callback always runs when the transfer ends, so there is no timeout here: the bus must never be
// released while a transfer is still using it, no matter how long it takes (e.g. a large display update).
static void WaitAsyncTransfer(NF_PAL_SPI *palSpi)
{
    osalSysLock();

    if (!palSpi->AsyncTransferComplete)
    {
        // resumed by the completion callback
        (void)osalThreadSuspendS(&palSpi->AsyncWaiter);
    }

    osalSysUnlock();
}

// Completes the async transfer started by the calling thread, if any, and releases the bus.
// The bus is a mutex owned by the thread that acquired it, so only that thread can release it. Any other thread
// returns without doing anything and gets the bus through spiAcquireBus(), once the owner has released it.
static void CompleteAsyncTranfer(NF_PAL_SPI *palSpi)
{
    if (palSpi->AsyncOwner != chThdGetSelfX())
    {
        // no async transfer in progress, or it belongs to another thread
        return;
    }

    WaitAsyncTransfer(palSpi);

    palSpi->AsyncOwner = nullptr;

    CompleteTranfer(palSpi);
}

// Return the NF_PAL structure for an SPI driver
// Return nullptr if the driver isn't one of the buses of the SPI PAL
static NF_PAL_SPI *GetNfPalFromDriver(SPIDriver *spip)
{
#if STM32_SPI_USE_SPI1
    if (spip == &SPID1)
    {
        return &SPI1_PAL;
    }
#endif
#if STM32_SPI_USE_SPI2
    if (spip == &SPID2)
    {
        return &SPI2_PAL;
    }
#endif
#if STM32_SPI_USE_SPI3
    if (spip == &SPID3)
    {
        return &SPI3_PAL;
    }
#endif
#if STM32_SPI_USE_SPI4
    if (spip == &SPID4)
    {
        return &SPI4_PAL;
    }
#endif
#if STM32_SPI_USE_SPI5
    if (spip == &SPID5)
    {
        return &SPI5_PAL;
    }
#endif
#if STM32_SPI_USE_SPI6
    if (spip == &SPID6)
    {
        return &SPI6_PAL;
    }
#endif

    (void)spip;

    return nullptr;
}

// Ends the transfer in progress, from the SPI callbacks (ISR context)
static void FinishTransferFromIsr(NF_PAL_SPI *palSpi, bool failed)
{
    if (!failed)
    {
        // invalidate cache over the read buffer, if any
        InvalidateReadBuffer(palSpi);
    }

    if (palSpi->ChipSelect >= 0)
    {
        CPU_GPIO_TogglePinState(palSpi->ChipSelect);
    }

    palSpi->TransferFailed = failed;

    if (palSpi->Callback)
    {
        palSpi->Callback(palSpi->BusIndex);
    }

    // flag the transfer as completed and resume the thread waiting for it, if any
    osalSysLockFromISR();
    palSpi->AsyncTransferComplete = true;
    osalThreadResumeI(&palSpi->AsyncWaiter, MSG_OK);
    osalSysUnlockFromISR();
}

// Callback used when a async opertion completes
static void SpiCallback(SPIDriver *spip)
{
    NATIVE_INTERRUPT_START

    NF_PAL_SPI *palSpi = GetNfPalFromDriver(spip);

    if (palSpi == nullptr)
    {
        NATIVE_INTERRUPT_END
        return;
    }

    // check if there is any Rx operation due
    if (palSpi->SequentialTxRx)
    {
        // yes there is!
        // clear flag and...
        palSpi->SequentialTxRx = false;

        // ... start it
        if (palSpi->BusConfiguration == SpiBusConfiguration_HalfDuplex)
        {
            // half duplex operation, clear output enable bit
            palSpi->Driver->spi->CR1 &= ~SPI_CR1_BIDIOE;
        }

        osalSysLockFromISR();
        spiStartReceiveI(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
        osalSysUnlockFromISR();
    }
    else
    {
        // all done here!
        FinishTransferFromIsr(palSpi, false);
    }

    NATIVE_INTERRUPT_END
};

// Callback used when an async operation fails with a DMA error
// (only reached when the target DMA error hook doesn't halt the system, see mcuconf.h)
static void SpiErrorCallback(SPIDriver *spip)
{
    NATIVE_INTERRUPT_START

    NF_PAL_SPI *palSpi = GetNfPalFromDriver(spip);

    if (palSpi != nullptr)
    {
        // the Rx phase of a sequential transfer won't happen
        palSpi->SequentialTxRx = false;

        FinishTransferFromIsr(palSpi, true);
    }

    NATIVE_INTERRUPT_END
}

// Computes the SPI peripheral baud rate according to the requested frequency
uint16_t ComputeBaudRate(SPI_DEVICE_CONFIGURATION &config, int32_t &actualFrequency)
{
    uint16_t divider = 0;
    int32_t maxSpiFrequency;
    int32_t requestedFrequency = config.Clock_RateHz;
    // bus index is 0 based, here it's 1 based
    int busIndex = config.Spi_Bus + 1;

#if defined(STM32L0XX)

    // SP1 is feed by APB2 (STM32_PCLK2)
    actualFrequency = STM32_PCLK2;

    // SPI2 is feed by APB1 (STM32_PCLK1)
    if (busIndex == 2)
    {
        actualFrequency = STM32_PCLK1;
    }

    // from datasheet
    maxSpiFrequency = 12000000;

#elif defined(STM32F0XX)

    (void)busIndex;

    // STM32F0 SPI is always feed by APB1
    actualFrequency = STM32_PCLK1;

    // from datasheet
    maxSpiFrequency = 18000000;

#elif defined(STM32F4XX) || defined(STM32F7XX)

    // SP1, SPI4, SPI5 and SPI6 are feed by APB2 (STM32_PCLK2)
    actualFrequency = STM32_PCLK2;

    // SPI2 and SPI3 are feed by APB1 (STM32_PCLK1)
    if (busIndex == 2 || busIndex == 3)
    {
        actualFrequency = STM32_PCLK1;
    }

    // this is not really accurate because there are different max SPI clocks depending on which APB clock source if
    // feeding the SPI because ChibiOS doesn't offer that we have to go with minimum common denominator
    maxSpiFrequency = STM32_SPII2S_MAX;

#elif defined(STM32H7XX)

    // SP1, SPI4, SPI5 and SPI6 are feed by APB2 (STM32_PCLK2)
    actualFrequency = STM32_PCLK2;
    maxSpiFrequency = STM32_SPI456_MAX;

    // SPI1, SPI2 and SPI3 are feed by APB1 (STM32_PCLK1)
    if (busIndex == 2 || busIndex == 3)
    {
        actualFrequency = STM32_PCLK1;
        maxSpiFrequency = STM32_SPI123_MAX;
    }

#elif defined(STM32L4XX)

    // SPI1 is feed by APB2 (STM32_PCLK2)
    actualFrequency = STM32_PCLK2;

    // SPI2 and SPI3 are feed by APB1 (STM32_PCLK1)
    if (busIndex == 2 || busIndex == 3)
    {
        actualFrequency = STM32_PCLK1;
    }

    // from datasheet, SPI max is half of the feeding peripheral clock
    maxSpiFrequency = 40000000;

#else

#error "Error setting max SPI frequency. Check if the target series is defined."

#endif

    // when requested frequency is 0, means that the developer hasn't set ClockFrequency in SpiConnectionSettings
    // default to the max possible SPI frequency
    if (requestedFrequency == 0)
    {
        requestedFrequency = maxSpiFrequency;
    }

    for (; divider < 8; divider++)
    {
        actualFrequency = actualFrequency / 2;

        if (actualFrequency <= requestedFrequency)
        {
            // best match for the requested frequency
            // just check if it's below the max SPI frequency
            if (actualFrequency <= maxSpiFrequency)
            {
                // we are good with this value
                break;
            }
        }
    }

    // the baud rate bits are in the position B5:3 so need to left shit the divider value
    return divider << 3;
}

// Return the NF_PAL structure for busIndex
// Return nullptr is invalid bus
NF_PAL_SPI *GetNfPalfromBusIndex(uint8_t busIndex)
{
    NF_PAL_SPI *palSpi = nullptr;

    // get the PAL struct for the SPI bus
    // bus index is 0 based, here it's 1 based
    switch (busIndex + 1)
    {

#if STM32_SPI_USE_SPI1
        case 1:
            palSpi = &SPI1_PAL;
            break;
#endif

#if STM32_SPI_USE_SPI2
        case 2:
            palSpi = &SPI2_PAL;
            break;
#endif

#if STM32_SPI_USE_SPI3
        case 3:
            palSpi = &SPI3_PAL;
            break;
#endif

#if STM32_SPI_USE_SPI4
        case 4:
            palSpi = &SPI4_PAL;
            break;
#endif

#if STM32_SPI_USE_SPI5
        case 5:
            palSpi = &SPI5_PAL;
            break;
#endif

#if STM32_SPI_USE_SPI6
        case 6:
            palSpi = &SPI6_PAL;
            break;
#endif

        default:
            // the requested SPI bus is not valid
            break;
    }

    return palSpi;
}

// Give a complete low-level SPI configuration from passed SPI_DEVICE_CONFIGURATION
void GetSPIConfig(SPI_DEVICE_CONFIGURATION &config, SPI_WRITE_READ_SETTINGS &wrc, SPIConfig *llConfig)
{
    int32_t actualFrequency;

    // clear values
    llConfig->cr1 = 0;
    llConfig->cr2 = 0;

    // SPI mode
    switch (config.Spi_Mode)
    {
        case SpiMode_Mode1:
            llConfig->cr1 |= SPI_CR1_CPHA;
            break;

        case SpiMode_Mode2:
            llConfig->cr1 |= SPI_CR1_CPOL;
            break;

        case SpiMode_Mode3:
            llConfig->cr1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
            break;

        default: // Default to Mode0 if invalid mode specified
            break;
    }

    // compute baud rate of SPI peripheral according to the requested frequency
    llConfig->cr1 |= ComputeBaudRate(config, actualFrequency);

    // set data transfer length according passed setting
    if (wrc.Bits16ReadWrite)
    {
        // Set data transfer length to 16 bits
#ifdef STM32F4XX
        llConfig->cr1 |= SPI_CR1_DFF;
#endif
#ifdef STM32F7XX
        llConfig->cr2 = SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
#endif
        // Sets the order of bytes transmission : MSB first or LSB first
        int bitOrder = config.DataOrder16;
        if (bitOrder == DataBitOrder_LSB)
        {
            llConfig->cr1 |= SPI_CR1_LSBFIRST;
        }
    }
    else
    {
        // set transfer length to 8bits
#ifdef STM32F4XX
        llConfig->cr1 &= ~SPI_CR1_DFF;
#endif
#ifdef STM32F7XX
        llConfig->cr2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
#endif
    }

    // set bus configuration
    // only required for half duplex mode
    if (config.BusConfiguration == SpiBusConfiguration_HalfDuplex)
    {
#ifdef STM32F4XX
        llConfig->cr1 |= SPI_CR1_BIDIMODE;
#endif
#ifdef STM32F7XX
        llConfig->cr2 |= SPI_CR1_BIDIMODE;
#endif
    }

    // Create the low level configuration
    llConfig->data_cb = SpiCallback;
    llConfig->error_cb = SpiErrorCallback;
}

// Performs a read/write operation on 8-bit word data.
//
// Parameters
//  deviceHandle
//      Device handle from add_device
//  sdev
//		reference to SPI_DEVICE_CONFIGURATION
//  wrc
//		reference to SPI_WRITE_READ_SETTINGS
//  writeData
//      A pointer to the buffer from which the data is to be written to the device.
//  writeSize
//      The number of elements(8 or 16) to be written.
//  readData
//      A pointer to the buffer into which the data is to be read from the device.
//  readSize
//      The number of elements(8 or 16) to be read.
//
// return S_OK=Successful, Async started=CLR_BUSY, Error=CLR_E_OUT_OF_MEMORY, CLR_E_INVALID_PARAMETER, CLR_E_FAIL
//
HRESULT CPU_SPI_nWrite_nRead(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &wrc,
    uint8_t *writeBuffer,
    int32_t writeSize,
    uint8_t *readBuffer,
    int32_t readSize)
{
    NANOCLR_HEADER();
    {
        bool busConfigIsHalfDuplex;
        NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;
        bool sync = (wrc.callback == 0); // If callback then use aync operation

        SPIDriver *driver = palSpi->Driver;

        if (driver == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // complete a previous async transfer from this thread, which still holds the bus
        // (otherwise this thread would deadlock trying to acquire a bus that it already owns)
        CompleteAsyncTranfer(palSpi);

        // acquire the bus before touching the PAL struct: it's shared by all the devices on this bus and it's also
        // used by the completion callback of a transfer that can still be in progress
        spiAcquireBus(driver);

        if (palSpi->Driver != driver)
        {
            spiReleaseBus(driver);

            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // clear transfer error state
        palSpi->TransferFailed = false;
        ClearDmaError(palSpi->Driver);

        // Save width of transfer
        palSpi->BufferIs16bits = wrc.Bits16ReadWrite;

        // Callback sync / async
        palSpi->Callback = wrc.callback;

        if (writeBuffer != nullptr)
        {
            palSpi->WriteSize = writeSize;
        }

        if (readBuffer != nullptr)
        {
            palSpi->ReadSize = readSize;
        }

        // === Setup the operation and init buffers ===
        palSpi->BusIndex = sdev.Spi_Bus;

        // get the LL SPI configuration, depending on passed parameters and buffer element size
        GetSPIConfig(sdev, wrc, &palSpi->Configuration);

        // set bus config flag
        busConfigIsHalfDuplex = palSpi->BusConfiguration == SpiBusConfiguration_HalfDuplex;

        // Clear callbacks if sync
        if (sync)
        {
            palSpi->Configuration.data_cb = nullptr;
            palSpi->Configuration.error_cb = nullptr;
        }

        if (writeBuffer != nullptr)
        {
            // set the pointer to the write buffer as BYTE
            palSpi->WriteBuffer = (uint8_t *)writeBuffer;

            // set DMA write buffer
            if (palSpi->BufferIs16bits)
            {
                // flush DMA buffer to ensure cache coherency
                // (only required for Cortex-M7)
                cacheBufferFlush(palSpi->WriteBuffer, (palSpi->WriteSize * 2));
            }
            else
            {
                // flush DMA buffer to ensure cache coherency
                // (only required for Cortex-M7)
                cacheBufferFlush(palSpi->WriteBuffer, palSpi->WriteSize);
            }
        }

        if (readBuffer != nullptr)
        {
            // set DMA read buffer
            if (palSpi->ReadSize > 0)
            {
                palSpi->ReadBuffer = (uint8_t *)readBuffer;
            }
        }

        // configure the bus for this device (the bus was acquired above)
        spiStart(palSpi->Driver, &palSpi->Configuration);

        // just to satisfy the driver ceremony, no actual implementation for STM32
        spiSelect(palSpi->Driver);

        palSpi->ChipSelect = wrc.DeviceChipSelect;
        // if CS is to be controlled by the driver, set the GPIO
        if (wrc.DeviceChipSelect >= 0)
        {
            // assert pin based on CS active level
            CPU_GPIO_SetPinState(wrc.DeviceChipSelect, (GpioPinValue)wrc.ChipSelectActiveState);
        }

        if (sync)
        {
            // Sync operation
            // perform SPI operation using driver's SYNC API
            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                // Transmit+Receive
                if (wrc.fullDuplex)
                {
                    // Full duplex
                    // Uses the largest buffer size as transfer size
                    spiExchange(
                        palSpi->Driver,
                        palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                        palSpi->WriteBuffer,
                        palSpi->ReadBuffer);
                }
                else
                {
                    // send operation
                    if (busConfigIsHalfDuplex)
                    {
                        // half duplex operation, set output enable
                        palSpi->Driver->spi->CR1 |= SPI_CR1_BIDIOE;
                    }

                    spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);

                    // receive operation, unless the send failed
                    if (!DmaErrorOccurred(palSpi->Driver))
                    {
                        if (busConfigIsHalfDuplex)
                        {
                            // half duplex operation, set output enable
                            palSpi->Driver->spi->CR1 &= ~SPI_CR1_BIDIOE;
                        }

                        spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                    }
                }
            }
            else
            {
                // Transmit only or Receive only
                if (palSpi->ReadSize != 0)
                {
                    // receive
                    if (busConfigIsHalfDuplex)
                    {
                        // half duplex operation, set output enable
                        palSpi->Driver->spi->CR1 &= ~SPI_CR1_BIDIOE;
                    }

                    spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    // send
                    if (busConfigIsHalfDuplex)
                    {
                        // half duplex operation, set output enable
                        palSpi->Driver->spi->CR1 |= SPI_CR1_BIDIOE;
                    }

                    spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }

            // a DMA error fails the transfer
            bool failed = DmaErrorOccurred(palSpi->Driver);
            palSpi->TransferFailed = failed;

            if (!failed)
            {
                // invalidate cache over the read buffer, if any
                InvalidateReadBuffer(palSpi);
            }

            // Release bus etc
            CompleteTranfer(palSpi);

            // if CS is to be controlled by the driver, set the GPIO
            if (wrc.DeviceChipSelect >= 0)
            {
                // de-assert pin based on CS active level
                CPU_GPIO_SetPinState(wrc.DeviceChipSelect, (GpioPinValue)!wrc.ChipSelectActiveState);
            }

            if (failed)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_IO);
            }
        }
        else
        {
            // Start an Asyncronous SPI transfer
            // perform SPI operation using driver's ASYNC API
            // Completed on calling Spi Callback

            // the bus stays held by this thread until the transfer completes (see CompleteAsyncTranfer)
            palSpi->AsyncTransferComplete = false;
            palSpi->AsyncOwner = chThdGetSelfX();

            // if CS is to be controlled by the driver, set the GPIO
            if (wrc.DeviceChipSelect >= 0)
            {
                // assert pin based on CS active level
                CPU_GPIO_SetPinState(wrc.DeviceChipSelect, (GpioPinValue)wrc.ChipSelectActiveState);
            }

            // this is a Async operation
            // perform SPI operation using driver's ASYNC API
            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                // Transmit+Receive
                if (wrc.fullDuplex)
                {
                    // Full duplex
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // Uses the largest buffer size as transfer size
                    spiStartExchange(
                        palSpi->Driver,
                        palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                        palSpi->WriteBuffer,
                        palSpi->ReadBuffer);
                }
                else
                {
                    // flag that an Rx is required after the Tx operation completes
                    palSpi->SequentialTxRx = true;

                    // start send operation
                    if (busConfigIsHalfDuplex)
                    {
                        // half duplex operation, set output enable
                        palSpi->Driver->spi->CR1 |= SPI_CR1_BIDIOE;
                    }

                    spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                    // receive operation will be started in the callback after the above completes
                }
            }
            else
            {
                // Transmit only or Receive only
                if (palSpi->ReadSize != 0)
                {
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // start receive
                    spiStartReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // start send
                    spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }

            // Inform caller async operation started
            NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
        }
    }

    NANOCLR_NOCLEANUP();
}

SPI_OP_STATUS CPU_SPI_OP_Status(uint8_t busIndex, uint32_t deviceHandle)
{
    (void)busIndex;

    NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;
    SPI_OP_STATUS os;

    if (palSpi->AsyncOwner != nullptr)
    {
        // async transfer in progress, or completed and still holding the bus
        if (!palSpi->AsyncTransferComplete)
        {
            return SPI_OP_RUNNING;
        }

        bool failed = palSpi->TransferFailed;

        // completed: release the bus
        CompleteAsyncTranfer(palSpi);

        return failed ? SPI_OP_FAILED : SPI_OP_COMPLETE;
    }

    switch (palSpi->Driver->state)
    {
        default:
        case SPI_UNINIT:
        case SPI_STOP:
        case SPI_READY:
            os = SPI_OP_READY;
            break;

        case SPI_ACTIVE:
            os = SPI_OP_RUNNING;
            break;

        case SPI_COMPLETE:
            os = SPI_OP_COMPLETE;
            break;
    }
    return os;
}

// Waits for an async transfer started by the calling thread to complete and releases the bus
void CPU_SPI_Wait_Busy(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION &sdev)
{
    (void)sdev;

    NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;

    if (palSpi != nullptr)
    {
        CompleteAsyncTranfer(palSpi);
    }
}

// Aborts the async transfer started by the calling thread, if any, and releases the bus.
// DMA is stopped before the bus is released, so the transfer buffers are no longer in use when this returns.
void CPU_SPI_Abort(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION &sdev)
{
    (void)sdev;

    NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;
    bool aborted = false;

    if (palSpi == nullptr || palSpi->AsyncOwner != chThdGetSelfX())
    {
        // no async transfer in progress, or it belongs to another thread
        return;
    }

    osalSysLock();

    if (!palSpi->AsyncTransferComplete)
    {
        // stop the transfer,
        (void)spiStopTransferI(palSpi->Driver, NULL);

        // the transfer is over: report it as failed
        palSpi->SequentialTxRx = false;
        palSpi->TransferFailed = true;
        palSpi->AsyncTransferComplete = true;

        aborted = true;
    }

    osalSysUnlock();

    if (aborted && palSpi->ChipSelect >= 0)
    {
        CPU_GPIO_TogglePinState(palSpi->ChipSelect);
    }

    // release the bus
    CompleteAsyncTranfer(palSpi);
}

bool CPU_SPI_Initialize(uint8_t busIndex, const SPI_DEVICE_CONFIGURATION &spiDeviceConfig)
{
    // init the PAL struct for this SPI bus and assign the respective driver
    // all this occurs if not already done
    // why do we need this? because several SPIDevice objects can be created associated to the same bus

    // bus index is 0 based, here it's 1 based
    switch (busIndex + 1)
    {
#if STM32_SPI_USE_SPI1
        case 1:
            if (SPI1_PAL.Driver == nullptr)
            {
                ConfigPins_SPI1(spiDeviceConfig);
                SPI1_PAL.Driver = &SPID1;
                SPI1_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
#if STM32_SPI_USE_SPI2
        case 2:
            if (SPI2_PAL.Driver == nullptr)
            {
                ConfigPins_SPI2(spiDeviceConfig);
                SPI2_PAL.Driver = &SPID2;
                SPI2_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
#if STM32_SPI_USE_SPI3
        case 3:
            if (SPI3_PAL.Driver == nullptr)
            {
                ConfigPins_SPI3(spiDeviceConfig);
                SPI3_PAL.Driver = &SPID3;
                SPI3_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
#if STM32_SPI_USE_SPI4
        case 4:
            if (SPI4_PAL.Driver == nullptr)
            {
                ConfigPins_SPI4(spiDeviceConfig);
                SPI4_PAL.Driver = &SPID4;
                SPI4_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
#if STM32_SPI_USE_SPI5
        case 5:
            if (SPI5_PAL.Driver == nullptr)
            {
                ConfigPins_SPI5(spiDeviceConfig);
                SPI5_PAL.Driver = &SPID5;
                SPI5_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
#if STM32_SPI_USE_SPI6
        case 6:
            if (SPI6_PAL.Driver == nullptr)
            {
                ConfigPins_SPI6(spiDeviceConfig);
                SPI6_PAL.Driver = &SPID6;
                SPI6_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
        default:
            // this SPI bus is not valid
            return false;
    }

    return true;
}

bool CPU_SPI_Uninitialize(uint8_t busIndex)
{
    // get the PAL struct for the SPI bus
    NF_PAL_SPI *palSpi = GetNfPalfromBusIndex(busIndex);

    if (palSpi == nullptr)
    {
        return false;
    }

    if (palSpi->Driver == nullptr)
    {
        // this bus is not initialized, nothing to do here
        return true;
    }

    SPIDriver *driver = palSpi->Driver;

    // complete an async transfer from this thread, which still holds the bus
    CompleteAsyncTranfer(palSpi);

    // acquire the bus before stopping it: it can be in use by a transfer from another thread
    // or by another driver sharing it
    spiAcquireBus(driver);

    spiStop(driver);

    palSpi->Driver = nullptr;

    spiReleaseBus(driver);

    return true;
}

// return Map of available SPI ports
uint32_t CPU_SPI_PortsMap()
{
    uint32_t map = 0;

#if STM32_SPI_USE_SPI1
    map |= 0x01;
#endif
#if STM32_SPI_USE_SPI2
    map |= 0x02;
#endif
#if STM32_SPI_USE_SPI3
    map |= 0x04;
#endif
#if STM32_SPI_USE_SPI4
    map |= 0x08;
#endif
#if STM32_SPI_USE_SPI5
    map |= 0x10;
#endif
#if STM32_SPI_USE_SPI6
    map |= 0x20;
#endif
    return map;
}

// Add a device to SPi Bus (Optional)
// Returns a device handle.  Returns 0 if error
HRESULT CPU_SPI_Add_Device(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, uint32_t &handle)
{
    // check supported bus configuration: all valid except simplex
    if (spiDeviceConfig.BusConfiguration == SpiBusConfiguration_Simplex)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    handle = (uint32_t)GetNfPalfromBusIndex(spiDeviceConfig.Spi_Bus);

    return S_OK;
}

// Return pins used for SPI bus
void CPU_SPI_GetPins(uint32_t busIndex, GPIO_PIN &clk, GPIO_PIN &miso, GPIO_PIN &mosi)
{
    (void)busIndex;

    clk = (GPIO_PIN)-1;
    miso = (GPIO_PIN)-1;
    mosi = (GPIO_PIN)-1;
}

// Minimum and Maximum clock frequency available based on bus and configured pins
HRESULT CPU_SPI_MinClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    // bus index is 0 based, here it's 1 based
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // Max prescaler value = 256
    // SPI2 or SPI3 are on APB1, so divide max frequency by four.
    // bus index is 0 based, here it's 1 based
    *frequency = (spiBus + 1 == 2 or spiBus + 1 == 3) ? SystemCoreClock >>= 9 : SystemCoreClock >> 8;

    return S_OK;
}

HRESULT CPU_SPI_MaxClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    // bus index is 0 based, here it's 1 based
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // According to STM : "At a minimum, the clock frequency should be twice the required communication frequency."
    // So maximum useable frequency is CoreClock / 2.
    // SPI2 or SPI3 are on APB1, so divide max frequency by four.
    // bus index is 0 based, here it's 1 based
    *frequency = (spiBus + 1 == 2 or spiBus + 1 == 3) ? SystemCoreClock >>= 2 : SystemCoreClock >> 1;

    return S_OK;
}

// Maximum number of SPI devices that can be opened on a bus
uint32_t CPU_SPI_ChipSelectLineCount(uint32_t busIndex)
{
    (void)busIndex;

    return MAX_SPI_DEVICES;
}

#endif
