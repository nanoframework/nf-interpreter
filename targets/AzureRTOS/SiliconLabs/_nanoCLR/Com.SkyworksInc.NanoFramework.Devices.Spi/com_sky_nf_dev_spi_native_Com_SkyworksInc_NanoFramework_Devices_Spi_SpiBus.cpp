// Copyright Skyworks Solutions, Inc. All Rights Reserved.

#include "com_sky_nf_dev_spi_native_target.h"

extern NF_PAL_SPI *GetNfPalfromBusIndex(uint8_t busIndex);
extern void SpiTransferCompleteCallback(NF_SpiDriver_Handle_t handle, Ecode_t transferStatus, int itemsTransferred);
extern bool CPU_SPI_Initialize_Extended(
    uint8_t busIndex,
    const SPI_DEVICE_CONFIGURATION &busConfiguration,
    bool reconfigure);

typedef Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBus Devices_Spi_SpiBus;
typedef Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBaseConfiguration
    SpiBaseConfiguration;
typedef Library_corlib_native_System_SpanByte SpanByte;

static HRESULT SPI_nWrite_nRead(
    NF_PAL_SPI *palSpi,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &wrc,
    uint8_t *writeBuffer,
    int32_t writeSize,
    uint8_t *readBuffer,
    int32_t readSize);

// Saved config for each available SPI bus
SPI_DEVICE_CONFIGURATION SpiConfigs[NUM_SPI_BUSES];

// flags for bus config changes pending
bool BusConfigChangesPending[NUM_SPI_BUSES];

void Com_Sky_Spi_Callback(int busIndex)
{
    (void)busIndex;

    // fire event for SPI transaction complete
    Events_Set(SYSTEM_EVENT_FLAG_SPI_MASTER);
}

void ConfigureAndInitSpiBus(
    int8_t busIndex,
    SPI_DEVICE_CONFIGURATION *&spiDeviceConfig,
    const CLR_RT_HeapBlock *baseConfig)
{
    NF_PAL_SPI *palSpi = NULL;

    // CS is always active low
    spiDeviceConfig->ChipSelectActiveState = false;
    // CS is controled by the Gecko SDK driver
    spiDeviceConfig->DeviceChipSelect = -1;
    // always bus master
    spiDeviceConfig->BusMode = SpiBusMode_master;

    // SPI bus index is 1 based, but the array is 0 based
    spiDeviceConfig->Spi_Bus = busIndex + 1;

    // this comes from Com.SkyworksInc.NanoFramework.Devices.Spi.SpiWireMode
    // which is stored at _spiPhasePolarityMode
    spiDeviceConfig->Spi_Mode =
        (SpiMode)baseConfig[SpiBaseConfiguration::FIELD___spiPhasePolarityMode].NumericByRefConst().s4;
    spiDeviceConfig->DataOrder16 =
        (DataBitOrder)baseConfig[SpiBaseConfiguration::FIELD___dataFlow].NumericByRefConst().s4;
    spiDeviceConfig->Clock_RateHz = baseConfig[SpiBaseConfiguration::FIELD___clockFrequency].NumericByRefConst().s4;
    spiDeviceConfig->ByteTime = (1.0 / spiDeviceConfig->Clock_RateHz) * 1000 * 8;
    // this comes from Com.SkyworksInc.NanoFramework.Devices.Spi.SpiWireMode
    // which is stored at _spiWireMode field
    spiDeviceConfig->BusConfiguration =
        (SpiBusConfiguration)baseConfig[SpiBaseConfiguration::FIELD___spiWireMode].NumericByRefConst().s4;
    spiDeviceConfig->DataIs16bits =
        baseConfig[SpiBaseConfiguration::FIELD___databitLength].NumericByRefConst().s4 == 16 ? true : false;

    // get PAL SPI
    palSpi = GetNfPalfromBusIndex(busIndex);

    // store this here too
    palSpi->BufferIs16bits = spiDeviceConfig->DataIs16bits;

    CPU_SPI_Initialize_Extended(busIndex, *spiDeviceConfig, true);

    // lower changes pending flag
    BusConfigChangesPending[busIndex] = false;
}

// duplicated from src\System.Device.Spi\sys_dev_spi_native_System_Device_Spi_SpiDevice.cpp
// estimate the time required to perform the SPI transaction
// TODO doesn't take into account of full duplex or sequential ( assumes sequential at the moment )
bool System_Device_IsLongRunningOperation(
    uint32_t writeSize,
    uint32_t readSize,
    bool fullDuplex,
    bool bufferIs16bits,
    float byteTime,
    uint32_t &estimatedDurationMiliseconds)
{
    if (bufferIs16bits)
    {
        // double the buffers size
        writeSize = 2 * writeSize;
        readSize = 2 * readSize;
    }

    if (fullDuplex)
    {
        estimatedDurationMiliseconds = byteTime * MAX(writeSize, readSize);
    }
    else
    {
        estimatedDurationMiliseconds = byteTime * (writeSize + readSize);
    }

    if (estimatedDurationMiliseconds > CLR_RT_Thread::c_TimeQuantum_Milliseconds)
    {
        // total operation time will exceed thread quantum, so this is a long running operation
        return true;
    }
    else
    {
        return false;
    }
}

