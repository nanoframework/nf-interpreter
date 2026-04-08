/*
    ChibiOS - Copyright (C) 2006-2026 Giovanni Di Sirio.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    RP2040/hal_efl_lld.c
 * @brief   RP2040 Embedded Flash subsystem low level driver source.
 * @note    This is a self-contained flash driver that directly manipulates
 *          the SSI peripheral.
 * @note    Erase operations are non-blocking: @p efl_lld_start_erase_sector()
 *          sends the command and returns immediately; the caller must poll
 *          @p efl_lld_query_erase() for completion.  Program and status-poll
 *          operations lock interrupts only for brief XIP-transition windows
 *          (~100-200 us), allowing USB and other time-critical ISRs to
 *          execute between operations.
 *
 * @addtogroup HAL_EFL
 * @{
 */

#include <string.h>

#include "hal.h"

#if (HAL_USE_EFL == TRUE) || defined(__DOXYGEN__)

/* The erase/query functions are called through ChibiOS vtable pointers
   so the compiler cannot see the references.  Suppress the false
   "defined but not used" warning for these non-static functions. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/**
 * @brief   Macro to place functions in RAM.
 */
#define RAMFUNC __attribute__((noinline, section(".ramtext")))

/**
 * @name    SSI register offsets
 * @{
 */
#define SSI_CTRLR0                          0x00U
#define SSI_CTRLR1                          0x04U
#define SSI_SSIENR                          0x08U
#define SSI_SER                             0x10U
#define SSI_BAUDR                           0x14U
#define SSI_TXFLR                           0x20U
#define SSI_RXFLR                           0x24U
#define SSI_SR                              0x28U
#define SSI_ICR                             0x48U
#define SSI_DR0                             0x60U
#define SSI_SPI_CTRLR0                      0xF4U
/** @} */

/**
 * @name    SSI status register bits
 * @{
 */
#define SSI_SR_TFE                          (1U << 2)   /* TX FIFO empty */
#define SSI_SR_BUSY                         (1U << 0)   /* SSI busy */
/** @} */

/**
 * @name    SSI CTRLR0 bits
 * @{
 */
#define SSI_CTRLR0_DFS_32_POS               16U
#define SSI_CTRLR0_DFS_32_8BIT              (7U << SSI_CTRLR0_DFS_32_POS)
/** @} */

/**
 * @name    SSI configuration for direct SPI mode
 * @{
 */
#define SSI_BAUDR_DEFAULT                   6U
/** @} */

/**
 * @name    SSI configuration for XIP mode
 * @{
 */
/* SPI_FRF=0 DFS_32=31 TMOD=3 */
#define SSI_CTRLR0_XIP                      0x001F0300U

/* XIP_CMD=0x03 INST_L=2 ADDR_L=6 TRANS_TYPE=0 */
#define SSI_SPI_CTRLR0_XIP                  0x03000218U
/** @} */

/**
 * @name    IO QSPI base and register offsets
 * @{
 */
#define RP_IOQSPI_BASE                      0x40018000U
#define IOQSPI_GPIO_QSPI_SS_CTRL            0x0CU
/** @} */

/**
 * @name    IO QSPI output override values
 * @{
 */
#define IOQSPI_OUTOVER_NORMAL               0U
#define IOQSPI_OUTOVER_LOW                  2U
#define IOQSPI_OUTOVER_HIGH                 3U
/** @} */

/**
 * @name    XIP control register offsets
 * @{
 */
#define XIP_CTRL_OFF                        0x00U
#define XIP_FLUSH_OFF                       0x04U
#define XIP_STAT_OFF                        0x08U
/** @} */

/**
 * @name    PADS QSPI base and register offsets
 * @{
 */
#define RP_PADS_QSPI_BASE                   0x40020000U
#define PADS_QSPI_GPIO_QSPI_SD0             0x08U
#define PADS_QSPI_GPIO_QSPI_SD1             0x0CU
#define PADS_QSPI_GPIO_QSPI_SD2             0x10U
#define PADS_QSPI_GPIO_QSPI_SD3             0x14U
/** @} */

/**
 * @name    PADS QSPI control bits
 * @{
 */
#define PADS_QSPI_OD_BITS                   (1U << 7)   /* Output disable */
#define PADS_QSPI_IE_BITS                   (1U << 6)   /* Input enable */
#define PADS_QSPI_PUE_BITS                  (1U << 3)   /* Pull up enable */
#define PADS_QSPI_PDE_BITS                  (1U << 2)   /* Pull down enable */
/** @} */

/**
 * @name    Standard JEDEC Flash commands
 * @{
 */
#define FLASHCMD_WRITE_ENABLE               0x06U
#define FLASHCMD_READ_STATUS                0x05U
#define FLASHCMD_PAGE_PROGRAM               0x02U
#define FLASHCMD_SECTOR_ERASE               0x20U
#define FLASHCMD_BLOCK_ERASE_32K            0x52U
#define FLASHCMD_BLOCK_ERASE_64K            0xD8U
#define FLASHCMD_READ_UNIQUE_ID             0x4BU
/** @} */

/**
 * @name    Flash status register bits
 * @{
 */
#define FLASH_STATUS_BUSY                   (1U << 0)
#define FLASH_STATUS_WEL                    (1U << 1)
/** @} */

/**
 * @name    Page alignment
 * @{
 */
#define RP_FLASH_PAGE_MASK                  (RP_FLASH_PAGE_SIZE - 1U)
/** @} */

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   EFL1 driver identifier.
 * @note    EFLD1.ssi is statically initialized to allow use before hal init
 */
EFlashDriver EFLD1 = {
  .ssi = (volatile uint32_t *)RP_SSI_BASE
};

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

