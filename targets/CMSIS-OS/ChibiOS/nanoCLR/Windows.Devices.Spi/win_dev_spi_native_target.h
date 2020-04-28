//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_DEV_SPI_NATIVE_TARGET_H_
#define _WIN_DEV_SPI_NATIVE_TARGET_H_

#include <win_dev_spi_native.h>
#include <hal.h>

// struct representing the SPI 
struct NF_PAL_SPI
{
    SPIDriver*  Driver;
    SPIConfig   Configuration;
    float ByteTime;
    bool SequentialTxRx;

    uint8_t* WriteBuffer;
    uint16_t WriteSize;

    uint8_t* ReadBuffer;
    uint16_t ReadSize;
};

///////////////////////////////////////////
// declaration of the the SPI PAL strucs //
///////////////////////////////////////////
#if STM32_SPI_USE_SPI1
    extern NF_PAL_SPI SPI1_PAL;
#endif
#if STM32_SPI_USE_SPI2
    extern NF_PAL_SPI SPI2_PAL;
#endif
#if STM32_SPI_USE_SPI3
    extern NF_PAL_SPI SPI3_PAL;
#endif
#if STM32_SPI_USE_SPI4
    extern NF_PAL_SPI SPI4_PAL;
#endif
#if STM32_SPI_USE_SPI5
    extern NF_PAL_SPI SPI5_PAL;
#endif
#if STM32_SPI_USE_SPI6
    extern NF_PAL_SPI SPI6_PAL;
#endif

// the following macro defines a function that configures the GPIO pins for an STM32 SPI peripheral
// it gets called in the Windows_Devices_SPi_SPiDevice::NativeInit function
// this is required because the SPI peripherals can use multiple GPIO configuration combinations
#define SPI_CONFIG_PINS(num, gpio_port_sck, gpio_port_miso, gpio_port_mosi, alternate_function) void ConfigPins_SPI##num() \
{ \
    palSetLineMode(gpio_port_sck, \
                    PAL_MODE_ALTERNATE(alternate_function) | \
                    PAL_STM32_OSPEED_HIGHEST); \
    palSetLineMode(gpio_port_miso, \
                    PAL_MODE_ALTERNATE(alternate_function) | \
                    PAL_STM32_OSPEED_HIGHEST); \
    palSetLineMode(gpio_port_mosi, \
                    PAL_MODE_ALTERNATE(alternate_function) | \
                    PAL_STM32_OSPEED_HIGHEST); \
} 
//the above `}` should be deleted if we include the below, but cannot have comments in the define.
    // palSetLine(LINE_ARD_D15);
    // palSetLineMode(LINE_ARD_D15, 
    //                 PAL_MODE_OUTPUT_PUSHPULL);         /* CS0.                 */
    // palSetLine(LINE_ARD_D14);
    // palSetLineMode(LINE_ARD_D14,
    //                 PAL_MODE_OUTPUT_PUSHPULL);         /* CS1.                 */
//}

//////////////////////////////////////////////////////////////////////////////////////////////
// when an SPI is defined the declarations below will have the real function/configuration //
// in the target folder @ target_windows_devices_spi_config.cpp                             //
//////////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_SPI1();
void ConfigPins_SPI2();
void ConfigPins_SPI3();
void ConfigPins_SPI4();
void ConfigPins_SPI5();
void ConfigPins_SPI6();

#endif  //_WIN_DEV_SPI_NATIVE_TARGET_H_
