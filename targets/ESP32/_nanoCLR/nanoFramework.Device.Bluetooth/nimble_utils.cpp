//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

uint8_t blueSig[16] = {0, 0, 0, 0, 0, 0, 0, 0x10, 0x80, 0, 0, 0x80, 0x5f, 0x9b, 0x34, 0xfb};

void NimbleUUID16ToGuid(ble_uuid16_t *b16, uint8_t *guid)
{
    memcpy(guid, blueSig, sizeof(blueSig));
    guid[1] = (b16->value >> 8);
    guid[0] = (b16->value & 0xff);
}

void NimbleUUID32ToGuid(ble_uuid32_t *b32, uint8_t *guid)
{
    memcpy(guid, blueSig, sizeof(blueSig));
    guid[3] = ((b32->value >> 24) & 0xff);
    guid[2] = ((b32->value >> 16) & 0xff);
    guid[1] = ((b32->value >> 8) & 0xff);
    guid[0] = (b32->value & 0xff);
}

void NimbleUUID128ToGuid(ble_uuid128_t *b128, uint8_t *guid)
{
    int map[16]{12, 13, 14, 15, 10, 11, 8, 9, 7, 6, 5, 4, 3, 2, 1, 0};

    // Map bytes to correct LSB order
    uint8_t *pSource = b128->value;
    for (int x = 0; x < 16; x++)
    {
        guid[x] = pSource[map[x]];
    }
}

// Convert Nimble ble_uuid_any_t to a 16 byte Guid
void NimbleUUIDToGuid(ble_uuid_any_t *bAny, uint8_t *guid)
{

    switch (bAny->u.type)
    {
        case BLE_UUID_TYPE_16:
            NimbleUUID16ToGuid(&bAny->u16, guid);
            break;

        case BLE_UUID_TYPE_32:
            NimbleUUID32ToGuid(&bAny->u32, guid);
            break;

        default: // assume 128
            NimbleUUID128ToGuid(&bAny->u128, guid);
            break;
    }
}

// Convert Guid 16 bytes to Nimble ble_uuid_any_t format
// Assumes Guid is 16 bytes length
void GuidToNimbleUUID(uint8_t *guid, ble_uuid_any_t *bAny)
{
    // Build UUID based on size/ leading zeroes, 16, 32 or 128
    // 00 00 00 00 - 00 00 - 00 00 - 00 00 - 00 00 00 00 00 00 - 128 bits
    // 00 00 xx xx - 00 00 - 10 00 - 80 00 - 00 80 5F 9B 34 FB - 16 bits
    // xx xx xx xx       "       "      "           "          - 32 bits

    // 16 or 32 bit ?
    if (memcmp(guid + 4, blueSig + 4, sizeof(blueSig) - 4) == 0)
    {
        // 16 Bit
        if (guid[0] == 0 && guid[1] == 0)
        {
            bAny->u16.u.type = BLE_UUID_TYPE_16;
            bAny->u16.value = (uint16_t)(guid[2] << 8) + guid[3];
        }
        else
        {
            bAny->u32.u.type = BLE_UUID_TYPE_32;
            bAny->u32.value = (uint32_t)(guid[0] << 24) + (guid[1] << 16) + (guid[2] << 8) + guid[3];
        }
    }
    else
    {
        int map[16]{15, 14, 13, 12, 11, 10, 9, 8, 6, 7, 4, 5, 0, 1, 2, 3};

        bAny->u128.u.type = BLE_UUID_TYPE_128;

        // Map bytes to correct LSB order
        uint8_t *dest = &bAny->u128.value[0];
        for (int x = 0; x < 16; x++)
        {
            dest[x] = guid[map[x]];
        }
    }
}

u64_t BleAddressToUlong(u8_t *address)
{
    u64_t bleAdr = 0;
    address += 5;

    for (int index = 0; index <= 5; index++)
    {
        bleAdr <<= 8;
        bleAdr += *address;
        address--;
    }

    return bleAdr;
}