static const flash_descriptor_t efl_lld_descriptor = {
  .attributes       = FLASH_ATTR_ERASED_IS_ONE |
                      FLASH_ATTR_MEMORY_MAPPED |
                      FLASH_ATTR_REWRITABLE,
  .page_size        = RP_FLASH_PAGE_SIZE,
  .sectors_count    = RP_FLASH_SECTORS_COUNT,
  .sectors          = NULL,
  .sectors_size     = RP_FLASH_SECTOR_SIZE,
  .address          = (uint8_t *)RP_FLASH_BASE,
  .size             = RP_FLASH_SIZE
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Force chip select high or low
 * @note    This function MUST be in RAM.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @param[in] high      true for CS high (deassert), false for CS low (assert)
 */
RAMFUNC static void rp_flash_cs_force(EFlashDriver *eflp, bool high) {
  (void)eflp;

  volatile uint32_t *ioqspi_ss_ctrl =
      (volatile uint32_t *)(RP_IOQSPI_BASE + IOQSPI_GPIO_QSPI_SS_CTRL);
  uint32_t val = high ? IOQSPI_OUTOVER_HIGH : IOQSPI_OUTOVER_LOW;

  *ioqspi_ss_ctrl = (*ioqspi_ss_ctrl & ~(3U << 8)) | (val << 8);

  /* Read back to ensure write is flushed */
  (void)*ioqspi_ss_ctrl;
}

/**
 * @brief   Transfer data to/from flash
 * @note    This function MUST be in RAM.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @param[in] tx        transmit buffer (NULL to send zeros)
 * @param[out] rx       receive buffer (NULL to discard)
 * @param[in] count     number of bytes to transfer
 */
RAMFUNC static void rp_flash_put_get(EFlashDriver *eflp, const uint8_t *tx,
                                     uint8_t *rx, size_t count) {
  volatile uint32_t *ssi = eflp->ssi;
  size_t tx_remaining = count;
  size_t rx_remaining = count;
  const size_t max_in_flight = 14U; /* FIFO is 16 deep so we leave a margin */

  while ((tx_remaining > 0U) || (rx_remaining > 0U)) {
    size_t in_flight = (count - tx_remaining) - (count - rx_remaining);

    while ((tx_remaining > 0U) && (in_flight < max_in_flight)) {
      uint8_t data = (tx != NULL) ? *tx++ : 0U;
      ssi[SSI_DR0 / 4U] = data;
      tx_remaining--;
      in_flight++;
    }

    while ((rx_remaining > 0U) && (ssi[SSI_RXFLR / 4U] > 0U)) {
      uint8_t data = (uint8_t)ssi[SSI_DR0 / 4U];
      if (rx != NULL) {
        *rx++ = data;
      }
      rx_remaining--;
    }
  }
}

/**
 * @brief   Execute a flash command.
 * @note    This function MUST be in RAM.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @param[in] cmd       command byte
 * @param[in] tx        transmit data after command (NULL if none)
 * @param[out] rx       receive buffer (NULL to discard)
 * @param[in] count     number of bytes to transfer after command
 */
RAMFUNC static void rp_flash_do_cmd(EFlashDriver *eflp, uint8_t cmd,
                                    const uint8_t *tx, uint8_t *rx,
                                    size_t count) {
  /* Assert CS. */
  rp_flash_cs_force(eflp, false);

  /* Send command byte. */
  eflp->ssi[SSI_DR0 / 4U] = cmd;
  while (eflp->ssi[SSI_RXFLR / 4U] == 0U) {
  }
  (void)eflp->ssi[SSI_DR0 / 4U];

  /* Transfer remaining data. */
  if (count > 0U) {
    rp_flash_put_get(eflp, tx, rx, count);
  }

  /* Deassert CS. */
  rp_flash_cs_force(eflp, true);
}

/**
 * @brief   Send write enable command.
 * @note    This function MUST be in RAM.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 */
RAMFUNC static void rp_flash_write_enable(EFlashDriver *eflp) {

  rp_flash_do_cmd(eflp, FLASHCMD_WRITE_ENABLE, NULL, NULL, 0U);
}

/**
 * @brief   Flush XIP cache
 * @note    This function MUST be in RAM.
 */
RAMFUNC static void rp_flash_flush_cache(void) {
  volatile uint32_t *xip = (volatile uint32_t *)RP_XIP_CTRL_BASE;

  /* Write to flush register to trigger cache flush. */
  xip[XIP_FLUSH_OFF / 4U] = 1U;

  /* Read back to block until flush completes (RP2040 stalls the bus
     read until the cache has been fully invalidated). */
  (void)xip[XIP_FLUSH_OFF / 4U];

  /* Enable the cache */
  xip[XIP_CTRL_OFF / 4U] = 1U;
}

/**
 * @brief   Exit XIP mode and configure for direct access.
 * @note    This function MUST be in RAM.
 * @note    This follows a similar pattern to the ROM's flash_exit_xip()
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 */
RAMFUNC static void rp_flash_exit_xip(EFlashDriver *eflp) {
  volatile uint32_t *ssi = eflp->ssi;
  volatile uint32_t *pads_qspi = (volatile uint32_t *)RP_PADS_QSPI_BASE;
  uint32_t padctrl_save;
  uint32_t padctrl_tmp;
  unsigned i;
  volatile unsigned delay;

  /* Save current XIP configuration before modifying. */
  eflp->xip_ctrlr0 = ssi[SSI_CTRLR0 / 4U];
  eflp->xip_ctrlr1 = ssi[SSI_CTRLR1 / 4U];
  eflp->xip_spi_ctrlr0 = ssi[SSI_SPI_CTRLR0 / 4U];
  eflp->xip_baudr = ssi[SSI_BAUDR / 4U];

  /* Wait for any pending work.*/
  while ((ssi[SSI_SR / 4U] & SSI_SR_BUSY) != 0U) {
  }

  /* Default non XIP SPI configuration */
  ssi[SSI_SSIENR / 4U] = 0U;
  ssi[SSI_BAUDR / 4U] = SSI_BAUDR_DEFAULT;
  ssi[SSI_CTRLR0 / 4U] = SSI_CTRLR0_DFS_32_8BIT;
  ssi[SSI_SER / 4U] = 1U;
  ssi[SSI_SSIENR / 4U] = 1U;

  /*
   * Exit continuous read mode sequence:
   * 1. CS high 32 clocks with IO pulled down
   * 2. CS low 32 clocks with IO pulled up
   * 3. Send 0xFF, 0xFF
   */

  /* Save pad control and configure with output disabled.*/
  padctrl_save = pads_qspi[PADS_QSPI_GPIO_QSPI_SD0 / 4U];
  padctrl_tmp = (padctrl_save & ~(PADS_QSPI_OD_BITS | PADS_QSPI_PUE_BITS |
                                  PADS_QSPI_PDE_BITS))
                | PADS_QSPI_OD_BITS | PADS_QSPI_PDE_BITS;

  /* 1. CS high */
  rp_flash_cs_force(eflp, true);

  pads_qspi[PADS_QSPI_GPIO_QSPI_SD0 / 4U] = padctrl_tmp;
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD1 / 4U] = padctrl_tmp;
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD2 / 4U] = padctrl_tmp;
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD3 / 4U] = padctrl_tmp;

  /* Delay for bus settling after pad reconfiguration (~1 us). */
  for (delay = 0U; delay < 32U; delay++) {
  }

  /* Send 4 bytes / 32 clocks */
  for (i = 0U; i < 4U; i++) {
    ssi[SSI_DR0 / 4U] = 0U;
    while (ssi[SSI_RXFLR / 4U] == 0U) {
    }
    (void)ssi[SSI_DR0 / 4U];
  }

  padctrl_tmp = (padctrl_tmp & ~PADS_QSPI_PDE_BITS) | PADS_QSPI_PUE_BITS;

  /* 2. CS low */
  rp_flash_cs_force(eflp, false);

  pads_qspi[PADS_QSPI_GPIO_QSPI_SD0 / 4U] = padctrl_tmp;
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD1 / 4U] = padctrl_tmp;
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD2 / 4U] = padctrl_tmp;
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD3 / 4U] = padctrl_tmp;

  /* Delay for bus settling after pad reconfiguration (~1 us). */
  for (delay = 0U; delay < 32U; delay++) {
  }

  /* Send 4 bytes / 32 clocks */
  for (i = 0U; i < 4U; i++) {
    ssi[SSI_DR0 / 4U] = 0U;
    while (ssi[SSI_RXFLR / 4U] == 0U) {
    }
    (void)ssi[SSI_DR0 / 4U];
  }

  /* Restore pad controls. */
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD0 / 4U] = padctrl_save;
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD1 / 4U] = padctrl_save;
  padctrl_save = (padctrl_save & ~PADS_QSPI_PDE_BITS) | PADS_QSPI_PUE_BITS;
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD2 / 4U] = padctrl_save;
  pads_qspi[PADS_QSPI_GPIO_QSPI_SD3 / 4U] = padctrl_save;

  /* 3. Send 0xFF, 0xFF */
  rp_flash_cs_force(eflp, false);
  ssi[SSI_DR0 / 4U] = 0xFFU;
  ssi[SSI_DR0 / 4U] = 0xFFU;
  while (ssi[SSI_RXFLR / 4U] < 2U) {
  }
  (void)ssi[SSI_DR0 / 4U];
  (void)ssi[SSI_DR0 / 4U];
  rp_flash_cs_force(eflp, true);
}

