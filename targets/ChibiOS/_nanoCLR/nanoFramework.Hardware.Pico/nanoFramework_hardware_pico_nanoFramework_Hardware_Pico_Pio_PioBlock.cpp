//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_pico.h"
#include "nanoFramework_hardware_pico_target.h"

#define EVENT_TYPE_PICO_PIO 160

typedef Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioProgram PioProgram;

static constexpr uint32_t PioFifoIrqMask = PIO_IRQ_RXNEMPTY(0) | PIO_IRQ_RXNEMPTY(1) | PIO_IRQ_RXNEMPTY(2) |
                                           PIO_IRQ_RXNEMPTY(3) | PIO_IRQ_TXNFULL(0) | PIO_IRQ_TXNFULL(1) |
                                           PIO_IRQ_TXNFULL(2) | PIO_IRQ_TXNFULL(3);
static constexpr uint32_t PioSmIrqMask = PIO_IRQ_SM(0) | PIO_IRQ_SM(1) | PIO_IRQ_SM(2) | PIO_IRQ_SM(3);
static constexpr uint32_t PioSmIrqPos = 8u;

static void PioChibiOSCallback(void *param, const uint32_t flags)
{
    const int block = reinterpret_cast<intptr_t>(param);
    const rp_pio_block_t *pioBlock = RP_PIO_BLOCK(block);

    if (const uint32_t fifoFlags = flags & PioFifoIrqMask; fifoFlags != 0u)
    {
        pioDisableInterruptX(pioBlock, fifoFlags);
        Events_Set(SYSTEM_EVENT_FLAG_PICOPIO);
    }

    const uint32_t raisedSmFlags = (flags & PioSmIrqMask) >> PioSmIrqPos;
    const uint32_t latched = raisedSmFlags & pioIrqGetX(pioBlock);
    const uint32_t forced = raisedSmFlags & pioBlock->pio->IRQ_FORCE;
    const uint32_t smFlags = latched | forced;

    if (smFlags == 0u)
    {
        return;
    }

    if (latched != 0u)
    {
        pioIrqClearX(pioBlock, latched);
    }

    if (forced != 0u)
    {
        pioBlock->pio->CLR.IRQ_FORCE = forced;
    }

    PostManagedEvent(EVENT_PICO_PIO, EVENT_TYPE_PICO_PIO, static_cast<uint16_t>(block), smFlags);
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioBlock::
    AddProgram___U4__nanoFrameworkHardwarePicoPioPioProgram(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    rp_pio_program_t pioProgram;
    CLR_RT_HeapBlock *program;
    CLR_RT_HeapBlock_Array *instructions;
    int block, length, offset;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    program = stack.Arg1().Dereference();
    FAULT_ON_NULL(program);

    instructions = program[PioProgram::FIELD__Instructions].DereferenceArray();
    FAULT_ON_NULL(instructions);

    length = static_cast<int>(instructions->m_numOfElements);

    if (length <= 0 || length > static_cast<int>(RP_PIO_NUM_INSTR_MEM))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    pioProgram.instructions = reinterpret_cast<uint16_t *>(instructions->GetFirstElement());
    pioProgram.length = length;
    pioProgram.origin = program[PioProgram::FIELD__Origin].NumericByRef().s4;

    offset = pioProgramLoad(RP_PIO_BLOCK(block), &pioProgram);

    if (offset < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    stack.SetResult_I4(offset);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioBlock::
    RemoveProgram___VOID__nanoFrameworkHardwarePicoPioPioProgram__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *program;
    CLR_RT_HeapBlock_Array *instructions;
    int block, length, offset;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    program = stack.Arg1().Dereference();
    FAULT_ON_NULL(program);

    instructions = program[PioProgram::FIELD__Instructions].DereferenceArray();
    FAULT_ON_NULL(instructions);

    length = static_cast<int>(instructions->m_numOfElements);
    offset = stack.Arg2().NumericByRef().s4;

    if (offset < 0 || length <= 0 || length > static_cast<int>(RP_PIO_NUM_INSTR_MEM) ||
        offset > static_cast<int>(RP_PIO_NUM_INSTR_MEM) - length)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    pioProgramUnload(RP_PIO_BLOCK(block), offset, length);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioBlock::NativeClaimSm___I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const rp_pio_sm_t *stateMachine;
    int block, wanted;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    wanted = stack.Arg1().NumericByRef().s4;

    if (wanted != static_cast<int>(RP_PIO_SM_ID_ANY))
    {
        VALIDATE_SM(wanted);
    }

    stateMachine = pioSmAlloc(
        RP_PIO_BLOCK(block),
        static_cast<uint32_t>(wanted),
        NF_PICO_PIO_IRQ_PRIORITY,
        PioChibiOSCallback,
        reinterpret_cast<void *>(block));

    if (stateMachine == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    stack.SetResult_I4(static_cast<CLR_INT32>(stateMachine->smidx));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioBlock::InitGpio___VOID__I4__nanoFrameworkHardwarePicoPioPioPinPull(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint32_t padBits = RP_PIO_PAD_DEFAULT;
    int block, pin;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    pin = stack.Arg1().NumericByRef().s4;

    if (pin < 0 || pin > PIO_MAX_PIN)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    switch (stack.Arg2().NumericByRef().s4)
    {
        case PioPinPull_None:
            break;

        case PioPinPull_Up:
            padBits |= RP_PIO_PAD_PUE;
            break;

        case PioPinPull_Down:
            padBits |= RP_PIO_PAD_PDE;
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    pioGpioRoutePadX(RP_PIO_BLOCK(block), static_cast<uint32_t>(pin), padBits);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioBlock::ForceIrq___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int block, irq;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    irq = stack.Arg1().NumericByRef().s4;

    if (irq < 0 || irq >= NF_PICO_PIO_IRQ_FLAG_COUNT)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    pioIrqForceX(RP_PIO_BLOCK(block), 1u << irq);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioBlock::ClearIrq___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int block, irq;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    irq = stack.Arg1().NumericByRef().s4;

    if (irq < 0 || irq >= NF_PICO_PIO_IRQ_FLAG_COUNT)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    pioIrqClearX(RP_PIO_BLOCK(block), 1u << irq);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_pico_nanoFramework_Hardware_Pico_Pio_PioBlock::
    NativeSetIrqEnabled___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int block;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    block = pThis[FIELD___index].NumericByRef().s4;

    VALIDATE_PIO_BLOCK(block);

    if (stack.Arg1().NumericByRef().u1 != 0)
    {
        pioEnableInterruptX(RP_PIO_BLOCK(block), PioSmIrqMask);
    }
    else
    {
        pioDisableInterruptX(RP_PIO_BLOCK(block), PioSmIrqMask);
    }

    NANOCLR_NOCLEANUP();
}
