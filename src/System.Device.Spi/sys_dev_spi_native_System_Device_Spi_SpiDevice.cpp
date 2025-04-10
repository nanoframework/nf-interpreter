//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <targetPAL.h>
#include <sys_dev_spi_native.h>

// define this type here to make it shorter and improve code readability
typedef Library_sys_dev_spi_native_System_Device_Spi_SpiConnectionSettings SpiConnectionSettings;
typedef Library_corlib_native_System_SpanByte SpanByte;

void System_Device_nano_spi_callback(int busIndex)
{
    (void)busIndex;

    // fire event for SPI transaction complete
    Events_Set(SYSTEM_EVENT_FLAG_SPI_MASTER);
}

// estimate the time required to perform the SPI transaction
// TODO doesn't take into account of full duplex or sequential ( assumes sequential at the moment )
bool System_Device_IsLongRunningOperation(
    uint32_t writeSize,
    uint32_t readSize,
    bool fullDuplex,
    bool bufferIs16bits,
    int32_t byteTime,
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

HRESULT Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::
    NativeTransfer___VOID__SystemSpanByte__SystemSpanByte__BOOLEAN(CLR_RT_StackFrame &stack)
{
    return NativeTransfer(stack, false);
}

HRESULT Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    return NativeTransfer(stack, true);
}

