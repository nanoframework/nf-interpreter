//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

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
        I2cTransferStatus_ClockStretchTimeout = 0,
        I2cTransferStatus_FullTransfer,
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
#if STM32_SPI_USE_I2C1
        case 1 :   _drv = &I2C1;
                    break;
#endif
#if STM32_SPI_USE_I2C2
        case 2 :    _drv = &I2C2;
                    break;
#endif
#if STM32_SPI_USE_I2C3
        case 3 :    _drv = &I2C3;
                    break;
#endif
    }

    // Create the final configuration
    nfI2CConfig cfg =
    {
        {
            OPMODE_I2C,
            busSpeed == I2cBusSpeed_StandardMode ? 100000U : 400000U,
            busSpeed == I2cBusSpeed_StandardMode ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2
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

        int returnStatus = I2cTransferStatus_FullTransfer;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        uint8_t bus = (uint8_t)(pThis[ FIELD___i2cBus ].NumericByRef().s4 / 1000);

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

        i2cStart(cfg.Driver, &cfg.Configuration);
        i2cAcquireBus(cfg.Driver);
        if (readSize == 0)  // Write only
        {
            i2cMasterTransmitTimeout(cfg.Driver, cfg.SlaveAddress, &writeData[0], writeSize, NULL, 0, TIME_INFINITE);
        }
        else                // WriteRead
        {
            i2cMasterTransmitTimeout(cfg.Driver, cfg.SlaveAddress, &writeData[0], writeSize, &readData[0], readSize, TIME_INFINITE);
        }
        i2cReleaseBus(cfg.Driver);
        i2cStop(cfg.Driver);

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

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeRead___I4__SZARRAY_U1__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        unsigned char * readData = NULL;
        int readSize = 0;

        int returnStatus = I2cTransferStatus_FullTransfer;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        uint8_t bus = (uint8_t)(pThis[ FIELD___i2cBus ].NumericByRef().s4 / 1000);

        // Get a complete low-level SPI configuration, depending on user's managed parameters
        nfI2CConfig cfg = GetConfig(bus, pThis[ FIELD___connectionSettings ].Dereference());

        CLR_RT_HeapBlock_Array* readBuffer = stack.Arg1().DereferenceArray();
        if (readBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            readData = readBuffer->GetFirstElement();

            // get the size of the buffer by reading the number of elements in the HeapBlock array
            readSize = readBuffer->m_numOfElements;
        }

        i2cStart(cfg.Driver, &cfg.Configuration);
        i2cAcquireBus(cfg.Driver);
        i2cMasterReceiveTimeout (cfg.Driver, cfg.SlaveAddress, &readData[0], readSize, TIME_INFINITE);
        i2cReleaseBus(cfg.Driver);
        i2cStop(cfg.Driver);

        // null pointers and vars
        readData = NULL;
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
       // declare the device selector string whose max size is "I2C1,I2C2,I2C3," + terminator and init with the terminator
       char deviceSelectorString[ 15 + 1] = { 0 };

   #if STM32_SPI_USE_I2C1
       strcat(deviceSelectorString, "I2C1,");
   #endif
   #if STM32_SPI_USE_I2C2
       strcat(deviceSelectorString, "I2C2,");
   #endif
   #if STM32_SPI_USE_I2C3
       strcat(deviceSelectorString, "I2C3,");
   #endif

       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP();
}
