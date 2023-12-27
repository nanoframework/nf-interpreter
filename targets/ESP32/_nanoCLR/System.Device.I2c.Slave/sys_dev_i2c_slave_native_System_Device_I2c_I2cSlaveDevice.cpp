//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_slave_native_target.h"

typedef Library_corlib_native_System_SpanByte SpanByte;

HRESULT Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint16_t deviceAddress;
    i2c_port_t bus;
    gpio_num_t dataPin;
    gpio_num_t clockPin;
    esp_err_t res;
    i2c_config_t configSlave = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = 0,
        .scl_io_num = 0,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave = {.addr_10bit_en = 0, .slave_addr = 0, .maximum_speed = 0},
        .clk_flags = 0};

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get bus index
    // subtract 1 to get ESP32 bus number
    bus = (i2c_port_t)(pThis[FIELD___busId].NumericByRef().s4 - 1);

    if (bus != I2C_NUM_0
#if I2C_NUM_MAX > 1
        && bus != I2C_NUM_1
#endif
    )
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // check if this bus is already used
    if (Esp_I2C_Initialised_Flag[bus] != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // get the address field
    deviceAddress = pThis[FIELD___deviceAddress].NumericByRef().s4;

    // compose the configuration
    dataPin = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2C, bus, 0);
    clockPin = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2C, bus, 1);

    configSlave.sda_io_num = dataPin;
    configSlave.scl_io_num = clockPin;
    configSlave.slave.slave_addr = deviceAddress;

    // config I2C parameters
    res = i2c_param_config(bus, &configSlave);

    ASSERT(res == ESP_OK);

    if (res != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    res = i2c_driver_install(bus, I2C_MODE_MASTER, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);

    if (res != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Ensure driver gets uninitialized during soft reboot
    HAL_AddSoftRebootHandler(Esp32_I2c_UnitializeAll);

    // flag bus as initialized & used
    Esp_I2C_Initialised_Flag[bus]++;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    i2c_port_t bus;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get bus index
    // subtract 1 to get ESP32 bus number
    bus = (i2c_port_t)(pThis[FIELD___busId].NumericByRef().s4 - 1);

    // delete driver
    i2c_driver_delete(bus);

    // flag bus as uninitialized & unused
    Esp_I2C_Initialised_Flag[bus] = 0;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::
    NativeTransmit___I4__SystemSpanByte__SystemSpanByte__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t *workBuffer = NULL;
    int bufferOffset = 0;
    int bufferSize = 0;
    i2c_port_t bus;
    int readCount;
    bool isRead;
    int64_t timeoutMs;

    CLR_RT_HeapBlock *readSpanByte;
    CLR_RT_HeapBlock *writeSpanByte;
    CLR_RT_HeapBlock *workingSpanByte;
    CLR_RT_HeapBlock_Array *workingData = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get bus index
    // subtract 1 to get ESP32 bus number
    bus = (i2c_port_t)(pThis[FIELD___busId].NumericByRef().s4 - 1);

    // get read buffer
    readSpanByte = stack.Arg1().Dereference();

    // get write buffer
    writeSpanByte = stack.Arg2().Dereference();

    if (readSpanByte != NULL)
    {
        workingSpanByte = readSpanByte;
        isRead = true;
    }
    else if (writeSpanByte != NULL)
    {
        workingSpanByte = writeSpanByte;
        isRead = false;
    }
    else
    {
        // something wrong here!!
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    workingData = workingSpanByte[SpanByte::FIELD___array].DereferenceArray();

    // Get the read offset, only the elements defined by the span must be read, not the whole array
    bufferOffset = workingSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

    // use the span length as read size, only the elements defined by the span must be read
    bufferSize = workingSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

    if (bufferSize == 0 || workingData == NULL)
    {
        // nothing to do here
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    timeoutMs = stack.Arg3().NumericByRef().s4;
    if (timeoutMs == -1)
    {
        timeoutMs = portMAX_DELAY;
    }
    else
    {
        timeoutMs = timeoutMs / portTICK_PERIOD_MS;
    }

    // need to allocate buffer from internal memory
    workBuffer = (uint8_t *)heap_caps_malloc(bufferSize, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    if (workBuffer == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear allocated buffer
    memset(workBuffer, 0, bufferSize);

    if (isRead)
    {
        // read data from I2C master
        readCount = i2c_slave_read_buffer(bus, workBuffer, bufferSize, timeoutMs);

        // copy over to the managed buffer
        // grab the pointer to the array by starting and the offset specified in the span
        memcpy(workingData->GetElement(bufferOffset), workBuffer, bufferSize);
    }
    else
    {
        // copy buffer content
        memcpy(workBuffer, (uint8_t *)workingData->GetElement(bufferOffset), bufferSize);

        // write data to I2C master
        readCount = i2c_slave_write_buffer(bus, workBuffer, bufferSize, timeoutMs);
    }

    // set result to the number of bytes read/written
    stack.SetInteger(readCount);

    NANOCLR_CLEANUP();

    if (workBuffer != NULL)
    {
        heap_caps_free(workBuffer);
    }

    NANOCLR_CLEANUP_END();
}
