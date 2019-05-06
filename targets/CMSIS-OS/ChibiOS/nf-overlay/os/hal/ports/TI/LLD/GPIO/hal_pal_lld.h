//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

/**
 * @file    GPIO/hal_pal_lld.h
 * @brief   TICC3200 PAL subsystem low level driver header.
 *
 * @addtogroup PAL
 * @{
 */

#ifndef HAL_PAL_LLD_H
#define HAL_PAL_LLD_H

#if HAL_USE_PAL || defined(__DOXYGEN__)

/*===========================================================================*/
/* Unsupported modes and specific modes                                      */
/*===========================================================================*/

#undef PAL_MODE_RESET
#undef PAL_MODE_UNCONNECTED
#undef PAL_MODE_INPUT
#undef PAL_MODE_INPUT_PULLUP
#undef PAL_MODE_INPUT_PULLDOWN
#undef PAL_MODE_INPUT_ANALOG
#undef PAL_MODE_OUTPUT_PUSHPULL
#undef PAL_MODE_OUTPUT_OPENDRAIN

/**
 * @name    TICC3200 specific I/O mode flags
 * @{
 */
#define PAL_TICC3200_DIR_MASK           (1 << 0)
#define PAL_TICC3200_DIR_INPUT          (0 << 0)
#define PAL_TICC3200_DIR_OUTPUT         (1 << 0)

#define PAL_TICC3200_AFSEL_MASK         (1 << 1)
#define PAL_TICC3200_AFSEL_GPIO         (0 << 1)
#define PAL_TICC3200_AFSEL_ALTERNATE    (1 << 1)

#define PAL_TICC3200_DR2R_MASK          (1 << 2)
#define PAL_TICC3200_DR2R_DISABLE       (0 << 2)
#define PAL_TICC3200_DR2R_ENABLE        (1 << 2)

#define PAL_TICC3200_DR4R_MASK          (1 << 3)
#define PAL_TICC3200_DR4R_DISABLE       (0 << 3)
#define PAL_TICC3200_DR4R_ENABLE        (1 << 3)

#define PAL_TICC3200_DR8R_MASK          (1 << 4)
#define PAL_TICC3200_DR8R_DISABLE       (0 << 4)
#define PAL_TICC3200_DR8R_ENABLE        (1 << 4)

#define PAL_TICC3200_ODR_MASK           (1 << 5)
#define PAL_TICC3200_ODR_PUSHPULL       (0 << 5)
#define PAL_TICC3200_ODR_OPENDRAIN      (1 << 5)

#define PAL_TICC3200_PUR_MASK           (1 << 6)
#define PAL_TICC3200_PUR_DISABLE        (0 << 6)
#define PAL_TICC3200_PUR_ENABLE         (1 << 6)

#define PAL_TICC3200_PDR_MASK           (1 << 7)
#define PAL_TICC3200_PDR_DISABLE        (0 << 7)
#define PAL_TICC3200_PDR_ENABLE         (1 << 7)

#define PAL_TICC3200_SLR_MASK           (1 << 8)
#define PAL_TICC3200_SLR_DISABLE        (0 << 8)
#define PAL_TICC3200_SLR_ENABLE         (1 << 8)

#define PAL_TICC3200_DEN_MASK           (1 << 9)
#define PAL_TICC3200_DEN_DISABLE        (0 << 9)
#define PAL_TICC3200_DEN_ENABLE         (1 << 9)

#define PAL_TICC3200_AMSEL_MASK         (1 << 10)
#define PAL_TICC3200_AMSEL_DISABLE      (0 << 10)
#define PAL_TICC3200_AMSEL_ENABLE       (1 << 10)

#define PAL_TICC3200_PCTL_MASK          (7 << 11)
#define PAL_TICC3200_PCTL(n)            ((n) << 11)

/**
 * @brief   Alternate function.
 *
 * @param[in] n         alternate function selector
 */
#define PAL_MODE_ALTERNATE(n)       (PAL_TICC3200_AFSEL_ALTERNATE |               \
                                     PAL_TICC3200_PCTL(n))
/** @} */

/**
 * @name    Standard I/O mode flags
 * @{
 */
/**
 * @brief   This mode is implemented as input.
 */
#define PAL_MODE_RESET                  PAL_MODE_INPUT

/**
 * @brief   This mode is implemented as input with pull-up.
 */
