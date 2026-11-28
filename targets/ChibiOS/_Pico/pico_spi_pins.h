//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

#if defined(NF_CHIBIOS_PICO_SDCARD) && NF_CHIBIOS_PICO_SDCARD
#define PICO_SPI_CONFIG_PINS(num, sck, miso_pin, mosi_pin)                                                             \
    SPI_CONFIG_PINS(num, sck, miso_pin, mosi_pin)                                                                      \
    void GetPins_SPI##num(GPIO_PIN &clk, GPIO_PIN &miso, GPIO_PIN &mosi)                                               \
    {                                                                                                                  \
        clk = sck;                                                                                                     \
        miso = miso_pin;                                                                                               \
        mosi = mosi_pin;                                                                                               \
    }
#else
#define PICO_SPI_CONFIG_PINS(num, sck, miso, mosi) SPI_CONFIG_PINS(num, sck, miso, mosi)
#endif
