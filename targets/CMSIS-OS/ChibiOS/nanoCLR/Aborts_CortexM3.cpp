//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

//--//

#if !defined(BUILD_RTM)

void monitor_debug_printf( const char* format, ... )
{
    char buffer[256];
    va_list arg_ptr;

    //va_start( arg_ptr, format );

    //int len = hal_vsnprintf( buffer, sizeof(buffer)-1, format, arg_ptr );

    // flush existing characters
    // UNDONE: FIXME
    //USART_Flush( ConvertCOM_ComPort(DEBUG_TEXT_PORT) );

    // write string
    // UNDONE: FIXME
    //USART_Write( ConvertCOM_ComPort(DEBUG_TEXT_PORT), buffer, len );

    // flush new characters
    // UNDONE: FIXME
    // USART_Flush( ConvertCOM_ComPort(DEBUG_TEXT_PORT) );

    //va_end( arg_ptr );
}

void FAULT_HandlerDisplay( unsigned int *registers, unsigned int exception )
{
    int i;

    // TODO
    // USART_Initialize( ConvertCOM_ComPort(USART_DEFAULT_PORT), USART_DEFAULT_BAUDRATE, USART_PARITY_NONE, 8, USART_STOP_BITS_ONE, USART_FLOW_NONE );

    // UNDONE: FIXME
    // if( exception )
    //     monitor_debug_printf("EXCEPTION 0x%02x:\r\n", exception);
    // else
    //     monitor_debug_printf("ERROR:\r\n");

    // monitor_debug_printf("  cpsr=0x%08x\r\n", registers[15]);
    // monitor_debug_printf("  pc  =0x%08x\r\n", registers[14]);
    // monitor_debug_printf("  lr  =0x%08x\r\n", registers[13]);
    // monitor_debug_printf("  sp  =0x%08x\r\n", registers + 16);
    
    // for(i = 0; i <= 12; i++)
    //     monitor_debug_printf("  r%02d =0x%08x\r\n", i, registers[i]);
}

#endif  // !defined(BUILD_RTM)

extern "C"
{
    void FAULT_Handler( unsigned int* registers, unsigned int exception )
    {    
        // TODO
        // ASSERT_IRQ_MUST_BE_OFF();
    #if !defined(BUILD_RTM)
        FAULT_HandlerDisplay(registers, exception);
        // TODO
        // CPU_Halt();
    #else
        CPU_Reset();
    #endif  // !defined(BUILD_RTM)
    }

    void HARD_Breakpoint_Handler(unsigned int *registers)
    {    
    #if !defined(BUILD_RTM)
        // TODO
        // GLOBAL_LOCK(irq);
        FAULT_HandlerDisplay(registers, 0);
        // TODO
        // CPU_Halt();
    #else
        CPU_Reset();
    #endif  // !defined(BUILD_RTM)
    }
}
