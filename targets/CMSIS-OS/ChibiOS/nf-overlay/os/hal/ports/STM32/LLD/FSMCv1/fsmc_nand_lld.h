//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2014 Uladzimir Pylinsky aka barthess
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_NAND_LLD_H_
#define HAL_NAND_LLD_H_

#include "hal_stm32_fsmc"
#include <hal_nf_community.h>

#if (HAL_USE_NAND == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver constants.                                                         //
///////////////////////////////////////////////////////////////////////////////
#define NAND_MIN_PAGE_SIZE       256
#define NAND_MAX_PAGE_SIZE       8192

///////////////////////////////////////////////////////////////////////////////
// Driver pre-compile time settings.                                         //
///////////////////////////////////////////////////////////////////////////////

//  Configuration options

// FSMC1 interrupt priority level setting.
#if !defined(STM32_EMC_FSMC1_IRQ_PRIORITY)
#define STM32_EMC_FSMC1_IRQ_PRIORITY      10
#endif

//  NAND driver enable switch.
// If set to @p TRUE the support for NAND1 is included.

#if !defined(STM32_NAND_USE_NAND1)
#define STM32_NAND_USE_NAND1              FALSE
#endif

// NAND driver enable switch.
// If set to @p TRUE the support for NAND2 is included.
#if !defined(STM32_NAND_USE_NAND2)
#define STM32_NAND_USE_NAND2              FALSE
#endif

// NAND DMA error hook.
// The default action for DMA errors is a system halt because DMA
// error can only happen because programming errors.
#if !defined(STM32_NAND_DMA_ERROR_HOOK)
#define STM32_NAND_DMA_ERROR_HOOK(nandp)  osalSysHalt("DMA failure")
#endif

// NAND interrupt enable switch.
// If set to @p TRUE the support for internal FSMC interrupt included.
#if !defined(STM32_NAND_USE_INT)
#define STM32_NAND_USE_INT                FALSE
#endif

// NAND1 DMA priority (0..3|lowest..highest).
#if !defined(STM32_NAND_NAND1_DMA_PRIORITY)
#define STM32_NAND_NAND1_DMA_PRIORITY     0
#endif

// NAND2 DMA priority (0..3|lowest..highest).
#if !defined(STM32_NAND_NAND2_DMA_PRIORITY)
#define STM32_NAND_NAND2_DMA_PRIORITY     0
#endif

// DMA stream used for NAND operations.
// This option is only available on platforms with enhanced DMA.
#if !defined(STM32_NAND_DMA_STREAM)
#define STM32_NAND_DMA_STREAM             STM32_DMA_STREAM_ID(2, 6)
#endif

///////////////////////////////////////////////////////////////////////////////
// Derived constants and error checks.                                       //
///////////////////////////////////////////////////////////////////////////////

#if !STM32_NAND_USE_FSMC_NAND1 && !STM32_NAND_USE_FSMC_NAND2
#error "NAND driver activated but no NAND peripheral assigned"
#endif

#if (STM32_NAND_USE_FSMC_NAND2 || STM32_NAND_USE_FSMC_NAND1) && !STM32_HAS_FSMC
#error "FSMC not present in the selected device"
#endif

#if !defined(STM32_DMA_REQUIRED)
#define STM32_DMA_REQUIRED
#endif

///////////////////////////////////////////////////////////////////////////////
// Driver data structures and types.                                         //
///////////////////////////////////////////////////////////////////////////////

// Type of a structure representing an NAND driver.
typedef struct NANDDriver NANDDriver;

// Type of interrupt handler function.
typedef void (*nandisrhandler_t)(NANDDriver *nandp);

// Driver configuration structure.
// It could be empty on some architectures.
typedef struct {
  // Number of erase blocks in NAND device.
  uint32_t                  blocks;

  //  Number of data bytes in page.
  uint32_t                  page_data_size;

  // Number of spare bytes in page.
  uint32_t                  page_spare_size;

  // Number of pages in block.
  uint32_t                  pages_per_block;
  
  // Number of write cycles for row addressing.
  uint8_t                   rowcycles;
  
  //  Number of write cycles for column addressing.
  uint8_t                   colcycles;

  // End of the mandatory fields.
  //  Number of wait cycles. This value will be used both for
  //  PMEM and PATTR registers
  // For proper calculation procedure please look at AN2784 document from STMicroelectronics
  uint32_t                  pmem;

} NANDConfig;

// Structure representing an NAND driver.
struct NANDDriver {
  //  Driver state.
  nandstate_t               state;
  
  // Current configuration data.
  const NANDConfig          *config;
  
  // Array to store bad block map.
#if NAND_USE_MUTUAL_EXCLUSION
#if CH_CFG_USE_MUTEXES
  // Mutex protecting the bus.
  mutex_t                   mutex;
#elif CH_CFG_USE_SEMAPHORES
  semaphore_t               semaphore;
#endif
#endif // NAND_USE_MUTUAL_EXCLUSION 
  // End of the mandatory fields.
  
  // Function enabling interrupts from FSMC.
  nandisrhandler_t          isr_handler;
  
  //  Pointer to current transaction buffer.
  void                      *rxdata;
  
  // Current transaction length in bytes.
  size_t                    datalen;
  
  // DMA mode bit mask.
  uint32_t                  dmamode;
  
  //  DMA channel.
  const stm32_dma_stream_t  *dma;
 
  // Thread waiting for I/O completion.
  thread_t                  *thread;
  
  // Pointer to the FSMC NAND registers block.
  FSMC_NAND_TypeDef         *nand;
  
  // Memory mapping for data.
  uint16_t                  *map_data;
  
  //  Memory mapping for commands.
  uint16_t                  *map_cmd;
  
  // Memory mapping for addresses.
  uint16_t                  *map_addr;

};

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// External declarations.                                                    //
///////////////////////////////////////////////////////////////////////////////

#if STM32_NAND_USE_FSMC_NAND1
extern NANDDriver NANDD1;
#endif

#if STM32_NAND_USE_FSMC_NAND2
extern NANDDriver NANDD2;
#endif

#ifdef __cplusplus
extern "C" {
#endif

  void fsmcNandInit(void);
  void fsmcNandStart(NANDDriver *nandp);
  void fsmcNandStop(NANDDriver *nandp);
  uint8_t nand_lld_erase(NANDDriver *nandp, uint8_t *addr, size_t addrlen);
  void nand_lld_read_data(NANDDriver *nandp, uint16_t *data,
                size_t datalen, uint8_t *addr, size_t addrlen, uint32_t *ecc);
  void nand_lld_write_addr(NANDDriver *nandp, const uint8_t *addr, size_t len);
  void nand_lld_write_cmd(NANDDriver *nandp, uint8_t cmd);
  uint8_t nand_lld_write_data(NANDDriver *nandp, const uint16_t *data,
                size_t datalen, uint8_t *addr, size_t addrlen, uint32_t *ecc);
  uint8_t nand_lld_read_status(NANDDriver *nandp);
  void nand_lld_reset(NANDDriver *nandp);

#ifdef __cplusplus
}
#endif

#endif // HAL_USE_NAND 

#endif // HAL_NAND_LLD_H_ 
