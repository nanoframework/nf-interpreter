//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"

__attribute__((used))
void prvGetRegistersFromStack(unsigned int * pulFaultStackAddress)
{
    unsigned int stacked_r0;
    unsigned int stacked_r1;
    unsigned int stacked_r2;
    unsigned int stacked_r3;
    unsigned int stacked_r12;
    unsigned int stacked_lr;
    unsigned int stacked_pc;
    unsigned int stacked_psr;

    stacked_r0 = ((unsigned long) hardfault_args[0]);
    stacked_r1 = ((unsigned long) hardfault_args[1]);
    stacked_r2 = ((unsigned long) hardfault_args[2]);
    stacked_r3 = ((unsigned long) hardfault_args[3]);

    stacked_r12 = ((unsigned long) hardfault_args[4]);
    stacked_lr = ((unsigned long) hardfault_args[5]);
    stacked_pc = ((unsigned long) hardfault_args[6]);
    stacked_psr = ((unsigned long) hardfault_args[7]);

    // forces a breakpoint causing the debugger to stop
    // if no debugger is attached this is ignored
    __asm volatile("BKPT #0\n");
    
    // If no debugger connected, just reset the board
    //NVIC_SystemReset();

    for(;;);
}

__attribute__((naked))
void HardFault_Handler(void)
{
    asm volatile(  
        " movs r0, #4                   \n"
        " mov  r1, lr                   \n"
        " tst  r0, r1                   \n"
        " beq 1f                        \n"
        " mrs r0, psp                   \n"
        " ldr r1,handler2_address_const \n"
        " bx r1                         \n"  
        " 1:mrs r0,msp                  \n"  
        " ldr r1,=hard_fault_handler_c  \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
        : /* no output */  
        : /* no input */  
        : "r0" /* clobber */

    ); 
}

