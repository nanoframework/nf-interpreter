//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_i2c_native.h"

//////////
// I2C1 //
//////////

// buffers size
// tx buffer size (in bytes)
#define I2C1_WR_SIZE  100
// rx buffer size (in bytes)
#define I2C1_RD_SIZE  100

// buffers declaration
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t I2C1_WriteBuffer[I2C1_WR_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t I2C1_ReadBuffer[I2C1_RD_SIZE];

// initialization for I2C1
I2C_INIT(1, I2C1_WR_SIZE, I2C1_RD_SIZE)

// un-initialization for I2C1
I2C_UNINIT(1)
