//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

////////////////////////////////////
// buffers configuration
#define SPIFFS_LOGICAL_PAGE_SIZE        (256)

#define SPIFFS_WORK_BUFFER_SIZE         (SPIFFS_LOGICAL_PAGE_SIZE * 2)
#define SPIFFS_FILE_DESCRIPTORS_SPACE   (32 * 2)
#define SPIFFS_CACHE_SIZE               ((SPIFFS_LOGICAL_PAGE_SIZE + 32) * 4)

#define SPIFFS_DRIVER_QSPI              (1)

//  MX25L512 Configuration   
#define MX25L512_FLASH_SIZE                  0x4000000 /* 512 MBits => 64MBytes */
#define MX25L512_SECTOR_SIZE                 0x10000   /* 1024 sectors of 64KBytes */
#define MX25L512_SUBSECTOR_SIZE              0x1000    /* 16384 subsectors of 4kBytes */
#define MX25L512_PAGE_SIZE                   0x100     /* 262144 pages of 256 bytes */

#define MX25L512_DUMMY_CYCLES_READ_QUAD      3
#define MX25L512_DUMMY_CYCLES_READ           8
#define MX25L512_DUMMY_CYCLES_READ_QUAD_IO   10
#define MX25L512_DUMMY_CYCLES_READ_DTR       6
#define MX25L512_DUMMY_CYCLES_READ_QUAD_DTR  8

#define MX25L512_BULK_ERASE_MAX_TIME         600000
#define MX25L512_SECTOR_ERASE_MAX_TIME       2000
#define MX25L512_SUBSECTOR_ERASE_MAX_TIME    800

//  MX25L512 Commands 
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

/* Identification Operations */
#define READ_ID_CMD                          0x9F
#define MULTIPLE_IO_READ_ID_CMD              0xAF
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A

/* Read Operations */
#define READ_CMD                             0x03
#define READ_4_BYTE_ADDR_CMD                 0x13

#define FAST_READ_CMD                        0x0B
#define FAST_READ_DTR_CMD                    0x0D
#define FAST_READ_4_BYTE_ADDR_CMD            0x0C

#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x3C

#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define DUAL_INOUT_FAST_READ_DTR_CMD         0xBD
#define DUAL_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xBC

#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x6C

#define QUAD_INOUT_FAST_READ_CMD             0xEB
#define QUAD_INOUT_FAST_READ_DTR_CMD         0xED  
#define QSPI_READ_4_BYTE_ADDR_CMD            0xEC

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG_CMD                  0x05
#define READ_CFG_REG_CMD                     0x15   
#define WRITE_STATUS_CFG_REG_CMD             0x01

#define READ_LOCK_REG_CMD                    0x2D
#define WRITE_LOCK_REG_CMD                   0x2C

#define READ_EXT_ADDR_REG_CMD                0xC8
#define WRITE_EXT_ADDR_REG_CMD               0xC5

/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QSPI_PAGE_PROG_4_BYTE_ADDR_CMD       0x12

#define QUAD_IN_FAST_PROG_CMD                0x38
#define EXT_QUAD_IN_FAST_PROG_CMD            0x38 
#define QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD    0x3E

/* Erase Operations */
#define SUBSECTOR_ERASE_CMD                  0x20
#define SUBSECTOR_ERASE_4_BYTE_ADDR_CMD      0x21
   
#define SECTOR_ERASE_CMD                     0xD8
#define SECTOR_ERASE_4_BYTE_ADDR_CMD         0xDC

#define BULK_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x30
#define PROG_ERASE_SUSPEND_CMD               0xB0

/* 4-byte Address Mode Operations */
#define ENTER_4_BYTE_ADDR_MODE_CMD           0xB7
#define EXIT_4_BYTE_ADDR_MODE_CMD            0xE9

/* Quad Operations */
#define ENTER_QUAD_CMD                       0x35
#define EXIT_QUAD_CMD                        0xF5
  
/* Added for compatibility */ 
#define QPI_READ_4_BYTE_ADDR_CMD             QSPI_READ_4_BYTE_ADDR_CMD
#define QPI_PAGE_PROG_4_BYTE_ADDR_CMD        QSPI_PAGE_PROG_4_BYTE_ADDR_CMD


//  MX25L512 Registers
/* Status Register */
#define MX25L512_SR_WIP                      ((uint8_t)0x01)    /*!< Write in progress */
#define MX25L512_SR_WREN                     ((uint8_t)0x02)    /*!< Write enable latch */
#define MX25L512_SR_BLOCKPR                  ((uint8_t)0x5C)    /*!< Block protected against program and erase operations */
#define MX25L512_SR_PRBOTTOM                 ((uint8_t)0x20)    /*!< Protected memory area defined by BLOCKPR starts from top or bottom */
#define MX25L512_SR_QUADEN                   ((uint8_t)0x40)    /*!< Quad IO mode enabled if =1 */
#define MX25L512_SR_SRWREN                   ((uint8_t)0x80)    /*!< Status register write enable/disable */

/* Configuration Register */
#define MX25L512_CR_ODS                      ((uint8_t)0x07)    /*!< Output driver strength */
#define MX25L512_CR_ODS_30                   ((uint8_t)0x07)    /*!< Output driver strength 30 ohms (default)*/
#define MX25L512_CR_ODS_15                   ((uint8_t)0x06)    /*!< Output driver strength 15 ohms */
#define MX25L512_CR_ODS_20                   ((uint8_t)0x05)    /*!< Output driver strength 20 ohms */
#define MX25L512_CR_ODS_45                   ((uint8_t)0x03)    /*!< Output driver strength 45 ohms */
#define MX25L512_CR_ODS_60                   ((uint8_t)0x02)    /*!< Output driver strength 60 ohms */
#define MX25L512_CR_ODS_90                   ((uint8_t)0x01)    /*!< Output driver strength 90 ohms */
#define MX25L512_CR_TB                       ((uint8_t)0x08)    /*!< Top/Bottom bit used to configure the block protect area */
#define MX25L512_CR_PBE                      ((uint8_t)0x10)    /*!< Preamble Bit Enable */
#define MX25L512_CR_4BYTE                    ((uint8_t)0x20)    /*!< 3-bytes or 4-bytes addressing */
#define MX25L512_CR_NB_DUMMY                 ((uint8_t)0xC0)    /*!< Number of dummy clock cycles */

#define MX25L512_FLASH_SIZE             0x4000000 // 512MBits => 64MBytes
#define MX25L512_SECTOR_SIZE            0x10000   // 1024 sectors of 64KBytes
#define MX25L512_SUBSECTOR_SIZE         0x1000    // 16384 subsectors of 4kBytes
#define MX25L512_PAGE_SIZE              0x100     // 262144 pages of 256 bytes

#define MX25L512_SUBSECTOR_ERASE_MAX_TIME    800
#define MX25L512_MANUFACTURER_ID             ((uint8_t)0xC2)
#define MX25L512_DEVICE_ID_MEM_TYPE          ((uint8_t)0x20)
#define MX25L512_DEVICE_ID_MEM_CAPACITY      ((uint8_t)0x1A)

////////////////////////////////

#define SPIFFS_TOTAL_SIZE               MX25L512_FLASH_SIZE
#define SPIFFS_ERASE_BLOCK_SIZE         MX25L512_SECTOR_SIZE
#define SPIFFS_LOGICAL_BLOCK_SIZE       (64*1024)
