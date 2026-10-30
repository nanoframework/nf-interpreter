//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_pico.h"
#include "nanoFramework_hardware_pico_target.h"
#include <hal.h>
#include <nanoHAL.h>
#include <cmath>
#include <cstring>

typedef Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine PioStateMachine;
typedef Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioBlock PioBlock;

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

enum PioFifoDir
{
    PioFifoRx = 0,
    PioFifoTx = 1,
    PioFifoDirCount = 2,
};

struct PioSmContext
{
    const rp_pio_sm_t *stateMachine;
    int block;
    int sm;
};

struct PioDmaWork
{
    const rp_dma_channel_t *Channel;
    uint32_t *Buffer;
    uint32_t Count;
    uint32_t Status;
};

#define PIO_FIELD_MASK(field) ((field##_Msk) >> (field##_Pos))

static constexpr unsigned int PioPinWindowSize = 32u;
static constexpr unsigned int PioHighGpioBase = 16u;
static constexpr unsigned int PioShiftRegisterBits = 32u;
static constexpr int PioClockDivisorMax = 65536;
static constexpr int PioClockDivisorFracSteps = 256;

static PioDmaWork g_PioDmaWork[PIO_MAX_BLOCK + 1][RP_PIO_NUM_STATE_MACHINES][PioFifoDirCount] = {};

static unsigned int PioRelPin(unsigned int gpio, unsigned int gpioBase)
{
    if (gpio >= gpioBase && (gpio - gpioBase) < PioPinWindowSize)
    {
        return gpio - gpioBase;
    }

    return gpio % PioPinWindowSize;
}

static unsigned int PioShiftThreshold(unsigned int threshold)
{
    return (threshold == 0u || threshold > PioShiftRegisterBits) ? PioShiftRegisterBits : threshold;
}

static bool PioFifoBlocked(const rp_pio_sm_t *stateMachine, const PioFifoDir dir)
{
    return dir == PioFifoTx ? pioSmIsTxFullX(stateMachine) : pioSmIsRxEmptyX(stateMachine);
}

