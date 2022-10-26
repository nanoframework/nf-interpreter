//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "Core.h"
#include <targetPAL.h>
#include "sys_dev_i2s_native.h"

#include "Esp32_DeviceMapping.h"

#define DMA_BUF_LEN_IN_I2S_FRAMES (256)

typedef Library_sys_dev_i2s_native_System_Device_I2s_I2sConnectionSettings I2sConnectionSettings;
typedef Library_corlib_native_System_SpanByte SpanByte;

static char Esp_I2S_Initialised_Flag[I2S_NUM_MAX] = {0, 0};

void swap_32_bit_stereo_channels(unsigned char *buffer, int length)
{
    uint32_t swap_sample;
    uint32_t *sample = (uint32_t *)buffer;
    uint32_t num_samples = length / 4;

    for (uint32_t i = 0; i < num_samples; i += 2)
    {
        swap_sample = sample[i + 1];
        sample[i + 1] = sample[i];
        sample[i] = swap_sample;
    }
}

void Esp32_I2s_UnitializeAll()
{
    for (int c = 0; c < I2S_NUM_MAX; c++)
    {
        if (Esp_I2S_Initialised_Flag[c])
        {
            // Delete bus driver
            i2s_driver_uninstall((i2s_port_t)c);
            Esp_I2S_Initialised_Flag[c] = 0;
        }
    }
}

i2s_bits_per_sample_t get_dma_bits(uint8_t mode, i2s_bits_per_sample_t bits)
{
    if (mode == (I2S_MODE_MASTER | I2S_MODE_TX))
    {
        return bits;
    }
    else
    { // Master Rx
        // read 32 bit samples for I2S hardware.  e.g. MEMS microphones
        return I2S_BITS_PER_SAMPLE_32BIT;
    }
}

uint32_t get_dma_buf_count(uint8_t mode, i2s_bits_per_sample_t bits, i2s_channel_fmt_t format, int32_t ibuf)
{
    // calculate how many DMA buffers need to be allocated
    uint32_t dma_frame_size_in_bytes = (get_dma_bits(mode, bits) / 8) * (format == I2S_CHANNEL_FMT_RIGHT_LEFT ? 2 : 1);

    uint32_t dma_buf_count = ibuf / (DMA_BUF_LEN_IN_I2S_FRAMES * dma_frame_size_in_bytes);

    return dma_buf_count;
}

i2s_comm_format_t get_i2s_commformat(int commformat)
{
    switch (commformat)
    {
        case I2sCommunicationFormat_I2S:
            commformat = I2S_COMM_FORMAT_STAND_I2S;
            break;

        case I2sCommunicationFormat_Msb:
            commformat = I2S_COMM_FORMAT_STAND_MSB;
            break;

        case I2sCommunicationFormat_PcmShort:
            commformat = I2S_COMM_FORMAT_STAND_PCM_SHORT;
            break;

        case I2sCommunicationFormat_PcmLong:
            commformat = I2S_COMM_FORMAT_STAND_PCM_LONG;
            break;

        case I2sCommunicationFormat_Max:
            commformat = I2S_COMM_FORMAT_STAND_MAX;
            break;
    }

    return (i2s_comm_format_t)commformat;
}

HRESULT SetI2sConfig(i2s_port_t bus, CLR_RT_HeapBlock *config)
{
    NANOCLR_HEADER();

    i2s_pin_config_t pin_config;
    pin_config.mck_io_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 0);
    pin_config.bck_io_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 1);
    pin_config.ws_io_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 2);
    pin_config.data_out_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 3);
    pin_config.data_in_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 4);

    // Important: this will have to be adjusted for IDF5
    i2s_config_t conf;
    int commformat = config[I2sConnectionSettings::FIELD___i2sConnectionFormat].NumericByRef().s4;
    i2s_mode_t mode = (i2s_mode_t)config[I2sConnectionSettings::FIELD___i2sMode].NumericByRef().s4;
    i2s_bits_per_sample_t bits =
        (i2s_bits_per_sample_t)config[I2sConnectionSettings::FIELD___i2sBitsPerSample].NumericByRef().s4;
    i2s_channel_fmt_t format =
        (i2s_channel_fmt_t)config[I2sConnectionSettings::FIELD___i2sChannelFormat].NumericByRef().s4;
    i2s_bits_per_sample_t sample_rate =
        (i2s_bits_per_sample_t)config[I2sConnectionSettings::FIELD___sampleRate].NumericByRef().s4;
    int bufferSize = config[I2sConnectionSettings::FIELD___bufferSize].NumericByRef().s4;

    conf.communication_format = get_i2s_commformat(commformat);
    conf.mode = mode;
    conf.bits_per_sample = get_dma_bits(mode, bits);
    conf.channel_format = format;
    conf.sample_rate = sample_rate;
    conf.intr_alloc_flags = ESP_INTR_FLAG_LOWMED;
    conf.dma_buf_count = get_dma_buf_count(mode, bits, format, bufferSize);
    conf.dma_buf_len = DMA_BUF_LEN_IN_I2S_FRAMES;
    conf.use_apll = false;
    conf.tx_desc_auto_clear = true;
    conf.fixed_mclk = 0;
