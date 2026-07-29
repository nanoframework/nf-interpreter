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
#include <cmath>
#include <cstring>

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

static inline unsigned int PioRelPin(unsigned int gpio, unsigned int gpioBase)
{
    return (gpio >= gpioBase && (gpio - gpioBase) < 32u) ? (gpio - gpioBase) : (gpio & 0x1Fu);
}

static inline unsigned int PioShiftThreshold(unsigned int threshold)
{
    return (threshold == 0u || threshold > 32u) ? 32u : threshold;
}

enum PioFifoDir
{
    PioFifoRx = 0,
    PioFifoTx = 1,
};

struct PioDmaWork
{
    const rp_dma_channel_t *Channel;
    uint32_t *Buffer;
    uint32_t Count;
    uint32_t Status;
};

static PioDmaWork g_PioDmaWork[3][4][2] = {};

#define PIO_DMA_ERROR_MASK (DMA_CTRL_TRIG_AHB_ERROR | DMA_CTRL_TRIG_READ_ERROR | DMA_CTRL_TRIG_WRITE_ERROR)

static void PioDmaCallback(void *p, const uint32_t ct)
{

    if (auto *work = static_cast<PioDmaWork *>(p); work != nullptr)
    {
        work->Status = ct & PIO_DMA_ERROR_MASK;
    }

    Events_Set(SYSTEM_EVENT_FLAG_PICOPIO);
}

// Stops a transfer and gives the channel and bounce buffer back.
static void PioDmaFinish(PioDmaWork *work)
{
    const rp_dma_channel_t *ch = work->Channel;

    if (ch == nullptr)
    {
        return;
    }

    if (dmaChannelIsBusyX(ch))
    {
        dmaChannelDisableX(ch);
    }

    (void)dmaChannelGetAndClearInterrupts(ch);
    dmaChannelFree(ch);

    if (work->Buffer != nullptr)
    {
        platform_free(work->Buffer);
    }

    work->Channel = nullptr;
    work->Buffer = nullptr;
    work->Count = 0;
    work->Status = 0;
}

// Aborts and releases both directions of a state machine.
void PioDmaReleaseSm(const int block, const int sm)
{
    PioDmaFinish(&g_PioDmaWork[block][sm][PioFifoRx]);
    PioDmaFinish(&g_PioDmaWork[block][sm][PioFifoTx]);
}

// Arms a channel to move count words between the state machine FIFO and the bounce buffer.
static void PioDmaArm(
    const rp_dma_channel_t *ch,
    const rp_pio_sm_t *smp,
    const PioFifoDir dir,
    uint32_t *buf,
    const uint32_t count)
{
    if (dir == PioFifoRx)
    {
        dmaChannelSetSourceX(ch, reinterpret_cast<uint32_t>(pioSmRxFifoAddrX(smp)));
        dmaChannelSetDestinationX(ch, reinterpret_cast<uint32_t>(buf));
        dmaChannelSetCounterX(ch, count);
        dmaChannelSetModeX(
            ch,
            DMA_CTRL_TRIG_DATA_SIZE_WORD | DMA_CTRL_TRIG_INCR_WRITE | DMA_CTRL_TRIG_TREQ_SEL(pioSmRxDreqX(smp)));
    }
    else
    {
        dmaChannelSetSourceX(ch, reinterpret_cast<uint32_t>(buf));
        dmaChannelSetDestinationX(ch, reinterpret_cast<uint32_t>(pioSmTxFifoAddrX(smp)));
        dmaChannelSetCounterX(ch, count);
        dmaChannelSetModeX(
            ch,
            DMA_CTRL_TRIG_DATA_SIZE_WORD | DMA_CTRL_TRIG_INCR_READ | DMA_CTRL_TRIG_TREQ_SEL(pioSmTxDreqX(smp)));
    }
    dmaChannelEnableInterruptX(ch);
    dmaChannelEnableX(ch);
}