#define PAL_MODE_UNCONNECTED            PAL_MODE_INPUT_PULLUP

/**
 * @brief   Regular input high-Z pad.
 */
#define PAL_MODE_INPUT                  (PAL_TICC3200_DEN_ENABLE |                \
                                         PAL_TICC3200_DIR_INPUT)

/**
 * @brief   Input pad with weak pull up resistor.
 */
#define PAL_MODE_INPUT_PULLUP           (PAL_TICC3200_DIR_INPUT |                 \
                                         PAL_TICC3200_PUR_ENABLE |                \
                                         PAL_TICC3200_DEN_ENABLE)

/**
 * @brief   Input pad with weak pull down resistor.
 */
#define PAL_MODE_INPUT_PULLDOWN         (PAL_TICC3200_DIR_INPUT |                 \
                                         PAL_TICC3200_PDR_ENABLE |                \
                                         PAL_TICC3200_DEN_ENABLE)

/**
 * @brief   Analog input mode.
 */
#define PAL_MODE_INPUT_ANALOG           (PAL_TICC3200_DEN_DISABLE |               \
                                         PAL_TICC3200_AMSEL_ENABLE)

/**
 * @brief   Push-pull output pad.
 */
#define PAL_MODE_OUTPUT_PUSHPULL        (PAL_TICC3200_DIR_OUTPUT |                \
                                         PAL_TICC3200_DR2R_ENABLE |               \
                                         PAL_TICC3200_ODR_PUSHPULL |              \
                                         PAL_TICC3200_DEN_ENABLE)

/**
 * @brief   Open-drain output pad.
 */
#define PAL_MODE_OUTPUT_OPENDRAIN       (PAL_TICC3200_DIR_OUTPUT |                \
                                         PAL_TICC3200_DR2R_ENABLE |               \
                                         PAL_TICC3200_ODR_OPENDRAIN |             \
                                         PAL_TICC3200_DEN_ENABLE)
/** @} */

/*===========================================================================*/
/* I/O Ports Types and constants.                                            */
/*===========================================================================*/

/**
 * @name    Port related definitions
 * @{
 */
/**
 * @brief   Width, in bits, of an I/O port.
 */
#define PAL_IOPORTS_WIDTH 8

/**
 * @brief   Whole port mask.
 * @brief   This macro specifies all the valid bits into a port.
 */
#define PAL_WHOLE_PORT ((ioportmask_t)0xFF)
/** @} */

/**
 * @name    Line handling macros
 * @{
 */
/**
 * @brief   Forms a line identifier.
 * @details A port/pad pair are encoded into an @p ioline_t type. The encoding
 *          of this type is platform-dependent.
 * @note    In this driver the pad number is encoded in the lower 4 bits of
 *          the GPIO address which are guaranteed to be zero.
 */
#define PAL_LINE(port, pad)                                                 \
  ((ioline_t)((uint32_t)(port)) | ((uint32_t)(pad)))

/**
 * @brief   Decodes a port identifier from a line identifier.
 */
#define PAL_PORT(line)                                                      \
  ((ioportid_t)(((uint32_t)(line)) & 0xFFFFFFF0U))

/**
 * @brief   Decodes a pad identifier from a line identifier.
 */
#define PAL_PAD(line)                                                       \
  ((uint32_t)((uint32_t)(line) & 0x0000000FU))

/**
 * @brief   Value identifying an invalid line.
 */
#define PAL_NOLINE                      0U
/** @} */

/**
 * @brief   GPIO port setup info.
 */
typedef struct
{
  /** @brief Initial value for DATA register.*/
  uint32_t data;
  /** @brief Initial value for DIR register.*/
  uint32_t dir;
  /** @brief Initial value for AFSEL register.*/
  uint32_t afsel;
  /** @brief Initial value for DR2R register.*/
  uint32_t dr2r;
  /** @brief Initial value for DR4R register.*/
  uint32_t dr4r;
  /** @brief Initial value for DR8R register.*/
  uint32_t dr8r;
  /** @brief Initial value for ODR register.*/
  uint32_t odr;
  /** @brief Initial value for PUR register.*/
  uint32_t pur;
  /** @brief Initial value for PDR register.*/
  uint32_t pdr;
  /** @brief Initial value for SLR register.*/
  uint32_t slr;
  /** @brief Initial value for DEN register.*/
  uint32_t den;
  /** @brief Initial value for AMSEL register.*/
  uint32_t amsel;
  /** @brief Initial value for PCTL register.*/
  uint32_t pctl;
} tiva_gpio_setup_t;

