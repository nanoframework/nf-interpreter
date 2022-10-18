//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_native_target.h"

#include <drivers/i2c.h>
#include <zephyr.h>

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

#if (CONFIG_I2C)

bool IsLongRunningOperation(
    uint16_t writeSize,
    uint16_t readSize,
    float byteTime,
    uint32_t &estimatedDurationMiliseconds);
/////////////////////////////////////////////////////
// I2C PAL strucs declared in win_dev_i2c_native.h //
/////////////////////////////////////////////////////
struct i2c_bus
{
    uint8_t index;
    uint8_t cnt;
    uint16_t Address;
    const char *name;
    const struct device *drv;
    float ByteTime;
    uint8_t *WriteBuffer;
    uint8_t *ReadBuffer;
    uint8_t WriteSize;
    uint8_t ReadSize;
};
struct i2c_bus i2c_bus_state[] = {
#if (CONFIG_I2C_0 || CONFIG_I2C_0_NRF_TWIM)
    {0, 0, 0, "I2C_0", NULL, 0, 0, 0, 0, 0},
#endif
#if (CONFIG_I2C_1 || CONFIG_I2C_1_NRF_TWIM)
    {1, 0, 0, "I2C_1", NULL, 0, 0, 0, 0, 0},
#endif
#if (CONFIG_I2C_2 || CONFIG_I2C_2_NRF_TWIM)
    {2, 0, 0, "I2C_2", NULL, 0, 0, 0, 0, 0},
#endif
#if (CONFIG_I2C_3 || CONFIG_I2C_3_NRF_TWIM)
    {3, 0, 0, "I2C_3", NULL, 0, 0, 0, 0, 0},
#endif
};

