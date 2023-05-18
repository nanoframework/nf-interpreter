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

#include <nanoHAL.h>
#include <Esp32_DeviceMapping.h>

#include <sys_dev_spi_native_target.h>

// Max frequency over GPIO mux pins, full duplex
#define MAX_CLOCK_FREQUENCY_GPIO_FULL 26000000
// Max frequency over GPIO mux pins, half duplex
#define MAX_CLOCK_FREQUENCY_GPIO_HALF 40000000
// Max over native spi pins
#define MAX_CLOCK_FREQUENCY_NATIVE 80000000

// Tag for ESP32 logging
static const char *TAG = "SpiDevice";

// struct representing the SPI transaction per bus
struct NF_PAL_SPI
{
    spi_transaction_t trans;

    int BusIndex;
    SPI_OP_STATUS status; // Current status
    SPI_Callback callback;

    int32_t writeSize;
    int32_t readSize;
    int32_t readOffset;

    bool fullDuplex;
    unsigned char *originalReadData;
    unsigned char *readDataBuffer;
    unsigned char *writeDataBuffer;

    // -1 = Chip Select is not handled | >0 Chip Select is to be controlled with this GPIO
    int32_t CurrentChipSelect;
    int32_t NumberSpiDeviceInUse;
    uint32_t Handle;
};

NF_PAL_SPI nf_pal_spi[2];
bool haveAsyncTrans[2];

// Remove device from bus
// return true of OK, false = error
bool CPU_SPI_Remove_Device(uint32_t deviceHandle)
{
    // Find the bus
    int spiBus = -1;
    if (nf_pal_spi[0].Handle == deviceHandle)
    {
        spiBus = 0;
    }
    else if (nf_pal_spi[1].Handle == deviceHandle)
    {
        spiBus = 1;
    }

    if (spiBus == -1)
    {
        return false;
    }

    // We remove only if it's the last device
    nf_pal_spi[spiBus].NumberSpiDeviceInUse--;
    if (nf_pal_spi[spiBus].NumberSpiDeviceInUse == 0)
    {
        return spi_bus_remove_device((spi_device_handle_t)deviceHandle) != ESP_OK;
    }

    return true;
}

// Initialise the physical SPI bus
// Bus index 0 or 1
// return true of successful, false if error
bool CPU_SPI_Initialize(uint8_t busIndex, const SPI_DEVICE_CONFIGURATION &spiDeviceConfig)
{
    // Do we already initialized the SPI bus?
    if (nf_pal_spi[busIndex].NumberSpiDeviceInUse > 0)
    {
        return true;
    }

    GPIO_PIN clockPin, misoPin, mosiPin;

    // Get pins used by SPI bus
    CPU_SPI_GetPins(busIndex, clockPin, misoPin, mosiPin);

    if (spiDeviceConfig.BusConfiguration == SpiBusConfiguration_FullDuplex)
    {
        // Check that ALL pins have been configured
        if (clockPin == GPIO_PIN_NONE || misoPin == GPIO_PIN_NONE || mosiPin == GPIO_PIN_NONE)
        {
            ESP_LOGE(TAG, "Spi pins for SPI%d not configured", busIndex);
            return false;
        }
    }
    else
    {
        // Half duplex only requires CLK and MOSI
        if (clockPin == GPIO_PIN_NONE || mosiPin == GPIO_PIN_NONE)
        {
            ESP_LOGE(TAG, "Spi pins for SPI%d not configured", busIndex);
            return false;
        }
    }

    spi_bus_config_t bus_config{

        // mosi pin
        mosi_io_num : (int)mosiPin,
        // miso pin
        miso_io_num : (int)misoPin,
        // Clock
        sclk_io_num : (int)clockPin,
        // Quad Write protect (-1 not used)
        quadwp_io_num : -1,
        // Quad Hold
        quadhd_io_num : -1,
        // GPIO pin for spi data4 signal in octal mode, -1 if not used.
        data4_io_num : -1,
        // GPIO pin for spi data5 signal in octal mode, // -1 if not used.
        data5_io_num : -1,
        // GPIO pin for spi data6 signal in octal mode, // -1 if not used.
        data6_io_num : -1,
        // GPIO pin for spi data7 signal in octal mode, //-1 if not used.
        data7_io_num : -1,
        // max transfer size
        max_transfer_sz : 16384,
        // SPICOMMON_BUSFLAG_* flags
        flags : 0,
        intr_flags : ESP_INTR_FLAG_IRAM
    };

    // Try with DMA first
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
    // First available bus on ESP32_C3/S3 is SPI2_HOST
    esp_err_t ret = spi_bus_initialize((spi_host_device_t)(busIndex + SPI2_HOST), &bus_config, SPI_DMA_CH_AUTO);
#else
    // First available bus on ESP32 is HSPI_HOST(1)
    esp_err_t ret = spi_bus_initialize((spi_host_device_t)(busIndex + HSPI_HOST), &bus_config, SPI_DMA_CH_AUTO);
#endif

    if (ret != ESP_OK)
    {
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
        ESP_LOGE(TAG, "Unable to init SPI bus %d esp_err %d", busIndex, ret);
#else
        ESP_LOGE(TAG, "Unable to init SPI bus %d esp_err %d", busIndex + HSPI_HOST, ret);
#endif
        return false;
    }

    nf_pal_spi[busIndex].BusIndex = busIndex;
    nf_pal_spi[busIndex].status = SPI_OP_STATUS::SPI_OP_READY;

    haveAsyncTrans[busIndex] = false;

    return true;
}

