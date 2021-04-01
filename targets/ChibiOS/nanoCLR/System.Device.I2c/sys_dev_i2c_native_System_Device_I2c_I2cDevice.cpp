//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_native_target.h"

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

/////////////////////////////////////////////////////
// I2C PAL strucs declared in win_dev_i2c_native.h //
/////////////////////////////////////////////////////
#if (STM32_I2C_USE_I2C1 == TRUE)
uint8_t I2C1_DeviceCounter;
#endif
#if defined(STM32_I2C_USE_I2C2) && (STM32_I2C_USE_I2C2 == TRUE)
uint8_t I2C2_DeviceCounter;
#endif
#if defined(STM32_I2C_USE_I2C3) && (STM32_I2C_USE_I2C3 == TRUE)
uint8_t I2C3_DeviceCounter;
#endif
#if defined(STM32_I2C_USE_I2C4) && (STM32_I2C_USE_I2C4 == TRUE)
uint8_t I2C4_DeviceCounter;
#endif

// need to declare these as external
// TODO move them here after Windows.Devices.I2c is removed
extern void GetI2cConfig(CLR_RT_HeapBlock *managedConfig, I2CConfig *llConfig);
extern bool IsLongRunningOperation(
    uint16_t writeSize,
    uint16_t readSize,
    float byteTime,
    uint32_t &estimatedDurationMiliseconds);

