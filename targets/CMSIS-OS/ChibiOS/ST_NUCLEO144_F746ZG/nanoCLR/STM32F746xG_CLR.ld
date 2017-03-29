/*
//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//
*/

/*
 * STM32F746xG generic setup.
 * 
 * RAM0 - Data, Heap.
 * RAM3 - Main Stack, Process Stack, BSS, NOCACHE, ETH.
 *
 * Notes:
 * BSS is placed in DTCM RAM in order to simplify DMA buffers management.
 */
MEMORY
{
    flash       : org = 0x08008000, len = 1M - 32k - 768k   /* flash size less the space reserved for nanoBooter and application deployment*/
    flash_itcm  : org = 0x00200000, len = 1M
    deployment  : org = 0x08040000, len = 768k              /* space reserved for application deployment */
    ramvt       : org = 0x00000000, len = 0                 /* initial RAM address is reserved for a copy of the vector table */
    ram0        : org = 0x20010000, len = 256k              /* SRAM1 + SRAM2 */
    ram1        : org = 0x20010000, len = 240k              /* SRAM1 */
    ram2        : org = 0x2004C000, len = 16k               /* SRAM2 */
    ram3        : org = 0x20000000, len = 64k               /* DTCM-RAM */
    ram4        : org = 0x00000000, len = 16k               /* ITCM-RAM */
    ram5        : org = 0x40024000, len = 4k                /* BCKP SRAM */
    ram6        : org = 0x00000000, len = 0
    ram7        : org = 0x00000000, len = 0
}

/* RAM region to be used for Main stack. This stack accommodates the processing
   of all exceptions and interrupts*/
REGION_ALIAS("MAIN_STACK_RAM", ram3);

/* RAM region to be used for the process stack. This is the stack used by
   the main() function.*/
REGION_ALIAS("PROCESS_STACK_RAM", ram3);

/* RAM region to be used for data segment.*/
REGION_ALIAS("DATA_RAM", ram0);

/* RAM region to be used for BSS segment.*/
REGION_ALIAS("BSS_RAM", ram3);

/* RAM region to be used for the default heap.*/
REGION_ALIAS("HEAP_RAM", ram0);

/* RAM region to be used for nocache segment.*/
REGION_ALIAS("NOCACHE_RAM", ram3);

/* RAM region to be used for eth segment.*/
REGION_ALIAS("ETH_RAM", ram3);

/* RAM region to be used for the nanoFramework CLR managed heap.*/
REGION_ALIAS("CLR_MANAGED_HEAP_RAM", ram0);

INCLUDE rules_STM32F7xx.ld
