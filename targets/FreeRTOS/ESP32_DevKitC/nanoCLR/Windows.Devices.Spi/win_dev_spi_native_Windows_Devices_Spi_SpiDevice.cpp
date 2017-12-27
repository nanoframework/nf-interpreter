//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include <Esp32_os.h>
#include <LaunchCLR.h>
#include <string.h>
#include <targetPAL.h>
#include "win_dev_spi_native.h"

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

static const char* TAG = "SpiDevice";


// Inialise the SPI Bus
static void InitSpiBus( spi_host_device_t bus)
{
    spi_bus_config_t bus_config 
    {
       23, // -1,         // mosi pin
       25, // -1,         // miso pin
       19, // -1,         // Clock
        -1,         // Quad Write protect
        -1,         // Quad Hold
        0           // Default max transfer size ( 4096 )
    };

    esp_err_t ret =  spi_bus_initialize(bus,  &bus_config, 1);
    if ( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "Unable to init SPI bus");
        return; 
    }
}


// Give a complete low-level SPI configuration from user's managed connectionSettings
spi_device_interface_config_t Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetConfig( int bus, CLR_RT_HeapBlock* config)
{
    int csPin    = config[ SpiConnectionSettings::FIELD___csLine ].NumericByRef().s4;
    uint8_t spiMode  = (uint8_t)config[ SpiConnectionSettings::FIELD___spiMode ].NumericByRef().s4;
    int bitOrder = config[ SpiConnectionSettings::FIELD___bitOrder ].NumericByRef().s4;
    int clockHz  = config[ SpiConnectionSettings::FIELD___clockFrequency ].NumericByRef().s4;
    //int fullDuplex  = config[ SpiConnectionSettings::FIELD___fullDuplex ].NumericByRef().s4;
    
//ets_printf( "Spi config cspin:%d spiMode:%d bitorder:%d clockHz:%d\n", csPin, spiMode, bitOrder, clockHz);
    uint32_t flags = (bitOrder == 1) ? (SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST) : 0;
   // flags |= fullDuplex? 0 : SPI_DEVICE_HALFDUPLEX;

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

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        uint8_t bus = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        // If databitLength is set to 16 bit, then temporarily set it to 8 bit
        int databitLength = pConfig[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4;

        // dereference the write and read buffers from the arguments
        CLR_RT_HeapBlock_Array* writeBuffer = stack.Arg1().DereferenceArray();
        if (writeBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            writeData = writeBuffer->GetFirstElement();

            // get the size of the buffer by reading the number of elements in the HeapBlock array
            writeSize = writeBuffer->m_numOfElements;
        }

        CLR_RT_HeapBlock_Array* readBuffer = stack.Arg2().DereferenceArray();
        if (readBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            readData = readBuffer->GetFirstElement();

            // get the size of the buffer by reading the number of elements in the HeapBlock array
            readSize = readBuffer->m_numOfElements;
        }

        // TODO: (fullDuplex) We need to know this when setting up device config, so it should be in the configuration not as parameter on transfer

        // Are we using SPI full-duplex for transfer ?
    //    bool fullDuplex = (bool)stack.Arg3().NumericByRef().u1;
    
        // Set up SPI Transaction
        spi_transaction_t trans_desc;
        trans_desc.flags = 0;
        trans_desc.cmd = 0;
        trans_desc.addr = 0;
        trans_desc.length = (writeSize * databitLength);
        trans_desc.rxlength = (readSize * databitLength);
        trans_desc.user = NULL;
        trans_desc.tx_buffer = writeData;
        trans_desc.rx_buffer = readData;

        esp_err_t ret = spi_device_transmit( spiconfig[bus].deviceHandles[0], &trans_desc);
        if ( ret != ESP_OK )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // null pointers and vars
        writeData = NULL;
        readData = NULL;
        writeBuffer = NULL;
        readBuffer = NULL;
        pThis = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        unsigned short * writeData = NULL;
        unsigned short * readData = NULL;
        int writeSize = 0;
        int readSize = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        uint8_t bus = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

         // If current config databitLength is 8bit, then set it temporarily to 16bit
        int databitLength = pConfig[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4;

        // dereference the write and read buffers from the arguments
        CLR_RT_HeapBlock_Array* writeBuffer = stack.Arg1().DereferenceArray();
        if (writeBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            writeData = writeBuffer->GetFirstElementUInt16();

            // get the size of the buffer by reading the number of elements in the HeapBlock array
            writeSize = writeBuffer->m_numOfElements;
        }

        CLR_RT_HeapBlock_Array* readBuffer = stack.Arg2().DereferenceArray();
        if (readBuffer != NULL)
        {
            // grab the pointer to the array by getting the first element of the array
            readData = readBuffer->GetFirstElementUInt16();

            // get the size of the buffer by reading the number of elements in the HeapBlock array
            readSize = readBuffer->m_numOfElements;
        }

        int Maxlength = (readSize > writeSize)? readSize: writeSize;

        // Set up SPI Transaction
        spi_transaction_t trans_desc;
        trans_desc.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_RXDATA;
        trans_desc.cmd = 0;
        trans_desc.addr = 0;
        trans_desc.length = databitLength * Maxlength;
        trans_desc.rxlength = (readSize * databitLength);
        trans_desc.user = NULL;
        trans_desc.tx_buffer = writeData;
        trans_desc.rx_buffer = readData;

        esp_err_t ret = spi_device_transmit( spiconfig[bus].deviceHandles[0], &trans_desc);
        if ( ret != ESP_OK )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
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

    spi_device_interface_config_t dev_config;
    spi_host_device_t bus = HSPI_HOST;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  //FAULT_ON_NULL(pThis);

 
    if ( !nfSpiInited )
    {
        for( int i=0; i<NUM_SPI_BUSES; i++)
        {
            spiconfig[i].spiBusInited = false;
            memset( &spiconfig[i].deviceHandles, 0, sizeof(spiconfig[i].deviceHandles) );
        }      
        nfSpiInited = true;  
    }

    // Check valid bus
    if ( bus == SPI_HOST )
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if ( !spiconfig[bus].spiBusInited )
    {
        InitSpiBus(bus);
        spiconfig[bus].spiBusInited = true;
    }

    // Get a complete low-level SPI configuration, depending on user's managed parameters
    dev_config = GetConfig(bus, pThis[ FIELD___connectionSettings ].Dereference());
    
    // Add device to bus
    if ( spi_bus_add_device( bus, &dev_config, &spiconfig[bus].deviceHandles[0] ) != ESP_OK )
    {
        ESP_LOGE( TAG, "Unable to init SPI device");
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        spi_host_device_t bus = HSPI_HOST;
        
        ets_printf( "Spi dispose%d\n");
        ESP_LOGE( TAG, "Test dispose");
        // Remove device from bus
        spi_bus_remove_device( spiconfig[bus].deviceHandles[0] );
        spiconfig[bus].deviceHandles[0] = 0;
        
        // Free up bus if last device(TODO:)
        spi_bus_free(bus);
        spiconfig[bus].spiBusInited = false;
        
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "SPI1,SPI2,SPI3,SPI4,SPI5,SPI6," + terminator and init with the terminator
       char deviceSelectorString[ 30 + 1] = { 0 };

       strcat(deviceSelectorString, "HSPI,");
       strcat(deviceSelectorString, "VSPI,");
   
       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP();
}

