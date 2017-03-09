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

bool CheckValidCLRImage(uint32_t address)
{
    // load nanoCLR vector table
    const vectors_t* nanoCLRVectorTable = (vectors_t*) address;

    // 1st check: the flash content pointed by the address can't be all 0's neither all F's
    // meaning that the Flash is neither 'all burnt' or erased
    
    // the stack pointer is at the 1st position of vectors_t
    if(nanoCLRVectorTable->init_stack == 0xFFFFFFFF ||
       nanoCLRVectorTable->init_stack == 0x00000000)
    {
        // check failed, there is no valid CLR image
        return false;
    }

    // volatile uint16_t* temp1 = (uint16_t*)nanoCLRVectorTable->reset_handler;
    // temp1 = ((uint8_t*)temp1) - 0x1;
    // volatile uint16_t temp = (uint16_t)(*temp1);

    
    // 2nd check: the content pointed by the reset vector has to be 0xB672 
    // this is the opcode for 'CPSID I' which is the very 1st assembly instruction of a ChibiOS nanoCLR image
    uint16_t* opCodeAddress = (uint16_t*)nanoCLRVectorTable->reset_handler;
    // the casts bellow are there because the opcode is a 16 bit value and we need to subtract 1 from the reset vector address pointing to it
    opCodeAddress = (uint8_t*)opCodeAddress - 0x1;

    if((uint16_t)*opCodeAddress == 0xB672)
    {
        // check, there seems to be a valid CLR image
        return true;
    }
    else
    {
        // got here so there isn't a valid CLR imaged flashed
        return false;
    }
}
