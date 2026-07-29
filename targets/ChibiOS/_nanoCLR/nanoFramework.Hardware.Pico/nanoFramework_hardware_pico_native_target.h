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

#define NF_PICO_PIO_IRQ_PRIORITY    3
#define NF_PICO_PIO_FIFO_TIMEOUT_MS 1000u
#define NF_PICO_PIO_DRAIN_LIMIT     0x10000u

#define VALIDATE_PIO_BLOCK(block)                                                                                      \
    if ((block) < 0 || (block) > PIO_MAX_BLOCK)                                                                        \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);                                                                     \
    }                                                                                                                  \
    if ((block) < PIO_MIN_BLOCK)                                                                                       \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);                                                                    \
    }

#define VALIDATE_SM(sm)                                                                                                \
    if ((sm) < 0 || (sm) > 3)                                                                                          \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);                                                                     \
    }

#define VALIDATE_NOT_DISPOSED(pThis)                                                                                   \
    FAULT_ON_NULL(pThis);                                                                                              \
    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)                                                                \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);                                                                  \
    }

extern const rp_pio_sm_t *g_AllocatedSMs[3][4];
void PioDmaReleaseSm(int block, int sm);

struct PioSmContext
{
    const rp_pio_sm_t *smp;
    int block;
    int sm;
};

inline HRESULT PioGetSmContext(CLR_RT_HeapBlock *pThis, PioSmContext &ctx, const bool allowDisposed = false)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pPioBlock;

    FAULT_ON_NULL(pThis);

    if (!allowDisposed &&
        pThis[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::
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

#define NANOCLR_PIO_SM_PROLOGUE(ctx)                                                                                   \
    {                                                                                                                  \
        NANOCLR_CHECK_HRESULT(PioGetSmContext(stack.This(), ctx));                                                     \
    }

#define NANOCLR_PIO_SM_TEARDOWN_PROLOGUE(ctx)                                                                          \
    {                                                                                                                  \
        NANOCLR_CHECK_HRESULT(PioGetSmContext(stack.This(), ctx, true));                                               \
    }

#define NF_PIO_INSTR_OUT_NULL_32 0x6060u
#define NF_PIO_INSTR_PULL_NOBLOCK 0x8080u

#if defined(RP2350)
#define NF_PIO_SM_SHIFTCTRL_FJOIN_RX_GET (1u << 14)
#define NF_PIO_SM_SHIFTCTRL_FJOIN_RX_PUT (1u << 15)
#endif

inline void NfPioSmConfigSetFifoJoin(rp_pio_sm_config_t *cfgp, const unsigned int join)
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

inline void NfPioBlockEnableInterrupt(const rp_pio_block_t *block, const uint32_t mask)
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

inline void NfPioBlockDisableInterrupt(const rp_pio_block_t *block, const uint32_t mask)
{
    block->pio->CLR.IRQ0_INTE = mask;
    block->pio->CLR.IRQ1_INTE = mask;
}

inline void NfPioBlockForceIrq(const rp_pio_block_t *block, const uint32_t irq)
{
    block->pio->IRQ_FORCE = 1u << irq;
}

inline void NfPioBlockClearIrq(const rp_pio_block_t *block, const uint32_t irq)
{
    block->pio->IRQ = 1u << irq;
}

inline void NfPioBlockGpioInit(const rp_pio_block_t *block, uint32_t gpio, uint32_t padbits)
{
    static constexpr uint32_t funcsels[] = {
        RP_PIO_FUNCSEL_PIO0,
        RP_PIO_FUNCSEL_PIO1,
#if RP_HAS_PIO2 == TRUE
        RP_PIO_FUNCSEL_PIO2,
#endif
    };

    const uint32_t funcsel = funcsels[block->pioidx];

#if defined(RP2350)
    PADS_BANK0->GPIO[gpio] = padbits | RP_PIO_PAD_ISO;
    IO_BANK0->GPIO[gpio].CTRL = funcsel;
    PADS_BANK0->GPIO[gpio] = padbits;
#else
    PADS_BANK0->GPIO[gpio] = padbits;
    IO_BANK0->GPIO[gpio].CTRL = funcsel;
#endif
}

inline bool NfPioSmDrainTxFifo(const rp_pio_sm_t *smp, unsigned int spinLimit)
{
    const uint32_t shiftctrl = smp->block->pio->SM[smp->smidx].SHIFTCTRL;
    bool drained = true;

    if ((shiftctrl & PIO_SM_SHIFTCTRL_AUTOPULL) != 0u)
    {
        smp->block->pio->SM[smp->smidx].SHIFTCTRL = shiftctrl & ~PIO_SM_SHIFTCTRL_AUTOPULL;
    }

    while ((smp->block->pio->FSTAT & PIO_FSTAT_TXEMPTY(smp->smidx)) == 0u)
    {
        if (spinLimit-- == 0u)
        {
            drained = false;
            break;
        }

        pioSmExecX(smp, NF_PIO_INSTR_PULL_NOBLOCK);
    }

    smp->block->pio->SM[smp->smidx].SHIFTCTRL = shiftctrl;

    return drained;
}

inline uint32_t NfDmaChannelGetCounter(const rp_dma_channel_t *dmachp)
{
#if defined(RP2350)
    return dmachp->channel->TRANS_COUNT & ~DMA_TRANS_COUNT_MODE_Msk;
#else
    return dmachp->channel->TRANS_COUNT;
#endif
}

#endif // NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H
