//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef DRIVERS_SPI_DECL_H
#define DRIVERS_SPI_DECL_H

#define SPI_CTRL_DEFAULT 0

// Callback when operation complete on bus in async operation
typedef void (*SPI_Callback)(int busIndex);

///////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Device.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////
typedef enum __nfpack DataBitOrder
{
    DataBitOrder_MSB = 0,
    DataBitOrder_LSB = 1
} DataBitOrder;

////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Device.Spi.SpiConfiguration (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////

typedef enum __nfpack SpiBusConfiguration
{
    SpiBusConfiguration_FullDuplex = 0,
    SpiBusConfiguration_HalfDuplex = 1,
    SpiBusConfiguration_Simplex = 2,
} SpiBusConfiguration;

///////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Device.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////

// SPI mode
typedef enum __nfpack SpiMode
{
    SpiMode_Mode0 = 0,
    SpiMode_Mode1 = 1,
    SpiMode_Mode2 = 2,
    SpiMode_Mode3 = 3
} SpiMode;

typedef enum __nfpack SpiBusMode
{
    SpiBusMode_slave = 0,
    SpiBusMode_master = 1
} SpiBusMode;

// Async operation status
typedef enum __nfpack SPI_OP_STATUS
{
    SPI_OP_READY,   // Bus ready for new operation
    SPI_OP_RUNNING, // operation still running
    SPI_OP_COMPLETE // Last operation complete
} SPI_OP_STATUS;

struct SPI_DEVICE_CONFIGURATION
{
    // SPi bus thats connected to device
    uint32_t Spi_Bus;
    // Slave or master(default)
    SpiBusMode BusMode;
    // GPIO pin used for device Chip select, if -1 it means the ChipSelect is handled manually
    int32_t DeviceChipSelect;
    // False = LOW active,      True = HIGH active
    bool ChipSelectActiveState;
    // SPI mode 0 -> 3
    SpiMode Spi_Mode;
    // SPI bus Configuration (full-duplex is default)
    SpiBusConfiguration BusConfiguration;
    // True = SPI data takes the form of 16-bit words otherwise 8-bit words.
    bool DataIs16bits;
    // Data order for 16 bit operation
    DataBitOrder DataOrder16;
    // Rough estimate on the time it takes to send/receive one byte (in milliseconds)
    // Used to compute length of time for each IO to see if this is a long running operation
    float ByteTime;

    // Master Only
    uint32_t Clock_RateHz;   // Master - SPI bus clock frequency, in hertz (Hz).
    uint32_t CS_Setup_uSecs; // Master -  Not sure if we need these !!
    uint32_t CS_Hold_uSecs;  // Master -
};

struct SPI_WRITE_READ_SETTINGS
{
    bool fullDuplex;       // Full duplex opertaion
    int readOffset;        // Read offset on half duplex read ( from end of write )
    bool Bits16ReadWrite;  // True if a 16bit operation
    SPI_Callback callback; // NUll is operation is Synchronous
    // GPIO pin used for device Chip select, if -1 it means the ChipSelect is handled manually
    int32_t DeviceChipSelect;
    // False = LOW active,      True = HIGH active
    bool ChipSelectActiveState;
};

#define CPU_SPI_ERROR_PARAM   -1
#define CPU_SPI_ERROR_NOMEM   -2
#define CPU_SPI_ERROR_FAIL    -3
#define CPU_SPI_ERROR_TIMEOUT -4

// HAL SPi functions (porting layer)

// Initialise an SPI bus, called before any devices opened (optional)
bool CPU_SPI_Initialize(uint8_t bus, const SPI_DEVICE_CONFIGURATION &spiDeviceConfig);

// Unintialise spi bus, called after last device removed (optional)
bool CPU_SPI_Uninitialize(uint8_t bus);

// Return status of current SPI operation
// Used to find status of an Async SPI call
SPI_OP_STATUS CPU_SPI_OP_Status(uint8_t busIndex, uint32_t deviceHandle);

// Add a device to SPi Bus (Optional)
// Returns a device handle in the handle pointer.
// deviceHandle is a reference to underlying OS handle/address of device. If not required then return value other than
// 0. returns S_OK on success, else S_FALSE on failure.
HRESULT CPU_SPI_Add_Device(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, uint32_t &handle);

// Remove device from bus (Optional)
// return true is successful
bool CPU_SPI_Remove_Device(uint32_t deviceHandle);

// Write / read 8 bit data to device specified by handle
// return result 0=S_OK, CLR_E_BUSY async operation and operation still running or another error code
HRESULT CPU_SPI_nWrite_nRead(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &swrs,
    uint8_t *writePtr,
    int32_t writeSize,
    uint8_t *readPtr,
    int32_t readSize);

void CPU_SPI_Wait_Busy(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION &sdev);

// Write / read 16 bit data to device specified by handle
// return result 0=S_OK, CLR_E_BUSY async operation and operation still running or another error code
HRESULT CPU_SPI_nWrite16_nRead16(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &swrs,
    uint16_t *writePtr,
    int32_t writeSize,
    uint16_t *readPtr,
    int32_t readSize);

// Return bit map of available SPI buses
// bit 0 - SPI1
// bit 1 - SPI2
// bit 2 - SPI3
// etc
uint32_t CPU_SPI_PortsMap();

// Total number of SPI buses available
uint32_t CPU_SPI_PortsCount();

// Return pins used for SPI bus
// return -1 if not known
void CPU_SPI_GetPins(uint32_t busIndex, GPIO_PIN &clk, GPIO_PIN &miso, GPIO_PIN &mosi);

// Minimum and Maximum clock frequency available based on bus and configured pins
uint32_t CPU_SPI_ChipSelectLineCount(uint32_t busIndex);
HRESULT CPU_SPI_MinClockFrequency(uint32_t busIndex, int32_t *frequency);
HRESULT CPU_SPI_MaxClockFrequency(uint32_t busIndex, int32_t *frequency);

// return the SPI hande for the SPI device (platform specific)
uint32_t CPU_SPI_GetSpiHandle(uint32_t deviceHandle);

#endif // DRIVERS_SPI_DECL_H
