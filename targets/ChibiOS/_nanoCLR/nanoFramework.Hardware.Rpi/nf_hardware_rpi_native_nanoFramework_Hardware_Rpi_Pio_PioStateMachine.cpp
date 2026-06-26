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

// busy-wait cap so a stalled SM times out instead of hanging
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
    PIO_CFG_MOV_STATUS_SEL = 22,
    PIO_CFG_MOV_STATUS_N = 23,
    PIO_CFG_OUT_STICKY = 24,
    PIO_CFG_INLINE_OUT_EN = 25,
    PIO_CFG_OUT_EN_SEL = 26,
    PIO_CFG_BLOB_LENGTH = 27,
};

// claim bitmask owned by PioBlock.cpp
extern unsigned int g_PioClaimedSm[3];

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeInit___STATIC__VOID__I4__I4__I4__SZARRAY_U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;
        int offset = stack.Arg2().NumericByRef().s4;
        CLR_RT_HeapBlock_Array *blobArray = stack.Arg3().DereferenceArray();

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3 || blobArray == nullptr ||
            (int)blobArray->m_numOfElements < PIO_CFG_BLOB_LENGTH)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        unsigned int *b = (unsigned int *)blobArray->GetFirstElement();

        // disable + restart before reconfigure
        pio->CTRL &= ~(1u << sm);
        pio->CTRL |= (1u << (4 + sm)); // SM_RESTART

        // CLKDIV: int [31:16], frac [15:8]
        pio->SM[sm].CLKDIV = ((b[PIO_CFG_CLKDIV_INT] & 0xFFFFu) << 16) | ((b[PIO_CFG_CLKDIV_FRAC] & 0xFFu) << 8);

        // side-set count includes the opt enable bit; each field masked to its width
        unsigned int sidesetTotal = b[PIO_CFG_SIDESET_COUNT] + b[PIO_CFG_SIDESET_OPT];
        pio->SM[sm].PINCTRL =
            ((sidesetTotal & 0x7u) << 29) | ((b[PIO_CFG_SET_COUNT] & 0x7u) << 26) |
            ((b[PIO_CFG_OUT_COUNT] & 0x3Fu) << 20) | ((b[PIO_CFG_IN_BASE] & 0x1Fu) << 15) |
            ((b[PIO_CFG_SIDESET_BASE] & 0x1Fu) << 10) | ((b[PIO_CFG_SET_BASE] & 0x1Fu) << 5) |
            (b[PIO_CFG_OUT_BASE] & 0x1Fu);

        // EXECCTRL: wrap [16:12], wrap_target [11:7], side_en [30], side_pindir [29], jmp_pin [28:24]
        unsigned int execCtrl =
            ((b[PIO_CFG_WRAP] & 0x1Fu) << 12) | ((b[PIO_CFG_WRAP_TARGET] & 0x1Fu) << 7) |
            ((b[PIO_CFG_JMP_PIN] & 0x1Fu) << 24) | ((b[PIO_CFG_MOV_STATUS_SEL] & 1u) << 4) |
            (b[PIO_CFG_MOV_STATUS_N] & 0xFu) | ((b[PIO_CFG_OUT_STICKY] & 1u) << 17) |
            ((b[PIO_CFG_INLINE_OUT_EN] & 1u) << 18) | ((b[PIO_CFG_OUT_EN_SEL] & 0x1Fu) << 19);
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
            ((b[PIO_CFG_IN_SHIFT_RIGHT] & 1u) << 18) | ((b[PIO_CFG_OUT_SHIFT_RIGHT] & 1u) << 19) |
            ((b[PIO_CFG_AUTOPUSH] & 1u) << 16) | ((b[PIO_CFG_AUTOPULL] & 1u) << 17) | (pushThresh << 20) |
            (pullThresh << 25) | ((join & 3u) << 30) | (((join >> 2) & 3u) << 14);

#if defined(RP2350)
        // GPIOBASE is 0 or 16; CMSIS types it __I so write through a volatile pointer
        *(volatile unsigned int *)&pio->GPIOBASE = (b[PIO_CFG_GPIO_BASE] == 16) ? 16u : 0u;