/**
 * @brief   Tiva GPIO static initializer.
 * @details An instance of this structure must be passed to @p palInit() at
 *          system startup time in order to initialized the digital I/O
 *          subsystem. This represents only the initial setup, specific pads
 *          or whole ports can be reprogrammed at later time.
 */
typedef struct
{
  /** @brief GPIO port A setup data.*/
  tiva_gpio_setup_t     PAData;
  /** @brief GPIO port B setup data.*/
  tiva_gpio_setup_t     PBData;
  /** @brief GPIO port C setup data.*/
  tiva_gpio_setup_t     PCData;
  /** @brief GPIO port D setup data.*/
  tiva_gpio_setup_t     PDData;
  /** @brief GPIO port E setup data.*/
  tiva_gpio_setup_t     PEData;
  /** @brief GPIO port F setup data.*/
  tiva_gpio_setup_t     PFData;
#if TICC3200_HAS_GPIOG || defined(__DOXYGEN__)
  /** @brief GPIO port G setup data.*/
  tiva_gpio_setup_t     PGData;
#endif
#if TICC3200_HAS_GPIOH || defined(__DOXYGEN__)
  /** @brief GPIO port H setup data.*/
  tiva_gpio_setup_t     PHData;
#endif
#if TICC3200_HAS_GPIOJ || defined(__DOXYGEN__)
  /** @brief GPIO port J setup data.*/
  tiva_gpio_setup_t     PJData;
#endif
#if TICC3200_HAS_GPIOK || defined(__DOXYGEN__)
  /** @brief GPIO port K setup data.*/
  tiva_gpio_setup_t     PKData;
#endif
#if TICC3200_HAS_GPIOL || defined(__DOXYGEN__)
  /** @brief GPIO port L setup data.*/
  tiva_gpio_setup_t     PLData;
#endif
#if TICC3200_HAS_GPIOM || defined(__DOXYGEN__)
  /** @brief GPIO port M setup data.*/
  tiva_gpio_setup_t     PMData;
#endif
#if TICC3200_HAS_GPION || defined(__DOXYGEN__)
  /** @brief GPIO port N setup data.*/
  tiva_gpio_setup_t     PNData;
#endif
#if TICC3200_HAS_GPIOP || defined(__DOXYGEN__)
  /** @brief GPIO port P setup data.*/
  tiva_gpio_setup_t     PPData;
#endif
#if TICC3200_HAS_GPIOQ || defined(__DOXYGEN__)
  /** @brief GPIO port Q setup data.*/
  tiva_gpio_setup_t     PQData;
#endif
#if TICC3200_HAS_GPIOR || defined(__DOXYGEN__)
  /** @brief GPIO port R setup data.*/
  tiva_gpio_setup_t     PRData;
#endif
#if TICC3200_HAS_GPIOS || defined(__DOXYGEN__)
  /** @brief GPIO port S setup data.*/
  tiva_gpio_setup_t     PSData;
#endif
#if TICC3200_HAS_GPIOT || defined(__DOXYGEN__)
  /** @brief GPIO port T setup data.*/
  tiva_gpio_setup_t     PTData;
#endif
} PALConfig;

/**
 * @brief   Digital I/O port sized unsigned type.
 */
typedef uint32_t ioportmask_t;

/**
 * @brief   Digital I/O modes.
 */
typedef uint32_t iomode_t;

/**
 * @brief   Type of an I/O line.
 */
typedef uint32_t ioline_t;

/**
 * @brief   Port Identifier.
 */
typedef uint32_t ioportid_t;

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

#if defined(TM4C123x)

/**
 * @brief   GPIOA AHB enable switch.
 * @details When set to @p TRUE the AHB bus is used to access GPIOA. When set
 *          to @p FALSE the APB bus is used to access GPIOA.
 */
#if !defined(TICC3200_GPIO_GPIOA_USE_AHB) || defined(__DOXYGEN__)
#define TICC3200_GPIO_GPIOA_USE_AHB             TRUE
#endif

/**
 * @brief   GPIOB AHB enable switch.
 * @details When set to @p TRUE the AHB bus is used to access GPIOB. When set
 *          to @p FALSE the APB bus is used to access GPIOB.
 */
