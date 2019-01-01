/*
//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
*/

/*
 * ST32F439xI memory setup.
 * Note: Use of ram1, ram2 and ram3 is mutually exclusive with use of ram0.
 */
MEMORY
{
    flash       : org = 0x08000000, len = 32k     /* space reserved for nanoBooter (1st two sectors 0x08000000 to 0x08008000)*/
    config      : org = 0x08008000, len = 32k     /* space reserved for configuration block */
    deployment  : org = 0x00000000, len = 0       /* space reserved for application deployment */
    ramvt       : org = 0x00000000, len = 0       /* initial RAM address is reserved for a copy of the vector table */
    ram0        : org = 0x20000000, len = 192k    /* SRAM1 + SRAM2 + SRAM3 */
    ram1        : org = 0x20000000, len = 112k    /* SRAM1 */
    ram2        : org = 0x2001C000, len = 16k     /* SRAM2 */
    ram3        : org = 0x20020000, len = 64k     /* SRAM3 */
    ram4        : org = 0x10000000, len = 64k     /* CCM SRAM */
    ram5        : org = 0x40024000, len = 4k      /* BCKP SRAM */
    ram6        : org = 0x00000000, len = 0
    ram7        : org = 0x00000000, len = 0
}

/* For each data/text section two region are defined, a virtual region
   and a load region (_LMA suffix).*/

/* Flash region to be used for exception vectors.*/
REGION_ALIAS("VECTORS_FLASH", flash);
REGION_ALIAS("VECTORS_FLASH_LMA", flash);

/* Flash region to be used for constructors and destructors.*/
REGION_ALIAS("XTORS_FLASH", flash);
REGION_ALIAS("XTORS_FLASH_LMA", flash);

/* Flash region to be used for code text.*/
REGION_ALIAS("TEXT_FLASH", flash);
REGION_ALIAS("TEXT_FLASH_LMA", flash);

/* Flash region to be used for read only data.*/
REGION_ALIAS("RODATA_FLASH", flash);
REGION_ALIAS("RODATA_FLASH_LMA", flash);

/* Flash region to be used for various.*/
REGION_ALIAS("VARIOUS_FLASH", flash);
REGION_ALIAS("VARIOUS_FLASH_LMA", flash);

/* Flash region to be used for RAM(n) initialization data.*/
REGION_ALIAS("RAM_INIT_FLASH_LMA", flash);

/* RAM region to be used for Main stack. This stack accommodates the processing
   of all exceptions and interrupts.*/
REGION_ALIAS("MAIN_STACK_RAM", ram0);

/* RAM region to be used for the process stack. This is the stack used by
   the main() function.*/
REGION_ALIAS("PROCESS_STACK_RAM", ram0);

/* RAM region to be used for data segment.*/
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash);

/* RAM region to be used for BSS segment.*/
REGION_ALIAS("BSS_RAM", ram0);

/* RAM region to be used for the default heap.*/
REGION_ALIAS("HEAP_RAM", ram4);

/* Stacks rules inclusion.*/
INCLUDE rules_stacks.ld

/* RAM region to be used for eth segment.*/
REGION_ALIAS("ETH_RAM", ram0);

SECTIONS
{
    /* Special section for Ethernet DMA non cache-able areas.*/
    .eth (NOLOAD) : ALIGN(4)
    {
        __eth_base__ = .;
        *(.eth)
        *(.eth.*)
        *(.bss.__eth_*)
        . = ALIGN(4);
        __eth_end__ = .;
    } > ETH_RAM
}

/* Code rules inclusion.*/
INCLUDE rules_code.ld

/* Data rules inclusion.*/
INCLUDE rules_data.ld
