//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include <string.h>
#include <targetPAL.h>

#include "win_dev_i2c_native.h"

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

nfI2CConfig Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::GetConfig(int bus, CLR_RT_HeapBlock* config)
{
    I2CDriver * _drv;
    
    int slaveAddress = config[ I2cConnectionSettings::FIELD___slaveAddress ].NumericByRef().s4;
    int busSpeed = config[ I2cConnectionSettings::FIELD___busSpeed ].NumericByRef().s4;

    // Choose the driver that is mapped to the chosen bus
    switch (bus)
    {
#if STM32_I2C_USE_I2C1
        case 1 :   _drv = &I2CD1;
                    break;
#endif
#if STM32_I2C_USE_I2C2
        case 2 :    _drv = &I2CD2;
                    break;
#endif
#if STM32_I2C_USE_I2C3
        case 3 :    _drv = &I2CD3;
                    break;
#endif
#if STM32_I2C_USE_I2C4
        case 4 :    _drv = &I2CD4;
                    break;
#endif
    }

    // Create the final configuration
    nfI2CConfig cfg =
    {
        {
#ifdef STM32F4xx_MCUCONF
            OPMODE_I2C,
            busSpeed == I2cBusSpeed_StandardMode ? 100000U : 400000U,
            busSpeed == I2cBusSpeed_StandardMode ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2
#endif
#ifdef STM32F7xx_MCUCONF
            // Standard mode : 100 KHz, Rise time 120 ns, Fall time 25 ns, 54MHz clock source
            // Fast mode : 400 KHz, Rise time 120 ns, Fall time 25 ns, 54MHz clock source
            // Timing register value calculated by STM32 CubeMx
            busSpeed == I2cBusSpeed_StandardMode ? 0x80201721 : 0x00B01B59,
            0,
            0
#endif
        },
        _drv,
        (uint16_t)slaveAddress
    };

    return cfg;
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeTransmit___I4__SZARRAY_U1__U4__SZARRAY_U1__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        unsigned char * writeData = NULL;
        unsigned char * readData = NULL;
        int writeSize = 0;
        int readSize = 0;
        msg_t i2cStatus;
        int returnStatus = I2cTransferStatus_FullTransfer;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        uint8_t bus = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        // Get a complete low-level SPI configuration, depending on user's managed parameters
        nfI2CConfig cfg = GetConfig(bus, pThis[ FIELD___connectionSettings ].Dereference());

        // dereference the write and read buffers from the arguments
        CLR_RT_HeapBlock_Array* writeBuffer = stack.Arg1().DereferenceArray();
        if (writeBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            writeData = writeBuffer->GetFirstElement();

            // get the size of the buffer by reading the number of elements in the HeapBlock array
            writeSize = writeBuffer->m_numOfElements;
        }

        CLR_RT_HeapBlock_Array* readBuffer = stack.Arg3().DereferenceArray();
        if (readBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            readData = readBuffer->GetFirstElement();

            // get the size of the buffer by reading the number of elements in the HeapBlock array
            readSize = readBuffer->m_numOfElements;
        }

        // because the bus access is shared, acquire the appropriate bus
        i2cStart(cfg.Driver, &cfg.Configuration);
        i2cAcquireBus(cfg.Driver);

#if defined(STM32F756xx) || defined(STM32F746xx) || defined(STM32F745xx) || defined(STM32F767xx) || \
defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx) || defined(STM32F722xx) || \
defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)

        // Data synchronous Barrier, forcing the CPU to respect the sequence of instruction without optimization
        __DSB();
#endif

        // WriteRead
        if (readSize != 0 && writeSize != 0)
        {
            i2cStatus = i2cMasterTransmitTimeout(cfg.Driver, cfg.SlaveAddress, &writeData[0], writeSize, &readData[0], readSize, TIME_INFINITE);
        }
        else
        {
            if (readSize == 0)      //Write
            {
                i2cStatus = i2cMasterTransmitTimeout(cfg.Driver, cfg.SlaveAddress, &writeData[0], writeSize, NULL, 0, TIME_INFINITE);
            }
            else                    // Read
            {
                i2cStatus = i2cMasterReceiveTimeout (cfg.Driver, cfg.SlaveAddress, &readData[0], readSize, TIME_INFINITE);
            }
        }

        // release the I2C now
        i2cReleaseBus(cfg.Driver);

        if (i2cStatus != MSG_OK)
        {
            int errorMask = i2cGetErrors(cfg.Driver);

            //TODO: return correct error status regarding UWP API
            returnStatus = I2cTransferStatus_UnknownError;
        }

        // null pointers and vars
        writeData = NULL;
        readData = NULL;
        writeBuffer = NULL;
        readBuffer = NULL;
        pThis = NULL;

        stack.SetResult_I4(returnStatus);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "I2C1,I2C2,I2C3,I2C4," + terminator and init with the terminator
       char deviceSelectorString[ 20 + 1] = { 0 };

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
