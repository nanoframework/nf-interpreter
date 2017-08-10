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

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__STRING__SZARRAY_U1__SZARRAY_U1__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__STRING__SZARRAY_U2__SZARRAY_U2__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        uint8_t data[2];
        data[1] = 31;
        data[0] = 0;

        palSetPad(GPIOE, GPIOE_LED2);
        spiSelect(&SPID1);
        spiSend(&SPID1, 2, &data);
        spiUnselect(&SPID1);
        palSetPad(GPIOC, GPIOC_LED3);
        /*

        int bus = 1;    //Fixme : retrieve real bus number from param0
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
        if (sizeof(param1) != 0 && sizeof(param2) != 0)     // Transmit+Receive
        {
            if (param3) spiExchange(&_drv,sizeof(param1), &param1, &param2);    // Full duplex
            else
            {
                spiSend(&_drv, sizeof(param1), &param1);
                spiReceive(&_drv, sizeof(param2), &param2);
            }
        }
        else                    // Transmit only or Receive only
        {
            if (sizeof(param2) != 0) spiReceive(&_drv, sizeof(param2), &param2);
            else spiSend(&_drv, sizeof(param1), &param1);
        }
        spiUnselect(&_drv);
        */
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID__STRING__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        uint8_t data[2];
        data[1] = 31;
        data[0] = 0;

        int bus = 1;    //Fixme : retrieve real bus number from param0
        SPIConfig spi_cfg =
            {
                NULL,
                GPIOA,
                GPIOA_CS1,
                SPI_CR1_BR_0,       // Fixme : set correct bits from config parameters
            };
        palSetPad(GPIOE, GPIOE_LED1);
        spiStart(&SPID1, &spi_cfg);
        
        palSetPad(GPIOE, GPIOE_LED2);
        spiSelect(&SPID1);
        spiSend(&SPID1, 2, &data);
        spiUnselect(&SPID1);
        palSetPad(GPIOC, GPIOC_LED3);
        
        switch (bus)
        {
    #if STM32_SPI_USE_SPI1
            case 1 :    spiStart(&SPID1, &spi_cfg);
        
        palSetPad(GPIOE, GPIOE_LED2);
        spiSelect(&SPID1);
        spiSend(&SPID1, 2, &data);
        spiUnselect(&SPID1);
        palSetPad(GPIOC, GPIOC_LED3);
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