#define _countof(a) (sizeof(a) / sizeof(a[0]))
struct i2c_bus *findI2cStruct(uint32_t bus)
{
    unsigned int i;
    struct i2c_bus *busStruct = NULL;
    for (i = 0; i < _countof(i2c_bus_state); i++)
    {
        if (i2c_bus_state[i].index == bus)
        {
            busStruct = &i2c_bus_state[i];
            break;
        }
    }
    return busStruct;
}
#else
#warning CONFIG_I2C not defined. I2C .NET libraries not implemented.
#endif
//
// Init selected I2C bus
//
HRESULT
Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
#if (CONFIG_I2C)
    CLR_RT_HeapBlock *connectionSettings;
    I2cBusSpeed busSpeed;
    struct i2c_bus *busI2c;
    uint8_t busIndex;

    printk("Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___"
           "VOID++\n");

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a pointer to the managed I2C connectionSettings object instance
    connectionSettings = pThis[FIELD___connectionSettings].Dereference();

    // get bus index
    busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;
    printk("I2C busIndex %d\n", busIndex);
    // Find the pre-populated struct for this I2C bus
    busI2c = findI2cStruct(busIndex);
    if (busI2c == NULL)
    {
        printk("I2C instance not found.\n");
        // this I2C bus is not valid
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    if (busI2c->drv == NULL)
    {
        busI2c->drv = device_get_binding(busI2c->name);
        if (busI2c->drv == NULL)
        {
            printk("I2C failed to open Zephyr driver.\n");
            // this I2C bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    // Inc the use count.
    busI2c->cnt++;
    // Get i2c bus speed value
    busSpeed = (I2cBusSpeed)connectionSettings[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4;

    // compute rough estimate on the time to tx/rx a byte (in milliseconds)
    if (busSpeed == I2cBusSpeed_StandardMode)
    {
        // 100kbit/s: this is roughly 0.10ms per byte, give or take
        busI2c->ByteTime = 0.1;
    }
    else
    {
        // 400kbit/s: this is roughly 0.02ms per byte, give or take
        busI2c->ByteTime = 0.02;
    }
#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif  // CONFIG_I2C
    NANOCLR_NOCLEANUP();
}

HRESULT
Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
#if (CONFIG_I2C)
    uint8_t busIndex;
    struct i2c_bus *busI2c;
    CLR_RT_HeapBlock *connectionSettings;

    printk("Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::"
           "NativeDispose___VOID++\n");

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a pointer to the managed I2C connectionSettings object instance
    connectionSettings = pThis[FIELD___connectionSettings].Dereference();

    // get bus index
    busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

    // Find the I2C struct for this I2C index
    busI2c = findI2cStruct(busIndex);
    if (busI2c == NULL)
    {
        // this I2C bus is not valid
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    if (busI2c->drv == NULL)
    {
        // Very unexpected as we're disposing
        // this I2C bus is not valid
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    // Dec the use count.
    if (busI2c->cnt)
    {
        busI2c->cnt--;
    }
    // TODO:: Stop the I2C bus
    // TODO:: Put in low power state?
#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif  // CONFIG_I2C
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::
    NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
#if (CONFIG_I2C)
    {
        uint8_t busIndex;
        struct i2c_bus *busI2c;
        bool isLongRunningOperation = false;
        int transactionResult = 0;

        CLR_RT_HeapBlock hbTimeout;
        CLR_INT64 *timeout;
        // bool eventResult = true;
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

        // Find the I2C struct for this I2C index
        busI2c = findI2cStruct(busIndex);
        if (busI2c == NULL)
        {
            // this I2C bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        if (busI2c->drv == NULL)
        {
            // this I2C bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // dereference the write and read SpanByte from the arguments
        writeSpanByte = stack.Arg1().Dereference();
        if (writeSpanByte != NULL)
        {
            // get buffer
            writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
            if (writeBuffer != NULL)
            {
                // Get the write offset, only the elements defined by the span must be
                // written, not the whole array
                writeOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // use the span length as write size, only the elements defined by the
                // span must be written
                busI2c->WriteSize = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            }
        }

        if (writeBuffer == NULL)
        {
            // nothing to write, have to zero this
            busI2c->WriteSize = 0;
        }

        readSpanByte = stack.Arg2().Dereference();
        if (readSpanByte != NULL)
        {
            // get buffer
            readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
            if (readBuffer != NULL)
            {
                // Get the read offset, only the elements defined by the span must be
                // read, not the whole array
                readOffset = readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // use the span length as read size, only the elements defined by the
                // span must be read
                busI2c->ReadSize = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            }
        }

        if (readBuffer == NULL)
        {
            // nothing to read, have to zero this
            busI2c->ReadSize = 0;
        }
// printk ("write %x  %d  read %x %d\n", (uint32_t) writeBuffer,
// busI2c->WriteSize,  (uint32_t)readBuffer, busI2c->ReadSize);
#if 0
        // check if this is a long running operation
        isLongRunningOperation = IsLongRunningOperation(
            busI2c->WriteSize,
            busI2c->ReadSize,
            busI2c->ByteTime,
            (uint32_t &)estimatedDurationMiliseconds);
#else
        isLongRunningOperation = false;
#endif

        if (isLongRunningOperation)
        {
            // if this is a long running operation, set a timeout equal to the
            // estimated transaction duration in milliseconds this value has to be in
            // ticks to be properly loaded by SetupTimeoutFromTicks() below
            hbTimeout.SetInteger((CLR_INT64)estimatedDurationMiliseconds * TIME_CONVERSION__TO_MILLISECONDS);

            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

            // protect the buffers from GC so DMA can find them where they are
            // supposed to be
            CLR_RT_ProtectFromGC gcWriteBuffer(*writeBuffer);
            CLR_RT_ProtectFromGC gcReadBuffer(*readBuffer);
        }

        // this is going to be used to check for the right event in case of
        // simultaneous I2C transaction
        if (!isLongRunningOperation || stack.m_customState == 1)
        {
            // get slave address from connection settings field
            busI2c->Address =
                (uint16_t)connectionSettings[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;

            //             // when using I2Cv1 driver the address needs to be loaded
            //             in the I2C driver struct
            // #if defined(STM32F1XX) || defined(STM32F4XX) || defined(STM32L1XX)
            //             busI2c->Driver->addr = busI2c->Address;
            // #endif

            if (writeBuffer != NULL)
            {
                // grab the pointer to the array by starting and the offset specified in
                // the span
                busI2c->WriteBuffer = (uint8_t *)writeBuffer->GetElement(writeOffset);

                // flush DMA buffer to ensure cache coherency
                // (only required for Cortex-M7)
                //                cacheBufferFlush(busI2c->WriteBuffer,
                //                busI2c->WriteSize);
            }

            if (readBuffer != NULL)
            {
                // grab the pointer to the array by starting and the offset specified in
                // the span
                busI2c->ReadBuffer = (uint8_t *)readBuffer->GetElement(readOffset);
            }

            // because the bus access is shared, acquire the appropriate bus
            // i2cAcquireBus(busI2c->Driver);
            // i2cStart(busI2c->Driver, &busI2c->Configuration);
        }

        if (isLongRunningOperation)
        {
            //             // this is a long running operation and hasn't started yet
            //             // perform I2C transaction using driver's ASYNC API which
            //             is launching a thread to perform it if (stack.m_customState
            //             == 1)
            //             {
            //                 // spawn working thread to perform the I2C transaction
            //                 busI2c->WorkingThread = chThdCreateFromHeap(
            //                     NULL,
            //                     THD_WORKING_AREA_SIZE(256),
            //                     "I2CWT",
            //                     NORMALPRIO,
            //                     I2CWorkingThread,
            //                     busI2c);

            //                 if (busI2c->WorkingThread == NULL)
            //                 {
            //                     NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            //                 }

            //                 // bump custom state
            //                 stack.m_customState = 2;
            //             }
        }
        else
        {
            // this is NOT a long running operation
            // perform I2C transaction using driver's SYNC API

            if (busI2c->ReadSize != 0 && busI2c->WriteSize != 0)
            {
                // this is a Write/Read transaction
                transactionResult = i2c_write_read(
                    busI2c->drv,
                    busI2c->Address,
                    busI2c->WriteBuffer,
                    busI2c->WriteSize,
                    busI2c->ReadBuffer,
                    busI2c->ReadSize);
            }
            else
            {
                if (busI2c->ReadSize == 0)
                {
                    // this is Write only transaction
                    transactionResult = i2c_write(busI2c->drv, busI2c->WriteBuffer, busI2c->WriteSize, busI2c->Address);
                }
                else
                {
                    // this is a Read only transaction
                    transactionResult = i2c_read(busI2c->drv, busI2c->ReadBuffer, busI2c->ReadSize, busI2c->Address);
                }
            }
        }

        //         while (eventResult)
        //         {
        //             if (!isLongRunningOperation)
        //             {
        //                 // this is not a long running operation so nothing to do
        //                 here break;
        //             }

        //             if (busI2c->WorkingThread->state == CH_STATE_FINAL)
        //             {
        //                 // I2C working thread is now complete
        //                 break;
        //             }

        //             // non-blocking wait allowing other threads to run while we
        //             wait for the I2C transaction to complete
        //             NANOCLR_CHECK_HRESULT(
        //                 g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread,
        //                 *timeout, Event_I2cMaster, eventResult));
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

        //             i2cReleaseBus(busI2c->Driver);

        // create the return object (I2cTransferResult)
        // only at this point we are sure that there will be a return from this
        // thread so it's OK to use the managed stack
        {
            CLR_RT_HeapBlock &top = stack.PushValueAndClear();
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));
            result = top.Dereference();
            FAULT_ON_NULL(result);
        }

        // Indicate error of necessary
        if (transactionResult != 0)
        {
            result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_UnknownError);

            // set the bytes transferred count to 0 because we don't have a way to
            // know how many bytes were actually sent/received
            result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(0);
        }
        else
        {
            // successfull transaction
            // set the result field
            result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_FullTransfer);

            // set the bytes transferred field
            result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(
                (CLR_UINT32)(busI2c->WriteSize + busI2c->ReadSize));
        }

        //             if (busI2c->ReadSize > 0)
        //             {
        //                 // invalidate cache over read buffer to ensure that
        //                 content from DMA is read
        //                 // (only required for Cortex-M7)
        //                 cacheBufferInvalidate(busI2c->ReadBuffer,
        //                 busI2c->ReadSize);
        //             }
    }
    // printk("NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte--\n");
#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif  // CONFIG_I2C
    NANOCLR_NOCLEANUP();
}
// estimate the time required to perform the I2C transaction
bool IsLongRunningOperation(
    uint16_t writeSize,
    uint16_t readSize,
    float byteTime,
    uint32_t &estimatedDurationMiliseconds)
{
    // add an extra byte to account for the address
    estimatedDurationMiliseconds = byteTime * (writeSize + readSize + 1);

    if (estimatedDurationMiliseconds > CLR_RT_Thread::c_TimeQuantum_Milliseconds)
    {
        // total operation time will exceed thread quantum, so this is a long
        // running operation
        return true;
    }
    else
    {
        return false;
    }
}
