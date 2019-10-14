//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2014 Uladzimir Pylinsky aka barthess
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_USE_NAND == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver local definitions.                                                 //
///////////////////////////////////////////////////////////////////////////////
#define NAND_DMA_CHANNEL                                                  \
  STM32_DMA_GETCHANNEL(STM32_NAND_DMA_STREAM,                             \
                       STM32_FSMC_DMA_CHN)

//  Bus width of NAND IC.
// Must be 8 or 16
#if ! defined(STM32_NAND_BUS_WIDTH)
#define STM32_NAND_BUS_WIDTH        8
#endif

//  DMA transaction width on AHB bus in bytes
#define AHB_TRANSACTION_WIDTH       2

///////////////////////////////////////////////////////////////////////////////
// Driver exported variables.                                                //
///////////////////////////////////////////////////////////////////////////////

// NAND1 driver identifier.
#if STM32_NAND_USE_FSMC_NAND1
NANDDriver NANDD1;
#endif

//  NAND2 driver identifier.
#if STM32_NAND_USE_FSMC_NAND2
NANDDriver NANDD2;
#endif

///////////////////////////////////////////////////////////////////////////////
// Driver local variables and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////

// Helper function.
static void align_check(const void *ptr, uint32_t len) {
  osalDbgCheck((((uint32_t)ptr % AHB_TRANSACTION_WIDTH) == 0) &&
                         ((len % AHB_TRANSACTION_WIDTH) == 0) &&
                         (len >= AHB_TRANSACTION_WIDTH));
  (void)ptr;
  (void)len;
}

//  Work around errata in STM32's FSMC core.
//  Constant output clock (if enabled) disappears when CLKDIV value
//   sets to 1 (FMC_CLK period = 2 × HCLK periods) AND 8-bit async
//   transaction generated on AHB. This workaround eliminates 8-bit
//   transactions on bus when you use 8-bit memory. It suitable only
//   for 8-bit memory (i.e. PWID bits in PCR register must be set
//   to 8-bit mode).
static void set_16bit_bus(NANDDriver *nandp) {
#if STM32_NAND_BUS_WIDTH
  nandp->nand->PCR |= FSMC_PCR_PWID_16;
#else
  (void)nandp;
#endif
}

static void set_8bit_bus(NANDDriver *nandp) {
#if STM32_NAND_BUS_WIDTH
  nandp->nand->PCR &= ~FSMC_PCR_PWID_16;
#else
  (void)nandp;
#endif
}

// Wakes up the waiting thread.
//  nandp  pointer to the @p NANDDriver object
//  msg       wakeup message
static void wakeup_isr(NANDDriver *nandp) {

  osalDbgCheck(nandp->thread != NULL);
  osalThreadResumeI(&nandp->thread, MSG_OK);
}

//  Put calling thread in suspend and switch driver state
// nandp    pointer to the @p NANDDriver object
static void nand_lld_suspend_thread(NANDDriver *nandp) {

  osalThreadSuspendS(&nandp->thread);
}

//  Caclulate ECCPS register value
//  nandp    pointer to the @p NANDDriver object
static uint32_t calc_eccps(NANDDriver *nandp) {

  uint32_t i = 0;
  uint32_t eccps = nandp->config->page_data_size;

  eccps = eccps >> 9;
  while (eccps > 0){
    i++;
    eccps >>= 1;
  }

  return i << 17;
}

///////////////////////////////////////////////////////////////////////////////
// Driver local variables and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver interrupt handlers.                                                //
///////////////////////////////////////////////////////////////////////////////

//  Enable interrupts from NAND
// nandp    pointer to the @p NANDDriver object
static void nand_ready_isr_enable(NANDDriver *nandp) {

  nandp->nand->SR &= ~(FSMC_SR_IRS | FSMC_SR_ILS | FSMC_SR_IFS |
                                    FSMC_SR_ILEN | FSMC_SR_IFEN);
  nandp->nand->SR |= FSMC_SR_IREN;
}

//  Disable interrupts from NAND
// nandp    pointer to the @p NANDDriver object
static void nand_ready_isr_disable(NANDDriver *nandp) {

  nandp->nand->SR &= ~FSMC_SR_IREN;
}

