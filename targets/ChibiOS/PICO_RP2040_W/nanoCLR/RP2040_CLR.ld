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
 * RP2040 nanoCLR memory setup.
 *
 * Flash layout (WiFi variant — needs more code space for CYW43 firmware blob):
 *   0x10000000 - 0x1000BFFF : nanoBooter   (48KB)   - not used by CLR
 *   0x1000C000 - 0x10013FFF : Config block (32KB)   - read-only from CLR
 *   0x10014000 - 0x10153FFF : nanoCLR      (1280KB)
 *   0x10154000 - 0x101F7FFF : Deployment   (656KB)
 *   0x101F8000 - 0x101FFFFF : littlefs     (32KB)
 *
 * When updating the flash1 address below make sure to update the address
 * in nf_generate_bin_package if applicable.
 */
MEMORY
{
    flash0     (rx) : org = 0x10014000, len = 1280k     /* XIP: nanoCLR (also defines __nanoImage_start__) */
    flash1     (rx) : org = 0x10014000, len = 1280k     /* XIP: nanoCLR */
    flash2     (rx) : org = 0x00000000, len = 0
    flash3     (rx) : org = 0x00000000, len = 0
    flash4     (rx) : org = 0x00000000, len = 0
    flash5     (rx) : org = 0x00000000, len = 0
    flash6     (rx) : org = 0x00000000, len = 0
    flash7     (rx) : org = 0x00000000, len = 0
    config     (rw) : org = 0x1000C000, len = 32k       /* configuration block */
    deployment (rx) : org = 0x10154000, len = 656k      /* deployment area */
    ramvt      (wx) : org = 0x00000000, len = 0         /* no RAM vector table needed on M0+ */
    ram0       (wx) : org = 0x20000000, len = 256k-48   /* SRAM0 striped */
    ram1       (wx) : org = 0x00000000, len = 0
    ram2       (wx) : org = 0x00000000, len = 0
    ram3       (wx) : org = 0x00000000, len = 0
    ram4       (wx) : org = 0x20040000, len = 4k        /* SRAM4 */
    ram5       (wx) : org = 0x20041000, len = 4k        /* SRAM5 */
    ram6       (wx) : org = 0x00000000, len = 0
    ram7       (wx) : org = 0x00000000, len = 0
    bootclpbrd  : org = 0x2003FFD0, len = 48            /* boot clipboard at end of SRAM */
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

/* RAM region to be used for the nanoFramework CLR managed heap.*/
REGION_ALIAS("CLR_MANAGED_HEAP_RAM", ram0);

/* RAM region to be used for the boot clipboard.*/
REGION_ALIAS("SECTION_FOR_BOOTCLIPBOARD", bootclpbrd);

/* RP2040 second stage bootloader must be placed at the very start of flash.
   For nanoCLR, the boot2 section is empty since nanoBooter owns the flash start.
   However, the vector table still needs to be at the start of CLR flash region. */
SECTIONS
{
    .flash_begin : {
        __flash_binary_start = .;
    } > flash1
}

INCLUDE rules_stacks.ld
INCLUDE rules_stacks_c1.ld
INCLUDE rules_code.ld
INCLUDE rules_data.ld
INCLUDE rules_memory.ld
INCLUDE rules_clr.ld
INCLUDE rules_bootclipboard.ld

SECTIONS
{
    .flash_end : {
        __flash_binary_end = .;
    } > flash1
}
