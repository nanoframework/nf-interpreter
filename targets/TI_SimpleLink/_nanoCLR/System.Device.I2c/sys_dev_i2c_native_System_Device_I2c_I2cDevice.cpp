//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_native_target.h"

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

/////////////////////////////////////////////////////
// I2C PAL structs declared in sys_dev_i2c_native.h //
/////////////////////////////////////////////////////
NF_PAL_I2C I2C1_PAL;

uint8_t I2C1_DeviceCounter = 0;

void HostI2C_CallbackFxn(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus)
{
    NATIVE_INTERRUPT_START

    // fire event for I2C transaction complete
    Events_Set(SYSTEM_EVENT_FLAG_I2C_MASTER);

    NATIVE_INTERRUPT_END
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NF_PAL_I2C *palI2c = NULL;
        I2cBusSpeed busSpeed;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a pointer to the managed I2C connectionSettings object instance
        CLR_RT_HeapBlock *pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        busIndex = (uint8_t)pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        // config GPIO pins used by the I2C peripheral
        // init the PAL struct for this I2C bus and assign the respective driver
        // all this occurs if not already done
        // why do we need to check if this is already done? because several I2cDevice objects can be created associated
        // to the same bus just using different addresses
        switch (busIndex)
        {
            case 1:
                if (I2C1_PAL.i2c == NULL)
                {
                    palI2c = &I2C1_PAL;
                }

                // add device counter
                I2C1_DeviceCounter++;

                break;

            default:
                // this I2C bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // Create I2C for usage
        I2C_Params_init(&palI2c->i2cParams);
        busSpeed = (I2cBusSpeed)pConfig[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4;
        if (busSpeed == I2cBusSpeed::I2cBusSpeed_FastModePlus)
        {
            palI2c->i2cParams.bitRate = I2C_1000kHz;
        }
        else if (busSpeed == I2cBusSpeed::I2cBusSpeed_FastMode)
        {
            palI2c->i2cParams.bitRate = I2C_400kHz;
        }
        else
        {
            // Default mode is standard
            palI2c->i2cParams.bitRate = I2C_100kHz;
        }

        palI2c->i2cParams.transferMode = I2C_MODE_CALLBACK;
        palI2c->i2cParams.transferCallbackFxn = HostI2C_CallbackFxn;
        palI2c->i2c = I2C_open(Board_I2C0, &palI2c->i2cParams);
        FAULT_ON_NULL(palI2c->i2c);
        palI2c->i2cTransaction.slaveAddress =
            (I2cBusSpeed)pConfig[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pConfig;

    uint8_t busIndex;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get bus index
    busIndex = (uint8_t)pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

    // get the driver for the I2C bus
    switch (busIndex)
    {
        case 1:
            // decrease device counter
            I2C1_DeviceCounter--;

            if (I2C1_DeviceCounter == 0)
            {
                // no more devices on the bus
                // deactivates the I2C peripheral
                I2C_close(I2C1_PAL.i2c);
                I2C1_PAL.i2c == NULL;
            }

            break;

        default:
            // this I2C bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::
    NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t busIndex;
    NF_PAL_I2C *palI2c = NULL;

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;
    uint32_t estimatedDurationMiliseconds;

    CLR_RT_HeapBlock *result;
    CLR_RT_HeapBlock *writeSpanByte;
    CLR_RT_HeapBlock *readSpanByte;
    CLR_RT_HeapBlock *connectionSettings;
    CLR_RT_HeapBlock_Array *writeBuffer = NULL;
    CLR_RT_HeapBlock_Array *readBuffer = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get pointer to connection settings field
    connectionSettings = pThis[FIELD___connectionSettings].Dereference();

    // get bus index
    busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

    // get the driver for the I2C bus
    switch (busIndex)
    {
        case 1:
            palI2c = &I2C1_PAL;
            break;

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
            // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
            palI2c->i2cTransaction.writeCount = writeBuffer->m_numOfElements;
        }
    }

    if (writeBuffer == NULL)
    {
        // nothing to write, have to zero this
        palI2c->i2cTransaction.writeCount = 0;
    }

    readSpanByte = stack.Arg2().Dereference();
    if (readSpanByte != NULL)
    {
        // get buffer
        readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
        if (readBuffer != NULL)
        {
            // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
            palI2c->i2cTransaction.readCount = readBuffer->m_numOfElements;
        }
    }

    if (readBuffer == NULL)
    {
        // nothing to read, have to zero this
        palI2c->i2cTransaction.readCount = 0;
    }

    // set a timeout to an infinite timeout
    // the catch is that the working thread MUST ALWAYS return at some point
    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    // pin the buffers so DMA can find them where they are supposed to be
    if (writeBuffer != NULL)
    {
        writeBuffer->Pin();
        palI2c->i2cTransaction.writeBuf = (uint8_t *)writeBuffer->GetFirstElement();
    }

    if (readBuffer != NULL)
    {
        readBuffer->Pin();
        palI2c->i2cTransaction.readBuf = (uint8_t *)readBuffer->GetFirstElement();
    }

    // perform I2C transaction using driver's callback which will set the appropriate event on completion
    if (stack.m_customState == 1)
    {
        I2C_transfer(palI2c->i2c, &palI2c->i2cTransaction);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the I2C transaction to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_I2cMaster, eventResult));

        if (eventResult)
        {
            // event occurred

            // create the return object (I2cTransferResult)
            // only at this point we are sure that there will be a return from this thread so it's OK to use the
            // managed stack
            CLR_RT_HeapBlock &top = stack.PushValueAndClear();
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));
            result = top.Dereference();
            FAULT_ON_NULL(result);

            // successful transaction
            // set the result field
            result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status].SetInteger(
                (CLR_UINT32)I2cTransferStatus_FullTransfer);

            // set the bytes transferred field
            result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___bytesTransferred]
                .SetInteger((CLR_UINT32)(palI2c->i2cTransaction.writeCount + palI2c->i2cTransaction.readCount));

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING)
    {
        // un-pin the buffers
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
