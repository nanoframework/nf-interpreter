//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_DEV_ADC_NATIVE_H_
#define _WIN_DEV_ADC_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <hal.h>

struct Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel
{
    static const int FIELD___channelNumber = 1;
    static const int FIELD___adcController = 2;
    static const int FIELD___syncLock = 3;
    static const int FIELD___disposedValue = 4;

    NANOCLR_NATIVE_DECLARE(NativeReadValue___I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeDisposeChannel___VOID__I4);

    //--//

};

struct Library_win_dev_adc_native_Windows_Devices_Adc_AdcController
{
    static const int FIELD_STATIC__s_deviceCollection = 0;

    static const int FIELD___deviceId = 1;
    static const int FIELD___channelMode = 2;

    NANOCLR_NATIVE_DECLARE(NativeOpenChannel___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetChannelCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeGetMaxValue___I4);
    NANOCLR_NATIVE_DECLARE(NativeGetMinValue___I4);
    NANOCLR_NATIVE_DECLARE(NativeIsChannelModeSupported___BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetResolutionInBits___I4);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Adc;

struct NF_PAL_ADC_PORT_PIN_CHANNEL
{
    stm32_gpio_t*   portId;
    uint8_t         pin;
    uint8_t         adcChannel;
};


// the following macro defines a function that configures the GPIO pins for a STM32 UART/USART
// it gets called in the Windows_Devices_SerialCommunication_SerialDevice::NativeConfig function
// this is required because the UART/USART peripherals can use multiple GPIO configuration combinations
#define ADC_CONFIG_PIN(num, channel, gpio_port, adc_pin) void ConfigADC##num_In##channel() { \
  palSetGroupMode(gpio_port, PAL_PORT_BIT(adc_pin), 0, PAL_MODE_INPUT_ANALOG); \
    }


#if STM32_ADC_USE_ADC1
    extern const NF_PAL_ADC_PORT_PIN_CHANNEL Adc1PortPinConfig[];
    extern const int Adc1ChannelCount;
#endif
#if STM32_ADC_USE_ADC2
    extern const NF_PAL_ADC_PORT_PIN_CHANNEL Adc2PortPinConfig[];
    extern const int Adc2ChannelCount;
#endif
#if STM32_ADC_USE_ADC3
    extern const NF_PAL_ADC_PORT_PIN_CHANNEL Adc3PortPinConfig[];
    extern const int Adc3ChannelCount;
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
// when a UART/USART is defined the declarations bellow will have the real function/configuration 
// in the target folder @ target_windows_devices_adc_config.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigADC1_In0();
void ConfigADC1_In1();
void ConfigADC1_In2();
void ConfigADC1_In3();
void ConfigADC1_In4();
void ConfigADC1_In5();
void ConfigADC1_In6();
void ConfigADC1_In7();
void ConfigADC1_In8();
void ConfigADC1_In9();
void ConfigADC1_In10();
void ConfigADC1_In11();
void ConfigADC1_In12();
void ConfigADC1_In13();
void ConfigADC1_In14();
void ConfigADC1_In15();

void ConfigADC2_In0();
void ConfigADC2_In1();
void ConfigADC2_In2();
void ConfigADC2_In3();
void ConfigADC2_In4();
void ConfigADC2_In5();
void ConfigADC2_In6();
void ConfigADC2_In7();
void ConfigADC2_In8();
void ConfigADC2_In9();
void ConfigADC2_In10();
void ConfigADC2_In11();
void ConfigADC2_In12();
void ConfigADC2_In13();
void ConfigADC2_In14();
void ConfigADC2_In15();

void ConfigADC3_In0();
void ConfigADC3_In1();
void ConfigADC3_In2();
void ConfigADC3_In3();
void ConfigADC3_In4();
void ConfigADC3_In5();
void ConfigADC3_In6();
void ConfigADC3_In7();
void ConfigADC3_In8();
void ConfigADC3_In9();
void ConfigADC3_In10();
void ConfigADC3_In11();
void ConfigADC3_In12();
void ConfigADC3_In13();
void ConfigADC3_In14();
void ConfigADC3_In15();

#endif  //_WIN_DEV_ADC_NATIVE_H_
