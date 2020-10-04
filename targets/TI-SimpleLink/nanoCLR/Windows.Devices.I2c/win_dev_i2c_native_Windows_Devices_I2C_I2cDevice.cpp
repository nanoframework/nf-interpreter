//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include "win_dev_i2c_native_target.h"

typedef Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings I2cConnectionSettings;

/////////////////////////////////////////////////////
// I2C PAL strucs declared in win_dev_i2c_native.h //
/////////////////////////////////////////////////////
NF_PAL_I2C I2C1_PAL;

void HostI2C_CallbackFxn(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus)
{
    NATIVE_INTERRUPT_START

    // fire event for I2C transaction complete
    Events_Set(SYSTEM_EVENT_FLAG_I2C_MASTER);

    NATIVE_INTERRUPT_END
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NF_PAL_I2C *palI2c = NULL;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a pointer to the managed I2C connectionSettings object instance
        CLR_RT_HeapBlock *pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the I2cDevice() constructor in managed code for details
        uint8_t busIndex = (uint8_t)(pThis[FIELD___deviceId].NumericByRef().s4 / 1000);

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
                break;

            default:
                // this I2C bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // Create I2C for usage
        I2C_Params_init(&palI2c->i2cParams);
        palI2c->i2cParams.bitRate =
            (I2cBusSpeed)pConfig[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4 == I2cBusSpeed_StandardMode
                ? I2C_100kHz
                : I2C_400kHz;
        palI2c->i2cParams.transferMode = I2C_MODE_CALLBACK;
        palI2c->i2cParams.transferCallbackFxn = HostI2C_CallbackFxn;
        palI2c->i2c = I2C_open(Board_I2C0, &palI2c->i2cParams);
        FAULT_ON_NULL(palI2c->i2c);
        palI2c->i2cTransaction.slaveAddress =
            (I2cBusSpeed)pConfig[I2cConnectionSettings::FIELD___slaveAddress].NumericByRef().s4;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeDispose___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t busIndex;
    bool disposeController = false;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get disposeController
    disposeController = (bool)stack.Arg0().NumericByRef().u1;

    if (disposeController)
    {
        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the I2cDevice() constructor in managed code for details
        busIndex = (uint8_t)(pThis[FIELD___deviceId].NumericByRef().s4 / 1000);

        // get the driver for the I2C bus
        switch (busIndex)
        {
            case 1:
                // deactivates the I2C peripheral
                I2C_close(I2C1_PAL.i2c);
                I2C1_PAL.i2c == NULL;
                break;

            default:
                // this I2C bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::
    NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint8_t busIndex;
        NF_PAL_I2C *palI2c = NULL;

        CLR_RT_HeapBlock hbTimeout;
        CLR_INT64 *timeout;
        bool eventResult = true;
        uint32_t estimatedDurationMiliseconds;

        CLR_RT_HeapBlock_Array *writeBuffer;
        CLR_RT_HeapBlock_Array *readBuffer;
        CLR_RT_HeapBlock *result;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // // get pointer to connection settings field
        // CLR_RT_HeapBlock* connectionSettings = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the I2cDevice() constructor in managed code for details
        busIndex = (uint8_t)(pThis[FIELD___deviceId].NumericByRef().s4 / 1000);

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

        // dereference the write and read buffers from the arguments
        writeBuffer = stack.Arg1().DereferenceArray();
        if (writeBuffer != NULL)
        {
            // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
            palI2c->i2cTransaction.writeCount = writeBuffer->m_numOfElements;
        }
        else
        {
            // nothing to write, have to zero this
            palI2c->i2cTransaction.writeCount = 0;
        }

        readBuffer = stack.Arg2().DereferenceArray();
        if (readBuffer != NULL)
        {
            // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
            palI2c->i2cTransaction.readCount = readBuffer->m_numOfElements;
        }
        else
        {
            // nothing to read, have to zero this
            palI2c->i2cTransaction.readCount = 0;
        }

        // set a timeout to an infinite timeout
        // the catch is that the working thread MUST ALWAYS return at some point
        // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
        hbTimeout.SetInteger((CLR_INT64)-1);

        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

        // protect the buffers from GC so DMA can find them where they are supposed to be
        CLR_RT_ProtectFromGC gcWriteBuffer(*writeBuffer);
        CLR_RT_ProtectFromGC gcReadBuffer(*readBuffer);

        if (writeBuffer != NULL)
        {
            palI2c->i2cTransaction.writeBuf = (uint8_t *)writeBuffer->GetFirstElement();
        }

        if (readBuffer != NULL)
        {
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

                // successfull transaction
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
    }
    NANOCLR_NOCLEANUP();
}