//  Ready interrupt handler
// nandp    pointer to the @p NANDDriver object
static void nand_isr_handler(NANDDriver *nandp) {

  osalSysLockFromISR();

  osalDbgCheck(nandp->nand->SR & FSMC_SR_IRS); // spurious interrupt happened
  nandp->nand->SR &= ~FSMC_SR_IRS;

  switch (nandp->state){
  case NAND_READ:
    nandp->state = NAND_DMA_RX;
    dmaStartMemCopy(nandp->dma, nandp->dmamode, nandp->map_data, nandp->rxdata,
                    nandp->datalen/AHB_TRANSACTION_WIDTH);
    // thread will be waked up from DMA ISR
    break;

  case NAND_ERASE:      // NAND reports about erase finish
  case NAND_PROGRAM:    // NAND reports about page programming finish
  case NAND_RESET:      // NAND reports about finished reset recover
    nandp->state = NAND_READY;
    wakeup_isr(nandp);
    break;

  default:
    osalSysHalt("Unhandled case");
    break;
  }
  osalSysUnlockFromISR();
}

//  DMA RX end IRQ handler.
// nandp    pointer to the @p NANDDriver object
// flags    pre-shifted content of the ISR register
static void nand_lld_serve_transfer_end_irq(NANDDriver *nandp, uint32_t flags) {
  // DMA errors handling.
#if defined(STM32_NAND_DMA_ERROR_HOOK)
  if ((flags & (STM32_DMA_ISR_TEIF | STM32_DMA_ISR_DMEIF)) != 0) {
    STM32_NAND_DMA_ERROR_HOOK(nandp);
  }
#else
  (void)flags;
#endif

  osalSysLockFromISR();

  dmaStreamDisable(nandp->dma);

  switch (nandp->state){
  case NAND_DMA_TX:
    nandp->state = NAND_PROGRAM;
    nandp->map_cmd[0] = NAND_CMD_PAGEPROG;
    // thread will be woken up from ready_isr()
    break;

  case NAND_DMA_RX:
    nandp->state = NAND_READY;
    nandp->rxdata = NULL;
    nandp->datalen = 0;
    wakeup_isr(nandp);
    break;

  default:
    osalSysHalt("Unhandled case");
    break;
  }

  osalSysUnlockFromISR();
}

///////////////////////////////////////////////////////////////////////////////
// Driver exported functions.                                                //
///////////////////////////////////////////////////////////////////////////////

//  Low level NAND driver initialization.
void fsmcNandInit(void) {

  stm32FsmcInit();

#if STM32_NAND_USE_FSMC_NAND1
  // Driver initialization.
  nandObjectInit(&NANDD1);
  NANDD1.rxdata   = NULL;
  NANDD1.datalen  = 0;
  NANDD1.thread   = NULL;
  NANDD1.dma      = STM32_DMA_STREAM(STM32_NAND_DMA_STREAM);
  NANDD1.nand     = FSMCD1.nand1;
  NANDD1.map_data = (void *)FSMC_Bank2_MAP_COMMON_DATA;
  NANDD1.map_cmd  = (uint16_t *)FSMC_Bank2_MAP_COMMON_CMD;
  NANDD1.map_addr = (uint16_t *)FSMC_Bank2_MAP_COMMON_ADDR;
  NANDD1.bb_map   = NULL;
#endif // STM32_NAND_USE_FSMC_NAND1

#if STM32_NAND_USE_FSMC_NAND2
  // Driver initialization.
  nandObjectInit(&NANDD2);
  NANDD2.rxdata   = NULL;
  NANDD2.datalen  = 0;
  NANDD2.thread   = NULL;
  NANDD2.dma      = STM32_DMA_STREAM(STM32_NAND_DMA_STREAM);
  NANDD2.nand     = FSMCD1.nand2;
  NANDD2.map_data = (void *)FSMC_Bank3_MAP_COMMON_DATA;
  NANDD2.map_cmd  = (uint16_t *)FSMC_Bank3_MAP_COMMON_CMD;
  NANDD2.map_addr = (uint16_t *)FSMC_Bank3_MAP_COMMON_ADDR;
  NANDD2.bb_map   = NULL;
#endif // STM32_NAND_USE_FSMC_NAND2
}

