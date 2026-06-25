//
// Copyright (c) .NET nanoFramework PIO contributors
//
//

#include "nanoFramework_Hardware_Rp2040.h"
#include "nanoFramework_Hardware_Rp2040_nanoFramework_Hardware_Rp2040_Pio_PioBlock.h"
#if defined(RP2350)
#include "rp2350.h"
#else
#include "rp2040.h"
#endif

using namespace nanoFramework_Hardware_Rp2040::nanoFramework_Hardware_Rp2040;

static PIO_TypeDef *PioFromIndex(int index)
{
    switch (index)
    {
        case 0:
            return PIO0;
        case 1:
            return PIO1;
#if defined(RP2350)
        case 2:
            return PIO2;
#endif
        default:
            return nullptr;
    }
}

static int s_nextFree[3] = {0, 0, 0};

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

    unsigned short *instr = param1.GetBuffer();
    int length = param2;
    int origin = param3;

    int offset = origin >= 0 ? origin : s_nextFree[param0];
    if (offset + length > 32)
    {
        return -1; // no room
    }

    bool relocate = origin < 0;
    for (int i = 0; i < length; i++)
    {
        unsigned short w = instr[i];
        if (relocate && (w & 0xE000) == 0x0000)
        {
            unsigned int target = ((unsigned int)(w & 0x1F) + (unsigned int)offset) & 0x1F;
            w = (unsigned short)((w & ~0x1F) | target);
        }
        pio->INSTR_MEM[offset + i] = w;
    }

    if (origin < 0)
    {
        s_nextFree[param0] = offset + length;
    }

    return offset;
}

void PioBlock::NativeRemoveProgram(signed int param0, signed int param1, signed int param2, HRESULT &hr)
{
    (void)param1;
    (void)param2;

    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
    }
}

signed int PioBlock::NativeClaimUnusedSm(signed int param0, bool param1, HRESULT &hr)
{
    (void)param1;

    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return -1;
    }

    for (int sm = 0; sm < 4; sm++)
    {
        if ((pio->CTRL & (1u << sm)) == 0)
        {
            return sm;
        }
    }

    return 0;
}

void PioBlock::NativeInitGpio(signed int param0, signed int param1, HRESULT &hr)
{
    int block = param0;
    int pin = param1;
    if (block < 0 || block > 1 || pin < 0 || pin > 29)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    IO_BANK0->GPIO[pin].CTRL = (block == 0) ? 6u : 7u;
    PADS_BANK0->GPIO[pin] = (PADS_BANK0->GPIO[pin] & ~0x80u) | 0x40u; // clear OD, set IE
}
