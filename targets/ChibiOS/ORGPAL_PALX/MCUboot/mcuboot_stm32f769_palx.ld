/*
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//
*/

/*
 * STM32F76xx MCUboot bootloader linker script for ORGPAL_PALX (STM32F769NI).
 *
 * Based on the nanoBooter STM32F76xx_booter.ld for this board, with the
 * following changes:
 *   - bootclpbrd region removed (no boot clipboard in the MCUboot bootloader)
 *   - INCLUDE rules_bootclipboard.ld removed
 *   - ram3 expanded back to full 128 kB (org = 0x20000000)
 *
 * RAM0 - Data, Heap.
 * RAM3 - Main Stack, Process Stack, BSS, NOCACHE, ETH.
 */
MEMORY
{
    flash0     (rx) : org = 0x08000000, len = 64k     /* MCUboot bootloader (sectors 0-1) */
    flash1     (rx) : org = 0x00200000, len = 64k     /* ITCM alias of flash0 */
    flash2     (rx) : org = 0x00000000, len = 0
    flash3     (rx) : org = 0x00000000, len = 0
    flash4     (rx) : org = 0x00000000, len = 0
    flash5     (rx) : org = 0x00000000, len = 0
    flash6     (rx) : org = 0x00000000, len = 0
    flash7     (rx) : org = 0x00000000, len = 0
    config     (rw) : org = 0x08010000, len = 32k     /* configuration block */
    deployment (rx) : org = 0x00000000, len = 0
    ramvt      (wx) : org = 0x00000000, len = 0
    ram0       (wx) : org = 0x20020000, len = 384k    /* SRAM1 + SRAM2 */
    ram1       (wx) : org = 0x20020000, len = 368k    /* SRAM1 */
    ram2       (wx) : org = 0x2007C000, len = 16k     /* SRAM2 */
    ram3       (wx) : org = 0x20000000, len = 128k    /* DTCM-RAM (full — no boot clipboard) */
    ram4       (wx) : org = 0x00000000, len = 16k     /* ITCM-RAM */
    ram5       (wx) : org = 0x40024000, len = 4k      /* BCKP SRAM */
    ram6       (wx) : org = 0x00000000, len = 0
    ram7       (wx) : org = 0x00000000, len = 0
}

/* Flash region to be used for exception vectors.*/
REGION_ALIAS("VECTORS_FLASH", flash1);
REGION_ALIAS("VECTORS_FLASH_LMA", flash0);

/* Flash region to be used for constructors and destructors.*/
REGION_ALIAS("XTORS_FLASH", flash1);
REGION_ALIAS("XTORS_FLASH_LMA", flash0);

/* Flash region to be used for code text.*/
REGION_ALIAS("TEXT_FLASH", flash1);
REGION_ALIAS("TEXT_FLASH_LMA", flash0);

/* Flash region to be used for read only data.*/
REGION_ALIAS("RODATA_FLASH", flash0);
REGION_ALIAS("RODATA_FLASH_LMA", flash0);

/* Flash region to be used for various.*/
REGION_ALIAS("VARIOUS_FLASH", flash1);
REGION_ALIAS("VARIOUS_FLASH_LMA", flash0);

/* Flash region to be used for RAM(n) initialization data.*/
REGION_ALIAS("RAM_INIT_FLASH_LMA", flash0);

/* RAM region for the main stack.*/
REGION_ALIAS("MAIN_STACK_RAM", ram3);

/* RAM region for the process stack.*/
REGION_ALIAS("PROCESS_STACK_RAM", ram3);

/* RAM region for .data segment.*/
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash0);

/* RAM region for .bss segment.*/
REGION_ALIAS("BSS_RAM", ram3);

/* RAM region for the default heap.*/
REGION_ALIAS("HEAP_RAM", ram0);

/* rules inclusion.*/
INCLUDE rules_stacks.ld

/*===========================================================================*/
/* Custom sections for STM32F7xx.                                            */
/*===========================================================================*/

/* RAM region for nocache segment.*/
REGION_ALIAS("NOCACHE_RAM", ram3);

/* RAM region for eth segment.*/
REGION_ALIAS("ETH_RAM", ram3);

SECTIONS
{
    /* Special section for non cache-able areas.*/
    .nocache (NOLOAD) : ALIGN(4)
    {
        __nocache_base__ = .;
        *(.nocache)
        *(.nocache.*)
        *(.bss.__nocache_*)
        . = ALIGN(4);
        __nocache_end__ = .;
    } > NOCACHE_RAM

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

/* Data rules inclusion.*/
INCLUDE rules_memory.ld
