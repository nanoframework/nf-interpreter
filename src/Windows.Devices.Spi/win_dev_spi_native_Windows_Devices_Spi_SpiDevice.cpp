//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <targetPAL.h>
#include "win_dev_spi_native_target.h"

// define this type here to make it shorter and improve code readability
typedef Library_win_dev_spi_native_Windows_Devices_Spi_SpiConnectionSettings SpiConnectionSettings;

void nano_spi_callback(int busIndex)
{
    (void)busIndex;

    // fire event for SPI transaction complete
    Events_Set(SYSTEM_EVENT_FLAG_SPI_MASTER);
}

// estimate the time required to perform the SPI transaction
// TODO doesn't take into account of full duplex or sequential ( assumes sequential at the moment )
bool IsLongRunningOperation(
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

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::
    NativeTransfer___VOID__SZARRAY_U1__SZARRAY_U1__BOOLEAN(CLR_RT_StackFrame &stack)
{
    return NativeTransfer(stack, false);
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::
    NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN(CLR_RT_StackFrame &stack)
{
    return NativeTransfer(stack, true);
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer(
    CLR_RT_StackFrame &stack,
    bool data16Bits)
{
    NANOCLR_HEADER();
    {
        unsigned char *writeData = NULL;
        unsigned char *readData = NULL;
        int writeSize = 0;
        int readSize = 0;
        SPI_WRITE_READ_SETTINGS rws;

        bool isLongRunningOperation;
        uint32_t estimatedDurationMiliseconds;
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT64 *timeout;
        bool eventResult = true;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get device handle saved on open
        uint32_t deviceId =
            pThis[Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::FIELD___deviceId].NumericByRef().u4;

        if (stack.m_customState == 0)
        {
            // get a pointer to the managed spi connectionSettings object instance
            CLR_RT_HeapBlock *pConfig = pThis[FIELD___connectionSettings].Dereference();

            // get data bit length
            int databitLength = pConfig[SpiConnectionSettings::FIELD___databitLength].NumericByRef().s4;
            if (databitLength <= 0)
                databitLength = 8;
            if (data16Bits)
                databitLength = 16;

            // dereference the write and read buffers from the arguments
            CLR_RT_HeapBlock_Array *writeBuffer = stack.Arg1().DereferenceArray();
            if (writeBuffer != NULL)
            {
                // grab the pointer to the array by getting the first element of the array
                if (data16Bits)
                    writeData = (unsigned char *)writeBuffer->GetFirstElementUInt16();
                else
                    writeData = writeBuffer->GetFirstElement();

                // get the size of the buffer by reading the number of elements in the HeapBlock array
                writeSize = writeBuffer->m_numOfElements;
            }

            CLR_RT_HeapBlock_Array *readBuffer = stack.Arg2().DereferenceArray();
            if (readBuffer != NULL)
            {
                // grab the pointer to the array by getting the first element of the array
                if (data16Bits)
                    readData = (unsigned char *)readBuffer->GetFirstElementUInt16();
                else
                    readData = readBuffer->GetFirstElement();

                // get the size of the buffer by reading the number of elements in the HeapBlock array
                readSize = readBuffer->m_numOfElements;
            }

            // Are we using SPI full-duplex for transfer ?
            bool fullDuplex = (bool)stack.Arg3().NumericByRef().u1;

            // Only tranfer in 16 bit if Buffer and request are for 16 bit
            bool dataTransfer16 =
                (pConfig[SpiConnectionSettings::FIELD___databitLength].NumericByRef().s4 == 16) && data16Bits;

            // Set up read/write settings for SPI_Write_Read call
            rws = {fullDuplex, 0, dataTransfer16, 0};

            // Check to see if we should run async so as not to hold up other tasks
            isLongRunningOperation = IsLongRunningOperation(
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

                // protect the buffers from GC so DMA can find them where they are supposed to be
                if (writeBuffer != NULL)
                    CLR_RT_ProtectFromGC gcWriteBuffer(*writeBuffer);
                if (readBuffer != NULL)
                    CLR_RT_ProtectFromGC gcReadBuffer(*readBuffer);

                // Set callback for async calls to nano spi
                rws.callback = nano_spi_callback;
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
                NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents(
                    stack.m_owningThread,
                    *timeout,
                    CLR_RT_ExecutionEngine::c_Event_SpiMaster,
                    eventResult));

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
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeOpenDevice___I4(CLR_RT_StackFrame &stack)
{
    {
        NANOCLR_HEADER();
        {
            int32_t controllerID;
            SPI_DEVICE_CONFIGURATION spiConfig;

            // get a pointer to the managed object instance and check that it's not NULL
            CLR_RT_HeapBlock *pThis = stack.This();
            FAULT_ON_NULL(pThis);

            // Get reference to manage code SPI controller
            CLR_RT_HeapBlock *controller =
                pThis[Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::FIELD___spiController].Dereference();

            controllerID =
                controller[Library_win_dev_spi_native_Windows_Devices_Spi_SpiController::FIELD___controllerId]
                    .NumericByRef()
                    .s4;
            // Get reference to manage code SPI settings
            CLR_RT_HeapBlock *config =
                pThis[Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::FIELD___connectionSettings]
                    .Dereference();

            // bus zero based
            spiConfig.BusMode = SpiBusMode_master;
            spiConfig.Spi_Bus = controllerID;
            spiConfig.DeviceChipSelect = config[SpiConnectionSettings::FIELD___csLine].NumericByRef().s4;
            spiConfig.ChipSelectActive = false; // TODO - is this something we would like to expose to managed code
            spiConfig.Spi_Mode = (SpiMode)config[SpiConnectionSettings::FIELD___spiMode].NumericByRef().s4;
            spiConfig.DataOrder16 = (DataBitOrder)config[SpiConnectionSettings::FIELD___bitOrder].NumericByRef().s4;
            spiConfig.Clock_RateHz = config[SpiConnectionSettings::FIELD___clockFrequency].NumericByRef().s4;

            // Returns handle to device
            uint32_t handle;

            hr = nanoSPI_OpenDevice(spiConfig, handle);
            NANOCLR_CHECK_HRESULT(hr);

            // Return device handle
            stack.SetResult_I4(handle);
        }
        NANOCLR_NOCLEANUP();
    }
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        (void)stack;
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();

        // get device handle
        int32_t deviceId =
            pThis[Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::FIELD___deviceId].NumericByRef().s4;

        nanoSPI_CloseDevice(deviceId);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
