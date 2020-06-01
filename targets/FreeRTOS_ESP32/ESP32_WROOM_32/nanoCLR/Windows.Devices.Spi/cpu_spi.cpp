//
// Copyright (c) 2020 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
//
// CPU_SPI_xxxx
//
// Read / Write physical SPI bus
//

// Esp32 has 4 SPI devices called SPI0, SPI1, HSPI and VSPI
//  SPIO, SPI1 are dedicated entirely to flash
//
//  HSPI("SPI1") and VSPI("SPI2") are free to use
//
//  For maximum speed access the IOMUX must be used ( 80mhz )
//  if pins are through the GPIO matrix the speed is limited to 26.6mhz ( 40mhz half duplex and no dma )
//
//  SPI IOMUX ( direct pins )
//  GPIO	Function
//   2      HSPIWP  (WP)
//   4      HSPIHD  (HD)
//  12      HSPIQ   (miso) *
//  13		HSPID   (mosi) *
//	14		HSPICLK (clockPin) *
//	15		HSPICS0 (cs)
//
//   5    	VSPICS0 (cs)
//	18		VSPICLK (clockPin) *
//  19      VSPIQ   (miso) *
//  21		VSPIHD  (HD)
//	22		VSPIWP  (WP)
//	23		VSPID   (mosi) *
//
//  The defaults pins for SPI1 are the Wrover SPI display pins
//  mosiPin = GPIO_NUM_23, misoPin = GPIO_NUM_25, clockPin = GPIO_NUM_19    
//
//  SPI2 - The pins are not assigned so need to be assigned.

#include <targetHAL.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

#include <Esp32_os.h>
#include "Esp32_DeviceMapping.h"


// Max frequency over GPIO mux pins, full duplex
#define MAX_CLOCK_FREQUENCY_GPIO_FULL 26000000
// Max frequency over GPIO mux pins, half duplex
#define MAX_CLOCK_FREQUENCY_GPIO_HALF 40000000
// Max over native spi pins
#define MAX_CLOCK_FREQUENCY_NATIVE 80000000


// Tag for ESP32 logging
static const char* TAG = "SpiDevice";



// Remove device from bus 
// return true of OK, false = error
bool CPU_SPI_Remove_Device(uint32_t deviceHandle)
{
    return spi_bus_remove_device((spi_device_handle_t)deviceHandle) != ESP_OK;
}

// Initialise the physical SPI bus
// return true of successful, false if error
bool   CPU_SPI_Initialize(uint8_t spiBus)
{
    GPIO_PIN clockPin, misoPin, mosiPin;

    // Get pins used by spi bus
    CPU_SPI_GetPins(spiBus, clockPin, misoPin, mosiPin);

    // Check pins have been configured
    if (clockPin == GPIO_PIN_NONE || misoPin == GPIO_PIN_NONE || mosiPin == GPIO_PIN_NONE)
    {
        ESP_LOGE(TAG, "Spi pins for SPI%d not configured", spiBus);
        return false;
    }

    spi_bus_config_t bus_config
    {
        mosi_io_num:  (int)mosiPin,		// mosi pin
        miso_io_num : (int)misoPin,      // miso pin
        sclk_io_num : (int)clockPin,     // Clock
        quadwp_io_num : -1,         // Quad Write protect
        quadhd_io_num : -1,         // Quad Hold
        max_transfer_sz : 0,        // Default max transfer size ( 4096 )
        flags : 0,					// SPICOMMON_BUSFLAG_* flags
        intr_flags : 0				// Interrupt flags
    };

    // First available bus on ESP32 is HSPI_HOST(1)
    // Try with DMA first
    esp_err_t ret = spi_bus_initialize((spi_host_device_t)(spiBus + HSPI_HOST), &bus_config, spiBus + HSPI_HOST);
    if (ret != ESP_OK)
    {
        // Try again without DMA
        esp_err_t ret = spi_bus_initialize((spi_host_device_t)(spiBus + HSPI_HOST), &bus_config, 0);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Unable to init SPI bus %d esp_err %d", spiBus, ret);
            return false;
        }
    }

    return true;
}

// Uninitialise the bus 
bool   CPU_SPI_Uninitialize(uint8_t spiBus)
{
    if (spi_bus_free((spi_host_device_t)(spiBus)) != ESP_OK)
        return false;

    return true;
}

