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
 * RP2040 nanoBooter memory setup.
 * nanoBooter occupies the first 64KB of XIP flash.
 *
 * Flash layout:
 *   0x10000000 - 0x1000FFFF : nanoBooter   (64KB)
 *   0x10010000 - 0x10017FFF : Config block (32KB)
 *   0x10018000 - 0x100FFFFF : nanoCLR      (928KB)
 *   0x10100000 - 0x101FFFFF : Deployment   (1MB)
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
    config     (rw) : org = 0x10010000, len = 32k     /* configuration block */
    deployment (rx) : org = 0x00000000, len = 0       /* not used in booter */
    ramvt      (wx) : org = 0x00000000, len = 0       /* no RAM vector table in booter */
    ram0       (wx) : org = 0x20000000, len = 256k-48 /* SRAM0 striped */
    ram1       (wx) : org = 0x00000000, len = 0
    ram2       (wx) : org = 0x00000000, len = 0
    ram3       (wx) : org = 0x00000000, len = 0
    ram4       (wx) : org = 0x20040000, len = 4k      /* SRAM4 */
    ram5       (wx) : org = 0x20041000, len = 4k      /* SRAM5 */
    ram6       (wx) : org = 0x00000000, len = 0
    ram7       (wx) : org = 0x00000000, len = 0
    bootclpbrd  : org = 0x2003FFD0, len = 48          /* boot clipboard at end of SRAM */
}

/* For each data/text section two region are defined, a virtual region
   and a load region (_LMA suffix).*/

/* Flash region to be used for exception vectors.*/
REGION_ALIAS("VECTORS_FLASH", flash1);
REGION_ALIAS("VECTORS_FLASH_LMA", flash1);

/* Flash region to be used for constructors and destructors.*/
REGION_ALIAS("XTORS_FLASH", flash1);
REGION_ALIAS("XTORS_FLASH_LMA", flash1);

/* Flash region to be used for code text.*/
REGION_ALIAS("TEXT_FLASH", flash1);
REGION_ALIAS("TEXT_FLASH_LMA", flash1);

/* Flash region to be used for read only data.*/
REGION_ALIAS("RODATA_FLASH", flash1);
REGION_ALIAS("RODATA_FLASH_LMA", flash1);

/* Flash region to be used for various.*/
REGION_ALIAS("VARIOUS_FLASH", flash1);
REGION_ALIAS("VARIOUS_FLASH_LMA", flash1);

/* Flash region to be used for RAM(n) initialization data.*/
REGION_ALIAS("RAM_INIT_FLASH_LMA", flash1);

/* RAM region to be used for Main stack. This stack accommodates the processing
   of all exceptions and interrupts.*/
REGION_ALIAS("MAIN_STACK_RAM", ram4);

/* RAM region to be used for the process stack. This is the stack used by
   the main() function.*/
REGION_ALIAS("PROCESS_STACK_RAM", ram4);

/* Core 1 stack regions for SMP.*/
REGION_ALIAS("C1_MAIN_STACK_RAM", ram5);
REGION_ALIAS("C1_PROCESS_STACK_RAM", ram5);

/* RAM region to be used for data segment.*/
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash1);

/* RAM region to be used for BSS segment.*/
REGION_ALIAS("BSS_RAM", ram0);

/* RAM region to be used for the default heap.*/
REGION_ALIAS("HEAP_RAM", ram0);

/* RAM region to be used for the boot clipboard.*/
REGION_ALIAS("SECTION_FOR_BOOTCLIPBOARD", bootclpbrd);

/* RP2040 second stage bootloader must be placed at the very start of flash */
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

/* Stacks rules inclusion.*/
INCLUDE rules_stacks.ld

/* Core 1 stacks rules inclusion (SMP).*/
INCLUDE rules_stacks_c1.ld

/* Code rules inclusion.*/
INCLUDE rules_code.ld

/* Data rules inclusion.*/
INCLUDE rules_data.ld

/* Memory rules inclusion.*/
INCLUDE rules_memory.ld

/* Boot clipboard rules inclusion.*/
INCLUDE rules_bootclipboard.ld

SECTIONS
{
    .flash_end : {
        __flash_binary_end = .;
    } > flash1
}
