//
// Copyright (c) .NET nanoFramework PIO contributors
//
// PioBlock InternalCalls. Drives PIO registers directly (CMSIS, no pico-sdk). PIO v0 is a prefix of v1
// so the same code serves RP2040 and RP2350 (which adds PIO2).
//

#include "nf_hardware_rpi_native.h"
#include "nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioBlock.h"
#if defined(RP2350)
#include "rp2350.h"
#else
#include "rp2040.h"
#endif
#include "nf_hardware_rpi_native_target.h"

using namespace nf_hardware_rpi_native::nanoFramework_Hardware_Rpi;

// per-block bookkeeping (external linkage; PioStateMachine::NativeUnclaim releases a claim too)
//   g_PioInstrUsed: bit per instruction slot 0..31 in use
//   g_PioClaimedSm: bit per state machine 0..3 claimed
unsigned int g_PioInstrUsed[3] = {0, 0, 0};
unsigned int g_PioClaimedSm[3] = {0, 0, 0};

// mask of `length` slots from `offset`
static unsigned int PioSlotMask(int offset, int length)
{
    if (length <= 0)
    {
        return 0u;
    }

    unsigned int span = (length >= 32) ? 0xFFFFFFFFu : ((1u << length) - 1u);
    return span << offset;
}

// PIO powers up held in reset and ChibiOS doesn't un-reset it (only UART/USB/GPIO), so registers read 0
// and ignore writes until the block (and the IO/PAD banks) are taken out of reset.
#if defined(RP2350)
#define PIO_RESET_LSB 11u // RP2350: PIO0=11, PIO1=12, PIO2=13
#define IO_BANK0_RESET_BIT (1u << 6)
#define PADS_BANK0_RESET_BIT (1u << 9)
#else
#define PIO_RESET_LSB 10u // RP2040: PIO0=10, PIO1=11
#define IO_BANK0_RESET_BIT (1u << 5)
#define PADS_BANK0_RESET_BIT (1u << 8)
#endif

static void PioEnsureOutOfReset(int blockIndex)
{
    unsigned int bits =
        (1u << (PIO_RESET_LSB + (unsigned int)blockIndex)) | IO_BANK0_RESET_BIT | PADS_BANK0_RESET_BIT;
    RESETS->CLR.RESET = bits; // atomic-clear alias
    while ((RESETS->RESET_DONE & bits) != bits)
    {
    }
}

signed int PioBlock::NativeAddProgram(
    signed int param0,
    CLR_RT_TypedArray_UINT16 param1,
    signed int param2,
    signed int param3,
    HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr || param0 < 0 || param0 > 2)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return -1;
    }

    // first PIO call in any flow, so un-reset here
    PioEnsureOutOfReset(param0);

    unsigned short *instr = param1.GetBuffer();
    int length = param2;
    int origin = param3;

    if (length <= 0 || length > 32)
    {
        return -1;
    }

    // fixed origin needs its exact slots free; relocatable takes the lowest free run (first-fit)
    int offset = -1;
    if (origin >= 0)
    {
        if (origin + length <= 32 && (g_PioInstrUsed[param0] & PioSlotMask(origin, length)) == 0)
        {
            offset = origin;
        }
    }
    else
    {
        for (int candidate = 0; candidate + length <= 32; candidate++)
        {
            if ((g_PioInstrUsed[param0] & PioSlotMask(candidate, length)) == 0)
            {
                offset = candidate;
                break;
            }
        }
    }

    if (offset < 0)
    {
        return -1; // no room
    }

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

    g_PioInstrUsed[param0] |= PioSlotMask(offset, length);

    return offset;
}

void PioBlock::NativeRemoveProgram(signed int param0, signed int param1, signed int param2, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    int length = param1;
    int offset = param2;
    if (pio == nullptr || offset < 0 || length <= 0 || offset + length > 32)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    // reclaim slots, blank to JMP-to-self so a stray enable can't run stale opcodes
    for (int i = 0; i < length; i++)
    {
        pio->INSTR_MEM[offset + i] = (unsigned short)((offset + i) & 0x1F);
    }

    g_PioInstrUsed[param0] &= ~PioSlotMask(offset, length);
}

signed int PioBlock::NativeClaimUnusedSm(signed int param0, bool param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return -1;
    }

    // first unclaimed SM
    for (int sm = 0; sm < 4; sm++)
    {
        if ((g_PioClaimedSm[param0] & (1u << sm)) == 0)
        {
            g_PioClaimedSm[param0] |= (1u << sm);
            return sm;
        }
    }

    // none free: `required` hard-fails, else soft -1
    if (param1)
    {
        hr = CLR_E_INVALID_OPERATION;
    }

    return -1;
}

void PioBlock::NativeInitGpio(signed int param0, signed int param1, HRESULT &hr)
{
    int block = param0;
    int pin = param1;
#if defined(RP2350)
    if (block < 0 || block > 2 || pin < 0 || pin > 47)
#else
    if (block < 0 || block > 1 || pin < 0 || pin > 29)
#endif
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    // FUNCSEL = 6 (PIO0), 7 (PIO1), 8 (PIO2)
    IO_BANK0->GPIO[pin].CTRL = 6u + (unsigned int)block;
    // clear OD (bit 7), set IE (bit 6); also clear the RP2350 pad isolation latch (bit 8, resets to 1)
    PADS_BANK0->GPIO[pin] = (PADS_BANK0->GPIO[pin] & ~0x180u) | 0x40u;
}

void PioBlock::NativeForceIrq(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr || param1 < 0 || param1 > 7)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    pio->IRQ_FORCE = (1u << param1);
}

void PioBlock::NativeClearIrq(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr || param1 < 0 || param1 > 7)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    pio->IRQ = (1u << param1);
}