#if (ESP_IDF_VERSION_MAJOR == 4) && (ESP_IDF_VERSION_MINOR >= 4)
    conf.mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT;
    conf.bits_per_chan = (i2s_bits_per_chan_t)0;
#endif

    // If this is first device on Bus then init driver
    if (Esp_I2S_Initialised_Flag[bus] == 0)
    {
        // Install driver without events
        esp_err_t res = i2s_driver_install(bus, &conf, 0, NULL);
        if (res != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Ensure driver gets unitialized during soft reboot
        HAL_AddSoftRebootHandler(Esp32_I2s_UnitializeAll);

        Esp_I2S_Initialised_Flag[bus]++;
    }

// apply low-level workaround for bug in some ESP-IDF versions that swap
// the left and right channels
// https://github.com/espressif/esp-idf/issues/6625
#if CONFIG_IDF_TARGET_ESP32S3
    REG_SET_BIT(I2S_TX_CONF_REG(bus), I2S_TX_MSB_SHIFT);
    REG_SET_BIT(I2S_TX_CONF_REG(bus), I2S_RX_MSB_SHIFT);
#else
    REG_SET_BIT(I2S_CONF_REG(bus), I2S_TX_MSB_RIGHT);
    REG_SET_BIT(I2S_CONF_REG(bus), I2S_RX_MSB_RIGHT);
#endif

#if (ESP_IDF_VERSION_MAJOR == 4) && (ESP_IDF_VERSION_MINOR >= 4)
    pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
#endif

    if (mode == (I2S_MODE_MASTER | I2S_MODE_RX))
    {
        pin_config.data_out_num = I2S_PIN_NO_CHANGE;
    }
    else
    { // TX
        pin_config.data_in_num = I2S_PIN_NO_CHANGE;
    }

    // Set pin
    i2s_set_pin(bus, &pin_config);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::Read___VOID__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::Write___VOID__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    {
        CLR_RT_HeapBlock *pConfig;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        CLR_RT_HeapBlock *writeSpanByte = NULL;
        CLR_RT_HeapBlock_Array *writeBuffer = NULL;
        uint8_t *writeData = NULL;
        int writeSize = 0;
        int writeOffset = 0;
        size_t bytesWritten;

        esp_err_t opResult;

        // get a pointer to the managed spi connectionSettings object instance
        pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        // subtract 1 to get ESP32 bus number
        i2s_port_t bus = (i2s_port_t)(pConfig[I2sConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);
        i2s_bits_per_sample_t bitsPerSample =
            (i2s_bits_per_sample_t)(pConfig[I2sConnectionSettings::FIELD___i2sBitsPerSample].NumericByRef().s4);

        if (bus != I2S_NUM_0 && bus != I2S_NUM_1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // dereference the write and read SpanByte from the arguments
        writeSpanByte = stack.Arg1().Dereference();
        if (writeSpanByte != NULL)
        {
            writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();

            if (writeBuffer != NULL)
            {
                // Get the write offset, only the elements defined by the span must be written, not the whole array
                writeOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // use the span length as write size, only the elements defined by the span must be written
                writeSize = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

                if (writeSize > 0)
                {
                    CLR_RT_ProtectFromGC gcWriteBuffer(*writeBuffer);
                    writeData = (unsigned char *)writeBuffer->GetElement(writeOffset);

                    if (bitsPerSample == I2S_BITS_PER_SAMPLE_32BIT)
                    {
                        swap_32_bit_stereo_channels(writeData, writeSize);
                    }

                    // setup write transaction
                    opResult = i2s_write(bus, writeData, writeSize, &bytesWritten, portMAX_DELAY);

                    if (opResult != ESP_OK)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                    }
                }
            }
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pConfig;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a pointer to the managed I2s connectionSettings object instance
        pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        // subtract 1 to get ESP32 bus number
        i2s_port_t bus = (i2s_port_t)(pConfig[I2sConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

        if (bus != I2S_NUM_0 && bus != I2S_NUM_1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Set the Bus parameters
        NANOCLR_CHECK_HRESULT(SetI2sConfig(bus, pConfig));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
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
        i2s_port_t bus = (i2s_port_t)(pConfig[I2sConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

        Esp_I2S_Initialised_Flag[bus]--;

        if (Esp_I2S_Initialised_Flag[bus] <= 0)
        {
            i2s_driver_uninstall(bus);

            Esp_I2S_Initialised_Flag[bus] = 0;
        }
    }
    NANOCLR_NOCLEANUP();
}
