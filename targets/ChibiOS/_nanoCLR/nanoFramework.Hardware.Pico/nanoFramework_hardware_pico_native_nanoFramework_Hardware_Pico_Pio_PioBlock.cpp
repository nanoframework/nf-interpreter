//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_pico_native.h"
#include "nanoFramework_hardware_pico_native_target.h"
#if defined(RP2350)
#include "rp2350.h"
#else
#include "rp2040.h"
#endif

#define EVENT_TYPE_PICO_PIO 160

const rp_pio_sm_t *g_AllocatedSMs[3][4] = {{nullptr}};

static void PioChibiOSCallback(void *param, const uint32_t flags)
{
    const int block = reinterpret_cast<intptr_t>(param);
    PIO_TypeDef *pio = __rp_pio_blocks[block].pio;

    // IRQ0_INTS bits [11:8] are the SM-raised flags, IRQ holds the same flags live

    if (const uint32_t fifoFlags = flags & 0xFFu; fifoFlags != 0u)
    {
        NfPioBlockDisableInterrupt(&__rp_pio_blocks[block], fifoFlags);
        Events_Set(SYSTEM_EVENT_FLAG_PICOPIO);
    }

    const uint32_t smFlags = ((flags >> 8) & 0x0Fu) & pio->IRQ;

    if (smFlags == 0u)
    {
        return;
    }

    pio->IRQ = smFlags;

    PostManagedEvent(EVENT_PICO_PIO, EVENT_TYPE_PICO_PIO, static_cast<uint16_t>(block), smFlags);
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::
    AddProgram___U4__nanoFrameworkHardwarePicoPioPioProgram(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    rp_pio_program_t prog;
    int offset, origin, length;
    uint8_t block;
    CLR_RT_HeapBlock_Array *program_array;

    CLR_RT_HeapBlock *pProgram = stack.Arg1().Dereference();

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    program_array =
        pProgram
            [Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioProgram::FIELD__Instructions]
                .DereferenceArray();

    FAULT_ON_NULL(program_array);

    length = static_cast<int>(program_array->m_numOfElements);
    origin =
        pProgram[Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioProgram::FIELD__Origin]
            .NumericByRef()
            .s4;

    if (program_array == nullptr || length <= 0 || length > 32 ||
        static_cast<int>(program_array->m_numOfElements) < length)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    prog.instructions = reinterpret_cast<uint16_t *>(program_array->GetFirstElement());
    prog.length = length;
    prog.origin = origin;

    offset = pioProgramLoad(&__rp_pio_blocks[block], &prog);

    if (offset < 0)
    {
        // No memory left
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    stack.SetResult_I4(offset);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::
    RemoveProgram___VOID__nanoFrameworkHardwarePicoPioPioProgram__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t block;
    int length;
    CLR_RT_HeapBlock_Array *program;

    const CLR_RT_HeapBlock *pProgram = stack.Arg1().Dereference();
    const int offset = stack.Arg2().NumericByRef().s4;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    program =
        pProgram
            [Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioProgram::FIELD__Instructions]
                .DereferenceArray();

    FAULT_ON_NULL(program);

    length = static_cast<int>(program->m_numOfElements);

    VALIDATE_PIO_BLOCK(block);

    if (offset < 0 || length <= 0 || length > 32 || offset > 32 - length)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    pioProgramUnload(&__rp_pio_blocks[block], offset, length);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::NativeClaimSm___I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int block;
    const rp_pio_sm_t *smp;

    const int wanted = stack.Arg1().NumericByRef().s4;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    if (wanted != static_cast<int>(RP_PIO_SM_ID_ANY))
    {
        VALIDATE_SM(wanted);
    }

    smp = pioSmAlloc(
        &__rp_pio_blocks[block],
        static_cast<uint32_t>(wanted),
        NF_PICO_PIO_IRQ_PRIORITY,
        PioChibiOSCallback,
        reinterpret_cast<void *>(static_cast<intptr_t>(block)));

    if (smp == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    g_AllocatedSMs[block][smp->smidx] = smp;

    stack.SetResult_I4(static_cast<CLR_INT32>(smp->smidx));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::InitGpio___VOID__I4__nanoFrameworkHardwarePicoPioPioPinPull(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int block;
    uint32_t padbits = RP_PIO_PAD_DEFAULT;

    const int pin = stack.Arg1().NumericByRef().s4;
    const int pull = stack.Arg2().NumericByRef().s4;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    if (pin < 0 || pin > PIO_MAX_PIN)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    if (pull == 1)
    {
        padbits |= RP_PIO_PAD_PUE;
    }
    else if (pull == 2)
    {
        padbits |= RP_PIO_PAD_PDE;
    }
    else if (pull != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    NfPioBlockGpioInit(&__rp_pio_blocks[block], static_cast<uint32_t>(pin), padbits);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::ForceIrq___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int block;

    const int irq = stack.Arg1().NumericByRef().s4;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    if (irq < 0 || irq > 7)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    NfPioBlockForceIrq(&__rp_pio_blocks[block], static_cast<uint32_t>(irq));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::ClearIrq___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int block;

    const int irq = stack.Arg1().NumericByRef().s4;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    if (irq < 0 || irq > 7)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    NfPioBlockClearIrq(&__rp_pio_blocks[block], static_cast<uint32_t>(irq));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::
    NativeSetIrqEnabled___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int block;

    const uint32_t mask = PIO_IRQ_SM(0) | PIO_IRQ_SM(1) | PIO_IRQ_SM(2) | PIO_IRQ_SM(3);

    const bool enabled = static_cast<bool>(stack.Arg1().NumericByRef().u1);

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    if (enabled)
    {
        NfPioBlockEnableInterrupt(&__rp_pio_blocks[block], mask);
    }
    else
    {
        NfPioBlockDisableInterrupt(&__rp_pio_blocks[block], mask);
    }

    NANOCLR_NOCLEANUP();
}
