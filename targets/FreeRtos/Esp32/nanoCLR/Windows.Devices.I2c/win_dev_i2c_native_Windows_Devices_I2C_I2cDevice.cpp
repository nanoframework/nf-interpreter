//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <targetPAL.h>

#include "win_dev_i2c_native.h"

// Default I2C gpio pins
#define  I2C1_DATA      18
#define  I2C1_CLOCK     19
#define  I2C2_DATA      25
#define  I2C2_CLOCK     26

 
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


void Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::SetConfig(i2c_port_t bus, CLR_RT_HeapBlock* config)
{
    int busSpeed = config[ I2cConnectionSettings::FIELD___busSpeed ].NumericByRef().s4;

    i2c_config_t conf;
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num =  (gpio_num_t)((bus == I2C_NUM_0) ? I2C1_DATA : I2C2_DATA);
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = (gpio_num_t)((bus == I2C_NUM_0) ? I2C1_CLOCK : I2C2_CLOCK);
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = (busSpeed==0)? 100000 : 400000;

   i2c_param_config(bus, &conf);
}


HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
  
        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details, subtract 1 to get ESP32 bus number
        i2c_port_t bus = (i2c_port_t)((pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000) - 1);
        if ( bus != I2C_NUM_0 && bus != I2C_NUM_1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // Set the Bus parameters
        SetConfig( bus, pConfig);

        esp_err_t res =  i2c_driver_install( bus, I2C_MODE_MASTER, 0, 0, 0);
        if ( res != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
   }
   NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        i2c_port_t bus = (i2c_port_t)((pThis[ FIELD___i2cBus ].NumericByRef().s4 / 1000) - 1);
        bus = I2C_NUM_0;

        i2c_driver_delete(bus);
    }    
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
        esp_err_t i2cStatus;
        int returnStatus = I2cTransferStatus_FullTransfer;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details, subtract 1 to get ESP32 bus number
        i2c_port_t bus = (i2c_port_t)((pThis[ FIELD___i2cBus ].NumericByRef().s4 / 1000) - 1);
        bus = I2C_NUM_0;

        // Set the Bus parameters
        SetConfig( bus, pConfig);

        int slaveAddress = pConfig[ I2cConnectionSettings::FIELD___slaveAddress ].NumericByRef().s4;
    
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

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte( cmd, ( slaveAddress << 1 ) | I2C_MASTER_WRITE, 1);

        if ( writeSize != 0 )  // Write
        {
            i2c_master_write(cmd, &writeData[0], writeSize,  true);
        }
        if (readSize != 0 )  // Read
        {
            i2cStatus = i2c_master_read(cmd, &readData[0], readSize, true);
        }

        i2c_master_stop(cmd);
 
        i2cStatus = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);

        if (i2cStatus != ESP_OK)
        {
            if ( i2cStatus == ESP_FAIL )
                returnStatus = I2cTransferStatus_SlaveAddressNotAcknowledged;
            else
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
       // declare the device selector string whose max size is "I2C1,I2C2,I2C3," + terminator and init with the terminator
       char deviceSelectorString[ 15 + 1] = { 0 };

       strcat(deviceSelectorString, "I2C1,I2C2");
    
       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP();
}
