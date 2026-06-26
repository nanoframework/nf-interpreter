//
// Copyright (c) .NET nanoFramework PIO contributors
//
// PioStateMachine InternalCalls. NativeInit unpacks the config blob into the SM registers.
//

#include "nanoFramework_Hardware_Rp2040.h"
#include "nanoFramework_Hardware_Rp2040_nanoFramework_Hardware_Rp2040_Pio_PioStateMachine.h"
#if defined(RP2350)
#include "rp2350.h"
#else
#include "rp2040.h"
#endif

using namespace nanoFramework_Hardware_Rp2040::nanoFramework_Hardware_Rp2040;

// FIFO busy-wait cap so a stalled SM times out instead of hanging the interpreter.
static const unsigned int PIO_FIFO_WAIT_LIMIT = 0x4000000u;

// blob indices, must match PioStateMachineConfig.cs
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
    PIO_CFG_FIFO_JOIN = 20,
    PIO_CFG_GPIO_BASE = 21,
    PIO_CFG_BLOB_LENGTH = 22,
};

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

    // disable + restart before reconfigure
    pio->CTRL &= ~(1u << sm);
    pio->CTRL |= (1u << (4 + sm)); // SM_RESTART

    // CLKDIV: int [31:16], frac [15:8]
    pio->SM[sm].CLKDIV = (b[PIO_CFG_CLKDIV_INT] << 16) | (b[PIO_CFG_CLKDIV_FRAC] << 8);

    // side-set count includes the optional enable bit
    unsigned int sidesetTotal = b[PIO_CFG_SIDESET_COUNT] + b[PIO_CFG_SIDESET_OPT];
    pio->SM[sm].PINCTRL =
        (sidesetTotal << 29) | (b[PIO_CFG_SET_COUNT] << 26) | (b[PIO_CFG_OUT_COUNT] << 20) |
        (b[PIO_CFG_IN_BASE] << 15) | (b[PIO_CFG_SIDESET_BASE] << 10) | (b[PIO_CFG_SET_BASE] << 5) |
        b[PIO_CFG_OUT_BASE];

    // EXECCTRL: wrap [16:12], wrap_target [11:7], side_en [30], side_pindir [29], jmp_pin [28:24]
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

    // a 32-bit threshold encodes as 0 in the 5-bit field
    unsigned int pushThresh = b[PIO_CFG_PUSH_THRESHOLD] & 0x1F;
    unsigned int pullThresh = b[PIO_CFG_PULL_THRESHOLD] & 0x1F;
    // FJOIN: low 2 bits -> TX [30] / RX [31]; high 2 bits (PIO v1) -> RX_GET [14] / RX_PUT [15]
    unsigned int join = b[PIO_CFG_FIFO_JOIN];
    pio->SM[sm].SHIFTCTRL =
        (b[PIO_CFG_IN_SHIFT_RIGHT] << 18) | (b[PIO_CFG_OUT_SHIFT_RIGHT] << 19) |
        (b[PIO_CFG_AUTOPUSH] << 16) | (b[PIO_CFG_AUTOPULL] << 17) | (pushThresh << 20) |
        (pullThresh << 25) | ((join & 3u) << 30) | ((join >> 2) << 14);

#if defined(RP2350)
    // GPIOBASE is 0 or 16; CMSIS types it __I so write through a volatile pointer
    *(volatile unsigned int *)&pio->GPIOBASE = b[PIO_CFG_GPIO_BASE];
#endif

    // set PC via JMP <offset> (opcode 0)
    pio->SM[sm].INSTR = (unsigned int)(param2 & 0x1F);
}

void PioStateMachine::NativeSetEnabled(signed int param0, signed int param1, bool param2, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr || param1 < 0 || param1 > 3)
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
    if (pio == nullptr || param1 < 0 || param1 > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    // FSTAT TX_FULL = bits [19:16]
    unsigned int guard = PIO_FIFO_WAIT_LIMIT;
    while ((pio->FSTAT & (1u << (16 + param1))) && --guard)
    {
    }
    if (guard == 0)
    {
        hr = CLR_E_TIMEOUT;
        return;
    }
    pio->TXF[param1] = param2;
}

