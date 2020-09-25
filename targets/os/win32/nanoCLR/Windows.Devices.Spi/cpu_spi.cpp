//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
//
// CPU_SPI_xxxx
//
// Read / Write physical SPI bus
//

#include <targetHAL.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>


// Remove device from bus 
// return true of OK, false = error
bool CPU_SPI_Remove_Device(uint32_t deviceHandle)
{
    return spi_bus_remove_device((spi_device_handle_t)deviceHandle) != ESP_OK;
}
// Performs a read/write operation on 8-bit word data. 
//
// Parameters
//  deviceHandle
//      Device handle from add_device
//  sdev
//		reference to SPI_DEVICE_CONFIGURATION
//  wrc
//		reference to SPI_WRITE_READ_SETTINGS 
//  writeData
//      A pointer to the buffer from which the data is to be written to the device.
//  writeSize
//      The number of elements(8 or 16) to be written.
//  readData
//      A pointer to the buffer into which the data is to be read from the device.
//  readSize
//      The number of elements(8 or 16) to be read.
//
// return S_OK=Successful, Async started=CLR_BUSY, Error=CLR_E_OUT_OF_MEMORY, CLR_E_INVALID_PARAMETER, CLR_E_FAIL
//
HRESULT CPU_SPI_nWrite_nRead(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION & sdev, SPI_WRITE_READ_SETTINGS & wrc, uint8_t* writeData, int32_t writeSize, uint8_t* readData, int32_t readSize)
{
    (void)deviceHandle;
    (void)sdev;
    (void)wrc;
    (void)writeData;
    (void)writeSize;
    (void)readData;
    (void)readSize;

    return CLR_E_FAIL;
}

// Performs a read/write operation on 16-bit word data. 
HRESULT CPU_SPI_nWrite16_nRead16(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION & sdev, SPI_WRITE_READ_SETTINGS & swrs, uint16_t * writePtr, int32_t writeSize, uint16_t * readPtr, int32_t readSize)
{
    swrs.Bits16ReadWrite = true;
    return CPU_SPI_nWrite_nRead(deviceHandle, sdev, swrs, (uint8_t *)writePtr, writeSize, (uint8_t *)readPtr, readSize);
}

// Return map of available SPI buses as a bit map
uint32_t CPU_SPI_PortsMap()
{
    return 0x00;
}

// Returns the SPI clock, MISO and MOSI pin numbers for a specified SPI module. 
void   CPU_SPI_GetPins( uint32_t spi_bus, GPIO_PIN& clockPin, GPIO_PIN& misoPin, GPIO_PIN& mosiPin )
{
    clockPin = -1;
    misoPin  = -1;
    mosiPin  = -1;
}

// Return SPI minimum clock frequency
uint32_t CPU_SPI_MinClockFrequency( uint32_t spi_bus )
{
    (void)spi_bus;

    return 0;
}

// Return SPI maximum clock frequency

uint32_t CPU_SPI_MaxClockFrequency( uint32_t spi_bus )
{
    return 1000000;
}

//
// Return the number of chip select lines available on the bus.
uint32_t CPU_SPI_ChipSelectLineCount(uint32_t spi_bus)
{
    (void)spi_bus;

    return 5;
}