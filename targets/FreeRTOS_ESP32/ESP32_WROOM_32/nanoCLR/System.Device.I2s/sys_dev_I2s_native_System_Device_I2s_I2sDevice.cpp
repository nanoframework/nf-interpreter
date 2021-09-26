//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "Core.h"
#include <targetPAL.h>
#include "sys_dev_I2s_native.h"
#include "Esp32_DeviceMapping.h"

typedef Library_sys_dev_I2s_native_System_Device_I2s_I2sConnectionSettings I2sConnectionSettings;

static char Esp_I2S_Initialised_Flag[I2S_NUM_MAX] = {0, 0};

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

HRESULT SetI2sConfig(i2s_port_t bus, CLR_RT_HeapBlock *config)
{
    NANOCLR_HEADER();

    i2s_pin_config_t pin_config;
    // You will be able to uncomment when moved to later IDF
    // pin_config.mck_io_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 0);
    pin_config.bck_io_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 1);
    pin_config.ws_io_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 2);
    pin_config.data_out_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 3);
    pin_config.data_in_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 4);

    CLR_Debug::Printf("bck=%d,ws=%d,dto=%d,dti=%d\n",pin_config.bck_io_num,pin_config.ws_io_num,pin_config.data_out_num,pin_config.data_in_num);

    i2s_config_t conf;
    conf.mode = (i2s_mode_t)config[I2sConnectionSettings::FIELD___i2sMode].NumericByRef().s4;
    conf.sample_rate = (i2s_bits_per_sample_t)config[I2sConnectionSettings::FIELD___sampleRate].NumericByRef().s4;
    conf.bits_per_sample = (i2s_bits_per_sample_t)config[I2sConnectionSettings::FIELD___i2sBitsPerSample].NumericByRef().s4;
    conf.channel_format = (i2s_channel_fmt_t)config[I2sConnectionSettings::FIELD___i2sChannelFormat].NumericByRef().s4;

    int commformat = config[I2sConnectionSettings::FIELD___i2sConnectionFormat].NumericByRef().s4;
    // Important: this will have to have to be adjusted for IDF4
    switch(commformat)
    {
        case I2sCommunicationFormat_PcmLong:
            commformat = I2S_COMM_FORMAT_PCM_LONG;
            break;

        case I2sCommunicationFormat_PcmShort:
            commformat = I2S_COMM_FORMAT_PCM_SHORT;
            break;

        case I2sCommunicationFormat_StandardI2sLsb:
            commformat = I2S_COMM_FORMAT_I2S_LSB;
            break;

        case I2sCommunicationFormat_StandardI2sMsb:
            commformat = I2S_COMM_FORMAT_I2S_MSB;
            break;

        case I2sCommunicationFormat_StandardI2sPcm:
            commformat = I2S_COMM_FORMAT_PCM;
            break;
        case I2sCommunicationFormat_StandardI2s:
            commformat = I2S_COMM_FORMAT_I2S;
            break;
    }
    // I2sCommunicationFormat
    
    conf.communication_format = (i2s_comm_format_t)commformat;
    // As recommended from the sample
    conf.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    // Default size used in samples
    conf.dma_buf_count = 2;
    conf.dma_buf_len = 128;
    
    CLR_Debug::Printf("mod=%d,rate=%d,chfrmt=%d,chfrmt=%d,commfrmt=%d\n",conf.mode,conf.sample_rate,conf.bits_per_sample,conf.channel_format,conf.communication_format);

// TO BE DELETED
// Just to compare the default values setup from M5StickC sample
  i2s_config_t i2s_config;
  i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);// Set the I2S operating mode.  设置I2S工作模式
  i2s_config.sample_rate =  44100;// Set the I2S sampling rate.  设置I2S采样率
  i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT; // Fixed 12-bit stereo MSB.  固定为12位立体声MSB
  i2s_config.channel_format = I2S_CHANNEL_FMT_ALL_RIGHT;// Set the channel format.  设置频道格式
  i2s_config.communication_format = I2S_COMM_FORMAT_I2S;// Set the format of the communication.  设置通讯格式
  i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;// Set the interrupt flag.  设置中断的标志
  i2s_config.dma_buf_count = 2;//DMA buffer count.  DMA缓冲区计数
  i2s_config.dma_buf_len = 128;//DMA buffer length.  DMA缓冲区长度
  
CLR_Debug::Printf("mod=%d,rate=%d,chfrmt=%d,chfrmt=%d,commfrmt=%d\n",i2s_config.mode,i2s_config.sample_rate,i2s_config.bits_per_sample,i2s_config.channel_format,i2s_config.communication_format);



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

    // Set pin
    i2s_set_pin(bus, &pin_config);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::Read___VOID__SystemSpanByte( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::Read___VOID__SZARRAY_U2( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::Write___VOID__SZARRAY_U2( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::Write___VOID__SystemSpanByte( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::NativeInit___VOID( CLR_RT_StackFrame &stack )
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

        CLR_Debug::Printf("bus=%d\n", bus);

        if (bus != I2S_NUM_0 && bus != I2S_NUM_1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Set the Bus parameters
        NANOCLR_CHECK_HRESULT(SetI2sConfig(bus, pConfig));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::NativeDispose___VOID( CLR_RT_StackFrame &stack )
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
