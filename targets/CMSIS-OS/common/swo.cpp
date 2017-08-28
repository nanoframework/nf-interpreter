//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <cmsis_os.h>

#include "nanoCLR_Types.h"

extern "C" void SwoInit()
{
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
    ITM_SendChar(c);
}

extern "C" void SwoPrintString(const char *s)
{
    // print char until terminator is found
    while(*s)
    {
        SwoPrintChar(*s++);
    }
}

int CLR_Debug::Printf( const char *format, ... )
{
    char    buffer[256];
    va_list arg_ptr;

    va_start( arg_ptr, format );

   int len = hal_vsnprintf( buffer, sizeof(buffer)-1, format, arg_ptr );

    // send characters directly to the trace port
    for( char* p = buffer; *p != '\0' || p-buffer >= 256; ++p )
    ITM_SendChar( *p );

    va_end( arg_ptr );
}
