/*
// Copyright (c) .NET Foundation and Contributors
// Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
// See LICENSE file in the project root for full license information.
*/

/* Set the RAM segment used end for threadx */
__RAM_segment_used_end__ = 0; 

MEMORY {
    ROM         (rx) : ORIGIN = 0x00000000, LENGTH = 0x00010000         /* 64kB ROM */
    flash0      (rx) : ORIGIN = 0x10000000, LENGTH = 0x00080000         /* 512KB Flash */
    SRAM       (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00020000-0x30    /* 128kB SRAM */
    bootclpbrd (rwx) : ORIGIN = 0x2001FFD0, LENGTH = 0x00000030         /* boot clipboard area */
    config      (rw) : ORIGIN = 0x00000000, LENGTH = 0                  /* space reserved for configuration block */
}

/* RAM region to be used for the boot clipboard.*/
REGION_ALIAS("SECTION_FOR_BOOTCLIPBOARD", bootclpbrd);

SECTIONS {
    .rom :
    {
        KEEP(*(.rom_vector))
        *(.rom_handlers*)
    } > ROM

    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)    /* program code */
        *(.rodata*)  /* read-only data: "const" */

        KEEP(*(.init))
        KEEP(*(.fini))

        /* C++ Exception handling */
        KEEP(*(.eh_frame*))
        _etext = .;
    } > flash0

    /* Binary import */
    .bin_storage :
    {
       FILL(0xFF)
      _bin_start_ = .;
      KEEP(*(.bin_storage_img))
      _bin_end_ = .;
      . = ALIGN(4);
    } > flash0
    
    .rom_code :
    {
        . = ALIGN(16);
        _sran_code = .;
        *(.rom_code_section)
        _esran_code = .;
    } > ROM

    .flash_code :
    {
        . = ALIGN(16);
        _sran_code = .;
        *(.flash_code_section)
        _esran_code = .;
    } > flash0

    .sram_code :
    {
        . = ALIGN(16);
        _sran_code = .;
        *(.sram_code_section)
        _esran_code = .;
    } > SRAM

    /* it's used for C++ exception handling      */
    /* we need to keep this to avoid overlapping */
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > flash0

    .data :
    {
        _data = ALIGN(., 4);
        *(.data*)           /*read-write initialized data: initialized global variable*/

        /* These array sections are used by __libc_init_array to call static C++ constructors */
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        _edata = ALIGN(., 4);
    } > SRAM AT>flash0
    __load_data = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _bss = .;
        *(.bss*)     /*read-write zero initialized data: uninitialzed global variable*/
        *(COMMON)
        _ebss = ALIGN(., 4);
    } > SRAM

    .shared :
    {
        . = ALIGN(4);
        _shared = .;
        *(.mailbox*)
        . = ALIGN(4);
        *(.shared*)     /*read-write zero initialized data: uninitialzed global variable*/
        _eshared = ALIGN(., 4);
    } > SRAM
    __shared_data = LOADADDR(.shared);

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(SRAM) + LENGTH(SRAM);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > SRAM

    .heap (COPY):
    {
        . = ALIGN(4);
        *(.heap*)
        __HeapLimit = ABSOLUTE(__StackLimit);
    } > SRAM

    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= _ebss, "region RAM overflowed with stack")
}

/* Code rules inclusion.*/
INCLUDE rules_code.ld

/* boot clipboard rules inclusion.*/
INCLUDE rules_bootclipboard.ld
