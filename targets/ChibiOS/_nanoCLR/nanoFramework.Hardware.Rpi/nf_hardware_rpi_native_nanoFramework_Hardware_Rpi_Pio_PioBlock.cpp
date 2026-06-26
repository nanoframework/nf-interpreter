//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_rpi_native.h"
#if defined(RP2350)
#include "rp2350.h"
#else
#include "rp2040.h"
#endif
#include "nf_hardware_rpi_native_target.h"

unsigned int g_PioInstrUsed[3] = {0, 0, 0};
unsigned int g_PioClaimedSm[3] = {0, 0, 0};

static unsigned int PioSlotMask(int offset, int length)
{
    if (length <= 0)
    {
        return 0u;
    }

    unsigned int span = (length >= 32) ? 0xFFFFFFFFu : ((1u << length) - 1u);
    return span << offset;
}

#if defined(RP2350)
#define PIO_RESET_LSB 11u
#define IO_BANK0_RESET_BIT (1u << 6)
#define PADS_BANK0_RESET_BIT (1u << 9)
#else
#define PIO_RESET_LSB 10u
#define IO_BANK0_RESET_BIT (1u << 5)
#define PADS_BANK0_RESET_BIT (1u << 8)
#endif

static void PioEnsureOutOfReset(int blockIndex)
{
    unsigned int bits =
        (1u << (PIO_RESET_LSB + (unsigned int)blockIndex)) | IO_BANK0_RESET_BIT | PADS_BANK0_RESET_BIT;
    RESETS->CLR.RESET = bits;
    while ((RESETS->RESET_DONE & bits) != bits)
    {
    }
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioBlock::
    NativeAddProgram___STATIC__I4__I4__SZARRAY_U2__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        unsigned short *instr = (unsigned short *)stack.Arg1().DereferenceArray()->GetFirstElement();
        int length = stack.Arg2().NumericByRef().s4;
        int origin = stack.Arg3().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || block < 0 || block > 2)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // first PIO call in any flow, so un-reset here
        PioEnsureOutOfReset(block);

        int offset = -1;
        if (length > 0 && length <= 32)
        {
            // fixed origin needs its exact slots free; relocatable takes the lowest free run (first-fit)
            if (origin >= 0)
            {
                if (origin + length <= 32 && (g_PioInstrUsed[block] & PioSlotMask(origin, length)) == 0)
                {
                    offset = origin;
                }
            }
            else
            {
                for (int candidate = 0; candidate + length <= 32; candidate++)
                {
                    if ((g_PioInstrUsed[block] & PioSlotMask(candidate, length)) == 0)
                    {
                        offset = candidate;
                        break;
                    }
                }
            }
        }

        if (offset >= 0)
        {
            bool relocate = origin < 0;
            for (int i = 0; i < length; i++)
            {
                unsigned short w = instr[i];
                // relocate JMP (opcode 0b000) targets by the load offset
                if (relocate && (w & 0xE000) == 0x0000)
                {
                    unsigned int target = ((unsigned int)(w & 0x1F) + (unsigned int)offset) & 0x1F;
                    w = (unsigned short)((w & ~0x1F) | target);
                }
                pio->INSTR_MEM[offset + i] = w;
            }
            g_PioInstrUsed[block] |= PioSlotMask(offset, length);
        }

        stack.SetResult_I4(offset);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioBlock::
    NativeRemoveProgram___STATIC__VOID__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int length = stack.Arg1().NumericByRef().s4;
        int offset = stack.Arg2().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || offset < 0 || length <= 0 || offset + length > 32)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // reclaim slots, blank to JMP-to-self so a stray enable can't run stale opcodes
        for (int i = 0; i < length; i++)
        {
            pio->INSTR_MEM[offset + i] = (unsigned short)((offset + i) & 0x1F);
        }

        g_PioInstrUsed[block] &= ~PioSlotMask(offset, length);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioBlock::
    NativeClaimUnusedSm___STATIC__I4__I4__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        bool required = stack.Arg1().NumericByRef().u1;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        int claimed = -1;
        for (int sm = 0; sm < 4; sm++)
        {
            if ((g_PioClaimedSm[block] & (1u << sm)) == 0)
            {
                g_PioClaimedSm[block] |= (1u << sm);
                claimed = sm;
                break;
            }
        }

        // none free: `required` hard-fails, else soft -1
        if (claimed < 0 && required)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        stack.SetResult_I4(claimed);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioBlock::
    NativeInitGpio___STATIC__VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int pin = stack.Arg1().NumericByRef().s4;

#if defined(RP2350)
        if (block < 0 || block > 2 || pin < 0 || pin > 47)
#else
        if (block < 0 || block > 1 || pin < 0 || pin > 29)
#endif
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // FUNCSEL = 6 (PIO0), 7 (PIO1), 8 (PIO2)
        IO_BANK0->GPIO[pin].CTRL = 6u + (unsigned int)block;
        // clear OD (bit 7), set IE (bit 6); also clear the RP2350 pad isolation latch (bit 8, resets to 1)
        PADS_BANK0->GPIO[pin] = (PADS_BANK0->GPIO[pin] & ~0x180u) | 0x40u;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioBlock::
    NativeForceIrq___STATIC__VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int irq = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || irq < 0 || irq > 7)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        pio->IRQ_FORCE = (1u << irq);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioBlock::
    NativeClearIrq___STATIC__VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int irq = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || irq < 0 || irq > 7)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        pio->IRQ = (1u << irq);
    }
    NANOCLR_NOCLEANUP();
}
