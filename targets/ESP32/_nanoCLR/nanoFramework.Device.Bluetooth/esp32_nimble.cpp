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

void Esp32BleStartAdvertise(bleServicesContext *context);
void BleCentralStartScan();

uint16_t ble_event_next_id = 1;
device_ble_event_data ble_event_data;
EventGroupHandle_t ble_event_waitgroup;
bool ble_initialized = false;

extern BluetoothNanoDevice_Mode ble_operatingMode;

//
// Look up Attr_handle in characteristicsDefs table to find our characteristicsId
// return 0xffff if not found otherwise characteristicsId
uint16_t FindIdFromHandle(bleServicesContext *context, uint16_t attr_handle)
{
    bool found = false;
    uint16_t id = 0xffff;

    for (int service = 0; service < context->serviceCount; service++)
    {
        ble_context *srv = &context->bleSrvContexts[service];
        ble_gatt_chr_def *characteristicsDefs = srv->characteristicsDefs;

        // Check all characteristics except terminator (-1)
        for (int index = 0; index < (srv->characteristicsCount - 1); index++)
        {
            // ESP_LOGI(tag, "FindIdFromHandle; find ah=%d  - vh %d arg %d\n", attr_handle,
            // *(characteristicsDefs[index].val_handle), (uint32_t)characteristicsDefs[index].arg );
            uint16_t *pValue = characteristicsDefs[index].val_handle;
            if (pValue != 0 && *pValue == attr_handle)
            {
                id = (uint16_t)(uint32_t)characteristicsDefs[index].arg;
                found = true;
                break;
            }
        }
        if (found)
        {
            break;
        }
    }

    return id;
}

uint16_t FindHandleIdFromId(bleServicesContext &context, uint16_t characteristicId)
{
    bool found = false;
    uint16_t handle = 0xffff;

    for (int service = 0; service < context.serviceCount; service++)
    {
        ble_context *srv = &context.bleSrvContexts[service];
        ble_gatt_chr_def *characteristicsDefs = srv->characteristicsDefs;

        for (int index = 0; index < srv->characteristicsCount; index++)
        {
            // ESP_LOGI(tag, "FindHandleIdFromId; find ah=%d  - vh %d arg %d\n", attr_handle,
            // *(characteristicsDefs[index].val_handle), (uint32_t)characteristicsDefs[index].arg );
            if ((uint32_t)characteristicsDefs[index].arg == (uint32_t)characteristicId)
            {
                handle = *(characteristicsDefs[index].val_handle);
                found = true;
                break;
            }
        }

        if (found)
        {
            break;
        }
    }

    return handle;
}

bool PostAndWaitManagedGapEvent(uint8_t op, uint16_t data1, uint32_t data2)
{
    EventBits_t uxBits;

    // Make sure group clear before we post event
    xEventGroupClearBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);

    BLE_DEBUG_PRINTF("Postevent\n");

    // Post event to managed code
    PostManagedEvent(EVENT_BLUETOOTH, op, data1, data2);

    // Wait for 1 second for event to be handled in managed code otherwise fail request
    uxBits = xEventGroupWaitBits(
        ble_event_waitgroup,
        N_BLE_EVENT_HANDLED,
        pdTRUE,
        pdFALSE,
        (TickType_t)(2000 / portTICK_PERIOD_MS));
    BLE_DEBUG_PRINTF("wait handled ? complete %X\n", uxBits);
    if (uxBits & 1)
    {
        // Reset event data
        if (LockEventMutex())
        {
            ble_event_data.eventId = -1; // reset event ID
            ble_event_data.gapEvent = 0;

            ReleaseEventMutex();
            return true;
        }
    }

    return false;
}

