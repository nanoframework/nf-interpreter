//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_I2C_H
#define TARGET_HAL_I2C_H

#include <target_platform.h>
#include <esp32_idf.h>
#include <nanoHAL_v2.h>

extern int16_t Esp_I2C_Initialised_Flag[I2C_NUM_MAX];

void Esp32_I2c_UnitializeAll();

#endif // TARGET_HAL_I2C_H
