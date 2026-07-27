//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H
#define NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H

#include <ch.h>
#include <hal.h>
#include <nanoHAL.h>
#include <nanoFramework_hardware_pico_native.h>

//
// Everything the PIO binding needs on top of the ChibiOS RP low level drivers, and nothing else:
// the boilerplate that turns a CLR_RT_StackFrame into a validated state machine handle, plus the
// handful of operations the LLD does not expose yet.
//
// Each of those is marked "LLD gap" and mirrors a change proposed upstream in the
// begeistert/chibios branches enhance-pico-pio-api / enhance-pico-dma-api. When one lands the
// workaround is deleted and the call is replaced by the LLD function of the same shape.
//
// What ChibiOS already does is called, never reimplemented: the DMA abort sequence with the
// RP2040-E13 / RP2350-E5 workarounds, the state machine init sequence, the SET PINDIRS chunking,
// JMP relocation on program load.
//

// ---------------------------------------------------------------------------
// Target constants
// ---------------------------------------------------------------------------

#if defined(RP2350)
#define PIO_MAX_BLOCK 2
#define PIO_MAX_PIN   47
#else
#define PIO_MAX_BLOCK 1
#define PIO_MAX_PIN   29
#endif

#if defined(TARGET_HAS_WIFI)
#define PIO_MIN_BLOCK 1
#else
#define PIO_MIN_BLOCK 0
#endif

// NVIC priority requested for the PIO and DMA shared vectors.
//
// Both pioSmAlloc() and dmaChannelAlloc() apply the priority only on the first allocation a core
// makes, and ignore it afterwards: for PIO that is per block, for DMA it is global. So this is
// the effective priority only if this binding is the first PIO/DMA consumer to start. On a WiFi
// target the CYW43 bus driver claims a PIO0 state machine with priority 0 first, which is why
// PIO_MIN_BLOCK keeps block 0 out of reach of managed code.
#define NF_PICO_PIO_IRQ_PRIORITY 3

// Budget for a blocking single word FIFO transfer: a tight spin first, because a state machine
// that is running usually frees a FIFO slot within a few cycles, then millisecond sleeps up to
// the timeout so a stalled state machine cannot hold the CLR thread indefinitely.
#define NF_PICO_PIO_FIFO_SPIN_LIMIT 256u
#define NF_PICO_PIO_FIFO_TIMEOUT_MS 1000u

// Instructions DrainTxFifo executes before giving up. The loop only ends if the state machine
// consumes what is exec'd into it, so this bounds the case where it is stalled on something else.
#define NF_PICO_PIO_DRAIN_LIMIT 0x10000u

// ---------------------------------------------------------------------------
// Argument validation
// ---------------------------------------------------------------------------

#define VALIDATE_PIO_BLOCK(block)                                                                                      \
    if ((block) < 0 || (block) > PIO_MAX_BLOCK)                                                                        \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);                                                                \
    }                                                                                                                  \
    if ((block) < PIO_MIN_BLOCK)                                                                                       \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);                                                                    \
    }

#define VALIDATE_SM(sm)                                                                                                \
    if ((sm) < 0 || (sm) > 3)                                                                                          \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);                                                                \
    }

#define VALIDATE_NOT_DISPOSED(pThis)                                                                                   \
    FAULT_ON_NULL(pThis);                                                                                              \
    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)                                                                \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);                                                                  \
    }

// ---------------------------------------------------------------------------
// Shared state
// ---------------------------------------------------------------------------

// Handles returned by pioSmAlloc() at claim time, indexed [block][sm]. A null entry means the
// state machine is not claimed, so this doubles as the claim bookkeeping.
extern const rp_pio_sm_t *g_AllocatedSMs[3][4];

// Aborts and releases any DMA work in flight on a state machine. Defined in the PioStateMachine
// native so NativeUnclaim can stop the transfer before the state machine handle goes away.
void PioDmaReleaseSm(int block, int sm);

// ---------------------------------------------------------------------------
// State machine context
// ---------------------------------------------------------------------------

// Everything a native method needs about the state machine it was called on.
struct PioSmContext
{
    const rp_pio_sm_t *smp;
    int block;
    int sm;
};

