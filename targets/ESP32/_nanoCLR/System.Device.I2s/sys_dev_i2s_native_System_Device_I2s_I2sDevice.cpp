//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "Core.h"
#include <targetPAL.h>
#include "sys_dev_i2s_native.h"

#include "Esp32_DeviceMapping.h"

#define DMA_BUF_LEN_IN_I2S_FRAMES (256)

// The transform buffer is used with the readinto() method to bridge the opaque DMA memory on the ESP devices
// with the app buffer.  It facilitates audio sample transformations.  e.g.  32-bits samples to 16-bit samples.
// The size of 240 bytes is an engineering optimum that balances transfer performance with an acceptable use of heap
// space
#define SIZEOF_TRANSFORM_BUFFER_IN_BYTES (240)

#define NUM_I2S_USER_FORMATS       (8)
#define I2S_RX_FRAME_SIZE_IN_BYTES (8)

typedef Library_sys_dev_i2s_native_System_Device_I2s_I2sConnectionSettings I2sConnectionSettings;
typedef Library_corlib_native_System_SpanByte SpanByte;

static char Esp_I2S_Initialised_Flag[I2S_NUM_MAX] = {
    0,
#if (I2S_NUM_MAX > 1)
    0
#endif
};

#if SOC_I2S_SUPPORTS_ADC
static bool Adc_Mode_Enabled = false;
#endif

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

#if SOC_I2S_SUPPORTS_ADC
            if (Adc_Mode_Enabled && c == I2S_NUM_0)
            {
                i2s_adc_disable((i2s_port_t)c);
                Adc_Mode_Enabled = false;
            }
#endif
            i2s_driver_uninstall((i2s_port_t)c);
            Esp_I2S_Initialised_Flag[c] = 0;
        }
    }
}

