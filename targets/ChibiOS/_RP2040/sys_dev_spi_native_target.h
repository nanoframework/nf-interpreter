//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_SPI_NATIVE_TARGET_H
#define SYS_DEV_SPI_NATIVE_TARGET_H

#include <sys_dev_spi_native.h>
#include <hal.h>

// struct representing the SPI bus
struct NF_PAL_SPI
{
    int BusIndex;
    SPIDriver *Driver;
    SPIConfig Configuration;
    SpiBusConfiguration BusConfiguration;

    SPI_Callback Callback;

    bool SequentialTxRx;
    bool BufferIs16bits;

    uint8_t *WriteBuffer;
    uint16_t WriteSize;

    uint8_t *ReadBuffer;
    uint16_t ReadSize;

    // -1 = Chip Select is not handled | >0 Chip Select is to be controlled with this GPIO
    int32_t ChipSelect;
};

// RP2040 SPI pin configuration macro
// RP2040 has flexible pin muxing — SPI function is always FUNCSEL=1
// Configure SCK, MOSI, MISO pins with SPI alternate function and CS as GPIO output
#define SPI_CONFIG_PINS(num, sck_pin, miso_pin, mosi_pin)                                                              \
    void ConfigPins_SPI##num(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig)                                          \
    {                                                                                                                  \
        palSetPadMode(IOPORT1, sck_pin, PAL_MODE_ALTERNATE_SPI);                                                       \
        palSetPadMode(IOPORT1, mosi_pin, PAL_MODE_ALTERNATE_SPI);                                                      \
        if (spiDeviceConfig.BusConfiguration != SpiBusConfiguration_HalfDuplex)                                        \
        {                                                                                                              \
            palSetPadMode(IOPORT1, miso_pin, PAL_MODE_ALTERNATE_SPI);                                                  \
        }                                                                                                              \
        if (spiDeviceConfig.DeviceChipSelect >= 0)                                                                     \
        {                                                                                                              \
            palSetPadMode(IOPORT1, spiDeviceConfig.DeviceChipSelect, PAL_MODE_OUTPUT_PUSHPULL);                        \
            if (spiDeviceConfig.ChipSelectActiveState)                                                                 \
            {                                                                                                          \
                palSetPad(IOPORT1, spiDeviceConfig.DeviceChipSelect);                                                  \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                palClearPad(IOPORT1, spiDeviceConfig.DeviceChipSelect);                                                \
            }                                                                                                          \
        }                                                                                                              \
    }

// RP2040 has 2 SPI buses (SPI0, SPI1)
void ConfigPins_SPI0(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig);
void ConfigPins_SPI1(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig);

#endif // SYS_DEV_SPI_NATIVE_TARGET_H