HRESULT Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeTransfer(
    CLR_RT_StackFrame &stack,
    bool data16Bits)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *writeSpanByte;
    CLR_RT_HeapBlock *readSpanByte;
    CLR_RT_HeapBlock_Array *writeBuffer = NULL;
    CLR_RT_HeapBlock_Array *readBuffer = NULL;
    uint8_t *writeData = NULL;
    uint8_t *readData = NULL;
    int16_t writeSize = 0;
    int16_t readSize = 0;
    int16_t readOffset = 0;
    int16_t writeOffset = 0;
    SPI_WRITE_READ_SETTINGS rws;
    uint32_t deviceId;

    bool isLongRunningOperation;
    uint32_t estimatedDurationMiliseconds;
    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_RT_HeapBlock *connectionSettings;
    FAULT_ON_NULL(pThis);

    // get device handle saved on open
    deviceId = pThis[Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::FIELD___deviceId].NumericByRef().u4;

    if (stack.m_customState == 0)
    {
        // For 16 bits, it's directly a buffer, for 8 bits, it's a SpanByte
        if (data16Bits)
        {
            writeBuffer = stack.Arg1().DereferenceArray();
            if (writeBuffer != NULL)
            {
                // grab the pointer to the array by getting the first element of the array
                writeData = (unsigned char *)writeBuffer->GetFirstElementUInt16();

                // get the size of the buffer by reading the number of elements in the HeapBlock array
                writeSize = writeBuffer->m_numOfElements;

                // pin the buffer
                writeBuffer->Pin();
            }

            readBuffer = stack.Arg2().DereferenceArray();
            if (readBuffer != NULL)
            {
                // grab the pointer to the array by getting the first element of the array
                readData = (unsigned char *)readBuffer->GetFirstElementUInt16();

                // get the size of the buffer by reading the number of elements in the HeapBlock array
                readSize = readBuffer->m_numOfElements;

                // pin the buffer
                readBuffer->Pin();
            }
        }
        else
        {
            // dereference the write and read SpanByte from the arguments
            writeSpanByte = stack.Arg1().Dereference();
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

                    // pin the buffer
                    writeBuffer->Pin();
                }
            }

            if (writeData == NULL)
            {
                // nothing to write, have to zero this
                writeSize = 0;
            }

            readSpanByte = stack.Arg2().Dereference();
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

                    // pin the buffer
                    readBuffer->Pin();
                }
            }

            if (readData == NULL)
            {
                // nothing to read, have to zero this
                readSize = 0;
            }
        }

        // Are we using SPI full-duplex for transfer ?
        bool fullDuplex = (bool)stack.Arg3().NumericByRef().u1;

        // Set up read/write settings for SPI_Write_Read call
        // Gets the CS and active state
        connectionSettings =
            pThis[Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::FIELD___connectionSettings].Dereference();
        int32_t chipSelect =
            connectionSettings[Library_sys_dev_spi_native_System_Device_Spi_SpiConnectionSettings::FIELD___csLine]
                .NumericByRef()
                .s4;
        bool chipSelectActiveState =
            (bool)connectionSettings
                [Library_sys_dev_spi_native_System_Device_Spi_SpiConnectionSettings::FIELD___chipSelectLineActiveState]
                    .NumericByRef()
                    .u1;
        rws = {fullDuplex, 0, data16Bits, 0, chipSelect, chipSelectActiveState};

        // Check to see if we should run async so as not to hold up other tasks
        isLongRunningOperation = System_Device_IsLongRunningOperation(
            writeSize,
            readSize,
            fullDuplex,
            data16Bits,
            nanoSPI_GetByteTime(deviceId),
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

            // Set callback for async calls to nano spi
            rws.callback = System_Device_nano_spi_callback;
        }

        // Start SPI transfer
        // We can ask for async transfer by setting callback but it depends if underlying supports it
        // return of CLR_E_BUSY means async started
        hr = nanoSPI_Write_Read(
            deviceId,
            rws,
            (uint8_t *)writeData,
            (int32_t)writeSize,
            (uint8_t *)readData,
            (int32_t)readSize);

        // Async transfer started, go to custom 2 state ( wait completion )
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
            // Has it completed ?
            if (nanoSPI_Op_Status(deviceId) == SPI_OP_COMPLETE)
            {
                // SPI driver is ready meaning that the SPI transaction(s) is(are) completed
                break;
            }

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

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING)
    {
        // unpin buffers
        if (writeBuffer != NULL && writeBuffer->IsPinned())
        {
            writeBuffer->Unpin();
        }

        if (readBuffer != NULL && readBuffer->IsPinned())
        {
            readBuffer->Unpin();
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeOpenDevice___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint32_t handle = -1;
    SPI_DEVICE_CONFIGURATION spiConfig;
    CLR_RT_HeapBlock *config = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get reference to manage code SPI settings
    config = pThis[Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::FIELD___connectionSettings].Dereference();

    spiConfig.BusMode = SpiBusMode_master;
    spiConfig.DataIs16bits = false;

    // internally SPI bus ID is zero based, so better take care of that here
    spiConfig.Spi_Bus = config[SpiConnectionSettings::FIELD___busId].NumericByRef().s4 - 1;

    spiConfig.DeviceChipSelect = config[SpiConnectionSettings::FIELD___csLine].NumericByRef().s4;

    // sanity check chip select line
    if (spiConfig.DeviceChipSelect < -1)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // load CS active state from config (which is always PinValue.Low or PinValue.High
    spiConfig.ChipSelectActiveState =
        (bool)config[SpiConnectionSettings::FIELD___chipSelectLineActiveState].NumericByRef().s4;

    spiConfig.Spi_Mode = (SpiMode)config[SpiConnectionSettings::FIELD___spiMode].NumericByRef().s4;
    spiConfig.DataOrder16 = (DataBitOrder)config[SpiConnectionSettings::FIELD___dataFlow].NumericByRef().s4;
    spiConfig.Clock_RateHz = config[SpiConnectionSettings::FIELD___clockFrequency].NumericByRef().s4;
    spiConfig.BusConfiguration =
        (SpiBusConfiguration)config[SpiConnectionSettings::FIELD___busConfiguration].NumericByRef().s4;

    // Returns handle to device
    hr = nanoSPI_OpenDevice(spiConfig, handle);
    NANOCLR_CHECK_HRESULT(hr);

    // Return device handle
    stack.SetResult_I4(handle);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        (void)stack;
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();

        // get device handle
        int32_t deviceId =
            pThis[Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::FIELD___deviceId].NumericByRef().s4;

        nanoSPI_CloseDevice(deviceId);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