#if !defined(TICC3200_GPIO_GPIOB_USE_AHB) || defined(__DOXYGEN__)
#define TICC3200_GPIO_GPIOB_USE_AHB             TRUE
#endif

/**
 * @brief   GPIOC AHB enable switch.
 * @details When set to @p TRUE the AHB bus is used to access GPIOC. When set
 *          to @p FALSE the APB bus is used to access GPIOC.
 */
#if !defined(TICC3200_GPIO_GPIOC_USE_AHB) || defined(__DOXYGEN__)
#define TICC3200_GPIO_GPIOC_USE_AHB             TRUE
#endif

/**
 * @brief   GPIOD AHB enable switch.
 * @details When set to @p TRUE the AHB bus is used to access GPIOD. When set
 *          to @p FALSE the APB bus is used to access GPIOD.
 */
#if !defined(TICC3200_GPIO_GPIOD_USE_AHB) || defined(__DOXYGEN__)
#define TICC3200_GPIO_GPIOD_USE_AHB             TRUE
#endif

/**
 * @brief   GPIOE AHB enable switch.
 * @details When set to @p TRUE the AHB bus is used to access GPIOE. When set
 *          to @p FALSE the APB bus is used to access GPIOE.
 */
#if !defined(TICC3200_GPIO_GPIOE_USE_AHB) || defined(__DOXYGEN__)
#define TICC3200_GPIO_GPIOE_USE_AHB             TRUE
#endif

/**
 * @brief   GPIOF AHB enable switch.
 * @details When set to @p TRUE the AHB bus is used to access GPIOF. When set
 *          to @p FALSE the APB bus is used to access GPIOF.
 */
#if !defined(TICC3200_GPIO_GPIOF_USE_AHB) || defined(__DOXYGEN__)
#define TICC3200_GPIO_GPIOF_USE_AHB             TRUE
#endif

/**
 * @brief   GPIOG AHB enable switch.
 * @details When set to @p TRUE the AHB bus is used to access GPIOG. When set
 *          to @p FALSE the APB bus is used to access GPIOG.
 */
#if !defined(TICC3200_GPIO_GPIOG_USE_AHB) || defined(__DOXYGEN__)
#define TICC3200_GPIO_GPIOG_USE_AHB             TRUE
#endif

/**
 * @brief   GPIOH AHB enable switch.
 * @details When set to @p TRUE the AHB bus is used to access GPIOH. When set
 *          to @p FALSE the APB bus is used to access GPIOH.
 */
#if !defined(TICC3200_GPIO_GPIOH_USE_AHB) || defined(__DOXYGEN__)
#define TICC3200_GPIO_GPIOH_USE_AHB             TRUE
#endif

/**
 * @brief   GPIOJ AHB enable switch.
 * @details When set to @p TRUE the AHB bus is used to access GPIOJ. When set
 *          to @p FALSE the APB bus is used to access GPIOJ.
 */
#if !defined(TICC3200_GPIO_GPIOJ_USE_AHB) || defined(__DOXYGEN__)
#define TICC3200_GPIO_GPIOJ_USE_AHB             TRUE
#endif

#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if TICC3200_GPIO_GPIOA_USE_AHB && defined(TM4C123x)
#define GPIOA                               GPIO_PORTA_AHB_BASE
#else
#define GPIOA                               GPIO_PORTA_BASE
#endif

#if TICC3200_GPIO_GPIOB_USE_AHB && defined(TM4C123x)
#define GPIOB                               GPIO_PORTB_AHB_BASE
#else
#define GPIOB                               GPIO_PORTB_BASE
#endif

#if TICC3200_GPIO_GPIOC_USE_AHB && defined(TM4C123x)
#define GPIOC                               GPIO_PORTC_AHB_BASE
#else
#define GPIOC                               GPIO_PORTC_BASE
#endif

#if TICC3200_GPIO_GPIOD_USE_AHB && defined(TM4C123x)
#define GPIOD                               GPIO_PORTD_AHB_BASE
#else
#define GPIOD                               GPIO_PORTD_BASE
#endif

#if TICC3200_GPIO_GPIOE_USE_AHB && defined(TM4C123x)
#define GPIOE                               GPIO_PORTE_AHB_BASE
#else
#define GPIOE                               GPIO_PORTE_BASE
#endif