/**
 * @brief   Enter XIP mode using safe standard SPI 03h reads.
 * @note    This function MUST be in RAM.
 * @note    After @p rp_flash_exit_xip() the flash chip is no longer in
 *          QSPI continuous-read mode.  Restoring the boot2 Quad config
 *          (INST_L=0) would cause cache-miss reads to fail because the
 *          SSI would not send an instruction byte, but the flash expects
 *          one.  Instead we configure standard SPI read (0x03) which
 *          always sends instruction + address and works regardless of
 *          the flash's continuous-read state.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 */
RAMFUNC static void rp_flash_enter_xip(EFlashDriver *eflp) {
  volatile uint32_t *ssi = eflp->ssi;
  volatile uint32_t *ioqspi_ss_ctrl =
      (volatile uint32_t *)(RP_IOQSPI_BASE + IOQSPI_GPIO_QSPI_SS_CTRL);

  /* Reset CS control to normal */
  *ioqspi_ss_ctrl = 0U;

  /* Configure standard SPI 03h read — safe after exit_xip.
     SPI_FRF=0 (standard), DFS_32=31 (32-bit frame), TMOD=3 (EEPROM_READ).
     XIP_CMD=0x03, INST_L=2 (8-bit), ADDR_L=6 (24-bit), TRANS_TYPE=0. */
  ssi[SSI_SSIENR / 4U] = 0U;
  ssi[SSI_BAUDR / 4U] = SSI_BAUDR_DEFAULT;
  ssi[SSI_CTRLR0 / 4U] = SSI_CTRLR0_XIP;
  ssi[SSI_CTRLR1 / 4U] = 0U;
  ssi[SSI_SPI_CTRLR0 / 4U] = SSI_SPI_CTRLR0_XIP;
  ssi[SSI_SER / 4U] = 1U;
  ssi[SSI_SSIENR / 4U] = 1U;

  rp_flash_flush_cache();
}

/**
 * @brief   Enter XIP mode without flushing the cache.
 * @note    This function MUST be in RAM.
 * @note    Uses standard SPI 03h reads (not the boot2 Quad config)
 *          because the flash is not in continuous-read mode after
 *          @p rp_flash_exit_xip().  Standard 03h always sends the
 *          instruction byte, so cache-miss reads work correctly.
 *          Skips the cache flush so that already-cached code remains
 *          available for ISR handlers.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 */
