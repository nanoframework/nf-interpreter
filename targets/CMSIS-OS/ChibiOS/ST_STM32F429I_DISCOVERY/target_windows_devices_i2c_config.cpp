//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_i2c_native.h"

//////////
// I2C3 //
//////////

// buffers size
// write buffer size (in bytes)
#define I2C3_WR_SIZE  100
// read buffer size (in bytes)
#define I2C3_RD_SIZE  100

// buffers declaration
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t I2C3_WriteBuffer[I2C3_WR_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t I2C3_ReadBuffer[I2C3_RD_SIZE];

// initialization of I2C3 PAL structure
I2C_INIT(3, I2C3_WR_SIZE, I2C3_RD_SIZE)

// un-initialization of I2C3 PAL structure
I2C_UNINIT(3)