unsigned int PioStateMachine::NativeGetBlocking(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr || param1 < 0 || param1 > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return 0;
    }

    // FSTAT RX_EMPTY = bits [11:8]
    unsigned int guard = PIO_FIFO_WAIT_LIMIT;
    while ((pio->FSTAT & (1u << (8 + param1))) && --guard)
    {
    }
    if (guard == 0)
    {
        hr = CLR_E_TIMEOUT;
        return 0;
    }
    return pio->RXF[param1];
}

bool PioStateMachine::NativeTxFull(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr || param1 < 0 || param1 > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return false;
    }

    return (pio->FSTAT & (1u << (16 + param1))) != 0;
}

bool PioStateMachine::NativeRxEmpty(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    if (pio == nullptr || param1 < 0 || param1 > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return true;
    }

    return (pio->FSTAT & (1u << (8 + param1))) != 0;
}

// claim bitmask owned by PioBlock.cpp
extern unsigned int g_PioClaimedSm[3];

void PioStateMachine::NativeUnclaim(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    int sm = param1;
    if (pio == nullptr || sm < 0 || sm > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    // stop + release the claim
    pio->CTRL &= ~(1u << sm);
    g_PioClaimedSm[param0] &= ~(1u << sm);
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

    // in chunks of up to 5 pins, point SET at them and exec "SET pindirs, dirs", then restore PINCTRL
    int pin = param2;
    int remaining = param3;
    unsigned int savedPinCtrl = pio->SM[sm].PINCTRL;

    while (remaining > 0)
    {
        int chunk = remaining < 5 ? remaining : 5;
        unsigned int dirs = param4 ? ((1u << chunk) - 1u) : 0u;

        // PINCTRL: SET_COUNT [28:26], SET_BASE [9:5]
        pio->SM[sm].PINCTRL = ((unsigned int)chunk << 26) | ((unsigned int)pin << 5);
        // SET pindirs, dirs
        pio->SM[sm].INSTR = 0xE000u | (4u << 5) | (dirs & 0x1Fu);

        remaining -= chunk;
        pin += chunk;
    }

    pio->SM[sm].PINCTRL = savedPinCtrl;
}

void PioStateMachine::NativeClearFifos(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    int sm = param1;
    if (pio == nullptr || sm < 0 || sm > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    // toggle FJOIN_RX (bit 31) twice to flush both FIFOs, SHIFTCTRL unchanged
    unsigned int fjoinRx = (1u << 31);
    pio->SM[sm].SHIFTCTRL ^= fjoinRx;
    pio->SM[sm].SHIFTCTRL ^= fjoinRx;
}

void PioStateMachine::NativeDrainTxFifo(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    int sm = param1;
    if (pio == nullptr || sm < 0 || sm > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    // exec OUT NULL,32 (autopull) or PULL noblock until TX empty. FSTAT TX_EMPTY = bits [27:24]
    unsigned int autopull = pio->SM[sm].SHIFTCTRL & (1u << 17);
    unsigned int instr = autopull ? 0x6060u : 0x8080u;
    while ((pio->FSTAT & (1u << (24 + sm))) == 0)
    {
        pio->SM[sm].INSTR = instr;
    }
}

void PioStateMachine::NativeRestart(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    int sm = param1;
    if (pio == nullptr || sm < 0 || sm > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    // SM_RESTART = bits [7:4]
    pio->CTRL |= (1u << (4 + sm));
}

void PioStateMachine::NativeClkDivRestart(signed int param0, signed int param1, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    int sm = param1;
    if (pio == nullptr || sm < 0 || sm > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    // CLKDIV_RESTART = bits [11:8]
    pio->CTRL |= (1u << (8 + sm));
}

void PioStateMachine::NativeExec(signed int param0, signed int param1, unsigned short param2, HRESULT &hr)
{
    PIO_TypeDef *pio = PioFromIndex(param0);
    int sm = param1;
    if (pio == nullptr || sm < 0 || sm > 3)
    {
        hr = CLR_E_INVALID_PARAMETER;
        return;
    }

    // exec out of band, PC unchanged
    pio->SM[sm].INSTR = (unsigned int)param2;
}
