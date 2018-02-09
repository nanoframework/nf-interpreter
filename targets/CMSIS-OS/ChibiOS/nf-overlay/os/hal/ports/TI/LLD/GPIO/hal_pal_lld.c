//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

/**
 * @file    GPIO/hal_pal_lld.c
 * @brief   TICC3200 PAL subsystem low level driver.
 *
 * @addtogroup PAL
 * @{
 */

#include "hal.h"

#if HAL_USE_PAL || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#if TICC3200_HAS_GPIOA || defined(__DOXYGEN__)
#define GPIOA_BIT       (1 << 0)
#if TICC3200_GPIO_GPIOA_USE_AHB && defined(TM4C123x)
#define GPIOA_AHB_BIT   (1 << 0)
#else
#define GPIOA_AHB_BIT   0
#endif
#else
#define GPIOA_BIT       0
#define GPIOA_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOB || defined(__DOXYGEN__)
#define GPIOB_BIT       (1 << 1)
#if TICC3200_GPIO_GPIOB_USE_AHB && defined(TM4C123x)
#define GPIOB_AHB_BIT   (1 << 1)
#else
#define GPIOB_AHB_BIT   0
#endif
#else
#define GPIOB_BIT       0
#define GPIOB_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOC || defined(__DOXYGEN__)
#define GPIOC_BIT       (1 << 2)
#if TICC3200_GPIO_GPIOC_USE_AHB && defined(TM4C123x)
#define GPIOC_AHB_BIT   (1 << 2)
#else
#define GPIOC_AHB_BIT   0
#endif
#else
#define GPIOC_BIT       0
#define GPIOC_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOD || defined(__DOXYGEN__)
#define GPIOD_BIT       (1 << 3)
#if TICC3200_GPIO_GPIOD_USE_AHB && defined(TM4C123x)
#define GPIOD_AHB_BIT   (1 << 3)
#else
#define GPIOD_AHB_BIT   0
#endif
#else
#define GPIOD_BIT       0
#define GPIOD_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOE || defined(__DOXYGEN__)
#define GPIOE_BIT       (1 << 4)
#if TICC3200_GPIO_GPIOE_USE_AHB && defined(TM4C123x)
#define GPIOE_AHB_BIT   (1 << 4)
#else
#define GPIOE_AHB_BIT   0
#endif
#else
#define GPIOE_BIT       0
#define GPIOE_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOF || defined(__DOXYGEN__)
#define GPIOF_BIT       (1 << 5)
#if TICC3200_GPIO_GPIOF_USE_AHB && defined(TM4C123x)
#define GPIOF_AHB_BIT   (1 << 5)
#else
#define GPIOF_AHB_BIT   0
#endif
#else
#define GPIOF_BIT       0
#define GPIOF_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOG || defined(__DOXYGEN__)
#define GPIOG_BIT       (1 << 6)
#if TICC3200_GPIO_GPIOG_USE_AHB && defined(TM4C123x)
#define GPIOG_AHB_BIT   (1 << 6)
#else
#define GPIOG_AHB_BIT   0
#endif
#else
#define GPIOG_BIT       0
#define GPIOG_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOH || defined(__DOXYGEN__)
#define GPIOH_BIT       (1 << 7)
#if TICC3200_GPIO_GPIOH_USE_AHB && defined(TM4C123x)
#define GPIOH_AHB_BIT   (1 << 7)
#else
#define GPIOH_AHB_BIT   0
#endif
#else
#define GPIOH_BIT       0
#define GPIOH_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOJ || defined(__DOXYGEN__)
#define GPIOJ_BIT       (1 << 8)
#if TICC3200_GPIO_GPIOJ_USE_AHB && defined(TM4C123x)
#define GPIOJ_AHB_BIT   (1 << 8)
#else
#define GPIOJ_AHB_BIT   0
#endif
#else
#define GPIOJ_BIT       0
#define GPIOJ_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOK || defined(__DOXYGEN__)
#define GPIOK_BIT       (1 << 9)
#define GPIOK_AHB_BIT   (1 << 9)
#else
#define GPIOK_BIT       0
#define GPIOK_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOL || defined(__DOXYGEN__)
#define GPIOL_BIT       (1 << 10)
#define GPIOL_AHB_BIT   (1 << 10)
#else
#define GPIOL_BIT       0
#define GPIOL_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOM || defined(__DOXYGEN__)
#define GPIOM_BIT       (1 << 11)
#define GPIOM_AHB_BIT   (1 << 11)
#else
#define GPIOM_BIT       0
#define GPIOM_AHB_BIT   0
#endif