RAMFUNC static void rp_flash_enter_xip_fast(EFlashDriver *eflp) {
  volatile uint32_t *ssi = eflp->ssi;
  volatile uint32_t *ioqspi_ss_ctrl =
      (volatile uint32_t *)(RP_IOQSPI_BASE + IOQSPI_GPIO_QSPI_SS_CTRL);

  /* Reset CS control to normal */
  *ioqspi_ss_ctrl = 0U;

  /* Configure standard SPI 03h read — safe after exit_xip.
     SPI_FRF=0 (standard), DFS_32=31 (32-bit frame), TMOD=3 (EEPROM_READ).
     XIP_CMD=0x03, INST_L=2 (8-bit), ADDR_L=6 (24-bit), TRANS_TYPE=0. */
  ssi[SSI_SSIENR / 4U] = 0U;
  ssi[SSI_BAUDR / 4U] = SSI_BAUDR_DEFAULT;
  ssi[SSI_CTRLR0 / 4U] = SSI_CTRLR0_XIP;
  ssi[SSI_CTRLR1 / 4U] = 0U;
  ssi[SSI_SPI_CTRLR0 / 4U] = SSI_SPI_CTRLR0_XIP;
  ssi[SSI_SER / 4U] = 1U;
  ssi[SSI_SSIENR / 4U] = 1U;

  /* No cache flush — preserve existing cache contents for ISR code. */
}

/**
 * @brief   Program a page of flash.
 * @note    This function MUST be in RAM.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @param[in] offset    flash offset (must be page-aligned or within page)
 * @param[in] data      data to program
 * @param[in] len       number of bytes (must not cross page boundary)
 */
RAMFUNC static void rp_flash_program_page(EFlashDriver *eflp, uint32_t offset,
                                          const uint8_t *data, size_t len) {
  volatile uint32_t *ssi = eflp->ssi;
  uint8_t addr[3];

  /* Send write enable. */
  rp_flash_write_enable(eflp);

  /* Prepare 24-bit address (big-endian). */
  addr[0] = (uint8_t)(offset >> 16);
  addr[1] = (uint8_t)(offset >> 8);
  addr[2] = (uint8_t)offset;

  /* Assert CS. */
  rp_flash_cs_force(eflp, false);

  /* Send page program command. */
  ssi[SSI_DR0 / 4U] = FLASHCMD_PAGE_PROGRAM;
  while (ssi[SSI_RXFLR / 4U] == 0U) {
  }
  (void)ssi[SSI_DR0 / 4U];

  /* Send address. */
  rp_flash_put_get(eflp, addr, NULL, 3U);

  /* Send data. */
  rp_flash_put_get(eflp, data, NULL, len);

  /* Deassert CS — programming starts asynchronously in the flash chip. */
  rp_flash_cs_force(eflp, true);
}

/**
 * @brief   Send an erase command to flash.
 * @note    This function MUST be in RAM.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @param[in] cmd       JEDEC erase command byte
 * @param[in] offset    flash offset (must be aligned to erase unit)
 */
RAMFUNC static void rp_flash_erase_cmd(EFlashDriver *eflp, uint8_t cmd,
                                        uint32_t offset) {
  uint8_t addr[3];

  /* Send write enable. */
  rp_flash_write_enable(eflp);

  /* Prepare 24-bit address (big-endian). */
  addr[0] = (uint8_t)(offset >> 16);
  addr[1] = (uint8_t)(offset >> 8);
  addr[2] = (uint8_t)offset;

  /* Send erase command with address. */
  rp_flash_do_cmd(eflp, cmd, addr, NULL, 3U);
}

/**
 * @brief   Initiate an erase operation without waiting for completion.
 * @note    This function MUST be in RAM. It exits XIP, sends the erase
 *          command, and re-enters XIP using the fast path (no cache flush).
 *          The flash chip erases asynchronously; the caller must poll
 *          @p rp_flash_is_busy() to determine completion, then flush
 *          the cache.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @param[in] cmd       JEDEC erase command byte
 * @param[in] offset    flash offset (must be aligned to erase unit)
 */
RAMFUNC static void rp_flash_start_erase(EFlashDriver *eflp, uint8_t cmd,
                                          uint32_t offset) {

  /* Exit XIP mode. */
  rp_flash_exit_xip(eflp);

  /* Send erase command — the flash chip erases asynchronously. */
  rp_flash_erase_cmd(eflp, cmd, offset);

  /* Re-enter XIP mode without flushing cache. */
  rp_flash_enter_xip_fast(eflp);
}

/**
 * @brief   Initiate a single-page program without waiting for completion.
 * @note    This function MUST be in RAM. It exits XIP, sends the program
 *          command and data, and re-enters XIP using the fast path (no
 *          cache flush).  The flash chip programs asynchronously; the
 *          caller must poll @p rp_flash_is_busy() to determine completion,
 *          then flush the cache.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @param[in] offset    flash offset (must not cross page boundary)
 * @param[in] data      pointer to data in RAM
 * @param[in] len       number of bytes to program
 */
RAMFUNC static void rp_flash_start_program_page(EFlashDriver *eflp,
                                                uint32_t offset,
                                                const uint8_t *data,
                                                size_t len) {

  /* Exit XIP mode. */
  rp_flash_exit_xip(eflp);

  /* Send program command and data — programming starts asynchronously. */
  rp_flash_program_page(eflp, offset, data, len);

  /* Re-enter XIP mode without flushing cache. */
  rp_flash_enter_xip_fast(eflp);
}

/**
 * @brief   Check if flash is busy (direct SPI mode, no XIP transitions).
 * @note    This function MUST be in RAM. It can only be called when the
 *          SSI is already in direct SPI mode (after @p rp_flash_exit_xip).
 *          Unlike @p rp_flash_is_busy(), it does NOT exit/enter XIP,
 *          making it safe to call in a tight loop while the flash chip
 *          is busy (when XIP reads would fail).
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @return              true if flash is still busy
 */
RAMFUNC static bool rp_flash_is_busy_direct(EFlashDriver *eflp) {
  uint8_t status;

  rp_flash_do_cmd(eflp, FLASHCMD_READ_STATUS, NULL, &status, 1U);

  return (status & FLASH_STATUS_BUSY) != 0U;
}

/**
 * @brief   Check if flash is busy, with XIP transitions.
 * @note    This function MUST be in RAM. It briefly exits XIP to read
 *          the flash status register, then re-enters XIP using the fast
 *          path (no cache flush) to preserve the instruction cache for
 *          interrupt handlers.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @return              true if flash is still busy
 */
RAMFUNC static bool rp_flash_is_busy(EFlashDriver *eflp) {
  uint8_t status;

  rp_flash_exit_xip(eflp);
  rp_flash_do_cmd(eflp, FLASHCMD_READ_STATUS, NULL, &status, 1U);
  rp_flash_enter_xip_fast(eflp);

  return (status & FLASH_STATUS_BUSY) != 0U;
}

