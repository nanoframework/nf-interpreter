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
    const int block = reinterpret_cast<int>(param);

    // Align the flags to the PIO block - See IRQ0_INTS documentation
    const uint32_t sm_flags = (flags >> 8) & 0x0F;

    PostManagedEvent(EVENT_PICO_PIO, EVENT_TYPE_PICO_PIO, static_cast<uint16_t>(block), sm_flags);

    // Clear the flag in hardware to avoid an infinite loop of interrupts.
    __rp_pio_blocks[block].pio->IRQ = sm_flags;
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

    // Get the data from the PioProgram class passed as argument
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
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    prog.instructions = reinterpret_cast<uint16_t *>(program_array->GetFirstElement());
    prog.length = length;
    prog.origin = origin;

    offset = pioProgramLoad(&__rp_pio_blocks[block], &prog);

    if (offset < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION); // No memory left
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
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    pioProgramUnload(&__rp_pio_blocks[block], offset, length);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::
    NativeClaimUnusedSm___STATIC__I4__I4__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const int block = stack.Arg0().NumericByRef().s4;
    const bool required = stack.Arg1().NumericByRef().u1;

    VALIDATE_PIO_BLOCK(block);

    if (const rp_pio_sm_t *sm = pioSmAlloc(
            &__rp_pio_blocks[block],
            RP_PIO_SM_ID_ANY,
            3,
            PioChibiOSCallback,
            reinterpret_cast<void *>(block));
        sm == nullptr)
    {
        if (required)
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        stack.SetResult_I4(-1);
    }
    else
    {
        g_AllocatedSMs[block][sm->smidx] = sm;

        stack.SetResult_I4(static_cast<CLR_INT32>(sm->smidx));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::InitGpio___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint32_t mode;
    int block;

    const int pin = stack.Arg1().NumericByRef().s4;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    if (pin < 0 || pin > PIO_MAX_PIN)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    mode = (block == 0) ? PAL_MODE_ALTERNATE_PIO0 : PAL_MODE_ALTERNATE_PIO1;
#if defined(RP2350)
    if (block == 2)
        mode = PAL_MODE_ALTERNATE_PIO2;
#endif

    palSetPadMode(0, pin, mode);

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
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    __rp_pio_blocks[block].pio->IRQ_FORCE = (1u << irq);

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
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    __rp_pio_blocks[block].pio->IRQ = (1u << irq);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::
    NativeSetIrqEnabled___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int block;

    const bool enabled = static_cast<bool>(stack.Arg1().NumericByRef().u1);

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    if (enabled)
    {
        __rp_pio_blocks[block].pio->IRQ0_INTE |= (0x0Fu << 8);
    }
    else
    {
        __rp_pio_blocks[block].pio->IRQ0_INTE &= ~(0x0Fu << 8);
    }

    NANOCLR_NOCLEANUP();
}