#if TICC3200_HAS_GPION || defined(__DOXYGEN__)
#define GPION_BIT       (1 << 12)
#define GPION_AHB_BIT   (1 << 12)
#else
#define GPION_BIT       0
#define GPION_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOP || defined(__DOXYGEN__)
#define GPIOP_BIT       (1 << 13)
#define GPIOP_AHB_BIT   (1 << 13)
#else
#define GPIOP_BIT       0
#define GPIOP_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOQ || defined(__DOXYGEN__)
#define GPIOQ_BIT       (1 << 14)
#define GPIOQ_AHB_BIT   (1 << 14)
#else
#define GPIOQ_BIT       0
#define GPIOQ_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOR || defined(__DOXYGEN__)
#define GPIOR_BIT       (1 << 15)
#define GPIOR_AHB_BIT   (1 << 15)
#else
#define GPIOR_BIT       0
#define GPIOR_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOS || defined(__DOXYGEN__)
#define GPIOS_BIT       (1 << 16)
#define GPIOS_AHB_BIT   (1 << 16)
#else
#define GPIOS_BIT       0
#define GPIOS_AHB_BIT   0
#endif

#if TICC3200_HAS_GPIOT || defined(__DOXYGEN__)
#define GPIOT_BIT       (1 << 17)
#define GPIOT_AHB_BIT   (1 << 17)
#else
#define GPIOT_BIT       0
#define GPIOT_AHB_BIT   0
#endif

#define RCGCGPIO_MASK   (GPIOA_BIT | GPIOB_BIT | GPIOC_BIT | GPIOD_BIT |      \
                         GPIOE_BIT | GPIOF_BIT | GPIOG_BIT | GPIOH_BIT |      \
                         GPIOJ_BIT | GPIOK_BIT | GPIOL_BIT | GPIOM_BIT |      \
                         GPION_BIT | GPIOP_BIT | GPIOQ_BIT | GPIOR_BIT |      \
                         GPIOS_BIT | GPIOR_BIT)

#define GPIOHBCTL_MASK  (GPIOA_AHB_BIT | GPIOB_AHB_BIT | GPIOC_AHB_BIT |      \
                         GPIOD_AHB_BIT | GPIOE_AHB_BIT | GPIOF_AHB_BIT |      \
                         GPIOG_AHB_BIT | GPIOH_AHB_BIT | GPIOJ_AHB_BIT |      \
                         GPIOK_AHB_BIT | GPIOL_AHB_BIT | GPIOM_AHB_BIT |      \
                         GPION_AHB_BIT | GPIOP_AHB_BIT | GPIOQ_AHB_BIT |      \
                         GPIOR_AHB_BIT | GPIOS_AHB_BIT | GPIOT_AHB_BIT)

#define GPIOC_JTAG_MASK     (0x0F)
#define GPIOD_NMI_MASK      (0x80)
#define GPIOF_NMI_MASK      (0x01)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Initializes the port with the port configuration.
 *
 * @param[in] port      the port identifier
 * @param[in] config    the port configuration
 */
static void gpio_init(ioportid_t port, const tiva_gpio_setup_t *config)
{
  HWREG(port + GPIO_O_DATA)  = config->data;
  HWREG(port + GPIO_O_DIR)   = config->dir;
  HWREG(port + GPIO_O_AFSEL) = config->afsel;
  HWREG(port + GPIO_O_DR2R)  = config->dr2r;
  HWREG(port + GPIO_O_DR4R)  = config->dr4r;
  HWREG(port + GPIO_O_DR8R)  = config->dr8r;
  HWREG(port + GPIO_O_ODR)   = config->odr;
  HWREG(port + GPIO_O_PUR)   = config->pur;
  HWREG(port + GPIO_O_PDR)   = config->pdr;
  HWREG(port + GPIO_O_SLR)   = config->slr;
  HWREG(port + GPIO_O_DEN)   = config->den;
  HWREG(port + GPIO_O_AMSEL) = config->amsel;
  HWREG(port + GPIO_O_PCTL)  = config->pctl;
}

