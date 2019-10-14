
/**
 * @file    TICC3200/ticc3200_isr.h
 * @brief   TICC3200 ISR remapper driver header.
 *
 * @addtogroup TICC3200_ISR
 * @{
 */

#ifndef _TICC3200_ISR_H_
#define _TICC3200_ISR_H_

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    ISR names and numbers remapping
 * @{
 */

// /* UDMA units.*/
// #define TICC3200_UDMA_SW_HANDLER                VectorF8
// #define TICC3200_UDMA_ERR_HANDLER               VectorFC

// #define TICC3200_UDMA_SW_NUMBER                 46
// #define TICC3200_UDMA_ERR_NUMBER                47

/* GPIO units.*/
#define TICC3200_GPIOA_HANDLER                  INT_GPIOA0
#define TICC3200_GPIOB_HANDLER                  INT_GPIOA1
#define TICC3200_GPIOC_HANDLER                  INT_GPIOA2
#define TICC3200_GPIOD_HANDLER                  INT_GPIOA3

#define TICC3200_GPIOA_NUMBER                   0
#define TICC3200_GPIOB_NUMBER                   1
#define TICC3200_GPIOC_NUMBER                   2
#define TICC3200_GPIOD_NUMBER                   3

#endif // _TICC3200_ISR_H_