/**
 ******************************************************************************
 * @file      startup_stm32f769.s
 * @brief     Minimal Cortex-M7 startup for the standalone MCUboot bootloader.
 *            Based on the CMSIS Device F7 GCC template but trimmed to only the
 *            vectors needed by the bootloader (no RTOS, no ChibiOS).
 *
 * Copyright (c) .NET Foundation and Contributors
 * Portions copyright (c) STMicroelectronics - see CMSIS Device F7 template.
 * See LICENSE file in the project root for full license information.
 ******************************************************************************
 */

    .syntax unified
    .cpu cortex-m7
    .fpu softvfp
    .thumb

.global g_pfnVectors
.global Default_Handler

/* Data section initialiser symbols provided by the linker script. */
.word _sidata   /* LMA of .data (flash load address)  */
.word _sdata    /* VMA start of .data (RAM)            */
.word _edata    /* VMA end   of .data (RAM)            */
.word _sbss     /* start of .bss (RAM)                 */
.word _ebss     /* end   of .bss (RAM)                 */

/**
 * Reset_Handler:
 *   1. Set the stack pointer from the linker-defined _estack symbol.
 *   2. Call SystemInit() (FPU enable, VTOR setup).
 *   3. Copy .data initializers from flash to RAM.
 *   4. Zero-fill .bss.
 *   5. Call C++ static constructors (__libc_init_array).
 *   6. Call main().
 *   7. Spin forever if main() returns (should not happen in a bootloader).
 */
    .section .text.Reset_Handler
    .weak Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    ldr   sp, =_estack

    /* Call SystemInit before .data copy so FPU is enabled early. */
    bl    SystemInit

    /* Copy .data from flash (LMA = _sidata) to RAM (VMA = _sdata.._edata). */
    ldr   r0, =_sdata
    ldr   r1, =_edata
    ldr   r2, =_sidata
    movs  r3, #0
    b     LoopCopyData

CopyData:
    ldr   r4, [r2, r3]
    str   r4, [r0, r3]
    adds  r3, r3, #4

LoopCopyData:
    adds  r4, r0, r3
    cmp   r4, r1
    bcc   CopyData

    /* Zero-fill .bss. */
    ldr   r2, =_sbss
    ldr   r4, =_ebss
    movs  r3, #0
    b     LoopZeroBss

ZeroBss:
    str   r3, [r2]
    adds  r2, r2, #4

LoopZeroBss:
    cmp   r2, r4
    bcc   ZeroBss

    /* Call C++ static constructors (required if any C++ globals exist). */
    bl    __libc_init_array

    /* Branch to main(). */
    bl    main

    /* main() should never return in a bootloader — spin if it does. */
Hang:
    b     Hang

    .size Reset_Handler, .-Reset_Handler

/**
 * Default_Handler — catches any unhandled exception or interrupt.
 * Enters an infinite loop; attach a debugger to identify the source.
 */
    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
    b  Infinite_Loop
    .size Default_Handler, .-Default_Handler

/*---------------------------------------------------------------------------
 * Cortex-M7 minimal vector table.
 * All peripheral IRQ handlers are aliased to Default_Handler.
 * The bootloader does not use any peripheral interrupts; all flash and SPI
 * operations are performed by polling.
 *---------------------------------------------------------------------------*/
    .section .isr_vector,"a",%progbits
    .type g_pfnVectors, %object

g_pfnVectors:
    /* Core exceptions */
    .word _estack                     /* 0x00: Initial Main Stack Pointer      */
    .word Reset_Handler               /* 0x04: Reset Handler                   */
    .word NMI_Handler                 /* 0x08: NMI Handler                     */
    .word HardFault_Handler           /* 0x0C: Hard Fault Handler              */
    .word MemManage_Handler           /* 0x10: MPU Fault Handler               */
    .word BusFault_Handler            /* 0x14: Bus Fault Handler               */
    .word UsageFault_Handler          /* 0x18: Usage Fault Handler             */
    .word 0                           /* 0x1C: Reserved                        */
    .word 0                           /* 0x20: Reserved                        */
    .word 0                           /* 0x24: Reserved                        */
    .word 0                           /* 0x28: Reserved                        */
    .word SVC_Handler                 /* 0x2C: SVC Handler                     */
    .word DebugMon_Handler            /* 0x30: Debug Monitor Handler           */
    .word 0                           /* 0x34: Reserved                        */
    .word PendSV_Handler              /* 0x38: PendSV Handler                  */
    .word SysTick_Handler             /* 0x3C: SysTick Handler                 */

    /* External peripheral interrupts — all default to Default_Handler.
     * STM32F769xx has 110 peripheral interrupt lines (IRQ 0..109).
     * The full table is required so the vector table size matches VTOR alignment. */
    .rept 110
    .word Default_Handler
    .endr

    .size g_pfnVectors, .-g_pfnVectors

/*---------------------------------------------------------------------------
 * Weak aliases — allow the application to override individual handlers.
 *---------------------------------------------------------------------------*/
    .weak NMI_Handler
    .thumb_set NMI_Handler, Default_Handler

    .weak HardFault_Handler
    .thumb_set HardFault_Handler, Default_Handler

    .weak MemManage_Handler
    .thumb_set MemManage_Handler, Default_Handler

    .weak BusFault_Handler
    .thumb_set BusFault_Handler, Default_Handler

    .weak UsageFault_Handler
    .thumb_set UsageFault_Handler, Default_Handler

    .weak SVC_Handler
    .thumb_set SVC_Handler, Default_Handler

    .weak DebugMon_Handler
    .thumb_set DebugMon_Handler, Default_Handler

    .weak PendSV_Handler
    .thumb_set PendSV_Handler, Default_Handler

    .weak SysTick_Handler
    .thumb_set SysTick_Handler, Default_Handler