/**
 * @brief   Unlocks the masked pins of the GPIO peripheral.
 * @note    This function is only useful for PORTC0-3, PORTD7 and PORTF0.
 *
 * @param[in] port      the port identifier
 * @param[in] mask      the pin mask
 */
static void gpio_unlock(ioportid_t port, ioportmask_t mask)
{

  HWREG(port + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  HWREG(port + GPIO_O_CR) = mask;
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Tiva I/O ports configuration.
 * @details Ports A-F (G, H, J, K, L, M, N, P, Q, R, S, T) clocks enabled.
 *
 * @param[in] config    the Tiva ports configuration
 *
 * @notapi
 */
void _pal_lld_init(const PALConfig *config)
{
  /*
   * Enables all GPIO clocks.
   */
  HWREG(SYSCTL_RCGCGPIO) = RCGCGPIO_MASK;
#if defined(TM4C123x)
  HWREG(SYSCTL_GPIOHBCTL) = GPIOHBCTL_MASK;
#endif

  /* Wait until all GPIO modules are ready */
  while (!((HWREG(SYSCTL_PRGPIO) & RCGCGPIO_MASK) == RCGCGPIO_MASK))
    ;
  
#if TICC3200_HAS_GPIOA
  gpio_init(GPIOA, &config->PAData);
#endif
#if TICC3200_HAS_GPIOB
  gpio_init(GPIOB, &config->PBData);
#endif
#if TICC3200_HAS_GPIOC
  /* Unlock JTAG pins.*/
  gpio_unlock(GPIOC, GPIOC_JTAG_MASK);
  gpio_init(GPIOC, &config->PCData);
#endif
#if TICC3200_HAS_GPIOD
  /* Unlock NMI pin.*/
  gpio_unlock(GPIOD, GPIOD_NMI_MASK);
  gpio_init(GPIOD, &config->PDData);
#endif
#if TICC3200_HAS_GPIOE
  gpio_init(GPIOE, &config->PEData);
#endif
#if TICC3200_HAS_GPIOF
  /* Unlock NMI pin.*/
  gpio_unlock(GPIOF, GPIOF_NMI_MASK);
  gpio_init(GPIOF, &config->PFData);
#endif
#if TICC3200_HAS_GPIOG || defined(__DOXYGEN__)
  gpio_init(GPIOG, &config->PGData);
#endif
#if TICC3200_HAS_GPIOH || defined(__DOXYGEN__)
  gpio_init(GPIOH, &config->PHData);
#endif
#if TICC3200_HAS_GPIOJ || defined(__DOXYGEN__)
  gpio_init(GPIOJ, &config->PJData);
#endif
#if TICC3200_HAS_GPIOK || defined(__DOXYGEN__)
  gpio_init(GPIOK, &config->PKData);
#endif
#if TICC3200_HAS_GPIOL || defined(__DOXYGEN__)
  gpio_init(GPIOL, &config->PLData);
#endif
#if TICC3200_HAS_GPIOM || defined(__DOXYGEN__)
  gpio_init(GPIOM, &config->PMData);
#endif
#if TICC3200_HAS_GPION || defined(__DOXYGEN__)
  gpio_init(GPION, &config->PNData);
#endif
#if TICC3200_HAS_GPIOP || defined(__DOXYGEN__)
  gpio_init(GPIOP, &config->PPData);
#endif
#if TICC3200_HAS_GPIOQ || defined(__DOXYGEN__)
  gpio_init(GPIOQ, &config->PQData);
#endif
#if TICC3200_HAS_GPIOR || defined(__DOXYGEN__)
  gpio_init(GPIOR, &config->PRData);
#endif
#if TICC3200_HAS_GPIOS || defined(__DOXYGEN__)
  gpio_init(GPIOS, &config->PSData);
#endif
#if TICC3200_HAS_GPIOT || defined(__DOXYGEN__)
  gpio_init(GPIOT, &config->PTData);
#endif
}

/**
 * @brief   Pads mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 *
 * @param[in] port      the port identifier
 * @param[in] mask      the group mask
 * @param[in] mode      the mode
 *
 * @notapi
 */
void _pal_lld_setgroupmode(ioportid_t port, ioportmask_t mask, iomode_t mode)
{
  uint32_t dir   = (mode & PAL_TICC3200_DIR_MASK)   >> 0;
  uint32_t afsel = (mode & PAL_TICC3200_AFSEL_MASK) >> 1;
  uint32_t dr2r  = (mode & PAL_TICC3200_DR2R_MASK)  >> 2;
  uint32_t dr4r  = (mode & PAL_TICC3200_DR4R_MASK)  >> 3;
  uint32_t dr8r  = (mode & PAL_TICC3200_DR8R_MASK)  >> 4;
  uint32_t odr   = (mode & PAL_TICC3200_ODR_MASK)   >> 5;
  uint32_t pur   = (mode & PAL_TICC3200_PUR_MASK)   >> 6;
  uint32_t pdr   = (mode & PAL_TICC3200_PDR_MASK)   >> 7;
  uint32_t slr   = (mode & PAL_TICC3200_SLR_MASK)   >> 8;
  uint32_t den   = (mode & PAL_TICC3200_DEN_MASK)   >> 9;
  uint32_t amsel = (mode & PAL_TICC3200_AMSEL_MASK) >> 10;
  uint32_t pctl  = (mode & PAL_TICC3200_PCTL_MASK)  >> 11;
  uint32_t bit   = 0;

  while(TRUE) {
    uint32_t pctl_mask = (7 << (4 * bit));
    uint32_t bit_mask = (1 << bit);

    if ((mask & 1) != 0) {
      HWREG(port + GPIO_O_DIR)   = (HWREG(port + GPIO_O_DIR)   & ~bit_mask)  | dir;
      HWREG(port + GPIO_O_AFSEL) = (HWREG(port + GPIO_O_AFSEL) & ~bit_mask)  | afsel;
      HWREG(port + GPIO_O_DR2R)  = (HWREG(port + GPIO_O_DR2R)  & ~bit_mask)  | dr2r;
      HWREG(port + GPIO_O_DR4R)  = (HWREG(port + GPIO_O_DR4R)  & ~bit_mask)  | dr4r;
      HWREG(port + GPIO_O_DR8R)  = (HWREG(port + GPIO_O_DR8R)  & ~bit_mask)  | dr8r;
      HWREG(port + GPIO_O_ODR)   = (HWREG(port + GPIO_O_ODR)   & ~bit_mask)  | odr;
      HWREG(port + GPIO_O_PUR)   = (HWREG(port + GPIO_O_PUR)   & ~bit_mask)  | pur;
      HWREG(port + GPIO_O_PDR)   = (HWREG(port + GPIO_O_PDR)   & ~bit_mask)  | pdr;
      HWREG(port + GPIO_O_SLR)   = (HWREG(port + GPIO_O_SLR)   & ~bit_mask)  | slr;
      HWREG(port + GPIO_O_DEN)   = (HWREG(port + GPIO_O_DEN)   & ~bit_mask)  | den;
      HWREG(port + GPIO_O_AMSEL) = (HWREG(port + GPIO_O_AMSEL) & ~bit_mask)  | amsel;
      HWREG(port + GPIO_O_PCTL)  = (HWREG(port + GPIO_O_PCTL)  & ~pctl_mask) | pctl;
    }

    mask >>= 1;
    if (!mask) {
      return;
    }

    dir   <<= 1;
    afsel <<= 1;
    dr2r  <<= 1;
    dr4r  <<= 1;
    dr8r  <<= 1;
    odr   <<= 1;
    pur   <<= 1;
    pdr   <<= 1;
    slr   <<= 1;
    den   <<= 1;
    amsel <<= 1;
    pctl  <<= 4;

    bit++;
  }
}

#endif /* HAL_USE_PAL */

/**
 * @}
 */