HRESULT ExecuteTransfer(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *baseConfig = NULL;
    CLR_RT_HeapBlock_Array *writeBuffer;
    CLR_RT_HeapBlock_Array *readBuffer;
    CLR_RT_HeapBlock *writeSpanByte;
    CLR_RT_HeapBlock *readSpanByte;
    uint8_t *writeData = NULL;
    uint8_t *readData = NULL;
    int16_t writeSize = 0;
    int16_t readSize = 0;
    int16_t readOffset = 0;
    int16_t writeOffset = 0;
    bool fullDuplex;

    bool isLongRunningOperation;
    uint32_t estimatedDurationMiliseconds;
    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;
    int8_t busIndex;
    SPI_WRITE_READ_SETTINGS rws;
    NF_PAL_SPI *palSpi = NULL;
    SPI_DEVICE_CONFIGURATION *spiDeviceConfig = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get bus index
    busIndex = (int8_t)stack.Arg1().NumericByRef().s4;

    // SPI bus index is 1 based, but the array is 0 based
    busIndex--;

    spiDeviceConfig = &SpiConfigs[busIndex];

    if (stack.m_customState == 0)
    {
        // check if this SPI has been initialized
        palSpi = GetNfPalfromBusIndex(busIndex);

        if (palSpi->Handle == NULL || BusConfigChangesPending[busIndex])
        {
            // SPI bus not initialized or config changes pending

            // compose SPI_DEVICE_CONFIGURATION
            // get SpiBaseConfiguration from argument
            baseConfig = stack.Arg5().Dereference();

            ConfigureAndInitSpiBus(busIndex, spiDeviceConfig, baseConfig);
        }

        // dereference the write and read SpanByte from the arguments
        writeSpanByte = stack.Arg2().Dereference();

        if (writeSpanByte != NULL)
        {
            // get buffer
            writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();

            if (writeBuffer != NULL)
            {
                // Get the write offset, only the elements defined by the span must be written, not the whole
                // array
                writeOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // use the span length as write size, only the elements defined by the span must be written
                writeSize = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
                writeData = (unsigned char *)writeBuffer->GetElement(writeOffset);
            }
        }

        if (writeData == NULL)
        {
            // nothing to write, have to zero this
            writeSize = 0;
        }

        readSpanByte = stack.Arg3().Dereference();

        if (readSpanByte != NULL)
        {
            // get buffer
            readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();

            if (readBuffer != NULL)
            {
                // Get the read offset, only the elements defined by the span must be read, not the whole array
                readOffset = readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // use the span length as read size, only the elements defined by the span must be read
                readSize = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
                readData = (unsigned char *)readBuffer->GetElement(readOffset);
            }
        }

        if (readData == NULL)
        {
            // nothing to read, have to zero this
            readSize = 0;
        }

        // assuming full duplex all the time
        fullDuplex = true;

        // Set up read/write settings for SPI_Write_Read call
        rws = {fullDuplex, 0, palSpi->BufferIs16bits, 0, palSpi->ChipSelect, false};

        // Check to see if we should run async so as not to hold up other tasks
        isLongRunningOperation = System_Device_IsLongRunningOperation(
            writeSize,
            readSize,
            fullDuplex,
            palSpi->BufferIs16bits,
            spiDeviceConfig->ByteTime,
            (uint32_t &)estimatedDurationMiliseconds);

        if (isLongRunningOperation)
        {
            // if this is a long running operation, set a timeout equal to the estimated transaction duration in
            // milliseconds this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() below

            // Use twice the estimated Duration as timeout
            estimatedDurationMiliseconds *= 2;

            hbTimeout.SetInteger((CLR_INT64)estimatedDurationMiliseconds * TIME_CONVERSION__TO_MILLISECONDS);

            // if m_customState == 0 then push timeout on to eval stack[0] then move to m_customState = 1
            // Return current timeout value
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

            // protect the buffers from GC so DMA can find them where they are supposed to be
            if (writeData != NULL)
            {
                CLR_RT_ProtectFromGC gcWriteBuffer(*writeBuffer);
            }
            if (readData != NULL)
            {
                CLR_RT_ProtectFromGC gcReadBuffer(*readBuffer);
            }

            // Set callback for async calls to nano spi
            rws.callback = Com_Sky_Spi_Callback;
        }

        // Start SPI transfer
        // We can ask for async transfer by setting callback but it depends if underlying supports it
        // return of CLR_E_BUSY means async started
        hr = SPI_nWrite_nRead(
            palSpi,
            SpiConfigs[busIndex - 1],
            rws,
            (uint8_t *)writeData,
            (int32_t)writeSize,
            (uint8_t *)readData,
            (int32_t)readSize);

        // Async transfer started, go to custom 2 state (wait completion)
        if (hr == CLR_E_BUSY)
        {
            stack.m_customState = 2;
        }
    }

    // Waiting for Async operation to complete
    if (stack.m_customState == 2)
    {
        // Get timeout from eval stack we set up
        stack.SetupTimeoutFromTicks(hbTimeout, timeout);

        while (eventResult)
        {
            // non-blocking wait allowing other threads to run while we wait for the Spi transaction to complete
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_SpiMaster, eventResult));

            if (!eventResult)
            {
                // Timeout
                NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
            }
        }

        // pop timeout heap block from stack
        stack.PopValue();

        // null pointers and vars
        pThis = NULL;
    }

    NANOCLR_NOCLEANUP();
}

