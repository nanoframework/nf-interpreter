//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
//
// nano_SPI_xxxx
//
// Logical SPI device access
//
#include <nanoHAL.h>
#include <targetPAL.h>

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

// Create a handle built from device type, SPI bus number and device index
#define CreateSpiHandle(spiBusIndex, deviceIndex) ((CPU_DEVICE_TYPE_SPI << 16) + (spiBusIndex << 8) + deviceIndex)

#define GetBusFromHandle(handle)    ((handle >> 8) & 0x00ff);
#define GetTypeFromHandle(handle)   ((handle >> 16) & 0x00ff);
#define GetDeviceFromHandle(handle) ((handle) & 0x00ff);

// Saved config for each available SPI bus
nanoSPI_BusConfig spiconfig[NUM_SPI_BUSES];

// Define weak functions for some optional PAL functions
// so low level implementations don't require them.
__nfweak bool CPU_SPI_Initialize(uint8_t bus, const SPI_DEVICE_CONFIGURATION &spiDeviceConfig)
{
    (void)bus;
    (void)spiDeviceConfig;

    return true;
}
__nfweak bool CPU_SPI_Uninitialize(uint8_t bus)
{
    (void)bus;
    return true;
}
__nfweak SPI_OP_STATUS CPU_SPI_OP_Status(uint8_t spi_bus, uint32_t deviceHandle)
{
    (void)spi_bus;
    (void)deviceHandle;

    return SPI_OP_STATUS::SPI_OP_COMPLETE;
}

__nfweak HRESULT CPU_SPI_Add_Device(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, uint32_t &handle)
{
    (void)spiDeviceConfig;
    handle = 1;

    return S_OK;
}

__nfweak void CPU_SPI_Wait_Busy(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION &spiDeviceConfig)
{
    (void)deviceHandle;
    (void)spiDeviceConfig;
}

__nfweak bool CPU_SPI_Remove_Device(uint32_t deviceHandle)
{
    (void)deviceHandle;
    return true;
};

//
// Number of SPI buses available
__nfweak uint32_t CPU_SPI_PortsCount()
{
    uint32_t count = 0;
    uint32_t map = CPU_SPI_PortsMap();

    while (map > 0)
    {
        if (map & 0x01)
        {
            count++;
        }
        map >>= 1;
    }

    return count;
}

// Pins not know
__nfweak void CPU_SPI_GetPins(uint32_t spi_bus, GPIO_PIN &clockPin, GPIO_PIN &misoPin, GPIO_PIN &mosiPin)
{
    (void)spi_bus;

    clockPin = GPIO_PIN_NONE;
    misoPin = GPIO_PIN_NONE;
    mosiPin = GPIO_PIN_NONE;
}

//
//  Find the Bus / Device ptr from the handle
//
//  return true = handle valid
static bool getDevice(uint32_t handle, uint8_t &spiBus, int &deviceIndex)
{
    int type = GetTypeFromHandle(handle);
    deviceIndex = GetDeviceFromHandle(handle);
    spiBus = GetBusFromHandle(handle);

    // Validate type, bus, no need to check the device index as we're managing this manually
    if (type != CPU_DEVICE_TYPE_SPI || spiBus >= NUM_SPI_BUSES)
    {
        return false;
    }

    return true;
}

// Find a free slot in the device table
// Return index or -1 if no free slots
static int FindFreeDeviceSlotSpi(int spiBus, int32_t cs)
{
    for (int deviceIndex = 0; deviceIndex < MAX_SPI_DEVICES; deviceIndex++)
    {
        if (spiconfig[spiBus].deviceHandles[deviceIndex] == 0)
        {
            return deviceIndex;
        }

        // Check device chip select not already in use
        if (spiconfig[spiBus].deviceConfig[deviceIndex].DeviceChipSelect == cs)
        {
            if (spiconfig[spiBus].deviceConfig[deviceIndex].DeviceChipSelect == cs)
            {
                return -2;
            }
        }
    }

    return -1;
}

