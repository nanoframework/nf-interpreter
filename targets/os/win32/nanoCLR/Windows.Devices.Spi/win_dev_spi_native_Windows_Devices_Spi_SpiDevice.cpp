//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//


#include "stdafx.h"
#include "win_dev_spi_native.h"

//SPIConfig spi_cfg;

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////
//
//enum DataBitOrder
//{
//    DataBitOrder_MSB = 0,
//    DataBitOrder_LSB = 1
//};
//
/////////////////////////////////////////////////////////////////////////////////////////
//// !!! KEEP IN SYNC WITH Windows.Devices.Spi.SpiMode (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////
//
//enum SpiModes
//{
//    SpiMode_Mode0 = 0,
//    SpiMode_Mode1 = 1,
//    SpiMode_Mode2 = 2,
//    SpiMode_Mode3 = 3
//};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

// define this type here to make it shorter and improve code readability
typedef Library_win_dev_spi_native_Windows_Devices_Spi_SpiConnectionSettings SpiConnectionSettings;

// Computes the prescaler registers given a frequency
// uint16_t Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::ComputePrescaler (uint8_t bus, int32_t requestedFrequency)
// {
//     uint16_t pre = 0;
// 	int32_t clock = STM32_SPII2S_MAX >> 1;		// SP1, SPI4, SPI5 and SPI6 on APB2
// 	if (bus == 2 || bus == 3) clock >>= 1;		// SPI2 and SPI3 on APB1

// 	if (clock > requestedFrequency << 3)
//     {
// 		clock >>= 4;
// 		pre |= SPI_CR1_BR_2;
// 	}
// 	if (clock > requestedFrequency << 1)
//     {
// 		clock >>= 2;
// 		pre |= SPI_CR1_BR_1;
// 	}
// 	if (clock > requestedFrequency)	pre |= SPI_CR1_BR_0;

//   return pre;
// }

// Give a complete low-level SPI configuration from user's managed connectionSettings
// nfSPIConfig Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetConfig(int bus, CLR_RT_HeapBlock* config)
// {
//     SPIDriver * _drv;
    
//     int csPin = config[ SpiConnectionSettings::FIELD___csLine ].NumericByRef().s4;
//     uint16_t CR1 = SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_SPE;

//     switch (config[ SpiConnectionSettings::FIELD___spiMode ].NumericByRef().s4)
//     {
//         case SpiMode_Mode1 :
//             CR1 |= SPI_CR1_CPHA;
//             break;
//         case SpiMode_Mode2 :
//             CR1 |= SPI_CR1_CPOL;
//             break;
//         case SpiMode_Mode3 :
//             CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
//             break;
//         default :   // Default to Mode0 if invalid mode specified
//             break;
//     }

//     if (config[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4 == 16)
//     {
//         // data length is 16 bits
//         CR1 |= SPI_CR1_DFF;

//         // Sets the order of bytes transmission : MSB first or LSB first
//         int bitOrder = config[ SpiConnectionSettings::FIELD___bitOrder ].NumericByRef().s4;
//         if (bitOrder == DataBitOrder_LSB) CR1 |= SPI_CR1_LSBFIRST;
//     }

//     // SPI on STM32 needs a prescaler value to set the frequency used
//     CR1 |= ComputePrescaler(bus, config[ SpiConnectionSettings::FIELD___clockFrequency ].NumericByRef().s4);

//     // Choose the driver that is mapped to the chosen bus
//     switch (bus)
//     {
// #if STM32_SPI_USE_SPI1
//         case 1 :   _drv = &SPID1;
//                     break;
// #endif
// #if STM32_SPI_USE_SPI2
//         case 2 :    _drv = &SPID2;
//                     break;
// #endif
// #if STM32_SPI_USE_SPI3
//         case 3 :    _drv = &SPID3;
//                     break;
// #endif
// #if STM32_SPI_USE_SPI4
//         case 4 :    _drv = &SPID4;
//                     break;
// #endif
// #if STM32_SPI_USE_SPI5
//         case 5 :    _drv = &SPID5;
//                     break;
// #endif
// #if STM32_SPI_USE_SPI6
//         case 6 :    _drv = &SPID6;
//                     break;
// #endif
//     }

//     // Create the final configuration
//     nfSPIConfig cfg =
//     {
//         {
//             NULL,
//             GPIO_PORT(csPin),
//             csPin % 16,
//             CR1,
//             0
//         },
//         _drv
//     };

