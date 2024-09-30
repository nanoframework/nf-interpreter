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

bool Esp32BleStartAdvertise(bleServicesContext *context);
void BleCentralStartScan();
extern "C" void ble_store_config_init();

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

    BLE_DEBUG_PRINTF("Esp32GapEvent type %d \n", event->type);

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
                    "BLE_GAP_EVENT_DISC event id:%d event:%X event type:%d adr:%X rssi:%d\n",
                    ble_event_data.eventId,
                    ble_event_data.gapEvent,
                    event->disc.event_type,
                    (unsigned int)event->disc.addr.val,
                    event->disc.rssi);

                ReleaseEventMutex();

                // Post Discovery event to managed code
                if (PostAndWaitManagedGapEvent(BluetoothEventType_AdvertisementDiscovered, 0, ble_event_data.eventId))
                {
                    // Handled
                    BLE_DEBUG_PRINTF("disc handled\n");
                }

                BLE_DEBUG_PRINTF("adv2 adr:%x rssi:%d\n", &event->disc.addr, event->disc.rssi);
            }

            return 0;

        //
        // Discovery Scan complete
        //
        case BLE_GAP_EVENT_DISC_COMPLETE:
            BLE_DEBUG_PRINTF("Discovery complete; reason=%d\n", event->disc_complete.reason);

            // Post event to managed code
            PostManagedEvent(EVENT_BLUETOOTH, BluetoothEventType_ScanningComplete, event->disc_complete.reason, 0);

            return 0;

        case BLE_GAP_EVENT_CONNECT:
            // A new connection was established or a connection attempt failed
            BLE_DEBUG_PRINTF(
                "Connection %s; status=%d  handle %d\n",
                event->connect.status == 0 ? "established" : "failed",
                event->connect.status,
                event->connect.conn_handle);

            if (event->connect.status == 0)
            {
                PostManagedEvent(
                    EVENT_BLUETOOTH,
                    BluetoothEventType_ClientConnected,
                    event->connect.conn_handle,
                    event->connect.status);
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            BLE_DEBUG_PRINTF("BLE_GAP_EVENT_DISCONNECT; reason=%d\n", event->disconnect.reason);

            PostManagedEvent(
                EVENT_BLUETOOTH,
                BluetoothEventType_ClientDisconnected,
                event->disconnect.conn.conn_handle,
                event->disconnect.reason);

            // Connection terminated; resume advertising
            Esp32BleStartAdvertise(con);
            break;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            BLE_DEBUG_PRINTF("BLE_GAP_EVENT_ADV_COMPLETE adv complete");
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
        {
            BluetoothEventType op = BluetoothEventType_Read;

            // Find characteristicId from attr_handle
            uint16_t characteristicId = FindIdFromHandle(con, event->subscribe.attr_handle);

            BLE_DEBUG_PRINTF(
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
            BLE_DEBUG_PRINTF(
                "BLE_GAP_EVENT_MTU mtu update event; conn_handle=%d channel id=%d mtu=%d\n",
                event->mtu.conn_handle,
                event->mtu.channel_id,
                event->mtu.value);

            PostManagedEvent(
                EVENT_BLUETOOTH,
                BluetoothEventType_ClientSessionChanged,
                event->mtu.conn_handle,
                (event->mtu.value << 16) + event->mtu.channel_id);

            break;

        case BLE_GAP_EVENT_ENC_CHANGE:
        {
            struct ble_gap_conn_desc desc;

            int rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
            if (rc != 0)
            {
                return BLE_ATT_ERR_INVALID_HANDLE;
            }

            uint16_t securityState = 0;
            securityState += desc.sec_state.encrypted ? 1 : 0;
            securityState += desc.sec_state.authenticated ? 2 : 0;
            securityState += desc.sec_state.bonded ? 4 : 0;

            DevicePairingResultStatus pairingStatus = MapNimbleErrorToStatus(event->enc_change.status);

            BLE_DEBUG_PRINTF(
                "Srv BLE_GAP_EVENT_ENC_CHANGE conn_handle=%d status=%d security=%X pairingStatus = %d\n",
                event->enc_change.conn_handle,
                event->enc_change.status,
                securityState,
                pairingStatus);

            PostManagedEvent(
                EVENT_BLUETOOTH,
                BluetoothEventType_AuthenticationComplete,
                event->enc_change.conn_handle,
                (securityState << 16) + pairingStatus);
        }
        break;

        case BLE_GAP_EVENT_PASSKEY_ACTION:
        {
            BLE_DEBUG_PRINTF(
                "Srv BLE_GAP_EVENT_PASSKEY_ACTION conn_handle=%d action=%d\n",
                event->passkey.conn_handle,
                event->passkey.params.action);

            if (event->passkey.params.action == BLE_SM_IOACT_NUMCMP)
            {
                // As BLE_SM_IOACT_NUMCMP action we use unique BluetoothEventType_PassKeyActions_numcmp so we can pass
                // pin instead of action
                if (PostAndWaitManagedGapEvent(
                        BluetoothEventType_PassKeyActionsNumericComparison,
                        event->passkey.conn_handle,
                        event->passkey.params.numcmp))
                {
                    BLE_DEBUG_PRINTF(" BluetoothEventType_PassKeyActions NUMCMP handled");
                    return 0;
                }
            }
            else
            {
                if (PostAndWaitManagedGapEvent(
                        BluetoothEventType_PassKeyActions,
                        event->passkey.conn_handle,
                        PairingActionToDevicePairingKinds(event->passkey.params.action) << 16))
                {
                    BLE_DEBUG_PRINTF(" BluetoothEventType_PassKeyActions handled");
                    return 0;
                }
            }

            return 1;
        }
        break;

        case BLE_GAP_EVENT_CONN_UPDATE:
        {
            BLE_DEBUG_PRINTF(
                "BLE_GAP_EVENT_CONN_UPDATE conn_handle=%d status=%d\n",
                event->conn_update.conn_handle,
                event->conn_update.status);
        }
        break;

        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        {
            // Peer updating connection params.
            BLE_DEBUG_PRINTF("BLE_GAP_EVENT_CONN_UPDATE_REQ conn_handle=%d\n", event->conn_update_req.conn_handle);
            BLE_DEBUG_PRINTF(
                "BLE_GAP_EVENT_CONN_UPDATE_REQ conn_handle=%d\n",
                "Min Interval: %d, Max Interval: %d, Timeout: %d, Latency: %d ",
                event->conn_update_req.peer_params->itvl_min,
                event->conn_update_req.peer_params->itvl_max,
                event->conn_update_req.peer_params->supervision_timeout,
                event->conn_update_req.peer_params->latency);
            // do nothing for now
        }
        break;

        case BLE_GAP_EVENT_REPEAT_PAIRING:
        {
            ble_gap_conn_desc out_desc;

            BLE_DEBUG_PRINTF(
                "BLE_GAP_EVENT_REPEAT_PAIRING conn_handle=%d cur_authenticated=%d\n",
                event->repeat_pairing.conn_handle,
                event->repeat_pairing.cur_authenticated);

            // For now just delete old bond so we can continue with pairing

            // Delete the old bond
            int rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &out_desc);
            if (rc != 0)
            {
                return BLE_GAP_REPEAT_PAIRING_IGNORE;
            }

            ble_store_util_delete_peer(&out_desc.peer_id_addr);

            // Host to continue with the pairing operation.
            return BLE_GAP_REPEAT_PAIRING_RETRY;
        }
    }

    return 0;
}

