//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c)  Apache Software Foundation (ASF).  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef H_BLEHR_SENSOR_
#define H_BLEHR_SENSOR_

#include "nimble/ble.h"
#include "modlog/modlog.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Heart-rate configuration */
#define GATT_HRS_UUID                           0x180D
#define GATT_HRS_MEASUREMENT_UUID               0x2A37
#define GATT_HRS_BODY_SENSOR_LOC_UUID           0x2A38
#define GATT_DEVICE_INFO_UUID                   0x180A
#define GATT_MANUFACTURER_NAME_UUID             0x2A29
#define GATT_MODEL_NUMBER_UUID                  0x2A24

extern uint16_t hrs_hrm_handle;

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);

#ifdef __cplusplus
}
#endif

#endif