//
// Convert from SPI_DEVICE_CONFIGURATION to spi_device_interface_config_t used for ESP33
spi_device_interface_config_t GetConfig( const SPI_DEVICE_CONFIGURATION& spiDeviceConfig )
{
    int csPin    = spiDeviceConfig.DeviceChipSelect;
    uint8_t spiMode  = spiDeviceConfig.SpiMode;
    int clockHz  = spiDeviceConfig.Clock_RateHz;
    
    // if clock frequency is unset use the maximum frequency
    if (clockHz == 0)
    {
        clockHz = CPU_SPI_MaxClockFrequency(spiDeviceConfig.Spi_Bus);
    }

    //ets_printf( "Spi config cspin:%d spiMode:%d bitorder:%d clockHz:%d\n", csPin, spiMode, bitOrder, clockHz);
    uint32_t flags = (spiDeviceConfig.DataOrder16 == DataBitOrder_LSB) ? (SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST) : 0;
    
    // Positive Chip Select for Active
    if (spiDeviceConfig.ChipSelectActive)  flags |= SPI_DEVICE_POSITIVE_CS;

    // Fill in device config    
    spi_device_interface_config_t dev_config
    {
        0,          // Command bits 
        0,          // Address bits
        0,          // Dummy bits
        spiMode,    // SPi Mode
        0,          // Duty cycle 50/50
        0,          // cs_ena_pretrans
        0,          // cs_ena_posttrans
        clockHz,    // Clock speed in Hz
        0,          // Input_delay_ns
        csPin,      // Chip select
        flags,      // SPI_DEVICE flags
        1,          // Queue size
        0,          // Callback before
        0,          // Callback after
    };

    return dev_config;
}


