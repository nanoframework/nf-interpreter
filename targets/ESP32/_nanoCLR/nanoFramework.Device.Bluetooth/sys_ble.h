//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_BLE_H
#define SYS_BLE_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#pragma GCC diagnostic pop

typedef struct
{
    uint16_t eventId;
    uint16_t conn_handle;
    uint16_t characteristicId;
    struct ble_gatt_access_ctxt *ctxt;
    int result;
} device_ble_event_data;

struct ble_context
{
    bool isDiscoverable;
    bool isConnectable;
    char *pDeviceName;

    int serviceCount;
    ble_gatt_svc_def *gatt_service_def;

    int characteristicsCount;
    ble_gatt_chr_def *characteristicsDefs;
    ble_uuid_any_t *characteristicsUuids;
    uint16_t *attrHandles;

    int descriptorCount;
    ble_gatt_dsc_def *descriptorDefs;
    ble_uuid_any_t *descriptorUuids;
};

extern ble_context blecontext;
extern device_ble_event_data ble_event_data;

#endif // SYS_BLE_H