#if TICC3200_GPIO_GPIOF_USE_AHB && defined(TM4C123x)
#define GPIOF                               GPIO_PORTF_AHB_BASE
#else
#define GPIOF                               GPIO_PORTF_BASE
#endif

#if TICC3200_GPIO_GPIOG_USE_AHB && defined(TM4C123x)
#define GPIOG                               GPIO_PORTG_AHB_BASE
#else
#define GPIOG                               GPIO_PORTG_BASE
#endif

#if TICC3200_GPIO_GPIOH_USE_AHB && defined(TM4C123x)
#define GPIOH                               GPIO_PORTH_AHB_BASE
#else
#define GPIOH                               GPIO_PORTH_BASE
#endif

#if TICC3200_GPIO_GPIOJ_USE_AHB && defined(TM4C123x)
#define GPIOJ                               GPIO_PORTJ_AHB_BASE
#else
#define GPIOJ                               GPIO_PORTJ_BASE
#endif

#define GPIOK                               GPIO_PORTK_BASE
#define GPIOL                               GPIO_PORTL_BASE
#define GPIOM                               GPIO_PORTM_BASE
#define GPION                               GPIO_PORTN_BASE
#define GPIOP                               GPIO_PORTP_BASE
#define GPIOQ                               GPIO_PORTQ_BASE
#define GPIOR                               GPIO_PORTR_BASE
#define GPIOS                               GPIO_PORTS_BASE
#define GPIOT                               GPIO_PORTT_BASE

/*===========================================================================*/
/* I/O Ports Identifiers.                                                    */
/*===========================================================================*/

/**
 * @brief   GPIO port A identifier.
 */
#define IOPORT1         GPIOA

/**
 * @brief   GPIO port B identifier.
 */
#define IOPORT2         GPIOB

/**
 * @brief   GPIO port C identifier.
 */
#define IOPORT3         GPIOC

/**
 * @brief   GPIO port D identifier.
 */
#define IOPORT4         GPIOD

/**
 * @brief   GPIO port E identifier.
 */
#define IOPORT5         GPIOE

/**
 * @brief   GPIO port F identifier.
 */
#define IOPORT6         GPIOF

/**
 * @brief   GPIO port G identifier.
 */
#if TICC3200_HAS_GPIOG || defined(__DOXYGEN__)
#define IOPORT7         GPIOG
#endif

/**
 * @brief   GPIO port H identifier.
 */
#if TICC3200_HAS_GPIOH || defined(__DOXYGEN__)
#define IOPORT8         GPIOH
#endif

/**
 * @brief   GPIO port J identifier.
 */
#if TICC3200_HAS_GPIOJ || defined(__DOXYGEN__)
#define IOPORT9         GPIOJ
#endif

/**
 * @brief   GPIO port K identifier.
 */
#if TICC3200_HAS_GPIOK || defined(__DOXYGEN__)
#define IOPORT10        GPIOK
#endif

/**
 * @brief   GPIO port L identifier.
 */
#if TICC3200_HAS_GPIOL || defined(__DOXYGEN__)
#define IOPORT11        GPIOL
#endif

/**
 * @brief   GPIO port M identifier.
 */
#if TICC3200_HAS_GPIOM || defined(__DOXYGEN__)
#define IOPORT12        GPIOM
#endif

/**
 * @brief   GPIO port N identifier.
 */
#if TICC3200_HAS_GPION || defined(__DOXYGEN__)
#define IOPORT13        GPION
#endif

/**
 * @brief   GPIO port P identifier.
 */
#if TICC3200_HAS_GPIOP || defined(__DOXYGEN__)
#define IOPORT14        GPIOP
#endif

/**
 * @brief   GPIO port Q identifier.
 */
#if TICC3200_HAS_GPIOQ || defined(__DOXYGEN__)
#define IOPORT15        GPIOQ
#endif

/**
 * @brief   GPIO port R identifier.
 */
#if TICC3200_HAS_GPIOR || defined(__DOXYGEN__)
#define IOPORT16        GPIOR
#endif

/**
 * @brief   GPIO port S identifier.
 */
#if TICC3200_HAS_GPIOS || defined(__DOXYGEN__)
#define IOPORT17        GPIOS
#endif

/**
 * @brief   GPIO port T identifier.
 */
