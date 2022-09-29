//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <zephyr.h>
//#include <hal.h>
#include <targetPAL.h>
#include <nanoHAL.h>
#include "win_dev_i2c_native_target.h"

typedef Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings I2cConnectionSettings;

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    // NANOCLR_HEADER();

    // NANOCLR_NOCLEANUP();
return 0;
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeDispose___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    // NANOCLR_HEADER();

    // NANOCLR_NOCLEANUP();
    return 0;
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::
    NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
//    NANOCLR_HEADER();
//     {
//         uint8_t busIndex;
//         NF_PAL_I2C *palI2c = NULL;
//         bool isLongRunningOperation = false;
//         msg_t transactionResult = MSG_OK;

//         CLR_RT_HeapBlock hbTimeout;
//         CLR_INT64 *timeout;
//         bool eventResult = true;
//         uint32_t estimatedDurationMiliseconds;

//         CLR_RT_HeapBlock_Array *writeBuffer;
//         CLR_RT_HeapBlock_Array *readBuffer;
//         CLR_RT_HeapBlock *result;

//         // get a pointer to the managed object instance and check that it's not NULL
//         CLR_RT_HeapBlock *pThis = stack.This();
//         FAULT_ON_NULL(pThis);

//         // get pointer to connection settings field
//         CLR_RT_HeapBlock *connectionSettings = pThis[FIELD___connectionSettings].Dereference();

//         // get bus index
//         // this is coded with a multiplication, need to perform and int division to get the number
//         // see the comments in the I2cDevice() constructor in managed code for details
//         busIndex = (uint8_t)(pThis[FIELD___deviceId].NumericByRef().s4 / 1000);

//         // get the driver for the I2C bus
//         switch (busIndex)
//         {
// #if (STM32_I2C_USE_I2C1 == TRUE)
//             case 1:
//                 palI2c = &I2C1_PAL;
//                 break;
// #endif
// #if defined(STM32_I2C_USE_I2C2) && (STM32_I2C_USE_I2C2 == TRUE)
//             case 2:
//                 palI2c = &I2C2_PAL;
//                 break;
// #endif
// #if defined(STM32_I2C_USE_I2C3) && (STM32_I2C_USE_I2C3 == TRUE)
//             case 3:
//                 palI2c = &I2C3_PAL;
//                 break;
// #endif
// #if defined(STM32_I2C_USE_I2C4) && (STM32_I2C_USE_I2C4 == TRUE)
//             case 4:
//                 palI2c = &I2C4_PAL;
//                 break;
// #endif
//             default:
//                 // the requested I2C bus is not valid
//                 NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
//                 break;
//         }

//         // dereference the write and read buffers from the arguments
//         writeBuffer = stack.Arg1().DereferenceArray();
//         if (writeBuffer != NULL)
//         {
//             // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
//             palI2c->WriteSize = writeBuffer->m_numOfElements;
//         }
//         else
//         {
//             // nothing to write, have to zero this
//             palI2c->WriteSize = 0;
//         }

//         readBuffer = stack.Arg2().DereferenceArray();
//         if (readBuffer != NULL)
//         {
//             // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
//             palI2c->ReadSize = readBuffer->m_numOfElements;
//         }
//         else
//         {
//             // nothing to read, have to zero this
//             palI2c->ReadSize = 0;
//         }

//         // check if this is a long running operation
//         isLongRunningOperation = IsLongRunningOperation(
//             palI2c->WriteSize,
//             palI2c->ReadSize,
//             palI2c->ByteTime,
//             (uint32_t &)estimatedDurationMiliseconds);

//         if (isLongRunningOperation)
//         {
//             // if this is a long running operation, set a timeout equal to the estimated transaction duration in
//             // milliseconds this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() below
//             hbTimeout.SetInteger((CLR_INT64)estimatedDurationMiliseconds * TIME_CONVERSION__TO_MILLISECONDS);

//             NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

//             // protect the buffers from GC so DMA can find them where they are supposed to be
//             CLR_RT_ProtectFromGC gcWriteBuffer(*writeBuffer);
//             CLR_RT_ProtectFromGC gcReadBuffer(*readBuffer);
//         }

//         // this is going to be used to check for the right event in case of simultaneous I2C transaction
//         if (!isLongRunningOperation || stack.m_customState == 1)
//         {
//             // get slave address from connection settings field
//             palI2c->Address =
//                 (i2caddr_t)connectionSettings
//                     [Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings::FIELD___slaveAddress]
//                         .NumericByRef()
//                         .s4;

//             // when using I2Cv1 driver the address needs to be loaded in the I2C driver struct
// #if defined(STM32F1XX) || defined(STM32F4XX) || defined(STM32L1XX)
//             palI2c->Driver->addr = palI2c->Address;
// #endif

//             if (writeBuffer != NULL)
//             {
//                 palI2c->WriteBuffer = (uint8_t *)writeBuffer->GetFirstElement();

//                 // flush DMA buffer to ensure cache coherency
//                 // (only required for Cortex-M7)
//                 cacheBufferFlush(palI2c->WriteBuffer, palI2c->WriteSize);
//             }

//             if (readBuffer != NULL)
//             {
//                 palI2c->ReadBuffer = (uint8_t *)readBuffer->GetFirstElement();
//             }

