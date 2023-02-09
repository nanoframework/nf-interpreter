//
// Copyright (c) .NET Foundation and Contributors
// Copyright 2020 Silicon Laboratories Inc. www.silabs.com
// See LICENSE file in the project root for full license information.
//

#include "sl_i2cspm.h"
#include "em_cmu.h"
// Include instance config 
#include "sl_i2cspm_sensor_config.h"

sl_i2cspm_t *sl_i2cspm_sensor = SL_I2CSPM_SENSOR_PERIPHERAL;

#if SL_I2CSPM_SENSOR_SPEED_MODE == 0
#define SL_I2CSPM_SENSOR_HLR i2cClockHLRStandard
#define SL_I2CSPM_SENSOR_MAX_FREQ I2C_FREQ_STANDARD_MAX
#elif SL_I2CSPM_SENSOR_SPEED_MODE == 1
#define SL_I2CSPM_SENSOR_HLR i2cClockHLRAsymetric
#define SL_I2CSPM_SENSOR_MAX_FREQ I2C_FREQ_FAST_MAX
#elif SL_I2CSPM_SENSOR_SPEED_MODE == 2
#define SL_I2CSPM_SENSOR_HLR i2cClockHLRFast
#define SL_I2CSPM_SENSOR_MAX_FREQ I2C_FREQ_FASTPLUS_MAX
#endif

I2CSPM_Init_TypeDef init_sensor = { 
  .port = SL_I2CSPM_SENSOR_PERIPHERAL,
  .sclPort = SL_I2CSPM_SENSOR_SCL_PORT,
  .sclPin = SL_I2CSPM_SENSOR_SCL_PIN,
  .sdaPort = SL_I2CSPM_SENSOR_SDA_PORT,
  .sdaPin = SL_I2CSPM_SENSOR_SDA_PIN,
  .portLocationScl = SL_I2CSPM_SENSOR_SCL_LOC,
  .portLocationSda = SL_I2CSPM_SENSOR_SDA_LOC,
  .i2cRefFreq = 0,
  .i2cMaxFreq = SL_I2CSPM_SENSOR_MAX_FREQ,
  .i2cClhr = SL_I2CSPM_SENSOR_HLR
};

void sl_i2cspm_init_instances(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
}
