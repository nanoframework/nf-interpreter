//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>
#include "Esp32_DeviceMapping.h"
#include "nanoPAL.h"
#include "sys_dev_ble_native.h"

static const char *tag = "BLE";
static uint8_t esp32_addr_type;

void esp32_ble_start_advertise(ble_context *context);

uint16_t ble_event_next_id = 1;
device_ble_event_data ble_event_data;
EventGroupHandle_t ble_event_waitgroup;
bool ble_initialized = false;
//
// Look up Attr_handle in characteristicsDefs table to find our characteristicsId
// return 0xffff if not found otherwise characteristicsId
uint16_t FindIdFromHandle(ble_gatt_chr_def *characteristicsDefs, int countDefs, uint16_t attr_handle)
{
    uint16_t id = 0xffff;

    for (int index = 0; index < countDefs; index++)
    {
        // ESP_LOGI(tag, "FindIdFromHandle; find ah=%d  - vh %d arg %d\n", attr_handle,
        // *(characteristicsDefs[index].val_handle), (uint32_t)characteristicsDefs[index].arg );
        if (*(characteristicsDefs[index].val_handle) == attr_handle)
        {
            id = (uint16_t)(uint32_t)characteristicsDefs[index].arg;
            break;
        }
    }

    return id;
}

uint16_t FindHandleIdFromId(ble_gatt_chr_def *characteristicsDefs, int countDefs, uint16_t characteristicId)
{
    uint16_t handle = 0xffff;

    for (int index = 0; index < countDefs; index++)
    {
        // ESP_LOGI(tag, "FindHandleIdFromId; find ah=%d  - vh %d arg %d\n", attr_handle,
        // *(characteristicsDefs[index].val_handle), (uint32_t)characteristicsDefs[index].arg );
        if ((uint32_t)characteristicsDefs[index].arg == (uint32_t)characteristicId)
        {
            handle = *(characteristicsDefs[index].val_handle);
            break;
        }
    }

    return handle;
}

static int esp32_gap_event(struct ble_gap_event *event, void *arg)
{
    ble_context *con = (ble_context *)arg;

    switch (event->type)
    {

        case BLE_GAP_EVENT_CONNECT:
            // A new connection was established or a connection attempt failed
            ESP_LOGI(
                tag,
                "connection %s; status=%d\n",
                event->connect.status == 0 ? "established" : "failed",
                event->connect.status);

            break;

        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(tag, "BLE_GAP_EVENT_DISCONNECT; reason=%d\n", event->disconnect.reason);

            // Connection terminated; resume advertising
            esp32_ble_start_advertise(con);
            break;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI(tag, "BLE_GAP_EVENT_ADV_COMPLETE adv complete");
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
        {
            BluetoothEventType op = BluetoothEventType_Read;

            // Find characteristicId from attr_handle
            uint16_t characteristicId =
                FindIdFromHandle(con->characteristicsDefs, con->characteristicsCount, event->subscribe.attr_handle);

            ESP_LOGI(
                tag,
                "BLE_GAP_EVENT_SUBSCRIBE conn_handle=%d attr_handle=%d "
                "reason=%d prevn=%d curn=%d previ=%d curi=%d arg=%d\n",
                event->subscribe.conn_handle,
                event->subscribe.attr_handle,
                event->subscribe.reason,
                event->subscribe.prev_notify,
                event->subscribe.cur_notify,
                event->subscribe.prev_indicate,
                event->subscribe.cur_indicate,
                characteristicId);

            // Valid characteristicId
            if (characteristicId == 0xffff)
            {
                break; // ignore
            }

            switch (event->subscribe.reason)
            {
                // Peer's CCCD subscription state changed due to a descriptor write.
                case BLE_GAP_SUBSCRIBE_REASON_WRITE:
                    if (event->subscribe.prev_indicate == 0 && event->subscribe.cur_indicate == 1)
                    {
                        op = BluetoothEventType_ClientSubscribed;
                    }
                    else if (event->subscribe.prev_notify == 0 && event->subscribe.cur_notify == 1)
                    {
                        op = BluetoothEventType_ClientSubscribed;
                    }
                    else if (event->subscribe.prev_indicate == 1 && event->subscribe.cur_indicate == 0)
                    {
                        op = BluetoothEventType_ClientUnsubscribed;
                    }
                    else if (event->subscribe.prev_notify == 1 && event->subscribe.cur_notify == 0)
                    {
                        op = BluetoothEventType_ClientUnsubscribed;
                    }
                    break;

                // Peer's CCCD subscription state cleared due to connection termination.
                case BLE_GAP_SUBSCRIBE_REASON_TERM:
                    op = BluetoothEventType_ClientUnsubscribed;
                    break;

                // Peer's CCCD subscription state changed due to restore from persistence
                case BLE_GAP_SUBSCRIBE_REASON_RESTORE:
                    op = BluetoothEventType_ClientSubscribed;
                    break;
            }

            if (op != BluetoothEventType_Read)
            {
                PostManagedEvent(EVENT_BLUETOOTH, op, characteristicId, event->subscribe.conn_handle);
            }

            //         ESP_LOGI(tag, "BLE_GAP_SUBSCRIBE_EVENT conn_handle from subscribe=%d\n", conn_handle);
        }
        break;

        case BLE_GAP_EVENT_MTU:
            ESP_LOGI(
                tag,
                "BLE_GAP_EVENT_MTU mtu update event; conn_handle=%d mtu=%d\n",
                event->mtu.conn_handle,
                event->mtu.value);
            break;
    }

    return 0;
}