i2s_bits_per_sample_t get_dma_bits(uint8_t mode, i2s_bits_per_sample_t bits)
{
#if SOC_I2S_SUPPORTS_ADC
    if (mode & I2S_MODE_ADC_BUILT_IN)
    {
        return bits;
    }
#endif

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

int8_t get_frame_mapping_index(i2s_bits_per_sample_t bits, i2s_channel_fmt_t format)
{
    if (format != I2S_CHANNEL_FMT_RIGHT_LEFT)
    {
        // Mono
        switch (bits)
        {
            case I2S_BITS_PER_SAMPLE_8BIT:
                return 0;
            case I2S_BITS_PER_SAMPLE_16BIT:
                return 1;
            case I2S_BITS_PER_SAMPLE_24BIT:
                return 2;
            case I2S_BITS_PER_SAMPLE_32BIT:
                return 3;
        }
    }
    else
    {
        // Stereo
        switch (bits)
        {
            case I2S_BITS_PER_SAMPLE_8BIT:
                return 4;
            case I2S_BITS_PER_SAMPLE_16BIT:
                return 5;
            case I2S_BITS_PER_SAMPLE_24BIT:
                return 6;
            case I2S_BITS_PER_SAMPLE_32BIT:
                return 7;
        }
    }

    return 0;
}

// The frame map is used with the readinto() method to transform the audio sample data coming
// from DMA memory (32-bit stereo, with the L and R channels reversed) to the format specified
// in the I2S constructor.  e.g.  16-bit mono
const int8_t i2s_frame_map[NUM_I2S_USER_FORMATS][I2S_RX_FRAME_SIZE_IN_BYTES] = {
    {7, -1, -1, -1, -1, -1, -1, -1}, // Mono, 8-bits
    {6, 7, -1, -1, -1, -1, -1, -1},  // Mono, 16-bits
    {5, 6, 7, -1, -1, -1, -1, -1},   // Mono, 24-bits
    {4, 5, 6, 7, -1, -1, -1, -1},    // Mono, 32-bits
    {7, 3, -1, -1, -1, -1, -1, -1},  // Stereo, 8-bits
    {6, 7, 2, 3, -1, -1, -1, -1},    // Stereo, 16-bits
    {5, 6, 7, 1, 2, 3, -1, -1},      // Stereo, 24-bits
    {4, 5, 6, 7, 0, 1, 2, 3},        // Stereo, 32-bits
};

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

#if !(SOC_I2S_SUPPORTS_ADC)
    if (mode & I2sMode_AdcBuiltIn)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
#endif

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

#if SOC_I2S_SUPPORTS_ADC

        // Configure ADC Mode
        if (mode & I2S_MODE_ADC_BUILT_IN)
        {
            // TODO - make attenuation configurable?
            adc_atten_t atten = ADC_ATTEN_DB_11;

            // TODO Re-use logic in ADC?
            int channelNumber = -1;

            // Find out ADC Channel for pin configured as I2S_DATAIN
            for (unsigned int i = 0; i < sizeof(Esp32_ADC_DevicePinMap) / sizeof(int8_t); i++)
            {
                if (Esp32_ADC_DevicePinMap[i] == pin_config.data_in_num)
                {
                    channelNumber = i;
                    break;
                }
            }
            if (channelNumber < 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
            }

            int adcUnit = channelNumber <= 9 ? 1 : 2;
            switch (adcUnit)
            {
                case 1:
                    // Normal channel 0-7 ?
                    if (channelNumber <= 7)
                    {
                        // TODO - Make ADC Resolution configurable?
                        // adc1_config_width(width_bit);

                        if (adc1_config_channel_atten((adc1_channel_t)channelNumber, atten) != ESP_OK)
                        {
                            NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
                        }

                        if (i2s_set_adc_mode(ADC_UNIT_1, (adc1_channel_t)channelNumber) != ESP_OK)
                        {
                            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                        }
                    }
                    break;

                case 2:
                    // Adjust for ADC2
                    channelNumber -= 10;

                    // TODO - Make ADC Resolution configurable?
                    // adc2_config_width(width_bit);

                    if (adc2_config_channel_atten((adc2_channel_t)channelNumber, atten) != ESP_OK)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
                    }

                    if (i2s_set_adc_mode(ADC_UNIT_2, (adc1_channel_t)channelNumber) != ESP_OK)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                    }

                    break;
            }

            if (i2s_adc_enable(bus) != ESP_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
            }
        }
#endif

        // Ensure driver gets unitialized during soft reboot
        HAL_AddSoftRebootHandler(Esp32_I2s_UnitializeAll);

        Esp_I2S_Initialised_Flag[bus]++;
#if SOC_I2S_SUPPORTS_ADC
        if (mode & I2S_MODE_ADC_BUILT_IN)
        {
            Adc_Mode_Enabled = true;
        }
