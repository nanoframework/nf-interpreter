// Copyright Skyworks Solutions, Inc. All Rights Reserved.

#ifndef COM_SKY_NF_DEV_SPI_NATIVE_TARGET_H
#define COM_SKY_NF_DEV_SPI_NATIVE_TARGET_H

#include <com_sky_nf_dev_spi_native.h>
#include <target_com_sky_nf_dev_spi_config.h>
#include <targetPAL.h>

#include <em_device.h>
#include <em_gpio.h>
#include <../System.Device.Spi/nf_gecko_spi_driver.h>
#include <../System.Device.Spi/sys_dev_spi_native_target.h>

// all PAL declarations are coming from CPU SPI

///////////////////////////////////////////////////
// for Skyworks SPI CS is controlled by SPI driver
#define SPI_CS_CONTROL  spidrvCsControlAuto
///////////////////////////////////////////////////

#if defined(_USART_ROUTELOC0_MASK)

////////////////////////////////////////////////////////
// INIT_SPI_CONFIG from System.Device.Spi redefined here
#if defined(INIT_SPI_CONFIG)
#undef INIT_SPI_CONFIG
#endif
////////////////////////////////////////////////////////

// the following macro defines a function that configures the GPIO pins for an Gecko SPI peripheral
// it gets called in the System_Device_SPi_SPiDevice::NativeInit function
// this is required because the SPI peripherals can use multiple GPIO configuration combinations
#define INIT_SPI_CONFIG(num, sck_port_location, mosi_port_location, miso_port_location, cs_port_location)              \
    void InitSpiConfig##num(NF_SpiDriver_Init_t &initSpiData, bool isHalfDuplex)                                       \
    {                                                                                                                  \
        initSpiData.port = USART##num;                                                                                 \
        initSpiData.portLocationTx = mosi_port_location;                                                               \
        initSpiData.portLocationClk = sck_port_location;                                                               \
        initSpiData.portLocationCs = cs_port_location;                                                                 \
        if (!isHalfDuplex)                                                                                             \
        {                                                                                                              \
            initSpiData.portLocationRx = miso_port_location;                                                           \
        }                                                                                                              \
    }

#else
#error "This routing configuration is not supported. Need to have _USART_ROUTELOC0_MASK."
#endif

#endif // SYS_DEV_SPI_NATIVE_TARGET_H
