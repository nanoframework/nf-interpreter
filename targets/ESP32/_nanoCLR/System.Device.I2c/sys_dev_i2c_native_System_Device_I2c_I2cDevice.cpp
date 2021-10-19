//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_native_target.h"

// copied from esp-idf\components\driver\test\test_i2c.c because they aren't made available on a header
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
// I2C master will check ack from slave
#define ACK_CHECK_EN 0x1

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

static char Esp_I2C_Initialised_Flag[I2C_NUM_MAX] = {0, 0};

// need to declare these as external
// TODO move them here after Windows.Devices.I2c is removed
extern void Esp32_I2c_UnitializeAll();
extern bool SetConfig(i2c_port_t bus, CLR_RT_HeapBlock *config);

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
        if (!SetConfig(bus, pConfig))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // If this is first device on Bus then init driver
        if (Esp_I2C_Initialised_Flag[bus] == 0)
        {
            // install driver in RAM to prevent issues with PSRAM cache
            esp_err_t res = i2c_driver_install(
                bus,
                I2C_MODE_MASTER,
                I2C_MASTER_RX_BUF_DISABLE,
                I2C_MASTER_TX_BUF_DISABLE,
                ESP_INTR_FLAG_IRAM);

            if (res != ESP_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // Ensure driver gets uninitialized during soft reboot
            HAL_AddSoftRebootHandler(Esp32_I2c_UnitializeAll);

            Esp_I2C_Initialised_Flag[bus]++;
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pConfig;
    i2c_port_t bus;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a pointer to the managed I2C connectionSettings object instance
    pConfig = pThis[FIELD___connectionSettings].Dereference();

    // get bus index
    // subtract 1 to get ESP32 bus number
    bus = (i2c_port_t)(pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

    Esp_I2C_Initialised_Flag[bus]--;

    if (Esp_I2C_Initialised_Flag[bus] <= 0)
    {
        i2c_driver_delete(bus);

        Esp_I2C_Initialised_Flag[bus] = 0;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::
    NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t *writeBuffer = NULL;
    uint8_t *readBuffer = NULL;
    int writeOffset = 0;
    int writeSize = 0;
    int readOffset = 0;
    int readSize = 0;
    esp_err_t opResult;
    uint32_t transferResult;
    i2c_port_t bus;
    int slaveAddress;
    i2c_cmd_handle_t cmd;

    CLR_RT_HeapBlock *result;
    CLR_RT_HeapBlock *writeSpanByte;
    CLR_RT_HeapBlock *readSpanByte;
    CLR_RT_HeapBlock_Array *writeData = NULL;
    CLR_RT_HeapBlock_Array *readData = NULL;
    CLR_RT_HeapBlock *pConfig = NULL;

    // create the return object (I2cTransferResult)
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a pointer to the managed I2C connectionSettings object instance
    pConfig = pThis[FIELD___connectionSettings].Dereference();

    // get bus index
    // subtract 1 to get ESP32 bus number
    bus = (i2c_port_t)(pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

    slaveAddress = pConfig[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;

    cmd = i2c_cmd_link_create();

    // dereference the write and read SpanByte from the arguments
    writeSpanByte = stack.Arg1().Dereference();
    if (writeSpanByte != NULL)
    {
        writeData = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();

        if (writeData != NULL)
        {
            // Get the write offset, only the elements defined by the span must be written, not the whole array
            writeOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

            // use the span length as write size, only the elements defined by the span must be written
            writeSize = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

            if (writeSize > 0)
            {
                // need to allocate buffer from internal memory
                writeBuffer = (uint8_t *)heap_caps_malloc(writeSize, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

                if (writeBuffer == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                // copy buffer content
                memcpy(writeBuffer, (uint8_t *)writeData->GetElement(writeOffset), writeSize);

                // setup write transaction
                i2c_master_start(cmd);
                i2c_master_write_byte(cmd, (slaveAddress << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
                opResult = i2c_master_write(cmd, writeBuffer, writeSize, true);

                ASSERT(opResult == ESP_OK);
            }
        }
    }

    readSpanByte = stack.Arg2().Dereference();
    if (readSpanByte != 0)
    {
        readData = readSpanByte[SpanByte::FIELD___array].DereferenceArray();

        if (readData != NULL)
        {
            // Get the read offset, only the elements defined by the span must be read, not the whole array
            readOffset = readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

            // use the span length as read size, only the elements defined by the span must be read
            readSize = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

            if (readSize > 0)
            {
                // need to allocate buffer from internal memory
                readBuffer = (uint8_t *)heap_caps_malloc(readSize, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

                if (readBuffer == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                // clear allocated buffer
                memset(readBuffer, 0, readSize);

                // setup read transaction
                i2c_master_start(cmd);
                i2c_master_write_byte(cmd, (slaveAddress << 1) | I2C_MASTER_READ, ACK_CHECK_EN);

                // read all bytes with ACK except the last one
                opResult = i2c_master_read(cmd, readBuffer, readSize, I2C_MASTER_LAST_NACK);

                ASSERT(opResult == ESP_OK);
            }
        }
    }

    i2c_master_stop(cmd);

    opResult = i2c_master_cmd_begin(bus, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    // create return object
    NANOCLR_CHECK_HRESULT(
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));

    result = top.Dereference();

    FAULT_ON_NULL(result);

    if (opResult != ESP_OK)
    {
        // set the result field
        if (opResult == ESP_FAIL)
        {
            transferResult = I2cTransferStatus_SlaveAddressNotAcknowledged;
        }
        else if (opResult == ESP_ERR_TIMEOUT)
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
        // was there a read operation?
        if (readSize > 0)
        {
            // grab the pointer to the array by starting and the offset specified in the span
            memcpy(readData->GetElement(readOffset), readBuffer, readSize);
        }

        result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_FullTransfer);

        // set the bytes transferred field
        result[I2cTransferResult::FIELD___bytesTransferred].SetInteger((CLR_UINT32)(writeSize + readSize));
    }

    NANOCLR_CLEANUP();

    if (writeBuffer)
    {
        platform_free(writeBuffer);
    }

    if (readBuffer)
    {
        platform_free(readBuffer);
    }

    NANOCLR_CLEANUP_END();
}
