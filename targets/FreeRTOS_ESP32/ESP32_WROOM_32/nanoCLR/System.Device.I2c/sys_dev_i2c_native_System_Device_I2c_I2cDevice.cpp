//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_native_target.h"

static const char *TAG = "I2C";

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

static char Esp_I2C_Initialised_Flag[I2C_NUM_MAX] = {0, 0};

// need to declare these as external
// TODO move them here after Windows.Devices.I2c is removed
extern void Esp32_I2c_UnitializeAll();
extern void SetConfig(i2c_port_t bus, CLR_RT_HeapBlock *config);

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pConfig;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a pointer to the managed spi connectionSettings object instance
        pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        // subtract 1 to get ESP32 bus number
        i2c_port_t bus = (i2c_port_t)(pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

        if (bus != I2C_NUM_0 && bus != I2C_NUM_1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Set the Bus parameters
        SetConfig(bus, pConfig);

        // If this is first device on Bus then init driver
        if (Esp_I2C_Initialised_Flag[bus] == 0)
        {
            esp_err_t res = i2c_driver_install(bus, I2C_MODE_MASTER, 0, 0, 0);
            if (res != ESP_OK)
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

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pConfig;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a pointer to the managed spi connectionSettings object instance
        pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        // subtract 1 to get ESP32 bus number
        i2c_port_t bus = (i2c_port_t)(pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

        Esp_I2C_Initialised_Flag[bus]--;

        if (Esp_I2C_Initialised_Flag[bus] <= 0)
        {
            i2c_driver_delete(bus);

            Esp_I2C_Initialised_Flag[bus] = 0;
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::
    NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        unsigned char *writeData = NULL;
        unsigned char *readData = NULL;
        int writeOffset = 0;
        int writeSize = 0;
        int readOffset = 0;
        int readSize = 0;
        esp_err_t i2cStatus;

        CLR_RT_HeapBlock *result;
        CLR_RT_HeapBlock *writeSpanByte;
        CLR_RT_HeapBlock *readSpanByte;
        CLR_RT_HeapBlock_Array *writeBuffer = NULL;
        CLR_RT_HeapBlock_Array *readBuffer = NULL;

        // create the return object (I2cTransferResult)
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock *pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        // subtract 1 to get ESP32 bus number
        i2c_port_t bus = (i2c_port_t)(pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

        int slaveAddress = pConfig[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;

        // dereference the write and read SpanByte from the arguments
        writeSpanByte = stack.Arg1().Dereference();
        if (writeSpanByte != NULL)
        {
            writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
            if (writeBuffer != NULL)
            {
                // Get the write offset, only the elements defined by the span must be written, not the whole array
                writeOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // grab the pointer to the array by starting and the offset specified in the span
                writeData = writeBuffer->GetElement(writeOffset);

                // use the span length as write size, only the elements defined by the span must be written
                writeSize = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            }
        }

        readSpanByte = stack.Arg2().Dereference();
        if (readSpanByte != 0)
        {
            readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
            if (readBuffer != NULL)
            {
                // Get the read offset, only the elements defined by the span must be read, not the whole array
                readOffset = readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // grab the pointer to the array by starting and the offset specified in the span
                readData = readBuffer->GetElement(readOffset);

                // use the span length as read size, only the elements defined by the span must be read
                readSize = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
            }
        }

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();

        if (writeSize != 0) // Write
        {
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (slaveAddress << 1) | I2C_MASTER_WRITE, 1);
            i2cStatus = i2c_master_write(cmd, &writeData[0], writeSize, true);
            if (i2cStatus != ESP_OK)
                ESP_LOGE(TAG, "i2c_master_write error:%d", i2cStatus);
        }
        if (readSize != 0) // Read
        {
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (slaveAddress << 1) | I2C_MASTER_READ, 1);
            if (readSize > 1)
            {
                // Additional read bytes with ACK
                i2c_master_read(cmd, &readData[0], readSize - 1, I2C_MASTER_ACK);
            }
            // Last read byte with NACK
            i2c_master_read_byte(cmd, &readData[readSize - 1], I2C_MASTER_NACK);
        }

        i2c_master_stop(cmd);

        i2cStatus = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);

        // create return object
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));
        result = top.Dereference();
        FAULT_ON_NULL(result);

        if (i2cStatus != ESP_OK)
        {
            uint32_t transferResult = I2cTransferStatus_FullTransfer;

            // set the result field
            if (i2cStatus == ESP_FAIL)
            {
                transferResult = I2cTransferStatus_SlaveAddressNotAcknowledged;
            }
            else if (i2cStatus == ESP_ERR_TIMEOUT)
            {
                transferResult = I2cTransferStatus_ClockStretchTimeout;
            }
            else
            {
                transferResult = I2cTransferStatus_UnknownError;
            }

            result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)transferResult);

            // set the bytes transferred field
            result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(0);
        }
        else
        {
            result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_FullTransfer);

            // set the bytes transferred field
            result[I2cTransferResult::FIELD___bytesTransferred].SetInteger((CLR_UINT32)(writeSize + readSize));
        }
    }
    NANOCLR_NOCLEANUP();
}
