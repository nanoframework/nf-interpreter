//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <target_platform.h>
#include <esp32_idf.h>
#include <nanoHAL_v2.h>

int16_t Esp_I2C_Initialised_Flag[I2C_NUM_MAX] = {
    0
#if I2C_NUM_MAX > 1
    ,
    0
#endif
};

void Esp32_I2c_UnitializeAll()
{
    for (int c = 0; c < I2C_NUM_MAX; c++)
    {
        if (Esp_I2C_Initialised_Flag[c])
        {
            // Delete bus driver
            i2c_driver_delete((i2c_port_t)c);
            Esp_I2C_Initialised_Flag[c] = 0;
        }
    }
}
