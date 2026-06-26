//
// Copyright (c) .NET nanoFramework PIO contributors
//
//

#include <hal.h>

#if !defined(RP2350)

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
