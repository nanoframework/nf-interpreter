//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _FLEXSPI_NOR_FLASH_OPS_H_
#define _FLEXSPI_NOR_FLASH_OPS_H_

#include "fsl_flexspi.h"

#define FLASH_SIZE 0x1C00 /* 7168kB */
#define FLASH_PAGE_SIZE 256
#define SECTOR_SIZE 0x1000 /* 4K */


#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL 7
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST 13
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD 0
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS 1
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE 2
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR 3
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE 6
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD 4
#define NOR_CMD_LUT_SEQ_IDX_READID 8
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG 9
#define NOR_CMD_LUT_SEQ_IDX_ENTERQPI 10
#define NOR_CMD_LUT_SEQ_IDX_EXITQPI 11
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG 12
#define NOR_CMD_LUT_SEQ_IDX_ERASECHIP 5

#define CUSTOM_LUT_LENGTH 60
#define FLASH_BUSY_STATUS_POL 1
#define FLASH_BUSY_STATUS_OFFSET 0

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
__attribute__ ((section(".ramfunc.$RAM2")))
static inline void flexspi_clock_init(void)
{
    CLOCK_SetMux(kCLOCK_FlexspiMux, 0x2); /* Choose PLL2 PFD2 clock as flexspi source clock. 396M */
    CLOCK_SetDiv(kCLOCK_FlexspiDiv, 2);   /* flexspi clock 133M. */
}

void flexspi_nor_flash_init(FLEXSPI_Type *base);
status_t flexspi_nor_erase_chip(FLEXSPI_Type *base);
status_t flexspi_nor_get_vendor_id(FLEXSPI_Type *base, uint8_t *vendorId);
status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t dstAddr, const uint32_t *src);
status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address);
status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base);

/*${prototype:end}*/
#endif /* _FLEXSPI_NOR_FLASH_OPS_H_ */