void UlongToBleAddress(u64_t address, uint8_t adrType, ble_addr_t &bleAddr)
{
    bleAddr.type = adrType;

    bleAddr.val[0] = address & 0xff;
    bleAddr.val[1] = (address >> 8) & 0xff;
    bleAddr.val[2] = (address >> 16) & 0xff;
    bleAddr.val[3] = (address >> 24) & 0xff;
    bleAddr.val[4] = (address >> 32) & 0xff;
    bleAddr.val[5] = (address >> 40) & 0xff;
}

void PrintAddress(ble_addr_t &bleAddr)
{
    BLE_DEBUG_PRINTF("ble_addr type %d Adr:", bleAddr.type);
    for (int i = 5; i >= 0; i--)
    {
        BLE_DEBUG_PRINTF("%0X ", bleAddr.val[i]);
    }
    BLE_DEBUG_PRINTF("\n");
}

void PrintBytes(uint8_t *pc, int len)
{
    for (int x = 0; x < len; x++)
    {
        BLE_DEBUG_PRINTF("%0X ", (int)*pc);
        pc++;
    }
    BLE_DEBUG_PRINTF("\n");
}

void PrintUuid(const ble_uuid_t *puuid)
{
    ble_uuid_any_t *pu = (ble_uuid_any_t *)puuid;

    switch (pu->u.type)
    {
        case BLE_UUID_TYPE_16:

            BLE_DEBUG_PRINTF("uuid16:%x\n", (int)pu->u16.value);
            break;

        case BLE_UUID_TYPE_32:
            BLE_DEBUG_PRINTF("uuid32:%x\n", pu->u32.value);
            break;

        case BLE_UUID_TYPE_128:
            BLE_DEBUG_PRINTF("uuid128:%x ");
            for (int x = 0; x < 16; x++)
            {
                BLE_DEBUG_PRINTF("%X", (int)pu->u128.value[x]);
            }
            BLE_DEBUG_PRINTF("\n");
            break;

        default:
            BLE_DEBUG_PRINTF("Invalid uuid type:%x \n", pu->u.type);
            break;
    }
}

HRESULT OmBufferToStack(CLR_RT_StackFrame &stack, os_mbuf *om)
{
    NANOCLR_HEADER();
    {
        uint8_t *pReturnBuffer;
        uint16_t om_len;
        uint16_t out_len;

        // Get length of available data
        om_len = OS_MBUF_PKTLEN(om);

        // Create managed byte array of correct size as per OM buffer
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), om_len, g_CLR_RT_WellKnownTypes.m_UInt8));

        // get a pointer to the first object in the array
        pReturnBuffer = stack.TopValue().DereferenceArray()->GetFirstElement();

        // Copy OM buffer to return byte buffer
        ble_hs_mbuf_to_flat(om, pReturnBuffer, om_len, &out_len);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT PushEmptyBufferToStack(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // Error return empty array, event not found ?
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 0, g_CLR_RT_WellKnownTypes.m_UInt8));
    }
    NANOCLR_NOCLEANUP();
}

bool LockEventMutex()
{
    return xSemaphoreTake(ble_event_data.mutex, (TickType_t)(1000 / portTICK_PERIOD_MS));
}

void ReleaseEventMutex()
{
    xSemaphoreGive(ble_event_data.mutex);
}

bool WaitForBleStackStart(int waitMs)
{
    EventBits_t uxBits = xEventGroupWaitBits(
        ble_event_waitgroup,
        N_BLE_EVENT_STARTED,
        pdTRUE,
        pdFALSE,
        (TickType_t)(waitMs / portTICK_PERIOD_MS));
    BLE_DEBUG_PRINTF("wait handled ? complete %X\n", uxBits);
    if (uxBits & N_BLE_EVENT_STARTED)
    {
        return true;
    }
    return false;
}