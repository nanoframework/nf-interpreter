

#include <stdio.h>

// #include "tx_api.h"

// #include "board_init.h"
// #include "cmsis_utils.h"

//  Application entry point.
int main(void)
{
    // Initialize the board
    // board_init();

    // Enter the ThreadX kernel
    tx_kernel_enter();
    return 0;
}