//  Configures and activates the NAND peripheral.
// nandp         pointer to the @p NANDDriver object
void fsmcNandStart(NANDDriver *nandp) {

  bool b;
  uint32_t dmasize;
  uint32_t pcr_bus_width;

  if (FSMCD1.state == FSMC_STOP)
    fsmc_start(&FSMCD1);

  if (nandp->state == NAND_STOP) {
    b = dmaStreamAllocate(nandp->dma,
                          STM32_EMC_FSMC1_IRQ_PRIORITY,
                          (stm32_dmaisr_t)nand_lld_serve_transfer_end_irq,
                          (void *)nandp);
    osalDbgAssert(!b, "stream already allocated");

#if AHB_TRANSACTION_WIDTH == 4
    dmasize = STM32_DMA_CR_PSIZE_WORD | STM32_DMA_CR_MSIZE_WORD;
#elif AHB_TRANSACTION_WIDTH == 2
    dmasize = STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD;
#elif AHB_TRANSACTION_WIDTH == 1
    dmasize = STM32_DMA_CR_PSIZE_BYTE | STM32_DMA_CR_MSIZE_BYTE;
#else
#error "Incorrect AHB_TRANSACTION_WIDTH"
#endif

    nandp->dmamode = STM32_DMA_CR_CHSEL(NAND_DMA_CHANNEL) |
                     STM32_DMA_CR_PL(STM32_NAND_NAND1_DMA_PRIORITY) |
                     dmasize |
                     STM32_DMA_CR_DMEIE |
                     STM32_DMA_CR_TEIE |
                     STM32_DMA_CR_TCIE;

#if STM32_NAND_BUS_WIDTH == 8
    pcr_bus_width = FSMC_PCR_PWID_8;
#elif STM32_NAND_BUS_WIDTH == 16
    pcr_bus_width = FSMC_PCR_PWID_16;
#else
#error "Bus width must be 8 or 16 bits"
#endif
    nandp->nand->PCR = pcr_bus_width | calc_eccps(nandp) |
                       FSMC_PCR_PTYP_NAND | FSMC_PCR_PBKEN;
    nandp->nand->PMEM = nandp->config->pmem;
    nandp->nand->PATT = nandp->config->pmem;
    nandp->isr_handler = nand_isr_handler;
    nand_ready_isr_enable(nandp);
  }
}

//   Deactivates the NAND peripheral.
// nandp         pointer to the @p NANDDriver object
void fsmcNandStop(NANDDriver *nandp) {

  if (nandp->state == NAND_READY) {
    dmaStreamRelease(nandp->dma);
    nandp->nand->PCR &= ~FSMC_PCR_PBKEN;
    nand_ready_isr_disable(nandp);
    nandp->isr_handler = NULL;
  }
}

//   Read data from NAND.
// nandp         pointer to the @p NANDDriver object
// data         pointer to data buffer
// datalen       size of data buffer in bytes
// addr          pointer to address buffer
// addrlen       length of address
// ecc          pointer to store computed ECC. Ignored when NULL.
void nand_lld_read_data(NANDDriver *nandp, uint16_t *data, size_t datalen,
                        uint8_t *addr, size_t addrlen, uint32_t *ecc){

  align_check(data, datalen);

  nandp->state = NAND_READ;
  nandp->rxdata = data;
  nandp->datalen = datalen;

  set_16bit_bus(nandp);
  nand_lld_write_cmd(nandp, NAND_CMD_READ0);
  nand_lld_write_addr(nandp, addr, addrlen);
  osalSysLock();
  nand_lld_write_cmd(nandp, NAND_CMD_READ0_CONFIRM);
  set_8bit_bus(nandp);

  // Here NAND asserts busy signal and starts transferring from memory
  //   array to page buffer. After the end of transmission ready_isr functions
  //   starts DMA transfer from page buffer to MCU's RAM.
  osalDbgAssert((nandp->nand->PCR & FSMC_PCR_ECCEN) == 0,
          "State machine broken. ECCEN must be previously disabled.");

  if (NULL != ecc){
    nandp->nand->PCR |= FSMC_PCR_ECCEN;
  }

  nand_lld_suspend_thread(nandp);
  osalSysUnlock();

  // thread was woken up from DMA ISR
  if (NULL != ecc){
    while (! (nandp->nand->SR & FSMC_SR_FEMPT))
      ;
    *ecc = nandp->nand->ECCR;
    nandp->nand->PCR &= ~FSMC_PCR_ECCEN;
  }
}