// Initialise the spiconfig structure
// Called on CLR startup
bool nanoSPI_Initialize()
{
    for (int spiBus = 0; spiBus < NUM_SPI_BUSES; spiBus++)
    {
        spiconfig[spiBus].spiBusInited = false;
        spiconfig[spiBus].devicesInUse = 0;
        memset(&spiconfig[spiBus].deviceHandles, 0, sizeof(spiconfig[spiBus].deviceHandles));
    }

    return true;
}

// Uninitializes (resets) all SPI devices.
// Called on CLR closedown
void nanoSPI_Uninitialize()
{
    for (int spiBusIndex = 0; spiBusIndex < NUM_SPI_BUSES; spiBusIndex++)
    {
        if (spiconfig[spiBusIndex].spiBusInited)
        {
            // Remove any devices
            // Bus will be closed when last device closed in SPI_CloseDevice
            for (int deviceIndex = 0; deviceIndex < MAX_SPI_DEVICES; deviceIndex++)
            {
                uint32_t deviceHandle = spiconfig[spiBusIndex].deviceHandles[deviceIndex];

                if (deviceHandle != 0)
                {
                    nanoSPI_CloseDevice(CreateSpiHandle(spiBusIndex, deviceIndex));
                }
            }
        }
    }
}

SPI_OP_STATUS nanoSPI_Op_Status(uint32_t handle)
{
    uint8_t spiBus;
    int deviceIndex;

    getDevice(handle, spiBus, deviceIndex);

    SPI_OP_STATUS status = CPU_SPI_OP_Status(spiBus, spiconfig[spiBus].deviceHandles[deviceIndex]);

    return status;
}

// Open SPI bus / device using device configuration
// Register GPIO pins as in use
// Return handle ( index to device on bus), negative = error
HRESULT nanoSPI_OpenDevice(SPI_DEVICE_CONFIGURATION &Configuration, uint32_t &handle)
{
    return nanoSPI_OpenDeviceEx(Configuration, handle, GPIO_PIN_NONE, GPIO_PIN_NONE, GPIO_PIN_NONE);
}

// Reserve SPI bus pins
HRESULT nanoSPI_ReserveBusPins(int spiBus, bool reserve)
{
    GPIO_PIN pins[3];

    CPU_SPI_GetPins(spiBus, pins[0], pins[1], pins[2]);

    if (reserve)
    {
        // if reserve , Check can reserve
        for (int i = 0; i < 3; i++)
        {
            if (pins[0] != GPIO_PIN_NONE)
            {
                if (CPU_GPIO_PinIsBusy(pins[i]))
                {
                    return CLR_E_INVALID_PARAMETER;
                }
            }
        }
    }

    // Reserve / UnReserve pins
    for (int i = 0; i < 3; i++)
    {
        if (pins[i] != GPIO_PIN_NONE)
        {
            if (CPU_GPIO_ReservePin(pins[i], reserve) == false)
            {
                return CLR_E_INVALID_PARAMETER;
            }
        }
    }

    return S_OK;
}

