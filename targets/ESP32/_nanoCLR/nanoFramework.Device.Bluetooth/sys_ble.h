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
    // Service UUID
    ble_uuid_t *serviceUuid;

    // Number of items in characteristics definitions/Uuid array & Attribute handles
    int characteristicsCount;
    // Ptr to characteristics definitions array
    ble_gatt_chr_def *characteristicsDefs;
    // Ptr to characteristics Uuid array
    ble_uuid_any_t *characteristicsUuids;
    // Ptr to array of attribute handles
    uint16_t *attrHandles;

    // Number of items in array of descriptor definitions & descriptor UUID
    int descriptorCount;
    // Ptr to descriptor definitions array
    ble_gatt_dsc_def *descriptorDefs;
    // Ptr to descriptor Uuid array
    ble_uuid_any_t *descriptorUuids;
};

struct ble_services_context
{
    // The service definition is discoverable
    bool isDiscoverable;
    // The service can be connected to
    bool isConnectable;
    // The device name
    char *pDeviceName;

    // Number of services in service definition
    int serviceCount;

    // Ptr to NIMBLE service definition
    ble_gatt_svc_def *gatt_service_def;

    // Array of ble contexts, 1 per service
    ble_context *bleSrvContexts;
};

extern ble_services_context bleContext;
extern device_ble_event_data ble_event_data;

#endif // SYS_BLE_H
