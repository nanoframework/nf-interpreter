//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright 2017 NXP. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef __FLEXSPI_NOR_BOOT_H__
#define __FLEXSPI_NOR_BOOT_H__

#include <stdint.h>
#include "board.h"

/*! @name Driver version */
/*@{*/
/*! @brief XIP_DEVICE driver version 2.0.0. */
#define FSL_XIP_DEVICE_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/************************************* 
 *  IVT Data 
 *************************************/
typedef struct _ivt_ {
    /** @ref hdr with tag #HAB_TAG_IVT, length and HAB version fields
     *  (see @ref data)
     */
    uint32_t hdr;
    /** Absolute address of the first instruction to execute from the
     *  image
     */
    uint32_t entry;
    /** Reserved in this version of HAB: should be NULL. */
    uint32_t reserved1;
    /** Absolute address of the image DCD: may be NULL. */
    uint32_t dcd;
    /** Absolute address of the Boot Data: may be NULL, but not interpreted
     *  any further by HAB
     */
    uint32_t boot_data;
    /** Absolute address of the IVT.*/
    uint32_t self;
    /** Absolute address of the image CSF.*/
    uint32_t csf;
    /** Reserved in this version of HAB: should be zero. */
    uint32_t reserved2;
} ivt;

#define IVT_MAJOR_VERSION           0x4
#define IVT_MAJOR_VERSION_SHIFT     0x4
#define IVT_MAJOR_VERSION_MASK      0xF
#define IVT_MINOR_VERSION           0x1
#define IVT_MINOR_VERSION_SHIFT     0x0
#define IVT_MINOR_VERSION_MASK      0xF

#define IVT_VERSION(major, minor)   \
  ((((major) & IVT_MAJOR_VERSION_MASK) << IVT_MAJOR_VERSION_SHIFT) |  \
  (((minor) & IVT_MINOR_VERSION_MASK) << IVT_MINOR_VERSION_SHIFT))

/* IVT header */  
#define IVT_TAG_HEADER        0xD1       /**< Image Vector Table */
#define IVT_SIZE              0x2000
#define IVT_PAR               IVT_VERSION(IVT_MAJOR_VERSION, IVT_MINOR_VERSION)
#define IVT_HEADER           (IVT_TAG_HEADER | (IVT_SIZE << 8) | (IVT_PAR << 24))


extern uint32_t __Vectors[];
extern uint32_t __boot_hdr_start__[];
#define IMAGE_ENTRY_ADDRESS ((uint32_t)__Vectors)
#define FLASH_BASE          ((uint32_t)__boot_hdr_start__)

#define DCD_ADDRESS           dcd_data
#define BOOT_DATA_ADDRESS     &boot_data
#define CSF_ADDRESS           0
#define IVT_RSVD             (uint32_t)(0x00000000)

/************************************* 
 *  Boot Data 
 *************************************/
typedef struct _boot_data_ {
  uint32_t start;           /* boot start location */
  uint32_t size;            /* size */
  uint32_t plugin;          /* plugin flag - 1 if downloaded application is plugin */
  uint32_t placeholder;		/* placehoder to make even 0x10 size */
}BOOT_DATA_T;

#define FLASH_SIZE            BOARD_FLASH_SIZE
#define PLUGIN_FLAG           (uint32_t)0

/* External Variables */
const BOOT_DATA_T boot_data;
extern const uint8_t dcd_data[];

#endif /* __FLEXSPI_NOR_BOOT_H__ */