// Specify alternate pins for SPI or -1 for use default for bus
HRESULT nanoSPI_OpenDeviceEx(
    SPI_DEVICE_CONFIGURATION &spiDeviceConfig,
    uint32_t &handle,
    GPIO_PIN altMsk,
    GPIO_PIN altMiso,
    GPIO_PIN altMosi)
{
    // Use alternate pins if defined
    // TODO params
    (void)altMsk;
    (void)altMiso;
    (void)altMosi;

    int deviceIndex;
    uint32_t deviceHandle;
    HRESULT hr;

    if (spiDeviceConfig.Spi_Bus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // Validate Bus available
    if (!(CPU_SPI_PortsMap() & (1 << spiDeviceConfig.Spi_Bus)))
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // Check not maximum devices per SPI bus reached
    if (spiconfig[spiDeviceConfig.Spi_Bus].devicesInUse >= MAX_SPI_DEVICES)
    {
        return CLR_E_INDEX_OUT_OF_RANGE;
    }

    // Initialise Bus if not already initialised
    if (!spiconfig[spiDeviceConfig.Spi_Bus].spiBusInited)
    {
        if (!CPU_SPI_Initialize(spiDeviceConfig.Spi_Bus, spiDeviceConfig))
        {
            return CLR_E_INVALID_PARAMETER;
        }

        // Reserve pins used by SPI bus
        hr = nanoSPI_ReserveBusPins(spiDeviceConfig.Spi_Bus, true);

        if (hr != S_OK)
        {
            return hr;
        }

        spiconfig[spiDeviceConfig.Spi_Bus].spiBusInited = true;
    }

    // Find if device slot is available and check
    deviceIndex = FindFreeDeviceSlotSpi(spiDeviceConfig.Spi_Bus, spiDeviceConfig.DeviceChipSelect);

    if (deviceIndex < 0)
    {
        if (deviceIndex == -1)
        {
            // No device slots left
            return CLR_E_INDEX_OUT_OF_RANGE;
        }
        else
        {
            // Return NOT_SUPPORTED when Device already in use. Not really any other relevant exception that's
            // currently raised in managed code
            return CLR_E_NOT_SUPPORTED;
        }
    }

    // Add device and get handle
    hr = CPU_SPI_Add_Device(spiDeviceConfig, deviceHandle);

    if (hr != S_OK)
    {
        spiconfig[spiDeviceConfig.Spi_Bus].deviceHandles[deviceIndex] = 0;
        spiconfig[spiDeviceConfig.Spi_Bus].devicesInUse--;

        // Unreserve CS pin
        if (spiconfig[spiDeviceConfig.Spi_Bus].deviceConfig[deviceIndex].DeviceChipSelect >= 0)
        {
            CPU_GPIO_ReservePin(spiconfig[spiDeviceConfig.Spi_Bus].deviceConfig->DeviceChipSelect, false);
        }

        // Last device on bus then close bus and also remove bus pin reserves
        if (spiconfig[spiDeviceConfig.Spi_Bus].devicesInUse <= 0)
        {
            // Uninitialise bus and reset init flag
            CPU_SPI_Uninitialize(spiDeviceConfig.Spi_Bus);
            spiconfig[spiDeviceConfig.Spi_Bus].spiBusInited = false;

            // Unreserve bus pins
            nanoSPI_ReserveBusPins(spiDeviceConfig.Spi_Bus, false);
        }

        // free SPI device, if we ever got a valid handle
        if (deviceHandle != 0)
        {
            CPU_SPI_Remove_Device(deviceHandle);
        }

        return hr;
    }

    if (deviceHandle != 0 && spiDeviceConfig.DeviceChipSelect >= 0)
    {
        // Reserve chip select pin
        if (CPU_GPIO_ReservePin((GPIO_PIN)spiDeviceConfig.DeviceChipSelect, true) == false)
        {
            // Failed to reserve CS pin
            CPU_SPI_Remove_Device(deviceHandle);

            return CLR_E_FAIL;
        }

        // Set CS pin to output and inactive
        CPU_GPIO_EnableOutputPin(
            (GPIO_PIN)spiDeviceConfig.DeviceChipSelect,
            (GpioPinValue)!spiDeviceConfig.ChipSelectActiveState,
            PinMode_Output);
    }

    // Compute rough estimate on the time to tx/rx a byte (in milliseconds)
    // Used to compute length of time for each IO to see if this is a long running operation
    // Store for each device as each device could use a different bit rate
    spiDeviceConfig.ByteTime = (1.0 / spiDeviceConfig.Clock_RateHz) * 1000.0 * 8;

    // Add next Device - Copy device config, save handle, increment number devices on bus
    nanoSPI_BusConfig *pBusConfig = &spiconfig[spiDeviceConfig.Spi_Bus];
    pBusConfig->deviceConfig[deviceIndex] = spiDeviceConfig;
    pBusConfig->deviceHandles[deviceIndex] = deviceHandle;

    pBusConfig->devicesInUse++;

    // Return unique generated device handle
    handle = CreateSpiHandle(spiDeviceConfig.Spi_Bus, deviceIndex);

    // perform dummy SPI transaction to ensure that the device is ready and leaving signals in a known state
    SPI_WRITE_READ_SETTINGS wrc{
        false,
        0,
        spiDeviceConfig.DataIs16bits,
        NULL,
        spiDeviceConfig.DeviceChipSelect,
        spiDeviceConfig.ChipSelectActiveState};
    uint8_t buffer[2] = {0, 0};
    nanoSPI_Write_Read(handle, wrc, buffer, 2, buffer, 2);

    return S_OK;
}

//  SPI_CloseDevice
//  SPI_CloseDevice
//
//  Close device on SPI bus
//
HRESULT nanoSPI_CloseDevice(uint32_t handle)
{
    uint8_t spiBus;
    int deviceIndex;

    if (!getDevice(handle, spiBus, deviceIndex))
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // Wait until device is not busy
    CPU_SPI_Wait_Busy(spiconfig[spiBus].deviceHandles[deviceIndex], spiconfig[spiBus].deviceConfig[deviceIndex]);

    // Remove device from bus (ignore any error )
    CPU_SPI_Remove_Device(spiconfig[spiBus].deviceHandles[deviceIndex]);

    spiconfig[spiBus].deviceHandles[deviceIndex] = 0;
    spiconfig[spiBus].devicesInUse--;

    // Unreserve CS pin
    if (spiconfig[spiBus].deviceConfig[deviceIndex].DeviceChipSelect >= 0)
    {
        CPU_GPIO_ReservePin(spiconfig[spiBus].deviceConfig[deviceIndex].DeviceChipSelect, false);
    }

    // Last device on bus then close bus and also remove bus pin reserves
    if (spiconfig[spiBus].devicesInUse <= 0)
    {
        // Uninitialise bus and reset init flag
        CPU_SPI_Uninitialize(spiBus);
        spiconfig[spiBus].spiBusInited = false;

        // Unreserve bus pins
        nanoSPI_ReserveBusPins(spiBus, false);
    }

    return S_OK;
}

//
// Get time to send 1 byte on bus using current config
//
float nanoSPI_GetByteTime(uint32_t handle)
{
    uint8_t spiBus;
    int deviceIndex;

    getDevice(handle, spiBus, deviceIndex);

    return spiconfig[spiBus].deviceConfig->ByteTime;
}

void nanoSPI_Wait_Busy(uint32_t handle)
{
    uint8_t spiBus;
    int deviceIndex;

    getDevice(handle, spiBus, deviceIndex);

    CPU_SPI_Wait_Busy(spiconfig[spiBus].deviceHandles[deviceIndex], spiconfig[spiBus].deviceConfig[deviceIndex]);
}

//
// Write and/or read data to device on SPi bus
//
HRESULT nanoSPI_Write_Read(
    uint32_t handle,
    SPI_WRITE_READ_SETTINGS &swrs,
    uint8_t *writePtr,
    int32_t wsize,
    uint8_t *readPtr,
    int32_t readSize)
{
    uint8_t spiBus;
    int deviceIndex;

    if (!getDevice(handle, spiBus, deviceIndex))
    {
        return CLR_E_INVALID_PARAMETER;
    }

    return CPU_SPI_nWrite_nRead(
        spiconfig[spiBus].deviceHandles[deviceIndex],
        spiconfig[spiBus].deviceConfig[deviceIndex],
        swrs,
        writePtr,
        wsize,
        readPtr,
        readSize);
}

//
// Get the SPI handle from the device handle
//
uint32_t CPU_SPI_GetSpiHandle(uint32_t deviceHandle)
{
    uint8_t spiBus;
    int deviceIndex;

    if (!getDevice(deviceHandle, spiBus, deviceIndex))
    {
        return 0;
    }

    return spiconfig[spiBus].deviceHandles[deviceIndex];
}
