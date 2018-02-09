

#include "hal.h"


/**
 * @brief   Early initialization code.
 * @details This initialization is performed just after reset before BSS and
 *          DATA segments initialization.
 */
void __early_init(void)
{
  ticc3200_clock_init();
}

/**
 * @brief   Late initialization code.
 * @note    This initialization is performed after BSS and DATA segments 
 *          initialization and before invoking the main() function.
 */
void boardInit(void)
{
}
