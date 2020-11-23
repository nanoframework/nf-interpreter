//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <targetPAL.h>

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

#define NUM_I2C_BUSES 2
#define MAX_I2C_DEVICES 5


// Create a handle built from device type, I2C bus number and device index
#define CreateI2cHandle(i2cBusIndex, deviceIndex) ((CPU_DEVICE_TYPE_I2C << 16) + (i2cBusIndex << 8) + deviceIndex)
#define GetBusFromHandle(handle) ((handle >> 8) & 0x00ff)
#define GetDeviceIndexFromHandle(handle)  (handle & 0x00f)


struct nanoI2cConfig
{
    bool busInited;
    uint8_t devicesInUse;
    uint8_t slaveAddress[MAX_I2C_DEVICES];
};

nanoI2cConfig i2cConfig[NUM_I2C_BUSES];

// Initialise the i2cConfig structure
// Called on CLR startup
bool nanoI2C_Initialize()
{
    for (int i2cBusIndex = 0; i2cBusIndex < NUM_I2C_BUSES; i2cBusIndex++)
    {
        i2cConfig[i2cBusIndex].busInited = false;
        i2cConfig[i2cBusIndex].devicesInUse = 0;
        memset(&i2cConfig[i2cBusIndex].slaveAddress, 0, sizeof(i2cConfig[i2cBusIndex].slaveAddress));
    }
    return true;
}

// Uninitializes (resets) all SPI devices.
// Called on CLR closedown
void nanoI2C_Uninitialize()
{
    for (int i2cBusIndex = 0; i2cBusIndex < NUM_I2C_BUSES; i2cBusIndex++)
    {
        if (i2cConfig[i2cBusIndex].busInited)
        {
            // Remove any devices
            // Bus will be closed when last device closed in SPI_CloseDevice
            for (int deviceIndex = 0; deviceIndex < MAX_I2C_DEVICES; deviceIndex++)
            {
                uint8_t address = i2cConfig[i2cBusIndex].slaveAddress[deviceIndex];
                if (address != 0)
                    nanoI2C_CloseDevice(CreateI2cHandle(i2cBusIndex, deviceIndex));
            }
        }
    }
}

// Reserve SPI bus pins
HRESULT nanoI2C_ReserveBusPins(int i2cBus, bool reserve)
{
    GPIO_PIN pins[2];

    CPU_I2C_GetPins(i2cBus, pins[0], pins[1]);

    if (reserve)
    {
        // if reserve , Check can reserve
        for (int i = 0; i < 2; i++)
        {
            if (pins[0] != GPIO_PIN_NONE)
            {
                if (CPU_GPIO_PinIsBusy(pins[i]))
                    return CLR_E_INVALID_PARAMETER;
            }
        }
    }

    // Reserve / UnReserve pins
    for (int i = 0; i < 2; i++)
    {
        if (pins[i] != GPIO_PIN_NONE)
        {
            if (CPU_GPIO_ReservePin(pins[i], reserve) == false)
                return CLR_E_INVALID_PARAMETER;
        }
    }

    return S_OK;
}

// Find a free slot in the device table
// Return index or -1 if no free slots
static int FindFreeDeviceSlotI2C(int i2CBus, int slaveAddress)
{
    for (int deviceIndex = 0; deviceIndex < MAX_I2C_DEVICES; deviceIndex++)
    {
        uint8_t adr = i2cConfig[i2CBus].slaveAddress[deviceIndex];
        // Free slot ?
        if (adr == 0)
        {
            return deviceIndex;
        }
        // Already in opened
        if (adr == slaveAddress)
        {
            return -1;
        }
    }
    return -1;
}

// Open I2C device on bus
//
HRESULT nanoI2C_OpenDevice(I2C_DEVICE_CONFIGURATION &config, uint32_t &deviceHandle)
{
    (void)config;
    (void)deviceHandle;
    
    // i2cBus 0 to (number of buses - 1)
    uint8_t i2cBusIndex = (uint8_t)config.i2cBus - 1;
    if (i2cBusIndex >= NUM_I2C_BUSES)
        return CLR_E_INVALID_PARAMETER;

       // Initialise Bus if not already initialised
    if (!i2cConfig[i2cBusIndex].busInited)
    {
        if (!CPU_I2C_Initialize(i2cBusIndex, config.fastMode))
            return CLR_E_INVALID_PARAMETER;

        // Reserve pins used by SPI bus
        HRESULT hr = nanoI2C_ReserveBusPins(i2cBusIndex, true);
        if (hr != S_OK)
            return hr;

        i2cConfig[i2cBusIndex].busInited = true;
    }

    int deviceIndex = FindFreeDeviceSlotI2C(i2cBusIndex, config.address);
    if (deviceIndex < 0)
    {
        if (deviceIndex == -1)
            // No device slots left
            return CLR_E_INDEX_OUT_OF_RANGE;
        else
            // Return NOT_SUPPORTED when Device already in use. Not really any other relevant exception that's
            // currently raised in managed code
            return CLR_E_NOT_SUPPORTED;
    }

    // Return device hanlde
    deviceHandle = CreateI2cHandle(i2cBusIndex, deviceIndex);
    
    i2cConfig[i2cBusIndex].slaveAddress[deviceIndex] = config.address;
                           
    return S_OK;
}
    
I2cTransferStatus nanoI2C_WriteRead(
    uint32_t deviceHandle,
    uint8_t *writeBuffer,
    uint16_t writeSize,
    uint8_t *readBuffer,
    uint16_t readSize)
{
    int i2cBusIndex = GetBusFromHandle(deviceHandle);
    int deviceIndex = GetDeviceIndexFromHandle(deviceHandle);

    return CPU_I2C_WriteRead(
        i2cBusIndex,
        i2cConfig[i2cBusIndex].slaveAddress[deviceIndex],
        writeBuffer,
        writeSize,
        readBuffer,
        readSize);
}
    
HRESULT nanoI2C_CloseDevice(uint32_t deviceHandle)
{
    int i2cBusIndex = GetBusFromHandle(deviceHandle);
    int deviceIndex = GetDeviceIndexFromHandle(deviceHandle);
    
    i2cConfig[i2cBusIndex].slaveAddress[deviceIndex] = 0;
    i2cConfig[i2cBusIndex].devicesInUse--;
    if (i2cConfig[i2cBusIndex].devicesInUse <=0)
    {
        CPU_I2C_Uninitialize(i2cBusIndex);
        i2cConfig[i2cBusIndex].busInited = false;
    }
    
    return S_OK;
}
    
