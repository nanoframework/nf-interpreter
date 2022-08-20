// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

#define connectTimeout 7000
#define asyncTimeout   20000

#define GattDeviceService                                                                                              \
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattDeviceService
#define GattCharacteristic                                                                                             \
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattCharacteristic
#define GattDescriptor Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattDescriptor

enum centralOp
{
    connect,
    discoverServices,
    discoverCharacteristics
};

HRESULT asyncCentralCall(centralOp op, CLR_RT_StackFrame &stack);
extern bool PostAndWaitManagedGapEvent(uint8_t op, uint16_t data1, uint32_t data2);

struct central_context : public HAL_DblLinkedNode<central_context>
{
    uint16_t connectionHandle;
    uint16_t serviceHandle;
    uint16_t characteristicId;

    centralOp op;

    int status;
    void *data;

    // Incoming
    void *eventData;
};

// Active device connection list
static bool ConnectionList_inited = false;
static HAL_DblLinkedList<central_context> ConnectionList;

// Find active connection and return context ptr
central_context *GetContext(ushort connection)
{
    central_context *ptr = ConnectionList.FirstNode();

    while (ptr->Next() != NULL)
    {
        if (ptr->connectionHandle == connection)
        {
            return ptr;
        }

        ptr = ptr->Next();
    }

    return NULL;
}

// Add connection by device connection(peer) handle to list
central_context *AddConnection(ushort connection)
{
    central_context *ptr = (central_context *)platform_malloc(sizeof(central_context));
    memset(ptr, 0, sizeof(central_context));

    ptr->connectionHandle = connection;
    ConnectionList.LinkAtBack(ptr);

    return ptr;
}

// Terminate connection and remove from list
void RemoveConnection(ushort connection)
{
    BLE_DEBUG_PRINTF("RemoveConnection %d\n", connection);
    central_context *ptr = GetContext(connection);
    if (ptr)
    {
        // Remove from connection list
        ptr->Unlink();
        platform_free(ptr);
    }
    else
    {
        BLE_DEBUG_PRINTF("Connection not found %d\n", connection);
    }
}

void TerminateConnection(ushort connection)
{
    BLE_DEBUG_PRINTF("ble_gap_terminate %d\n", connection);
    int rc = ble_gap_terminate(connection, BLE_ERR_REM_USER_CONN_TERM);
    if (rc)
    {
        BLE_DEBUG_PRINTF("ble_gap_terminate failed %d\n", rc);
    }
}

// Return number of active connections
int ConnectionCount()
{
    return ConnectionList.NumOfNodes();
}

//
//  Post Central event to managed code and continue.
///
void PostCentralEvent(
    uint8_t op,
    uint16_t conn_handle,
    uint16_t status,
    uint16_t serviceHandle,
    uint16_t characteristicHandle)
{
    uint16_t data1 = conn_handle;
    uint32_t data2 = (serviceHandle << 24) + (characteristicHandle << 8) + (status & 0xff);

    PostManagedEvent(EVENT_BLUETOOTH, op, data1, data2);
}

//
//  Post an event to managed code and wait for a callback to pick up data.
//
bool PostAndWaitCentralEvent(
    uint8_t op,
    uint16_t conn_handle,
    uint16_t status,
    uint16_t serviceHandle,
    uint16_t characteristicHandle)
{
    EventBits_t uxBits;

    BLE_DEBUG_PRINTF("PostManagedEvent->\n");

    // Make sure group clear before we post event
    xEventGroupClearBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);

    // Post central event to managed code
    PostCentralEvent(op, conn_handle, status, serviceHandle, characteristicHandle);

    // Wait for 1 second for event to be handled in managed code otherwise fail request
    uxBits = xEventGroupWaitBits(
        ble_event_waitgroup,
        N_BLE_EVENT_HANDLED,
        pdTRUE,
        pdFALSE,
        (TickType_t)(2000 / portTICK_PERIOD_MS));
    if (uxBits & N_BLE_EVENT_HANDLED)
    {
        // Handled
        return true;
    }

    // Not handled
    return false;
}

