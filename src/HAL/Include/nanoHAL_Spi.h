//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_SPI_H_
#define _NANOHAL_SPI_H_ 1

#include "TargetHAL_Spi.h"

#include "CPU_SPI_decl.h"

// Global SPI handle type
#define CPU_DEVICE_TYPE_SPI 1

struct nanoSPI_BusConfig
{
    bool spiBusInited;
    int8_t devicesInUse;
    SPI_DEVICE_CONFIGURATION deviceCongfig[MAX_SPI_DEVICES];
    uint32_t deviceHandles[MAX_SPI_DEVICES];
    float byteTime[MAX_SPI_DEVICES];
    SPI_OP_STATUS spiStatus;
};

// Called on CLR startup
bool nanoSPI_Initialize();

// Called on CLR closedown
void nanoSPI_Uninitialize();

// HAL Access layer

// Open SPI bus / device using device configuration
// Register GPIO pins as in use
// Return result, if S_OK then handle returned in handle
HRESULT nanoSPI_OpenDevice(SPI_DEVICE_CONFIGURATION &Configuration, uint32_t &handle);

// Specify alternate pins or -1 for default, Needed ?
// only possible of first device open
HRESULT nanoSPI_OpenDeviceEx(
    SPI_DEVICE_CONFIGURATION &Configuration,
    uint32_t &handle,
    GPIO_PIN altMsk,
    GPIO_PIN altMiso,
    GPIO_PIN altMosi);

// Close SPI device
// When last device closed bus is closed
HRESULT nanoSPI_CloseDevice(uint32_t handle);

// Return the time in ms for a byte transfer
float nanoSPI_GetByteTime(uint32_t handle);

// Execute a SPI write/read operation
// if callback is null the operation will completed as part of the call
// if callback is not null then the job will be queued and the callback called when completed
// return status of operation
//   >0 = operation Id
//   0=ok completed (null callback)
//  -1 = SPI error  (null callback)
//  -2 = Unable to queue operation( queue full )
HRESULT nanoSPI_Write_Read(
    uint32_t handle,
    SPI_WRITE_READ_SETTINGS &settings,
    uint8_t *writeData,
    int32_t writeSize,
    uint8_t *readData,
    int32_t readSize);

// Return status of last async operation
SPI_OP_STATUS nanoSPI_Op_Status(uint32_t handle);

// Start a read / write spi job
// result = -1 Busy  ( wait for bus to be released )
// callback called on job completion
HRESULT nanoSPI_start_nWrite_nRead(
    uint32_t handle,
    SPI_WRITE_READ_SETTINGS &settings,
    uint8_t *writeData,
    int32_t writeSize,
    uint8_t *readData,
    int32_t readSize);

#endif // _NANOHAL_SPI_H_
