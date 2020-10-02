//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

/**
 * @file    CC3200/cmparams.h
 * @brief   ARM Cortex-M4 parameters for the CC3200.
 *
 * @defgroup ARMCMx_CC3200 CC3200 Specific Parameters
 * @ingroup ARMCMx_SPECIFIC
 * @details This file contains the Cortex-M4 specific parameters for the
 *          CC3200 platform.
 * @{
 */

#ifndef CMPARAMS_H
#define CMPARAMS_H

/* Defines required for correct CMSIS header functioning */
#define __MPU_PRESENT           1       /**< MPU present                     */
#define __NVIC_PRIO_BITS        3       /**< Bits used for Priority Levels   */
#define __Vendor_SysTickConfig  1       /**< Use different SysTick Config    */
#define __FPU_PRESENT           1       /**< FPU present                     */

/* The following two defines are needed by ChibiOS */
#define SVCall_IRQn             -5
#define PendSV_IRQn             -3

/**
 * @brief   Cortex core model.
 */
#define CORTEX_MODEL            4

/**
 * @brief   Floating Point unit presence.
 */
#define CORTEX_HAS_FPU          1

/**
 * @brief   Number of bits in priority masks.
 */
#define CORTEX_PRIORITY_BITS    4

/**
 * @brief   Number of interrupt vectors.
 * @note    This number does not include the 16 system vectors and must be
 *          rounded to a multiple of 8.
 */
#define CORTEX_NUM_VECTORS      184

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

// /* If the device type is not externally defined, for example from the Makefile,
//    then a file named board.h is included. This file must contain a device
//    definition compatible with the vendor include file.*/
// #if !defined(STM32F405xx) && !defined(STM32F415xx) &&                       \
//     !defined(STM32F407xx) && !defined(STM32F417xx) &&                       \
//     !defined(STM32F427xx) && !defined(STM32F437xx) &&                       \
//     !defined(STM32F429xx) && !defined(STM32F439xx) &&                       \
//     !defined(STM32F401xC) && !defined(STM32F401xE) &&                       \
//     !defined(STM32F410Cx) && !defined(STM32F410Rx) &&                       \
//     !defined(STM32F411xE) && !defined(STM32F446xx) &&                       \
//     !defined(STM32F469xx) && !defined(STM32F479xx)
#include "board.h"
// #endif

typedef int IRQn_Type;

#include "core_cm4.h"

/* Including the device CMSIS header. Note, we are not using the definitions
   from this header because we need this file to be usable also from
   assembler source files. We verify that the info matches instead.*/
//#include "cc3200.h"


/* Including the CCWare peripheral headers.*/
#include "asmdefs.h"
#include "hw_adc.h"
#include "hw_aes.h"
#include "hw_apps_config.h"
#include "hw_apps_rcm.h"
#include "hw_camera.h"
#include "hw_common_reg.h"
#include "hw_des.h"
#include "hw_dthe.h"
#include "hw_flash_ctrl.h"
#include "hw_gpio.h"
#include "hw_gprcm.h"
#include "hw_hib1p2.h"
#include "hw_hib3p3.h"
#include "hw_i2c.h"
#include "hw_ints.h"
#include "hw_mcasp.h"
#include "hw_mcspi.h"
#include "hw_memmap.h"
#include "hw_mmchs.h"
#include "hw_nvic.h"
#include "hw_ocp_shared.h"
#include "hw_shamd5.h"
#include "hw_stack_die_ctrl.h"
#include "hw_timer.h"
#include "hw_types.h"
#include "hw_uart.h"
#include "hw_udma.h"
#include "hw_wdt.h"

#if CORTEX_NUM_VECTORS != ((((NUM_INTERRUPTS - 16) + 7) / 8) * 8)
#error "CCWare NUM_INTERRUPTS mismatch"
#endif

#endif /* !defined(_FROM_ASM_) */

#endif /* CMPARAMS_H */

/** @} */
