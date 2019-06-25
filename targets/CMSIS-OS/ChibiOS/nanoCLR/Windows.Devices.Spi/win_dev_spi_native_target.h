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

#endif  //_WIN_DEV_SPI_NATIVE_TARGET_H_
