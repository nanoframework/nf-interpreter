//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

//
//  Handling for Fixed characteristics for device information (Maybe this should be done in managed code)
//  Note: Using "C" file as struct initializers don't work in CPP when using nimble, i.e. BLE_UUID16_DECLARE

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#pragma GCC diagnostic pop

#define GATT_MANUFACTURER_NAME_UUID 0x2A29
#define GATT_MODEL_NUMBER_UUID      0x2A24

static const char *manuf_name = "Nanoframework";
static const char *model_num = "ESP32";

static int gatt_svr_chr_access_device_info(
    uint16_t conn_handle,
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg);

const struct ble_gatt_chr_def gatt_char_device_info[] = {
    {
        // Characteristic:  Manufacturer name
        .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
        .access_cb = gatt_svr_chr_access_device_info,
        .flags = BLE_GATT_CHR_F_READ,
    },
    {
        // Characteristic: Model number string
        .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
        .access_cb = gatt_svr_chr_access_device_info,
        .flags = BLE_GATT_CHR_F_READ,
    },
    {
        // No more characteristics in this service
        0,
    }};

static int gatt_svr_chr_access_device_info(
    uint16_t conn_handle,
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    uint16_t uuid;
    int rc;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == GATT_MODEL_NUMBER_UUID)
    {
        rc = os_mbuf_append(ctxt->om, model_num, strlen(model_num));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (uuid == GATT_MANUFACTURER_NAME_UUID)
    {
        rc = os_mbuf_append(ctxt->om, manuf_name, strlen(manuf_name));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    return BLE_ATT_ERR_UNLIKELY;
}
