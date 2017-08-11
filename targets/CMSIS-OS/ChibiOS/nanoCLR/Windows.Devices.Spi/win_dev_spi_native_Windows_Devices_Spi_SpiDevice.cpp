//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include <LaunchCLR.h>
#include <string.h>


#include "win_dev_spi_native.h"


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


stm32_gpio_t* gpioPortSpi[] = { GPIOA, GPIOB
#if STM32_HAS_GPIOC
, GPIOC
#endif
#if STM32_HAS_GPIOD
, GPIOD
#endif
#if STM32_HAS_GPIOE
, GPIOE
#endif
#if STM32_HAS_GPIOF
, GPIOF
#endif
#if STM32_HAS_GPIOG
, GPIOG
#endif
#if STM32_HAS_GPIOH
, GPIOH
#endif
#if STM32_HAS_GPIOI
, GPIOI
#endif
#if STM32_HAS_GPIOJ
, GPIOJ
#endif
#if STM32_HAS_GPIOK
, GPIOK
#endif
};

#define GPIO_PORT(pin) (gpioPortSpi[pin/16])

uint16_t ComputePrescaler (uint8_t bus, int32_t requestedFrequency)
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

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__STRING__SZARRAY_U1__SZARRAY_U1__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    const char * spiBus = stack.Arg0().StringText();
    uint8_t bus = (uint8_t)spiBus[3] - 48;

    int wCount = stack.Arg1().DereferenceArray()->m_numOfElements;
    int rCount = stack.Arg2().DereferenceArray()->m_numOfElements;

    uint8_t * wBuf = stack.Arg1().DereferenceArray()->GetFirstElement();
    uint8_t * rBuf = stack.Arg2().DereferenceArray()->GetFirstElement();

    SPIDriver _drv;

    switch (bus)
    {
#if STM32_SPI_USE_SPI1
        case 1 :   _drv = SPID1;
                    break;
#endif
#if STM32_SPI_USE_SPI2
        case 2 :    _drv = SPID2;
                    break;
#endif
#if STM32_SPI_USE_SPI3
        case 3 :    _drv = SPID3;
                    break;
#endif
#if STM32_SPI_USE_SPI4
        case 4 :    _drv = SPID4;
                    break;
#endif
#if STM32_SPI_USE_SPI5
        case 5 :    _drv = SPID5;
                    break;
#endif
#if STM32_SPI_USE_SPI6
        case 6 :    _drv = SPID6;
                    break;
#endif
    }
    spiSelect(&_drv);
    if (wCount != 0 && rCount != 0)     // Transmit+Receive
    {
        if (stack.Arg3().NumericByRef().u1 == 1) spiExchange(&_drv,wCount, wBuf, rBuf);    // Full duplex
        else
        {
            spiSend(&_drv, wCount, wBuf);
            spiReceive(&_drv, rCount, rBuf);
        }
    }
    else                    // Transmit only or Receive only
    {
        if (rCount != 0) spiReceive(&_drv, rCount, rBuf);
        else spiSend(&_drv, wCount, wBuf);
    }
    spiUnselect(&_drv);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__STRING__SZARRAY_U2__SZARRAY_U2__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // get connection settings from managed object
        CLR_RT_HeapBlock* connectionSettings = pThis[ FIELD___connectionSettings ].Dereference(); FAULT_ON_NULL(connectionSettings);

        const char* spiBus = pThis[ FIELD___spiBus ].RecoverString();
        FAULT_ON_NULL(spiBus);
        
        // spiBus is an ASCII string with the bus name in format 'SPIn'
        // need to grab 'n' from the string and convert to the integer value from the ASCII code
        uint8_t bus = (uint8_t)spiBus[3] - 48;

        uint16_t CR1 = SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_SPE;

        // mode
        int32_t mode = connectionSettings[ SpiConnectionSettings::FIELD___spiMode ].NumericByRef().s4;

        switch (mode)
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

        // Data format
        int32_t dataBitLength = connectionSettings[ SpiConnectionSettings::FIELD___databitLength ].NumericByRef().s4;

        if (dataBitLength == 16)
        {
            // data length is 16 bits
            CR1 |= SPI_CR1_DFF;
        }

        // Clock prescaler
        int32_t clockFrequency = connectionSettings[ SpiConnectionSettings::FIELD___clockFrequency ].NumericByRef().s4;

        CR1 |= ComputePrescaler(bus, clockFrequency);

        // chip select pin
        int32_t csPin = connectionSettings[ SpiConnectionSettings::FIELD___csLine ].NumericByRef().s4;


        SPIConfig spi_cfg =
        {
            NULL,
            GPIO_PORT(csPin),
            csPin % 16,
            CR1,
        };
    
        switch (bus)
        {
    #if STM32_SPI_USE_SPI1
            case 1 :    spiStart(&SPID1, &spi_cfg);
                        break;
    #endif
    #if STM32_SPI_USE_SPI2
            case 2 :    spiStart(&SPID2, &spi_cfg);
                        break;
    #endif
    #if STM32_SPI_USE_SPI3
            case 3 :    spiStart(&SPID3, &spi_cfg);
                        break;
    #endif
    #if STM32_SPI_USE_SPI4
            case 4 :    spiStart(&SPID4, &spi_cfg);
                        break;
    #endif
    #if STM32_SPI_USE_SPI5
            case 5 :    spiStart(&SPID5, &spi_cfg);
                        break;
    #endif
    #if STM32_SPI_USE_SPI6
            case 6 :    spiStart(&SPID6, &spi_cfg);
                        break;
    #endif

        }

    }
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
        strcat(retVal,"SPI1");
    #endif
    #if STM32_SPI_USE_SPI2
        strcat(retVal,",SPI2");
    #endif
    #if STM32_SPI_USE_SPI3
        strcat(retVal,",SPI3");
    #endif
    #if STM32_SPI_USE_SPI4
        strcat(retVal,",SPI4");
    #endif
    #if STM32_SPI_USE_SPI5
        strcat(retVal,",SPI5");
    #endif
    #if STM32_SPI_USE_SPI6
        strcat(retVal,",SPI6");
    #endif

        stack.SetResult_String( retVal );
    }
    NANOCLR_NOCLEANUP();
}
