//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Headers.h>
#include <em_device.h>

// See http://infocenter.arm.com/help/topic/com.arm.doc.dui0552a/BABBGBEC.html
typedef enum
{
    Reset = 1,
    NMI = 2,
    HardFault = 3,
    MemManage = 4,
    BusFault = 5,
    UsageFault = 6,
} FaultType;

// Generic ARM register
typedef void *regarm_t;

// This structure represents the stack frame saved during an interrupt handler.
struct port_extctx
{
    regarm_t spsr_irq;
    regarm_t lr_irq;
    regarm_t r0;
    regarm_t r1;
    regarm_t r2;
    regarm_t r3;
    regarm_t r12;
    regarm_t lr_usr;
};

void NMI_Handler(void)
{
    while (1)
        ;
}

// dev note: on all the following the variables need to be declared as volatile so they don't get optimized out by the
// linker dev note: the pragma below is to ignore the warning because the variables aren't actually being used despite
// needing to remain there for debug

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

// hard fault handler for Cortex-M3 & M4

void HardFault_Handler(void)
{

    // Copy to local variables (not pointers) to allow GDB "i loc" to directly show the info
    struct port_extctx ctx;

    // Get thread context. Contains main registers including PC and LR
    memcpy(&ctx, (void *)__get_PSP(), sizeof(struct port_extctx));
    (void)ctx;

    // Interrupt status register: Which interrupt have we encountered, e.g. HardFault?
    volatile FaultType faultType = (FaultType)__get_IPSR();

    // these are not available in all the STM32 series

    // Flags about hardfault / busfault
    // See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihdjcfc.html for reference
    //  Configurable Fault Status Register
    volatile uint32_t _CFSR = SCB->CFSR;

    // IACCVIOL: Instruction access violation
    volatile bool isFaultInstructionAccessViolation = ((_CFSR & (1 << 0)) ? true : false);
    // DACCVIOL: Data access violation
    volatile bool isFaultDataAccessViolation = ((_CFSR & (1 << 1)) ? true : false);
    // MUNSTKERR: Unstacking error
    volatile bool isFaultUnstackingError = ((_CFSR & (1 << 3)) ? true : false);
    // MSTKERR: Stacking error
    volatile bool isFaultStackingError = ((_CFSR & (1 << 4)) ? true : false);
    // MMARVALID: MMAR is valid
    volatile bool isMarkedMemoryAddressValid = ((_CFSR & (1 << 7)) ? true : false);
    // IBUSERR: Instruction bus error
    volatile bool isFaultInstructionBusError = ((_CFSR & (1 << 8)) ? true : false);
    // PRECISERR: Precise data bus error
    volatile bool isFaultPreciseDataBusError = ((_CFSR & (1 << 9)) ? true : false);
    // IMPRECISERR: Imprecise data bus error
    volatile bool isFaultImpreciseDataBusError = ((_CFSR & (1 << 10)) ? true : false);
    // LSPERR: Lazy state preservation error
    volatile bool isFaultLazyStatePreservationError = ((_CFSR & (1 << 13)) ? true : false);
    // BFARVALID: BFAR is valid
    volatile bool isFaultBusFaultAddressValid = ((_CFSR & (1 << 15)) ? true : false);
    // UNDEFINSTR: Undefined instruction usage fault
    volatile bool isUndefinedInstructionUsageFault = ((_CFSR & (1 << 16)) ? true : false);
    // INVSTATE: Invalid state usage fault
    volatile bool isInvalidStateUsageFault = ((_CFSR & (1 << 17)) ? true : false);
    // INVPC: Invalid PC load usage fault
    volatile bool isInvalidPcLoadUsageFault = ((_CFSR & (1 << 18)) ? true : false);
    // NOCP: No coprocessor usage fault
    volatile bool isNoCoprocessorUsageFault = ((_CFSR & (1 << 19)) ? true : false);
    // UNALIGNED: Unaligned access usage fault
    volatile bool isUnalignedAccessUsageFault = ((_CFSR & (1 << 24)) ? true : false);
    // DIVBYZERO: Divide by zero usage fault
    volatile bool isDivideByZeroUsageFault = ((_CFSR & (1 << 25)) ? true : false);

    // Hard Fault Status Register
    volatile unsigned long _HFSR = (*((volatile unsigned long *)(0xE000ED2C)));

    // Debug Fault Status Register
    volatile unsigned long _DFSR = (*((volatile unsigned long *)(0xE000ED30)));

    // Auxiliary Fault Status Register
    volatile unsigned long _AFSR = (*((volatile unsigned long *)(0xE000ED3C)));

    // Read the Fault Address Registers. These may not contain valid values.
    // Check BFARVALID/MMARVALID to see if they are valid values

    // MemManage Fault Address Register
    volatile unsigned long _MMAR = (*((volatile unsigned long *)(0xE000ED34)));

    // For HardFault/BusFault this is the address that was accessed causing the error
    volatile uint32_t faultAddress = SCB->BFAR;

    // forces a breakpoint causing the debugger to stop
    // if no debugger is attached this is ignored
    __asm volatile("BKPT #0\n");

    // If no debugger connected, just reset the board
    NVIC_SystemReset();
}

