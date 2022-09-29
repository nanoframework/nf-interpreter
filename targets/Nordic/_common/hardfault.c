//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <zephyr.h>
#include <assert.h>
#include <nanoHAL_v2.h>

//(db)TODO:: Clean up

void HAL_AssertEx()
{
    printk("***** HAL_AssertEx *****\n");
    // assert(0);
    while (true)
    { /*nop*/
        k_sleep(K_SECONDS(10));
    }
}

#if !defined(BUILD_RTM)

void HARD_Breakpoint()
{
    printk("***** HARD_Breakpoint *****\n");
    //    assert(0);
    //__BKPT(0);
    while (true)
    { /*nop*/
        k_sleep(K_SECONDS(10));
    }
};

#endif // !defined(BUILD_RTM)

// Implemented as "weak" to allow it to be replaced with "hard" implementation at target level.
__nfweak bool Target_CanChangeMacAddress()
{
    return false;
}
