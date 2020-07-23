//
// Copyright (c) 2020 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <common.h>

void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc)
{
    (void)hcrc;

    __HAL_RCC_CRC_CLK_ENABLE();
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc)
{
    (void)hcrc;

    __HAL_RCC_CRC_CLK_DISABLE();
}