// Uninitialise the bus
bool CPU_SPI_Uninitialize(uint8_t busIndex)
{
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
    esp_err_t ret = spi_bus_free((spi_host_device_t)(busIndex));
#else
    esp_err_t ret = spi_bus_free((spi_host_device_t)(busIndex + HSPI_HOST));
#endif

    if (ret != ESP_OK)
    {
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
        ESP_LOGE(TAG, "spi_bus_free bus %d esp_err %d", busIndex, ret);
#else
        ESP_LOGE(TAG, "spi_bus_free bus %d esp_err %d", busIndex + HSPI_HOST, ret);
#endif

        return false;
    }
    return true;
}

// Callback when a transaction has completed
static void IRAM_ATTR spi_trans_ready(spi_transaction_t *trans)
{
    NF_PAL_SPI *pnf_pal_spi = (NF_PAL_SPI *)trans->user;

    if (pnf_pal_spi != &nf_pal_spi[0] && pnf_pal_spi != &nf_pal_spi[1])
        return;

    pnf_pal_spi->status = SPI_OP_STATUS::SPI_OP_COMPLETE;

    // Finish up half duplex, copy read data and deallocate buffer
    if (!pnf_pal_spi->fullDuplex)
    {
        if (pnf_pal_spi->readSize)
        {
            // Copy the read data from allocated buffer
            memcpy(
                pnf_pal_spi->originalReadData,
                pnf_pal_spi->readDataBuffer + pnf_pal_spi->writeSize + pnf_pal_spi->readOffset,
                pnf_pal_spi->readSize);

            // free up buffers use to spoof half duplex transaction while running full duplex
            heap_caps_free(pnf_pal_spi->writeDataBuffer);
            heap_caps_free(pnf_pal_spi->readDataBuffer);
        }
    }

    // if CS is to be controlled by the driver, set the GPIO
    if (pnf_pal_spi->CurrentChipSelect >= 0)
    {
        // de-assert pin based on CS active level
        CPU_GPIO_TogglePinState(pnf_pal_spi->CurrentChipSelect);
    }

    // fire callback for SPI transaction complete
    // only if callback set
    SPI_Callback callback = (SPI_Callback)pnf_pal_spi->callback;

    if (callback)
    {
        callback(pnf_pal_spi->BusIndex);
    }
}

