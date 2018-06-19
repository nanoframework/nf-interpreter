//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

//  Esp32 has 4 SPI devices called SPI0, SPI1, HSPI and VSPI
//  SPIO, SPI1 are dedicated entirely to flash
//
//  HSPI("SPI1") and VSPI("SPI2") are free to use

#include <Esp32_os.h>
#include <LaunchCLR.h>
#include <string.h>
#include <targetPAL.h>
#include "win_dev_spi_native.h"
#include "Esp32_DeviceMapping.h"

static nfSpiBusConfig  spiconfig[NUM_SPI_BUSES];
static bool nfSpiInited = false;


///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum DataBitOrder
{
    DataBitOrder_MSB = 0,
    DataBitOrder_LSB
};

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum SpiModes
{
    SpiModes_Mode0 = 0,
    SpiModes_Mode1,
    SpiModes_Mode2,
    SpiModes_Mode3
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

// define this type here to make it shorter and improve code readability
typedef Library_win_dev_spi_native_Windows_Devices_Spi_SpiConnectionSettings SpiConnectionSettings;

// Tag for ESP32 logging
static const char* TAG = "SpiDevice";


// Define this to allow access to device config in spi master driver
struct spi_device_t {
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    spi_device_interface_config_t cfg;
};


void Remove_Spi_Device(int bus, int deviceIndex)
{
        // Remove device from bus
    spi_bus_remove_device( spiconfig[bus].deviceHandles[deviceIndex] );
    spiconfig[bus].deviceHandles[deviceIndex] = 0;
    spiconfig[bus].deviceId[deviceIndex] = 0;
}

// Unitialise SPI on restart
void Esp32_Spi_UnitializeAll()
{
    for (int bus = 0; bus < NUM_SPI_BUSES; bus++) 
    {
        if (spiconfig[bus].spiBusInited)
        {
            // Remove any devices
            for( int deviceIndex=0; deviceIndex < MAX_SPI_DEVICES; deviceIndex++)
            {
                if ( spiconfig[bus].deviceHandles[deviceIndex] != 0 )
                    Remove_Spi_Device(bus, deviceIndex);
            }

            // Delete bus driver 
            spi_bus_free((spi_host_device_t)bus);
            spiconfig[bus].spiBusInited = false;
        }
    }
}

// Inialise the SPI Bus
static void InitSpiBus( spi_host_device_t bus)
{
    gpio_num_t mosiPin  = (gpio_num_t)Esp32_GetMappedDevicePins( DEV_TYPE_SPI, bus, 0);
    gpio_num_t misoPin  = (gpio_num_t)Esp32_GetMappedDevicePins( DEV_TYPE_SPI, bus, 1);
    gpio_num_t clockPin = (gpio_num_t)Esp32_GetMappedDevicePins( DEV_TYPE_SPI, bus, 2);
 
    spi_bus_config_t bus_config 
    {
       mosiPin,          // mosi pin
       misoPin,          // miso pin
       clockPin,         // Clock
        -1,              // Quad Write protect
        -1,              // Quad Hold
        0                // Default max transfer size ( 4096 )
    };

    esp_err_t ret =  spi_bus_initialize(bus,  &bus_config, 1);
    if ( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "Unable to init SPI bus");
        return; 
    }
}


bool Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::Add_Spi_Device(int bus, CLR_RT_HeapBlock* pThis)
{
    spi_device_interface_config_t dev_config;
    nfSpiBusConfig * pBusConfig = &spiconfig[bus];

    // Check if all device slots used
    if ( pBusConfig->deviceCount >= MAX_SPI_DEVICES ) return false;

    // Get a complete low-level SPI configuration, depending on user's managed parameters
    dev_config = GetConfig(bus, pThis[ FIELD___connectionSettings ].Dereference());
    
     // Add device to bus
    spi_device_handle_t deviceHandle;
    if ( spi_bus_add_device( (spi_host_device_t)bus, &dev_config, &deviceHandle ) != ESP_OK )
    {
        ESP_LOGE( TAG, "Unable to init SPI device");
        return false; 
    }

    // Add next Device
    pBusConfig->deviceId[pBusConfig->deviceCount] = pThis[ FIELD___deviceId ].NumericByRef().s4;
    pBusConfig->deviceHandles[pBusConfig->deviceCount++ ] = deviceHandle;

    return true;
}

