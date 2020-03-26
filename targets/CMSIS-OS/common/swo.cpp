//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <cmsis_os.h>

#include "nanoCLR_Types.h"

#if (__CORTEX_M == 0)
    #error "ITM port is not available on Cortex-M0(+) cores. Need to set CMake option SWO_OUTPUT to OFF."
#else

// number of attempts to write to the ITM port before quitting
// developer note: this is an arbitrary value from trial & error attempts to get a satisfactory output on ST-Link SWO viewer
#define ITM_WRITE_ATTEMPTS      20000

extern "C" void SwoInit()
{
    // set SWO pin (PB3) to alternate mode (0 == the status after RESET) 
    // in case it's being set to a different function in board config
    palSetPadMode(GPIOB, 0x03, PAL_MODE_ALTERNATE(0) );
      
    // enable trace in core debug register
    CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk;

    // enable trace pins with async trace
    DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN | ~DBGMCU_CR_TRACE_MODE;

    // default speed for ST-Link is 2000kHz 
    uint32_t swoSpeed = 2000000; 
    uint32_t swoPrescaler = (STM32_SYSCLK / swoSpeed) + 1;

    // Write the TPIU Current Port Size Register to the desired value (default is 0x1 for a 1-bit port size)
    TPI->CSPSR = 1;
    // Write TPIU Formatter and Flush Control Register
    TPI->FFCR = 0x100;
    // Write the TPIU Select Pin Protocol to select the sync or async mode. Example: 0x2 for async NRZ mode (UART like)
    TPI->SPPR = 2;

    // Write clock prescaler
    TPI->ACPR = swoPrescaler;

    // unlock Write Access to the ITM registers
    ITM->LAR = 0xC5ACCE55;

    // core debug: 
    // Trace bus ID for TPIU
    // enable events
    // enable sync packets 
    // time stamp enable
    // trace main enable
    ITM->TCR = ITM_TCR_TraceBusID_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_ITMENA_Msk; /* ITM Trace Control Register */  

    // enable stimulus port 0
    // we are not using any other port right now
    ITM->TER |= 1;
    ITM->TPR = ITM_TPR_PRIVMASK_Msk;
}

extern "C" void SwoPrintChar(char c)
{
    uint32_t retryCounter = ITM_WRITE_ATTEMPTS;
    bool okToTx = (ITM->PORT[0U].u32 == 1UL);

    // wait (with timeout) until ITM port TX buffer is available
    while( !okToTx &&
           (retryCounter > 0) )
    {
        // do... nothing
        __NOP();

        // decrease retry counter
        retryCounter--;

        // check again
        okToTx = (ITM->PORT[0U].u32 == 1UL);
    }

    if(okToTx)
    {
        ITM->PORT[0U].u8 = (uint8_t)c;
    }
}

extern "C" void SwoPrintString(const char *s)
{
    // print char until terminator is found
    while(*s)
    {
        SwoPrintChar(*s++);
    }
}

// this function is heavily based in the CMSIS ITM_SendChar 
// but with small performance improvements as we are sending a string not individual chars
uint32_t GenericPort_Write_CMSIS(int portNum, const char* data, size_t size)
{
    (void)portNum;

    char* p = (char*)data;
    uint32_t counter = 0;
    uint32_t retryCounter;
    bool okToTx;

    while(  *p != '\0' && 
            counter < size )
    {
        retryCounter = ITM_WRITE_ATTEMPTS;
        okToTx = (ITM->PORT[0U].u32 == 1UL);

        // wait (with timeout) until ITM port TX buffer is available
        while( !okToTx &&
               (retryCounter > 0) )
        {
            // do... nothing
            __NOP();

            // decrease retry counter
            retryCounter--;

            // check again
            okToTx = (ITM->PORT[0U].u32 == 1UL);
        }

        if(okToTx)
        {
            ITM->PORT[0U].u8 = (uint8_t)*p++;
        }

        counter++;
    }

    return counter;
}

uint32_t GenericPort_Write(int portNum, const char* data, size_t size)
{
    return GenericPort_Write_CMSIS(portNum, data, size);
}

#endif // (__CORTEX_M == 0)
