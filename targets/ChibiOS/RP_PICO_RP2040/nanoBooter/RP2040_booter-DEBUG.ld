/*
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006..2026 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//
*/

/* The RP2040 requires 256-byte vector table alignment to run from flash. */
__vectors_align__ = 256;

/*
 * RP2040 nanoBooter DEBUG memory setup.
 * Same layout as release — debug builds just have larger code.
 */
MEMORY
{
    flash0     (rx) : org = 0x10000000, len = 96k     /* nanoBooter + config (for __nanoImage_end__) */
    flash1     (rx) : org = 0x10000000, len = 64k     /* XIP: nanoBooter */
    flash2     (rx) : org = 0x00000000, len = 0
    flash3     (rx) : org = 0x00000000, len = 0
    flash4     (rx) : org = 0x00000000, len = 0
    flash5     (rx) : org = 0x00000000, len = 0
    flash6     (rx) : org = 0x00000000, len = 0
    flash7     (rx) : org = 0x00000000, len = 0
    config     (rw) : org = 0x10010000, len = 32k
    deployment (rx) : org = 0x00000000, len = 0
    ramvt      (wx) : org = 0x00000000, len = 0
    ram0       (wx) : org = 0x20000000, len = 256k-48
    ram1       (wx) : org = 0x00000000, len = 0
    ram2       (wx) : org = 0x00000000, len = 0
    ram3       (wx) : org = 0x00000000, len = 0
    ram4       (wx) : org = 0x20040000, len = 4k
    ram5       (wx) : org = 0x20041000, len = 4k
    ram6       (wx) : org = 0x00000000, len = 0
    ram7       (wx) : org = 0x00000000, len = 0
    bootclpbrd  : org = 0x2003FFD0, len = 48
}

REGION_ALIAS("VECTORS_FLASH", flash1);
REGION_ALIAS("VECTORS_FLASH_LMA", flash1);
REGION_ALIAS("XTORS_FLASH", flash1);
REGION_ALIAS("XTORS_FLASH_LMA", flash1);
REGION_ALIAS("TEXT_FLASH", flash1);
REGION_ALIAS("TEXT_FLASH_LMA", flash1);
REGION_ALIAS("RODATA_FLASH", flash1);
REGION_ALIAS("RODATA_FLASH_LMA", flash1);
REGION_ALIAS("VARIOUS_FLASH", flash1);
REGION_ALIAS("VARIOUS_FLASH_LMA", flash1);
REGION_ALIAS("RAM_INIT_FLASH_LMA", flash1);
REGION_ALIAS("MAIN_STACK_RAM", ram4);
REGION_ALIAS("PROCESS_STACK_RAM", ram4);
REGION_ALIAS("C1_MAIN_STACK_RAM", ram5);
REGION_ALIAS("C1_PROCESS_STACK_RAM", ram5);
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash1);
REGION_ALIAS("BSS_RAM", ram0);
REGION_ALIAS("HEAP_RAM", ram0);
REGION_ALIAS("SECTION_FOR_BOOTCLIPBOARD", bootclpbrd);

SECTIONS
{
    .flash_begin : {
        __flash_binary_start = .;
    } > flash1

    .boot2 : {
        __boot2_start__ = .;
        KEEP (*(.boot2))
        __boot2_end__ = .;
    } > flash1

    ASSERT(__boot2_end__ - __boot2_start__ <= 256,
        "ERROR: The Pico second stage bootloader must be 256 bytes in size")
}

INCLUDE rules_stacks.ld
INCLUDE rules_stacks_c1.ld
INCLUDE rules_code.ld
INCLUDE rules_data.ld
INCLUDE rules_memory.ld
INCLUDE rules_bootclipboard.ld

SECTIONS
{
    .flash_end : {
        __flash_binary_end = .;
    } > flash1
}