#endif
    }

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

    {
        CLR_RT_HeapBlock *pConfig;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        CLR_RT_HeapBlock *readSpanByte = NULL;
        CLR_RT_HeapBlock_Array *readBuffer = NULL;
        uint8_t *readData = NULL;
        int readSize = 0;
        int readOffset = 0;
        uint8_t transform_buffer[SIZEOF_TRANSFORM_BUFFER_IN_BYTES];
        uint32_t a_index = 0;

        // get a pointer to the managed spi connectionSettings object instance
        pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        // subtract 1 to get ESP32 bus number
        i2s_port_t bus = (i2s_port_t)(pConfig[I2sConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

        // fetch some configs to calculate some things later on:
        i2s_bits_per_sample_t bitsPerSample =
            (i2s_bits_per_sample_t)(pConfig[I2sConnectionSettings::FIELD___i2sBitsPerSample].NumericByRef().s4);
        i2s_channel_fmt_t format =
            (i2s_channel_fmt_t)pConfig[I2sConnectionSettings::FIELD___i2sChannelFormat].NumericByRef().s4;
        uint8_t channels = (format == I2S_CHANNEL_FMT_RIGHT_LEFT ? 2 : 1);
        uint8_t appbuf_sample_size_in_bytes = (bitsPerSample / 8) * channels;

        if (bus != I2S_NUM_0
#if (I2S_NUM_MAX > 1)
            && bus != I2S_NUM_1
#endif
        )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // dereference the SpanByte from the arguments
        readSpanByte = stack.Arg1().Dereference();
        if (readSpanByte != NULL)
        {
            readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();

            if (readBuffer != NULL)
            {
                // Get the read offset, only the elements defined by the span must be read, not the whole array
                readOffset = readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

                // use the span length as read size, only the elements defined by the span must be read
                readSize = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

                if (readSize > 0)
                {
                    readData = (uint8_t *)readBuffer->GetElement(readOffset);

                    uint32_t num_bytes_needed_from_dma =
                        readSize * (I2S_RX_FRAME_SIZE_IN_BYTES / appbuf_sample_size_in_bytes);

                    CLR_RT_ProtectFromGC gcWriteBuffer(*readBuffer);

                    while (num_bytes_needed_from_dma)
                    {
                        size_t num_bytes_requested_from_dma = MIN(sizeof(transform_buffer), num_bytes_needed_from_dma);
                        size_t num_bytes_received_from_dma = 0;

                        esp_err_t ret = i2s_read(
                            bus,
                            transform_buffer,
                            num_bytes_requested_from_dma,
                            &num_bytes_received_from_dma,
                            portMAX_DELAY);

                        if (ret != ESP_OK)
                        {
                            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                        }

                        // process the transform buffer one frame at a time.
                        // copy selected bytes from the transform buffer into the user supplied readBuffer.
                        // Example:
                        //   a I2S object is configured for 16-bit mono.  This configuration associates to
                        //   a frame map index of 0 = { 6,  7, -1, -1, -1, -1, -1, -1 } in the i2s_frame_map array
                        //   This mapping indicates:
                        //      readBuffer[x+0] = frame[6]
                        //      readBuffer[x+1] = frame[7]
                        //      frame bytes 0-5 are not used

                        uint32_t t_index = 0;
                        uint8_t f_index = get_frame_mapping_index(bitsPerSample, format);
                        while (t_index < num_bytes_received_from_dma)
                        {
                            uint8_t *transform_p = transform_buffer + t_index;

                            for (uint8_t i = 0; i < I2S_RX_FRAME_SIZE_IN_BYTES; i++)
                            {
                                int8_t t_to_a_mapping = i2s_frame_map[f_index][i];
                                if (t_to_a_mapping != -1)
                                {
                                    *readData++ = transform_p[t_to_a_mapping];
                                    a_index++;
                                }
                                t_index++;
                            }
                        }

                        num_bytes_needed_from_dma -= num_bytes_received_from_dma;
                    }
                }
            }
        }
    }

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

        if (bus != I2S_NUM_0
#if (I2S_NUM_MAX > 1)
            && bus != I2S_NUM_1
#endif
        )
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

        if (bus != I2S_NUM_0
#if (I2S_NUM_MAX > 1)
            && bus != I2S_NUM_1
#endif
        )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

#if SOC_I2S_SUPPORTS_ADC
        i2s_mode_t mode = (i2s_mode_t)(pConfig[I2sConnectionSettings::FIELD___i2sMode].NumericByRef().s4);

        if (mode & I2S_MODE_ADC_BUILT_IN && bus != I2S_NUM_0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
#endif

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
#if SOC_I2S_SUPPORTS_ADC
            i2s_mode_t mode = (i2s_mode_t)(pConfig[I2sConnectionSettings::FIELD___i2sMode].NumericByRef().s4);
            if (mode & I2S_MODE_ADC_BUILT_IN)
            {
                i2s_adc_disable(bus);
            }
#endif

            i2s_driver_uninstall(bus);

            Esp_I2S_Initialised_Flag[bus] = 0;
        }
    }
    NANOCLR_NOCLEANUP();
}
