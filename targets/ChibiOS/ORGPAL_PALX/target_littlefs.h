//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_LITTLEFS_H
#define TARGET_LITTLEFS_H

#include <hal.h>
#include <cache.h>
#include <nanoHAL_v2.h>
#include <lfs.h>

// need to define how many litllefs instances we'll be running
#define LITTLEFS_INSTANCES_COUNT 1

#define LFS_DRIVER_QSPI (1)

// timeout for flash operation (4s)
// max timing for write & erase operations (except chip erase)
#define HAL_SPI_TIMEOUT_DEFAULT_VALUE ((uint32_t)4000)

/////////////////////////
// W25Q512 Configuration
// 512 MBits => 64MBytes ()
#define W25Q512_FLASH_SIZE 0x4000000
// 1028 blocks of 64KBytes
#define W25Q512_BLOCK_SIZE 0x10000
// 416384 sectors of 4kBytes (!!minimum erase size!!)
#define W25Q512_SECTOR_SIZE 0x1000
// 262144 pages of 256 bytes
#define W25Q512_PAGE_SIZE 0x100

// timings
#define W25Q512_DUMMY_CYCLES_READ      8
#define W25Q512_DUMMY_CYCLES_READ_QUAD 8
#define W25Q512_BULK_ERASE_MAX_TIME    10000
#define W25Q512_BLOCK_ERASE_MAX_TIME   3000
#define W25Q512_SECTOR_ERASE_MAX_TIME  800

// W25Q512 Commands
// Reset Operations
#define RESET_ENABLE_CMD 0x66
#define RESET_MEMORY_CMD 0x99

// Identification Operations
#define READ_ID_CMD  0x90
#define READ_ID_CMD2 0x9F

// Read Operations
#define QUAD_OUT_FAST_READ_CMD 0x6B

// Write Operations
#define WRITE_ENABLE_CMD 0x06

// Register Operations
#define READ_STATUS_REG1_CMD  0x05
#define READ_STATUS_REG2_CMD  0x35
#define WRITE_STATUS_REG2_CMD 0x31

// Program Operations
#define PAGE_PROG_CMD         0x02
#define QUAD_IN_FAST_PROG_CMD 0x32

// Erase Operations
#define SECTOR_ERASE_CMD 0x20

//  W25Q512 Registers
// Status Register 1
#define W25Q512_SR_WIP      ((uint8_t)0x01) /*!< Write in progress */
#define W25Q512_SR_WREN     ((uint8_t)0x02) /*!< Write enable latch */
#define W25Q512_SR_BLOCKPR  ((uint8_t)0x5C) /*!< Block protected against program and erase operations */
#define W25Q512_SR_PRBOTTOM ((uint8_t)0x20) /*!< Protected memory area defined by BLOCKPR starts from top or bottom */
#define W25Q512_SR_SRWREN   ((uint8_t)0x80) /*!< Status register write enable/disable */
// Status Register 2
#define W25Q512_SR2_QE ((uint8_t)0x02) /*!< Quad Enable */

// Nonvolatile Configuration Register
#define W25Q512_NVCR_LOCK     ((uint16_t)0x0001) /*!< Lock nonvolatile configuration register */
#define W25Q512_NVCR_DUAL     ((uint16_t)0x0004) /*!< Dual I/O protocol */
#define W25Q512_NVCR_QUAB     ((uint16_t)0x0008) /*!< Quad I/O protocol */
#define W25Q512_NVCR_RH       ((uint16_t)0x0010) /*!< Reset/hold */
#define W25Q512_NVCR_ODS      ((uint16_t)0x01C0) /*!< Output driver strength */
#define W25Q512_NVCR_XIP      ((uint16_t)0x0E00) /*!< XIP mode at power-on reset */
#define W25Q512_NVCR_NB_DUMMY ((uint16_t)0xF000) /*!< Number of dummy clock cycles */

// Volatile Configuration Register
#define W25Q512_VCR_WRAP     ((uint8_t)0x03) /*!< Wrap */
#define W25Q512_VCR_XIP      ((uint8_t)0x08) /*!< XIP */
#define W25Q512_VCR_NB_DUMMY ((uint8_t)0xF0) /*!< Number of dummy clock cycles */

// Enhanced Volatile Configuration Register
#define W25Q512_EVCR_ODS  ((uint8_t)0x07) /*!< Output driver strength */
#define W25Q512_EVCR_VPPA ((uint8_t)0x08) /*!< Vpp accelerator */
#define W25Q512_EVCR_RH   ((uint8_t)0x10) /*!< Reset/hold */
#define W25Q512_EVCR_DUAL ((uint8_t)0x40) /*!< Dual I/O protocol */
#define W25Q512_EVCR_QUAD ((uint8_t)0x80) /*!< Quad I/O protocol */

// Flag Status Register
#define W25Q512_FSR_PRERR  ((uint8_t)0x02) /*!< Protection error */
#define W25Q512_FSR_PGSUS  ((uint8_t)0x04) /*!< Program operation suspended */
#define W25Q512_FSR_VPPERR ((uint8_t)0x08) /*!< Invalid voltage during program or erase */
#define W25Q512_FSR_PGERR  ((uint8_t)0x10) /*!< Program error */
#define W25Q512_FSR_ERERR  ((uint8_t)0x20) /*!< Erase error */
#define W25Q512_FSR_ERSUS  ((uint8_t)0x40) /*!< Erase operation suspended */
#define W25Q512_FSR_READY  ((uint8_t)0x80) /*!< Ready or command in progress */

// IDs
#define W25Q512_MANUFACTURER_ID ((uint8_t)0xEF)
#define W25Q512_DEVICE_ID1      ((uint8_t)0x40)
#define W25Q512_DEVICE_ID2      ((uint8_t)0x20)

//////////////////////////////////
// remapping into littlefs defines

#define LFS0_READ_SIZE      1
#define LFS0_PROG_SIZE      1
#define LFS0_BLOCK_SIZE     W25Q512_SECTOR_SIZE
#define LFS0_BLOCK_COUNT    W25Q512_FLASH_SIZE / W25Q512_SECTOR_SIZE
#define LFS0_BLOCK_CYCLES   100
#define LFS0_CACHE_SIZE     W25Q512_PAGE_SIZE
#define LFS0_LOOKAHEAD_SIZE LFS0_BLOCK_COUNT / 8

#define LFS0_READ_HANDLER  hal_lfs_read_0
#define LFS0_PROG_HANDLER  hal_lfs_prog_0
#define LFS0_ERASE_HANDLER hal_lfs_erase_0
#define LFS0_SYNC_HANDLER  hal_lfs_sync_

#ifdef __cplusplus
extern "C"
{
#endif

    bool hal_lfs_erase_chip_0();
    int32_t hal_lfs_sync_(const struct lfs_config *c);
    int32_t hal_lfs_erase_0(const struct lfs_config *c, lfs_block_t block);
    int32_t hal_lfs_read_0(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
    int32_t hal_lfs_prog_0(
        const struct lfs_config *c,
        lfs_block_t block,
        lfs_off_t off,
        const void *buffer,
        lfs_size_t size);

#ifdef __cplusplus
}
#endif

#endif // TARGET_LITTLEFS_H
