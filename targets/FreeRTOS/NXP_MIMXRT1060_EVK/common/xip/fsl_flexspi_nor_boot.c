//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright 2017 NXP. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "fsl_flexspi_nor_boot.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.xip_device"
#endif

/************************************* 
 *  IVT Data 
 *************************************/

__attribute__((section(".boot_hdr.ivt")))
const ivt image_vector_table = {
  IVT_HEADER,                         /* IVT Header */
  IMAGE_ENTRY_ADDRESS,                /* Image Entry Function */
  IVT_RSVD,                           /* Reserved = 0 */
  (uint32_t)DCD_ADDRESS,              /* Address where DCD information is stored */
  (uint32_t)BOOT_DATA_ADDRESS,        /* Address where BOOT Data Structure is stored */
  (uint32_t)&image_vector_table,      /* Pointer to IVT Self (absolute address */
  (uint32_t)CSF_ADDRESS,              /* Address where CSF file is stored */
  IVT_RSVD                            /* Reserved = 0 */
};

/************************************* 
 *  Boot Data 
 *************************************/
__attribute__((section(".boot_hdr.boot_data")))
const BOOT_DATA_T boot_data = {
  FLASH_BASE,                 /* boot start location */
  FLASH_SIZE,                 /* size */
  PLUGIN_FLAG,                /* Plugin flag*/
  0xFFFFFFFF  				  /* empty - extra data word */
};


/*************************************
 *  DCD Data
 *************************************/
const uint8_t dcd_data[] = {0x00};