//
// Enables advertising with parameters from context.
//
bool Esp32BleStartAdvertise(bleServicesContext *context)
{
    int rc;
    bool result = true;
    struct ble_gap_adv_params adv_params;

    rc = ble_gap_adv_set_data((const uint8_t *)context->advertData, context->advertDataLen);
    if (rc != 0)
    {
        BLE_DEBUG_PRINTF("ble_gap_adv_set_data; rc=%d\n", rc);
    }

    if (context->scanResponseLen > 0)
    {
        rc = ble_gap_adv_rsp_set_data((const uint8_t *)context->scanResponse, context->scanResponseLen);
        if (rc != 0)
        {
            BLE_DEBUG_PRINTF("ble_gap_adv_rsp_set_data; rc=%d\n", rc);
        }
    }

    // Begin advertising
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.conn_mode = bleContext.isConnectable ? BLE_GAP_CONN_MODE_UND : BLE_GAP_CONN_MODE_NON;
    adv_params.disc_mode = bleContext.isDiscoverable ? BLE_GAP_DISC_MODE_GEN : BLE_GAP_DISC_MODE_NON;

    rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params, Esp32GapEvent, (void *)&bleContext);
    if (rc != 0)
    {
        BLE_DEBUG_PRINTF("error enabling advertisement; rc=%d\n", rc);
        result = false;
    }

    return result;
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
            BLE_DEBUG_PRINTF("Server mode started\n");
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
    }
    else
    {
        BLE_DEBUG_PRINTF("nimble_port_stop failed %d\n", rc);
    }

    BLE_DEBUG_PRINTF("delete event & mutexs\n");

    LockEventMutex();

    ble_event_data.eventId = 0; // block any further callbacks

    ReleaseEventMutex();

    if (ble_event_waitgroup)
    {
        vEventGroupDelete(ble_event_waitgroup);
    }
    if (ble_event_data.mutex)
    {
        vSemaphoreDelete(ble_event_data.mutex);
    }

    ble_event_waitgroup = NULL;
    ble_event_data.mutex = NULL;

    ble_initialized = false;

    BLE_DEBUG_PRINTF("Device_ble_dispose exit %d\n", ble_initialized);
}

