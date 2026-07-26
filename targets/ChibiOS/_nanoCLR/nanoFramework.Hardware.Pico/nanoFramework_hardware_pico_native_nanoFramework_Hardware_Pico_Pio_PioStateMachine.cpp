//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_pico_native.h"
#if defined(RP2350)
#include "rp2350.h"
#else
#include "rp2040.h"
#endif
#include "nanoFramework_hardware_pico_native_target.h"
#include <hal.h>
#include <nanoHAL.h>

#define PIO_FSTAT_RXEMPTY_LSB 8u
#define PIO_FSTAT_TXFULL_LSB  16u

static constexpr unsigned int PIO_FIFO_WAIT_LIMIT = 0x4000000u;

struct PioDmaWork
{
    const rp_dma_channel_t *Channel;
    unsigned int *Buffer;
    unsigned int Count;
};

static PioDmaWork g_PioDmaWork[3][4] = {};
static PioDmaWork g_PioDmaWorkTx[3][4] = {};

// DMA completion callback (ChibiOS shared handler dispatches here); just wake the waiting thread
static void PioDmaCallback(void *p, uint32_t ct)
{
    (void)p;
    (void)ct;
    Events_Set(SYSTEM_EVENT_FLAG_PICOPIO);
}

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

extern unsigned int g_PioClaimedSm[3];

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    NativeInit___VOID__I4__SZARRAY_U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    CLR_RT_HeapBlock_Array *blobArray;
    unsigned int *b;
    unsigned int sidesetTotal, execCtrl, pushThresh, pullThresh, join;
    int block, sm, offset;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    offset = stack.Arg1().NumericByRef().s4;
    blobArray = stack.Arg2().DereferenceArray();

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    FAULT_ON_NULL(blobArray);

    if (offset > 31)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    if (offset < 0 || offset > 31 || blobArray == nullptr ||
        static_cast<int>(blobArray->m_numOfElements) < PIO_CFG_BLOB_LENGTH)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    pio = __rp_pio_blocks[block].pio;
    b = reinterpret_cast<unsigned int *>(blobArray->GetFirstElement());

    // disable + restart before reconfigure
    pio->CTRL &= ~(1u << sm);
    pio->CTRL |= (1u << (4 + sm)); // SM_RESTART

    // CLKDIV: int [31:16], frac [15:8]
    pio->SM[sm].CLKDIV = ((b[PIO_CFG_CLKDIV_INT] & 0xFFFFu) << 16) | ((b[PIO_CFG_CLKDIV_FRAC] & 0xFFu) << 8);

    // side-set count includes the opt enable bit; each field masked to its width
    sidesetTotal = b[PIO_CFG_SIDESET_COUNT] + b[PIO_CFG_SIDESET_OPT];
    pio->SM[sm].PINCTRL = ((sidesetTotal & 0x7u) << 29) | ((b[PIO_CFG_SET_COUNT] & 0x7u) << 26) |
                          ((b[PIO_CFG_OUT_COUNT] & 0x3Fu) << 20) | ((b[PIO_CFG_IN_BASE] & 0x1Fu) << 15) |
                          ((b[PIO_CFG_SIDESET_BASE] & 0x1Fu) << 10) | ((b[PIO_CFG_SET_BASE] & 0x1Fu) << 5) |
                          (b[PIO_CFG_OUT_BASE] & 0x1Fu);

    // EXECCTRL: wrap [16:12], wrap_target [11:7], side_en [30], side_pindir [29], jmp_pin [28:24]
    execCtrl = ((b[PIO_CFG_WRAP] & 0x1Fu) << 12) | ((b[PIO_CFG_WRAP_TARGET] & 0x1Fu) << 7) |
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
    pushThresh = b[PIO_CFG_PUSH_THRESHOLD] & 0x1F;
    pullThresh = b[PIO_CFG_PULL_THRESHOLD] & 0x1F;
    // FJOIN: low 2 bits -> TX [30] / RX [31]; high 2 bits (PIO v1) -> RX_GET [14] / RX_PUT [15]
    join = b[PIO_CFG_FIFO_JOIN];
    pio->SM[sm].SHIFTCTRL = ((b[PIO_CFG_IN_SHIFT_RIGHT] & 1u) << 18) | ((b[PIO_CFG_OUT_SHIFT_RIGHT] & 1u) << 19) |
                            ((b[PIO_CFG_AUTOPUSH] & 1u) << 16) | ((b[PIO_CFG_AUTOPULL] & 1u) << 17) |
                            (pushThresh << 20) | (pullThresh << 25) | ((join & 3u) << 30) | (((join >> 2) & 3u) << 14);

