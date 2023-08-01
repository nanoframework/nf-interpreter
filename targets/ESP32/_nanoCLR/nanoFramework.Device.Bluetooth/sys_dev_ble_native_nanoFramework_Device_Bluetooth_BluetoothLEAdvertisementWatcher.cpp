//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

#define BluetoothLEAdvertisement                                                                                       \
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisement

bool bleScanActive = false;

//
// Initiates the GAP general discovery procedure.
//
void BleCentralStartScan()
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params;
    int rc;

    // Figure out address to use while advertising (no privacy for now)
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

    disc_params.passive = 0;

    // Use defaults for the rest of the parameters.
    disc_params.itvl = 0;   // defaults to BLE_GAP_SCAN_FAST_INTERVAL_MIN(30ms)
    disc_params.window = 0; // defaults to BLE_GAP_SCAN_FAST_WINDOW(30ms)
    disc_params.filter_policy = BLE_HCI_SCAN_FILT_NO_WL;
    disc_params.limited = 0;

    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params, Esp32GapEvent, NULL);
    BLE_DEBUG_PRINTF("ble_gap_disc; reason=%d  active %d\n", rc, ble_gap_disc_active());
    if (rc != 0)
    {
        BLE_DEBUG_PRINTF("Error initiating GAP discovery procedure; rc=%d\n", rc);
    }
}

static void BleCentralCancelScan()
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

        // Start scan
        BleCentralStartScan();
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEAdvertisementWatcher::
    NativeStopAdvertisementWatcher___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        BLE_DEBUG_PRINTF("Stop Advertisement Watcher\n");

        BleCentralCancelScan();
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

        // ptr to BluetoothLEAdvertisementReceivedEventArgs object
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Eventid
        CLR_INT32 eventID = stack.Arg1().NumericByRef().s4;

        BLE_DEBUG_PRINTF(
            "NativeCreateFromEvent eventid::%d ble_event:%d event:%X\n",
            eventID,
            ble_event_data.eventId,
            ble_event_data.gapEvent);

        // Get access to Event data
        if (LockEventMutex())
        {
            // Event ID correct
            if (ble_event_data.eventId == eventID)
            {
                struct ble_gap_event *gEvent = ble_event_data.gapEvent;

                BLE_DEBUG_PRINTF("Advert data length:%d ", (int)gEvent->disc.length_data);
                PrintBytes((uint8_t *)gEvent->disc.data, gEvent->disc.length_data);
                BLE_DEBUG_PRINTF("\n");

                // Fill in BluetoothLEAdvertisementReceivedEventArgs fields from event data
                BLE_DEBUG_PRINTF("Watch rssi:%d type:%d adr:", gEvent->disc.rssi, gEvent->disc.addr.type);
                PrintBytes(gEvent->disc.addr.val, 6);
                BLE_DEBUG_PRINTF("\n");

                pThis[FIELD___bluetoothAddress].NumericByRef().u8 = BleAddressToUlong(gEvent->disc.addr.val);
                pThis[FIELD___bluetoothAddressType].NumericByRef().u1 = gEvent->disc.addr.type;

                pThis[FIELD___advertisementType].NumericByRef().s4 = (int)gEvent->disc.event_type;
                pThis[FIELD___rawSignalStrengthInDBm].NumericByRef().s2 = (int16_t)gEvent->disc.rssi;

                // Return raw advert data bytes in _rawAdvertData field
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                    pThis[FIELD___rawAdvertData],
                    gEvent->disc.length_data,
                    g_CLR_RT_WellKnownTypes.m_UInt8));

                buffer = pThis[FIELD___rawAdvertData].DereferenceArray()->GetFirstElement();
                memcpy(buffer, gEvent->disc.data, gEvent->disc.length_data);
                result = true;

                // Signal BLE callback, event complete
                xEventGroupSetBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);
            }

            ReleaseEventMutex();
        }

        stack.SetResult_Boolean(result);
    }
    NANOCLR_NOCLEANUP();
}