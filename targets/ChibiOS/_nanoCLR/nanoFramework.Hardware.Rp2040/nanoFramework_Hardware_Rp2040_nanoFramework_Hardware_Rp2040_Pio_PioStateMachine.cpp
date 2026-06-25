//
// Copyright (c) .NET nanoFramework PIO contributors
//
//

#include "nanoFramework_Hardware_Rp2040.h"
#include "nanoFramework_Hardware_Rp2040_nanoFramework_Hardware_Rp2040_Pio_PioStateMachine.h"
#include "rp2040.h"

using namespace nanoFramework_Hardware_Rp2040::nanoFramework_Hardware_Rp2040;

enum PioCfgBlob
{
    PIO_CFG_OUT_BASE = 0,
    PIO_CFG_OUT_COUNT = 1,
    PIO_CFG_SET_BASE = 2,
    PIO_CFG_SET_COUNT = 3,
    PIO_CFG_SIDESET_BASE = 4,
    PIO_CFG_SIDESET_COUNT = 5,
    PIO_CFG_SIDESET_OPT = 6,
    PIO_CFG_SIDESET_PINDIRS = 7,
    PIO_CFG_IN_BASE = 8,
    PIO_CFG_JMP_PIN = 9,
    PIO_CFG_OUT_SHIFT_RIGHT = 10,
    PIO_CFG_AUTOPULL = 11,
    PIO_CFG_PULL_THRESHOLD = 12,
    PIO_CFG_IN_SHIFT_RIGHT = 13,
    PIO_CFG_AUTOPUSH = 14,
    PIO_CFG_PUSH_THRESHOLD = 15,
    PIO_CFG_WRAP_TARGET = 16,
    PIO_CFG_WRAP = 17,
    PIO_CFG_CLKDIV_INT = 18,
    PIO_CFG_CLKDIV_FRAC = 19,
    PIO_CFG_BLOB_LENGTH = 20,
};

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

void PioStateMachine::NativeInit(
    signed int param0,
    signed int param1,
    signed int param2,
    CLR_RT_TypedArray_UINT32 param3,
    HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    int sm = param1;
    if (pio == nullptr || sm < 0 || sm > 3 || param3.GetSize() < PIO_CFG_BLOB_LENGTH)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    unsigned int *b = param3.GetBuffer();

    pio->CTRL &= ~(1u << sm);
    pio->CTRL |= (1u << (4 + sm)); // SM_RESTART

    pio->SM[sm].CLKDIV = (b[PIO_CFG_CLKDIV_INT] << 16) | (b[PIO_CFG_CLKDIV_FRAC] << 8);

    unsigned int sidesetTotal = b[PIO_CFG_SIDESET_COUNT] + b[PIO_CFG_SIDESET_OPT];
    pio->SM[sm].PINCTRL =
        (sidesetTotal << 29) | (b[PIO_CFG_SET_COUNT] << 26) | (b[PIO_CFG_OUT_COUNT] << 20) |
        (b[PIO_CFG_IN_BASE] << 15) | (b[PIO_CFG_SIDESET_BASE] << 10) | (b[PIO_CFG_SET_BASE] << 5) |
        b[PIO_CFG_OUT_BASE];

    unsigned int execCtrl =
        (b[PIO_CFG_WRAP] << 12) | (b[PIO_CFG_WRAP_TARGET] << 7) | (b[PIO_CFG_JMP_PIN] << 24);
    if (b[PIO_CFG_SIDESET_OPT])
    {
        execCtrl |= (1u << 30);
    }
    if (b[PIO_CFG_SIDESET_PINDIRS])
    {
        execCtrl |= (1u << 29);
    }
    pio->SM[sm].EXECCTRL = execCtrl;

    unsigned int pushThresh = b[PIO_CFG_PUSH_THRESHOLD] & 0x1F;
    unsigned int pullThresh = b[PIO_CFG_PULL_THRESHOLD] & 0x1F;
    pio->SM[sm].SHIFTCTRL =
        (b[PIO_CFG_IN_SHIFT_RIGHT] << 18) | (b[PIO_CFG_OUT_SHIFT_RIGHT] << 19) |
        (b[PIO_CFG_AUTOPUSH] << 16) | (b[PIO_CFG_AUTOPULL] << 17) | (pushThresh << 20) |
        (pullThresh << 25);

    pio->SM[sm].INSTR = (unsigned int)(param2 & 0x1F);
}

void PioStateMachine::NativeSetEnabled(signed int param0, signed int param1, bool param2, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    if (param2)
    {
        pio->CTRL |= (1u << param1);
    }
    else
    {
        pio->CTRL &= ~(1u << param1);
    }
}

void PioStateMachine::NativePutBlocking(signed int param0, signed int param1, unsigned int param2, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    while (pio->FSTAT & (1u << (16 + param1)))
    {
    }
    pio->TXF[param1] = param2;
}

unsigned int PioStateMachine::NativeGetBlocking(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return 0;
    }

    while (pio->FSTAT & (1u << (8 + param1)))
    {
    }
    return pio->RXF[param1];
}

bool PioStateMachine::NativeTxFull(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return false;
    }

    return (pio->FSTAT & (1u << (16 + param1))) != 0;
}

bool PioStateMachine::NativeRxEmpty(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return true;
    }

    return (pio->FSTAT & (1u << (8 + param1))) != 0;
}

void PioStateMachine::NativeUnclaim(signed int param0, signed int param1, HRESULT &hr)
{
    (void)param1;

    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr)
    {
        hr = CLR_E_INVALID_PARAMETER;
    }
}

void PioStateMachine::NativeSetConsecutivePinDirs(
    signed int param0,
    signed int param1,
    signed int param2,
    signed int param3,
    bool param4,
    HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    int sm = param1;
    if (pio == nullptr || sm < 0 || sm > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    int pin = param2;
    int remaining = param3;
    unsigned int savedPinCtrl = pio->SM[sm].PINCTRL;

    while (remaining > 0)
    {
        int chunk = remaining < 5 ? remaining : 5;
        unsigned int dirs = param4 ? ((1u << chunk) - 1u) : 0u;

        pio->SM[sm].PINCTRL = ((unsigned int)chunk << 26) | ((unsigned int)pin << 5);
        pio->SM[sm].INSTR = 0xE000u | (4u << 5) | (dirs & 0x1Fu);

        remaining -= chunk;
        pin += chunk;
    }

    pio->SM[sm].PINCTRL = savedPinCtrl;
}
