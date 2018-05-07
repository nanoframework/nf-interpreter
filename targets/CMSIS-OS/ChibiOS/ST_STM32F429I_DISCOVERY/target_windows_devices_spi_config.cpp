//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native.h"

//////////
// SPI5 //
//////////

// buffers size
// write buffer size (in bytes)
#define SPI5_WR_SIZE  812
// read buffer size (in bytes)
#define SPI5_RD_SIZE  100

// buffers declaration
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI5_WriteBuffer[SPI5_WR_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI5_ReadBuffer[SPI5_RD_SIZE];

// initialization of SPI5 PAL structure
SPI_INIT(5, SPI5_WR_SIZE, SPI5_RD_SIZE)

// un-initialization of SPI5 PAL structure
SPI_UNINIT(5)
