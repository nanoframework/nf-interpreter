//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_DEV_SPI_NATIVE_TARGET_H_
#define _WIN_DEV_SPI_NATIVE_TARGET_H_

#include <win_dev_spi_native.h>
#include <Esp32_os.h>

// # buses but first can't be used as SPI flash
#define NUM_SPI_BUSES   3
#define MAX_SPI_DEVICES 3

// Max clock frequency is 20 MHz
#define MAX_CLOCK_FREQUENCY 20000000

struct nfSpiBusConfig
{
    bool                spiBusInited;
    int                 deviceId[MAX_SPI_DEVICES];            // Used to find correct device in config
    spi_device_handle_t deviceHandles[MAX_SPI_DEVICES];       // Max 3 devices per bus (IDF limit )
};

#endif  //_WIN_DEV_SPI_NATIVE_TARGET_H_
