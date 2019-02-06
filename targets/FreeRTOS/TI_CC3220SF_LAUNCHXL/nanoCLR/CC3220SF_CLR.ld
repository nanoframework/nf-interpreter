/*****************************************************************************
* blinky.ld
*
* GCC Linker script for blinky application.
*
* Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

/*
//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//
*/

/*
 * CC3220SF generic setup.
 * 
 * RAM0 - Data, Heap.
 * RAM3 - Main Stack, Process Stack, BSS, NOCACHE, ETH.
 *
 * Notes:
 * BSS is placed in DTCM RAM in order to simplify DMA buffers management.
 */
MEMORY
{
    flash       : org = 0x01000800, len = 0x0FF800          /* flash size less the space reserved for application deployment*/
    flash_itcm  : org = 0x00000000, len = 0
    deployment  : org = 0x01000800, len = 0                 /* space reserved for application deployment */
    ramvt       : org = 0x00000000, len = 0                 /* initial RAM address is reserved for a copy of the vector table */
    ram0        : org = 0x20000000, len = 0x00040000        /* SRAM */
    ram1        : org = 0x00000000, len = 0                 /*  */
    ram2        : org = 0x00000000, len = 0                 /*  */
    ram3        : org = 0x00000000, len = 0                 /*  */
    ram4        : org = 0x00000000, len = 0                 /*  */
    ram5        : org = 0x00000000, len = 0                 /*  */
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
REGION_ALIAS("HEAP_RAM", ram0);

/* RAM region to be used for the nanoFramework CLR managed heap.*/
REGION_ALIAS("CLR_MANAGED_HEAP_RAM", ram0);

/* rules inclusion.*/
INCLUDE rules_stacks.ld

/* Code rules inclusion.*/
INCLUDE rules_code.ld

/* Data rules inclusion.*/
INCLUDE rules_data.ld

/* nanoCLR rules inclusion.*/
INCLUDE rules_clr.ld
