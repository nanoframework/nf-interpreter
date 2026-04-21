/*
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//
*/

/*
 * STM32F76xx MCUboot bootloader linker script — ChibiOS-compatible.
 *
 * Memory layout:
 *   flash0 — 32 kB at 0x08000000 (internal flash sector 0): bootloader image.
 *   ram0   — 384 kB SRAM1+2 at 0x20020000: .data segment, heap.
 *   ram3   — 128 kB DTCM at 0x20000000: main stack, process stack, BSS.
 *
 * All code, rodata and vectors are placed directly in flash0 (no ITCM alias).
 * ChibiOS startup (crt0_v7m.s) reads __main_stack_end__ and
 * __process_stack_end__ from rules_stacks.ld.
 */

MEMORY
{
    flash0     (rx) : org = 0x08000000, len = 32k
    flash1     (rx) : org = 0x00000000, len = 0
    flash2     (rx) : org = 0x00000000, len = 0
    flash3     (rx) : org = 0x00000000, len = 0
    flash4     (rx) : org = 0x00000000, len = 0
    flash5     (rx) : org = 0x00000000, len = 0
    flash6     (rx) : org = 0x00000000, len = 0
    flash7     (rx) : org = 0x00000000, len = 0
    ramvt      (wx) : org = 0x00000000, len = 0
    ram0       (wx) : org = 0x20020000, len = 384k    /* SRAM1 + SRAM2 */
    ram1       (wx) : org = 0x20020000, len = 368k    /* SRAM1 */
    ram2       (wx) : org = 0x2007C000, len = 16k     /* SRAM2 */
    ram3       (wx) : org = 0x20000000, len = 128k    /* DTCM-RAM (full, no boot clipboard) */
    ram4       (wx) : org = 0x00000000, len = 0
    ram5       (wx) : org = 0x00000000, len = 0
    ram6       (wx) : org = 0x00000000, len = 0
    ram7       (wx) : org = 0x00000000, len = 0
}

/*
 * All sections live in flash0 (LMA = VMA = 0x08000000 region).
 * No ITCM alias is used — the Cortex-M7 instruction cache accelerates
 * direct flash execution sufficiently for a bootloader workload.
 */

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

/* RAM region for the main stack (exception/IRQ handler stack).*/
REGION_ALIAS("MAIN_STACK_RAM", ram3);

/* RAM region for the process stack (main() thread stack).*/
REGION_ALIAS("PROCESS_STACK_RAM", ram3);

/* RAM region for .data segment.*/
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash0);

/* RAM region for .bss segment.*/
REGION_ALIAS("BSS_RAM", ram3);

/* RAM region for the heap.*/
REGION_ALIAS("HEAP_RAM", ram0);

/* Stack sizes.
 * Main stack: holds IRQ/exception frames. Keep modest — bootloader has
 *   few interrupts (only SysTick and optionally SPI/WSPI DMA).
 * Process stack: main() thread — needs room for MCUboot image validation
 *   (ECDSA stack frames) and the SPI/WSPI driver blocking wait path. */
__main_stack_size__    = 0x800;     /* 2 kB */
__process_stack_size__ = 0x1000;    /* 4 kB */

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

/* Memory rules inclusion.*/
INCLUDE rules_memory.ld
