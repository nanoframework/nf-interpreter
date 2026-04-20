/*
 * Copyright (c) .NET Foundation and Contributors
 * See LICENSE file in the project root for full license information.
 *
 * Linker script for the standalone MCUboot bootloader on ST_STM32F769I_DISCOVERY
 * (STM32F769NI).
 *
 * Memory map:
 *   flash (32 kB) — MCUboot bootloader code + read-only data
 *   ram   (128 kB) — DTCM-RAM: stack, .data, .bss, heap
 *                    DTCM is directly connected to the Cortex-M7 core and
 *                    does not require D-cache management, which simplifies
 *                    bare-metal operation.
 *
 * The bootloader occupies flash sector 0 (32 kB).
 * It must not overlap the config sector 1 at 0x08008000 or the nanoCLR
 * primary slot at 0x08010000.
 *
 * Stack and heap sizing:
 *   _Min_Stack_Size = 4 kB  — adequate for MCUboot + ECDSA verification.
 *   _Min_Heap_Size  = 8 kB  — required by MCUboot's os_heap.h (stdlib malloc).
 */

MEMORY
{
    flash (rx)  : ORIGIN = 0x08000000, LENGTH = 32K
    ram   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

_Min_Stack_Size = 0x1000;   /* 4 kB minimum stack   */
_Min_Heap_Size  = 0x2000;   /* 8 kB minimum heap    */

ENTRY(Reset_Handler)

SECTIONS
{
    /* Interrupt vector table — must be at the start of flash. */
    .isr_vector :
    {
        . = ALIGN(512);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > flash

    /* Code and read-only data. */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;
    } > flash

    /* Read-only data. */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > flash

    /* ARM exception unwind tables (required for C++ and newlib). */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > flash

    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > flash

    /* C++ constructor/destructor lists. */
    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > flash

    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > flash

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > flash

    /* Initialised data: LMA in flash, VMA in RAM. */
    /* _sidata is the load address (LMA) used by Reset_Handler. */
    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > ram AT > flash

    /* Zero-initialised data. */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > ram

    /* Heap — grows upward from _end toward the stack. */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE(end = .);
        PROVIDE(_end = .);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } > ram

    /* _estack — initial Main Stack Pointer (top of RAM region). */
    _estack = ORIGIN(ram) + LENGTH(ram);

    /* Discard C++ exception unwind info (not needed in bare-metal bootloader). */
    /DISCARD/ :
    {
        libc.a   (*)
        libm.a   (*)
        libgcc.a (*)
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
