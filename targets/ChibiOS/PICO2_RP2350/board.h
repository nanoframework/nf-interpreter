//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef BOARD_H
#define BOARD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for Raspberry Pi Pico 2 board.
 */

/*
 * Board identifier.
 */
#define BOARD_PICO2_RP2350
#define BOARD_NAME "Raspberry Pi Pico 2"

/*
 * Board oscillators-related settings.
 */
#if !defined(RP_XOSCCLK)
#define RP_XOSCCLK 12000000U
#endif

/*
 * MCU type.
 */
#define RP2350

/*
 * IO pins assignments.
 */

/*
 * IO lines assignments.
 */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C"
{
#endif
    void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* BOARD_H */
