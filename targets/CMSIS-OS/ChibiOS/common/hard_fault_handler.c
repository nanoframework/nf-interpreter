//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <hal.h>
#include <ch.h>

//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0552a/BABBGBEC.html
typedef enum {
    Reset = 1,
    NMI = 2,
    HardFault = 3,
    MemManage = 4,
    BusFault = 5,
    UsageFault = 6,
} FaultType;

#if defined(STM32F4XX) || defined(STM32F7XX)

void NMI_Handler(void) {
    while(1);
}

#endif

// dev note: on all the following the variables need to be declared as volatile so they don't get optimized out by the linker
// dev note: the pragma below is to ignore the warning because the variables aren't actually being used despite needing to remain there for debug

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

void HardFault_Handler(void) {                                                                                   

    //Copy to local variables (not pointers) to allow GDB "i loc" to directly show the info
    struct port_extctx ctx;

    //Get thread context. Contains main registers including PC and LR
    memcpy(&ctx, (void*)__get_PSP(), sizeof(struct port_extctx));
    (void)ctx;

    //Interrupt status register: Which interrupt have we encountered, e.g. HardFault?
    volatile FaultType faultType = (FaultType)__get_IPSR();

    // these are not available in all the STM32 series
#if defined(STM32F4XX) || defined(STM32F7XX)

    //Flags about hardfault / busfault
    //See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihdjcfc.html for reference
    volatile bool isFaultPrecise = ((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & (1 << 1) ? true : false);
    volatile bool isFaultImprecise = ((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & (1 << 2) ? true : false);
    volatile bool isFaultOnUnstacking = ((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & (1 << 3) ? true : false);
    volatile bool isFaultOnStacking = ((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & (1 << 4) ? true : false);
    volatile bool isFaultAddressValid = ((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & (1 << 7) ? true : false);

    // Hard Fault Status Register
    volatile unsigned long _HFSR = (*((volatile unsigned long *)(0xE000ED2C))) ;

    // Debug Fault Status Register
    volatile unsigned long _DFSR = (*((volatile unsigned long *)(0xE000ED30))) ;

    // Auxiliary Fault Status Register
    volatile unsigned long _AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;

    // Read the Fault Address Registers. These may not contain valid values.
    // Check BFARVALID/MMARVALID to see if they are valid values
    
    // MemManage Fault Address Register
    volatile unsigned long _MMAR = (*((volatile unsigned long *)(0xE000ED34))) ;
    
    //For HardFault/BusFault this is the address that was accessed causing the error
    volatile uint32_t faultAddress = SCB->BFAR;

#endif
    
    // forces a breakpoint causing the debugger to stop
    // if no debugger is attached this is ignored
    __asm volatile("BKPT #0\n");

    // If no debugger connected, just reset the board
    NVIC_SystemReset();
}

void BusFault_Handler(void) __attribute__((alias("HardFault_Handler")));

void UsageFault_Handler(void) {

    //Copy to local variables (not pointers) to allow GDB "i loc" to directly show the info
    //Get thread context. Contains main registers including PC and LR
    struct port_extctx ctx;
    memcpy(&ctx, (void*)__get_PSP(), sizeof(struct port_extctx));
    (void)ctx;

    //Interrupt status register: Which interrupt have we encountered, e.g. HardFault?
    FaultType faultType = (FaultType)__get_IPSR();
    (void)faultType;

    // these are not available in all the STM32 series
#if defined(STM32F4XX) || defined(STM32F7XX)
    
    //Flags about hardfault / busfault
    //See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihdjcfc.html for reference
    volatile bool isUndefinedInstructionFault = ((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & (1 << 0) ? true : false);
    volatile bool isEPSRUsageFault = ((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & (1 << 1) ? true : false);
    volatile bool isInvalidPCFault = ((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & (1 << 2) ? true : false);
    volatile bool isNoCoprocessorFault = ((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & (1 << 3) ? true : false);
    volatile bool isUnalignedAccessFault = ((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & (1 << 8) ? true : false);
    volatile bool isDivideByZeroFault = ((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & (1 << 9) ? true : false);

#endif

    // forces a breakpoint causing the debugger to stop
    // if no debugger is attached this is ignored
    __asm volatile("BKPT #0\n");

    // If no debugger connected, just reset the board
    NVIC_SystemReset();
}

void MemManage_Handler(void) {

    //Copy to local variables (not pointers) to allow GDB "i loc" to directly show the info
    //Get thread context. Contains main registers including PC and LR
    struct port_extctx ctx;
    memcpy(&ctx, (void*)__get_PSP(), sizeof(struct port_extctx));
    (void)ctx;

    //Interrupt status register: Which interrupt have we encountered, e.g. HardFault?
    FaultType faultType = (FaultType)__get_IPSR();
    (void)faultType;

    // these are not available in all the STM32 series
#if defined(STM32F4XX) || defined(STM32F7XX)
    
    //For HardFault/BusFault this is the address that was accessed causing the error
    volatile uint32_t faultAddress = SCB->MMFAR;

    //Flags about hardfault / busfault
    //See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihdjcfc.html for reference
    volatile bool isInstructionAccessViolation = ((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & (1 << 0) ? true : false);
    volatile bool isDataAccessViolation = ((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & (1 << 1) ? true : false);
    volatile bool isExceptionUnstackingFault = ((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & (1 << 3) ? true : false);
    volatile bool isExceptionStackingFault = ((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & (1 << 4) ? true : false);
    volatile bool isFaultAddressValid = ((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & (1 << 7) ? true : false);

#endif

    // forces a breakpoint causing the debugger to stop
    // if no debugger is attached this is ignored
    __asm volatile("BKPT #0\n");

    // If no debugger connected, just reset the board
    NVIC_SystemReset();
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