static HRESULT SPI_nWrite_nRead(
    NF_PAL_SPI *palSpi,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &wrc,
    uint8_t *writeBuffer,
    int32_t writeSize,
    uint8_t *readBuffer,
    int32_t readSize)
{
    NANOCLR_HEADER();

    bool busConfigIsHalfDuplex;

    // If callback then use async operation
    bool sync = (wrc.callback == 0);

    // Save width of transfer
    palSpi->BufferIs16bits = wrc.Bits16ReadWrite;

    // Callback sync / async
    palSpi->Callback = wrc.callback;

    if (writeBuffer != NULL)
    {
        palSpi->WriteSize = writeSize;
    }

    if (readBuffer != NULL)
    {
        palSpi->ReadSize = readSize;
    }

    // === Setup the operation and init buffers ===
    palSpi->BusIndex = sdev.Spi_Bus;

    // adjust the bus index to match the PAL struct
    NF_SpiDriver_SetFramelength(palSpi->Handle, wrc.Bits16ReadWrite ? 16 : 8);

    // set bus config flag
    busConfigIsHalfDuplex = (palSpi->BusConfiguration == SpiBusConfiguration_HalfDuplex);

    if (writeBuffer != NULL)
    {
        // set the pointer to the write buffer as BYTE
        palSpi->WriteBuffer = (uint8_t *)writeBuffer;
    }

    if (readBuffer != NULL)
    {
        // set DMA read buffer
        if (palSpi->ReadSize > 0)
        {
            palSpi->ReadBuffer = (uint8_t *)readBuffer;
        }
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
                NF_SpiDriver_MTransferB(
                    palSpi->Handle,
                    palSpi->WriteBuffer,
                    palSpi->ReadBuffer,
                    palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize);
            }
            else
            {
                // send operation
                // TODO
                // if (busConfigIsHalfDuplex)
                // {
                //     // half duplex operation, set output enable
                //     palSpi->Handle->spi->CR1 |= SPI_CR1_BIDIOE;
                // }
                NF_SpiDriver_MTransmitB(palSpi->Handle, palSpi->WriteBuffer, palSpi->WriteSize);

                // receive operation
                // TODO
                // if (busConfigIsHalfDuplex)
                // {
                //     // half duplex operation, set output enable
                //     palSpi->Handle->spi->CR1 &= ~SPI_CR1_BIDIOE;
                // }
                NF_SpiDriver_MReceiveB(palSpi->Handle, palSpi->ReadBuffer, palSpi->ReadSize);
            }
        }
        else
        {
            // Transmit only or Receive only
            if (palSpi->ReadSize != 0)
            {
                // receive
                // TODO
                // if (busConfigIsHalfDuplex)
                // {
                //     // half duplex operation, set output enable
                //     palSpi->Handle->spi->CR1 &= ~SPI_CR1_BIDIOE;
                // }
                NF_SpiDriver_MReceiveB(palSpi->Handle, palSpi->ReadBuffer, palSpi->ReadSize);
            }
            else
            {
                // send
                // TODO
                if (busConfigIsHalfDuplex)
                {
                    // half duplex operation, set output enable
                    // palSpi->Handle->spi->CR1 |= SPI_CR1_BIDIOE;
                }
                NF_SpiDriver_MTransmitB(palSpi->Handle, palSpi->WriteBuffer, palSpi->WriteSize);
            }
        }
    }
    else
    {
        // Start an Asyncronous SPI transfer
        // perform SPI operation using driver's ASYNC API
        // Completed on calling SPI Callback

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
                NF_SpiDriver_MTransfer(
                    palSpi->Handle,
                    palSpi->WriteBuffer,
                    palSpi->ReadBuffer,
                    palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                    SpiTransferCompleteCallback);
            }
            else
            {
                // flag that an Rx is required after the Tx operation completes
                palSpi->SequentialTxRx = true;

                // start send operation
                if (busConfigIsHalfDuplex)
                {
                    // // half duplex operation, set output enable
                    // palSpi->Handle->spi->CR1 |= SPI_CR1_BIDIOE;
                }

                // receive operation will be started in the callback after the above completes
                NF_SpiDriver_MTransmit(
                    palSpi->Handle,
                    palSpi->WriteBuffer,
                    palSpi->WriteSize,
                    SpiTransferCompleteCallback);
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
                NF_SpiDriver_MReceive(
                    palSpi->Handle,
                    palSpi->ReadBuffer,
                    palSpi->ReadSize,
                    SpiTransferCompleteCallback);
            }
            else
            {
                // single operation, clear flag
                palSpi->SequentialTxRx = false;

                // start send
                NF_SpiDriver_MTransmit(
                    palSpi->Handle,
                    palSpi->WriteBuffer,
                    palSpi->WriteSize,
                    SpiTransferCompleteCallback);
            }
        }

        // Inform caller async operation started
        NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBus::
    NativeTransfer___VOID__I4__SystemSpanByte__SystemSpanByte__BOOLEAN__ComSkyworksIncNanoFrameworkDevicesSpiSpiBaseConfiguration(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(ExecuteTransfer(stack));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBus::
    NativeReportBusSettingsChanged___VOID__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int8_t busIndex;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // SPI bus index is 1 based, but the array is 0 based
    busIndex = (int8_t)stack.Arg1().NumericByRef().s4 - 1;

    BusConfigChangesPending[busIndex] = true;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBus::
    NativeGetBusSpeed___I4__I4__ComSkyworksIncNanoFrameworkDevicesSpiSpiBaseConfiguration(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_SPI *palSpi = NULL;
    int8_t busIndex;
    uint32_t clockDivValue;
    uint32_t refFreq;
    uint32_t realClk;
    CLR_RT_HeapBlock *baseConfig = NULL;
    SPI_DEVICE_CONFIGURATION *spiDeviceConfig = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get bus index
    // Gecko SPI bus index is 0 based
    busIndex = (int8_t)stack.Arg1().NumericByRef().s4 - 1;

    spiDeviceConfig = &SpiConfigs[busIndex];

    // get the PAL struct for the SPI bus
    switch (busIndex)
    {
#if GECKO_USE_SPI0 == TRUE
        case 0:
            palSpi = &SPI0_PAL;

            break;
#endif

#if GECKO_USE_SPI1 == TRUE
        case 1:
            palSpi = &SPI1_PAL;

            break;
#endif

#if GECKO_USE_SPI2 == TRUE
        case 2:
            palSpi = &SPI2_PAL;

            break;
#endif

#if GECKO_USE_SPI3 == TRUE
        case 3:
            palSpi = &SPI3_PAL;

            break;
#endif

#if GECKO_USE_SPI4 == TRUE
        case 4:
            palSpi = &SPI4_PAL;
            break;
#endif

#if GECKO_USE_SPI5 == TRUE
        case 5:
            palSpi = &SPI5_PAL;

            break;
#endif

        default:
            // the requested SPI bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (palSpi->Handle == NULL || BusConfigChangesPending[busIndex])
    {
        // the configuration has not been set yet, or there are pending changes
        // compose SPI_DEVICE_CONFIGURATION
        // get SpiBaseConfiguration from argument
        baseConfig = stack.Arg2().Dereference();

        ConfigureAndInitSpiBus(busIndex, spiDeviceConfig, baseConfig);
    }

    // The divider field of the USART->CLKDIV register is of the following form:
    // xxxxxxxxxxxxxxx.yyyyy where x is the 15 bits integral part of the divider
    // The driver it's only setting the integral part of the divider, so we just need to
    // rotate the value 5 + 3 bits to the right to get the actual divider value

    clockDivValue = palSpi->Handle->peripheral.usartPort->CLKDIV >> 8;
#if defined(_SILICON_LABS_32B_SERIES_2)
    refFreq = CMU_ClockFreqGet(cmuClock_PCLK);
#else
#if defined(_CMU_HFPERPRESCB_MASK)
    if (palSpi->Handle->peripheral.usartPort == USART2)
    {
        refFreq = CMU_ClockFreqGet(cmuClock_HFPERB);
    }
    else
    {
        refFreq = CMU_ClockFreqGet(cmuClock_HFPER);
    }
#else
    refFreq = CMU_ClockFreqGet(cmuClock_HFPER);
#endif
#endif

    realClk = (refFreq - 1) / (2 * clockDivValue);

    stack.SetResult_I4(realClk);

    NANOCLR_NOCLEANUP();
}
