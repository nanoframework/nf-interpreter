//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_pico_native.h"
#include <nanoHAL_v2.h>
#include <ch.h>  // chSysLockFromISR
#include <hal.h> // nvicEnableVector + RP_PIOx_IRQ_0_NUMBER
#if defined(RP2350)
#include "rp2350.h"
#else
#include "rp2040.h"
#endif
#include "nanoFramework_hardware_pico_native_target.h"

// dispatcher context per block
static CLR_RT_HeapBlock_NativeEventDispatcher *s_pioCtx[3] = {nullptr, nullptr, nullptr};

static int BlockOfContext(CLR_RT_HeapBlock_NativeEventDispatcher *pContext)
{
    for (int b = 0; b < 3; b++)
    {
        if (s_pioCtx[b] == pContext)
        {
            return b;
        }
    }

    return -1;
}

// Used only by the IRQ0->NVIC wiring, compiled out on RP_PIO_REQUIRED (Pico W).
#if !defined(RP_PIO_REQUIRED)
static int PioIrqVector(int block)
{
    switch (block)
    {
        case 0:
            return RP_PIO0_IRQ_0_NUMBER;
        case 1:
            return RP_PIO1_IRQ_0_NUMBER;
#if defined(RP2350)
        case 2:
            return RP_PIO2_IRQ_0_NUMBER;
#endif
        default:
            return -1;
    }
}
#endif

// Called from the block's IRQ0 ISR (PioIrqHandlers.c), hence extern "C".
extern "C" void PioIrqServiceBlock(int block)
{
    PIO_TypeDef *pio = PioFromIndex(block);
    if (pio == nullptr)
    {
        return;
    }

    // SaveNativeEventToHALQueue touches the shared HAL queue, so take the ISR critical section like GPIO.
    NATIVE_INTERRUPT_START

    chSysLockFromISR();

    // SM irq flags -> IRQ0_INTS bits [11:8]
    unsigned int flags = (pio->IRQ0_INTS >> 8) & 0x0Fu;
    if (flags != 0)
    {
        CLR_RT_HeapBlock_NativeEventDispatcher *ctx = s_pioCtx[block];
        if (ctx != nullptr)
        {
            CLR_UINT32 packed = ((CLR_UINT32)flags << 16) | ((CLR_UINT32)EVENT_CUSTOM << 8) | (CLR_UINT32)block;
            SaveNativeEventToHALQueue(ctx, packed, (CLR_UINT32)flags);
        }

        // write-1-to-clear
        pio->IRQ = flags;
    }

    chSysUnlockFromISR();

    NATIVE_INTERRUPT_END
}

// ---- NativeEventDispatcher driver procs -------------------------------------

static HRESULT PioIrqInitialize(CLR_RT_HeapBlock_NativeEventDispatcher *pContext, unsigned __int64 userData)
{
    int block = (int)(userData & 0xFF);
    if (block < 0 || block > 2 || PioFromIndex(block) == nullptr)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // one dispatcher per block
    if (s_pioCtx[block] != nullptr && s_pioCtx[block] != pContext)
    {
        return CLR_E_INVALID_OPERATION;
    }

    s_pioCtx[block] = pContext;
    return S_OK;
}

static HRESULT PioIrqEnableDisable(CLR_RT_HeapBlock_NativeEventDispatcher *pContext, bool fEnable)
{
    int block = BlockOfContext(pContext);
    PIO_TypeDef *pio = (block >= 0) ? PioFromIndex(block) : nullptr;
    if (pio == nullptr)
    {
        return CLR_E_INVALID_PARAMETER;
    }

#if !defined(RP_PIO_REQUIRED)
    int vector = PioIrqVector(block);
    if (fEnable)
    {
        pio->IRQ0_INTE |= (0x0Fu << 8);
        nvicEnableVector(vector, 3); // kernel-safe priority
    }
    else
    {
        pio->IRQ0_INTE &= ~(0x0Fu << 8);
        nvicDisableVector(vector);
    }
#else
    // PIO IRQ0 is owned by the CYW43 driver on Pico W, so it can't reach managed code
    (void)fEnable;
    return CLR_E_NOT_SUPPORTED;
#endif

    return S_OK;
}

static HRESULT PioIrqCleanup(CLR_RT_HeapBlock_NativeEventDispatcher *pContext)
{
    int block = BlockOfContext(pContext);
    if (block >= 0)
    {
#if !defined(RP_PIO_REQUIRED)
        PIO_TypeDef *pio = PioFromIndex(block);
        if (pio != nullptr)
        {
            pio->IRQ0_INTE &= ~(0x0Fu << 8);
            nvicDisableVector(PioIrqVector(block));
        }
#endif
        s_pioCtx[block] = nullptr;
    }

    CleanupNativeEventsFromHALQueue(pContext);
    return S_OK;
}

static const CLR_RT_DriverInterruptMethods g_PioIrqDriverMethods = {
    PioIrqInitialize,
    PioIrqEnableDisable,
    PioIrqCleanup};

// looked up by name from `new NativeEventDispatcher("PioIrqDriver", block)`
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Pico_PioIrqDriver =
    {"PioIrqDriver", DRIVER_INTERRUPT_METHODS_CHECKSUM, &g_PioIrqDriverMethods, {1, 0, 0, 0}};