void BusFault_Handler(void) __attribute__((alias("HardFault_Handler")));

void UsageFault_Handler(void)
{

    // Copy to local variables (not pointers) to allow GDB "i loc" to directly show the info
    // Get thread context. Contains main registers including PC and LR
    struct port_extctx ctx;
    memcpy(&ctx, (void *)__get_PSP(), sizeof(struct port_extctx));
    (void)ctx;

    // Interrupt status register: Which interrupt have we encountered, e.g. HardFault?
    FaultType faultType = (FaultType)__get_IPSR();
    (void)faultType;

    // Flags about hardfault / busfault
    // See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihdjcfc.html for reference
    //  Configurable Fault Status Register
    volatile uint32_t _CFSR = SCB->CFSR;

    // IACCVIOL: Instruction access violation
    volatile bool isFaultInstructionAccessViolation = ((_CFSR & (1 << 0)) ? true : false);
    // DACCVIOL: Data access violation
    volatile bool isFaultDataAccessViolation = ((_CFSR & (1 << 1)) ? true : false);
    // MUNSTKERR: Unstacking error
    volatile bool isFaultUnstackingError = ((_CFSR & (1 << 3)) ? true : false);
    // MSTKERR: Stacking error
    volatile bool isFaultStackingError = ((_CFSR & (1 << 4)) ? true : false);
    // MMARVALID: MMAR is valid
    volatile bool isMarkedMemoryAddressValid = ((_CFSR & (1 << 7)) ? true : false);
    // IBUSERR: Instruction bus error
    volatile bool isFaultInstructionBusError = ((_CFSR & (1 << 8)) ? true : false);
    // PRECISERR: Precise data bus error
    volatile bool isFaultPreciseDataBusError = ((_CFSR & (1 << 9)) ? true : false);
    // IMPRECISERR: Imprecise data bus error
    volatile bool isFaultImpreciseDataBusError = ((_CFSR & (1 << 10)) ? true : false);
    // LSPERR: Lazy state preservation error
    volatile bool isFaultLazyStatePreservationError = ((_CFSR & (1 << 13)) ? true : false);
    // BFARVALID: BFAR is valid
    volatile bool isFaultBusFaultAddressValid = ((_CFSR & (1 << 15)) ? true : false);
    // UNDEFINSTR: Undefined instruction usage fault
    volatile bool isUndefinedInstructionUsageFault = ((_CFSR & (1 << 16)) ? true : false);
    // INVSTATE: Invalid state usage fault
    volatile bool isInvalidStateUsageFault = ((_CFSR & (1 << 17)) ? true : false);
    // INVPC: Invalid PC load usage fault
    volatile bool isInvalidPcLoadUsageFault = ((_CFSR & (1 << 18)) ? true : false);
    // NOCP: No coprocessor usage fault
    volatile bool isNoCoprocessorUsageFault = ((_CFSR & (1 << 19)) ? true : false);
    // UNALIGNED: Unaligned access usage fault
    volatile bool isUnalignedAccessUsageFault = ((_CFSR & (1 << 24)) ? true : false);
    // DIVBYZERO: Divide by zero usage fault
    volatile bool isDivideByZeroUsageFault = ((_CFSR & (1 << 25)) ? true : false);

    // forces a breakpoint causing the debugger to stop
    // if no debugger is attached this is ignored
    __asm volatile("BKPT #0\n");

    // If no debugger connected, just reset the board
    NVIC_SystemReset();
}

