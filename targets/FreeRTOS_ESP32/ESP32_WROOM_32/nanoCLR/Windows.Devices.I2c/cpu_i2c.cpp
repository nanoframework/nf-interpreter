//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
//
// CPU_I2C
//
// Read / Write physical I2C bus
//

#include <targetHAL.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

#include <Esp32_os.h>
#include "Esp32_DeviceMapping.h"

// Tag for ESP32 logging
static const char *TAG = "I2cDevice";


void CPU_I2C_GetPins(uint8_t i2cBusIndex, GPIO_PIN &clockPin, GPIO_PIN &dataPin)
{
    dataPin = (GPIO_PIN)Esp32_GetMappedDevicePins(DEV_TYPE_I2C, i2cBusIndex, 0);
    clockPin = (GPIO_PIN)Esp32_GetMappedDevicePins(DEV_TYPE_I2C, i2cBusIndex, 1);
}


bool CPU_I2C_Initialize(uint8_t i2cBus, bool fastSpeed)
{
    GPIO_PIN clockPin, dataPin;
    esp_err_t res;

    // Get pins used by i2c bus
    CPU_I2C_GetPins(i2cBus, clockPin, dataPin);

    // Check pins have been configured
    if (clockPin == GPIO_PIN_NONE || dataPin == GPIO_PIN_NONE )
    {
        ESP_LOGE(TAG, "I2C pins for I2C%d not configured", i2cBus);
        return false;
    }

    i2c_config_t conf;

    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)dataPin;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)clockPin;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = (fastSpeed) ? 400000 : 100000;

    res = i2c_param_config((i2c_port_t)i2cBus, &conf);
    if (res != ESP_OK)
    {
        return false;
    }
    
    res = i2c_driver_install((i2c_port_t)i2cBus, I2C_MODE_MASTER, 0, 0, 0);
    if (res != ESP_OK)
    {
        return false;
    }

    return true;
}

// Uninitialise the I2C bus
bool CPU_I2C_Uninitialize(uint8_t i2cBus)
{
    esp_err_t ret = i2c_driver_delete((i2c_port_t)i2cBus); 
    if (ret != ESP_OK)
    {
        return false;
    }

    return true;
}

I2cTransferStatus CPU_I2C_WriteRead(
    uint8_t i2cBus,
    int slaveAddress,
    unsigned char *writeData,
    int writeSize,
    unsigned char *readData,
    int readSize)
{
    esp_err_t i2cStatus;
    I2cTransferStatus transferResult  = I2cTransferStatus_FullTransfer;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    if (writeSize != 0) // Write
    {
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (slaveAddress << 1) | I2C_MASTER_WRITE, 1);
        i2cStatus = i2c_master_write(cmd, &writeData[0], writeSize, true);
        if (i2cStatus != ESP_OK)
        {
            ESP_LOGE(TAG, "i2c_master_write error:%d", i2cStatus);
        }
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

    i2cStatus = i2c_master_cmd_begin((i2c_port_t)i2cBus, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    switch (i2cStatus)
    {
        default:
            transferResult = I2cTransferStatus_UnknownError;
            break;
        case ESP_FAIL :
            transferResult = I2cTransferStatus_SlaveAddressNotAcknowledged;
            break;
        case ESP_ERR_TIMEOUT:
            transferResult = I2cTransferStatus_ClockStretchTimeout;
            break;
        case ESP_OK:
            transferResult = I2cTransferStatus_FullTransfer;
            break;
    }

    return transferResult;
}