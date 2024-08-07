//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NIMBLE_UTIL_H
#define NIMBLE_UTIL_H

// Uncomment to enable BLE diagnostic debug messages
//#define NANO_BLE_DEBUG

#if defined(NANO_BLE_DEBUG)
#define BLE_DEBUG_PRINTF(format, ...)                                                                                  \
    {                                                                                                                  \
        debug_printf(format, ##__VA_ARGS__);                                                                           \
    }
#else
#define BLE_DEBUG_PRINTF(format, ...) ;
#endif

void NimbleUUID16ToGuid(ble_uuid16_t *b16, uint8_t *guid);
void NimbleUUID32ToGuid(ble_uuid32_t *b32, uint8_t *guid);
void NimbleUUID128ToGuid(ble_uuid128_t *b128, uint8_t *guid);
void NimbleUUIDToGuid(ble_uuid_any_t *bAny, uint8_t *guid);

void GuidToNimbleUUID(uint8_t *guid, ble_uuid_any_t *bAny);

u64_t BleAddressToUlong(u8_t *address);
void UlongToBleAddress(u64_t address, uint8_t adrType, ble_addr_t &bleAddr);

void PrintAddress(ble_addr_t &bleAddr);
void PrintBytes(uint8_t *pc, int len);
void PrintUuid(const ble_uuid_t *puuid);

HRESULT OmBufferToStack(CLR_RT_StackFrame &stack, os_mbuf *om);
HRESULT PushEmptyBufferToStack(CLR_RT_StackFrame &stack);

bool LockEventMutex();
void ReleaseEventMutex();

bool StartBleStack(char *devicename, uint16_t appearance);
bool StopBleStack();
bool WaitForBleStackStart(int waitMs);

DevicePairingResultStatus MapNimbleErrorToStatus(int errorCode);
DevicePairingKinds PairingActionToDevicePairingKinds(int action);
int DevicePairingKindsToPairingAction(DevicePairingKinds kinds);

#endif // NIMBLE_UTIL_H