/**
 * @brief   Read flash unique ID (runs entirely in RAM).
 * @note    This function MUST be in RAM. It handles the entire sequence
 *          from exit XIP to enter XIP so no flash code executes while
 *          XIP is disabled.
 *
 * @param[in] eflp      pointer to the EFlashDriver object
 * @param[out] rx       receive buffer
 * @param[in] count     number of bytes to transfer after command
 */
RAMFUNC static void rp_flash_read_uid_full(EFlashDriver *eflp,
                                            uint8_t *rx, size_t count) {

  /* Exit XIP mode. */
  rp_flash_exit_xip(eflp);

  /* Send read unique ID command. */
  rp_flash_do_cmd(eflp, FLASHCMD_READ_UNIQUE_ID, NULL, rx, count);

  /* Re-enter XIP mode. */
  rp_flash_enter_xip(eflp);
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* RP2040 ROM flash function support.                                        */
/*===========================================================================*/

/**
 * @name    ROM function types and lookup helpers
 * @note    The RP2040 boot ROM contains proven flash programming functions
 *          at well-known addresses.  Using these instead of custom SPI code
 *          avoids subtle timing/configuration issues with the SSI peripheral.
 * @{
 */
typedef void (*rom_void_fn)(void);
typedef void (*rom_flash_range_program_fn)(uint32_t, const uint8_t *, size_t);
typedef void (*rom_flash_range_erase_fn)(uint32_t, size_t, uint32_t, uint8_t);
typedef void *(*rom_table_lookup_fn)(uint16_t *, uint32_t);

#define ROM_HWORD_AS_PTR(addr)  ((uint16_t *)(uint32_t)(*(uint16_t *)(uint32_t)(addr)))
#define ROM_TABLE_LOOKUP_FN     ((rom_table_lookup_fn)(uint32_t)(*(uint16_t *)(uint32_t)0x18U))
#define ROM_FUNC_TABLE_PTR      ROM_HWORD_AS_PTR(0x14U)
#define ROM_CODE(c1, c2)        ((uint32_t)(c2) << 8U | (uint32_t)(c1))

/* Cached ROM function pointers (resolved once). */
static rom_void_fn                rom_connect_internal_flash;
static rom_void_fn                rom_flash_exit_xip;
static rom_flash_range_program_fn rom_flash_range_program;
static rom_flash_range_erase_fn   rom_flash_range_erase;
static rom_void_fn                rom_flash_flush_cache;
static rom_void_fn                rom_flash_enter_cmd_xip;

/**
 * @brief   Resolve ROM flash function pointers (lazy, runs once).
 */
static void rom_flash_lookup(void) {

  if (rom_connect_internal_flash != NULL) {
    return;
  }

  uint16_t *ft = ROM_FUNC_TABLE_PTR;
  rom_table_lookup_fn lookup = ROM_TABLE_LOOKUP_FN;

  rom_connect_internal_flash =
      (rom_void_fn)lookup(ft, ROM_CODE('I', 'F'));
  rom_flash_exit_xip =
      (rom_void_fn)lookup(ft, ROM_CODE('E', 'X'));
  rom_flash_range_program =
      (rom_flash_range_program_fn)lookup(ft, ROM_CODE('R', 'P'));
  rom_flash_range_erase =
      (rom_flash_range_erase_fn)lookup(ft, ROM_CODE('R', 'E'));
  rom_flash_flush_cache =
      (rom_void_fn)lookup(ft, ROM_CODE('F', 'C'));
  rom_flash_enter_cmd_xip =
      (rom_void_fn)lookup(ft, ROM_CODE('C', 'X'));
}

/**
 * @brief   Program one page using ROM functions (runs entirely from RAM).
 * @note    This RAMFUNC wrapper is needed because the ROM's flash_exit_xip
 *          disables XIP, so the caller's code (in flash) cannot execute
 *          between the ROM calls.  By placing this wrapper in RAM, the
 *          CPU can fetch instructions while XIP is off.
 *
 * @param[in] page_base   page-aligned flash offset
 * @param[in] page_buf    pointer to 256-byte page buffer in RAM
 * @param[in] connect     ROM connect_internal_flash function
 * @param[in] exit_xip    ROM flash_exit_xip function
 * @param[in] program     ROM flash_range_program function
 * @param[in] flush       ROM flash_flush_cache function
 * @param[in] enter_xip   ROM flash_enter_cmd_xip function
 */
RAMFUNC static void rp_flash_rom_program_page(
    uint32_t page_base, const uint8_t *page_buf,
    rom_void_fn connect, rom_void_fn exit_xip,
    rom_flash_range_program_fn program,
    rom_void_fn flush, rom_void_fn enter_xip) {

  connect();
  exit_xip();
  program(page_base, page_buf, RP_FLASH_PAGE_SIZE);
  flush();
  enter_xip();
}

/**
 * @brief   Erase a flash region using ROM functions (runs entirely from RAM).
 * @note    The ROM's flash_range_erase handles write-enable, the erase
 *          command, and busy-wait internally.  Total time ~50-400 ms
 *          depending on erase unit size.
 *
 * @param[in] offset      erase-unit-aligned flash offset
 * @param[in] erase_size  erase unit size in bytes (4096 sector, 32K, 64K)
 * @param[in] erase_cmd   JEDEC erase command byte
 * @param[in] connect     ROM connect_internal_flash function
 * @param[in] exit_xip    ROM flash_exit_xip function
 * @param[in] erase       ROM flash_range_erase function
 * @param[in] flush       ROM flash_flush_cache function
 * @param[in] enter_xip   ROM flash_enter_cmd_xip function
 */
RAMFUNC static void rp_flash_rom_erase(
    uint32_t offset, uint32_t erase_size, uint8_t erase_cmd,
    rom_void_fn connect, rom_void_fn exit_xip,
    rom_flash_range_erase_fn erase,
    rom_void_fn flush, rom_void_fn enter_xip) {

  connect();
  exit_xip();
  erase(offset, erase_size, erase_size, erase_cmd);
  flush();
  enter_xip();
}
/** @} */

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level Embedded Flash driver initialization.
 *
 * @notapi
 */
void efl_lld_init(void) {

  /* Driver initialization. */
  eflObjectInit(&EFLD1);
}

/**
 * @brief   Configures and activates the Embedded Flash peripheral.
 *
 * @param[in] eflp      pointer to a @p EFlashDriver structure
 *
 * @notapi
 */
void efl_lld_start(EFlashDriver *eflp) {

  (void)eflp;

  /* Nothing to do - flash is always accessible via XIP. */
}

/**
 * @brief   Deactivates the Embedded Flash peripheral.
 *
 * @param[in] eflp      pointer to a @p EFlashDriver structure
 *
 * @notapi
 */
void efl_lld_stop(EFlashDriver *eflp) {

  (void)eflp;

  /* Nothing to do. */
}

/**
 * @brief   Gets the flash descriptor structure.
 *
 * @param[in] instance  pointer to a @p EFlashDriver instance
 * @return              A flash device descriptor.
 *
 * @notapi
 */
const flash_descriptor_t *efl_lld_get_descriptor(void *instance) {

  (void)instance;

  return &efl_lld_descriptor;
}

/**
 * @brief   Read operation.
 *
 * @param[in] instance  pointer to a @p EFlashDriver instance
 * @param[in] offset    offset within full flash address space
 * @param[in] n         number of bytes to be read
 * @param[out] rp       pointer to the data buffer
 * @return              An error code.
 * @retval FLASH_NO_ERROR           if there is no erase operation in progress.
 * @retval FLASH_BUSY_ERASING       if there is an erase operation in progress.
 * @retval FLASH_ERROR_READ         if the read operation failed.
 * @retval FLASH_ERROR_HW_FAILURE   if access to the memory failed.
 *
 * @notapi
 */
flash_error_t efl_lld_read(void *instance, flash_offset_t offset,
                           size_t n, uint8_t *rp) {
  EFlashDriver *devp = (EFlashDriver *)instance;
  flash_error_t err = FLASH_NO_ERROR;

  osalDbgCheck((instance != NULL) && (rp != NULL) && (n > 0U));
  osalDbgCheck((size_t)offset + n <= (size_t)efl_lld_descriptor.size);
  osalDbgAssert((devp->state == FLASH_READY) || (devp->state == FLASH_ERASE),
                "invalid state");

  /* No reading while erasing. */
  if (devp->state == FLASH_ERASE) {
    return FLASH_BUSY_ERASING;
  }

  /* FLASH_READ state while the operation is performed. */
  devp->state = FLASH_READ;

  /* Read from memory-mapped XIP region. */
  memcpy((void *)rp, (const void *)(efl_lld_descriptor.address + offset), n);

  /* Ready state again. */
  devp->state = FLASH_READY;

  return err;
}

/**
 * @brief   Program operation.
 *
 * @param[in] instance  pointer to a @p EFlashDriver instance
 * @param[in] offset    offset within full flash address space
 * @param[in] n         number of bytes to be programmed
 * @param[in] pp        pointer to the data buffer
 * @return              An error code.
 * @retval FLASH_NO_ERROR           if there is no erase operation in progress.
 * @retval FLASH_BUSY_ERASING       if there is an erase operation in progress.
 * @retval FLASH_ERROR_PROGRAM      if the program operation failed.
 * @retval FLASH_ERROR_HW_FAILURE   if access to the memory failed.
 *
 * @notapi
 */
flash_error_t efl_lld_program(void *instance, flash_offset_t offset,
                              size_t n, const uint8_t *pp) {
  EFlashDriver *devp = (EFlashDriver *)instance;
  syssts_t sts;

  osalDbgCheck((instance != NULL) && (pp != NULL) && (n > 0U));
  osalDbgCheck((size_t)offset + n <= (size_t)efl_lld_descriptor.size);
  osalDbgAssert((devp->state == FLASH_READY) || (devp->state == FLASH_ERASE),
                "invalid state");

  /* No programming while erasing. */
  if (devp->state == FLASH_ERASE) {
    return FLASH_BUSY_ERASING;
  }

  /* FLASH_PGM state while the operation is performed. */
  devp->state = FLASH_PGM;

  /* Resolve ROM function pointers (lazy init, first call only). */
  rom_flash_lookup();

  /* Program in page-sized chunks using RP2040 ROM functions.
     The ROM functions live in boot ROM (not flash) and handle all
     SPI details correctly.  For each page we:
     1. Build a full 256-byte page buffer in RAM (pad with 0xFF)
     2. Lock IRQs
     3. Call ROM: connect → exit_xip → range_program → flush → enter_xip
     4. Unlock IRQs
     Padding with 0xFF is safe for NOR flash (all-ones = no change).
     The deployment region is erased before writing, so the padded
     bytes are already 0xFF. */
  while (n > 0U) {
    uint8_t page_buf[RP_FLASH_PAGE_SIZE];
    uint32_t page_base = offset & ~(uint32_t)RP_FLASH_PAGE_MASK;
    size_t page_offset = offset & RP_FLASH_PAGE_MASK;
    size_t page_remaining = RP_FLASH_PAGE_SIZE - page_offset;
    size_t chunk = (n < page_remaining) ? n : page_remaining;

    /* Fill page buffer: 0xFF padding + actual data + 0xFF padding.
       Source data is copied while XIP is still active (pp may be
       in XIP address space). */
    memset(page_buf, 0xFF, RP_FLASH_PAGE_SIZE);
    memcpy(page_buf + page_offset, pp, chunk);

    /* Lock IRQs for the entire ROM flash operation.  The ROM's
       exit_xip disables XIP so no flash code can execute.  The
       ROM's range_program busy-waits internally (~830 us/page). */
    sts = osalSysGetStatusAndLockX();

    rp_flash_rom_program_page(page_base, page_buf,
                              rom_connect_internal_flash,
                              rom_flash_exit_xip,
                              rom_flash_range_program,
                              rom_flash_flush_cache,
                              rom_flash_enter_cmd_xip);

    osalSysRestoreStatusX(sts);

    offset += chunk;
    pp += chunk;
    n -= chunk;
  }

  /* Ready state again. */
  devp->state = FLASH_READY;

  return FLASH_NO_ERROR;
}

/**
 * @brief   Starts a whole-device erase operation.
 * @note    This is not implemented for safety reasons - erasing the entire
 *          flash would destroy the running firmware.
 *
 * @param[in] instance  pointer to a @p EFlashDriver instance
 * @return              An error code.
 *
 * @notapi
 */
flash_error_t efl_lld_start_erase_all(void *instance) {

  (void)instance;

  return FLASH_ERROR_UNIMPLEMENTED;
}

/**
 * @brief   Starts a sector erase operation.
 *
 * @param[in] instance  pointer to a @p EFlashDriver instance
 * @param[in] sector    sector to be erased
 * @return              An error code.
 * @retval FLASH_NO_ERROR           if there is no erase operation in progress.
 * @retval FLASH_BUSY_ERASING       if there is an erase operation in progress.
 * @retval FLASH_ERROR_HW_FAILURE   if access to the memory failed.
 *
 * @notapi
 */
flash_error_t efl_lld_start_erase_sector(void *instance,
                                         flash_sector_t sector) {
  EFlashDriver *devp = (EFlashDriver *)instance;
  flash_offset_t offset;
  syssts_t sts;

  osalDbgCheck(instance != NULL);
  osalDbgCheck(sector < efl_lld_descriptor.sectors_count);
  osalDbgAssert((devp->state == FLASH_READY) || (devp->state == FLASH_ERASE),
                "invalid state");

  /* No erasing while erasing. */
  if (devp->state == FLASH_ERASE) {
    return FLASH_BUSY_ERASING;
  }

  /* Resolve ROM function pointers (lazy init, first call only). */
  rom_flash_lookup();

  /* Calculate sector offset. */
  offset = sector * RP_FLASH_SECTOR_SIZE;

  /* Erase synchronously using ROM functions.  The ROM handles
     write-enable, erase command, and busy-wait internally.
     IRQs are locked for the entire ~50-100 ms erase time because
     XIP is disabled and flash-resident code cannot execute. */
  sts = osalSysGetStatusAndLockX();

  rp_flash_rom_erase(offset, RP_FLASH_SECTOR_SIZE, FLASHCMD_SECTOR_ERASE,
                     rom_connect_internal_flash,
                     rom_flash_exit_xip,
                     rom_flash_range_erase,
                     rom_flash_flush_cache,
                     rom_flash_enter_cmd_xip);

  osalSysRestoreStatusX(sts);

  /* Erase is already complete — go directly to READY state.
     query_erase will return FLASH_NO_ERROR immediately. */
  devp->state = FLASH_READY;

  return FLASH_NO_ERROR;
}

/**
 * @brief   Starts a block erase operation.
 *
 * @param[in] instance    pointer to a @p EFlashDriver instance
 * @param[in] cmd         JEDEC erase command byte
 * @param[in] erase_size  erase unit size in bytes
 * @param[in] block       block number to be erased
 * @return                An error code.
 * @retval FLASH_NO_ERROR           if the block erase completed.
 * @retval FLASH_BUSY_ERASING       if there is an erase operation in progress.
 *
 * @notapi
 */
flash_error_t efl_lld_start_erase_block(void *instance,
                                        uint8_t cmd,
                                        uint32_t erase_size,
                                        uint32_t block) {
  EFlashDriver *devp = (EFlashDriver *)instance;
  flash_offset_t offset;
  syssts_t sts;

  osalDbgCheck(instance != NULL);
  osalDbgCheck(block < (RP_FLASH_SIZE / erase_size));
  osalDbgAssert((devp->state == FLASH_READY) || (devp->state == FLASH_ERASE),
                "invalid state");

  if (devp->state == FLASH_ERASE) {
    return FLASH_BUSY_ERASING;
  }

  /* Resolve ROM function pointers (lazy init, first call only). */
  rom_flash_lookup();

  offset = block * erase_size;

  /* Erase synchronously using ROM functions. */
  sts = osalSysGetStatusAndLockX();

  rp_flash_rom_erase(offset, erase_size, cmd,
                     rom_connect_internal_flash,
                     rom_flash_exit_xip,
                     rom_flash_range_erase,
                     rom_flash_flush_cache,
                     rom_flash_enter_cmd_xip);

  osalSysRestoreStatusX(sts);

  devp->state = FLASH_READY;

  return FLASH_NO_ERROR;
}

/**
 * @brief   Queries the driver for erase operation progress.
 *
 * @param[in] instance  pointer to a @p EFlashDriver instance
 * @param[out] msec     recommended time, in milliseconds, that
 *                      should be spent before calling this
 *                      function again, can be @p NULL
 * @return              An error code.
 * @retval FLASH_NO_ERROR           if there is no erase operation in progress.
 * @retval FLASH_BUSY_ERASING       if there is an erase operation in progress.
 * @retval FLASH_ERROR_ERASE        if the erase operation failed.
 * @retval FLASH_ERROR_HW_FAILURE   if access to the memory failed.
 *
 * @notapi
 */
flash_error_t efl_lld_query_erase(void *instance, uint32_t *msec) {
  EFlashDriver *devp = (EFlashDriver *)instance;

  (void)devp;

  /* Erase is performed synchronously by efl_lld_start_erase_sector
     using ROM functions, so there is never an erase in progress
     when this function is called. */
  if (msec != NULL) {
    *msec = 0U;
  }

  return FLASH_NO_ERROR;
}

/**
 * @brief   Returns the erase state of a sector.
 *
 * @param[in] instance  pointer to a @p EFlashDriver instance
 * @param[in] sector    sector to be verified
 * @return              An error code.
 * @retval FLASH_NO_ERROR           if the sector is erased.
 * @retval FLASH_BUSY_ERASING       if there is an erase operation in progress.
 * @retval FLASH_ERROR_VERIFY       if the verify operation failed.
 * @retval FLASH_ERROR_HW_FAILURE   if access to the memory failed.
 *
 * @notapi
 */
flash_error_t efl_lld_verify_erase(void *instance, flash_sector_t sector) {
  EFlashDriver *devp = (EFlashDriver *)instance;
  const uint32_t *address;
  flash_error_t err = FLASH_NO_ERROR;
  unsigned i;

  osalDbgCheck(instance != NULL);
  osalDbgCheck(sector < efl_lld_descriptor.sectors_count);
  osalDbgAssert((devp->state == FLASH_READY) || (devp->state == FLASH_ERASE),
                "invalid state");

  /* No verifying while erasing. */
  if (devp->state == FLASH_ERASE) {
    return FLASH_BUSY_ERASING;
  }

  /* Address of the sector in XIP space. */
  address = (const uint32_t *)(efl_lld_descriptor.address +
                               flashGetSectorOffset(getBaseFlash(devp), sector));

  /* FLASH_READ state while the operation is performed. */
  devp->state = FLASH_READ;

  /* Scanning the sector space. */
  for (i = 0U; i < RP_FLASH_SECTOR_SIZE / sizeof(uint32_t); i++) {
    if (address[i] != 0xFFFFFFFFU) {
      err = FLASH_ERROR_VERIFY;
      break;
    }
  }

  /* Ready state again. */
  devp->state = FLASH_READY;

  return err;
}

/**
 * @brief   Reads the flash chip's unique ID.
 * @note    The JEDEC 0x4B command requires 4 dummy bytes before the
 *          8-byte unique ID. The memcpy runs after XIP is restored
 *          so it is safe to call flash-resident libc.
 *
 * @param[in] eflp      pointer to a @p EFlashDriver structure
 * @param[out] uid      pointer to an 8-byte buffer for the unique ID
 *
 * @api
 */
void efl_lld_read_unique_id(EFlashDriver *eflp, uint8_t *uid) {
  uint8_t rx[4U + RP_FLASH_UNIQUE_ID_SIZE];

  osalDbgCheck((eflp != NULL) && (uid != NULL));

  osalSysLock();
  rp_flash_read_uid_full(eflp, rx, sizeof(rx));
  osalSysUnlock();

  memcpy(uid, rx + 4U, RP_FLASH_UNIQUE_ID_SIZE);
}

/**
 * @brief   Starts a single-page program operation (non-blocking).
 * @note    This is the program counterpart of @p efl_lld_start_erase_sector().
 *          It sends the page-program command and returns immediately.
 *          The caller must poll @p efl_lld_query_program() for completion.
 *          Only ONE page (up to 256 bytes, must not cross a page boundary)
 *          may be programmed per start/query cycle.
 *
 * @param[in] instance  pointer to a @p EFlashDriver instance
 * @param[in] offset    flash offset (must not cross page boundary)
 * @param[in] n         number of bytes to program (<= page size)
 * @param[in] pp        pointer to the data buffer (may be in flash/XIP)
 * @return              An error code.
 * @retval FLASH_NO_ERROR           if the command was sent successfully.
 * @retval FLASH_BUSY_ERASING       if there is an erase operation in progress.
 *
 * @notapi
 */
flash_error_t efl_lld_start_program_page(void *instance,
                                         flash_offset_t offset,
                                         size_t n, const uint8_t *pp) {
  EFlashDriver *devp = (EFlashDriver *)instance;
  uint8_t page_buf[RP_FLASH_PAGE_SIZE];
  syssts_t sts;

  osalDbgCheck((instance != NULL) && (pp != NULL) && (n > 0U));
  osalDbgCheck(n <= RP_FLASH_PAGE_SIZE);
  osalDbgCheck((size_t)offset + n <= (size_t)efl_lld_descriptor.size);
  osalDbgAssert((devp->state == FLASH_READY) || (devp->state == FLASH_ERASE),
                "invalid state");

  /* No programming while erasing. */
  if (devp->state == FLASH_ERASE) {
    return FLASH_BUSY_ERASING;
  }

  /* FLASH_PGM state while the operation is in progress. */
  devp->state = FLASH_PGM;

  /* Copy source data to RAM while XIP is still active. */
  memcpy(page_buf, pp, n);

  /* Lock while XIP is disabled for the SPI page-program transfer. */
  sts = osalSysGetStatusAndLockX();
  rp_flash_start_program_page(devp, offset, page_buf, n);
  osalSysRestoreStatusX(sts);

  return FLASH_NO_ERROR;
}

/**
 * @brief   Queries the driver for program operation progress.
 * @note    This is the program counterpart of @p efl_lld_query_erase().
 *          When programming is complete the XIP cache is flushed and
 *          the driver returns to FLASH_READY state.
 *
 * @param[in] instance  pointer to a @p EFlashDriver instance
 * @param[out] msec     recommended time, in milliseconds, that
 *                      should be spent before calling this
 *                      function again, can be @p NULL
 * @return              An error code.
 * @retval FLASH_NO_ERROR           if there is no program operation in progress.
 * @retval FLASH_BUSY_ERASING       if there is a program operation in progress.
 *
 * @notapi
 */
flash_error_t efl_lld_query_program(void *instance, uint32_t *msec) {
  EFlashDriver *devp = (EFlashDriver *)instance;
  syssts_t sts;
  bool busy;

  /* If there is a program in progress then the device must be checked. */
  if (devp->state == FLASH_PGM) {

    /* Lock while XIP is briefly disabled for flash status read. */
    sts = osalSysGetStatusAndLockX();
    busy = rp_flash_is_busy(devp);
    if (!busy) {
      rp_flash_flush_cache();
    }
    osalSysRestoreStatusX(sts);

    if (busy) {
      if (msec != NULL) {
        *msec = RP_FLASH_WAIT_TIME_MS;
      }
      return FLASH_BUSY_ERASING;
    }

    /* Program complete. */
    devp->state = FLASH_READY;
  }

  return FLASH_NO_ERROR;
}

#pragma GCC diagnostic pop

#endif /* HAL_USE_EFL == TRUE */

/** @} */
