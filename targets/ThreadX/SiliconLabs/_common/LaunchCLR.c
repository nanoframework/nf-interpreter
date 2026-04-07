//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Headers.h>
#include <target_common.h>
#include <vectors.h>
#include <targetHAL.h>

#include <em_device.h>

void LaunchCLR(uint32_t address)
{
    // function pointer to load nanoCLR ResetHandler address
    irq_vector_t JumpToNanoCLR;

    // report successful nanoBooter execution
    ReportSuccessfullNanoBooter();

    // load nanoCLR vector table
    const vectors_t *nanoCLRVectorTable = (vectors_t *)address;

    // load the jump address with the nanoCLR ResetHandler address
    JumpToNanoCLR = nanoCLRVectorTable->ResetHandler;

    // disable all interrupts
    __disable_irq();

    // clear any pending interrupts to make sure we are jumping straight to nanoCLR ResetHandler
    SCB->ICSR &= SCB_ICSR_PENDSVCLR_Msk;

    // need to set stack pointer from CLR vector table
    __set_MSP((uint32_t)nanoCLRVectorTable->InitStack);

    // make the jump to nanoCLR, at last
    JumpToNanoCLR();
}

bool CheckValidCLRImage(uint32_t address)
{
    uint32_t resetVectorAddress;

    // load nanoCLR vector table
    const vectors_t *nanoCLRVectorTable = (vectors_t *)address;

    // 1st check: the flash content pointed by the address can't be all 0's neither all F's
    // meaning that the Flash is neither 'all burnt' or erased
    if ((uint32_t)(*(uint32_t **)((uint32_t *)address)) == 0xFFFFFFFF ||
        (uint32_t)(*(uint32_t **)((uint32_t *)address)) == 0x00000000)
    {
        // check failed, there is no valid CLR image
        return false;
    }

    // 2nd check: the content pointed by the reset vector has to be 0xB508
    // that's an assembly "push	{r3, lr}" the very first one in the Reset_Handler function
    // see platform\Device\SiliconLabs\EFM32GG11B\Source\GCC\startup_efm32gg11b.c

    // "regular" address mapping
    resetVectorAddress = (uint32_t)((uint32_t *)nanoCLRVectorTable->ResetHandler);

    // sanity check for invalid address (out of flash range which causes a hard fault)
    if (resetVectorAddress <= FLASH1_MEMORY_StartAddress ||
        resetVectorAddress >= (FLASH1_MEMORY_StartAddress + FLASH1_MEMORY_Size))
    {
        // check failed, doesn't seem to be a valid CLR image
        return false;
    }

    // the linker can place this anywhere on the address space because of optimizations so we better check where the
    // reset pointer points to
    uint32_t opCodeAddress = (uint32_t)((uint32_t **)nanoCLRVectorTable->ResetHandler);

    // real address is -1
    opCodeAddress -= 1;

    uint32_t opCode = *((uint32_t *)opCodeAddress);
    if ((uint16_t)opCode == 0xB508)
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