//
//  Get the Bus and device index
//  return false if error
//
bool Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetDevice( CLR_RT_HeapBlock* pThis, uint8_t * pBus, int * pDeviceIndex)
{
    int32_t deviceId = pThis[ FIELD___deviceId ].NumericByRef().s4;
    *pBus = (uint8_t)(deviceId / 1000);
        
    // Find device in spiconfig
    for( int index=0; index < MAX_SPI_DEVICES; index++)
    {
        if ( spiconfig[*pBus].deviceId[index] == deviceId )
        {
            // Device found with same deviceId
            *pDeviceIndex = index;
            return true;
        }
    }
    return false;
}
// Give a complete low-level SPI configuration from user's managed connectionSettings
spi_device_interface_config_t Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetConfig( int bus, CLR_RT_HeapBlock* config)
{
    (void)bus;

    int csPin    = config[ SpiConnectionSettings::FIELD___csLine ].NumericByRef().s4;
    uint8_t spiMode  = (uint8_t)config[ SpiConnectionSettings::FIELD___spiMode ].NumericByRef().s4;
    int bitOrder = config[ SpiConnectionSettings::FIELD___bitOrder ].NumericByRef().s4;
    int clockHz  = config[ SpiConnectionSettings::FIELD___clockFrequency ].NumericByRef().s4;
    
    // if clock frequency is unset use the maximum frequency
    if (clockHz == 0)
    {
        clockHz = MAX_CLOCK_FREQUENCY;
    }

//ets_printf( "Spi config cspin:%d spiMode:%d bitorder:%d clockHz:%d\n", csPin, spiMode, bitOrder, clockHz);
    uint32_t flags = (bitOrder == 1) ? (SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST) : 0;
 
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
        csPin,      // Chip select
        flags,      // SPI_DEVICE flags
        1,          // Queue size
        0,          // Callback before
        0,          // Callback after
    };

    return dev_config;
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U1__SZARRAY_U1__BOOLEAN( CLR_RT_StackFrame& stack )
{
    return NativeTransfer( stack, false );
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN( CLR_RT_StackFrame& stack )
{
    return NativeTransfer( stack, true );
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer( CLR_RT_StackFrame& stack, bool data16 )
{
    NANOCLR_HEADER();
    {
        unsigned char * writeData = NULL;
        unsigned char * readData = NULL;
        int writeSize = 0;
        int readSize = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index and Device index
        uint8_t bus;
        int deviceIndex;
        if ( GetDevice( pThis, &bus, &deviceIndex) == false )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // get data bit length
        int databitLength = pConfig[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4;
        if ( data16 ) databitLength = 16;

        // dereference the write and read buffers from the arguments
        CLR_RT_HeapBlock_Array* writeBuffer = stack.Arg1().DereferenceArray();
        if (writeBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            if ( data16 )
                writeData = (unsigned char * )writeBuffer->GetFirstElementUInt16();
            else
                writeData = writeBuffer->GetFirstElement();

            // get the size of the buffer by reading the number of elements in the HeapBlock array
            writeSize = writeBuffer->m_numOfElements;
        }

        CLR_RT_HeapBlock_Array* readBuffer = stack.Arg2().DereferenceArray();
        if (readBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            if ( data16 )
               readData = (unsigned char * )readBuffer->GetFirstElementUInt16();
            else
               readData = readBuffer->GetFirstElement();
            
            // get the size of the buffer by reading the number of elements in the HeapBlock array
            readSize = readBuffer->m_numOfElements;
        }

        int MaxDatalength = (readSize > writeSize)? readSize: writeSize;

        int  totalBitLength = databitLength * MaxDatalength;
        int  rxBitLength = readSize * databitLength;

       // Set up SPI Transaction
        spi_transaction_t trans_desc;
        trans_desc.flags  = 0;
        trans_desc.cmd = 0;
        trans_desc.addr = 0;
        trans_desc.length = totalBitLength;
        trans_desc.rxlength = rxBitLength;
        trans_desc.user = NULL;
        trans_desc.tx_buffer = writeData;
        trans_desc.rx_buffer = readData;

        // Are we using SPI full-duplex for transfer ?
        bool fullDuplex = (bool)stack.Arg3().NumericByRef().u1;
        if ( fullDuplex)
        {
            // Do tx and rx
            esp_err_t ret = spi_device_transmit( spiconfig[bus].deviceHandles[deviceIndex], &trans_desc);
            if ( ret != ESP_OK )
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }
        }
        else
        {
            // Halfduplex, first do TX operation then RX.

            // Transmit data if any
            if ( writeSize > 0 )
            {
                trans_desc.length = writeSize * databitLength;
                trans_desc.rxlength = 0;
                esp_err_t ret = spi_device_transmit( spiconfig[bus].deviceHandles[deviceIndex], &trans_desc);
                if ( ret != ESP_OK )
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                }
            }

            // Read data if any
            if ( readSize > 0 )
            {
                trans_desc.length = readSize * databitLength;
                trans_desc.tx_buffer = 0;

                esp_err_t ret = spi_device_transmit( spiconfig[bus].deviceHandles[deviceIndex], &trans_desc);
                if ( ret != ESP_OK )
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                }
            }
        }

       // null pointers and vars
        writeData = NULL;
        readData = NULL;
        writeBuffer = NULL;
        readBuffer = NULL;
        pThis = NULL;
        pConfig = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    //spi_device_interface_config_t dev_config;
 
    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  //FAULT_ON_NULL(pThis);

    // get bus index
    // this is coded with a multiplication, need to perform and int division to get the number
    // see the comments in the SpiDevice() constructor in managed code for details ( 1 or 2 )
    int32_t deviceId = pThis[ FIELD___deviceId ].NumericByRef().s4;

    spi_host_device_t bus = (spi_host_device_t)( deviceId / 1000 );
    
    // Chip Select
    //int chipSelect  = deviceId % 1000;

    // Check valid bus
    if ( bus < HSPI_HOST || bus > VSPI_HOST )
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if ( !nfSpiInited )
    {
        for( int i=0; i<NUM_SPI_BUSES; i++)
        {
            spiconfig[i].spiBusInited = false;
            spiconfig[i].deviceCount = 0;
            memset( &spiconfig[i].deviceHandles, 0, sizeof(spiconfig[i].deviceHandles) );
        }      
        nfSpiInited = true;  
    }

    // Init bus if first device
    if ( !spiconfig[bus].spiBusInited )
    {
        InitSpiBus(bus);
        spiconfig[bus].spiBusInited = true;
        
        // Ensure driver gets unitialized during soft reboot
        HAL_AddSoftRebootHandler(Esp32_Spi_UnitializeAll);
    }

    // Add new device to bus
    if ( Add_Spi_Device( bus, pThis) == false )
    {
       NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This(); 

        // get bus index and Device index
        uint8_t bus;
        int deviceIndex;
        if ( GetDevice( pThis, &bus, &deviceIndex) == false )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Remove device from bus
        Remove_Spi_Device( (spi_host_device_t)bus, deviceIndex);

        // If last device on bus then free up bus
        bool NoDevices = true;
        for( int index=0; index < MAX_SPI_DEVICES; index++)
        {
            if ( spiconfig[bus].deviceHandles[index] != 0 )
            {
                NoDevices = false;
                break;
            }
        }
        if ( NoDevices )
        {
            spi_bus_free((spi_host_device_t)bus);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "SPI1,SPI2" + terminator and init with the terminator
       char deviceSelectorString[ 15 ] = { "SPI1,SPI2" };   // HSPI & VSPI

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP_NOLABEL();
}