//     return cfg;
// }

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

        // // Get a complete low-level SPI configuration, depending on user's managed parameters
        // nfSPIConfig cfg = GetConfig(bus, pThis[ FIELD___connectionSettings ].Dereference());

        // If databitLength is set to 16 bit, then temporarily set it to 8 bit
        int databitLength = pConfig[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4;
        //if (databitLength == 16) cfg.Configuration.cr1 &= ~SPI_CR1_DFF;

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

        // // because the bus access is shared, acquire and select the appropriate bus
        // spiStart(cfg.Driver, &cfg.Configuration);
        // spiAcquireBus(cfg.Driver);
        // spiSelect(cfg.Driver);

        // // Are we using SPI full-duplex for transfer ?
        // bool fullDuplex = (bool)stack.Arg3().NumericByRef().u1;

        // if (writeSize != 0 && readSize != 0)
        // {
        //     // Transmit+Receive
        //     if (fullDuplex)
        //     {
        //         // Full duplex
        //         spiExchange(cfg.Driver, writeSize, &writeData[0], &readData[0]);    
        //     }
        //     else
        //     {
        //         spiSend(cfg.Driver, writeSize, &writeData[0]);
        //         spiReceive(cfg.Driver, readSize, &readData[0]);
        //     }
        // }
        // else
        // {
        //     // Transmit only or Receive only
        //     if (readSize != 0) 
        //     {
        //         spiReceive(cfg.Driver, readSize, &readData[0]);
        //     }
        //     else
        //     {
        //         spiSend(cfg.Driver, writeSize, &writeData[0]);
        //     }
        // }

        // // Release the bus
        // spiUnselect(cfg.Driver);
        // spiReleaseBus(cfg.Driver);
        // spiStop(cfg.Driver);

        // // If databitLength was set to 16 bit above, then set it back to 16 bit
        // if (databitLength == 16) cfg.Configuration.cr1 |= SPI_CR1_DFF;

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
        CLR_RT_TypedArray<unsigned short> writeData;
        CLR_RT_TypedArray<unsigned short> readData;

        // marshal the write and read buffers
        Interop_Marshal_UINT16_ARRAY(stack, 1, writeData);
        Interop_Marshal_UINT16_ARRAY(stack, 2, readData);
        int writeSize = writeData.GetSize();
        int readSize = readData.GetSize();

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        uint8_t bus = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        // // Gets a complete low-level SPI configuration, depending on user's managed parameters
        // nfSPIConfig cfg = GetConfig(bus, pConfig);

        // If current config databitLength is 8bit, then set it temporarily to 16bit
        int databitLength = pConfig[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4;
        //if (databitLength == 8) cfg.Configuration.cr1 |= SPI_CR1_DFF;
        
        // // because the bus access is shared, acquire and select the appropriate bus
        // spiStart(cfg.Driver, &cfg.Configuration);
        // spiAcquireBus(cfg.Driver);
        // spiSelect(cfg.Driver);

        // // Are we using SPI full-duplex for transfer ?
        // bool fullDuplex = (bool)stack.Arg3().NumericByRef().u1;

        // if (writeSize != 0 && readSize != 0)
        // {
        //     // Transmit+Receive
        //     if (fullDuplex)
        //     {
        //         // Full duplex
        //         spiExchange(cfg.Driver, writeSize, &writeData[0], &readData[0]);    
        //     }
        //     else
        //     {
        //         spiSend(cfg.Driver, writeSize, &writeData[0]);
        //         spiReceive(cfg.Driver, readSize, &readData[0]);
        //     }
        // }
        // else
        // {
        //     // Transmit only or Receive only
        //     if (readSize != 0) 
        //     {
        //         spiReceive(cfg.Driver, readSize, &readData[0]);
        //     }
        //     else
        //     {
        //         spiSend(cfg.Driver, writeSize, (const void*)&buffer[0]);
        //     }
        // }

        // // Release the bus
        // spiUnselect(cfg.Driver);
        // spiReleaseBus(cfg.Driver);
        // spiStop(cfg.Driver);

        // // If current config databitLength was 8bit, then set it back to 8bit
        // if (databitLength == 8) cfg.Configuration.cr1 &= ~SPI_CR1_DFF;

        // null pointers and vars
        pThis = NULL;
        pConfig = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // nothing to do in the native init

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // set pin to input to save power
        // clear interrupts
        // release the pin
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "SPI1,SPI2,SPI3,SPI4,SPI5,SPI6," + terminator and init with the terminator
       char deviceSelectorString[ 30 + 1] = { 0 };

   #if STM32_SPI_USE_SPI1
       strcat(deviceSelectorString, "SPI1,");
   #endif
   #if STM32_SPI_USE_SPI2
       strcat(deviceSelectorString, "SPI2,");
   #endif
   #if STM32_SPI_USE_SPI3
       strcat(deviceSelectorString, "SPI3,");
   #endif
   #if STM32_SPI_USE_SPI4
       strcat(deviceSelectorString, "SPI4,");
   #endif
   #if STM32_SPI_USE_SPI5
       strcat(deviceSelectorString, "SPI5,");
   #endif
   #if STM32_SPI_USE_SPI6
       strcat(deviceSelectorString, "SPI6,");
   #endif

       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP();
}

