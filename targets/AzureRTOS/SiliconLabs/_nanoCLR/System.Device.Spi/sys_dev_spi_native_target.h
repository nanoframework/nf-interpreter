//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_SPI_NATIVE_TARGET_H
#define SYS_DEV_SPI_NATIVE_TARGET_H


// #include <LaunchCLR.h>
// #include <string.h>
// #include <targetPAL.h>


#include <sys_dev_spi_native.h>

#include <em_device.h>
#include <spidrv.h>

// struct representing the SPI bus
struct NF_PAL_SPI
{
    int BusIndex;
    SPIDRV_Handle_t *Driver;
    SPIDRV_Init_t Configuration;
    // SpiBusConfiguration BusConfiguration;

    //SPI_Callback Callback;

    bool SequentialTxRx;
    bool BufferIs16bits;

    uint8_t *WriteBuffer;
    uint16_t WriteSize;

    uint8_t *ReadBuffer;
    uint16_t ReadSize;
};

// the following macro defines a function that configures the GPIO pins for an Gecko SPI peripheral
// it gets called in the System_Device_SPi_SPiDevice::NativeInit function
// this is required because the SPI peripherals can use multiple GPIO configuration combinations
#define SPI_CONFIG_PINS(                                                                                               \
    num,                                                                                                               \
    gpio_port_sck,                                                                                                     \
    sck_pin,
    sck_port_location,                                                                                                         \
    gpio_port_miso,                                                                                                 \
    miso_pin,      
    miso_port_location,                                                                                                    \
    gpio_port_mosi,                                                                                                    \
    mosi_pin,
    mosi_port_location                                                                                                          \
    )  
                                                                                                 \
    void ConfigPins_SPI##num(bool isHalfDuplex)                                                                        \
    {   
        SPI##num##_PAL.Configuration->sclPort = gpio_port_scl;                                                         \
        SPI##num##_PAL.Configuration->sdaPort = gpio_port_sda;                                                         \
        SPI##num##_PAL.Configuration->sclPin = scl_pin;                                                                \
        SPI##num##_PAL.Configuration->sdaPin = sda_pin;                                                                \
        SPI##num##_PAL.Configuration->portLocationScl = scl_port_location;                                             \
        SPI##num##_PAL.Configuration->portLocationSda = sda_port_location;                                             \
                                                                                                              \
    }

//////////////////////////////////////////////////////////////////////////////////////////////
// when an SPI is defined the declarations below will have the real function/configuration //
// in the target folder @ target_windows_devices_spi_config.cpp                             //
//////////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_SPI1(bool isHalfDuplex);
void ConfigPins_SPI2(bool isHalfDuplex);
void ConfigPins_SPI3(bool isHalfDuplex);
void ConfigPins_SPI4(bool isHalfDuplex);
void ConfigPins_SPI5(bool isHalfDuplex);
void ConfigPins_SPI6(bool isHalfDuplex);

#endif // SYS_DEV_SPI_NATIVE_TARGET_H
