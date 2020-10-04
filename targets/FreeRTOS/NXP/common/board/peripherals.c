//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 NXP. All rights reserved.
// See LICENSE file in the project root for full license information.
//

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v1.0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/*******************************************************************************
 * Included files
 ******************************************************************************/
#include "peripherals.h"

/*******************************************************************************
 * BOARD_InitBootPeripherals function
 ******************************************************************************/
void BOARD_InitBootPeripherals(void)
{
    BOARD_ConfigMPU();
}
