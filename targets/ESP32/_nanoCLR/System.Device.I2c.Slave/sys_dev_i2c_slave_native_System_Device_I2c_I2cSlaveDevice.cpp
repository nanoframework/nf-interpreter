//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_slave_native_target.h"

// copied from esp-idf\components\driver\test\test_i2c.c because they aren't made available on a header
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

#define I2C_SLAVE_WORKER_TASK_STACK_SIZE 2048

typedef Library_corlib_native_System_SpanByte SpanByte;

#if SOC_I2C_NUM > 0
NF_PAL_I2CSLAVE I2cSlave0_PAL;
#endif
#if SOC_I2C_NUM > 1
NF_PAL_I2CSLAVE I2cSlave1_PAL;
#endif

NF_PAL_I2CSLAVE *GetPalI2cSlaveFromBusIndex(int busIndex)
{
    // find matching I2C PAL
    switch (busIndex)
    {

#if SOC_I2C_NUM > 0
        case I2C_NUM_0:
            return &I2cSlave0_PAL;
#endif

#if SOC_I2C_NUM > 1
        case I2C_NUM_1:
            // set UART PAL
            return &I2cSlave1_PAL;
#endif

        default:
            return NULL;
    }
}

void UninitializePalI2cSlave(NF_PAL_I2CSLAVE *palI2cSlave)
{
    if (palI2cSlave && palI2cSlave->I2cSlaveWorkerTask)
    {
        i2c_reset_tx_fifo(palI2cSlave->BusNum);
        i2c_reset_rx_fifo(palI2cSlave->BusNum);

        // delete driver
        i2c_driver_delete(palI2cSlave->BusNum);

        // flag bus as uninitialized & unused
        Esp_I2C_Initialised_Flag[palI2cSlave->BusNum] = 0;
    }
}

void Esp32_I2cSlave_UninitializeAll()
{
    for (int busIndex = 0; busIndex < I2C_NUM_MAX; busIndex++)
    {
        if (Esp_I2C_Initialised_Flag[busIndex])
        {
            // free buffers memory
            UninitializePalI2cSlave(GetPalI2cSlaveFromBusIndex(busIndex));
        }
    }
}

void I2cSlaveRxWorkerTask(void *pvParameters)
{
    // get PAL UART from task parameters
    NF_PAL_I2CSLAVE *palI2c = (NF_PAL_I2CSLAVE *)pvParameters;

    // read data to I2C slave
    palI2c->BytesTransferred =
        i2c_slave_read_buffer(palI2c->BusNum, palI2c->Buffer, palI2c->RequestedBytes, palI2c->TimeoutTicks);

    // set event flag for I2C Slave
    Events_Set(SYSTEM_EVENT_FLAG_I2C_SLAVE);

    // delete task
    vTaskDelete(NULL);
}

void I2cSlaveTxWorkerTask(void *pvParameters)
{
    // get PAL UART from task parameters
    NF_PAL_I2CSLAVE *palI2c = (NF_PAL_I2CSLAVE *)pvParameters;

    // reset TX fifo before the operation
    i2c_reset_tx_fifo(palI2c->BusNum);

    // write data to I2C slave
    palI2c->BytesTransferred =
        i2c_slave_write_buffer(palI2c->BusNum, palI2c->Buffer, palI2c->RequestedBytes, palI2c->TimeoutTicks);

    // set event flag for I2C Slave
    Events_Set(SYSTEM_EVENT_FLAG_I2C_SLAVE);

    // delete task
    vTaskDelete(NULL);
}

