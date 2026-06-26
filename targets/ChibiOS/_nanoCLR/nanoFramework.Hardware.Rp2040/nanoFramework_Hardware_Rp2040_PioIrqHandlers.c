//
// Copyright (c) .NET nanoFramework PIO contributors
//
// PIO IRQ0 vector handlers. Compiled as C so OSAL_IRQ_HANDLER's un-mangled symbol resolves; they just
// forward to PioIrqServiceBlock() in PioIrqDriver.cpp.
//

#include <hal.h>

// RP_PIO_REQUIRED -> ChibiOS rp_pio.c owns these vectors (Pico W / CYW43); skip ours to avoid a clash.
#if !defined(RP2350) && !defined(RP_PIO_REQUIRED)

extern void PioIrqServiceBlock(int block);

OSAL_IRQ_HANDLER(RP_PIO0_IRQ_0_HANDLER)
{
    OSAL_IRQ_PROLOGUE();
    PioIrqServiceBlock(0);
    OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(RP_PIO1_IRQ_0_HANDLER)
{
    OSAL_IRQ_PROLOGUE();
    PioIrqServiceBlock(1);
    OSAL_IRQ_EPILOGUE();
}

#endif // !RP2350
