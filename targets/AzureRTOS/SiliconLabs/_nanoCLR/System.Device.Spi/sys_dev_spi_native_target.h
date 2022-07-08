//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_SPI_NATIVE_TARGET_H
#define SYS_DEV_SPI_NATIVE_TARGET_H

#include <sys_dev_spi_native.h>
#include <target_system_device_spi_config.h>
#include <targetPAL.h>

#include <em_device.h>
#include <spidrv.h>

#define SL_SPIDRV_EXP_BITRATE 1000000

// set missing defines
#if defined(USART0)
#ifndef GECKO_USE_SPI0
#define GECKO_USE_SPI0 FALSE
#endif
#else
#define GECKO_USE_SPI0 FALSE
#endif

#if defined(USART1)
#ifndef GECKO_USE_SPI1
#define GECKO_USE_SPI1 FALSE
#endif
#else
#define GECKO_USE_SPI1 FALSE
#endif

#if defined(USART2)
#ifndef GECKO_USE_SPI2
#define GECKO_USE_SPI2 FALSE
#endif
#else
#define GECKO_USE_SPI2 FALSE
#endif

#if defined(USART3)
#ifndef GECKO_USE_SPI3
#define GECKO_USE_SPI3 FALSE
#endif
#else
#define GECKO_USE_SPI3 FALSE
#endif

#if defined(USART4)
#ifndef GECKO_USE_SPI4
#define GECKO_USE_SPI4 FALSE
#endif
#else
#define GECKO_USE_SPI4 FALSE
#endif

#if defined(USART5)
#ifndef GECKO_USE_SPI5
#define GECKO_USE_SPI5 FALSE
#endif
#else
#define GECKO_USE_SPI5 FALSE
#endif

// struct representing the SPI bus
struct NF_PAL_SPI
{
    int BusIndex;
    SPIDRV_Handle_t Handle;
    SpiBusConfiguration BusConfiguration;

    SPI_Callback Callback;

    bool SequentialTxRx;
    bool BufferIs16bits;

    uint8_t *WriteBuffer;
    uint16_t WriteSize;

    uint8_t *ReadBuffer;
    uint16_t ReadSize;
};

////////////////////////////////////////////
// declaration of the the SPI PAL structs //
////////////////////////////////////////////
#if GECKO_USE_SPI0 == TRUE
extern NF_PAL_SPI SPI0_PAL;
#endif
#if GECKO_USE_SPI1 == TRUE
extern NF_PAL_SPI SPI1_PAL;
#endif
#if GECKO_USE_SPI2 == TRUE
extern NF_PAL_SPI SPI2_PAL;
#endif
#if GECKO_USE_SPI3 == TRUE
extern NF_PAL_SPI SPI3_PAL;
#endif
#if GECKO_USE_SPI4 == TRUE
extern NF_PAL_SPI SPI4_PAL;
#endif
#if GECKO_USE_SPI5 == TRUE
extern NF_PAL_SPI SPI5_PAL;
#endif

#if defined(_USART_ROUTELOC0_MASK)

// the following macro defines a function that configures the GPIO pins for an Gecko SPI peripheral
// it gets called in the System_Device_SPi_SPiDevice::NativeInit function
// this is required because the SPI peripherals can use multiple GPIO configuration combinations
#define SPI_CONFIG_PINS(                                                                                               \
    num,                                                                                                               \
    port_location_mosi,                                                                                                \
    port_location_miso,                                                                                                \
    port_location_clk,                                                                                                 \
    pin_mosi,                                                                                                          \
    pin_miso,                                                                                                          \
    pin_clk,                                                                                                           \
    port_location)                                                                                                     \
                                                                                                                       \
    void ConfigPins_SPI##num(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, SPIDRV_Init_t &spiInit)                  \
    {                                                                                                                  \
        spiInit.portLocationTx = port_location_mosi;                                                                   \
        spiInit.portLocationClk = port_location_clk;                                                                   \
        if (spiDeviceConfig.BusConfiguration != SpiBusConfiguration_HalfDuplex)                                        \
        {                                                                                                              \
            spiInit.portLocationRx = port_location_miso;                                                               \
        }                                                                                                              \
        if (spiDeviceConfig.DeviceChipSelect >= 0)                                                                     \
        {                                                                                                              \
            spiInit.portLocationCs = GPIO_PORT(spiDeviceConfig.DeviceChipSelect);                                      \
            if (spiDeviceConfig.ChipSelectActive)                                                                      \
            {                                                                                                          \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
            }                                                                                                          \
        }                                                                                                              \
    }

#else
#error "This routing configuration is not supported. Need to have _USART_ROUTELOC0_MASK."
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// when an SPI is defined the declarations below will have the real function/configuration //
// in the target folder @ target_windows_devices_spi_config.cpp                             //
//////////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_SPI0(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, SPIDRV_Init_t &spiInit);
void ConfigPins_SPI1(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, SPIDRV_Init_t &spiInit);
void ConfigPins_SPI2(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, SPIDRV_Init_t &spiInit);
void ConfigPins_SPI3(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, SPIDRV_Init_t &spiInit);
void ConfigPins_SPI4(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, SPIDRV_Init_t &spiInit);
void ConfigPins_SPI5(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, SPIDRV_Init_t &spiInit);

#endif // SYS_DEV_SPI_NATIVE_TARGET_H
