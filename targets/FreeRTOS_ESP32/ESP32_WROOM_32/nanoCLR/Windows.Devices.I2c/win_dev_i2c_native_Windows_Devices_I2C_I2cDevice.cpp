//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <targetPAL.h>
#include "win_dev_i2c_native.h"
#include "Esp32_DeviceMapping.h"

 
static const char* TAG = "I2C";

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

static char Esp_I2C_Initialised_Flag[I2C_NUM_MAX] = {0,0};

void Esp32_I2c_UnitializeAll()
{
    for (int c = 0; c < I2C_NUM_MAX; c++) 
    {
        if (Esp_I2C_Initialised_Flag[c])
        {
            // Delete bus driver 
            i2c_driver_delete((i2c_port_t)c);
            Esp_I2C_Initialised_Flag[c] = 0;
        }
    }
}

void Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::SetConfig(i2c_port_t bus, CLR_RT_HeapBlock* config)
{
    int busSpeed = config[ I2cConnectionSettings::FIELD___busSpeed ].NumericByRef().s4;

    gpio_num_t DataPin  = (gpio_num_t)Esp32_GetMappedDevicePins( DEV_TYPE_I2C, bus, 0);
    gpio_num_t ClockPin = (gpio_num_t)Esp32_GetMappedDevicePins( DEV_TYPE_I2C, bus, 1);
    
    i2c_config_t conf;
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num =  DataPin;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = ClockPin;
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
 
        // If this is first devcie on Bus then init driver
        if ( Esp_I2C_Initialised_Flag[bus] == 0 )
        {
            esp_err_t res =  i2c_driver_install( bus, I2C_MODE_MASTER, 0, 0, 0);
            if ( res != ESP_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // Ensure driver gets unitialized during soft reboot
             HAL_AddSoftRebootHandler(Esp32_I2c_UnitializeAll);
            Esp_I2C_Initialised_Flag[bus]++;
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

        i2c_port_t bus = (i2c_port_t)((pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000) - 1);

        Esp_I2C_Initialised_Flag[bus]--;

        if ( Esp_I2C_Initialised_Flag[bus] <= 0 )
        {
            i2c_driver_delete(bus);

            Esp_I2C_Initialised_Flag[bus] = 0;
        }
    }    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        unsigned char * writeData = NULL;
        unsigned char * readData = NULL;
        int writeSize = 0;
        int readSize = 0;
        esp_err_t i2cStatus;

        CLR_RT_HeapBlock*       result;
        // create the return object (I2cTransferResult)
        CLR_RT_HeapBlock&       top = stack.PushValueAndClear();

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details, subtract 1 to get ESP32 bus number
        i2c_port_t bus = (i2c_port_t)((pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000) - 1);
 
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

        CLR_RT_HeapBlock_Array* readBuffer = stack.Arg2().DereferenceArray();
        if (readBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            readData = readBuffer->GetFirstElement();

            // get the size of the buffer by reading the number of elements in the HeapBlock array
            readSize = readBuffer->m_numOfElements;
        }

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();

        if ( writeSize != 0 )  // Write
        {
            i2c_master_start(cmd);
            i2c_master_write_byte( cmd, ( slaveAddress << 1 ) | I2C_MASTER_WRITE, 1);
            i2cStatus = i2c_master_write(cmd, &writeData[0], writeSize,  true);
            if (i2cStatus != ESP_OK) ESP_LOGE( TAG, "i2c_master_write error:%d", i2cStatus );

        }
        if (readSize != 0 )  // Read
        {
            i2c_master_start(cmd);
            i2c_master_write_byte( cmd, ( slaveAddress << 1 ) | I2C_MASTER_READ, 1);
            i2cStatus = i2c_master_read(cmd, &readData[0], readSize, I2C_MASTER_ACK );
            if (i2cStatus != ESP_OK) ESP_LOGE( TAG, "i2c_master_read error:%d", i2cStatus );
        }

        i2c_master_stop(cmd);
 
        i2cStatus = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);

        // create return object
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));
        result = top.Dereference(); FAULT_ON_NULL(result);

        if (i2cStatus != ESP_OK)
        {
            uint32_t transferResult = I2cTransferStatus_FullTransfer;

            // set the result field
            if ( i2cStatus == ESP_FAIL )
            {
                transferResult = I2cTransferStatus_SlaveAddressNotAcknowledged;
            }
            else if (i2cStatus == ESP_ERR_TIMEOUT )
            {
                transferResult = I2cTransferStatus_ClockStretchTimeout;
            }
            else
            {
                transferResult = I2cTransferStatus_UnknownError;
            }

            result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status ].SetInteger((CLR_UINT32)transferResult);

            // set the bytes transferred field
            result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___bytesTransferred ].SetInteger(0);
        }
        else
        {
            result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status ].SetInteger((CLR_UINT32)I2cTransferStatus_FullTransfer);

            // set the bytes transferred field
            result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___bytesTransferred ].SetInteger((CLR_UINT32)(writeSize + readSize));

            if(readSize > 0)
            {
                // because this was a Read transaction, need to copy from DMA buffer to managed buffer
                memcpy(readBuffer->GetFirstElement(), &readData[0], readSize);
            }
        }
   }
    NANOCLR_NOCLEANUP();
}
