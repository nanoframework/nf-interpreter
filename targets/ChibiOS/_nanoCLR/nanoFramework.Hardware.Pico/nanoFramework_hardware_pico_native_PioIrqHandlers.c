//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>

// RP_PIO_REQUIRED -> ChibiOS rp_pio.c owns these vectors (Pico W / CYW43); skip ours to avoid a clash.
#if !defined(RP_PIO_REQUIRED)

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

#if defined(RP2350)
OSAL_IRQ_HANDLER(RP_PIO2_IRQ_0_HANDLER)
{
    OSAL_IRQ_PROLOGUE();
    PioIrqServiceBlock(2);
    OSAL_IRQ_EPILOGUE();
}
#endif

#endif // !RP_PIO_REQUIRED
