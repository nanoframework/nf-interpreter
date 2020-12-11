/*
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//
*/

/*
 * STM32F76xxI generic setup.
 * 
 * RAM0 - Data, Heap.
 * RAM3 - Main Stack, Process Stack, BSS, NOCACHE, ETH.
 *
 * Notes:
 * BSS is placed in DTCM RAM in order to simplify DMA buffers management.
 */
MEMORY
{
    flash0       (rx) : org = 0x08010000, len = 2M - 32k - 32k - 1280k    /* flash size less the space reserved for nanoBooter, configuration block and application deployment*/
    flash1       (rx) : org = 0x00210000, len = 2M - 32k - 32k - 1280k
    flash2       (rx) : org = 0x00000000, len = 0
    flash3       (rx) : org = 0x00000000, len = 0
    flash4       (rx) : org = 0x00000000, len = 0
    flash5       (rx) : org = 0x00000000, len = 0
    flash6       (rx) : org = 0x00000000, len = 0
    flash7       (rx) : org = 0x00000000, len = 0
    config       (rw) : org = 0x08008000, len = 32k                       /* space reserved for configuration block */
    deployment   (rx) : org = 0x080C0000, len = 1280k                     /* space reserved for application deployment */
    ramvt        (wx) : org = 0x00000000, len = 0                         /* initial RAM address is reserved for a copy of the vector table */
    ram0         (wx) : org = 0x20020000, len = 384k                      /* SRAM1 + SRAM2 */
    ram1         (wx) : org = 0x20020000, len = 368k                      /* SRAM1 */
    ram2         (wx) : org = 0x2007C000, len = 16k                       /* SRAM2 */
    ram3         (wx) : org = 0x20000030, len = 128k-48                   /* DTCM-RAM */
    ram4         (wx) : org = 0x00000000, len = 16k                       /* ITCM-RAM */
    ram5         (wx) : org = 0x40024000, len = 4k                        /* BCKP SRAM */
    ram6         (wx) : org = 0x00000000, len = 0
    ram7         (wx) : org = 0x00000000, len = 0
    ext_ram      (wx) : org = 0xC0000000, len = 13631488                  /* external SDRAM */
    graphics_ram (wx) : org = 0xC0D00000, len = 2377728                  /* external graphics ram for frame buffer and bitmaps*/
    graphics_vfb (wx) : org = 0xC0F44800, len = 768000                   /* Frame buffer for video output mode  800*480*2 bytes ( 16 bits RGB565 )*/
    bootclpbrd   (wx) : org = 0x20000000, len = 48                        /* boot clipboard area */
}

/* For each data/text section two region are defined, a virtual region
   and a load region (_LMA suffix).*/

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

/* RAM region to be used for Main stack. This stack accommodates the processing
   of all exceptions and interrupts.*/
REGION_ALIAS("MAIN_STACK_RAM", ram3);

/* RAM region to be used for the process stack. This is the stack used by
   the main() function.*/
REGION_ALIAS("PROCESS_STACK_RAM", ram3);

/* RAM region to be used for data segment.*/
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash0);

/* RAM region to be used for BSS segment.*/
REGION_ALIAS("BSS_RAM", ram3);

/* RAM region to be used for the default heap.*/
REGION_ALIAS("HEAP_RAM", ram0);

/* RAM region to be used for the nanoFramework CLR managed heap.*/
REGION_ALIAS("CLR_MANAGED_HEAP_RAM", ext_ram);

/* RAM region to be used for the nanoFramework graphics heap.*/
REGION_ALIAS("GRAPHICS_HEAP_RAM", graphics_ram);

/* RAM region to be used for the nanoFramework graphics heap.*/
REGION_ALIAS("GRAPHICS_VIDEO_FRAME_BUFFER", graphics_vfb);

/* RAM region to be used for the boot clipboard.*/
REGION_ALIAS("SECTION_FOR_BOOTCLIPBOARD", bootclpbrd);

/* rules inclusion.*/
INCLUDE rules_stacks.ld

/*===========================================================================*/
/* Custom sections for STM32F7xx.                                            */
/*===========================================================================*/

/* RAM region to be used for nocache segment.*/
REGION_ALIAS("NOCACHE_RAM", ram3);

/* RAM region to be used for eth segment.*/
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

/* nanoCLR rules inclusion.*/
INCLUDE rules_clr.ld

/* Graphics rules inclusion.*/
INCLUDE rules_graphics.ld

/* boot clipboard rules inclusion.*/
INCLUDE rules_bootclipboard.ld