// ChibiOS I2C working thread
static THD_FUNCTION(I2CWorkingThread, arg)
{
    NF_PAL_I2C *palI2c = (NF_PAL_I2C *)arg;
    msg_t result;
    int estimatedDurationMiliseconds = palI2c->ByteTime * (palI2c->WriteSize + palI2c->ReadSize + 1);

    if (palI2c->ReadSize != 0 && palI2c->WriteSize != 0)
    {
        // this is a Write/Read transaction
        result = i2cMasterTransmitTimeout(
            palI2c->Driver,
            palI2c->Address,
            palI2c->WriteBuffer,
            palI2c->WriteSize,
            palI2c->ReadBuffer,
            palI2c->ReadSize,
            TIME_MS2I(estimatedDurationMiliseconds));
    }
    else
    {
        if (palI2c->ReadSize == 0)
        {
            // this is Write only transaction

            estimatedDurationMiliseconds = palI2c->ByteTime * (palI2c->WriteSize + 1);

            result = i2cMasterTransmitTimeout(
                palI2c->Driver,
                palI2c->Address,
                palI2c->WriteBuffer,
                palI2c->WriteSize,
                NULL,
                0,
                TIME_MS2I(estimatedDurationMiliseconds));
        }
        else
        {
            // this is a Read only transaction

            estimatedDurationMiliseconds = palI2c->ByteTime * (palI2c->ReadSize + 1);

            result = i2cMasterReceiveTimeout(
                palI2c->Driver,
                palI2c->Address,
                palI2c->ReadBuffer,
                palI2c->ReadSize,
                TIME_MS2I(estimatedDurationMiliseconds));
        }
    }

    // fire event for I2C transaction complete
    Events_Set(SYSTEM_EVENT_FLAG_I2C_MASTER);

    chThdExit(result);
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_I2C *palI2c = NULL;
    CLR_RT_HeapBlock *connectionSettings;
    uint8_t busIndex;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a pointer to the managed I2C connectionSettings object instance
    connectionSettings = pThis[FIELD___connectionSettings].Dereference();

    // get bus index
    busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

    // config GPIO pins used by the I2C peripheral
    // init the PAL struct for this I2C bus and assign the respective driver
    // all this occurs if not already done
    // why do we need to check if this is already done? because several I2cDevice objects can be created associated to
    // the same bus just using different addresses
    switch (busIndex)
    {
#if (STM32_I2C_USE_I2C1 == TRUE)
        case 1:
            if (I2C1_PAL.Driver == NULL)
            {
                ConfigPins_I2C1();

                I2C1_PAL.Driver = &I2CD1;
                palI2c = &I2C1_PAL;

                // increase device counter
                I2C1_DeviceCounter++;
            }
            break;
#endif
#if defined(STM32_I2C_USE_I2C2) && (STM32_I2C_USE_I2C2 == TRUE)
        case 2:
            if (I2C2_PAL.Driver == NULL)
            {
                ConfigPins_I2C2();

                I2C2_PAL.Driver = &I2CD2;
                palI2c = &I2C2_PAL;

                // increase device counter
                I2C2_DeviceCounter++;
            }
            break;
#endif
#if defined(STM32_I2C_USE_I2C3) && (STM32_I2C_USE_I2C3 == TRUE)
        case 3:
            if (I2C3_PAL.Driver == NULL)
            {
                ConfigPins_I2C3();

                I2C3_PAL.Driver = &I2CD3;
                palI2c = &I2C3_PAL;

                // increase device counter
                I2C3_DeviceCounter++;
            }
            break;
#endif
#if defined(STM32_I2C_USE_I2C4) && (STM32_I2C_USE_I2C4 == TRUE)
        case 4:
            if (I2C4_PAL.Driver == NULL)
            {
                ConfigPins_I2C4();

                I2C4_PAL.Driver = &I2CD4;
                palI2c = &I2C4_PAL;

                // increase device counter
                I2C4_DeviceCounter++;
            }
            break;
#endif

        default:
            // this I2C bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    // Get a general low-level I2C configuration, depending on user's managed parameters
    GetI2cConfig(connectionSettings, &palI2c->Configuration);

    // compute rough estimate on the time to tx/rx a byte (in milliseconds)
    if ((I2cBusSpeed)connectionSettings[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4 ==
        I2cBusSpeed_StandardMode)
    {
        // 100kbit/s: this is roughly 0.10ms per byte, give or take
        palI2c->ByteTime = 0.1;
    }
    else
    {
        // 400kbit/s: this is roughly 0.02ms per byte, give or take
        palI2c->ByteTime = 0.02;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t busIndex;

    CLR_RT_HeapBlock *connectionSettings;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a pointer to the managed I2C connectionSettings object instance
    connectionSettings = pThis[FIELD___connectionSettings].Dereference();

    // get bus index
    busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

    // get the driver for the I2C bus
    switch (busIndex)
    {
#if (STM32_I2C_USE_I2C1 == TRUE)
        case 1:
            // remove device
            I2C1_DeviceCounter--;

            if (I2C1_DeviceCounter == 0)
            {
                // no more devices on the bus
                // deactivates the I2C peripheral
                i2cStop(&I2CD1);
                // nulls driver
                I2C1_PAL.Driver = NULL;
            }
            break;
#endif

#if defined(STM32_I2C_USE_I2C2) && (STM32_I2C_USE_I2C2 == TRUE)
        case 2:
            // remove device
            I2C2_DeviceCounter--;

            if (I2C2_DeviceCounter == 0)
            {
                // no more devices on the bus
                // deactivates the I2C peripheral
                i2cStop(&I2CD2);
                // nulls driver
                I2C2_PAL.Driver = NULL;
            }

            break;
#endif

#if defined(STM32_I2C_USE_I2C3) && (STM32_I2C_USE_I2C3 == TRUE)
        case 3:
            // remove device
            I2C3_DeviceCounter--;

            if (I2C3_DeviceCounter == 0)
            {
                // no more devices on the bus
                // deactivates the I2C peripheral
                i2cStop(&I2CD3);
                // nulls driver
                I2C3_PAL.Driver = NULL;
            }
            break;
#endif

#if defined(STM32_I2C_USE_I2C4) && (STM32_I2C_USE_I2C4 == TRUE)
        case 4:
            // remove device
            I2C4_DeviceCounter--;

            if (I2C4_DeviceCounter == 0)
            {
                // no more devices on the bus
                // deactivates the I2C peripheral
                i2cStop(&I2CD4);
                // nulls driver
                I2C4_PAL.Driver = NULL;
            }
            break;
#endif

        default:
            // the requested I2C bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::
    NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint8_t busIndex;
        NF_PAL_I2C *palI2c = NULL;
        bool isLongRunningOperation = false;
        msg_t transactionResult = MSG_OK;

        CLR_RT_HeapBlock hbTimeout;
        CLR_INT64 *timeout;
        bool eventResult = true;
        uint32_t estimatedDurationMiliseconds;

        CLR_RT_HeapBlock *result;
        CLR_RT_HeapBlock *writeSpanByte;
        CLR_RT_HeapBlock *readSpanByte;
        CLR_RT_HeapBlock_Array *writeBuffer = NULL;
        CLR_RT_HeapBlock_Array *readBuffer = NULL;
        int readOffset = 0;
        int writeOffset = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get pointer to connection settings field
        CLR_RT_HeapBlock *connectionSettings = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        // get the driver for the I2C bus
        switch (busIndex)
        {
#if (STM32_I2C_USE_I2C1 == TRUE)
            case 1:
                palI2c = &I2C1_PAL;
                break;
#endif
#if defined(STM32_I2C_USE_I2C2) && (STM32_I2C_USE_I2C2 == TRUE)
            case 2:
                palI2c = &I2C2_PAL;
                break;
#endif
#if defined(STM32_I2C_USE_I2C3) && (STM32_I2C_USE_I2C3 == TRUE)
            case 3:
                palI2c = &I2C3_PAL;
                break;
#endif
#if defined(STM32_I2C_USE_I2C4) && (STM32_I2C_USE_I2C4 == TRUE)
            case 4:
                palI2c = &I2C4_PAL;
                break;
#endif
            default:
                // the requested I2C bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // dereference the write and read SpanByte from the arguments
        writeSpanByte = stack.Arg1().Dereference();
        if (writeSpanByte != NULL)
        {
            // get buffer
            writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
            if (writeBuffer != NULL)
            {
                // Get the write offset, only the elements defined by the span must be written, not the whole array
                writeOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // use the span length as write size, only the elements defined by the span must be written
                palI2c->WriteSize = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            }
        }

        if (writeBuffer == NULL)
        {
            // nothing to write, have to zero this
            palI2c->WriteSize = 0;
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
                palI2c->ReadSize = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            }
        }

        if (readBuffer == NULL)
        {
            // nothing to read, have to zero this
            palI2c->ReadSize = 0;
        }

        // check if this is a long running operation
        isLongRunningOperation = IsLongRunningOperation(
            palI2c->WriteSize,
            palI2c->ReadSize,
            palI2c->ByteTime,
            (uint32_t &)estimatedDurationMiliseconds);

        if (isLongRunningOperation)
        {
            // if this is a long running operation, set a timeout equal to the estimated transaction duration in
            // milliseconds this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() below
            hbTimeout.SetInteger((CLR_INT64)estimatedDurationMiliseconds * TIME_CONVERSION__TO_MILLISECONDS);

            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

            // protect the buffers from GC so DMA can find them where they are supposed to be
            CLR_RT_ProtectFromGC gcWriteBuffer(*writeBuffer);
            CLR_RT_ProtectFromGC gcReadBuffer(*readBuffer);
        }

        // this is going to be used to check for the right event in case of simultaneous I2C transaction
        if (!isLongRunningOperation || stack.m_customState == 1)
        {
            // get slave address from connection settings field
            palI2c->Address =
                (i2caddr_t)connectionSettings[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;

            // when using I2Cv1 driver the address needs to be loaded in the I2C driver struct
#if defined(STM32F1XX) || defined(STM32F4XX) || defined(STM32L1XX)
            palI2c->Driver->addr = palI2c->Address;
#endif

            if (writeBuffer != NULL)
            {
                // grab the pointer to the array by starting and the offset specified in the span
                palI2c->WriteBuffer = (uint8_t *)writeBuffer->GetElement(writeOffset);

                // flush DMA buffer to ensure cache coherency
                // (only required for Cortex-M7)
                cacheBufferFlush(palI2c->WriteBuffer, palI2c->WriteSize);
            }

            if (readBuffer != NULL)
            {
                // grab the pointer to the array by starting and the offset specified in the span
                palI2c->ReadBuffer = (uint8_t *)readBuffer->GetElement(readOffset);
            }

            // because the bus access is shared, acquire the appropriate bus
            i2cAcquireBus(palI2c->Driver);
            i2cStart(palI2c->Driver, &palI2c->Configuration);
        }

        if (isLongRunningOperation)
        {
            // this is a long running operation and hasn't started yet
            // perform I2C transaction using driver's ASYNC API which is launching a thread to perform it
            if (stack.m_customState == 1)
            {
                // spawn working thread to perform the I2C transaction
                palI2c->WorkingThread = chThdCreateFromHeap(
                    NULL,
                    THD_WORKING_AREA_SIZE(256),
                    "I2CWT",
                    NORMALPRIO,
                    I2CWorkingThread,
                    palI2c);

                if (palI2c->WorkingThread == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
                }

                // bump custom state
                stack.m_customState = 2;
            }
        }
        else
        {
            // this is NOT a long running operation
            // perform I2C transaction using driver's SYNC API

            if (palI2c->ReadSize != 0 && palI2c->WriteSize != 0)
            {
                // this is a Write/Read transaction
                transactionResult = i2cMasterTransmitTimeout(
                    palI2c->Driver,
                    palI2c->Address,
                    palI2c->WriteBuffer,
                    palI2c->WriteSize,
                    palI2c->ReadBuffer,
                    palI2c->ReadSize,
                    TIME_MS2I(20));
            }
            else
            {
                if (palI2c->ReadSize == 0)
                {
                    // this is Write only transaction
                    transactionResult = i2cMasterTransmitTimeout(
                        palI2c->Driver,
                        palI2c->Address,
                        palI2c->WriteBuffer,
                        palI2c->WriteSize,
                        NULL,
                        0,
                        TIME_MS2I(20));
                }
                else
                {
                    // this is a Read only transaction
                    transactionResult = i2cMasterReceiveTimeout(
                        palI2c->Driver,
                        palI2c->Address,
                        palI2c->ReadBuffer,
                        palI2c->ReadSize,
                        TIME_MS2I(20));
                }
            }
        }

        while (eventResult)
        {
            if (!isLongRunningOperation)
            {
                // this is not a long running operation so nothing to do here
                break;
            }

            if (palI2c->WorkingThread->state == CH_STATE_FINAL)
            {
                // I2C working thread is now complete
                break;
            }

            // non-blocking wait allowing other threads to run while we wait for the I2C transaction to complete
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_I2cMaster, eventResult));
        }

        if (isLongRunningOperation)
        {
            // pop timeout heap block from stack
            stack.PopValue();
        }

        if (eventResult || !isLongRunningOperation)
        {
            // event occurred
            // OR this is NOT a long running operation

            i2cReleaseBus(palI2c->Driver);

            // create the return object (I2cTransferResult)
            // only at this point we are sure that there will be a return from this thread so it's OK to use the
            // managed stack
            CLR_RT_HeapBlock &top = stack.PushValueAndClear();
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));
            result = top.Dereference();
            FAULT_ON_NULL(result);

            if (isLongRunningOperation)
            {
                // ChibiOS requirement: need to call chThdWait for I2C working thread in order to have it's memory
                // released to the heap, otherwise it won't be returned
                transactionResult = chThdWait(palI2c->WorkingThread);
            }

            // get the result from the working thread execution
            if (transactionResult != MSG_OK)
            {
                // error in transaction
                int errors = i2cGetErrors(palI2c->Driver);

                // figure out what was the error and set the status field
                switch (errors)
                {
                    case I2C_ACK_FAILURE:
                        result[I2cTransferResult::FIELD___status].SetInteger(
                            (CLR_UINT32)I2cTransferStatus_SlaveAddressNotAcknowledged);
                        break;

                    case I2C_TIMEOUT:
                        result[I2cTransferResult::FIELD___status].SetInteger(
                            (CLR_UINT32)I2cTransferStatus_ClockStretchTimeout);
                        break;

                    default:
                        result[I2cTransferResult::FIELD___status].SetInteger(
                            (CLR_UINT32)I2cTransferStatus_UnknownError);
                }

                // set the bytes transferred count to 0 because we don't have a way to know how many bytes were
                // actually sent/received
                result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(0);
            }
            else
            {
                // successfull transaction
                // set the result field
                result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_FullTransfer);

                // set the bytes transferred field
                result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(
                    (CLR_UINT32)(palI2c->WriteSize + palI2c->ReadSize));
            }

            if (palI2c->ReadSize > 0)
            {
                // invalidate cache over read buffer to ensure that content from DMA is read
                // (only required for Cortex-M7)
                cacheBufferInvalidate(palI2c->ReadBuffer, palI2c->ReadSize);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