#endif

        // set PC via JMP <offset> (opcode 0)
        pio->SM[sm].INSTR = (unsigned int)(offset & 0x1F);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeSetEnabled___STATIC__VOID__I4__I4__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;
        bool enabled = stack.Arg2().NumericByRef().u1;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        if (enabled)
        {
            pio->CTRL |= (1u << sm);
        }
        else
        {
            pio->CTRL &= ~(1u << sm);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativePutBlocking___STATIC__VOID__I4__I4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;
        unsigned int value = stack.Arg2().NumericByRef().u4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        // FSTAT TX_FULL = bits [19:16]
        unsigned int guard = PIO_FIFO_WAIT_LIMIT;
        while ((pio->FSTAT & (1u << (16 + sm))) && --guard)
        {
        }
        if (guard == 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
        pio->TXF[sm] = value;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeGetBlocking___STATIC__U4__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        // FSTAT RX_EMPTY = bits [11:8]
        unsigned int guard = PIO_FIFO_WAIT_LIMIT;
        while ((pio->FSTAT & (1u << (8 + sm))) && --guard)
        {
        }
        if (guard == 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
        stack.SetResult_U4(pio->RXF[sm]);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeTxFull___STATIC__BOOLEAN__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        stack.SetResult_Boolean((pio->FSTAT & (1u << (16 + sm))) != 0);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeRxEmpty___STATIC__BOOLEAN__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        stack.SetResult_Boolean((pio->FSTAT & (1u << (8 + sm))) != 0);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeUnclaim___STATIC__VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        // stop + release the claim
        pio->CTRL &= ~(1u << sm);
        g_PioClaimedSm[block] &= ~(1u << sm);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeSetConsecutivePinDirs___STATIC__VOID__I4__I4__I4__I4__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;
        int basePin = stack.Arg2().NumericByRef().s4;
        int count = stack.Arg3().NumericByRef().s4;
        bool output = stack.Arg4().NumericByRef().u1;

        PIO_TypeDef *pio = PioFromIndex(block);
#if defined(RP2350)
        if (pio == nullptr || sm < 0 || sm > 3 || basePin < 0 || count < 0 || basePin + count > 48)
#else
        if (pio == nullptr || sm < 0 || sm > 3 || basePin < 0 || count < 0 || basePin + count > 30)
#endif
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        // up to 5 pins per "SET pindirs" exec, then restore PINCTRL
        int pin = basePin;
        int remaining = count;
        unsigned int savedPinCtrl = pio->SM[sm].PINCTRL;

        while (remaining > 0)
        {
            int chunk = remaining < 5 ? remaining : 5;
            unsigned int dirs = output ? ((1u << chunk) - 1u) : 0u;

            // PINCTRL: SET_COUNT [28:26], SET_BASE [9:5]
            pio->SM[sm].PINCTRL = ((unsigned int)chunk << 26) | (((unsigned int)pin & 0x1Fu) << 5);
            // SET pindirs, dirs
            pio->SM[sm].INSTR = 0xE000u | (4u << 5) | (dirs & 0x1Fu);

            remaining -= chunk;
            pin += chunk;
        }

        pio->SM[sm].PINCTRL = savedPinCtrl;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeClearFifos___STATIC__VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        // toggle FJOIN_RX (bit 31) twice to flush both FIFOs, SHIFTCTRL unchanged
        unsigned int fjoinRx = (1u << 31);
        pio->SM[sm].SHIFTCTRL ^= fjoinRx;
        pio->SM[sm].SHIFTCTRL ^= fjoinRx;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeDrainTxFifo___STATIC__VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        // exec OUT NULL,32 (autopull) or PULL noblock until TX empty. FSTAT TX_EMPTY = bits [27:24]
        unsigned int autopull = pio->SM[sm].SHIFTCTRL & (1u << 17);
        unsigned int instr = autopull ? 0x6060u : 0x8080u;
        unsigned int guard = PIO_FIFO_WAIT_LIMIT;
        while ((pio->FSTAT & (1u << (24 + sm))) == 0 && --guard)
        {
            pio->SM[sm].INSTR = instr;
        }
        if (guard == 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeRestart___STATIC__VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        // SM_RESTART = bits [7:4]
        pio->CTRL |= (1u << (4 + sm));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeClkDivRestart___STATIC__VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        // CLKDIV_RESTART = bits [11:8]
        pio->CTRL |= (1u << (8 + sm));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeExec___STATIC__VOID__I4__I4__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;
        unsigned short instruction = stack.Arg2().NumericByRef().u2;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        // exec out of band, PC unchanged
        pio->SM[sm].INSTR = (unsigned int)instruction;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeTxLevel___STATIC__U4__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        stack.SetResult_U4((pio->FLEVEL >> (8 * sm)) & 0xFu);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeRxLevel___STATIC__U4__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        stack.SetResult_U4((pio->FLEVEL >> (8 * sm + 4)) & 0xFu);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeGetPc___STATIC__U4__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio == nullptr || sm < 0 || sm > 3)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        stack.SetResult_U4(pio->SM[sm].ADDR & 0x1Fu);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::
    NativeSetClockDivisor___STATIC__VOID__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int block = stack.Arg0().NumericByRef().s4;
        int sm = stack.Arg1().NumericByRef().s4;
        int clkDivInt = stack.Arg2().NumericByRef().s4;
        int clkDivFrac = stack.Arg3().NumericByRef().s4;

        PIO_TypeDef *pio = PioFromIndex(block);
        // 16-bit int + 8-bit frac fields
        if (pio == nullptr || sm < 0 || sm > 3 || clkDivInt < 0 || clkDivInt > 0xFFFF || clkDivFrac < 0 ||
            clkDivFrac > 0xFF)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        PioEnsureOutOfReset(block);

        pio->SM[sm].CLKDIV = ((unsigned int)clkDivInt << 16) | ((unsigned int)clkDivFrac << 8);
        pio->CTRL |= (1u << (8 + sm));
    }
    NANOCLR_NOCLEANUP();
}