//   Write data to NAND.
//  nandp         pointer to the @p NANDDriver object
// data          buffer with data to be written
// datalen       size of data buffer in bytes
// addr          pointer to address buffer
// addrlen       length of address
// ecc          pointer to store computed ECC. Ignored when NULL.
//   The operation status reported by NAND IC (0x70 command).
uint8_t nand_lld_write_data(NANDDriver *nandp, const uint16_t *data,
                size_t datalen, uint8_t *addr, size_t addrlen, uint32_t *ecc) {

  align_check(data, datalen);

  nandp->state = NAND_WRITE;

  set_16bit_bus(nandp);
  nand_lld_write_cmd(nandp, NAND_CMD_WRITE);
  osalSysLock();
  nand_lld_write_addr(nandp, addr, addrlen);
  set_8bit_bus(nandp);

  // Now start DMA transfer to NAND buffer and put thread in sleep state.
  //   Tread will be woken up from ready ISR.
  nandp->state = NAND_DMA_TX;
  osalDbgAssert((nandp->nand->PCR & FSMC_PCR_ECCEN) == 0,
          "State machine broken. ECCEN must be previously disabled.");

  if (NULL != ecc){
    nandp->nand->PCR |= FSMC_PCR_ECCEN;
  }

  dmaStartMemCopy(nandp->dma, nandp->dmamode, data, nandp->map_data,
                  datalen/AHB_TRANSACTION_WIDTH);

  nand_lld_suspend_thread(nandp);
  osalSysUnlock();

  if (NULL != ecc){
    while (! (nandp->nand->SR & FSMC_SR_FEMPT))
      ;
    *ecc = nandp->nand->ECCR;
    nandp->nand->PCR &= ~FSMC_PCR_ECCEN;
  }

  return nand_lld_read_status(nandp);
}

//  Soft reset NAND device.
// nandp         pointer to the @p NANDDriver object
void nand_lld_reset(NANDDriver *nandp) {

  nandp->state = NAND_RESET;

  set_16bit_bus(nandp);
  nand_lld_write_cmd(nandp, NAND_CMD_RESET);
  set_8bit_bus(nandp);

  osalSysLock();
  nand_lld_suspend_thread(nandp);
  osalSysUnlock();
}

//   Erase block.
// nandp         pointer to the @p NANDDriver object
// addr          pointer to address buffer
// addrlen       length of address
// return   The operation status reported by NAND IC (0x70 command).
uint8_t nand_lld_erase(NANDDriver *nandp, uint8_t *addr, size_t addrlen) {

  nandp->state = NAND_ERASE;

  set_16bit_bus(nandp);
  nand_lld_write_cmd(nandp, NAND_CMD_ERASE);
  nand_lld_write_addr(nandp, addr, addrlen);
  osalSysLock();
  nand_lld_write_cmd(nandp, NAND_CMD_ERASE_CONFIRM);
  set_8bit_bus(nandp);

  nand_lld_suspend_thread(nandp);
  osalSysUnlock();

  return nand_lld_read_status(nandp);
}

//   Send addres to NAND.
// nandp         pointer to the @p NANDDriver object
// len           length of address array
// addr          pointer to address array
void nand_lld_write_addr(NANDDriver *nandp, const uint8_t *addr, size_t len) {
  size_t i = 0;

  for (i=0; i<len; i++)
    nandp->map_addr[i] = addr[i];
}

//   Send command to NAND.
// nandp         pointer to the @p NANDDriver object
// cmd           command value
void nand_lld_write_cmd(NANDDriver *nandp, uint8_t cmd) {
  nandp->map_cmd[0] = cmd;
}

//   Read status byte from NAND.
// nandp         pointer to the @p NANDDriver object
// return    Status byte.
uint8_t nand_lld_read_status(NANDDriver *nandp) {

  uint16_t status;

  set_16bit_bus(nandp);
  nand_lld_write_cmd(nandp, NAND_CMD_STATUS);
  set_8bit_bus(nandp);
  status = nandp->map_data[0];

  return status & 0xFF;
}

#endif // HAL_USE_NAND
