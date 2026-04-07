//
// Copyright (c) .NET Foundation and Contributors
// Copyright 2020 Silicon Laboratories Inc. www.silabs.com
// See LICENSE file in the project root for full license information.
//
 
#ifndef SL_I2CSPM_INSTANCES_H
#define SL_I2CSPM_INSTANCES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_i2cspm.h"


#define SL_I2CSPM_SENSOR_PRESENT


extern sl_i2cspm_t *sl_i2cspm_sensor;

void sl_i2cspm_init_instances(void);

#ifdef __cplusplus
}
#endif

#endif // SL_I2CSPM_INSTANCES_H