//             // because the bus access is shared, acquire the appropriate bus
//             i2cAcquireBus(palI2c->Driver);
//             i2cStart(palI2c->Driver, &palI2c->Configuration);
//         }

//         if (isLongRunningOperation)
//         {
//             // this is a long running operation and hasn't started yet
//             // perform I2C transaction using driver's ASYNC API which is launching a thread to perform it
//             if (stack.m_customState == 1)
//             {
//                 // spawn working thread to perform the I2C transaction
//                 palI2c->WorkingThread = chThdCreateFromHeap(
//                     NULL,
//                     THD_WORKING_AREA_SIZE(256),
//                     "I2CWT",
//                     NORMALPRIO,
//                     I2CWorkingThread,
//                     palI2c);

//                 if (palI2c->WorkingThread == NULL)
//                 {
//                     NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
//                 }

//                 // bump custom state
//                 stack.m_customState = 2;
//             }
//         }
//         else
//         {
//             // this is NOT a long running operation
//             // perform I2C transaction using driver's SYNC API

//             if (palI2c->ReadSize != 0 && palI2c->WriteSize != 0)
//             {
//                 // this is a Write/Read transaction
//                 transactionResult = i2cMasterTransmitTimeout(
//                     palI2c->Driver,
//                     palI2c->Address,
//                     palI2c->WriteBuffer,
//                     palI2c->WriteSize,
//                     palI2c->ReadBuffer,
//                     palI2c->ReadSize,
//                     TIME_MS2I(20));
//             }
//             else
//             {
//                 if (palI2c->ReadSize == 0)
//                 {
//                     // this is Write only transaction
//                     transactionResult = i2cMasterTransmitTimeout(
//                         palI2c->Driver,
//                         palI2c->Address,
//                         palI2c->WriteBuffer,
//                         palI2c->WriteSize,
//                         NULL,
//                         0,
//                         TIME_MS2I(20));
//                 }
//                 else
//                 {
//                     // this is a Read only transaction
//                     transactionResult = i2cMasterReceiveTimeout(
//                         palI2c->Driver,
//                         palI2c->Address,
//                         palI2c->ReadBuffer,
//                         palI2c->ReadSize,
//                         TIME_MS2I(20));
//                 }
//             }
//         }

//         while (eventResult)
//         {
//             if (!isLongRunningOperation)
//             {
//                 // this is not a long running operation so nothing to do here
//                 break;
//             }

//             if (palI2c->WorkingThread->state == CH_STATE_FINAL)
//             {
//                 // I2C working thread is now complete
//                 break;
//             }

//             // non-blocking wait allowing other threads to run while we wait for the I2C transaction to complete
//             NANOCLR_CHECK_HRESULT(
//                 g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_I2cMaster, eventResult));
//         }

//         if (isLongRunningOperation)
//         {
//             // pop timeout heap block from stack
//             stack.PopValue();
//         }

//         if (eventResult || !isLongRunningOperation)
//         {
//             // event occurred
//             // OR this is NOT a long running operation

//             i2cReleaseBus(palI2c->Driver);

//             // create the return object (I2cTransferResult)
//             // only at this point we are sure that there will be a return from this thread so it's OK to use the managed
//             // stack
//             CLR_RT_HeapBlock &top = stack.PushValueAndClear();
//             NANOCLR_CHECK_HRESULT(
//                 g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult_old));
//             result = top.Dereference();
//             FAULT_ON_NULL(result);

//             if (isLongRunningOperation)
//             {
//                 // ChibiOS requirement: need to call chThdWait for I2C working thread in order to have it's memory
//                 // released to the heap, otherwise it won't be returned
//                 transactionResult = chThdWait(palI2c->WorkingThread);
//             }

//             // get the result from the working thread execution
//             if (transactionResult != MSG_OK)
//             {
//                 // error in transaction
//                 int errors = i2cGetErrors(palI2c->Driver);

//                 // figure out what was the error and set the status field
//                 switch (errors)
//                 {
//                     case I2C_ACK_FAILURE:
//                         result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status]
//                             .SetInteger((CLR_UINT32)I2cTransferStatus_SlaveAddressNotAcknowledged);
//                         break;

//                     case I2C_TIMEOUT:
//                         result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status]
//                             .SetInteger((CLR_UINT32)I2cTransferStatus_ClockStretchTimeout);
//                         break;

//                     default:
//                         result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status]
//                             .SetInteger((CLR_UINT32)I2cTransferStatus_UnknownError);
//                 }

//                 // set the bytes transferred count to 0 because we don't have a way to know how many bytes were actually
//                 // sent/received
//                 result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___bytesTransferred]
//                     .SetInteger(0);
//             }
//             else
//             {
//                 // successfull transaction
//                 // set the result field
//                 result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status].SetInteger(
//                     (CLR_UINT32)I2cTransferStatus_FullTransfer);

//                 // set the bytes transferred field
//                 result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___bytesTransferred]
//                     .SetInteger((CLR_UINT32)(palI2c->WriteSize + palI2c->ReadSize));
//             }

//             if (palI2c->ReadSize > 0)
//             {
//                 // invalidate cache over read buffer to ensure that content from DMA is read
//                 // (only required for Cortex-M7)
//                 cacheBufferInvalidate(palI2c->ReadBuffer, palI2c->ReadSize);
//             }
//         }
//     }
//    NANOCLR_NOCLEANUP();
    return 0;
}