HRESULT Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_I2CSLAVE *palI2c;

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

    if (bus < I2C_NUM_0 || bus >= SOC_I2C_NUM)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // if this bus hasn't been initialized yet, let's do it
    if (Esp_I2C_Initialised_Flag[bus] == 0)
    {
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

        res = i2c_driver_install(bus, I2C_MODE_SLAVE, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);

        if (res != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // Ensure driver gets uninitialized during soft reboot
        HAL_AddSoftRebootHandler(Esp32_I2cSlave_UninitializeAll);

        // flag bus as initialized & used
        Esp_I2C_Initialised_Flag[bus]++;

        palI2c = GetPalI2cSlaveFromBusIndex(bus);
        if (palI2c == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        palI2c->BusNum = bus;
        palI2c->Buffer = NULL;
        palI2c->RequestedBytes = 0;
        palI2c->BytesTransferred = 0;
        palI2c->TimeoutTicks = 0;

        // reset buffers, just in case
        i2c_reset_tx_fifo(bus);
        i2c_reset_rx_fifo(bus);
    }

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

    UninitializePalI2cSlave(GetPalI2cSlaveFromBusIndex(bus));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::
    NativeTransmit___I4__SystemSpanByte__SystemSpanByte__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_I2CSLAVE *palI2c = NULL;

    int32_t bufferOffset = 0;
    int32_t requestedCount = 0;
    uint32_t readCount = 0;
    uint32_t bytesTransfered = 0;
    i2c_port_t bus;
    bool isRead = false;
    int64_t timeoutMs;
    int64_t *timeoutTicks;
    bool eventResult = true;

    CLR_RT_HeapBlock hbTimeout;
    CLR_RT_HeapBlock *readSpanByte;
    CLR_RT_HeapBlock *writeSpanByte;
    CLR_RT_HeapBlock_Array *readBuffer = NULL;
    CLR_RT_HeapBlock_Array *writeBuffer = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get bus index
    // subtract 1 to get ESP32 bus number
    bus = (i2c_port_t)(pThis[FIELD___busId].NumericByRef().s4 - 1);

    // get pointer to PAL UART
    palI2c = GetPalI2cSlaveFromBusIndex(bus);
    if (palI2c == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // get read buffer
    readSpanByte = stack.Arg1().Dereference();

    // get write buffer
    writeSpanByte = stack.Arg2().Dereference();

    // both parameters can't be null
    if (!(readSpanByte) && !(writeSpanByte))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();

    if (readBuffer != NULL)
    {
        // set flag to read operation
        isRead = true;

        // Get the read offset, only the elements defined by the span must be read, not the whole array
        bufferOffset = readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

        // use the span length as read size, only the elements defined by the span must be read
        requestedCount = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
    }

    if (!isRead)
    {
        writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();

        if (writeBuffer != NULL)
        {
            // Get the write offset, only the elements defined by the span must be written, not the whole array
            bufferOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

            // use the span length as write size, only the elements defined by the span must be written
            requestedCount = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
        }
    }

    if (requestedCount == 0 || (writeBuffer == NULL && readBuffer == NULL))
    {
        // nothing to do here
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // setup timeout
    // always set CLR infinite timeout as the FreeRTOS task takes care of the timeout
    hbTimeout.SetInteger((CLR_INT64)-1);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    if (stack.m_customState == 1)
    {
        // perform parameter validation and setup TX operation
        timeoutMs = stack.Arg3().NumericByRef().s4;

        if (timeoutMs == -1)
        {
            palI2c->TimeoutTicks = portMAX_DELAY;
        }
        else
        {
            palI2c->TimeoutTicks = timeoutMs / portTICK_PERIOD_MS;
        }

        // need to allocate buffer from internal memory
        palI2c->Buffer = (uint8_t *)heap_caps_malloc(requestedCount, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

        if (palI2c->Buffer == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        if (isRead)
        {
            // read operation
            // clear allocated buffer
            memset(palI2c->Buffer, 0, requestedCount);

            // set buffer size
            palI2c->RequestedBytes = requestedCount;

            // ... and try to read the requested number of bytes from the I2C buffer
            palI2c->BytesTransferred = i2c_slave_read_buffer(palI2c->BusNum, palI2c->Buffer, palI2c->RequestedBytes, 1);

            // did we read the requested number of bytes?
            if (palI2c->BytesTransferred == requestedCount)
            {
                // yes, set buffer size to 0
                palI2c->RequestedBytes = 0;

                // update counter
                bytesTransfered = palI2c->BytesTransferred;

                // we have enough bytes, skip wait for event
                eventResult = false;

                // clear event by getting it
                Events_Get(SYSTEM_EVENT_FLAG_I2C_SLAVE);

                // push dummy value onto the eval stack
                stack.PushValueU4(0);
            }
            else
            {
                // no, adjust the requested bytes count
                palI2c->RequestedBytes = requestedCount - palI2c->BytesTransferred;

                // push read count onto the eval stack
                stack.PushValueU4(palI2c->BytesTransferred);

                // allocate memory for the task stack
                palI2c->I2cSlaveWorkerTaskStack = (StackType_t *)heap_caps_malloc(
                    I2C_SLAVE_WORKER_TASK_STACK_SIZE,
                    MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

                // create the worker task
                palI2c->I2cSlaveWorkerTask = xTaskCreateStatic(
                    I2cSlaveRxWorkerTask,
                    "I2CSlaveWrk",
                    I2C_SLAVE_WORKER_TASK_STACK_SIZE,
                    palI2c,
                    12,
                    palI2c->I2cSlaveWorkerTaskStack,
                    &palI2c->I2cSlaveWorkerTaskBuffer);
            }
        }
        else
        {
            // write operation

            // copy buffer content to working buffer
            memcpy(palI2c->Buffer, (uint8_t *)writeBuffer->GetElement(bufferOffset), requestedCount);

            if (requestedCount < I2C_SLAVE_TX_BUF_LEN)
            {
                // write data to I2C slave directly

                palI2c->BytesTransferred =
                    i2c_slave_write_buffer(palI2c->BusNum, palI2c->Buffer, requestedCount, portMAX_DELAY);

                // update counter
                bytesTransfered = palI2c->BytesTransferred;

                // skip waiting for the event
                eventResult = false;

                // clear event by getting it
                Events_Get(SYSTEM_EVENT_FLAG_I2C_SLAVE);

                // push dummy value onto the eval stack
                stack.PushValueU4(0);
            }
            else
            {
                // setup task to write data to I2C slave

                // no, adjust the requested bytes count
                palI2c->RequestedBytes = requestedCount;

                // push write count onto the eval stack
                stack.PushValueU4(palI2c->BytesTransferred);

                // allocate memory for the task stack
                palI2c->I2cSlaveWorkerTaskStack = (StackType_t *)heap_caps_malloc(
                    I2C_SLAVE_WORKER_TASK_STACK_SIZE,
                    MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

                // create the worker task
                palI2c->I2cSlaveWorkerTask = xTaskCreateStatic(
                    I2cSlaveTxWorkerTask,
                    "I2CSlaveWrk",
                    I2C_SLAVE_WORKER_TASK_STACK_SIZE,
                    palI2c,
                    12,
                    palI2c->I2cSlaveWorkerTaskStack,
                    &palI2c->I2cSlaveWorkerTaskBuffer);
            }
        }

        // update custom state
        stack.m_customState = 2;
    }

    // get count of bytes already read from the eval stack
    readCount = stack.m_evalStack[1].NumericByRef().u4;

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_I2cSlave, eventResult));

        if (eventResult)
        {
            // event occurred
            // check for errors
            if (palI2c->BytesTransferred < 0)
            {
                // error
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
            else
            {
                // update counter
                bytesTransfered = palI2c->BytesTransferred + readCount;

                // done here
                break;
            }

            // done here
            break;
        }
        else
        {
            // timeout expired
            // nothing to do here
            break;
        }
    }

    if (isRead)
    {
        // copy over to the managed buffer
        // grab the pointer to the array by starting and the offset specified in the span
        memcpy(readBuffer->GetElement(bufferOffset), palI2c->Buffer, bytesTransfered);
    }

    // pop read count from the stack
    stack.PopValue();

    // pop timeout heap block from stack
    stack.PopValue();

    // set result to the number of bytes read/written
    stack.SetResult_I4((CLR_INT32)bytesTransfered);

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING)
    {
        if (palI2c != NULL && palI2c->Buffer != NULL)
        {
            heap_caps_free(palI2c->Buffer);
            palI2c->Buffer = NULL;
        }
        if (palI2c != NULL && palI2c->I2cSlaveWorkerTaskStack != NULL)
        {
            heap_caps_free(palI2c->I2cSlaveWorkerTaskStack);
            palI2c->I2cSlaveWorkerTaskStack = NULL;
        }
    }

    NANOCLR_CLEANUP_END();
}