void MemManage_Handler(void)
{

    // Copy to local variables (not pointers) to allow GDB "i loc" to directly show the info
    // Get thread context. Contains main registers including PC and LR
    struct port_extctx ctx;
    memcpy(&ctx, (void *)__get_PSP(), sizeof(struct port_extctx));
    (void)ctx;

    // Interrupt status register: Which interrupt have we encountered, e.g. HardFault?
    FaultType faultType = (FaultType)__get_IPSR();
    (void)faultType;

    // For HardFault/BusFault this is the address that was accessed causing the error
    volatile uint32_t faultAddress = SCB->MMFAR;

    // Flags about hardfault / busfault
    // See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihdjcfc.html for reference
    //  Configurable Fault Status Register
    volatile uint32_t _CFSR = SCB->CFSR;

    // IACCVIOL: Instruction access violation
    volatile bool isFaultInstructionAccessViolation = ((_CFSR & (1 << 0)) ? true : false);
    // DACCVIOL: Data access violation
    volatile bool isFaultDataAccessViolation = ((_CFSR & (1 << 1)) ? true : false);
    // MUNSTKERR: Unstacking error
    volatile bool isFaultUnstackingError = ((_CFSR & (1 << 3)) ? true : false);
    // MSTKERR: Stacking error
    volatile bool isFaultStackingError = ((_CFSR & (1 << 4)) ? true : false);
    // MMARVALID: MMAR is valid
    volatile bool isMarkedMemoryAddressValid = ((_CFSR & (1 << 7)) ? true : false);
    // IBUSERR: Instruction bus error
    volatile bool isFaultInstructionBusError = ((_CFSR & (1 << 8)) ? true : false);
    // PRECISERR: Precise data bus error
    volatile bool isFaultPreciseDataBusError = ((_CFSR & (1 << 9)) ? true : false);
    // IMPRECISERR: Imprecise data bus error
    volatile bool isFaultImpreciseDataBusError = ((_CFSR & (1 << 10)) ? true : false);
    // LSPERR: Lazy state preservation error
    volatile bool isFaultLazyStatePreservationError = ((_CFSR & (1 << 13)) ? true : false);
    // BFARVALID: BFAR is valid
    volatile bool isFaultBusFaultAddressValid = ((_CFSR & (1 << 15)) ? true : false);
    // UNDEFINSTR: Undefined instruction usage fault
    volatile bool isUndefinedInstructionUsageFault = ((_CFSR & (1 << 16)) ? true : false);
    // INVSTATE: Invalid state usage fault
    volatile bool isInvalidStateUsageFault = ((_CFSR & (1 << 17)) ? true : false);
    // INVPC: Invalid PC load usage fault
    volatile bool isInvalidPcLoadUsageFault = ((_CFSR & (1 << 18)) ? true : false);
    // NOCP: No coprocessor usage fault
    volatile bool isNoCoprocessorUsageFault = ((_CFSR & (1 << 19)) ? true : false);
    // UNALIGNED: Unaligned access usage fault
    volatile bool isUnalignedAccessUsageFault = ((_CFSR & (1 << 24)) ? true : false);
    // DIVBYZERO: Divide by zero usage fault
    volatile bool isDivideByZeroUsageFault = ((_CFSR & (1 << 25)) ? true : false);

    // forces a breakpoint causing the debugger to stop
    // if no debugger is attached this is ignored
    __asm volatile("BKPT #0\n");

    // If no debugger connected, just reset the board
    NVIC_SystemReset();
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

///////////////////////////////////////////////////////////////////////////////////////////
// Hard Fault test code.
// Call this to cause a hard fault by accessing a nonexistent memory address @ 0xCCCCCCCC.
void HardFaultTest()
{
    volatile uint32_t *p;
    uint32_t n;
    p = (uint32_t *)0xCCCCCCCC;
    n = *p;
    (void)n;
}
///////////////////////////////////////////////////////////////////////////////////////////