int CentralGapEvent(struct ble_gap_event *event, void *arg)
{

    central_context *con = (central_context *)arg;
    uint16_t conn_handle;

    BLE_DEBUG_PRINTF("CentralGapEvent type %d \n", event->type);

    switch (event->type)
    {
        case BLE_GAP_EVENT_CONNECT:

            conn_handle = event->connect.conn_handle;

            //  New connection established/failed
            BLE_DEBUG_PRINTF(
                "BLE_GAP_EVENT_CONNECT route:%X status:%X handle:%X\n",
                con->connectionHandle,
                event->connect.status,
                conn_handle);

            // Complete connect using temporary connection  handle saved in context, passing in real connection Handle
            // on Characteristic handle param
            PostCentralEvent(
                BluetoothEventType_ConnectComplete,
                con->connectionHandle,
                event->connect.status,
                0,
                conn_handle);

            if (event->connect.status == 0)
            {
                // Update context with correct connection handle
                con->connectionHandle = conn_handle;
            }
            else
            {
                // Connection failed
                // Remove context with temporary connection handle
                RemoveConnection(con->connectionHandle);
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            conn_handle = event->disconnect.conn.conn_handle;

            // Connection has disconnected
            BLE_DEBUG_PRINTF("BLE_GAP_EVENT_DISCONNECT handle:%X status:%X\n", conn_handle, event->connect.status);

            // connection has disconnected, inform managed code
            PostCentralEvent(
                BluetoothEventType_ConnectionDisconnected,
                conn_handle,
                event->connect.status,
                0,
                conn_handle);
            break;

        case BLE_GAP_EVENT_NOTIFY_RX:
        {
            // Device sent us a notification or indication.
            BLE_DEBUG_PRINTF(
                "Received %s; conn_handle=%d attr_handle=%d "
                "attr_len=%d\n",
                event->notify_rx.indication ? "indication" : "notification",
                event->notify_rx.conn_handle,
                event->notify_rx.attr_handle,
                OS_MBUF_PKTLEN(event->notify_rx.om));

            // Attribute data is contained in event->notify_rx
            // Save event for managed code callback
            con->eventData = (void *)event;

            // Post value changed event, managed code can read new value
            // pass notify / indicate flag as status
            PostAndWaitCentralEvent(
                BluetoothEventType_AttributeValueChanged,
                event->notify_rx.conn_handle,
                event->notify_rx.indication,
                0,
                event->notify_rx.attr_handle);
        }
        break;

        default:
            break;
    }

    return 0;
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattCharacteristic::
    NativeReadEventData___SZARRAY_U1__U2__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint16_t conn_handle = stack.Arg1().NumericByRef().u2;

        central_context *con = GetContext(conn_handle);
        FAULT_ON_NULL(con);

        BLE_DEBUG_PRINTF("NativeReadEventData con:%d  \n", conn_handle);

        ble_gap_event *event = (ble_gap_event *)con->eventData;

        NANOCLR_CHECK_HRESULT(OmBufferToStack(stack, event->notify_rx.om));

        BLE_DEBUG_PRINTF("NativeReadEventData ok\n");

        // Signal BLE callback, event complete
        xEventGroupSetBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::
    NativeConnect___U2__U8__nanoFrameworkDeviceBluetoothBluetoothAddressType__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int rc;
        central_context *con;

        // get a pointer to the managed object instance
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        if (pThis[Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::FIELD___disposed]
                .NumericByRef()
                .u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // If BLE stack not started then start it
        if (!ble_hs_is_enabled())
        {
            // Initialise BLE stack
            DeviceBleInit();
            StartBleTask(bleDeviceName);

            // Wait to be ready
            while (ble_hs_is_enabled() == 0)
            {
                vTaskDelay(100);
            }
        }

        // Init linked list on first connect
        if (!ConnectionList_inited)
        {
            ConnectionList.Initialize();
            ConnectionList_inited = true;
        }

        // Get Params
        u64_t bleAddress = stack.Arg1().NumericByRef().u8;

        BluetoothAddressType bleAddressType = (BluetoothAddressType)stack.Arg2().NumericByRef().u2;

        // We use the eventrouting for routing the event to correct BluetoothLEDevice object during Connection
        // Once connected we use the connection handle.
        uint16_t eventRouting = stack.Arg3().NumericByRef().u2;

        BLE_DEBUG_PRINTF("NativeConnect %X %d %X\n", bleAddress, bleAddressType, eventRouting);

        uint8_t own_addr_type = BLE_OWN_ADDR_PUBLIC;

        rc = ble_hs_id_infer_auto(0, &own_addr_type);
        if (rc != 0)
        {
            BLE_DEBUG_PRINTF("ble_hs_id_infer_auto %X\n", rc);
            goto exit;
        }

        ble_addr_t peerAddr;

        UlongToBleAddress(bleAddress, (uint8_t)bleAddressType, peerAddr);

        // We use a temporary connection handle for making the connection
        // if connection successful we update connection handle, unsuccessful we remove context.
        con = AddConnection(eventRouting);
        FAULT_ON_NULL(con);

        // Fill in details for Gap event to use.
        con->connectionHandle = eventRouting;

        rc = ble_gap_connect(own_addr_type, &peerAddr, connectTimeout, NULL, CentralGapEvent, con);
        BLE_DEBUG_PRINTF("ble_gap_connect exit %X\n", rc);

        if (rc == BLE_HS_EDONE)
        {
            BLE_DEBUG_PRINTF("ble_gap_connect already connected\n");

            // Already connected ?
            ble_gap_conn_desc desc;
            if (ble_gap_conn_find_by_addr(&peerAddr, &desc) == 0)
            {
                BLE_DEBUG_PRINTF("ble_gap_connect post event\n");

                // Complete connect using dummy connection handle, passing in connection Handle
                PostCentralEvent(BluetoothEventType_ConnectComplete, eventRouting, 0, 0, desc.conn_handle);
                con->connectionHandle = desc.conn_handle;
                rc = 0;
            }
            else
            {
                // connection not found so, make it an error instead
                rc = BLE_HS_ENOTCONN;
            }
        }

    exit:
        stack.SetResult_U2(rc);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::NativeDisconnect___VOID__U2(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        ushort conn_Handle = stack.Arg1().NumericByRef().u2;

        BLE_DEBUG_PRINTF("NativeDisconnect ble_gap_terminate handle:%x\n", conn_Handle);

        // Start a terminate, this will fire a BLE_GAP_EVENT_DISCONNECT event when done
        int rc = ble_gap_terminate(conn_Handle, BLE_ERR_REM_USER_CONN_TERM);
        if (rc)
        {
            BLE_DEBUG_PRINTF("ble_gap_terminate failed %d\n", rc);

            // Terminate failed, already terminated so post disconnected event now to managed code
            PostCentralEvent(BluetoothEventType_ConnectionDisconnected, conn_Handle, rc, 0, conn_Handle);
        }
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::NativeDispose___VOID__U2(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        ushort conn_Handle = stack.Arg1().NumericByRef().u2;

        BLE_DEBUG_PRINTF("NativeDispose handle:%x\n", conn_Handle);

        RemoveConnection(conn_Handle);

        // If last connection then stop ble stack
        if (ConnectionCount() == 0)
        {
            BLE_DEBUG_PRINTF("NativeDispose last connection:%x\n", conn_Handle);
            Device_ble_dispose();
        }
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
// ======== Services ================

//
// Call back when discovering services on a connection
//
int DiscoverServicesCb(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
    const struct ble_gatt_svc *service,
    void *arg)
{
    central_context *con;

    con = (central_context *)arg;
    assert(con->connectionHandle == conn_handle);

    BLE_DEBUG_PRINTF("Info: DiscoverServicesCb error:%x  att:%d \n", error->status, error->att_handle);

    con->status = error->status;

    if (error->status == 0)
    {
        // Service discovered, save it to managed object via event
        // Save service data for processing
        con->data = (void *)service;

        ble_gatt_svc *ptr = (ble_gatt_svc *)con->data;

        BLE_DEBUG_PRINTF("Info: Update service start %d end %d\n", ptr->start_handle, ptr->end_handle);

        // Post Discovery managed event and wait for callback from managed code via NativeUpdateService method
        if (!PostAndWaitCentralEvent(
                BluetoothEventType_ServiceDiscovered,
                conn_handle,
                error->status,
                ptr->start_handle,
                0))
        {
            BLE_DEBUG_PRINTF("Error: PostAndWaitCentralEvent BluetoothEventType_ServiceDiscovered post failed !!!\n");
        }
    }
    else
    {
        // Complete / error
        PostCentralEvent(BluetoothEventType_ServiceDiscoveryComplete, conn_handle, error->status, 0, 0);
    }

    return 0;
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::NativeDiscoverServices___U2__U2(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        ushort conn_Handle = stack.Arg1().NumericByRef().u2;

        central_context *con = GetContext(conn_Handle);
        FAULT_ON_NULL(con);

        int rc = ble_gattc_disc_all_svcs(conn_Handle, DiscoverServicesCb, (void *)con);

        BLE_DEBUG_PRINTF("ble_gattc_disc_all_svcs %x\n", rc);

        stack.SetResult_U2(rc);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::
    NativeUpdateService___VOID__U2__nanoFrameworkDeviceBluetoothGenericAttributeProfileGattDeviceService(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        central_context *con;
        BLE_DEBUG_PRINTF("NativeUpdateService\n");

        // get a pointer to the managed object instance of BlueToothLEDevcie and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Check not disposed
        if (pThis[Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::FIELD___disposed]
                .NumericByRef()
                .u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        ushort con_handle = stack.Arg1().NumericByRef().u2;

        CLR_RT_HeapBlock *pGattDeviceService = stack.Arg2().Dereference();
        FAULT_ON_NULL(pGattDeviceService);

        con = GetContext(con_handle);
        FAULT_ON_NULL(con);

        BLE_DEBUG_PRINTF("NativeUpdateService connection handle %d\n", con_handle);

        ble_gatt_svc *service = (ble_gatt_svc *)con->data;

        // Update UUID
        CLR_RT_HeapBlock_Array *srvUuidArray = pGattDeviceService[GattDeviceService::FIELD___uuid].Array();
        uint8_t *guid = srvUuidArray->GetFirstElement();
        NimbleUUIDToGuid(&service->uuid, guid);

        // Characteristic start & end handles for requesting characteristic details
        pGattDeviceService[GattDeviceService::FIELD___startHandle].NumericByRef().u2 = service->start_handle;
        pGattDeviceService[GattDeviceService::FIELD___endHandle].NumericByRef().u2 = service->end_handle;

        BLE_DEBUG_PRINTF("NativeUpdateService update complete\n");

        // Signal BLE callback, event complete
        xEventGroupSetBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);
    }
    NANOCLR_NOCLEANUP();
}

// ======== Characteristics ================

//
//  Callback when discovering Characteristics
//
static int DiscoverChrCb(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
    const struct ble_gatt_chr *chr,
    void *arg)
{
    central_context *con;

    con = (central_context *)arg;
    assert(con->connectionHandle == conn_handle);

    BLE_DEBUG_PRINTF("DiscoverChrCb handle:%X sid:%d error:%d \n", conn_handle, con->serviceHandle, error->status);

    con->status = error->status;

    if (error->status == 0)
    {
        // Characteristic discovered for this service
        con->data = (ble_gatt_chr *)chr;
        con->characteristicId = chr->val_handle;

        // Post Discovery managed event and wait for callback from managed code
        BLE_DEBUG_PRINTF(
            "DiscoverChrCb call post CH:%X def handle %d val handle %d\n",
            conn_handle,
            chr->def_handle,
            chr->val_handle);

        if (!PostAndWaitCentralEvent(
                BluetoothEventType_CharacteristicDiscovered,
                conn_handle,
                error->status,
                con->serviceHandle,
                chr->val_handle))
        {
            BLE_DEBUG_PRINTF("Error: BluetoothEventType_CharacteristicDiscovered post failed\n");
        }
    }
    else
    {
        /// Characteristic discovery complete
        BLE_DEBUG_PRINTF("BluetoothEventType_CharacteristicDiscoveryComplete call post \n");

        // Complete / error
        PostCentralEvent(
            BluetoothEventType_CharacteristicDiscoveryComplete,
            conn_handle,
            error->status,
            con->serviceHandle,
            0);
    }

    return 0;
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattDeviceService::
    NativeDiscoverCharacteristics___U2__U2__U2__U2__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int rc;

        ushort conn_handle = stack.Arg1().NumericByRef().u2;
        ushort serviceHandle = stack.Arg2().NumericByRef().u2;
        ushort startHandle = stack.Arg3().NumericByRef().u2;
        ushort endHandle = stack.Arg4().NumericByRef().u2;

        BLE_DEBUG_PRINTF("NativeDiscoverCharacteristics %X %d\n", conn_handle, serviceHandle);

        central_context *con = GetContext(conn_handle);
        FAULT_ON_NULL(con);

        con->connectionHandle = conn_handle;
        con->serviceHandle = serviceHandle;

        rc = ble_gattc_disc_all_chrs(conn_handle, startHandle, endHandle, DiscoverChrCb, (void *)con);
        if (rc != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        stack.SetResult_U2(rc);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattDeviceService::
    NativeUpdateCharacteristic___VOID__U2__U2__U2__nanoFrameworkDeviceBluetoothGenericAttributeProfileGattCharacteristic(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance of GattDeviceService and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        ushort conn_handle = stack.Arg1().NumericByRef().u2;
        ushort serviceHandle = stack.Arg2().NumericByRef().u2;
        ushort charId = stack.Arg3().NumericByRef().u2;

        central_context *con = GetContext(conn_handle);
        FAULT_ON_NULL(con);

        CLR_RT_HeapBlock *pGattCharacteristic = stack.Arg4().Dereference();
        FAULT_ON_NULL(pGattCharacteristic);

        BLE_DEBUG_PRINTF("NativeUpdateCharacteristic con %d srvid %d charId %d\n", conn_handle, serviceHandle, charId);

        // Check context valid for call
        if (con->serviceHandle != serviceHandle && con->characteristicId != charId)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        ble_gatt_chr *characteristic = (ble_gatt_chr *)con->data;

        // Characteristic properties
        // The values for BLE_GATT_CHR_PROP_? are the same as managed code GattCharacteristicProperties_? properties
        // so no need to convert
        pGattCharacteristic[GattCharacteristic::FIELD___characteristicProperties].NumericByRef().u4 =
            (int)characteristic->properties;

        // Update UUID
        CLR_RT_HeapBlock_Array *chrUuidArray = pGattCharacteristic[GattCharacteristic::FIELD___uuid].Array();
        uint8_t *guid = chrUuidArray->GetFirstElement();
        NimbleUUIDToGuid(&characteristic->uuid, guid);

        BLE_DEBUG_PRINTF("NativeUpdateCharacteristic update complete\n");

        // Signal BLE callback, event complete
        xEventGroupSetBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);
    }
    NANOCLR_NOCLEANUP();
}

static int ReadCharCallback(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
    struct ble_gatt_attr *attr,
    void *arg)
{
    central_context *con = (central_context *)arg;

    if (error->status == 0)
    {
        BLE_DEBUG_PRINTF(
            "Read complete; status=%d conn_handle=%d handle:%d\n",
            error->status,
            conn_handle,
            attr->handle);
    }
    else
    {
        BLE_DEBUG_PRINTF("Read complete; status=%d conn_handle=%d\n", error->status, conn_handle);
    }

    // Save ble_gatt_attr for use in NativeReadValue
    con->data = attr;
    con->status = error->status;

    if (error->status == 0)
    {
        BLE_DEBUG_PRINTF("data length:%d\n", OS_MBUF_PKTLEN(attr->om));

        // Post event and wait for it to be handled
        if (PostAndWaitCentralEvent(
                BluetoothEventType_AttributeReadValueComplete,
                conn_handle,
                error->status,
                con->serviceHandle,
                attr->handle))
        {
            // TODO ?
            BLE_DEBUG_PRINTF("Read complete handled\n");
        }
    }
    else
    {
        PostCentralEvent(
            BluetoothEventType_AttributeReadValueComplete,
            conn_handle,
            error->status,
            con->serviceHandle,
            0);
    }

    return 0;
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::NativeStartReadValue___U2__U2__U2(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int rc;
        uint16_t conn_handle = stack.Arg1().NumericByRef().u2;
        uint16_t val_handle = stack.Arg2().NumericByRef().u2;

        central_context *con = GetContext(conn_handle);
        if (con == NULL)
        {
            rc = BLE_HS_ENOTCONN;
        }
        else
        {
            con->connectionHandle = conn_handle;
            con->characteristicId = val_handle;

            BLE_DEBUG_PRINTF("NativeStartReadValue con:%d  val handle:%d\n", conn_handle, val_handle);

            rc = ble_gattc_read(conn_handle, val_handle, ReadCharCallback, (void *)con);
            if (rc != 0)
            {
                BLE_DEBUG_PRINTF("Error: Failed to read characteristic; rc=%d\n", rc);
            }
        }

        stack.SetResult_U2(rc);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::
    NativeReadValue___SZARRAY_U1__U2__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint16_t conn_handle = stack.Arg1().NumericByRef().u2;
        uint16_t val_handle = stack.Arg2().NumericByRef().u2;

        central_context *con = GetContext(conn_handle);
        FAULT_ON_NULL(con);

        BLE_DEBUG_PRINTF("NativeReadValue con:%d val handle:%d eventid:%d\n", conn_handle, val_handle);

        if (con->characteristicId != val_handle)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        ble_gatt_attr *attr = (ble_gatt_attr *)con->data;

        NANOCLR_CHECK_HRESULT(OmBufferToStack(stack, attr->om));

        BLE_DEBUG_PRINTF("NativeReadValue ok\n");

        // Signal BLE callback, event complete
        xEventGroupSetBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);
    }
    NANOCLR_NOCLEANUP();
}

// Callback for writes with response
static int WriteCharWithResponseCallback(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
    struct ble_gatt_attr *attr,
    void *arg)
{

    central_context *con = (central_context *)arg;

    BLE_DEBUG_PRINTF(
        "Write complete; status=%d conn_handle=%d attr_handle=%d\n",
        error->status,
        conn_handle,
        attr->handle);

    PostCentralEvent(
        BluetoothEventType_AttributeWriteValueComplete,
        conn_handle,
        error->status,
        con->serviceHandle,
        attr->handle);

    return 0;
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice::
    NativeStartWriteValue___U2__U2__U2__BOOLEAN__SZARRAY_U1__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *heapBuffer;
        uint8_t *bufferPtr;
        uint16_t bufferLen;
        int rc;

        // Get arguments
        uint16_t conn_handle = stack.Arg1().NumericByRef().u2;
        uint16_t val_handle = stack.Arg2().NumericByRef().u2;
        bool withResponse = stack.Arg3().NumericByRef().u4;
        heapBuffer = stack.Arg4().DereferenceArray();
        bufferLen = stack.Arg5().NumericByRef().u2;

        FAULT_ON_NULL(heapBuffer);

        // Get ptr to byte buffer
        bufferPtr = heapBuffer->GetFirstElement();

        central_context *con = GetContext(conn_handle);
        if (con == NULL)
        {
            rc = BLE_HS_ENOTCONN;
        }
        else
        {
            BLE_DEBUG_PRINTF(
                "NativeStartWriteValue con:%d val handle:%d data len:%d\n",
                conn_handle,
                val_handle,
                bufferLen);

            if (withResponse)
            {
                // With response ,callback contains status
                con->characteristicId = val_handle;

                rc = ble_gattc_write_flat(
                    conn_handle,
                    val_handle,
                    bufferPtr,
                    bufferLen,
                    WriteCharWithResponseCallback,
                    (void *)con);
            }
            else
            {
                // No response so no callback
                rc = ble_gattc_write_no_rsp_flat(conn_handle, val_handle, bufferPtr, bufferLen);
            }
        }

        if (rc != 0)
        {
            BLE_DEBUG_PRINTF("NativeStartWriteValue failed write rc:%dd\n", rc);
        }

        stack.SetResult_U2(rc);
    }
    NANOCLR_NOCLEANUP();
}

// ======== Descriptors ================

static int DescriptorDiscoveryCallback(
    uint16_t conn_handle,
    const struct ble_gatt_error *error,
    uint16_t chr_val_handle,
    const struct ble_gatt_dsc *dsc,
    void *arg)
{
    central_context *con;

    con = (central_context *)arg;
    assert(con->connectionHandle == conn_handle);

    BLE_DEBUG_PRINTF("discoveredDescriptorCb handle:%X error:%d \n", conn_handle, error->status);

    con->status = error->status;

    if (error->status == 0)
    {
        // Characteristic discovered for this service
        con->data = (ble_gatt_dsc *)dsc;
        con->characteristicId = chr_val_handle;

        // Post Discovery managed event and wait for callback from managed code
        BLE_DEBUG_PRINTF("discoveredDescriptorCb call post CH:%X val handle %d\n", conn_handle, chr_val_handle);

        if (!PostAndWaitCentralEvent(
                BluetoothEventType_DescriptorDiscovered,
                conn_handle,
                error->status,
                con->serviceHandle,
                chr_val_handle))
        {
            BLE_DEBUG_PRINTF("Error: BluetoothEventType_DescriptorDiscovered post failed\n");
        }
    }
    else
    {
        /// Descriptor discovery complete
        BLE_DEBUG_PRINTF("BluetoothEventType_DescriptorDiscoveryComplete call post attribute %d\n", chr_val_handle);

        // Complete / error
        PostCentralEvent(
            BluetoothEventType_DescriptorDiscoveryComplete,
            conn_handle,
            error->status,
            con->serviceHandle,
            chr_val_handle);
    }

    return 0;
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattCharacteristic::
    NativeStartDiscoveryDescriptors___U2__U2__U2__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        ushort conn_handle = stack.Arg1().NumericByRef().u2;
        ushort start_handle = stack.Arg2().NumericByRef().u2;
        ushort end_handle = stack.Arg3().NumericByRef().u2;

        central_context *con = GetContext(conn_handle);
        FAULT_ON_NULL(con);

        int rc =
            ble_gattc_disc_all_dscs(conn_handle, start_handle, end_handle, DescriptorDiscoveryCallback, (void *)con);

        BLE_DEBUG_PRINTF(
            "NativeStartDiscoveryDescriptors con:%d start:%d end:%d rc:%d\n",
            conn_handle,
            start_handle,
            end_handle,
            rc);

        stack.SetResult_U2(rc);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattCharacteristic::
    NativeUpdateDescriptor___VOID__U2__U2__U2__nanoFrameworkDeviceBluetoothGenericAttributeProfileGattDescriptor(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        BLE_DEBUG_PRINTF("NativeUpdateDescriptor\n");

        ushort conn_handle = stack.Arg1().NumericByRef().u2;
        ushort serviceHandle = stack.Arg2().NumericByRef().u2;
        ushort charId = stack.Arg3().NumericByRef().u2;

        CLR_RT_HeapBlock *pGattDescriptor = stack.Arg4().Dereference();
        FAULT_ON_NULL(pGattDescriptor);

        central_context *con = GetContext(conn_handle);
        FAULT_ON_NULL(con);

        BLE_DEBUG_PRINTF("NativeUpdateDescriptor con %d srvid %d charId %d\n", conn_handle, serviceHandle, charId);

        // Check context valid for call
        if (con->serviceHandle != serviceHandle && con->characteristicId != charId)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        ble_gatt_dsc *dsc = (ble_gatt_dsc *)con->data;

        // Descriptor handle
        pGattDescriptor[GattDescriptor::FIELD___attributeHandle].NumericByRef().u2 = dsc->handle;

        // Update UUID
        CLR_RT_HeapBlock_Array *chrUuidArray = pGattDescriptor[GattDescriptor::FIELD___uuid].Array();
        uint8_t *guid = chrUuidArray->GetFirstElement();
        NimbleUUIDToGuid(&dsc->uuid, guid);

        // Signal BLE callback, event complete
        xEventGroupSetBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);
    }
    NANOCLR_NOCLEANUP();
}