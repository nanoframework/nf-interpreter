//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

extern bool device_ble_init();
extern void device_ble_dispose();
extern int device_ble_start(ble_context *context);
extern int device_ble_callback(
    uint16_t conn_handle,
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg);
extern void esp32_ble_start_advertise(int flags);

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

#define GATT_DEVICE_INFO_UUID 0x180A

ble_context blecontext;

static void InitContext(ble_context *context)
{
    memset(context, 0, sizeof(ble_context));
}

static void FreeContext(ble_context *context)
{
    if (context->pDeviceName)
    {
        platform_free((void *)context->pDeviceName);
        context->pDeviceName = NULL;
    }

    if (context->gatt_service_def)
    {
        platform_free((void *)context->gatt_service_def);
        context->gatt_service_def = NULL;
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

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider::
    NativeInitService___BOOLEAN(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();
    {
        InitContext(&blecontext);

        stack.SetResult_Boolean(true);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

ble_uuid_t *Ble_uuid16_declare(uint16_t id16)
{
    ble_uuid16_t *pU16 = (ble_uuid16_t *)platform_malloc(sizeof(ble_uuid16_t));
    pU16->u.type = BLE_UUID_TYPE_16;
    pU16->value = id16;
    return (ble_uuid_t *)pU16;
}

// Assumes UUID is 16 bytes length
void BuildUUID(uint8_t *pUuid, ble_uuid_any_t *pUany)
{
    // Build UUID based on size/ leading zeroes, 16, 32 or 128
    // 00 00 00 00 - 00 00 - 00 00 - 00 00 - 00 00 00 00 00 00 - 128 bits
    // 00 00 xx xx - 00 00 - 10 00 - 80 00 - 00 80 5F 9B 34 FB - 16 bits
    // xx xx xx xx       "       "      "           "          - 32 bits

    uint8_t BlueSig[12] = {0, 0, 0x10, 0, 0x80, 0, 0, 0x80, 0x5f, 0x9b, 0x34, 0xfb};

    // 16 or 32 bit ?
    if (memcmp(pUuid + 4, BlueSig, sizeof(BlueSig)) == 0)
    {
        // 16 Bit
        if (pUuid[0] == 0 && pUuid[1] == 0)
        {
            pUany->u16.u.type = BLE_UUID_TYPE_16;
            pUany->u16.value = (uint16_t)(pUuid[2] << 8) + pUuid[3];
        }
        else
        {
            pUany->u32.u.type = BLE_UUID_TYPE_32;
            pUany->u32.value = (uint32_t)(pUuid[0] << 24) + (pUuid[1] << 16) + (pUuid[2] << 8) + pUuid[3];
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

// Build single service with terminator
ble_gatt_svc_def *BuildGattService(uint8_t type, uint8_t *pUuid, const ble_gatt_chr_def *characteristics, int *count)
{
    *count = 3; // 3 service entries

    // Define 3 services, custom, device_info and null service(end)
    ble_gatt_svc_def *pSvcDef = (ble_gatt_svc_def *)platform_malloc(sizeof(ble_gatt_svc_def) * (*count));
    ble_gatt_svc_def *pCurSvcDef = pSvcDef;

    // 1st
    pCurSvcDef->type = type;
    pCurSvcDef->uuid = (ble_uuid_t *)BuildUuidAlloc(pUuid);
    pCurSvcDef->includes = 0;
    pCurSvcDef->characteristics = characteristics;

    // 2nd Device Info
    pCurSvcDef++;
    pCurSvcDef->type = BLE_GATT_SVC_TYPE_PRIMARY;
    pCurSvcDef->uuid = Ble_uuid16_declare(GATT_DEVICE_INFO_UUID);
    pCurSvcDef->includes = 0;
    pCurSvcDef->characteristics = gatt_char_device_info;

    // 3rd - Terminator for services
    pCurSvcDef++;
    pCurSvcDef->type = BLE_GATT_SVC_TYPE_END;

    return pSvcDef;
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

    pDsc->att_flags = 0;
    if (pPfItem[GattLocalDescriptor::FIELD___writeProtectionLevel].NumericByRef().u4)
    {
        pDsc->att_flags |= BLE_GATT_CHR_F_WRITE;
    }
    if (pPfItem[GattLocalDescriptor::FIELD___readProtectionLevel].NumericByRef().u4)
    {
        pDsc->att_flags |= BLE_GATT_CHR_F_READ;
    }

    pDsc->access_cb = device_ble_callback;

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

            // debug_printf("Array descriptors id=%X flags=%x cb=%x dindex=%d\n ",
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

void ParseAndBuildNimbleDefinition(ble_context &context, CLR_RT_HeapBlock *pGattLocalService)
{
    uint8_t *pUuid;
    CLR_RT_HeapBlock *pItem;
    int descriptorCount = 0;
    int charIndex = 0;
    int descIndex = 0;

    CLR_RT_HeapBlock_Array *pUuidArray =
        (CLR_RT_HeapBlock_Array *)pGattLocalService[GattLocalService::FIELD___serviceUuid].Array();

    pUuid = pUuidArray->GetFirstElement();

    CLR_RT_HeapBlock_ArrayList *pCharacteristics =
        (CLR_RT_HeapBlock_ArrayList *)pGattLocalService[GattLocalService::FIELD___characteristics].Dereference();

    CLR_INT32 CharacteristicsCount = pCharacteristics->GetSize();

    // +1 for Terminator definition
    CharacteristicsCount++;

    // Allocate an array of Characteristics definitions + null definition and save in context
    context.characteristicsCount = CharacteristicsCount;
    context.characteristicsDefs =
        (ble_gatt_chr_def *)platform_malloc(sizeof(ble_gatt_chr_def) * context.characteristicsCount);

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
                (CLR_RT_HeapBlock_ArrayList *)pItem[GattLocalCharacteristic::FIELD___presentationFormats].Dereference();

            descriptorCount += pDescriptors->GetSize();

            // Plus terminator
            descriptorCount++;
        }
    }

    context.descriptorCount = descriptorCount;

    // Allocate attribute handles for characteristics
    context.attrHandles = (uint16_t *)platform_malloc(sizeof(uint16_t) * context.characteristicsCount);

    // Allocate UUID table
    context.characteristicsUuids =
        (ble_uuid_any_t *)platform_malloc(sizeof(ble_uuid_any_t) * context.characteristicsCount);

    // Allocate tables
    context.descriptorDefs = (ble_gatt_dsc_def *)platform_malloc(sizeof(ble_gatt_dsc_def) * descriptorCount);
    context.descriptorUuids = (ble_uuid_any_t *)platform_malloc(sizeof(ble_uuid_any_t) * descriptorCount);

    // debug_printf("characteristicsCount = %d\n ", CharacteristicsCount);
    // debug_printf("descriptorCount = %d\n ", descriptorCount);
    // debug_printf("characteristicsDefs %X  end %X\n", context.characteristicsDefs,  context.characteristicsDefs +
    // (sizeof(ble_gatt_chr_def) * context.characteristicsCount) ); debug_printf("characteristicsUuids  %X  end %X\n",
    // context.characteristicsUuids, context.characteristicsUuids + (sizeof(ble_uuid_any_t) *
    // context.characteristicsCount)); debug_printf("attrHandles  %X  end %X\n", context.attrHandles,
    // context.attrHandles + (sizeof(uint16_t) * context.characteristicsCount)); debug_printf("descriptorDefs  %X  end
    // %X\n", context.descriptorDefs, context.descriptorDefs + (sizeof(ble_gatt_dsc_def) * context.descriptorCount));
    // debug_printf("descriptorUuids  %X  end %X\n", context.descriptorUuids, context.descriptorUuids +
    // (sizeof(ble_uuid_any_t) * context.descriptorCount));

    // Build definitions require for Nimble
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
            GattCharacteristicProperties properties =
                (GattCharacteristicProperties)pItem[GattLocalCharacteristic::FIELD___properties].NumericByRef().u4;
            ble_gatt_chr_flags flags = 0;

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
            // #define BLE_GATT_CHR_F_READ_ENC                         0x0200
            // #define BLE_GATT_CHR_F_READ_AUTHEN                      0x0400
            // #define BLE_GATT_CHR_F_READ_AUTHOR                      0x0800
            // #define BLE_GATT_CHR_F_WRITE_ENC                        0x1000
            // #define BLE_GATT_CHR_F_WRITE_AUTHEN                     0x2000
            // #define BLE_GATT_CHR_F_WRITE_AUTHOR                     0x400

            // TODO handle later
            // GattCharacteristicProperties_None:
            // GattCharacteristicProperties_ExtendedProperties:

            context.characteristicsDefs[charIndex].flags = flags;

            // Set callback used for all characteristics
            context.characteristicsDefs[charIndex].access_cb = device_ble_callback;

            // CLR_INT32 writeProtectionLevel =
            // pItem[GattLocalCharacteristic::FIELD___writeProtectionLevel].NumericByRef().s4;

            context.characteristicsDefs[charIndex].min_key_size = 0; // TODO

            // Assign next descriptor as start for this characteristics
            context.characteristicsDefs[charIndex].descriptors = &context.descriptorDefs[descIndex];

            // Set up User description descriptor if any
            CLR_RT_HeapBlock *pUserDescriptionDescriptors =
                (CLR_RT_HeapBlock *)pItem[GattLocalCharacteristic::FIELD___userDescriptionDescriptor].Dereference();
            if (pUserDescriptionDescriptors != NULL)
            {
                // Allocate descriptor for each PresentationFormat
                ble_gatt_dsc_def *pDsc = &context.descriptorDefs[descIndex];

                // Fill in descriptor details
                AssignDescriptor(pDsc, pUserDescriptionDescriptors, &context.descriptorUuids[descIndex++]);

                // debug_printf("User description id=%X flags=%x cb=%x dindex=%d\n ",
                //     pDsc->arg,
                //     pDsc->att_flags,
                //     pDsc->access_cb,
                //     descIndex - 1
                //);
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

            // debug_printf("Characteristic id=%X flags=%x cb=%x dindex=%d\n ",
            //     characteristicId,
            //     context.characteristicsDefs[index].flags,
            //     context.characteristicsDefs[index].access_cb,
            //     descIndex
            //);

            // debug_printf("Characteristic item %d\n", index);
            // debug_printf("writeProtectionLevel %d CharacteristicHandle:%d\n", writeProtectionLevel,
            // CharacteristicHandle);
        }
    }

    // Terminate characteristics
    context.characteristicsDefs[CharacteristicsCount - 1].uuid = NULL;

    // debug_printf("characteristics start %X last def %X \n ", context.characteristicsDefs,
    // &context.characteristicsDefs[CharacteristicsCount-1]); debug_printf("characteristics index %d  descriptor index
    // %d\n",charIndex, descIndex);

    context.gatt_service_def =
        BuildGattService(BLE_GATT_SVC_TYPE_PRIMARY, pUuid, context.characteristicsDefs, &context.serviceCount);
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider::
    NativeStartAdvertising___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        bool result;

        CLR_RT_HeapBlock *pThis = stack.This(); // ptr to GattServiceProvider
        FAULT_ON_NULL(pThis);

        CLR_RT_HeapBlock *pGattLocalService = pThis[GattServiceProvider::FIELD___service].Dereference();
        FAULT_ON_NULL(pGattLocalService);

        // Save Discoverable & Connectable flags in context
        bool isDiscoverable = pThis[GattServiceProvider::FIELD___isDiscoverable].NumericByRef().s4 != 0;
        bool isConnectable = pThis[GattServiceProvider::FIELD___isConnectable].NumericByRef().s4 != 0;
        blecontext.isDiscoverable = isDiscoverable;
        blecontext.isConnectable = isConnectable;

        // Save Device name in context
        CLR_RT_HeapBlock_Array *pDeviceNameField =
            (CLR_RT_HeapBlock_Array *)pThis[GattServiceProvider::FIELD___deviceName].Array();
        char *pDeviceName = (char *)pDeviceNameField->GetFirstElement();
        int deviceNameLen = pDeviceNameField->m_numOfElements;

        blecontext.pDeviceName = (char *)platform_malloc(deviceNameLen + 1);
        memcpy(blecontext.pDeviceName, pDeviceName, deviceNameLen);
        blecontext.pDeviceName[deviceNameLen] = 0;

        ParseAndBuildNimbleDefinition(blecontext, pGattLocalService);

        result = device_ble_init();
        if (result)
        {
            // debug_printf("device_ble_init complete\n");

            device_ble_start(&blecontext);

            // debug_printf("device_ble_start complete\n");
        }

        stack.SetResult_Boolean(result);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider::
    NativeStopAdvertising___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();
    {
        device_ble_dispose();

        FreeContext(&blecontext);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
