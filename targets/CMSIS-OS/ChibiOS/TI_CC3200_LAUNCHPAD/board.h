
#ifndef BOARD_H
#define BOARD_H

/*
 * Setup for Texas Instruments CC3200 Launchpad Board
 */

/*
 * Board identifier.
 */
#define BOARD_TI_CC3200_LAUNCHPAD
#define BOARD_NAME              "Texas Instruments CC3200 Launchpad"

/*
 * MCU type and revision as defined in the TI header.
 */
#define PART_CC3200
#define TARGET_IS_CC3200


#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif //  BOARD_H