//
// Convert from SPI_DEVICE_CONFIGURATION to spi_device_interface_config_t used for ESP33
spi_device_interface_config_t GetConfig(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig)
{
    uint8_t spiMode = spiDeviceConfig.Spi_Mode;
    int32_t clockHz = spiDeviceConfig.Clock_RateHz;

    // if clock frequency is unset use the maximum frequency
    if (clockHz == 0)
    {
        CPU_SPI_MaxClockFrequency(spiDeviceConfig.Spi_Bus, &clockHz);
    }

    uint32_t flags =
        (spiDeviceConfig.DataOrder16 == DataBitOrder_LSB) ? (SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST) : 0;

    flags |= SPI_DEVICE_NO_DUMMY;

    // check for half duplex
    if (spiDeviceConfig.BusConfiguration == SpiBusConfiguration_HalfDuplex)
    {
        flags |= SPI_DEVICE_HALFDUPLEX;
    }

    // Fill in device config
    spi_device_interface_config_t dev_config{
        0,               // Command bits
        0,               // Address bits
        0,               // Dummy bits
        spiMode,         // SPi Mode
        0,               // Duty cycle 50/50
        0,               // cs_ena_pretrans
        0,               // cs_ena_posttrans
        clockHz,         // Clock speed in Hz
        0,               // Input_delay_ns
        -1,              // Chip select, we will use manual chip select
        flags,           // SPI_DEVICE flags
        1,               // Queue size
        0,               // Callback before
        spi_trans_ready, // Callback after transaction complete
    };

    return dev_config;
}

//
//  Add a device to a SPI bus
//  returns device handle as uint32_t
//
HRESULT CPU_SPI_Add_Device(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, uint32_t &handle)
{
    // First available bus on ESP32 is HSPI_HOST(1), so add one
    spi_device_interface_config_t dev_config = GetConfig(spiDeviceConfig);

    // If the CS is low, then initial value is high and vice versa
    int initialPinValue = 0;
    if (spiDeviceConfig.ChipSelectActiveState == 0)
    {
        initialPinValue = 1;
    }

    // Sets the CS pin as output
    if (spiDeviceConfig.DeviceChipSelect >= 0)
    {
        // pinmode output
        CPU_GPIO_EnableOutputPin(spiDeviceConfig.DeviceChipSelect, (GpioPinValue)initialPinValue, PinMode_Output);
    }

    // check supported bus configuration: all valid except simplex
    if (spiDeviceConfig.BusConfiguration == SpiBusConfiguration_Simplex)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    if (nf_pal_spi[spiDeviceConfig.Spi_Bus].NumberSpiDeviceInUse == 0)
    {
        // Add device to bus
        spi_device_handle_t deviceHandle;

#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
        // First available bus on ESP32_C3/S3 is SPI2_HOST
        esp_err_t ret =
            spi_bus_add_device((spi_host_device_t)(spiDeviceConfig.Spi_Bus + SPI2_HOST), &dev_config, &deviceHandle);
#else
        // First available bus on ESP32 is HSPI_HOST(1)
        esp_err_t ret =
            spi_bus_add_device((spi_host_device_t)(spiDeviceConfig.Spi_Bus + HSPI_HOST), &dev_config, &deviceHandle);
#endif

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Unable to init SPI device, esp_err %d", ret);

            return S_FALSE;
        }

        handle = (uint32_t)deviceHandle;
        nf_pal_spi[spiDeviceConfig.Spi_Bus].Handle = (uint32_t)deviceHandle;
    }
    else
    {
        handle = nf_pal_spi[spiDeviceConfig.Spi_Bus].Handle;
    }

    nf_pal_spi[spiDeviceConfig.Spi_Bus].NumberSpiDeviceInUse++;
    return S_OK;
}