static HRESULT PioGetSmContext(CLR_RT_HeapBlock *pThis, PioSmContext &ctx, const bool allowDisposed = false)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pPioBlock;

    FAULT_ON_NULL(pThis);

    if (!allowDisposed && pThis[PioStateMachine::FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    pPioBlock = pThis[PioStateMachine::FIELD___block].Dereference();
    FAULT_ON_NULL(pPioBlock);

    ctx.block = pPioBlock[PioBlock::FIELD___index].NumericByRef().s4;
    ctx.sm = pThis[PioStateMachine::FIELD___sm].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(ctx.block);
    VALIDATE_SM(ctx.sm);

    // the allocation mask is what tells a claimed state machine from a released one
    if ((pioGetSmAllocatedMask(RP_PIO_BLOCK(ctx.block)) & (1u << ctx.sm)) == 0u)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    ctx.stateMachine = pioGetSmHandleX(RP_PIO_BLOCK(ctx.block), static_cast<uint32_t>(ctx.sm));

    NANOCLR_NOCLEANUP();
}

#define NANOCLR_PIO_SM_PROLOGUE(ctx)                                                                                   \
    {                                                                                                                  \
        NANOCLR_CHECK_HRESULT(PioGetSmContext(stack.This(), ctx));                                                     \
    }

#define NANOCLR_PIO_SM_TEARDOWN_PROLOGUE(ctx)                                                                          \
    {                                                                                                                  \
        NANOCLR_CHECK_HRESULT(PioGetSmContext(stack.This(), ctx, true));                                               \
    }

#define PIO_DMA_ERROR_MASK (DMA_CTRL_TRIG_AHB_ERROR | DMA_CTRL_TRIG_READ_ERROR | DMA_CTRL_TRIG_WRITE_ERROR)

static void PioDmaCallback(void *param, const uint32_t ctrlTrig)
{
    if (auto *work = static_cast<PioDmaWork *>(param); work != nullptr)
    {
        work->Status = ctrlTrig & PIO_DMA_ERROR_MASK;
    }

    Events_Set(SYSTEM_EVENT_FLAG_PICOPIO);
}

static void PioDmaFinish(PioDmaWork *work)
{
    const rp_dma_channel_t *channel = work->Channel;

    if (channel == nullptr)
    {
        return;
    }

    if (dmaChannelIsBusyX(channel))
    {
        dmaChannelDisableX(channel);
    }

    (void)dmaChannelGetAndClearInterrupts(channel);
    dmaChannelFree(channel);

    if (work->Buffer != nullptr)
    {
        platform_free(work->Buffer);
    }

    work->Channel = nullptr;
    work->Buffer = nullptr;
    work->Count = 0;
    work->Status = 0;
}

static void PioDmaReleaseSm(const int block, const int sm)
{
    PioDmaFinish(&g_PioDmaWork[block][sm][PioFifoRx]);
    PioDmaFinish(&g_PioDmaWork[block][sm][PioFifoTx]);
}

static void PioDmaArm(
    const rp_dma_channel_t *channel,
    const rp_pio_sm_t *pioStateMachine,
    const PioFifoDir direction,
    uint32_t *buffer,
    const uint32_t count)
{
    if (direction == PioFifoRx)
    {
        dmaChannelSetSourceX(channel, reinterpret_cast<uint32_t>(pioSmRxFifoAddrX(pioStateMachine)));
        dmaChannelSetDestinationX(channel, reinterpret_cast<uint32_t>(buffer));
        dmaChannelSetCounterX(channel, count);
        dmaChannelSetModeX(
            channel,
            DMA_CTRL_TRIG_DATA_SIZE_WORD | DMA_CTRL_TRIG_INCR_WRITE | DMA_CTRL_TRIG_TREQ_SEL(pioSmRxDreqX(pioStateMachine)));
    }
    else
    {
        dmaChannelSetSourceX(channel, reinterpret_cast<uint32_t>(buffer));
        dmaChannelSetDestinationX(channel, reinterpret_cast<uint32_t>(pioSmTxFifoAddrX(pioStateMachine)));
        dmaChannelSetCounterX(channel, count);
        dmaChannelSetModeX(
            channel,
            DMA_CTRL_TRIG_DATA_SIZE_WORD | DMA_CTRL_TRIG_INCR_READ | DMA_CTRL_TRIG_TREQ_SEL(pioSmTxDreqX(pioStateMachine)));
    }
    dmaChannelEnableInterruptX(channel);
    dmaChannelEnableX(channel);
}

static HRESULT PioDmaTransfer(CLR_RT_StackFrame &stack, const PioFifoDir dir)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    CLR_RT_HeapBlock hbTimeout{};
    CLR_INT64 *timeoutTicks;
    CLR_RT_HeapBlock_Array *buffer;
    const rp_dma_channel_t *channel;
    uint32_t *bounceBuffer;
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

        channel = dmaChannelAlloc(RP_DMA_CHANNEL_ID_ANY, NF_PICO_PIO_IRQ_PRIORITY, PioDmaCallback, work);
        if (channel == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        bounceBuffer = static_cast<uint32_t *>(platform_malloc(static_cast<size_t>(count) * sizeof(uint32_t)));
        if (bounceBuffer == nullptr)
        {
            dmaChannelFree(channel);
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        if (dir == PioFifoTx)
        {
            memcpy(
                bounceBuffer,
                buffer->GetFirstElement() + static_cast<size_t>(offset) * sizeof(uint32_t),
                static_cast<size_t>(count) * sizeof(uint32_t));
        }

        work->Channel = channel;
        work->Buffer = bounceBuffer;
        work->Count = static_cast<uint32_t>(count);
        work->Status = 0;
        ownedWork = work;

        PioDmaArm(channel, ctx.stateMachine, dir, bounceBuffer, static_cast<uint32_t>(count));

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
        const uint32_t remaining = dmaChannelGetCounterX(work->Channel);

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

static HRESULT PioFifoTransfer(CLR_RT_StackFrame &stack, const PioFifoDir dir)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    CLR_RT_HeapBlock hbTimeout{};
    CLR_INT64 *timeoutTicks;
    uint32_t inteMask = 0;
    bool eventResult = true;
    bool timeoutPushed = false;
    bool interruptEnabled = false;
    bool blocked;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    inteMask = (dir == PioFifoTx) ? PIO_IRQ_TXNFULL(ctx.sm) : PIO_IRQ_RXNEMPTY(ctx.sm);

    hbTimeout.SetInteger(static_cast<CLR_INT64>(NF_PICO_PIO_FIFO_TIMEOUT_MS) * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));
    timeoutPushed = true;

    if (stack.m_customState == 1)
    {
        Events_Get(SYSTEM_EVENT_FLAG_PICOPIO);
        stack.m_customState = 2;
    }

    while (PioFifoBlocked(ctx.stateMachine, dir))
    {
        pioSmEnableInterruptX(ctx.stateMachine, inteMask);
        interruptEnabled = true;

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_PicoPio, eventResult));

        if (!eventResult)
        {
            break;
        }
    }

    pioSmDisableInterruptX(ctx.stateMachine, inteMask);
    interruptEnabled = false;

    blocked = PioFifoBlocked(ctx.stateMachine, dir);

    stack.PopValue();
    timeoutPushed = false;

    if (blocked)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    if (dir == PioFifoTx)
    {
        pioSmPutX(ctx.stateMachine, stack.Arg1().NumericByRef().u4);
    }
    else
    {
        stack.SetResult_U4(pioSmGetX(ctx.stateMachine));
    }

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING)
    {
        if (interruptEnabled)
        {
            pioSmDisableInterruptX(ctx.stateMachine, inteMask);
        }

        if (timeoutPushed)
        {
            stack.PopValue();
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    NativeInit___VOID__I4__SZARRAY_U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    rp_pio_sm_config_t cfg;
    CLR_RT_HeapBlock_Array *blobArray;
    const unsigned int *base;
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

    base = reinterpret_cast<const unsigned int *>(blobArray->GetFirstElement());

#if RP_PIO_HAS_GPIOBASE == TRUE
    gpioBase = (base[PIO_CFG_GPIO_BASE] == PioHighGpioBase) ? PioHighGpioBase : 0u;
    ctx.stateMachine->block->pio->GPIOBASE = gpioBase;
#else
    gpioBase = 0;
#endif

    pioSmConfigDefaultX(&cfg);

    pioSmConfigSetClkdivX(
        &cfg,
        base[PIO_CFG_CLKDIV_INT] & PIO_FIELD_MASK(PIO_SM_CLKDIV_INT),
        base[PIO_CFG_CLKDIV_FRAC] & PIO_FIELD_MASK(PIO_SM_CLKDIV_FRAC));
    pioSmConfigSetWrapX(
        &cfg,
        base[PIO_CFG_WRAP_TARGET] & PIO_FIELD_MASK(PIO_SM_EXECCTRL_WRAP_BOTTOM),
        base[PIO_CFG_WRAP] & PIO_FIELD_MASK(PIO_SM_EXECCTRL_WRAP_TOP));

    pioSmConfigSetOutPinsX(
        &cfg,
        PioRelPin(base[PIO_CFG_OUT_BASE], gpioBase),
        base[PIO_CFG_OUT_COUNT] & PIO_FIELD_MASK(PIO_SM_PINCTRL_OUT_COUNT));
    pioSmConfigSetSetPinsX(
        &cfg,
        PioRelPin(base[PIO_CFG_SET_BASE], gpioBase),
        base[PIO_CFG_SET_COUNT] & PIO_FIELD_MASK(PIO_SM_PINCTRL_SET_COUNT));
    pioSmConfigSetInPinsX(&cfg, PioRelPin(base[PIO_CFG_IN_BASE], gpioBase));
    pioSmConfigSetJmpPinX(&cfg, PioRelPin(base[PIO_CFG_JMP_PIN], gpioBase));

    pioSmConfigSetSidesetX(
        &cfg,
        (base[PIO_CFG_SIDESET_COUNT] + base[PIO_CFG_SIDESET_OPT]) & PIO_FIELD_MASK(PIO_SM_PINCTRL_SIDESET_COUNT),
        base[PIO_CFG_SIDESET_OPT] != 0,
        base[PIO_CFG_SIDESET_PINDIRS] != 0);
    pioSmConfigSetSidesetPinsX(&cfg, PioRelPin(base[PIO_CFG_SIDESET_BASE], gpioBase));

    pioSmConfigSetInShiftX(
        &cfg,
        base[PIO_CFG_IN_SHIFT_RIGHT] != 0,
        base[PIO_CFG_AUTOPUSH] != 0,
        PioShiftThreshold(base[PIO_CFG_PUSH_THRESHOLD]));
    pioSmConfigSetOutShiftX(
        &cfg,
        base[PIO_CFG_OUT_SHIFT_RIGHT] != 0,
        base[PIO_CFG_AUTOPULL] != 0,
        PioShiftThreshold(base[PIO_CFG_PULL_THRESHOLD]));

    // the LLD takes a legal joining mode for granted and its debug checks are compiled out here,
    // so the value coming from the managed side is screened before it reaches SHIFTCTRL
    switch (base[PIO_CFG_FIFO_JOIN])
    {
        case PioFifoJoin_None:
        case PioFifoJoin_Tx:
        case PioFifoJoin_Rx:
            break;

        case PioFifoJoin_TxGet:
        case PioFifoJoin_TxPut:
        case PioFifoJoin_PutGet:
#if defined(RP2350)
            break;
#else
            // the RXF put/get modes only exist on the RP2350
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    pioSmConfigSetFifoJoinX(&cfg, static_cast<rp_pio_fifo_join_t>(base[PIO_CFG_FIFO_JOIN]));

    pioSmConfigSetMovStatusX(
        &cfg,
        static_cast<rp_pio_mov_status_t>(base[PIO_CFG_MOV_STATUS_SEL]),
        base[PIO_CFG_MOV_STATUS_N] & PIO_FIELD_MASK(PIO_SM_EXECCTRL_STATUS_N));

    pioSmConfigSetOutSpecialX(
        &cfg,
        base[PIO_CFG_OUT_STICKY] != 0,
        base[PIO_CFG_INLINE_OUT_EN] != 0,
        base[PIO_CFG_OUT_EN_SEL] & PIO_FIELD_MASK(PIO_SM_EXECCTRL_OUT_EN_SEL));

    pioSmInit(ctx.stateMachine, static_cast<uint32_t>(offset), &cfg);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_Enabled___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    VALIDATE_NOT_DISPOSED(pThis);

    stack.SetResult_Boolean(pThis[FIELD___enabled].NumericByRef().u1);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
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
        pioSmEnableX(ctx.stateMachine);
    }
    else
    {
        pioSmDisableX(ctx.stateMachine);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Put___VOID__U4(
    CLR_RT_StackFrame &stack)
{
    return PioFifoTransfer(stack, PioFifoTx);
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Get___U4(
    CLR_RT_StackFrame &stack)
{
    return PioFifoTransfer(stack, PioFifoRx);
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    TryPut___BOOLEAN__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    if (pioSmIsTxFullX(ctx.stateMachine))
    {
        stack.SetResult_Boolean(false);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    pioSmPutX(ctx.stateMachine, stack.Arg1().NumericByRef().u4);
    stack.SetResult_Boolean(true);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    TryGet___BOOLEAN__BYREF_U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    CLR_RT_HeapBlock *value;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    value = stack.Arg1().Dereference();
    FAULT_ON_NULL(value);

    if (pioSmIsRxEmptyX(ctx.stateMachine))
    {
        value->NumericByRef().u4 = 0;
        stack.SetResult_Boolean(false);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    value->NumericByRef().u4 = pioSmGetX(ctx.stateMachine);
    stack.SetResult_Boolean(true);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_IsTxFull___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_Boolean(pioSmIsTxFullX(ctx.stateMachine));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_IsRxEmpty___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_Boolean(pioSmIsRxEmptyX(ctx.stateMachine));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    NativeUnclaim___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_TEARDOWN_PROLOGUE(ctx);

    pioSmDisableX(ctx.stateMachine);
    PioDmaReleaseSm(ctx.block, ctx.sm);

    // clearing the allocation bit is what makes a later call see this instance as released
    pioSmFree(ctx.stateMachine);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
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

#if RP_PIO_HAS_GPIOBASE == TRUE
    gpioBase = static_cast<int>(pioGetGpioBaseX(ctx.stateMachine->block));
#else
    gpioBase = 0;
#endif

    if (basePin < 0 || basePin > PIO_MAX_PIN || count < 0 || count > static_cast<int>(PioPinWindowSize) ||
        (basePin + count) > (PIO_MAX_PIN + 1) || basePin < gpioBase ||
        (basePin - gpioBase + count) > static_cast<int>(PioPinWindowSize))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    pioSmSetConsecutivePindirsX(ctx.stateMachine, static_cast<uint32_t>(basePin), static_cast<uint32_t>(count), output);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::ClearFifos___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    pioSmClearFifosX(ctx.stateMachine);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::DrainTxFifo___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    if (!pioSmDrainTxFifoX(ctx.stateMachine, NF_PICO_PIO_DRAIN_LIMIT))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Restart___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    pioSmRestartX(ctx.stateMachine);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    ClockDivRestart___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    pioSmClkdivRestartX(ctx.stateMachine);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::Exec___VOID__U2(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    pioSmExecX(ctx.stateMachine, stack.Arg1().NumericByRef().u2);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::get_TxLevel___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_U4(pioSmTxFifoLevelX(ctx.stateMachine));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::get_RxLevel___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_U4(pioSmRxFifoLevelX(ctx.stateMachine));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_ProgramCounter___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    stack.SetResult_U4(pioSmGetAddrX(ctx.stateMachine) % RP_PIO_NUM_INSTR_MEM);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    get_ClockDivisor___R4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    uint32_t clkdiv, intPart, frac;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    clkdiv = pioSmGetClkdivX(ctx.stateMachine);
    intPart = (clkdiv & PIO_SM_CLKDIV_INT_Msk) >> PIO_SM_CLKDIV_INT_Pos;
    frac = (clkdiv & PIO_SM_CLKDIV_FRAC_Msk) >> PIO_SM_CLKDIV_FRAC_Pos;

    stack.SetResult_R4(
        static_cast<float>(intPart == 0u ? PioClockDivisorMax : intPart) +
        static_cast<float>(frac) / PioClockDivisorFracSteps);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    set_ClockDivisor___VOID__R4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    PioSmContext ctx{};
    int intPart, frac;
    float value;

    NANOCLR_PIO_SM_PROLOGUE(ctx);

    value = stack.Arg1().NumericByRef().r4;

    if (!(value >= 1.0f && value <= static_cast<float>(PioClockDivisorMax)))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    intPart = static_cast<int>(value);
    frac = static_cast<int>(roundf((value - static_cast<float>(intPart)) * PioClockDivisorFracSteps));
    if (frac >= PioClockDivisorFracSteps)
    {
        frac = 0;
        intPart += 1;
    }

    pioSmSetClkdivX(ctx.stateMachine, PIO_SM_CLKDIV(intPart >= PioClockDivisorMax ? 0 : intPart, frac));
    pioSmClkdivRestartX(ctx.stateMachine);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    Read___I4__SZARRAY_U4__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    return PioDmaTransfer(stack, PioFifoRx);
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
    Write___I4__SZARRAY_U4__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    return PioDmaTransfer(stack, PioFifoTx);
}
