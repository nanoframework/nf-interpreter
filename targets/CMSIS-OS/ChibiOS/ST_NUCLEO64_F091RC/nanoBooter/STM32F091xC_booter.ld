/*
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
*/


/*
 * STM32F091xC memory setup.
 */
MEMORY
{
    flash0     (rx) : org = 0x08000000, len = 10k     /* space reserved for nanoBooter */
    flash1     (rx) : org = 0x00000000, len = 0
    flash2     (rx) : org = 0x00000000, len = 0
    flash3     (rx) : org = 0x00000000, len = 0
    flash4     (rx) : org = 0x00000000, len = 0
    flash5     (rx) : org = 0x00000000, len = 0
    flash6     (rx) : org = 0x00000000, len = 0
    flash7     (rx) : org = 0x00000000, len = 0
    config     (rw) : org = 0x00000000, len = 0       /* space reserved for configuration block */
    deployment (rx) : org = 0x00000000, len = 0       /* space reserved for application deployment */
    ramvt      (wx) : org = 0x00000000, len = 0       /* initial RAM address is reserved for a copy of the vector table */
    ram0       (wx) : org = 0x20000000, len = 32k-48
    ram1       (wx) : org = 0x00000000, len = 0
    ram2       (wx) : org = 0x00000000, len = 0
    ram3       (wx) : org = 0x00000000, len = 0
    ram4       (wx) : org = 0x00000000, len = 0
    ram5       (wx) : org = 0x00000000, len = 0
    ram6       (wx) : org = 0x00000000, len = 0
    ram7       (wx) : org = 0x00000000, len = 0
    bootclpbrd  : org = 0x20007FD0, len = 48      /* boot clipboard area */
}

/* For each data/text section two region are defined, a virtual region
   and a load region (_LMA suffix).*/

/* Flash region to be used for exception vectors.*/
REGION_ALIAS("VECTORS_FLASH", flash0);
REGION_ALIAS("VECTORS_FLASH_LMA", flash0);

/* Flash region to be used for constructors and destructors.*/
REGION_ALIAS("XTORS_FLASH", flash0);
REGION_ALIAS("XTORS_FLASH_LMA", flash0);

/* Flash region to be used for code text.*/
REGION_ALIAS("TEXT_FLASH", flash0);
REGION_ALIAS("TEXT_FLASH_LMA", flash0);

/* Flash region to be used for read only data.*/
REGION_ALIAS("RODATA_FLASH", flash0);
REGION_ALIAS("RODATA_FLASH_LMA", flash0);

/* Flash region to be used for various.*/
REGION_ALIAS("VARIOUS_FLASH", flash0);
REGION_ALIAS("VARIOUS_FLASH_LMA", flash0);

/* Flash region to be used for RAM(n) initialization data.*/
REGION_ALIAS("RAM_INIT_FLASH_LMA", flash0);

/* RAM region to be used for Main stack. This stack accommodates the processing
   of all exceptions and interrupts.*/
REGION_ALIAS("MAIN_STACK_RAM", ram0);

/* RAM region to be used for the process stack. This is the stack used by
   the main() function.*/
REGION_ALIAS("PROCESS_STACK_RAM", ram0);

/* RAM region to be used for data segment.*/
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash0);

/* RAM region to be used for BSS segment.*/
REGION_ALIAS("BSS_RAM", ram0);

/* RAM region to be used for the default heap.*/
REGION_ALIAS("HEAP_RAM", ram0);

/* RAM region to be used for the boot clipboard.*/
REGION_ALIAS("SECTION_FOR_BOOTCLIPBOARD", bootclpbrd);

/* Stacks rules inclusion.*/
INCLUDE rules_stacks.ld

/* Code rules inclusion.*/
INCLUDE rules_code.ld

/* Data rules inclusion.*/
INCLUDE rules_data.ld

/* Data rules inclusion.*/
INCLUDE rules_memory.ld

/* boot clipboard rules inclusion.*/
INCLUDE rules_bootclipboard.ld
