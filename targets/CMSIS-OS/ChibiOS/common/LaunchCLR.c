//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>

#include <ch.h>
#include <hal.h>
#include <vectors.h>

void LaunchCLR(uint32_t address)
{
    // function pointer to load nanoCLR ResetHandler address
    irq_vector_t JumpToNanoCLR;

    // load nanoCLR vector table
    const vectors_t* nanoCLRVectorTable = (vectors_t*) address;

    // load the jump address with the nanoCLR ResetHandler address
    JumpToNanoCLR = nanoCLRVectorTable->reset_handler;

    // disable all interrupts in ChibiOS
    chSysDisable();

    // clear any pending interrupts to make sure we are jumping straight to nanoCLR ResetHandler
    SCB->ICSR &= SCB_ICSR_PENDSVCLR_Msk;

    // need to set stack pointer from CLR vector table
    __set_MSP((uint32_t)nanoCLRVectorTable->init_stack);

    // make the jump to nanoCLR, at last
    JumpToNanoCLR();
}