//
// Enables advertising with parameters:
//     o General discoverable mode
//     o Undirected connectable mode
//
void esp32_ble_start_advertise(ble_context *context)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    //
    //  Set the advertisement data included in our advertisements:
    //     o Flags (indicates advertisement type and other general info)
    //     o Advertising tx power
    //     o Device name
    //
    memset(&fields, 0, sizeof(fields));

    //
    // Advertise two flags:
    //     o Discoverability in forthcoming advertisement (general)
    //     o BLE-only (BR/EDR unsupported)
    //
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    //
    // Indicate that the TX power level field should be included; have the
    // stack fill this value automatically.  This is done by assigning the
    // special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
    //
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)context->pDeviceName;
    fields.name_len = hal_strlen_s(context->pDeviceName);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0)
    {
        ESP_LOGI(tag, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    // Begin advertising
    memset(&adv_params, 0, sizeof(adv_params));

    if (context->isConnectable)
    {
        adv_params.conn_mode |= BLE_GAP_CONN_MODE_UND;
    }
    if (context->isDiscoverable)
    {
        adv_params.conn_mode |= BLE_GAP_DISC_MODE_GEN;
    }
    rc = ble_gap_adv_start(esp32_addr_type, NULL, BLE_HS_FOREVER, &adv_params, esp32_gap_event, (void *)&blecontext);
    if (rc != 0)
    {
        ESP_LOGI(tag, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}

static void esp32_ble_on_sync(void)
{
    int rc;

    rc = ble_hs_id_infer_auto(0, &esp32_addr_type);
    if (rc != 0)
    {
        ESP_LOGI(tag, "error ble_hs_id_infer_auto; rc=%d\n", rc);
        return;
    }

    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(esp32_addr_type, addr_val, NULL);
    if (rc != 0)
    {
        ESP_LOGI(tag, "error ble_hs_id_copy_addr; rc=%d\n", rc);
        return;
    }

    ESP_LOGI(
        tag,
        "BLE on sync, adr %02x:%02x:%02x:%02x:%02x:%02x",
        addr_val[5],
        addr_val[4],
        addr_val[3],
        addr_val[2],
        addr_val[1],
        addr_val[0]);

    // Begin advertising
    // debug_printf("Begin advertising\n");
    esp32_ble_start_advertise(&blecontext);
}

static void esp32_ble_on_reset(int reason)
{
    ESP_LOGI(tag, "BLE on reset %d\n", reason);
}

void esp32_ble_host_task(void *param)
{
    (void)param;

    ESP_LOGI(tag, "Bluetooth LE Host Task Started");
    // This function will return only when nimble_port_stop() is execute
    nimble_port_run();

    nimble_port_freertos_deinit();
}

void device_ble_dispose()
{
    int rc = nimble_port_stop();
    if (rc == 0)
    {
        nimble_port_deinit();

        rc = esp_nimble_hci_and_controller_deinit();
        if (rc != ESP_OK)
        {
            ESP_LOGE(tag, "esp_nimble_hci_and_controller_deinit() failed with error: %d", rc);
        }
    }

    vEventGroupDelete(ble_event_waitgroup);

    ble_initialized = false;
}

bool device_ble_init()
{
    // If already initialized then dispose first
    // This can happen when you start debugger
    if (ble_initialized)
    {
        device_ble_dispose();
    }

    ble_event_waitgroup = xEventGroupCreate();

    //    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());
    if (esp_nimble_hci_and_controller_init() != ESP_OK)
    {
        device_ble_dispose();
        return false;
    }

    nimble_port_init();

    // Initialize the NimBLE host configuration
    ble_hs_cfg.sync_cb = esp32_ble_on_sync;
    ble_hs_cfg.reset_cb = esp32_ble_on_reset;

    ble_initialized = true;

    return true;
}

int device_ble_start(ble_context *con)
{
    int rc;
    ble_gatt_svc_def *gatt_svr_svcs = con->gatt_service_def;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0)
    {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0)
    {
        return rc;
    }

    // Set the default device name
    rc = ble_svc_gap_device_name_set(con->pDeviceName);
    assert(rc == 0);

    // Start the BLE task
    nimble_port_freertos_init(esp32_ble_host_task);

    return 0;
}

//
// Read data for attribute from managed code
//
void device_ble_read(uint8_t *buffer, int buffer_length)
{
    int rc;

    rc = os_mbuf_append(ble_event_data.ctxt->om, buffer, buffer_length);
    ble_event_data.result = rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    // Signal BLE callback
    xEventGroupSetBits(ble_event_waitgroup, 1);
}

//
//  Callback from Nimble for characteristic events
//  These are posted to Managed code and if not handled within 500ms an error is reported to Nimble back to client
//  This could be due to no event handler set up in managed code or just taking too long.
//
int device_ble_callback(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    EventBits_t uxBits;

    // Save event context
    ble_event_data.eventId = ble_event_next_id++;
    ble_event_data.conn_handle = conn_handle;
    ble_event_data.characteristicId = (uint16_t)(int)arg;
    ble_event_data.ctxt = ctxt;

    BluetoothEventType op;

    // debug_printf("device_ble_callback attr %d op %d\n", attr_handle, ctxt->op);

    switch (ctxt->op)
    {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            op = BluetoothEventType_Read;
            break;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            op = BluetoothEventType_Write;
            break;

            // TODO handle descriptors
            //#define BLE_GATT_ACCESS_OP_READ_DSC  2
            // #define BLE_GATT_ACCESS_OP_WRITE_DSC 3

        default:
            return BLE_ATT_ERR_REQ_NOT_SUPPORTED;
    }

    // Post event to managed code
    PostManagedEvent(EVENT_BLUETOOTH, op, ble_event_data.characteristicId, ble_event_data.eventId);

    //    debug_printf("xEventGroupWaitBits\n");

    // Wait for 1 second for event to be handled in managed code otherwise fail request
    uxBits = xEventGroupWaitBits(ble_event_waitgroup, 1, pdTRUE, pdFALSE, (TickType_t)(1000 / portTICK_PERIOD_MS));
    //    debug_printf("xEventGroupWaitBits exit %d\n", uxBits);
    if (uxBits & 1)
    {
        // Event handled in managed code
        return ble_event_data.result;
    }

    // Reset eventid so managed code callback doesn't try to do anything
    ble_event_data.eventId = -1;

    // Event not handled in manged code, timeout
    // Return bluetooth error BLE_ATT_ERR_UNLIKELY
    return BLE_ATT_ERR_UNLIKELY;
}