void CPU_SPI_Wait_Busy(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION &sdev)
{
    if (haveAsyncTrans[sdev.Spi_Bus])
    {
        spi_transaction_t *rtrans;
        spi_device_get_trans_result((spi_device_handle_t)deviceHandle, &rtrans, portMAX_DELAY);

        haveAsyncTrans[sdev.Spi_Bus] = false;
    }
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
HRESULT CPU_SPI_nWrite_nRead(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &wrc,
    uint8_t *writeData,
    int32_t writeSize,
    uint8_t *readData,
    int32_t readSize)
{
    NANOCLR_HEADER();
    {
        uint8_t *writeDataBuffer = NULL;
        uint8_t *readDataBuffer = NULL;
        bool async = (wrc.callback != 0);
        esp_err_t ret;

        // get data bit length
        int databitLength = wrc.Bits16ReadWrite ? 16 : 8;

        // // Are we using SPI full-duplex for transfer ?
        // // The SPI doesn't support half duplex operation with DMA and without DMA the receive length is limited to 4
        // bytes
        // // So we run in full duplex mode all the time and simulate half duplex mode by creating a larger receive
        // buffer for
        // // the whole of the transaction and then copying the rx part to original buffer
        // // For full duplex we just use passed buffers.
        int MaxElementlength;
        if (wrc.fullDuplex)
        {
            // for full duplex just use largest element
            // TODO : There may be a problem with read data overwriting passed read buff if write size > read size
            // !!!!!!
            //        May for this case we should always allocate a separate buffer for transfer
            MaxElementlength = (readSize > writeSize) ? readSize : writeSize;

            readDataBuffer = readData;
        }
        else
        {
            // for Half-duplex its the length of both items
            MaxElementlength = writeSize + readSize;
            if (readSize)
            {
                MaxElementlength += wrc.readOffset;
            }

            int maxByteDatalength = (wrc.Bits16ReadWrite) ? MaxElementlength * 2 : MaxElementlength;

            if (readSize) // Any read data then use alternative buffer with write & read data
            {
                // Allocate a new write and read buffers to spoof half duplex operation using full duplex so we can use
                // DMA. length included write data, any dummy bytes(readOffset) and read data size
                writeDataBuffer = (unsigned char *)heap_caps_malloc(maxByteDatalength, MALLOC_CAP_DMA);
                if (writeDataBuffer == 0)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                readDataBuffer = (unsigned char *)heap_caps_malloc(maxByteDatalength, MALLOC_CAP_DMA);
                if (readDataBuffer == 0)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                memset(writeDataBuffer, 0, maxByteDatalength);
                memcpy(writeDataBuffer, writeData, writeSize);
                writeData = writeDataBuffer;
            }
        }

        NF_PAL_SPI *pnf_pal_spi = &nf_pal_spi[sdev.Spi_Bus];

        pnf_pal_spi->writeSize = writeSize;
        pnf_pal_spi->readSize = readSize;
        pnf_pal_spi->originalReadData = readData;
        pnf_pal_spi->readDataBuffer = readDataBuffer;
        pnf_pal_spi->writeDataBuffer = writeDataBuffer;
        pnf_pal_spi->fullDuplex = wrc.fullDuplex;
        pnf_pal_spi->readOffset = wrc.readOffset; // dummy bytes between write & read on half duplex
        pnf_pal_spi->callback = wrc.callback;
        pnf_pal_spi->CurrentChipSelect = wrc.DeviceChipSelect;

        // Wait for any previously queued async transfer
        CPU_SPI_Wait_Busy(deviceHandle, sdev);

        // Set up SPI Transaction
        spi_transaction_t *pTrans = &pnf_pal_spi->trans;

        // use full duplex unless no read data or bus configuration is half duplex
        pTrans->flags =
            (readSize == 0) || (sdev.BusConfiguration == SpiBusConfiguration_HalfDuplex) ? SPI_DEVICE_HALFDUPLEX : 0;
        pTrans->cmd = 0;
        pTrans->addr = 0;
        // length - Full duplex is total length, half duplex the TX length
        pTrans->length = databitLength * MaxElementlength;
        // rxlength - Full duplex is same as length or 0, half duplex is read length
        pTrans->rxlength = 0;
        pTrans->user = (void *)pnf_pal_spi;
        pTrans->tx_buffer = writeData;
        pTrans->rx_buffer = readDataBuffer;

        // if CS is to be controlled by the driver, set the GPIO
        if (wrc.DeviceChipSelect >= 0)
        {
            // assert pin based on CS active level
            CPU_GPIO_SetPinState(wrc.DeviceChipSelect, (GpioPinValue)wrc.ChipSelectActiveState);
        }

        // Start asynchronous SPI transaction
        if (async)
        {
            pnf_pal_spi->callback = wrc.callback;

            ret = spi_device_queue_trans((spi_device_handle_t)deviceHandle, pTrans, portMAX_DELAY);
            if (ret != ESP_OK)
            {
                // Error so free up buffer if Half duplex
                if (!wrc.fullDuplex && readSize != 0)
                {
                    // Clean up
                    heap_caps_free(writeDataBuffer);
                    heap_caps_free(readDataBuffer);
                }

                switch (ret)
                {
                    case ESP_ERR_NO_MEM:
                        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

                    case ESP_ERR_INVALID_ARG:
                        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

                    case ESP_ERR_INVALID_STATE:
                    default:
                        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                }
            }
            else
            {
                haveAsyncTrans[sdev.Spi_Bus] = true;
            }

            pnf_pal_spi->status = SPI_OP_STATUS::SPI_OP_RUNNING;

            // Return Busy to indicate ASync call started and callback will be called on completion
            NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
        }
        else
        {
            // Synchronous SPI operation
            // Use Polling method to start and wait to complete(quickest)
            ret = spi_device_polling_transmit((spi_device_handle_t)deviceHandle, pTrans);

            pnf_pal_spi->status = SPI_OP_STATUS::SPI_OP_COMPLETE;

            if (ret != ESP_OK)
            {
                if (!wrc.fullDuplex && readSize != 0)
                {
                    heap_caps_free(writeDataBuffer);
                    heap_caps_free(readDataBuffer);
                }
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // Finish up half duplex, done in callback
            // frees buffer and copies read data
        }
    }

    NANOCLR_NOCLEANUP();
}

// Performs a read/write operation on 16-bit word data.
HRESULT CPU_SPI_nWrite16_nRead16(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &swrs,
    uint16_t *writePtr,
    int32_t writeSize,
    uint16_t *readPtr,
    int32_t readSize)
{
    swrs.Bits16ReadWrite = true;
    return CPU_SPI_nWrite_nRead(deviceHandle, sdev, swrs, (uint8_t *)writePtr, writeSize, (uint8_t *)readPtr, readSize);
}

// Return status of current SPI operation
// Used to find status of an Async SPI call
SPI_OP_STATUS CPU_SPI_OP_Status(uint8_t busIndex, uint32_t deviceHandle)
{
    (void)deviceHandle;

    NF_PAL_SPI *pnf_pal_spi = &nf_pal_spi[busIndex];

    return pnf_pal_spi->status;
}

// Return map of available SPI buses as a bit map
uint32_t CPU_SPI_PortsMap()
{
    // SPI1(0x01) & SPI2(0x02)
    return 0x03;
}

// Returns the SPI clock, MISO and MOSI pin numbers for a specified SPI module.
void CPU_SPI_GetPins(uint32_t busIndex, GPIO_PIN &clockPin, GPIO_PIN &misoPin, GPIO_PIN &mosiPin)
{
    clockPin = (GPIO_PIN)Esp32_GetMappedDevicePins(DEV_TYPE_SPI, busIndex, Esp32SpiPin_Clk);
    misoPin = (GPIO_PIN)Esp32_GetMappedDevicePins(DEV_TYPE_SPI, busIndex, Esp32SpiPin_Miso);
    mosiPin = (GPIO_PIN)Esp32_GetMappedDevicePins(DEV_TYPE_SPI, busIndex, Esp32SpiPin_Mosi);
}

// Return SPI minimum clock frequency
HRESULT CPU_SPI_MinClockFrequency(uint32_t busIndex, int32_t *frequency)
{
    if (busIndex >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // TODO check what is minimum ( Min clock that can be configured on chip)
    *frequency = 20000000 / 256;

    return S_OK;
}

// Return SPI maximum clock frequency
//
// Maximum frequency will depend on current configuration
// If using native SPI pins then maximum is 80mhz
// if SPI pins are routed over GPIO matrix then 40mhz half duplex 26mhz full
HRESULT CPU_SPI_MaxClockFrequency(uint32_t busIndex, int32_t *frequency)
{
    if (busIndex >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    bool directPin = false;

    GPIO_PIN clockPin, misoPin, mosiPin;

    CPU_SPI_GetPins(busIndex, clockPin, misoPin, mosiPin);

    // Check if direct pins being used
    switch (clockPin)
    {
        case 14:
            if (misoPin == 12 && mosiPin == 13)
            {
                directPin = true;
            }
            break;

        case 18:
            if (misoPin == 19 && mosiPin == 23)
            {
                directPin = true;
            }
            break;

        default:
            break;
    }

    if (directPin)
    {
        *frequency = MAX_CLOCK_FREQUENCY_NATIVE;
    }

    *frequency = MAX_CLOCK_FREQUENCY_GPIO_HALF;

    return S_OK;
}

//
// Return the number of chip select lines available on the bus.
//
uint32_t CPU_SPI_ChipSelectLineCount(uint32_t busIndex)
{
    (void)busIndex;

    return MAX_SPI_DEVICES;
}
