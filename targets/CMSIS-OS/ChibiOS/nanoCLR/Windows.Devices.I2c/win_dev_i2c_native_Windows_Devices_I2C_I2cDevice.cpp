//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <string.h>
#include <targetPAL.h>
#include <nanoHAL.h>
#include "win_dev_i2c_native.h"

#define I2C_TRANSACTION_TIMEOUT_MS      1000

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.I2c.I2cSharingMode (in managed code) !!!    //
///////////////////////////////////////////////////////////////////////////////////////
enum I2cSharingMode
{
    Exclusive = 0,
    Shared
};

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.I2c.I2cTransferStatus (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////
 enum I2cTransferStatus
{
    I2cTransferStatus_FullTransfer = 0,
    I2cTransferStatus_ClockStretchTimeout,
    I2cTransferStatus_PartialTransfer,
    I2cTransferStatus_SlaveAddressNotAcknowledged,
    I2cTransferStatus_UnknownError
};

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.I2c.I2cBusSpeed (in managed code) !!!       //
///////////////////////////////////////////////////////////////////////////////////////
enum I2cBusSpeed
{
    I2cBusSpeed_StandardMode = 0,
    I2cBusSpeed_FastMode
};

typedef Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings I2cConnectionSettings;


/////////////////////////////////////////////////////
// I2C PAL strucs declared in win_dev_i2c_native.h //
/////////////////////////////////////////////////////
#if STM32_I2C_USE_I2C1
    NF_PAL_I2C I2C1_PAL;
#endif
#if STM32_I2C_USE_I2C2
    NF_PAL_I2C I2C2_PAL;
#endif
#if STM32_I2C_USE_I2C3
    NF_PAL_I2C I2C3_PAL;
#endif
#if STM32_I2C_USE_I2C4
    NF_PAL_I2C I2C4_PAL;
#endif


// ChibiOS I2C working thread
static THD_FUNCTION(I2CWorkingThread, arg)
{
    NF_PAL_I2C* palI2c = (NF_PAL_I2C*)arg;
    msg_t result;

    // because the bus access is shared, acquire the appropriate bus
    i2cStart(palI2c->Driver, &palI2c->Configuration);
    i2cAcquireBus(palI2c->Driver);

    if (palI2c->ReadSize != 0 && palI2c->WriteSize != 0)
    {
        // this is a Write/Read transaction
       result = i2cMasterTransmitTimeout(palI2c->Driver, palI2c->Address, palI2c->WriteBuffer, palI2c->WriteSize, palI2c->ReadBuffer, palI2c->ReadSize, I2C_TRANSACTION_TIMEOUT_MS);
    }
    else
    {
        if (palI2c->ReadSize == 0)
        {
            // this is Write only transaction
            result = i2cMasterTransmitTimeout(palI2c->Driver, palI2c->Address, palI2c->WriteBuffer, palI2c->WriteSize, NULL, 0, I2C_TRANSACTION_TIMEOUT_MS);
        }
        else
        {
            // this is a Read only transaction
            result = i2cMasterReceiveTimeout (palI2c->Driver, palI2c->Address, palI2c->ReadBuffer, palI2c->ReadSize, I2C_TRANSACTION_TIMEOUT_MS);
        }
    }

    i2cReleaseBus(palI2c->Driver);

    // fire event for I2C transaction complete
    Events_Set(SYSTEM_EVENT_FLAG_I2C_MASTER);

    chThdExit(result);
}

void Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::GetConfig(CLR_RT_HeapBlock* managedConfig, I2CConfig* llConfig)
{
    I2cBusSpeed busSpeed = (I2cBusSpeed)managedConfig[ I2cConnectionSettings::FIELD___busSpeed ].NumericByRef().s4;

    // set the LL I2C configuration (according to I2C driver version)
    #if defined(STM32F1xx_MCUCONF) || defined(STM32F4xx_MCUCONF) || defined(STM32L1xx_MCUCONF)

    llConfig->op_mode = OPMODE_I2C;
    llConfig->clock_speed = busSpeed == I2cBusSpeed_StandardMode ? 100000U : 400000U;
    llConfig->duty_cycle = busSpeed == I2cBusSpeed_StandardMode ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;

    #endif

    #if defined(STM32F7xx_MCUCONF) || defined(STM32F3xx_MCUCONF) || defined(STM32F0xx_MCUCONF) || \
            defined(STM32L0xx_MCUCONF) ||  defined(STM32L4xx_MCUCONF) || \
            defined(STM32H7xx_MCUCONF) 

    // Standard mode : 100 KHz, Rise time 120 ns, Fall time 25 ns, 54MHz clock source
    // Fast mode : 400 KHz, Rise time 120 ns, Fall time 25 ns, 54MHz clock source
    // Timing register value calculated by STM32 CubeMx
    llConfig->timingr = busSpeed == I2cBusSpeed_StandardMode ? 0x80201721 : 0x00B01B59;
    llConfig->cr1 = 0;
    llConfig->cr2 = 0;

    #endif

}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the I2cDevice() constructor in managed code for details
        uint8_t busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        // init the PAL struct for this I2C bus and assign the respective driver
        // all this occurs if not already done
        // why do we need this? because several I2cDevice objects can be created associated to the same bus just using different addresses
        switch (busIndex)
        {
    #if STM32_I2C_USE_I2C1
            case 1 :
                if(I2C1_PAL.Driver == NULL)
                {
                    Init_I2C1();
                    I2C1_PAL.Driver = &I2CD1;
                }
                break;
    #endif
    #if STM32_I2C_USE_I2C2
            case 2 :
                if(I2C2_PAL.Driver == NULL)
                {
                    Init_I2C2();
                    I2C2_PAL.Driver = &I2CD2;
                }
                break;
    #endif
    #if STM32_I2C_USE_I2C3
            case 3 :
                if(I2C3_PAL.Driver == NULL)
                {
                    Init_I2C3();
                    I2C3_PAL.Driver = &I2CD3;
                }
                break;
    #endif
    #if STM32_I2C_USE_I2C4
            case 4 :
                if(I2C4_PAL.Driver == NULL)
                {
                    Init_I2C4();
                    I2C4_PAL.Driver = &I2CD4;
                }
                break;
    #endif
            default:
                // this I2C bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        NF_PAL_I2C* palI2c;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the I2cDevice() constructor in managed code for details
        uint8_t busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        // uninit the PAL struct for this I2C bus
        switch (busIndex)
        {
    #if STM32_I2C_USE_I2C1
            case 1 :
                UnInit_I2C1();
                break;
    #endif
    #if STM32_I2C_USE_I2C2
            case 2 :
                UnInit_I2C2();
                break;
    #endif
    #if STM32_I2C_USE_I2C3
            case 3 :
                UnInit_I2C3();
                break;
    #endif
    #if STM32_I2C_USE_I2C4
            case 4 :
                UnInit_I2C3();
                break;
    #endif
            default:
                // this I2C bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        uint8_t busIndex;
        NF_PAL_I2C* palI2c;
        
        CLR_RT_HeapBlock    hbTimeout;
        CLR_INT64*          timeout;
        bool                eventResult = true;

        CLR_RT_HeapBlock_Array* writeBuffer;
        CLR_RT_HeapBlock_Array* readBuffer;
        CLR_RT_HeapBlock*       result;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the I2cDevice() constructor in managed code for details
        busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        // get the driver for the I2C bus
        switch (busIndex)
        {
    #if STM32_I2C_USE_I2C1
            case 1 :
                palI2c = &I2C1_PAL;
                break;
    #endif
    #if STM32_I2C_USE_I2C2
            case 2 :
                palI2c = &I2C2_PAL;
                break;
    #endif
    #if STM32_I2C_USE_I2C3
            case 3 :
                palI2c = &I2C3_PAL;
                break;
    #endif
    #if STM32_I2C_USE_I2C4
            case 4 :
                palI2c = &I2C4_PAL;
                break;
    #endif
            default:
                // the requested I2C bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // set timeout to twice the I2C transaction timeout
        // it should be enough to ensure thread start and execution, including any potential timeout there
        // this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() bellow
        hbTimeout.SetInteger((CLR_INT64)I2C_TRANSACTION_TIMEOUT_MS * TIME_CONVERSION__TO_MILLISECONDS);

        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

        // this is going to be used to check for the right event in case of simultaneous I2C transaction
        if(stack.m_customState == 1)
        {
            // get pointer to connection settings field
            CLR_RT_HeapBlock* connectionSettings = pThis[ FIELD___connectionSettings ].Dereference();

            // get a complete low-level I2C configuration, based in connection settings
            GetConfig(connectionSettings, &palI2c->Configuration);

            // get slave address from connection settings field
            palI2c->Address = (i2caddr_t)connectionSettings[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings::FIELD___slaveAddress].NumericByRef().s4;

            // when using I2Cv1 driver the address needs to be loaded in the I2C driver struct
    #if defined(STM32F1xx_MCUCONF) || defined(STM32F4xx_MCUCONF) || defined(STM32L1xx_MCUCONF)
            palI2c->Driver->addr = palI2c->Address;
    #endif

            // dereference the write and read buffers from the arguments
            writeBuffer = stack.Arg1().DereferenceArray();
            if (writeBuffer != NULL)
            {
                // get the size of the buffer by reading the number of elements in the HeapBlock array
                palI2c->WriteSize = writeBuffer->m_numOfElements;

                // copy to DMA write buffer
                memcpy(palI2c->WriteBuffer, writeBuffer->GetFirstElement(), palI2c->WriteSize);
            }

            readBuffer = stack.Arg2().DereferenceArray();
            if (readBuffer != NULL)
            {
                // get the size of the buffer by reading the number of elements in the HeapBlock array
                palI2c->ReadSize = readBuffer->m_numOfElements;
            }

            // flush DMA buffer to ensure cache coherency
            // (only required for Cortex-M7)
            cacheBufferFlush(palI2c->WriteBuffer, palI2c->WriteSize);
            
            // spawn working thread to perform the I2C transaction
            palI2c->WorkingThread = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(128),
                                        "I2CWT", NORMALPRIO, I2CWorkingThread, palI2c);

            if(palI2c->WorkingThread == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            }
            
            // bump custom state
            stack.m_customState = 2;
        }

        while(eventResult)
        {
            if(palI2c->WorkingThread->state == CH_STATE_FINAL)
            {
                // I2C working thread is now complete
                break;
            }

            // non-blocking wait allowing other threads to run while we wait for the I2C transaction to complete
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_I2cMaster, eventResult ));
        }

        // pop timeout heap block from stack
        stack.PopValue();

        if(eventResult)
        {
            // event occurred
            // ChibiOS requirement: need to call chThdWait for I2C working thread in order to have it's memory released to the heap, otherwise it won't be returned
            msg_t threadResult = chThdWait(palI2c->WorkingThread);

            // create the return object (I2cTransferResult)
            // only at this point we are sure that there will be a return from this thread so it's OK to use the managed stack
            CLR_RT_HeapBlock& top = stack.PushValueAndClear();
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));
            result = top.Dereference(); FAULT_ON_NULL(result);

            // get the result from the working thread execution
            if (threadResult != MSG_OK)
            {
                // error in transaction
                int errors = i2cGetErrors(palI2c->Driver);
                
                // figure out what was the error and set the status field
                switch(errors)
                {
                    case I2C_ACK_FAILURE:
                        result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status ].SetInteger((CLR_UINT32)I2cTransferStatus_SlaveAddressNotAcknowledged);
                        break;

                    case I2C_TIMEOUT:
                        result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status ].SetInteger((CLR_UINT32)I2cTransferStatus_ClockStretchTimeout);
                        break;

                    default:
                        result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status ].SetInteger((CLR_UINT32)I2cTransferStatus_UnknownError);
                }

                // set the bytes transferred count to 0 because we don't have a way to know how many bytes were actually sent/received
                result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___bytesTransferred ].SetInteger(0);
            }
            else
            {
                // successfull transaction
                // set the result field
                result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status ].SetInteger((CLR_UINT32)I2cTransferStatus_FullTransfer);

                // set the bytes transferred field
                result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___bytesTransferred ].SetInteger((CLR_UINT32)(palI2c->WriteSize + palI2c->ReadSize));

                if(palI2c->ReadSize > 0)
                {
                    // because this was a Read transaction, need to copy from DMA buffer to managed buffer

                    // need to dereference readBuffer argument to be able to write back on it
                    readBuffer = stack.Arg2().DereferenceArray();

                    // invalidate cache over read buffer to ensure that content from DMA is read
                    // (only required for Cortex-M7)
                    cacheBufferInvalidate(palI2c->ReadBuffer, palI2c->ReadSize);

                    // copy I2C read buffer into managed buffer
                    memcpy(readBuffer->GetFirstElement(), palI2c->ReadBuffer, palI2c->ReadSize);
                }
            }
        }

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
       // declare the device selector string whose max size is "I2C1,I2C2,I2C3,I2C4," + terminator and init with the terminator
       char deviceSelectorString[20 + 1] = { 0 };

    #if STM32_I2C_USE_I2C1
        strcat(deviceSelectorString, "I2C1,");
    #endif
    #if STM32_I2C_USE_I2C2
        strcat(deviceSelectorString, "I2C2,");
    #endif
    #if STM32_I2C_USE_I2C3
        strcat(deviceSelectorString, "I2C3,");
    #endif
    #if STM32_I2C_USE_I2C4
        strcat(deviceSelectorString, "I2C4,");
    #endif
        // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP();
}
