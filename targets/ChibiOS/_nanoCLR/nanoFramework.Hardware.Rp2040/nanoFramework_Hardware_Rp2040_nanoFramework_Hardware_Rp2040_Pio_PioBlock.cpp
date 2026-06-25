//
// Copyright (c) .NET nanoFramework PIO contributors
//
//

#include "nanoFramework_Hardware_Rp2040.h"
#include "nanoFramework_Hardware_Rp2040_nanoFramework_Hardware_Rp2040_Pio_PioBlock.h"
#include "rp2040.h"

using namespace nanoFramework_Hardware_Rp2040::nanoFramework_Hardware_Rp2040;

static PIO_TypeDef *PioFromIndex(int index)
{
    switch (index)
    {
        case 0:
            return PIO0;
        case 1:
            return PIO1;
        default:
            return nullptr;
    }
}

signed int PioBlock::NativeAddProgram(
    signed int param0,
    CLR_RT_TypedArray_UINT16 param1,
    signed int param2,
    signed int param3,
    HRESULT &hr)
{
    (void)param3;

    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return -1;
    }

    unsigned short *instr = param1.GetBuffer();
    int length = param2;
    for (int i = 0; i < length && i < 32; i++)
    {
        pio->INSTR_MEM[i] = instr[i];
    }

    return 0;
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
