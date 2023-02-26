//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairing::NativeStartPair___U2__U2(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        BLE_DEBUG_PRINTF("NativeStartPair\n");

        ushort conn_handle = stack.Arg1().NumericByRef().u2;

        BLE_DEBUG_PRINTF(
            "NativeStartPair security iocaps:%d sc:%d mi:%d bd:%d ob:%d\n",
            ble_hs_cfg.sm_io_cap,
            ble_hs_cfg.sm_sc,
            ble_hs_cfg.sm_mitm,
            ble_hs_cfg.sm_bonding,
            ble_hs_cfg.sm_oob_data_flag);

        int rc = ble_gap_security_initiate(conn_handle);

        BLE_DEBUG_PRINTF("NativeStartPair conn:%d %d\n", conn_handle, rc);

        stack.SetResult_U2((ushort)rc);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairing::NativeSetPairAttributes___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This(); // ptr to DevicePairing
        FAULT_ON_NULL(pThis);

        DevicePairingIOCapabilities IOCaps =
            (DevicePairingIOCapabilities)
                pThis[Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairing::FIELD___ioCapabilities]
                    .NumericByRef()
                    .s4;
        DevicePairingProtectionLevel protectionLevel =
            (DevicePairingProtectionLevel)
                pThis[Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairing::FIELD___protectionLevel]
                    .NumericByRef()
                    .s4;
        bool allowBonding =
            pThis[Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairing::FIELD___bondingAllowed]
                .NumericByRef()
                .s4 != 0;
        bool allowOob =
            pThis[Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairing::FIELD___outOfBand]
                .NumericByRef()
                .s4 != 0;

        SetSecuritySettings(IOCaps, protectionLevel, allowBonding, allowOob);
    }
    NANOCLR_NOCLEANUP();
}

void AcceptHelper(CLR_RT_StackFrame &stack, bool passkey)
{
    struct ble_sm_io pkey = {0, 0};

    ushort conn_handle = stack.Arg1().NumericByRef().u2;
    pkey.action = DevicePairingKindsToPairingAction((DevicePairingKinds)stack.Arg2().NumericByRef().s4);
    uint32_t arg3 = stack.Arg3().NumericByRef().s4;

    if (passkey)
    {
        pkey.passkey = arg3;
    }
    else
    {
        pkey.numcmp_accept = (uint8_t)arg3;
    }

    int rc = ble_sm_inject_io(conn_handle, &pkey);

    BLE_DEBUG_PRINTF(
        "NativeAccept con=%d DevicePairingKinds=%d => action=%d rc=%d)\n",
        conn_handle,
        stack.Arg2().NumericByRef().s4,
        pkey.action,
        rc);
    if (passkey)
    {
        BLE_DEBUG_PRINTF("NativeAcceptPasskey passkey=%d)\n", pkey.passkey);
    }

    stack.SetResult_U2((ushort)rc);

    // Signal BLE callback, event complete
    xEventGroupSetBits(ble_event_waitgroup, N_BLE_EVENT_HANDLED);
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairingRequestedEventArgs::
    NativeAcceptYesNo___U2__U2__nanoFrameworkDeviceBluetoothDevicePairingKinds__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        AcceptHelper(stack, false);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairingRequestedEventArgs::
    NativeAcceptPasskey___U2__U2__nanoFrameworkDeviceBluetoothDevicePairingKinds__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        AcceptHelper(stack, true);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairingRequestedEventArgs::
    NativeAcceptCredentials___U2__U2__nanoFrameworkDeviceBluetoothDevicePairingKinds__SZARRAY_U1__SZARRAY_U1(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        struct ble_sm_io pkey = {0, 0};

        ushort conn_handle = stack.Arg1().NumericByRef().u2;
        pkey.action = stack.Arg2().NumericByRef().s4;

        int rc = ble_sm_inject_io(conn_handle, &pkey);

        BLE_DEBUG_PRINTF(
            "NativeAccept OBB con=%d action=%d passKey=%d rc=%d)\n",
            conn_handle,
            pkey.action,
            pkey.passkey,
            rc);

        stack.SetResult_U2((ushort)rc);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

// Static Bonding methods
HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Security_DeviceBonding::
    IsBonded___STATIC__BOOLEAN__U8(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int numPeers;
    int rc;
    int maxBonds = MYNEWT_VAL(BLE_STORE_MAX_BONDS);
    ble_addr_t peer_id_addrs[maxBonds];
    bool result = false;

    uint64_t myaddr = stack.Arg0().NumericByRef().u8;
    int mytype = stack.Arg1().NumericByRef().s4;

    if (!ble_hs_is_enabled())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    rc = ble_store_util_bonded_peers(peer_id_addrs, &numPeers, maxBonds);
    if (rc != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    for (int index = 0; index < numPeers; index++)
    {
        u64_t addr = BleAddressToUlong(peer_id_addrs[index].val);

        if (myaddr == addr && peer_id_addrs[index].type == mytype)
        {
            result = true;
            break;
        }
    }

    stack.SetResult_Boolean(result);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Security_DeviceBonding::DeleteAllBonds___STATIC__VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    if (!ble_hs_is_enabled())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    ble_store_clear();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Security_DeviceBonding::
    DeleteBondForPeer___STATIC__VOID__U8__nanoFrameworkDeviceBluetoothBluetoothAddressType(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    ble_addr_t bleDelAddr = {0, 0};
    int rc;

    uint64_t addr = stack.Arg0().NumericByRef().u8;
    UlongToBleAddress(addr, 0, bleDelAddr);

    if (!ble_hs_is_enabled())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    rc = ble_gap_unpair(&bleDelAddr);
    if (rc != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Security_DeviceBonding::Count___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int outCount = 0;

    if (!ble_hs_is_enabled())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    ble_store_util_count(BLE_STORE_OBJ_TYPE_PEER_SEC, &outCount);

    stack.SetResult_I4(outCount);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Security_DeviceBonding::
    GetBondInformationAt___STATIC__U8__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int numPeers;
    int rc;
    u64_t addr;
    int maxBonds = MYNEWT_VAL(BLE_STORE_MAX_BONDS);
    ble_addr_t peer_id_addrs[maxBonds];

    int index = stack.Arg0().NumericByRef().s4;

    if (!ble_hs_is_enabled())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    if (index < 0 || index >= maxBonds)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    rc = ble_store_util_bonded_peers(peer_id_addrs, &numPeers, maxBonds);
    if (rc != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    addr = BleAddressToUlong(peer_id_addrs[index].val);
    stack.SetResult_U8(addr);

    NANOCLR_NOCLEANUP();
}