// Resolves the PioBlock/sm fields of a PioStateMachine instance into a PioSmContext.
//
// Returns CLR_E_OBJECT_DISPOSED both for a disposed instance and for a state machine that is no
// longer claimed, which is what callers hit after Dispose(); every other failure is a bad field
// value and maps to CLR_E_INVALID_PARAMETER.
inline HRESULT PioGetSmContext(CLR_RT_HeapBlock *pThis, PioSmContext &ctx, bool allowDisposed = false)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pPioBlock;

    FAULT_ON_NULL(pThis);

    if (!allowDisposed && pThis[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
                                    FIELD___disposed]
                                  .NumericByRef()
                                  .u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    pPioBlock =
        pThis[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::FIELD___block]
            .Dereference();
    FAULT_ON_NULL(pPioBlock);

    ctx.block =
        pPioBlock[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::FIELD___index]
            .NumericByRef()
            .s4;
    ctx.sm =
        pThis[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::FIELD___sm]
            .NumericByRef()
            .s4;

    VALIDATE_PIO_BLOCK(ctx.block);
    VALIDATE_SM(ctx.sm);

    ctx.smp = g_AllocatedSMs[ctx.block][ctx.sm];
    if (ctx.smp == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    NANOCLR_NOCLEANUP();
}

// Opens a PioStateMachine native method: rejects a disposed instance and resolves the context.
//
// The PioSmContext is declared by the caller alongside its other locals rather than here, so
// that the goto this macro can take never jumps over an initialization.
#define NANOCLR_PIO_SM_PROLOGUE(ctx)                                                                                   \
    {                                                                                                                  \
        NANOCLR_CHECK_HRESULT(PioGetSmContext(stack.This(), ctx));                                                     \
    }

#define NANOCLR_PIO_SM_TEARDOWN_PROLOGUE(ctx)                                                                          \
    {                                                                                                                  \
        NANOCLR_CHECK_HRESULT(PioGetSmContext(stack.This(), ctx, true));                                               \
    }

// ---------------------------------------------------------------------------
// LLD gap: PIO instruction encoding
// ---------------------------------------------------------------------------
//
// pioSmExecX() takes a raw 16-bit instruction and ChibiOS offers no way to build one, so every
// consumer ends up with bare literals -- the CYW43 bus driver grew its own set in
// targets/ChibiOS/_WiFi/cyw43/cyw43_bus_pio_spi.c:116-123.
//

// OUT NULL, 32 -- discards a whole OSR word, used to drain a TX FIFO under autopull.
#define NF_PIO_INSTR_OUT_NULL_32 0x6060u
// PULL noblock -- discards a TX FIFO word when autopull is off.
//
// The PULL opcode is 0x8080: bit 7 is what distinguishes it from PUSH, which is 0x8000. This
// used to be encoded as 0x8000, so the non-autopull path of DrainTxFifo was executing PUSH
// noblock, pushing the ISR into the RX FIFO instead of discarding a TX FIFO word -- the TX FIFO
// never drained and the RX FIFO filled with garbage.
#define NF_PIO_INSTR_PULL_NOBLOCK 0x8080u

// ---------------------------------------------------------------------------
// LLD gap: FIFO joining beyond the three modes ChibiOS models
// ---------------------------------------------------------------------------
//
// rp_pio_fifo_join_t only has NONE/TX/RX, so pioSmConfigSetFifoJoinX() cannot express the RP2350
// RXF_PUTGET modes that live in SHIFTCTRL bits 14/15. The managed PioFifoJoin enum does expose
// them, so the low two bits go through the LLD builder and the high two are applied here.
//
// On the RP2040 those bits do not exist; writing them was harmless but wrong, so they are
// dropped rather than written through.
//

#if defined(RP2350)
#define NF_PIO_SM_SHIFTCTRL_FJOIN_RX_GET (1u << 14)
#define NF_PIO_SM_SHIFTCTRL_FJOIN_RX_PUT (1u << 15)
#endif

// Applies the managed PioFifoJoin value to a configuration image.
//
// Low two bits are the plain TX/RX joins, high two bits the RP2350 put/get variants.
inline void NfPioSmConfigSetFifoJoin(rp_pio_sm_config_t *cfgp, unsigned int join)
{
    pioSmConfigSetFifoJoinX(cfgp, static_cast<rp_pio_fifo_join_t>(join & 3u));

#if defined(RP2350)
    cfgp->shiftctrl &= ~(NF_PIO_SM_SHIFTCTRL_FJOIN_RX_GET | NF_PIO_SM_SHIFTCTRL_FJOIN_RX_PUT);

    if (join & 4u)
    {
        cfgp->shiftctrl |= NF_PIO_SM_SHIFTCTRL_FJOIN_RX_GET;
    }
    if (join & 8u)
    {
        cfgp->shiftctrl |= NF_PIO_SM_SHIFTCTRL_FJOIN_RX_PUT;
    }
#endif
}

// ---------------------------------------------------------------------------
// LLD gap: block level interrupt enables and IRQ flags
// ---------------------------------------------------------------------------
//
// The PIO_IRQ_SM(n) flags belong to the block, not to a state machine, but ChibiOS only offers
// pioSmEnableInterruptX()/pioSmDisableInterruptX(). These use the same atomic SET/CLR aliases and
// the same "enable on the current core, disable on both" convention.
//

inline void NfPioBlockEnableInterrupt(const rp_pio_block_t *block, uint32_t mask)
{
    if (SIO->CPUID == 0u)
    {
        block->pio->SET.IRQ0_INTE = mask;
    }
    else
    {
        block->pio->SET.IRQ1_INTE = mask;
    }
}

inline void NfPioBlockDisableInterrupt(const rp_pio_block_t *block, uint32_t mask)
{
    block->pio->CLR.IRQ0_INTE = mask;
    block->pio->CLR.IRQ1_INTE = mask;
}

// Raises a PIO IRQ flag from the CPU side (IRQ_FORCE is write-only, one bit per flag).
inline void NfPioBlockForceIrq(const rp_pio_block_t *block, uint32_t irq)
{
    block->pio->IRQ_FORCE = 1u << irq;
}

// Clears a PIO IRQ flag (the IRQ register is write-1-to-clear).
inline void NfPioBlockClearIrq(const rp_pio_block_t *block, uint32_t irq)
{
    block->pio->IRQ = 1u << irq;
}

// ---------------------------------------------------------------------------
// LLD gap: routing a GPIO to a block that may have no claimed state machine
// ---------------------------------------------------------------------------
//
// FUNCSEL selects a PIO *block*, but pioGpioInitX() is keyed on a state machine handle. The
// managed InitGpio lives on PioBlock and can run before any SM is claimed, so the block variant
// is open coded here with the same glitch-free ordering pioGpioInitPadX() uses on the RP2350.
//

inline void NfPioBlockGpioInit(const rp_pio_block_t *block, uint32_t gpio)
{
    // same table pioSmSetPinFunctionX() uses, keyed on the block instead of a state machine
    static const uint32_t funcsels[] = {
        RP_PIO_FUNCSEL_PIO0,
        RP_PIO_FUNCSEL_PIO1,
#if RP_HAS_PIO2 == TRUE
        RP_PIO_FUNCSEL_PIO2,
#endif
    };

    const uint32_t funcsel = funcsels[block->pioidx];

#if defined(RP2350)
    // reprogram the pad while still isolated, drop the latch only once FUNCSEL points at the PIO
    PADS_BANK0->GPIO[gpio] = RP_PIO_PAD_DEFAULT | RP_PIO_PAD_ISO;
    IO_BANK0->GPIO[gpio].CTRL = funcsel;
    PADS_BANK0->GPIO[gpio] = RP_PIO_PAD_DEFAULT;
#else
    PADS_BANK0->GPIO[gpio] = RP_PIO_PAD_DEFAULT;
    IO_BANK0->GPIO[gpio].CTRL = funcsel;
#endif
}

// ---------------------------------------------------------------------------
// LLD gap: draining the TX FIFO
// ---------------------------------------------------------------------------
//
// The pico-sdk has pio_sm_drain_tx_fifo(); ChibiOS has nothing. Which instruction empties the
// FIFO depends on autopull, so this cannot be written correctly without reading SHIFTCTRL.
//
// Returns false if the FIFO did not drain within the spin budget, which means the state machine
// is stalled and not consuming.
//

inline bool NfPioSmDrainTxFifo(const rp_pio_sm_t *smp, unsigned int spinLimit)
{
    const bool autopull = (smp->block->pio->SM[smp->smidx].SHIFTCTRL & PIO_SM_SHIFTCTRL_AUTOPULL) != 0u;
    const uint16_t instr = autopull ? NF_PIO_INSTR_OUT_NULL_32 : NF_PIO_INSTR_PULL_NOBLOCK;

    while ((smp->block->pio->FSTAT & PIO_FSTAT_TXEMPTY(smp->smidx)) == 0u)
    {
        if (spinLimit-- == 0u)
        {
            return false;
        }

        pioSmExecX(smp, instr);
    }

    return true;
}

// ---------------------------------------------------------------------------
// LLD gap: reading back a DMA transfer counter
// ---------------------------------------------------------------------------
//
// dmaChannelSetCounterX() exists but there is no getter, and on the RP2350 TRANS_COUNT[31:28] is
// the count MODE field, so a raw read has to be masked to get the remaining word count.
//

inline uint32_t NfDmaChannelGetCounter(const rp_dma_channel_t *dmachp)
{
#if defined(RP2350)
    return dmachp->channel->TRANS_COUNT & ~DMA_TRANS_COUNT_MODE_Msk;
#else
    return dmachp->channel->TRANS_COUNT;
#endif
}

#endif // NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H
