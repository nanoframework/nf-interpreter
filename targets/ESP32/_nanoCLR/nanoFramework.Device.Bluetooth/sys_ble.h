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

#include "nimble_utils.h"

typedef struct
{
    uint16_t eventId;

    // Control access to event data
    SemaphoreHandle_t mutex;

    int result;

    // Client Fields
    uint16_t conn_handle;
    uint16_t characteristicId;
    struct ble_gatt_access_ctxt *ctxt;

    // Central fields
    struct ble_gap_event *gapEvent;

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

struct bleServicesContext
{
    // The service definition is discoverable
    bool isDiscoverable;
    // The service can be connected to
    bool isConnectable;
    // The device name
    char *pDeviceName;

    // True if using extended advertisements (Bluetooth 5.0)
    bool useExtendedAdvert;

    // Ptr & length of advertisement packet
    int advertDataLen;
    uint8_t *advertData;

    // Ptr & length of scanresponse packet (Legacy)
    int scanResponseLen;
    uint8_t *scanResponse;

    // Number of services in service definition
    int serviceCount;

    // Ptr to NIMBLE service definition
    ble_gatt_svc_def *gatt_service_def;

    // Array of ble contexts, 1 per service
    ble_context *bleSrvContexts;
};

void StartBleTask(char *deviceName, uint16_t appearance);
bool DeviceBleInit();
void Device_ble_dispose();
int Esp32GapEvent(struct ble_gap_event *event, void *arg);
int ConnectionCount();
void SetSecuritySettings(
    DevicePairingIOCapabilities IOCaps,
    DevicePairingProtectionLevel protectionLevel,
    bool allowBonding,
    bool allowOob);

extern bleServicesContext bleContext;
extern device_ble_event_data ble_event_data;
extern EventGroupHandle_t ble_event_waitgroup;
extern char *bleDeviceName;

#define N_BLE_EVENT_HANDLED 1 // Bit 1 to signal event has been handled
#define N_BLE_EVENT_STARTED 2 // Bit 2 to signal BLE stack has started
#endif                        // SYS_BLE_H
