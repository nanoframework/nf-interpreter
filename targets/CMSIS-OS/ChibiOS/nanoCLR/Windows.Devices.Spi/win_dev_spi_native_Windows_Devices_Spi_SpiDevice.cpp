//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include <LaunchCLR.h>
#include <string.h>

#include <targetPAL.h>
#include "win_dev_spi_native.h"

SPIConfig spi_cfg;

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum DataBitOrder
{
    MSB = 0,
    LSB
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


uint16_t Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::ComputePrescaler (uint8_t bus, int32_t requestedFrequency)
{
    uint16_t pre = 0;
	int32_t clock = STM32_SPII2S_MAX >> 1;		// SP1, SPI4, SPI5 and SPI6 on APB2
	if (bus == 2 || bus == 3) clock >>= 1;		// SPI2 and SPI3 on APB1

	if (clock > requestedFrequency << 3)
    {
		clock >>= 4;
		pre |= SPI_CR1_BR_2;
	}
	if (clock > requestedFrequency << 1)
    {
		clock >>= 2;
		pre |= SPI_CR1_BR_1;
	}
	if (clock > requestedFrequency)	pre |= SPI_CR1_BR_0;

  return pre;
}


nfSPIConfig Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetConfig(int bus, CLR_RT_HeapBlock* config)
{
    SPIDriver * _drv;
    
    int csPin = config[ SpiConnectionSettings::FIELD___csLine ].NumericByRef().s4;
    uint16_t CR1 = SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_SPE;

    switch (config[ SpiConnectionSettings::FIELD___spiMode ].NumericByRef().s4)
    {
        case SpiModes_Mode1 :
            CR1 |= SPI_CR1_CPHA;
            break;
        case SpiModes_Mode2 :
            CR1 |= SPI_CR1_CPOL;
            break;
        case SpiModes_Mode3 :
            CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
            break;
        default :   // Default to Mode0 if invalid mode specified
            break;
    }
    if (config[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4 == 16)
    {
        // data length is 16 bits
        CR1 |= SPI_CR1_DFF;
    }
    CR1 |= ComputePrescaler(bus, config[ SpiConnectionSettings::FIELD___clockFrequency ].NumericByRef().s4);

    switch (bus)
    {
#if STM32_SPI_USE_SPI1
        case 1 :   _drv = &SPID1;
                    break;
#endif
#if STM32_SPI_USE_SPI2
        case 2 :    _drv = &SPID2;
                    break;
#endif
#if STM32_SPI_USE_SPI3
        case 3 :    _drv = &SPID3;
                    break;
#endif
#if STM32_SPI_USE_SPI4
        case 4 :    _drv = &SPID4;
                    break;
#endif
#if STM32_SPI_USE_SPI5
        case 5 :    _drv = &SPID5;
                    break;
#endif
#if STM32_SPI_USE_SPI6
        case 6 :    _drv = &SPID6;
                    break;
#endif
    }

    nfSPIConfig cfg =
    {
        {
            NULL,
            GPIO_PORT(csPin),
            csPin % 16,
            CR1,
            0
        },
        _drv
    };

    return cfg;
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

        // get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the SpiDevice() constructor in managed code for details
        uint8_t bus = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);

        nfSPIConfig cfg = GetConfig(bus, pThis[ FIELD___connectionSettings ].Dereference());

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

        // because the bus access is shared, acquire and select the appropriate bus
        spiStart(cfg.Driver, &cfg.Configuration);
        spiAcquireBus(cfg.Driver);
        spiSelect(cfg.Driver);

        bool fullDuplex = (bool)stack.Arg3().NumericByRef().u1;

        if (writeSize != 0 && readSize != 0)
        {
            // Transmit+Receive
            if (fullDuplex)
            {
                // Full duplex
                spiExchange(cfg.Driver, writeSize, &writeData[0], &readData[0]);    
            }
            else
            {
                spiSend(cfg.Driver, writeSize, &writeData[0]);
                spiReceive(cfg.Driver, readSize, &readData[0]);
            }
        }
        else
        {
            // Transmit only or Receive only
            if (readSize != 0) 
            {
                spiReceive(cfg.Driver, readSize, &readData[0]);
            }
            else
            {
                spiSend(cfg.Driver, writeSize, &writeData[0]);
            }
        }

        // Release the bus
        spiUnselect(cfg.Driver);
        spiReleaseBus(cfg.Driver);
        spiStop(cfg.Driver);

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

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

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
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

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
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        char* retVal = "";
    #if STM32_SPI_USE_SPI1
        strcat(retVal,"SPI1,");
    #endif
    #if STM32_SPI_USE_SPI2
        strcat(retVal,"SPI2,");
    #endif
    #if STM32_SPI_USE_SPI3
        strcat(retVal,"SPI3,");
    #endif
    #if STM32_SPI_USE_SPI4
        strcat(retVal,"SPI4,");
    #endif
    #if STM32_SPI_USE_SPI5
        strcat(retVal,"SPI5,");
    #endif
    #if STM32_SPI_USE_SPI6
        strcat(retVal,"SPI6");
    #endif

        stack.SetResult_String( retVal );
    }
    NANOCLR_NOCLEANUP();
}