// Shared body of Read and Write.
static HRESULT PioDmaTransfer(CLR_RT_StackFrame &stack, const PioFifoDir dir)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    CLR_RT_HeapBlock hbTimeout{};
    CLR_INT64 *timeoutTicks;
    CLR_RT_HeapBlock_Array *buffer;
    PioDmaWork *work;
    PioDmaWork *ownedWork = nullptr;
    uint32_t dmaStatus = 0;
    bool eventResult = true;
    bool timeoutPushed = false;
    int transferred = 0;
    int offset, count, timeoutMs;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    buffer = stack.Arg1().DereferenceArray();
    offset = stack.Arg2().NumericByRef().s4;
    count = stack.Arg3().NumericByRef().s4;
    timeoutMs = stack.Arg4().NumericByRef().s4;

    FAULT_ON_NULL(buffer);

    if (offset < 0 || count < 0 || timeoutMs < 0 || count > static_cast<int>(buffer->m_numOfElements) ||
        offset > static_cast<int>(buffer->m_numOfElements) - count)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    if (count == 0)
    {
        stack.SetResult_I4(0);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    work = &g_PioDmaWork[ctx.block][ctx.sm][dir];

    if (stack.m_customState != 1)
    {
        ownedWork = work;
    }

    hbTimeout.SetInteger(static_cast<CLR_INT64>(timeoutMs) * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));
    timeoutPushed = true;

    if (stack.m_customState == 1)
    {
        if (work->Channel != nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        const rp_dma_channel_t *ch =
            dmaChannelAlloc(RP_DMA_CHANNEL_ID_ANY, NF_PICO_PIO_IRQ_PRIORITY, PioDmaCallback, work);
        if (ch == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        const auto buf = static_cast<uint32_t *>(platform_malloc(static_cast<size_t>(count) * sizeof(uint32_t)));
        if (buf == nullptr)
        {
            dmaChannelFree(ch);
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        if (dir == PioFifoTx)
        {
            memcpy(
                buf,
                buffer->GetFirstElement() + static_cast<size_t>(offset) * sizeof(uint32_t),
                static_cast<size_t>(count) * sizeof(uint32_t));
        }

        work->Channel = ch;
        work->Buffer = buf;
        work->Count = static_cast<uint32_t>(count);
        work->Status = 0;
        ownedWork = work;

        PioDmaArm(ch, ctx.smp, dir, buf, static_cast<uint32_t>(count));

        Events_Get(SYSTEM_EVENT_FLAG_PICOPIO);
        stack.m_customState = 2;
    }

    while (work->Channel != nullptr && dmaChannelIsBusyX(work->Channel))
    {
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_PicoPio, eventResult));

        if (!eventResult)
        {
            break;
        }
    }

    if (work->Channel != nullptr)
    {
        const uint32_t remaining = NfDmaChannelGetCounter(work->Channel);

        dmaStatus = work->Status;
        transferred = static_cast<int>(work->Count - (remaining > work->Count ? work->Count : remaining));

        if (dir == PioFifoRx && transferred > 0)
        {
            memcpy(
                buffer->GetFirstElement() + static_cast<size_t>(offset) * sizeof(uint32_t),
                work->Buffer,
                static_cast<size_t>(transferred) * sizeof(uint32_t));
        }

        PioDmaFinish(work);
    }

    stack.PopValue();
    timeoutPushed = false;

    if (dmaStatus != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    stack.SetResult_I4(transferred);

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING)
    {
        if (timeoutPushed)
        {
            stack.PopValue();
        }

        if (ownedWork != nullptr)
        {
            PioDmaFinish(ownedWork);
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    NativeInit___VOID__I4__SZARRAY_U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    rp_pio_sm_config_t cfg;
    CLR_RT_HeapBlock_Array *blobArray;
    const unsigned int *b;
    unsigned int gpioBase;
    int offset;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    offset = stack.Arg1().NumericByRef().s4;
    blobArray = stack.Arg2().DereferenceArray();

    FAULT_ON_NULL(blobArray);

    if (offset < 0 || offset >= static_cast<int>(RP_PIO_NUM_INSTR_MEM))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    if (static_cast<int>(blobArray->m_numOfElements) < PIO_CFG_BLOB_LENGTH)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    b = reinterpret_cast<const unsigned int *>(blobArray->GetFirstElement());

#if defined(RP2350)
    gpioBase = (b[PIO_CFG_GPIO_BASE] == 16) ? 16u : 0u;
    ctx.smp->block->pio->GPIOBASE = gpioBase;
#else
    gpioBase = 0;
#endif

    pioSmConfigDefaultX(&cfg);

    pioSmConfigSetClkdivX(&cfg, b[PIO_CFG_CLKDIV_INT] & 0xFFFFu, b[PIO_CFG_CLKDIV_FRAC] & 0xFFu);
    pioSmConfigSetWrapX(&cfg, b[PIO_CFG_WRAP_TARGET] & 0x1Fu, b[PIO_CFG_WRAP] & 0x1Fu);

    pioSmConfigSetOutPinsX(&cfg, PioRelPin(b[PIO_CFG_OUT_BASE], gpioBase), b[PIO_CFG_OUT_COUNT] & 0x3Fu);
    pioSmConfigSetSetPinsX(&cfg, PioRelPin(b[PIO_CFG_SET_BASE], gpioBase), b[PIO_CFG_SET_COUNT] & 0x7u);
    pioSmConfigSetInPinsX(&cfg, PioRelPin(b[PIO_CFG_IN_BASE], gpioBase));
    pioSmConfigSetJmpPinX(&cfg, PioRelPin(b[PIO_CFG_JMP_PIN], gpioBase));

    pioSmConfigSetSidesetX(
        &cfg,
        (b[PIO_CFG_SIDESET_COUNT] + b[PIO_CFG_SIDESET_OPT]) & 0x7u,
        b[PIO_CFG_SIDESET_OPT] != 0,
        b[PIO_CFG_SIDESET_PINDIRS] != 0);
    pioSmConfigSetSidesetPinsX(&cfg, PioRelPin(b[PIO_CFG_SIDESET_BASE], gpioBase));

    pioSmConfigSetInShiftX(
        &cfg,
        b[PIO_CFG_IN_SHIFT_RIGHT] != 0,
        b[PIO_CFG_AUTOPUSH] != 0,
        PioShiftThreshold(b[PIO_CFG_PUSH_THRESHOLD]));
    pioSmConfigSetOutShiftX(
        &cfg,
        b[PIO_CFG_OUT_SHIFT_RIGHT] != 0,
        b[PIO_CFG_AUTOPULL] != 0,
        PioShiftThreshold(b[PIO_CFG_PULL_THRESHOLD]));

    NfPioSmConfigSetFifoJoin(&cfg, b[PIO_CFG_FIFO_JOIN]);

    pioSmConfigSetMovStatusX(
        &cfg,
        static_cast<rp_pio_mov_status_t>(b[PIO_CFG_MOV_STATUS_SEL]),
        b[PIO_CFG_MOV_STATUS_N] & (PIO_SM_EXECCTRL_STATUS_N_Msk >> PIO_SM_EXECCTRL_STATUS_N_Pos));

    pioSmConfigSetOutSpecialX(
        &cfg,
        b[PIO_CFG_OUT_STICKY] != 0,
        b[PIO_CFG_INLINE_OUT_EN] != 0,
        b[PIO_CFG_OUT_EN_SEL] & 0x1Fu);

    pioSmInit(ctx.smp, static_cast<uint32_t>(offset), &cfg);

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

    PioSmContext ctx{};
    bool value;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    value = stack.Arg1().NumericByRef().u1;

    stack.This()[FIELD___enabled].NumericByRef().u1 = value;

    if (value)
    {
        pioSmEnableX(ctx.smp);
    }
    else
    {
        pioSmDisableX(ctx.smp);
    }

    NANOCLR_NOCLEANUP();
}

static inline bool PioFifoBlocked(const rp_pio_sm_t *smp, const PioFifoDir dir)
{
    return (dir == PioFifoTx) ? pioSmIsTxFullX(smp) : pioSmIsRxEmptyX(smp);
}

static HRESULT PioFifoTransfer(CLR_RT_StackFrame &stack, const PioFifoDir dir)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    CLR_RT_HeapBlock hbTimeout{};
    CLR_INT64 *timeoutTicks;
    const rp_pio_block_t *blockp = nullptr;
    uint32_t inteMask = 0;
    bool eventResult = true;
    bool timeoutPushed = false;
    bool interruptEnabled = false;
    bool blocked;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    blockp = &__rp_pio_blocks[ctx.block];
    inteMask = (dir == PioFifoTx) ? PIO_IRQ_TXNFULL(ctx.sm) : PIO_IRQ_RXNEMPTY(ctx.sm);

    hbTimeout.SetInteger(static_cast<CLR_INT64>(NF_PICO_PIO_FIFO_TIMEOUT_MS) * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));
    timeoutPushed = true;

    if (stack.m_customState == 1)
    {
        Events_Get(SYSTEM_EVENT_FLAG_PICOPIO);
        stack.m_customState = 2;
    }

    while (PioFifoBlocked(ctx.smp, dir))
    {
        NfPioBlockEnableInterrupt(blockp, inteMask);
        interruptEnabled = true;

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_PicoPio, eventResult));

        if (!eventResult)
        {
            break;
        }
    }

    NfPioBlockDisableInterrupt(blockp, inteMask);
    interruptEnabled = false;

    blocked = PioFifoBlocked(ctx.smp, dir);

    stack.PopValue();
    timeoutPushed = false;

    if (blocked)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    if (dir == PioFifoTx)
    {
        pioSmPutX(ctx.smp, stack.Arg1().NumericByRef().u4);
    }
    else
    {
        stack.SetResult_U4(pioSmGetX(ctx.smp));
    }

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING)
    {
        if (interruptEnabled)
        {
            NfPioBlockDisableInterrupt(blockp, inteMask);
        }

        if (timeoutPushed)
        {
            stack.PopValue();
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Put___VOID__U4(
    CLR_RT_StackFrame &stack)
{
    return PioFifoTransfer(stack, PioFifoTx);
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Get___U4(
    CLR_RT_StackFrame &stack)
{
    return PioFifoTransfer(stack, PioFifoRx);
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    TryPut___BOOLEAN__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    if (pioSmIsTxFullX(ctx.smp))
    {
        stack.SetResult_Boolean(false);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    pioSmPutX(ctx.smp, stack.Arg1().NumericByRef().u4);
    stack.SetResult_Boolean(true);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    TryGet___BOOLEAN__BYREF_U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    CLR_RT_HeapBlock *value;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    value = stack.Arg1().Dereference();
    FAULT_ON_NULL(value);

    if (pioSmIsRxEmptyX(ctx.smp))
    {
        value->NumericByRef().u4 = 0;
        stack.SetResult_Boolean(false);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    value->NumericByRef().u4 = pioSmGetX(ctx.smp);
    stack.SetResult_Boolean(true);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_IsTxFull___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_Boolean(pioSmIsTxFullX(ctx.smp));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_IsRxEmpty___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_Boolean(pioSmIsRxEmptyX(ctx.smp));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    NativeUnclaim___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_TEARDOWN_PROLOGUE(ctx);

    pioSmDisableX(ctx.smp);
    PioDmaReleaseSm(ctx.block, ctx.sm);
    pioSmFree(ctx.smp);
    g_AllocatedSMs[ctx.block][ctx.sm] = nullptr;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    SetConsecutivePinDirs___VOID__I4__I4__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    int basePin, count, gpioBase;
    bool output;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    basePin = stack.Arg1().NumericByRef().s4;
    count = stack.Arg2().NumericByRef().s4;
    output = stack.Arg3().NumericByRef().u1 != 0;

    if (count == 0)
    {
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

#if defined(RP2350)
    gpioBase = static_cast<int>(ctx.smp->block->pio->GPIOBASE);
#else
    gpioBase = 0;
#endif

    if (basePin < 0 || basePin > PIO_MAX_PIN || count < 0 || count > 32 || (basePin + count) > (PIO_MAX_PIN + 1) ||
        basePin < gpioBase || (basePin - gpioBase + count) > 32)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    pioSmSetConsecutivePindirsX(ctx.smp, static_cast<uint32_t>(basePin), static_cast<uint32_t>(count), output);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::ClearFifos___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    pioSmClearFifosX(ctx.smp);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::DrainTxFifo___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    if (!NfPioSmDrainTxFifo(ctx.smp, NF_PICO_PIO_DRAIN_LIMIT))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Restart___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    pioSmRestartX(ctx.smp);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    ClockDivRestart___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    pioSmClkdivRestartX(ctx.smp);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Exec___VOID__U2(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    pioSmExecX(ctx.smp, stack.Arg1().NumericByRef().u2);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::get_TxLevel___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_U4(pioSmTxFifoLevelX(ctx.smp));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::get_RxLevel___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_U4(pioSmRxFifoLevelX(ctx.smp));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_ProgramCounter___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_U4(pioSmGetAddrX(ctx.smp) & 0x1Fu);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_ClockDivisor___R4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    uint32_t clkdiv, intPart, frac;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    clkdiv = ctx.smp->block->pio->SM[ctx.smp->smidx].CLKDIV;
    intPart = (clkdiv & PIO_SM_CLKDIV_INT_Msk) >> PIO_SM_CLKDIV_INT_Pos;
    frac = (clkdiv & PIO_SM_CLKDIV_FRAC_Msk) >> PIO_SM_CLKDIV_FRAC_Pos;

    stack.SetResult_R4((intPart == 0u ? 65536.0f : static_cast<float>(intPart)) + static_cast<float>(frac) / 256.0f);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    set_ClockDivisor___VOID__R4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    int intPart, frac;
    float value;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    value = stack.Arg1().NumericByRef().r4;

    if (!(value >= 1.0f && value <= 65536.0f))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    intPart = static_cast<int>(value);
    frac = static_cast<int>(roundf((value - static_cast<float>(intPart)) * 256.0f));
    if (frac > 255)
    {
        frac = 0;
        intPart += 1;
    }

    pioSmSetClkdivX(ctx.smp, PIO_SM_CLKDIV(intPart >= 65536 ? 0 : intPart, frac));
    pioSmClkdivRestartX(ctx.smp);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    Read___I4__SZARRAY_U4__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    return PioDmaTransfer(stack, PioFifoRx);
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    Write___I4__SZARRAY_U4__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    return PioDmaTransfer(stack, PioFifoTx);
}