//
//  Add a device to a SPI bus
//  returns device handle as uint32_t 
//
uint32_t CPU_SPI_Add_Device(const SPI_DEVICE_CONFIGURATION& spiDeviceConfig)
{
    // First available bus on ESP32 is HSPI_HOST(1), so add one
    spi_device_interface_config_t dev_config = GetConfig(spiDeviceConfig);

    // Add device to bus
    spi_device_handle_t deviceHandle;

    // First available bus on ESP32 is HSPI_HOST(1), so add one
    esp_err_t ret = spi_bus_add_device((spi_host_device_t)(spiDeviceConfig.Spi_Bus + HSPI_HOST), &dev_config, &deviceHandle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Unable to init SPI device, esp_err %d", ret);
        return 0;
    }

    return (uint32_t)deviceHandle;
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
    (void)sdev;

    NANOCLR_HEADER();
    {
        unsigned char * readDataBuffer = NULL;
       // bool async = (wrc.callback != 0);
        esp_err_t ret;

        // get data bit length
        int databitLength = wrc.Bits16ReadWrite ? 16 : 8;

        // // Are we using SPI full-duplex for transfer ?
        // // The SPI doesn't support half duplex operation with DMA and without DMA the receive length is limited to 4 bytes
        // // So we run in full duplex mode all the time and simulate half duplex mode by creating a larger receive buffer for
        // // the whole of the transaction and then copying the rx part to original buffer
        // // For full duplex we just use passed buffers.
        int  MaxElementlength;
        if (wrc.fullDuplex)
        {
            // for full duplex just use largest element
            // TODO : There may be a problem with read data overwritting passed read buff if write size > read size  !!!!!!
            //        May for this case we should always allocate a separate buffer for transafer
            MaxElementlength = (readSize > writeSize) ? readSize : writeSize;

            readDataBuffer = readData;
        }
        else
        {
            // for Halfduplex its the length of both items
            MaxElementlength = writeSize + readSize;
            int MaxDatalength = MaxElementlength;
            if (wrc.Bits16ReadWrite) MaxDatalength *= 2;

            if (readSize)
            {
                // Allocate a new read buffer to include total length(DMA capable)
                readDataBuffer = (unsigned char *)heap_caps_malloc(MaxDatalength, MALLOC_CAP_DMA);
                if (readDataBuffer == 0) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
        }

        // Set up SPI Transaction
        spi_transaction_t trans_desc;

        trans_desc.flags = 0;
        trans_desc.cmd = 0;
        trans_desc.addr = 0;
        // length - Full duplex is total length, half duplex the TX length
        trans_desc.length = databitLength * MaxElementlength;
        // rxlength - Full duplex is same as length or 0, half duplex is read length 
        trans_desc.rxlength = 0;
        trans_desc.user = NULL;
        trans_desc.tx_buffer = writeData;
        trans_desc.rx_buffer = readDataBuffer;


        /*
            // Esp32 doesn't have a async mode yet
            // This is the start of added that support, not sure if its required
            if (async)
            {

                ret = spi_device_queue_trans((spi_device_handle_t)deviceHandle, &trans_desc, portMAX_DELAY);
                if (ret != ESP_OK)
                {
                    if (!wrc.fullDuplex) heap_caps_free(readDataBuffer);

                    switch (ret)
                    {
                        case ESP_ERR_NO_MEM:
                            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

                        case ESP_ERR_INVALID_ARG:
                            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

                        default:
                        case ESP_ERR_INVALID_STATE:
                            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                    }
                }

                // There is no callback etc with ESP32 IDF so we need to start a task to wait for operation to complete on this device



                // Return Busy to indicate aync call started and callback will be called on completion
                NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
            }
            else
        */
        {
            // Synchronous SPI operation
            // Use Polling method to start and wait to complete(quickest)
            ret = spi_device_polling_transmit((spi_device_handle_t)deviceHandle, &trans_desc);
            if (ret != ESP_OK)
            {
                if (!wrc.fullDuplex) heap_caps_free(readDataBuffer);
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // Finish up half duplex
            if (!wrc.fullDuplex)
            {
                if (readSize)
                {
                    // Copy the read data
                    memcpy(readData, readDataBuffer + writeSize + wrc.readOffset, readSize - wrc.readOffset);
                    heap_caps_free(readDataBuffer);
                }
            }
        }
    }

    NANOCLR_NOCLEANUP();
}

// Performs a read/write operation on 16-bit word data. 
HRESULT CPU_SPI_nWrite16_nRead16(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION & sdev, SPI_WRITE_READ_SETTINGS & swrs, uint16_t * writePtr, int32_t writeSize, uint16_t * readPtr, int32_t readSize)
{
    swrs.Bits16ReadWrite = true;
    return CPU_SPI_nWrite_nRead(deviceHandle, sdev, swrs, (uint8_t *)writePtr, writeSize, (uint8_t *)readPtr, readSize);
}

// Return status of current SPI operation
// Used to find status of an Async SPI call ( Not supported ), just return complete
SPI_OP_STATUS CPU_SPI_OP_STATUS(uint32_t deviceHandle)
{
    (void)deviceHandle;

    return SPI_OP_STATUS::SPI_OP_COMPLETE;
}

// Return map of available SPI buses as a bit map
uint32_t CPU_SPI_PortsMap()
{
    // SPI1(0x01) & SPI2(0x02)
    return 0x03;
}

// Returns the SPI clock, MISO and MOSI pin numbers for a specified SPI module. 
void   CPU_SPI_GetPins( uint32_t spi_bus, GPIO_PIN& clockPin, GPIO_PIN& misoPin, GPIO_PIN& mosiPin )
{
    clockPin = (GPIO_PIN)Esp32_GetMappedDevicePins( DEV_TYPE_SPI, spi_bus, 2);
    misoPin  = (GPIO_PIN)Esp32_GetMappedDevicePins( DEV_TYPE_SPI, spi_bus, 1);
    mosiPin  = (GPIO_PIN)Esp32_GetMappedDevicePins( DEV_TYPE_SPI, spi_bus, 0);
}

// Return SPI minimum clock frequency
uint32_t CPU_SPI_MinClockFrequency( uint32_t spi_bus )
{
    (void)spi_bus;

    // TODO check what is minimum ( min clock that can be configured on chip)
    return 20000000/256;
}

// Return SPI maximum clock frequency
//
// Maximum frequency will depend on current configuration
// If using native spi pins then max is 80mhz
// if spi pins are routed over gpio maxtrix then 40mhz half duplex 26mhz full

uint32_t CPU_SPI_MaxClockFrequency( uint32_t spi_bus )
{
    bool directPin = false;

    GPIO_PIN clockPin, misoPin, mosiPin;

    CPU_SPI_GetPins(spi_bus, clockPin, misoPin, mosiPin);
    
    // Check if direct pins being used
    switch (clockPin)
    {
        case 14:
            if (misoPin == 12 && mosiPin == 13)
                directPin = true;
            break;

        case 18:
            if (misoPin == 19 && mosiPin == 23)
                directPin = true;
            break;

        default:
            break;
    }
 
    if (directPin) return MAX_CLOCK_FREQUENCY_NATIVE;

    return MAX_CLOCK_FREQUENCY_GPIO_HALF;
}

//
// Return the number of chip select lines available on the bus.
//
// ESP32 IDF/hardware allows only 3 devices per bus
// TODO Subtract used chip select count ?
uint32_t CPU_SPI_ChipSelectLineCount(uint32_t spi_bus)
{
    (void)spi_bus;

    return 3;
}