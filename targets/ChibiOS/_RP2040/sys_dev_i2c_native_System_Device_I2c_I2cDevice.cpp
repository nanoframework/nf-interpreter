//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_native_target.h"

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

////////////////////////////////////////////
// declaration of the I2C PAL structs     //
////////////////////////////////////////////
#if RP_I2C_USE_I2C0
NF_PAL_I2C I2C0_PAL;
uint8_t I2C0_DeviceCounter;
#endif
#if RP_I2C_USE_I2C1
NF_PAL_I2C I2C1_PAL;
uint8_t I2C1_DeviceCounter;
#endif

void GetI2cConfig(CLR_RT_HeapBlock *managedConfig, I2CConfig *llConfig)
{
    I2cBusSpeed busSpeed = (I2cBusSpeed)managedConfig[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4;

    // RP2040 I2CConfig only has baudrate field
    llConfig->baudrate = busSpeed == I2cBusSpeed_StandardMode ? 100000U : 400000U;
}

// estimate the time required to perform the I2C transaction
bool IsLongRunningOperation(
    uint16_t writeSize,
    uint16_t readSize,
    float byteTime,
    uint32_t &estimatedDurationMiliseconds)
{
    estimatedDurationMiliseconds = byteTime * (writeSize + readSize + 1);

    if (estimatedDurationMiliseconds > CLR_RT_Thread::c_TimeQuantum_Milliseconds)
    {
        return true;
    }

    return false;
}

// ChibiOS I2C working thread
static THD_FUNCTION(I2CWorkingThread, arg)
{
    NF_PAL_I2C *palI2c = (NF_PAL_I2C *)arg;
    msg_t result;
    int estimatedDurationMiliseconds = palI2c->ByteTime * (palI2c->WriteSize + palI2c->ReadSize + 1);

    if (palI2c->ReadSize != 0 && palI2c->WriteSize != 0)
    {
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
            estimatedDurationMiliseconds = palI2c->ByteTime * (palI2c->ReadSize + 1);

            result = i2cMasterReceiveTimeout(
                palI2c->Driver,
                palI2c->Address,
                palI2c->ReadBuffer,
                palI2c->ReadSize,
                TIME_MS2I(estimatedDurationMiliseconds));
        }
    }

    Events_Set(SYSTEM_EVENT_FLAG_I2C_MASTER);

    chThdExit(result);
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_I2C *palI2c = NULL;
    CLR_RT_HeapBlock *connectionSettings;
    uint8_t busIndex;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    connectionSettings = pThis[FIELD___connectionSettings].Dereference();

    // get bus index (0-based for RP2040)
    busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

    switch (busIndex)
    {
#if RP_I2C_USE_I2C0
        case 0:
            if (I2C0_PAL.Driver == NULL)
            {
                ConfigPins_I2C0();
                I2C0_PAL.Driver = &I2CD0;
                palI2c = &I2C0_PAL;
                I2C0_DeviceCounter++;
            }
            break;
#endif
#if RP_I2C_USE_I2C1
        case 1:
            if (I2C1_PAL.Driver == NULL)
            {
                ConfigPins_I2C1();
                I2C1_PAL.Driver = &I2CD1;
                palI2c = &I2C1_PAL;
                I2C1_DeviceCounter++;
            }
            break;
#endif

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    GetI2cConfig(connectionSettings, &palI2c->Configuration);

    if ((I2cBusSpeed)connectionSettings[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4 ==
        I2cBusSpeed_StandardMode)
    {
        palI2c->ByteTime = 0.1;
    }
    else
    {
        palI2c->ByteTime = 0.02;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t busIndex;
    CLR_RT_HeapBlock *connectionSettings;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    connectionSettings = pThis[FIELD___connectionSettings].Dereference();

    busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

    switch (busIndex)
    {
#if RP_I2C_USE_I2C0
        case 0:
            I2C0_DeviceCounter--;
            if (I2C0_DeviceCounter == 0)
            {
                i2cStop(&I2CD0);
                I2C0_PAL.Driver = NULL;
            }
            break;
#endif
#if RP_I2C_USE_I2C1
        case 1:
            I2C1_DeviceCounter--;
            if (I2C1_DeviceCounter == 0)
            {
                i2cStop(&I2CD1);
                I2C1_PAL.Driver = NULL;
            }
            break;
#endif

        default:
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
    bool isLongRunningOperation = false;
    msg_t transactionResult = MSG_OK;

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
    int readOffset = 0;
    int writeOffset = 0;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    connectionSettings = pThis[FIELD___connectionSettings].Dereference();

    busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

    switch (busIndex)
    {
#if RP_I2C_USE_I2C0
        case 0:
            palI2c = &I2C0_PAL;
            break;
#endif
#if RP_I2C_USE_I2C1
        case 1:
            palI2c = &I2C1_PAL;
            break;
#endif
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    writeSpanByte = stack.Arg1().Dereference();
    if (writeSpanByte != NULL)
    {
        writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
        if (writeBuffer != NULL)
        {
            writeOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
            palI2c->WriteSize = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            writeBuffer->Pin();
        }
    }

    if (writeBuffer == NULL)
    {
        palI2c->WriteSize = 0;
    }

    readSpanByte = stack.Arg2().Dereference();
    if (readSpanByte != NULL)
    {
        readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
        if (readBuffer != NULL)
        {
            readOffset = readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
            palI2c->ReadSize = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            readBuffer->Pin();
        }
    }

    if (readBuffer == NULL)
    {
        palI2c->ReadSize = 0;
    }

    isLongRunningOperation = IsLongRunningOperation(
        palI2c->WriteSize,
        palI2c->ReadSize,
        palI2c->ByteTime,
        (uint32_t &)estimatedDurationMiliseconds);

    if (isLongRunningOperation)
    {
        hbTimeout.SetInteger((CLR_INT64)estimatedDurationMiliseconds * TIME_CONVERSION__TO_MILLISECONDS);
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));
    }

    if (!isLongRunningOperation || stack.m_customState == 1)
    {
        palI2c->Address = (i2caddr_t)connectionSettings[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;

        if (writeBuffer != NULL)
        {
            palI2c->WriteBuffer = (uint8_t *)writeBuffer->GetElement(writeOffset);
        }

        if (readBuffer != NULL)
        {
            palI2c->ReadBuffer = (uint8_t *)readBuffer->GetElement(readOffset);
        }

        i2cAcquireBus(palI2c->Driver);
        i2cStart(palI2c->Driver, &palI2c->Configuration);
    }

    if (isLongRunningOperation)
    {
        if (stack.m_customState == 1)
        {
            palI2c->WorkingThread =
                chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(256), "I2CWT", NORMALPRIO, I2CWorkingThread, palI2c);

            if (palI2c->WorkingThread == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            }

            stack.m_customState = 2;
        }
    }
    else
    {
        if (palI2c->ReadSize != 0 && palI2c->WriteSize != 0)
        {
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
            break;
        }

        if (palI2c->WorkingThread->state == CH_STATE_FINAL)
        {
            break;
        }

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_I2cMaster, eventResult));
    }

    if (isLongRunningOperation)
    {
        stack.PopValue();
    }

    if (eventResult || !isLongRunningOperation)
    {
        i2cReleaseBus(palI2c->Driver);

        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));
        result = top.Dereference();
        FAULT_ON_NULL(result);

        if (isLongRunningOperation)
        {
            transactionResult = chThdWait(palI2c->WorkingThread);
        }

        if (transactionResult != MSG_OK)
        {
            int errors = i2cGetErrors(palI2c->Driver);

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
                    result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_UnknownError);
            }

            result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(0);
        }
        else
        {
            result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_FullTransfer);
            result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(
                (CLR_UINT32)(palI2c->WriteSize + palI2c->ReadSize));
        }
    }

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING)
    {
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