#if defined(RP2350)
    *reinterpret_cast<volatile unsigned int *>(&pio->GPIOBASE) = (b[PIO_CFG_GPIO_BASE] == 16) ? 16u : 0u;
#endif

    pio->SM[sm].INSTR = static_cast<unsigned int>(offset & 0x1F);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_Enabled___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    stack.SetResult_Boolean(pThis[FIELD___enabled].NumericByRef().u1);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    set_Enabled___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pPioBlock;
    int block, sm;
    bool value;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    value = stack.Arg1().NumericByRef().u1;

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pThis[FIELD___enabled].NumericByRef().u1 = value;

    if (value)
    {
        __rp_pio_blocks[block].pio->CTRL |= (1u << sm);
    }
    else
    {
        __rp_pio_blocks[block].pio->CTRL &= ~(1u << sm);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    NativePutBlocking___VOID__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    unsigned int guard, value;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    value = stack.Arg2().NumericByRef().u4;

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    // FSTAT TX_FULL = bits [19:16]
    guard = PIO_FIFO_WAIT_LIMIT;
    while ((pio->FSTAT & (1u << (PIO_FSTAT_TXFULL_LSB + sm))) && --guard)
    {
    }

    if (guard == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    pio->TXF[sm] = value;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    NativeGetBlocking___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    unsigned int guard;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    // FSTAT RX_EMPTY = bits [11:8]
    guard = PIO_FIFO_WAIT_LIMIT;
    while ((pio->FSTAT & (1u << (PIO_FSTAT_RXEMPTY_LSB + sm))) && --guard)
    {
    }

    if (guard == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    stack.SetResult_U4(pio->RXF[sm]);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_IsTxFull___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    stack.SetResult_Boolean((pio->FSTAT & (1u << (PIO_FSTAT_TXFULL_LSB + sm))) != 0);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_IsRxEmpty___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    stack.SetResult_Boolean((pio->FSTAT & (1u << (PIO_FSTAT_RXEMPTY_LSB + sm))) != 0);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    NativeUnclaim___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pPioBlock;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    __rp_pio_blocks[block].pio->CTRL &= ~(1u << sm);

    if (g_AllocatedSMs[block][sm] != nullptr)
    {
        pioSmFree(g_AllocatedSMs[block][sm]);
        g_AllocatedSMs[block][sm] = nullptr;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    SetConsecutivePinDirs___VOID__I4__I4__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int pin, remaining, gpioBase;
    int block, sm, basePin, count, output;
    unsigned int savedPinCtrl;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    basePin = stack.Arg1().NumericByRef().s4;
    count = stack.Arg2().NumericByRef().s4;
    output = stack.Arg3().NumericByRef().u1;

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

#if defined(RP2350)
    gpioBase = static_cast<int>(pio->GPIOBASE);
#else
    gpioBase = 0;
#endif

    // SET_BASE is 5-bit, relative to the SM's GPIO base, so reject pins outside the 32-pin window
    if (basePin < 0 || basePin > PIO_MAX_PIN || count < 0 || count > 32 || (basePin + count) > (PIO_MAX_PIN + 1) ||
        basePin < gpioBase || (basePin - gpioBase + count) > 32)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // up to 5 pins per "SET pindirs" exec, then restore PINCTRL
    pin = basePin;
    remaining = count;
    savedPinCtrl = pio->SM[sm].PINCTRL;

    while (remaining > 0)
    {
        const int chunk = remaining < 5 ? remaining : 5;
        const unsigned int dirs = output ? ((1u << chunk) - 1u) : 0u;

        // PINCTRL: SET_COUNT [28:26], SET_BASE [9:5]
        pio->SM[sm].PINCTRL =
            (static_cast<unsigned int>(chunk) << 26) | ((static_cast<unsigned int>(pin - gpioBase) & 0x1Fu) << 5);
        // SET pindirs, dirs
        pio->SM[sm].INSTR = 0xE000u | (4u << 5) | (dirs & 0x1Fu);

        remaining -= chunk;
        pin += chunk;
    }

    pio->SM[sm].PINCTRL = savedPinCtrl;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::ClearFifos___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    unsigned int fjoinRx;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    // toggle FJOIN_RX (bit 31) twice to flush both FIFOs, SHIFTCTRL unchanged
    fjoinRx = (1u << 31);
    pio->SM[sm].SHIFTCTRL ^= fjoinRx;
    pio->SM[sm].SHIFTCTRL ^= fjoinRx;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::DrainTxFifo___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    unsigned int autopull, instr, guard;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    // exec OUT NULL,32 (autopull) or PULL noblock until TX empty. FSTAT TX_EMPTY = bits [27:24]
    autopull = pio->SM[sm].SHIFTCTRL & (1u << 17);
    instr = autopull ? 0x6060u : 0x8000u;
    guard = PIO_FIFO_WAIT_LIMIT;

    while ((pio->FSTAT & (1u << (24 + sm))) == 0 && --guard)
    {
        pio->SM[sm].INSTR = instr;
    }
    if (guard == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Restart___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    // SM_RESTART = bits [7:4]
    pio->CTRL |= (1u << (4 + sm));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    ClockDivRestart___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int block;
    int sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    // CLKDIV_RESTART = bits [11:8]
    pio->CTRL |= (1u << (8 + sm));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Exec___VOID__U2(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int block;
    int sm;
    unsigned short instruction;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    instruction = stack.Arg1().NumericByRef().u2;

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    // exec out of band, PC unchanged
    pio->SM[sm].INSTR = static_cast<unsigned int>(instruction);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::get_TxLevel___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    stack.SetResult_U4((pio->FLEVEL >> (8 * sm)) & 0xFu);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::get_RxLevel___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    stack.SetResult_U4((pio->FLEVEL >> (8 * sm + 4)) & 0xFu);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_ProgramCounter___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int block, sm;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;

    stack.SetResult_U4(pio->SM[sm].ADDR & 0x1Fu);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    set_ClockDivisor___VOID__R4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PIO_TypeDef *pio;
    CLR_RT_HeapBlock *pPioBlock;
    int block, sm, intPart, frac;
    float value;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    value = stack.Arg1().NumericByRef().r4;

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    if (!(value >= 1.0f && value <= 65536.0f))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    intPart = static_cast<int>(value);
    frac = static_cast<int>(roundf((value - static_cast<float>(intPart)) * 256.0f));
    if (frac > 255)
    {
        frac = 0;
        intPart += 1;
    }

    pio = __rp_pio_blocks[block].pio;

    pio->SM[sm].CLKDIV =
        (static_cast<unsigned int>(intPart >= 65536 ? 0 : intPart) << 16) | (static_cast<unsigned int>(frac) << 8);

    pio->CTRL |= (1u << (8 + sm));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    Read___I4__SZARRAY_U4__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout{};
    CLR_RT_HeapBlock *pPioBlock;
    CLR_INT64 *timeoutTicks;
    CLR_RT_HeapBlock_Array *buffer;
    bool eventResult = true;
    int transferred = 0;
    int block, sm, offset, count, timeoutMs;

    PIO_TypeDef *pio;
    PioDmaWork *work;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    buffer = stack.Arg1().DereferenceArray();
    offset = stack.Arg2().NumericByRef().s4;
    count = stack.Arg3().NumericByRef().s4;
    timeoutMs = stack.Arg4().NumericByRef().s4;

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    FAULT_ON_NULL(buffer);

    if (offset < 0 || count < 0 || timeoutMs < 0 || count > static_cast<int>(buffer->m_numOfElements) ||
        offset > static_cast<int>(buffer->m_numOfElements) - count)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (count == 0)
    {
        stack.SetResult_I4(0);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;
    work = &g_PioDmaWork[block][sm];

    hbTimeout.SetInteger(static_cast<CLR_INT64>(timeoutMs) * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // first call: validate + arm the DMA + park
    if (stack.m_customState == 1)
    {
        // PIO RX DREQ is only mapped for PIO0/PIO1; PIO2 (RP2350) not wired yet
        if (g_AllocatedSMs[block][sm] == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }
        if (buffer == nullptr || count <= 0 || work->Channel != nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        // ChibiOS-owned DMA channel + completion callback (no raw vector, no clash with the SPI DMA)
        const rp_dma_channel_t *ch = dmaChannelAlloc(RP_DMA_CHANNEL_ID_ANY, 3, PioDmaCallback, work);
        if (ch == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        unsigned int *buf = static_cast<unsigned int *>(platform_malloc(static_cast<size_t>(count) * 4));
        if (buf == nullptr)
        {
            dmaChannelFree(ch);
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // read = SM RX FIFO (fixed), write = bounce buffer (incrementing), paced by the SM RX DREQ
        const unsigned int dreq = (static_cast<unsigned int>(block) * 8u) + 4u + static_cast<unsigned int>(sm);

        ch->channel->READ_ADDR = reinterpret_cast<size_t>(&pio->RXF[sm]);
        ch->channel->WRITE_ADDR = reinterpret_cast<size_t>(buf);
        ch->channel->TRANS_COUNT = static_cast<unsigned int>(count);
        dmaChannelEnableInterruptX(ch);
        ch->channel->CTRL_TRIG = DMA_CTRL_TRIG_EN | DMA_CTRL_TRIG_DATA_SIZE_WORD | DMA_CTRL_TRIG_INCR_WRITE |
                                 DMA_CTRL_TRIG_TREQ_SEL(dreq) | DMA_CTRL_TRIG_CHAIN_TO(ch->chnidx);

        work->Channel = ch;
        work->Buffer = buf;
        work->Count = static_cast<unsigned int>(count);

        Events_Get(SYSTEM_EVENT_FLAG_PICOPIO);
        stack.m_customState = 2;
    }

    // wait on the shared PICOPIO event; re-check OUR channel's busy bit each wake (shared-event safe)
    while (work->Channel != nullptr && dmaChannelIsBusyX(work->Channel))
    {
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_PicoPio, eventResult));
        if (!eventResult)
        {
            // timed out; the finish path below aborts the channel
            break;
        }
    }

    if (work->Channel != nullptr)
    {
        const rp_dma_channel_t *ch = work->Channel;

        // on timeout the channel is still busy; stop it so it can't write after we free the buffer
        if (dmaChannelIsBusyX(ch))
        {
            ch->channel->CTRL_TRIG &= ~DMA_CTRL_TRIG_EN;
            DMA->CHAN_ABORT = (1u << ch->chnidx);

            unsigned int abortGuard = PIO_FIFO_WAIT_LIMIT;
            while (DMA->CHAN_ABORT & (1u << ch->chnidx) && --abortGuard)
            {
            }
        }

        transferred = static_cast<int>(work->Count - ch->channel->TRANS_COUNT);
        if (transferred < 0)
        {
            transferred = 0;
        }
        if (buffer != nullptr && offset >= 0 && transferred > 0 &&
            (offset + transferred) <= static_cast<int>(buffer->m_numOfElements))
        {
            memcpy(
                buffer->GetFirstElement() + static_cast<size_t>(offset) * 4,
                work->Buffer,
                static_cast<size_t>(transferred) * 4);
        }

        (void)dmaChannelGetAndClearInterrupts(ch);
        dmaChannelFree(ch);
        platform_free(work->Buffer);
        work->Channel = nullptr;
        work->Buffer = nullptr;
        work->Count = 0;
    }

    // pop the timeout heap block and return the word count actually transferred
    stack.PopValue();
    stack.SetResult_I4(transferred);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    Write___I4__SZARRAY_U4__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout{};
    CLR_RT_HeapBlock *pPioBlock;
    CLR_INT64 *timeoutTicks;
    CLR_RT_HeapBlock_Array *buffer = nullptr;
    bool eventResult = true;
    int transferred = 0;
    int block, sm, offset, count, timeoutMs;

    PIO_TypeDef *pio;
    PioDmaWork *work;

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    buffer = stack.Arg1().DereferenceArray();
    offset = stack.Arg2().NumericByRef().s4;
    count = stack.Arg3().NumericByRef().s4;
    timeoutMs = stack.Arg4().NumericByRef().s4;

    pPioBlock = pThis[FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    sm = pPioBlock[FIELD___sm].NumericByRef().s4;

    FAULT_ON_NULL(buffer);

    if (offset < 0 || count < 0 || timeoutMs < 0 || count > static_cast<int>(buffer->m_numOfElements) ||
        offset > static_cast<int>(buffer->m_numOfElements) - count)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (count == 0)
    {
        stack.SetResult_I4(0);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    VALIDATE_PIO_BLOCK(block);
    VALIDATE_SM(sm);

    pio = __rp_pio_blocks[block].pio;
    work = &g_PioDmaWorkTx[block][sm];

    hbTimeout.SetInteger(static_cast<CLR_INT64>(timeoutMs) * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // first call: validate + copy out + arm the DMA + park
    if (stack.m_customState == 1)
    {
        // PIO TX DREQ is only mapped for PIO0/PIO1; PIO2 (RP2350) not wired yet
        if (g_AllocatedSMs[block][sm] == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        if (buffer == nullptr || count <= 0 || work->Channel != nullptr || offset < 0 ||
            (offset + count) > static_cast<int>(buffer->m_numOfElements))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        // ChibiOS-owned DMA channel + completion callback (no raw vector, no clash with the SPI DMA)
        const rp_dma_channel_t *ch = dmaChannelAlloc(RP_DMA_CHANNEL_ID_ANY, 3, PioDmaCallback, work);
        if (ch == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        unsigned int *buf = static_cast<unsigned int *>(platform_malloc(static_cast<size_t>(count) * 4));
        if (buf == nullptr)
        {
            dmaChannelFree(ch);
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // copy the words out into the bounce buffer before the DMA drains it into the TX FIFO
        memcpy(buf, buffer->GetFirstElement() + static_cast<size_t>(offset) * 4, static_cast<size_t>(count) * 4);

        // read = bounce buffer (incrementing), write = SM TX FIFO (fixed), paced by the SM TX DREQ
        const unsigned int dreq = (static_cast<unsigned int>(block) * 8u) + static_cast<unsigned int>(sm);

        ch->channel->READ_ADDR = reinterpret_cast<size_t>(buf);
        ch->channel->WRITE_ADDR = reinterpret_cast<size_t>(&pio->TXF[sm]);
        ch->channel->TRANS_COUNT = static_cast<unsigned int>(count);
        dmaChannelEnableInterruptX(ch);
        ch->channel->CTRL_TRIG = DMA_CTRL_TRIG_EN | DMA_CTRL_TRIG_DATA_SIZE_WORD | DMA_CTRL_TRIG_INCR_READ |
                                 DMA_CTRL_TRIG_TREQ_SEL(dreq) | DMA_CTRL_TRIG_CHAIN_TO(ch->chnidx);

        work->Channel = ch;
        work->Buffer = buf;
        work->Count = static_cast<unsigned int>(count);

        Events_Get(SYSTEM_EVENT_FLAG_PICOPIO);
        stack.m_customState = 2;
    }

    // wait on the shared PICOPIO event; re-check OUR channel's busy bit each wake (shared-event safe)
    while (work->Channel != nullptr && dmaChannelIsBusyX(work->Channel))
    {
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_PicoPio, eventResult));
        if (!eventResult)
        {
            // timed out; the finish path below aborts the channel
            break;
        }
    }

    if (work->Channel != nullptr)
    {
        const rp_dma_channel_t *ch = work->Channel;

        if (dmaChannelIsBusyX(ch))
        {
            ch->channel->CTRL_TRIG &= ~DMA_CTRL_TRIG_EN;
            DMA->CHAN_ABORT = (1u << ch->chnidx);

            unsigned int abortGuard = PIO_FIFO_WAIT_LIMIT;
            while ((DMA->CHAN_ABORT & (1u << ch->chnidx)) && --abortGuard)
            {
            }
        }

        // data already went out to the TX FIFO -- nothing to copy back
        transferred = static_cast<int>(work->Count - ch->channel->TRANS_COUNT);
        if (transferred < 0)
        {
            transferred = 0;
        }

        (void)dmaChannelGetAndClearInterrupts(ch);
        dmaChannelFree(ch);
        platform_free(work->Buffer);
        work->Channel = nullptr;
        work->Buffer = nullptr;
        work->Count = 0;
    }

    // pop the timeout heap block and return the word count actually transferred
    stack.PopValue();
    stack.SetResult_I4(transferred);

    NANOCLR_NOCLEANUP();
}