void SetSecuritySettings(
    DevicePairingIOCapabilities IOCaps,
    DevicePairingProtectionLevel protectionLevel,
    bool allowBonding,
    bool allowOob)
{
    ble_hs_cfg.sm_io_cap = IOCaps;

    switch (protectionLevel)
    {
        case DevicePairingProtectionLevel_Encryption:
            ble_hs_cfg.sm_sc = 1;
            break;

        case DevicePairingProtectionLevel_EncryptionAndAuthentication:
            ble_hs_cfg.sm_sc = 1;
            ble_hs_cfg.sm_mitm = 1;
            break;

        default:
            ble_hs_cfg.sm_sc = 0;
            ble_hs_cfg.sm_mitm = 0;
            break;
    }

    ble_hs_cfg.sm_bonding = allowBonding ? 1 : 0;

    ble_hs_cfg.sm_oob_data_flag = allowOob ? 1 : 0;

    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    // Enable appropriate bit masks to make sure the keys are exchanged
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;

    BLE_DEBUG_PRINTF(
        "SetSecuritySettings sc=%d mitm=%d bonding %d our key %d their key %d\n",
        ble_hs_cfg.sm_sc,
        ble_hs_cfg.sm_mitm,
        ble_hs_cfg.sm_bonding,
        ble_hs_cfg.sm_our_key_dist,
        ble_hs_cfg.sm_their_key_dist);
}

bool DeviceBleInit()
{
    BLE_DEBUG_PRINTF("DeviceBleInit %d\n", ble_initialized);

    // If already initialized then dispose first
    // This can happen when you start debugger
    if (ble_initialized)
    {
        BLE_DEBUG_PRINTF("ble_initialized true\n");
        Device_ble_dispose();
    }

    ble_event_waitgroup = xEventGroupCreate();
    ble_event_data.mutex = xSemaphoreCreateMutex();

    if (!ble_event_waitgroup || !ble_event_data.mutex)
    {
        BLE_DEBUG_PRINTF("Ble Event Wait Group or mutex is null\n");
        return false;
    }

    nimble_port_init();

    // Initialize the NimBLE host configuration
    ble_hs_cfg.sync_cb = Esp32BleOnSync;
    ble_hs_cfg.reset_cb = Esp32BleOnReset;

    // Initialise default security/pairing settings
    SetSecuritySettings(DevicePairingIOCapabilities_NoInputNoOutput, DevicePairingProtectionLevel_Default, true, false);

    ble_store_config_init();

    ble_initialized = true;

    return true;
}

void StartBleTask(char *deviceName, uint16_t appearance)
{
    int rc;

    BLE_DEBUG_PRINTF("StartBleTask\n");

    // Set the default device name
    rc = ble_svc_gap_device_name_set(deviceName);
    assert(rc == 0);

    rc = ble_svc_gap_device_appearance_set(appearance);
    assert(rc == 0);
    BLE_DEBUG_PRINTF("ble_svc_gap_device_appearance_set %X\n", appearance);

    // Start the BLE task
    nimble_port_freertos_init(esp32_ble_host_task);
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