#if TICC3200_HAS_GPIOT || defined(__DOXYGEN__)
#define IOPORT18        GPIOT
#endif

/*===========================================================================*/
/* Implementation, some of the following macros could be implemented as      */
/* functions, if so please put them in pal_lld.c.                            */
/*===========================================================================*/

/**
 * @brief   Low level PAL subsystem initialization.
 *
 * @param[in] config    architecture-dependent ports configuration
 *
 * @notapi
 */
#define pal_lld_init(config) _pal_lld_init(config)

/**
 * @brief   Reads the physical I/O port states.
 *
 * @param[in] port      port identifier
 * @return              The port bits.
 *
 * @notapi
 */
#define pal_lld_readport(port) (HWREG((port) + GPIO_O_GPIO_DATA + (0xff << 2)))

/**
 * @brief   Reads the output latch.
 * @details The purpose of this function is to read back the latched output
 *          value.
 *
 * @param[in] port      port identifier
 * @return              The latched logical states.
 *
 * @notapi
 */
#define pal_lld_readlatch(port) pal_lld_readport(port)

/**
 * @brief   Writes a bits mask on a I/O port.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be written on the specified port
 *
 * @notapi
 */
#define pal_lld_writeport(port, bits) (HWREG((port) + GPIO_O_GPIO_DATA + (0xff << 2)) = (bits))

/**
 * @brief   Sets a bits mask on a I/O port.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be ORed on the specified port
 *
 * @notapi
 */
#define pal_lld_setport(port, bits) (HWREG((port) + (GPIO_O_GPIO_DATA + (bits << 2))) = 0xFF)

/**
 * @brief   Clears a bits mask on a I/O port.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be cleared on the specified port
 *
 * @notapi
 */
#define pal_lld_clearport(port, bits) (HWREG((port) + (GPIO_O_GPIO_DATA + (bits << 2))) = 0)

/**
 * @brief   Reads a group of bits.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @return              The group logical states.
 *
 * @notapi
 */
#define pal_lld_readgroup(port, mask, offset)   \
  (HWREG((port) + (GPIO_O_GPIO_DATA + (((mask) << (offset)) << 2))))

/**
 * @brief   Writes a group of bits.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @param[in] bits      bits to be written. Values exceeding the group width
 *                      are masked.
 *
 * @notapi
 */
#define pal_lld_writegroup(port, mask, offset, bits)    \
  (HWREG((port) + (GPIO_O_GPIO_DATA + (((mask) << (offset)) << 2))) = (bits))

/**
 * @brief   Pads group mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 * @note    Programming an unknown or unsupported mode is silently ignored.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @param[in] mode      group mode
 *
 * @notapi
 */
#define pal_lld_setgroupmode(port, mask, offset, mode)                      \
  _pal_lld_setgroupmode(port, mask << offset, mode)

/**
 * @brief   Reads a logical state from an I/O pad.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @return              The logical state.
 * @retval PAL_LOW      low logical state.
 * @retval PAL_HIGH     high logical state.
 *
 * @notapi
 */
#define pal_lld_readpad(port, pad) (HWREG((port) + (GPIO_O_GPIO_DATA + ((1 << (pad)) << 2))))

/**
 * @brief   Writes a logical state on an output pad.
 * @note    This function is not meant to be invoked directly by the
 *          application  code.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @param[in] bit       logical value, the value must be @p PAL_LOW or
 *                      @p PAL_HIGH
 *
 * @notapi
 */
#define pal_lld_writepad(port, pad, bit)    \
  (HWREG((port) + (GPIO_O_GPIO_DATA + ((1 << (pad)) << 2))) = (bit))

/**
 * @brief   Sets a pad logical state to @p PAL_HIGH.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_setpad(port, pad)   \
  (HWREG((port) + (GPIO_O_GPIO_DATA + ((1 << (pad)) << 2))) = 1 << (pad))

/**
 * @brief   Clears a pad logical state to @p PAL_LOW.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_clearpad(port, pad) \
  (HWREG((port) + (GPIO_O_GPIO_DATA + ((1 << (pad)) << 2))) = 0)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
extern const PALConfig pal_default_config;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void _pal_lld_init(const PALConfig *config);
  void _pal_lld_setgroupmode(ioportid_t port,
                             ioportmask_t mask,
                             iomode_t mode);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_PAL */

#endif /* HAL_PAL_LLD_H */

/** @} */
