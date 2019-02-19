//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_os.h>

void HAL_AssertEx()
{
    //__BKPT(0);
    while(true) { /*nop*/ }
}
