//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

extern bool DeviceBleInit();
extern void Device_ble_dispose();
extern int DeviceBleStart(bleServicesContext &context);
extern int DeviceBleCallback(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

extern void Esp32BleStartAdvertise(bleServicesContext *context);

extern const struct ble_gatt_chr_def gatt_char_device_info[];

typedef Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalService
    GattLocalService;
typedef Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider
    GattServiceProvider;
typedef Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalCharacteristic
    GattLocalCharacteristic;
typedef Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattPresentationFormat
    GattPresentationFormat;
typedef Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalDescriptor
    GattLocalDescriptor;

bleServicesContext bleContext;

static void InitContext(bleServicesContext *context)
{
    memset(context, 0, sizeof(bleServicesContext));
}

static void FreeContext(bleServicesContext &srvContext)
{
    if (srvContext.pDeviceName)
    {
        platform_free((void *)srvContext.pDeviceName);
        srvContext.pDeviceName = NULL;
    }

    if (srvContext.gatt_service_def)
    {
        platform_free((void *)srvContext.gatt_service_def);
        srvContext.gatt_service_def = NULL;
    }

    // For each Service free up memory
    for (int i = 0; i < srvContext.serviceCount; i++)
    {
        ble_context *context = &srvContext.bleSrvContexts[i];
        if (context->serviceUuid)
        {
            platform_free(context->serviceUuid);
            context->serviceUuid = NULL;
        }

        if (context->characteristicsDefs)
        {
            platform_free(context->characteristicsDefs);
            context->characteristicsDefs = NULL;
        }

        if (context->characteristicsUuids)
        {
            platform_free(context->characteristicsUuids);
            context->characteristicsUuids = NULL;
        }

        if (context->attrHandles)
        {
            platform_free(context->attrHandles);
            context->attrHandles = NULL;
        }

        if (context->descriptorDefs)
        {
            platform_free(context->descriptorDefs);
            context->descriptorDefs = NULL;
        }

        if (context->descriptorUuids)
        {
            platform_free(context->descriptorUuids);
            context->descriptorUuids = NULL;
        }
    }

    if (srvContext.bleSrvContexts)
    {
        platform_free((void *)srvContext.bleSrvContexts);
        srvContext.bleSrvContexts = NULL;
    }
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider::
    NativeInitService___BOOLEAN(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();
    {
        InitContext(&bleContext);

        stack.SetResult_Boolean(true);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

/*
ble_uuid_t *Ble_uuid16_declare(uint16_t id16)
{
    ble_uuid16_t *pU16 = (ble_uuid16_t *)platform_malloc(sizeof(ble_uuid16_t));
    pU16->u.type = BLE_UUID_TYPE_16;
    pU16->value = id16;
    return (ble_uuid_t *)pU16;
}
*/

// Assumes UUID is 16 bytes length
void BuildUUID(uint8_t *pUuid, ble_uuid_any_t *pUany)
{
    // Build UUID based on size/ leading zeroes, 16, 32 or 128
    // 00 00 00 00 - 00 00 - 00 00 - 00 00 - 00 00 00 00 00 00 - 128 bits
    // 00 00 xx xx - 00 00 - 10 00 - 80 00 - 00 80 5F 9B 34 FB - 16 bits
    // xx xx xx xx       "       "      "           "          - 32 bits

    // Order of last 12 bytes saved in a Guuid ( reverse )
    uint8_t BlueSig[12] = {0, 0, 0, 0x10, 0x80, 0, 0, 0x80, 0x5f, 0x9b, 0x34, 0xfb};

    // Is it a standard BlueSig 16 or 32 bit pattern
    if (memcmp(pUuid + 4, BlueSig, sizeof(BlueSig)) == 0)
    {
        // 16 Bit
        if (pUuid[2] == 0 && pUuid[3] == 0)
        {
            pUany->u16.u.type = BLE_UUID_TYPE_16;
            pUany->u16.value = (uint16_t)(pUuid[1] << 8) + pUuid[0];
        }
        else
        {
            pUany->u32.u.type = BLE_UUID_TYPE_32;
            pUany->u32.value = (uint32_t)(pUuid[3] << 24) + (pUuid[2] << 16) + (pUuid[1] << 8) + pUuid[0];
        }
    }
    else
    {
        int map[16]{15, 14, 13, 12, 11, 10, 9, 8, 6, 7, 4, 5, 0, 1, 2, 3};

        pUany->u128.u.type = BLE_UUID_TYPE_128;

        // Map bytes to correct LSB order
        uint8_t *pDest = &pUany->u128.value[0];
        for (int x = 0; x < 16; x++)
        {
            pDest[x] = pUuid[map[x]];
        }
    }
}

ble_uuid_any_t *BuildUuidAlloc(uint8_t *pUuid)
{
    ble_uuid_any_t *pUany = (ble_uuid_any_t *)platform_malloc(sizeof(ble_uuid_any_t));
    BuildUUID(pUuid, pUany);
    return pUany;
}

bool BuildGattServices(bleServicesContext &context)
{
    int srvCount = context.serviceCount;
    ble_context *def = context.bleSrvContexts;

    // Define all services and null service(end)
    int svcDefSize = sizeof(ble_gatt_svc_def) * (srvCount + 1);
    ble_gatt_svc_def *svcDef = (ble_gatt_svc_def *)platform_malloc(svcDefSize);
    if (svcDef == NULL)
    {
        return false;
    }

    memset(svcDef, 0, svcDefSize);
    context.gatt_service_def = svcDef;

    while (srvCount > 0)
    {
        svcDef->type = BLE_GATT_SVC_TYPE_PRIMARY;
        svcDef->uuid = def->serviceUuid;
        svcDef->includes = 0;
        svcDef->characteristics = def->characteristicsDefs;

        svcDef++;
        def++;
        srvCount--;
    }

    // Last - Terminator for services
    svcDef->type = BLE_GATT_SVC_TYPE_END;
    return true;
}

void SetUuid(CLR_RT_HeapBlock *pItem, int sourceFieldIndex, ble_uuid_any_t *targetUuidValue, ble_uuid_t **pUuid)
{
    CLR_RT_HeapBlock_Array *charUuidArray = (CLR_RT_HeapBlock_Array *)pItem[sourceFieldIndex].Array();
    uint8_t *uuid = charUuidArray->GetFirstElement();
    BuildUUID(uuid, targetUuidValue);

    *pUuid = (ble_uuid_t *)targetUuidValue;
}

void AssignDescriptor(ble_gatt_dsc_def *pDsc, CLR_RT_HeapBlock *pPfItem, ble_uuid_any_t *pUuid)
{
    SetUuid(pPfItem, GattLocalDescriptor::FIELD___uuid, pUuid, (ble_uuid_t **)&pDsc->uuid);

    pDsc->att_flags = BLE_ATT_F_READ;

    if (pPfItem[GattLocalDescriptor::FIELD__WriteRequested].Dereference() != NULL)
    {
        pDsc->att_flags |= BLE_ATT_F_WRITE;

        // Write protection - Authentication & Encryption
        GattProtectionLevel writeProtectLevel =
            (GattProtectionLevel)pPfItem[GattLocalDescriptor::FIELD___writeProtectionLevel].NumericByRef().u4;
        switch (writeProtectLevel)
        {
            case GattProtectionLevel_AuthenticationRequired:
                pDsc->att_flags += BLE_ATT_F_WRITE_AUTHEN;
                break;
            case GattProtectionLevel_EncryptionRequired:
                pDsc->att_flags += BLE_ATT_F_WRITE_ENC;
                break;
            case GattProtectionLevel_EncryptionAndAuthenticationRequired:
                pDsc->att_flags += BLE_ATT_F_WRITE_AUTHEN | BLE_ATT_F_WRITE_ENC;
                break;
            case GattProtectionLevel_Plain:
                break;
        }
    }

    // Read protection - Authentication & Encryption
    GattProtectionLevel readProtectLevel =
        (GattProtectionLevel)pPfItem[GattLocalCharacteristic::FIELD___readProtectionLevel].NumericByRef().u4;
    switch (readProtectLevel)
    {
        case GattProtectionLevel_AuthenticationRequired:
            pDsc->att_flags += BLE_ATT_F_READ_AUTHEN;
            break;
        case GattProtectionLevel_EncryptionRequired:
            pDsc->att_flags += BLE_ATT_F_READ_ENC;
            break;
        case GattProtectionLevel_EncryptionAndAuthenticationRequired:
            pDsc->att_flags += BLE_ATT_F_READ_AUTHEN | BLE_ATT_F_READ_ENC;
            break;
        case GattProtectionLevel_Plain:
            break;
    }

    pDsc->access_cb = DeviceBleCallback;

    pDsc->arg = (void *)(int32_t)pPfItem[GattLocalDescriptor::FIELD___descriptorId].NumericByRef().u2;

    pDsc->min_key_size = 0;
}

void AssignArrayDescriptors(ble_context &context, CLR_RT_HeapBlock *pArray, int sourceFieldIndex, int &descIndex)
{
    CLR_RT_HeapBlock *pArItem;
    CLR_RT_HeapBlock_ArrayList *pArrayDescriptors =
        (CLR_RT_HeapBlock_ArrayList *)pArray[sourceFieldIndex].Dereference();

    CLR_INT32 arrayCount = pArrayDescriptors->GetSize();

    // Set up array of descriptors if any
    for (int i = 0; i < arrayCount; i++)
    {
        if (SUCCEEDED(pArrayDescriptors->GetItem(i, pArItem)))
        {
            // Allocate descriptor
            ble_gatt_dsc_def *pDsc = &context.descriptorDefs[descIndex];

            // Fill in details
            AssignDescriptor(pDsc, pArItem, &context.descriptorUuids[descIndex]);

            // BLE_DEBUG_PRINTF("Array descriptors ptr=%X id=%X flags=%x cb=%x dindex=%d\n ",
            //     pDsc,
            //     (int)pDsc->arg,
            //     pDsc->att_flags,
            //     pDsc->access_cb,
            //     descIndex
            // );

            descIndex++;
        }
    }
}

void SetUuid(
    CLR_RT_HeapBlock *pItem,
    int sourceFieldIndex,
    ble_uuid_any_t *targetUuid,
    ble_gatt_chr_def *pCharacteristicsDef)
{
    CLR_RT_HeapBlock_Array *charUuidArray = (CLR_RT_HeapBlock_Array *)pItem[sourceFieldIndex].Array();
    uint8_t *uuid = charUuidArray->GetFirstElement();
    BuildUUID(uuid, targetUuid);

    pCharacteristicsDef->uuid = (ble_uuid_t *)targetUuid;
}

bool ParseAndBuildNimbleDefinition(ble_context &context, CLR_RT_HeapBlock *pGattLocalService)
{
    uint8_t *pUuid;
    CLR_RT_HeapBlock *pItem;
    int descriptorCount = 0;
    int charIndex = 0;
    int descIndex = 0;

    CLR_RT_HeapBlock_Array *pUuidArray =
        (CLR_RT_HeapBlock_Array *)pGattLocalService[GattLocalService::FIELD___serviceUuid].Array();

    pUuid = pUuidArray->GetFirstElement();
    context.serviceUuid = (ble_uuid_t *)BuildUuidAlloc(pUuid);

    CLR_RT_HeapBlock_ArrayList *pCharacteristics =
        (CLR_RT_HeapBlock_ArrayList *)pGattLocalService[GattLocalService::FIELD___characteristics].Dereference();

    CLR_INT32 CharacteristicsCount = pCharacteristics->GetSize();

    // +1 for Terminator definition
    CharacteristicsCount++;

    // Allocate an array of Characteristics definitions + null definition and save in context
    context.characteristicsCount = CharacteristicsCount;
    context.characteristicsDefs =
        (ble_gatt_chr_def *)platform_malloc(sizeof(ble_gatt_chr_def) * context.characteristicsCount);
    if (context.characteristicsDefs == NULL)
    {
        // Out of memory
        return false;
    }

    memset(context.characteristicsDefs, 0, sizeof(ble_gatt_chr_def) * context.characteristicsCount);

    // Initial pass to count number of descriptors required so we can allocate things in 1 lump and keep reference for
    // freeing Descriptors include user descriptors and presentation formats
    for (int index = 0; index < CharacteristicsCount; index++)
    {
        if (SUCCEEDED(pCharacteristics->GetItem(index, pItem)))
        {
            CLR_RT_HeapBlock *pUserDescriptionDescriptors =
                (CLR_RT_HeapBlock *)pItem[GattLocalCharacteristic::FIELD___userDescriptionDescriptor].Dereference();
            if (pUserDescriptionDescriptors != NULL)
            {
                descriptorCount++;
            }

            CLR_RT_HeapBlock_ArrayList *pPresentationFormatsDiscriptors =
                (CLR_RT_HeapBlock_ArrayList *)pItem[GattLocalCharacteristic::FIELD___presentationFormatsDescriptors]
                    .Dereference();

            descriptorCount += pPresentationFormatsDiscriptors->GetSize();

            CLR_RT_HeapBlock_ArrayList *pDescriptors =
                (CLR_RT_HeapBlock_ArrayList *)pItem[GattLocalCharacteristic::FIELD___descriptors].Dereference();

            descriptorCount += pDescriptors->GetSize();

            // Plus terminator
            descriptorCount++;

            // BLE_DEBUG_PRINTF("characteristics index=%d descriptorCount=%d\n ", index,  descriptorCount);
        }
    }

    context.descriptorCount = descriptorCount;

    // Allocate attribute handles for characteristics
    context.attrHandles = (uint16_t *)platform_malloc(sizeof(uint16_t) * context.characteristicsCount);
    if (context.attrHandles == NULL)
    {
        // Out of memory
        return false;
    }

    // Allocate UUID table
    context.characteristicsUuids =
        (ble_uuid_any_t *)platform_malloc(sizeof(ble_uuid_any_t) * context.characteristicsCount);
    if (context.characteristicsUuids == NULL)
    {
        // Out of memory
        return false;
    }

    // Allocate tables
    context.descriptorDefs = (ble_gatt_dsc_def *)platform_malloc(sizeof(ble_gatt_dsc_def) * descriptorCount);
    if (context.descriptorDefs == NULL)
    {
        // Out of memory
        return false;
    }
    context.descriptorUuids = (ble_uuid_any_t *)platform_malloc(sizeof(ble_uuid_any_t) * descriptorCount);
    if (context.descriptorUuids == NULL)
    {
        // Out of memory
        return false;
    }

    // Left in for the moment as this is still work in progress
    BLE_DEBUG_PRINTF("characteristicsCount = %d size=%X\n ", CharacteristicsCount, sizeof(ble_gatt_chr_def));
    BLE_DEBUG_PRINTF("descriptorCount = %d  size=%X\n ", descriptorCount, sizeof(ble_gatt_dsc_def));

    BLE_DEBUG_PRINTF(
        "characteristicsDefs %X  end %X\n",
        context.characteristicsDefs,
        ((uint8_t *)context.characteristicsDefs) + (sizeof(ble_gatt_chr_def) * context.characteristicsCount));

    BLE_DEBUG_PRINTF(
        "characteristicsUuids  %X  end %X\n",
        context.characteristicsUuids,
        ((uint8_t *)context.characteristicsUuids) + (sizeof(ble_uuid_any_t) * context.characteristicsCount));

    BLE_DEBUG_PRINTF(
        "attrHandles  %X  end %X\n",
        context.attrHandles,
        ((uint8_t *)context.attrHandles) + (sizeof(uint16_t) * context.characteristicsCount));

    BLE_DEBUG_PRINTF(
        "descriptorDefs  %X  end %X\n",
        context.descriptorDefs,
        ((uint8_t *)context.descriptorDefs) + (sizeof(ble_gatt_dsc_def) * context.descriptorCount));

    BLE_DEBUG_PRINTF(
        "descriptorUuids  %X  end %X\n",
        context.descriptorUuids,
        ((uint8_t *)context.descriptorUuids) + (sizeof(ble_uuid_any_t) * context.descriptorCount));

    // Build definitions required for Nimble
    for (charIndex = 0; charIndex < CharacteristicsCount; charIndex++)
    {
        if (SUCCEEDED(pCharacteristics->GetItem(charIndex, pItem)))
        {
            // Build entry for nimble characteristic definition
            // UUID
            SetUuid(
                pItem,
                GattLocalCharacteristic::FIELD___characteristicUuid,
                &context.characteristicsUuids[charIndex],
                (ble_uuid_t **)&context.characteristicsDefs[charIndex].uuid);

            // Save CharacteristicId in arg field for callback
            int characteristicId = pItem[GattLocalCharacteristic::FIELD___characteristicId].NumericByRef().u2;
            context.characteristicsDefs[charIndex].arg = (void *)characteristicId;

            // Save ptr for attr handle to be saved
            context.characteristicsDefs[charIndex].val_handle = &context.attrHandles[charIndex];

            // Set up Flags
            ble_gatt_chr_flags flags = 0;

            // Write protection - Authentication & Encryption
            GattProtectionLevel writeProtectLevel =
                (GattProtectionLevel)pItem[GattLocalCharacteristic::FIELD___writeProtectionLevel].NumericByRef().u4;
            switch (writeProtectLevel)
            {
                case GattProtectionLevel_AuthenticationRequired:
                    flags += BLE_GATT_CHR_F_WRITE_AUTHEN;
                    break;
                case GattProtectionLevel_EncryptionRequired:
                    flags += BLE_GATT_CHR_F_WRITE_ENC;
                    break;
                case GattProtectionLevel_EncryptionAndAuthenticationRequired:
                    flags += BLE_GATT_CHR_F_WRITE_AUTHEN | BLE_GATT_CHR_F_WRITE_ENC;
                    break;
                case GattProtectionLevel_Plain:
                    break;
            }

            // Read protection - Authentication & Encryption
            GattProtectionLevel readProtectLevel =
                (GattProtectionLevel)pItem[GattLocalCharacteristic::FIELD___readProtectionLevel].NumericByRef().u4;
            switch (readProtectLevel)
            {
                case GattProtectionLevel_AuthenticationRequired:
                    flags += BLE_GATT_CHR_F_READ_AUTHEN;
                    break;
                case GattProtectionLevel_EncryptionRequired:
                    flags += BLE_GATT_CHR_F_READ_ENC;
                    break;
                case GattProtectionLevel_EncryptionAndAuthenticationRequired:
                    flags += BLE_GATT_CHR_F_READ_AUTHEN | BLE_GATT_CHR_F_READ_ENC;
                    break;
                case GattProtectionLevel_Plain:
                    break;
            }

            GattCharacteristicProperties properties =
                (GattCharacteristicProperties)pItem[GattLocalCharacteristic::FIELD___properties].NumericByRef().u4;

            if (properties & GattCharacteristicProperties_Read)
                flags |= BLE_GATT_CHR_F_READ;
            if (properties & GattCharacteristicProperties_WriteWithoutResponse)
                flags |= BLE_GATT_CHR_F_WRITE_NO_RSP;
            if (properties & GattCharacteristicProperties_Write)
                flags |= BLE_GATT_CHR_F_WRITE;
            if (properties & GattCharacteristicProperties_Notify)
                flags |= BLE_GATT_CHR_F_NOTIFY;
            if (properties & GattCharacteristicProperties_Broadcast)
                flags |= BLE_GATT_CHR_F_BROADCAST;
            if (properties & GattCharacteristicProperties_Indicate)
                flags |= BLE_GATT_CHR_F_INDICATE;
            if (properties & GattCharacteristicProperties_AuthenticatedSignedWrites)
                flags |= BLE_GATT_CHR_F_AUTH_SIGN_WRITE;
            if (properties & GattCharacteristicProperties_ReliableWrites)
                flags |= BLE_GATT_CHR_F_RELIABLE_WRITE;
            if (properties & GattCharacteristicProperties_WritableAuxiliaries)
                flags |= BLE_GATT_CHR_F_AUTH_SIGN_WRITE;

            // Unhandled TODO
            // #define BLE_GATT_CHR_F_READ_AUTHOR                      0x0800
            // #define BLE_GATT_CHR_F_WRITE_AUTHOR                     0x400

            // TODO handle later
            // GattCharacteristicProperties_None:
            // GattCharacteristicProperties_ExtendedProperties:

            context.characteristicsDefs[charIndex].flags = flags;

            // Set callback used for all characteristics
            context.characteristicsDefs[charIndex].access_cb = DeviceBleCallback;

            context.characteristicsDefs[charIndex].min_key_size = 0; // TODO

            // Assign next descriptor as start for this characteristics
            context.characteristicsDefs[charIndex].descriptors = &context.descriptorDefs[descIndex];

            // Set up User description descriptor if any
            CLR_RT_HeapBlock *pUserDescriptionDescriptors =
                (CLR_RT_HeapBlock *)pItem[GattLocalCharacteristic::FIELD___userDescriptionDescriptor].Dereference();
            if (pUserDescriptionDescriptors != NULL)
            {
                // Allocate descriptor for each User Description
                ble_gatt_dsc_def *pDsc = &context.descriptorDefs[descIndex];

                // Fill in descriptor details
                AssignDescriptor(pDsc, pUserDescriptionDescriptors, &context.descriptorUuids[descIndex++]);

                // BLE_DEBUG_PRINTF("User description ptr=%X id=%X flags=%x cb=%x dindex=%d\n ",
                //     pDsc,
                //     pDsc->arg,
                //     pDsc->att_flags,
                //     pDsc->access_cb,
                //     descIndex - 1
                // );
            }

            // Set up Presentation Format descriptors if any
            AssignArrayDescriptors(
                context,
                pItem,
                GattLocalCharacteristic::FIELD___presentationFormatsDescriptors,
                descIndex);

            // Set up user descriptors if any
            AssignArrayDescriptors(context, pItem, GattLocalCharacteristic::FIELD___descriptors, descIndex);

            // Add terminator
            ble_gatt_dsc_def *pDsc = &context.descriptorDefs[descIndex++];
            pDsc->uuid = 0;

            BLE_DEBUG_PRINTF("ble_gatt_dsc_def * %X\n", pDsc);

            // BLE_DEBUG_PRINTF("Characteristic id=%X flags=%x cb=%x dindex=%d\n ",
            //     characteristicId,
            //     context.characteristicsDefs[charIndex].flags,
            //     context.characteristicsDefs[charIndex].access_cb,
            //     descIndex
            // );

            // BLE_DEBUG_PRINTF("Characteristic item %d\n", index);
            // BLE_DEBUG_PRINTF("writeProtectionLevel %d CharacteristicHandle:%d\n", writeProtectionLevel,
            // CharacteristicHandle);
        }
    }

    // Terminate characteristics
    context.characteristicsDefs[CharacteristicsCount - 1].uuid = NULL;

    BLE_DEBUG_PRINTF(
        "characteristics start %X last def %X \n ",
        context.characteristicsDefs,
        &context.characteristicsDefs[CharacteristicsCount - 1]);
    BLE_DEBUG_PRINTF("characteristics index %d  descriptor index %d\n", charIndex, descIndex);

    return true;
}

#ifdef BLE_NANO_DEBUG
void PrintChars(const ble_gatt_chr_def *pchars)
{
    ble_gatt_chr_def *pchar = (ble_gatt_chr_def *)pchars;

    BLE_DEBUG_PRINTF("==Characteristics definitions==\n");

    while (pchar->uuid != NULL)
    {
        BLE_DEBUG_PRINTF("==characteristic==\n");
        PrintUuid(pchar->uuid);

        BLE_DEBUG_PRINTF("flags %X CB %X val %X\n", pchar->flags, pchar->access_cb, pchar->val_handle);
        if (pchar->val_handle != 0)
        {
            BLE_DEBUG_PRINTF("ATT handle %d\n", *(pchar->val_handle));
        }
        pchar++;
    }
    BLE_DEBUG_PRINTF("==End characteristics definitions==\n");
}

void PrintSvrDefs(ble_gatt_svc_def *svcDef)
{

    ble_gatt_svc_def *pSDef = svcDef;

    BLE_DEBUG_PRINTF("==Services definition==\n");

    while (pSDef->type != 0)
    {
        BLE_DEBUG_PRINTF("==Service==\n");
        PrintUuid(pSDef->uuid);
        PrintChars(pSDef->characteristics);
        pSDef++;
    }

    BLE_DEBUG_PRINTF("==End services definition==\n");
}
#endif

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider::
    NativeStartAdvertising___BOOLEAN(CLR_RT_StackFrame &stack)
{
    bool result = false;

    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_ArrayList *pArrayServices;
        int deviceNameLen;

        CLR_RT_HeapBlock *pThis = stack.This(); // ptr to GattServiceProvider
        FAULT_ON_NULL(pThis);

        // Save Discoverable & Connectable flags in context
        bool isDiscoverable = pThis[GattServiceProvider::FIELD___isDiscoverable].NumericByRef().s4 != 0;
        bool isConnectable = pThis[GattServiceProvider::FIELD___isConnectable].NumericByRef().s4 != 0;

        bleContext.isDiscoverable = isDiscoverable;
        bleContext.isConnectable = isConnectable;

        // Save Device name in context
        CLR_RT_HeapBlock_Array *pDeviceNameField =
            (CLR_RT_HeapBlock_Array *)pThis[GattServiceProvider::FIELD___deviceName].Array();
        char *pDeviceName = (char *)pDeviceNameField->GetFirstElement();
        deviceNameLen = pDeviceNameField->m_numOfElements;

        bleContext.pDeviceName = (char *)platform_malloc(deviceNameLen + 1);
        memcpy(bleContext.pDeviceName, pDeviceName, deviceNameLen);
        bleContext.pDeviceName[deviceNameLen] = 0;

        pArrayServices = (CLR_RT_HeapBlock_ArrayList *)pThis[GattServiceProvider::FIELD___services].Dereference();
        bleContext.serviceCount = pArrayServices->GetSize();

        // Allocate contexts for all service definitions
        size_t bleContextSize = sizeof(ble_context) * bleContext.serviceCount;
        bleContext.bleSrvContexts = (ble_context *)platform_malloc(bleContextSize);
        if (bleContext.bleSrvContexts == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        memset(bleContext.bleSrvContexts, 0, bleContextSize);

        // Foreach Service set up the Nimble definitions
        CLR_RT_HeapBlock *serviceItem;
        for (int i = 0; i < bleContext.serviceCount; i++)
        {
            if (SUCCEEDED(pArrayServices->GetItem(i, serviceItem)))
            {
                if (!ParseAndBuildNimbleDefinition(bleContext.bleSrvContexts[i], serviceItem))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }
            }
            else
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
            }
        }

        if (!BuildGattServices(bleContext))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        BLE_DEBUG_PRINTF("Gatt Services built\n");

#ifdef NANO_BLE_DEBUG
        // PrintSvrDefs(bleContext.gatt_service_def);
#endif
        result = DeviceBleInit();
        if (!result)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        result = (DeviceBleStart(bleContext) == 0);

        // Wait for stack to start
        // Otherwise you will have problems if Stop is called straight away before stack has started
        if (WaitForBleStackStart(5000))
        {
            BLE_DEBUG_PRINTF("Stack start signaled\n");
        }

        stack.SetResult_Boolean(result);
    }

    NANOCLR_CLEANUP();

    if (!result)
    {
        Device_ble_dispose();
        FreeContext(bleContext);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider::
    NativeStopAdvertising___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();
    {
        BLE_DEBUG_PRINTF("Server: Stop Advertising\n");

        ble_gap_adv_stop();

        Device_ble_dispose();

        FreeContext(bleContext);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