int Esp32GapEvent(struct ble_gap_event *event, void *arg)
{
    bleServicesContext *con = (bleServicesContext *)arg;

    switch (event->type)
    {
        //
        // An advertisement report was received during a GAP discovery. (Scanning)
        //
        case BLE_GAP_EVENT_DISC:

            if (LockEventMutex())
            {
                ble_event_data.eventId = ble_event_next_id++;
                ble_event_data.gapEvent = event; // Save event ptr
                ble_event_data.result = -1;

                BLE_DEBUG_PRINTF(
                    "BLE_GAP_EVENT_DISC event type:%d adr:%X rssi:%d\n",
                    event->disc.event_type,
                    (unsigned int)event->disc.addr.val,
                    event->disc.rssi);

                ReleaseEventMutex();

                // Post Discovery event to managed code
                if (PostAndWaitManagedGapEvent(BluetoothEventType_AdvertisementDiscovered, 0, ble_event_data.eventId))
                {
                    // Handled
                    BLE_DEBUG_PRINTF("disc handled");
                }

                BLE_DEBUG_PRINTF("adv2 adr:%x rssi:%d\n", &event->disc.addr, event->disc.rssi);
            }

            return 0;

        //
        // Discovery Scan complete
        //
        case BLE_GAP_EVENT_DISC_COMPLETE:
            MODLOG_DFLT(INFO, "discovery complete; reason=%d\n", event->disc_complete.reason);

            // Post event to managed code
            PostManagedEvent(EVENT_BLUETOOTH, BluetoothEventType_ScanningComplete, event->disc_complete.reason, 0);

            return 0;

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
            Esp32BleStartAdvertise(con);
            break;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI(tag, "BLE_GAP_EVENT_ADV_COMPLETE adv complete");
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
        {
            BluetoothEventType op = BluetoothEventType_Read;

            // Find characteristicId from attr_handle
            uint16_t characteristicId = FindIdFromHandle(con, event->subscribe.attr_handle);

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
void Esp32BleStartAdvertise(bleServicesContext *context)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    struct ble_hs_adv_fields scanResp;
    bool useScanResponse = false;
    int rc;

    //
    //  Set the advertisement data included in our advertisements:
    //     o Flags (indicates advertisement type and other general info)
    //     o Advertising tx power
    //     o Device name
    //
    //  What doesn't fit in primary advert packet we add to scanData for Scan response
    //
    memset(&fields, 0, sizeof(fields));
    memset(&scanResp, 0, sizeof(scanResp));

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

    // Only advertise first Service UUID for now.
    // if 16 or 32 add to advert fields.
    // 128 uuid probably won't fix so add to scan response.
    switch (context->gatt_service_def->uuid->type)
    {
        case BLE_UUID_TYPE_16:
            fields.uuids16 = (ble_uuid16_t *)context->gatt_service_def->uuid;
            fields.num_uuids16 = 1;
            fields.uuids16_is_complete = 1;
            break;

        case BLE_UUID_TYPE_32:
            fields.uuids32 = (ble_uuid32_t *)context->gatt_service_def->uuid;
            fields.num_uuids32 = 1;
            fields.uuids32_is_complete = 1;
            break;

        case BLE_UUID_TYPE_128:
            scanResp.uuids128 = (ble_uuid128_t *)platform_malloc(sizeof(ble_uuid128_t));
            memcpy((void *)scanResp.uuids128, context->gatt_service_def->uuid, sizeof(ble_uuid128_t));
            scanResp.num_uuids128 = 1;
            scanResp.uuids128_is_complete = 1;
            useScanResponse = true;
            break;
    }

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0)
    {
        BLE_DEBUG_PRINTF("error setting advertisement data; rc=%d\n", rc);
    }

    if (useScanResponse)
    {
        rc = ble_gap_adv_rsp_set_fields(&scanResp);
        if (rc != 0)
        {
            BLE_DEBUG_PRINTF("setting scan response data; rc=%d\n", rc);
        }
    }

    // Free any allocated memory
    if (scanResp.uuids128 != 0)
    {
        platform_free((void *)scanResp.uuids128);
    }

    if (rc != 0)
    {
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
    rc = ble_gap_adv_start(esp32_addr_type, NULL, BLE_HS_FOREVER, &adv_params, Esp32GapEvent, (void *)&bleContext);
    if (rc != 0)
    {
        BLE_DEBUG_PRINTF("error enabling advertisement; rc=%d\n", rc);
        return;
    }
}

static void Esp32BleOnSync(void)
{
    int rc;

    BLE_DEBUG_PRINTF("Esp32BleOnSync\n");

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

    switch (ble_operatingMode)
    {
        case BluetoothNanoDevice_Mode_Server:
            // Begin advertising
            Esp32BleStartAdvertise(&bleContext);
            BLE_DEBUG_PRINTF("Server advertise started\n");
            break;

        case BluetoothNanoDevice_Mode_Client:
            BLE_DEBUG_PRINTF("Client mode started\n");

        default:
            break;
    }

    // Signal stack started
    xEventGroupSetBits(ble_event_waitgroup, N_BLE_EVENT_STARTED);
}

static void Esp32BleOnReset(int reason)
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

void Device_ble_dispose()
{
    BLE_DEBUG_PRINTF("Device_ble_dispose\n");

    int rc = nimble_port_stop();
    if (rc == 0 || rc == 2)
    {
        nimble_port_deinit();

        rc = esp_nimble_hci_and_controller_deinit();
        if (rc != ESP_OK)
        {
            ESP_LOGE(tag, "esp_nimble_hci_and_controller_deinit() failed with error: %d", rc);
        }
    }
    else
    {
        BLE_DEBUG_PRINTF("nimble_port_stop failed %d\n", rc);
    }

    BLE_DEBUG_PRINTF("delete mutexs\n");
    vEventGroupDelete(ble_event_waitgroup);
    vSemaphoreDelete(ble_event_data.mutex);

    ble_initialized = false;

    BLE_DEBUG_PRINTF("Device_ble_dispose exit %d\n", ble_initialized);
}

bool DeviceBleInit()
{
    esp_err_t err;

    BLE_DEBUG_PRINTF("DeviceBleInit %d\n", ble_initialized);

    // If already initialized then dispose first
    // This can happen when you start debugger
    if (ble_initialized)
    {
        BLE_DEBUG_PRINTF("ble_initialized true\n");
        Device_ble_dispose();
    }

    //    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());
    err = esp_nimble_hci_and_controller_init();
    if (err != ESP_OK)
    {
        BLE_DEBUG_PRINTF("esp_nimble_hci_and_controller_init failed %d\n", err);
        return false;
    }

    ble_event_waitgroup = xEventGroupCreate();
    ble_event_data.mutex = xSemaphoreCreateMutex();

    nimble_port_init();

    // Initialize the NimBLE host configuration
    ble_hs_cfg.sync_cb = Esp32BleOnSync;
    ble_hs_cfg.reset_cb = Esp32BleOnReset;

    ble_initialized = true;

    return true;
}

void StartBleTask(char *deviceName)
{
    int rc;

    BLE_DEBUG_PRINTF("StartBleTask\n");

    // Set the default device name
    rc = ble_svc_gap_device_name_set(deviceName);
    assert(rc == 0);

    // Start the BLE task
    nimble_port_freertos_init(esp32_ble_host_task);
}

int DeviceBleStart(bleServicesContext &con)
{
    int rc;
    ble_gatt_svc_def *gatt_svr_svcs = con.gatt_service_def;

    BLE_DEBUG_PRINTF("DeviceBleStart\n");

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0)
    {
        BLE_DEBUG_PRINTF("ble_gatts_count_cfg failed %d\n", rc);
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0)
    {
        BLE_DEBUG_PRINTF("ble_gatts_add_svcs failed %d\n", rc);
        return rc;
    }

    // Set device name & start BLE task
    StartBleTask(con.pDeviceName);

    return 0;
}

//
//  Callback from Nimble for characteristic events
//  These are posted to Managed code and if not handled within 500ms an error is reported to Nimble back to client
//  This could be due to no event handler set up in managed code or just taking too long.
//
int DeviceBleCallback(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (LockEventMutex())
    {
        // Save event context
        ble_event_data.eventId = ble_event_next_id++;
        ble_event_data.conn_handle = conn_handle;
        ble_event_data.characteristicId = (uint16_t)(int)arg;
        ble_event_data.ctxt = ctxt;

        ReleaseEventMutex();

        BluetoothEventType op;

        BLE_DEBUG_PRINTF(
            "DeviceBleCallback attr %d op %d id %X\n",
            attr_handle,
            ctxt->op,
            ble_event_data.characteristicId);

        switch (ctxt->op)
        {
            case BLE_GATT_ACCESS_OP_READ_CHR:
            case BLE_GATT_ACCESS_OP_READ_DSC:
                op = BluetoothEventType_Read;
                break;

            case BLE_GATT_ACCESS_OP_WRITE_CHR:
            case BLE_GATT_ACCESS_OP_WRITE_DSC:
                op = BluetoothEventType_Write;
                break;

            default:
                return BLE_ATT_ERR_REQ_NOT_SUPPORTED;
        }

        // Post event to managed code and wait upto 1 sec for it to be handled by managed code via callback
        if (PostAndWaitManagedGapEvent(op, ble_event_data.characteristicId, ble_event_data.eventId))
        {
            // Event handled in managed code
            return ble_event_data.result;
        }
    }
    // Event not handled in manged code, timeout
    // Return bluetooth error BLE_ATT_ERR_UNLIKELY
    return BLE_ATT_ERR_UNLIKELY;
}
