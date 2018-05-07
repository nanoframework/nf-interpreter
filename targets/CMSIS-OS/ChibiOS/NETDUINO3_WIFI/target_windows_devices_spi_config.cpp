//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native.h"

//////////
// SPI1 //
//////////

// buffers size
// write buffer size (in bytes)
#define SPI1_WR_SIZE  100
// read buffer size (in bytes)
#define SPI1_RD_SIZE  100

// buffers declaration
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI1_WriteBuffer[SPI1_WR_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI1_ReadBuffer[SPI1_RD_SIZE];

// initialization of SPI1 PAL structure
SPI_INIT(1, SPI1_WR_SIZE, SPI1_RD_SIZE)

// un-initialization of SPI1 PAL structure
SPI_UNINIT(1)

//////////
// SPI2 //
//////////

// buffers size
// write buffer size (in bytes)
#define SPI2_WR_SIZE  100
// read buffer size (in bytes)
#define SPI2_RD_SIZE  100

// buffers declaration
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI2_WriteBuffer[SPI2_WR_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI2_ReadBuffer[SPI2_RD_SIZE];

// initialization of SPI2 PAL structure
SPI_INIT(2, SPI2_WR_SIZE, SPI2_RD_SIZE)

// un-initialization of SPI2 PAL structure
SPI_UNINIT(2)

//////////
// SPI3 //
//////////

// buffers size
// write buffer size (in bytes)
#define SPI3_WR_SIZE  100
// read buffer size (in bytes)
#define SPI3_RD_SIZE  100

// buffers declaration
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI3_WriteBuffer[SPI3_WR_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI3_ReadBuffer[SPI3_RD_SIZE];

// initialization of SPI3 PAL structure
SPI_INIT(3, SPI3_WR_SIZE, SPI3_RD_SIZE)

// un-initialization of SPI3 PAL structure
SPI_UNINIT(3)

//////////
// SPI4 //
//////////

// buffers size
// write buffer size (in bytes)
#define SPI4_WR_SIZE  100
// read buffer size (in bytes)
#define SPI4_RD_SIZE  100

// buffers declaration
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI4_WriteBuffer[SPI4_WR_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t SPI4_ReadBuffer[SPI4_RD_SIZE];

// initialization of SPI4 PAL structure
SPI_INIT(4, SPI4_WR_SIZE, SPI4_RD_SIZE)

// un-initialization of SPI4 PAL structure
SPI_UNINIT(4)
