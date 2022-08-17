//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"
//#include "ble_gap.h"

#define BluetoothLEAdvertisement                                                                                       \
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisement

bool bleScanActive = false;

//
// Initiates the GAP general discovery procedure.
//
void bleCentralStartScan()
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params;
    int rc;

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0)
    {
        BLE_DEBUG_PRINTF("error determining address type; rc=%d\n", rc);
        return;
    }

    // Tell the controller to filter duplicates
    disc_params.filter_duplicates = 1;

    // Perform an active or passive scan
    disc_params.passive = bleScanActive ? 0 : 1;

    // Use defaults for the rest of the parameters.
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params, esp32_gap_event, NULL);

    BLE_DEBUG_PRINTF("ble_gap_disc; reason=%d  active %d\n", rc, ble_gap_disc_active());

    if (rc != 0)
    {
        BLE_DEBUG_PRINTF("Error initiating GAP discovery procedure; rc=%d\n", rc);
    }
}

static void bleCentralCancelScan()
{
    if (ble_gap_disc_active() == 1)
    {
        ble_gap_disc_cancel();
    }
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEAdvertisementWatcher::
    NativeStartAdvertisementWatcher___VOID__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // Passive or Active scan modes ( default passive )
        bleScanActive = (stack.Arg1().NumericByRef().s4 == BluetoothLEScanningMode_Active);

        BLE_DEBUG_PRINTF("Start Advertisement Watcher - Mode %d\n", bleScanActive);

        // Initialise BLE stack
        device_ble_init();
        start_ble_task(bleDeviceName);

        // Note:  bleCentralStartScan will be called when stack is ready (sync event called)
        // see esp32_ble_on_sync()
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEAdvertisementWatcher::
    NativeStopAdvertisementWatcher___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        BLE_DEBUG_PRINTF("Stop Advertisement Watcher\n");

        bleCentralCancelScan();

        device_ble_dispose();
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT
Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisementReceivedEventArgs::
    NativeCreateFromEvent___BOOLEAN__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        bool result = false;
        CLR_UINT8 *buffer;
        int uuidIndex;

        // ptr to BluetoothLEAdvertisementReceivedEventArgs object
        CLR_RT_HeapBlock *pThis = stack.This();
        CLR_RT_HeapBlock *pAdvert;
        FAULT_ON_NULL(pThis);

        // Eventid
        CLR_INT32 eventID = stack.Arg1().NumericByRef().s4;

        // Get access to Event data
        if (LockEventMutex())
        {
            if (ble_event_data.eventId == eventID)
            {

                // Event ID correct
                int rc;
                ble_hs_adv_fields fields;
                struct ble_gap_event *gEvent = ble_event_data.gapEvent;

                rc = ble_hs_adv_parse_fields(&fields, gEvent->disc.data, gEvent->disc.length_data);
                if (rc == 0)
                {
                    // Fill in BluetoothLEAdvertisementReceivedEventArgs fields from event data
                    BLE_DEBUG_PRINTF("Watch rssi:%d type:%d adr:", gEvent->disc.rssi, gEvent->disc.addr.type);
                    PrintBytes(gEvent->disc.addr.val, 6);
                    BLE_DEBUG_PRINTF("\n");

                    pThis[FIELD___bluetoothAddress].NumericByRef().u8 = BleAddressToUlong(gEvent->disc.addr.val);
                    pThis[FIELD___advertisementType].NumericByRef().s4 = 4; // TODO
                    pThis[FIELD___rawSignalStrengthInDBm].NumericByRef().s2 = (int16_t)gEvent->disc.rssi;

                    // Get reference to BluetoothLEAdvertisement object
                    pAdvert = pThis[FIELD___advertisement].Dereference();

                    // Fill in BluetoothLEAdvertisement fields
                    // Set localname
                    if (fields.name != 0)
                    {
                        // Create a null terminated string
                        char *pname = (char *)platform_malloc(fields.name_len + 1);
                        memcpy(pname, fields.name, fields.name_len);
                        pname[fields.name_len] = 0;

                        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(
                            pAdvert[BluetoothLEAdvertisement::FIELD___localName],
                            (const char *)pname));

                        platform_free(pname);
                    }
                    BLE_DEBUG_PRINTF("local Name %X len %d", fields.name, fields.name_len);

                    // == Service UUIDs ====
                    // Create Array of all UUID 16bit, UUID 32 bit & UUID 128bit as 16 byte GUID in byte buffer
                    // Create array byte the correct size to hold all service UUID in advert.
                    uuidIndex = (fields.num_uuids16 + fields.num_uuids32 + fields.num_uuids128) * 16;

                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                        pAdvert[BluetoothLEAdvertisement::FIELD___rawUuids],
                        uuidIndex,
                        g_CLR_RT_WellKnownTypes.m_UInt8));
                    buffer = pAdvert[BluetoothLEAdvertisement::FIELD___rawUuids].DereferenceArray()->GetFirstElement();

                    // Add UUID 16 bit to buffer
                    uuidIndex = fields.num_uuids16;
                    ble_uuid16_t *uuid16 = (ble_uuid16_t *)fields.uuids16;
                    while (uuidIndex)
                    {
                        BLE_DEBUG_PRINTF("adv uuid16 %X \n", uuid16->value);
                        // Convert to 128 bit UUID (16 byte)
                        NimbleUUID16ToGuid(uuid16, buffer);

#if defined(NANO_BLE_DEBUG)
                        BLE_DEBUG_PRINTF("adv guuid\n");
                        for (int i = 0; i <= 15; i++)
                        {
                            BLE_DEBUG_PRINTF("%X ", buffer[i]);
                        }
                        BLE_DEBUG_PRINTF("\n");
#endif
                        // Next UUID 16
                        uuidIndex--;
                        uuid16++;
                        buffer += 16;
                    }

                    // Add UUID 32 bit to buffer
                    uuidIndex = fields.num_uuids32;
                    ble_uuid32_t *uuid32 = (ble_uuid32_t *)fields.uuids32;
                    while (uuidIndex)
                    {
                        BLE_DEBUG_PRINTF("adv uuid32 %X \n", uuid32->value);
                        // Convert to 128 bit UUID (16 byte)
                        NimbleUUID32ToGuid(uuid32, buffer);

                        BLE_DEBUG_PRINTF("adv guuid\n");
                        for (int i = 0; i <= 15; i++)
                        {
                            BLE_DEBUG_PRINTF("%X ", buffer[i]);
                        }
                        BLE_DEBUG_PRINTF("\n");

                        // Next UUID 32
                        uuidIndex--;
                        uuid32++;
                        buffer += 16;
                    }

                    // Add UUID 128 bit to buffer
                    uuidIndex = fields.num_uuids128;
                    ble_uuid128_t *uu128 = (ble_uuid128_t *)fields.uuids128;
                    while (uuidIndex)
                    {
                        BLE_DEBUG_PRINTF("adv uuid128 \n");

                        for (int i = 0; i <= 15; i++)
                        {
                            BLE_DEBUG_PRINTF("%X ", uu128->value[i]);
                        }
                        BLE_DEBUG_PRINTF("\n");

                        // Copy 128 bit UUID (16 byte)
                        NimbleUUID128ToGuid(uu128, buffer);

                        BLE_DEBUG_PRINTF("adv guuid\n");
                        for (int i = 0; i <= 15; i++)
                        {
                            BLE_DEBUG_PRINTF("%X ", buffer[i]);
                        }
                        BLE_DEBUG_PRINTF("\n");

                        // Next UUID 128
                        uu128++;
                        uuidIndex--;
                        buffer += 16;
                    }

                    // ==== Manufacturer Data ====
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                        pAdvert[BluetoothLEAdvertisement::FIELD___rawManufacturerData],
                        fields.mfg_data_len,
                        g_CLR_RT_WellKnownTypes.m_UInt8));

                    buffer = pAdvert[BluetoothLEAdvertisement::FIELD___rawManufacturerData]
                                 .DereferenceArray()
                                 ->GetFirstElement();
                    memcpy(buffer, fields.mfg_data, fields.mfg_data_len);

                    BLE_DEBUG_PRINTF("Manufacturer data %X len %d\n", fields.mfg_data, fields.mfg_data_len);

                    result = true;

                    // Signal BLE callback, event complete
                    xEventGroupSetBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);
                }
            }

            ReleaseEventMutex();
        }

        stack.SetResult_Boolean(result);
    }
    NANOCLR_NOCLEANUP();
}
