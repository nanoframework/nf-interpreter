//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _SYS_DEV_BLE_NATIVE_H_
#define _SYS_DEV_BLE_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>

#include <string.h>

#include "sys_ble.h"

typedef enum __nfpack BluetoothError
{
    BluetoothError_Success = 0,
    BluetoothError_RadioNotAvailable = 1,
    BluetoothError_ResourceInUse = 2,
    BluetoothError_DeviceNotConnected = 3,
    BluetoothError_OtherError = 4,
    BluetoothError_DisabledByPolicy = 5,
    BluetoothError_NotSupported = 6,
    BluetoothError_DisabledByUser = 7,
    BluetoothError_ConsentRequired = 8,
    BluetoothError_TransportNotSupported = 9,
} BluetoothError;

typedef enum __nfpack BluetoothEventType
{
    BluetoothEventType_Read = 0,
    BluetoothEventType_Write = 1,
    BluetoothEventType_ClientSubscribed = 2,
    BluetoothEventType_ClientUnsubscribed = 3,
} BluetoothEventType;

typedef enum __nfpack GattCharacteristicProperties
{
    GattCharacteristicProperties_None = 0,
    GattCharacteristicProperties_Broadcast = 1,
    GattCharacteristicProperties_Read = 2,
    GattCharacteristicProperties_WriteWithoutResponse = 4,
    GattCharacteristicProperties_Write = 8,
    GattCharacteristicProperties_Notify = 16,
    GattCharacteristicProperties_Indicate = 32,
    GattCharacteristicProperties_AuthenticatedSignedWrites = 64,
    GattCharacteristicProperties_ExtendedProperties = 128,
    GattCharacteristicProperties_ReliableWrites = 256,
    GattCharacteristicProperties_WritableAuxiliaries = 512,
} GattCharacteristicProperties;

typedef enum __nfpack GattCommunicationStatus
{
    GattCommunicationStatus_Success = 0,
    GattCommunicationStatus_Unreachable = 1,
    GattCommunicationStatus_ProtocolError = 2,
    GattCommunicationStatus_AccessDenied = 3,
} GattCommunicationStatus;

typedef enum __nfpack GattProtectionLevel
{
    GattProtectionLevel_Plain = 0,
    GattProtectionLevel_AuthenticationRequired = 1,
    GattProtectionLevel_EncryptionRequired = 2,
    GattProtectionLevel_EncryptionAndAuthenticationRequired = 3,
} GattProtectionLevel;

typedef enum __nfpack GattServiceProviderAdvertisementStatus
{
    GattServiceProviderAdvertisementStatus_Created = 0,
    GattServiceProviderAdvertisementStatus_Stopped = 1,
    GattServiceProviderAdvertisementStatus_Started = 2,
    GattServiceProviderAdvertisementStatus_Aborted = 3,
    GattServiceProviderAdvertisementStatus_StartedWithoutAllAdvertisementData = 4,
} GattServiceProviderAdvertisementStatus;

typedef enum __nfpack GattSessionStatus
{
    GattSessionStatus_Closed = 0,
    GattSessionStatus_Active = 1,
} GattSessionStatus;

typedef enum __nfpack GattWriteOption
{
    GattWriteOption_WriteWithResponse = 0,
    GattWriteOption_WriteWithoutResponse = 1,
} GattWriteOption;

typedef enum __nfpack Utilities_GattNativeDescriptorUuid
{
    Utilities_GattNativeDescriptorUuid_CharacteristicExtendedProperties = 10496,
    Utilities_GattNativeDescriptorUuid_CharacteristicUserDescription = 10497,
    Utilities_GattNativeDescriptorUuid_ClientCharacteristicConfiguration = 10498,
    Utilities_GattNativeDescriptorUuid_ServerCharacteristicConfiguration = 10499,
    Utilities_GattNativeDescriptorUuid_CharacteristicPresentationFormat = 10500,
    Utilities_GattNativeDescriptorUuid_CharacteristicAggregateFormat = 10501,
    Utilities_GattNativeDescriptorUuid_ValidRange = 10502,
    Utilities_GattNativeDescriptorUuid_ExternalReportReference = 10503,
    Utilities_GattNativeDescriptorUuid_ReportReference = 10504,
} Utilities_GattNativeDescriptorUuid;

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothDeviceId
{
    static const int FIELD___id = 1;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothEvent
{
    static const int FIELD__type = 3;
    static const int FIELD__ID = 4;
    static const int FIELD__characteristicId = 5;
    static const int FIELD__descriptorId = 6;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Buffer
{
    static const int FIELD___buffer = 1;
    static const int FIELD___length = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalDescriptorParameters
{
    static const int FIELD___writeProtectionLevel = 1;
    static const int FIELD___readProtectionLevel = 2;
    static const int FIELD___staticValue = 3;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattPresentationFormat
{
    static const int FIELD___formatType = 1;
    static const int FIELD___exponent = 2;
    static const int FIELD___unit = 3;
    static const int FIELD___namespaceId = 4;
    static const int FIELD___description = 5;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattReadRequest
{
    static const int FIELD___readValue = 1;
    static const int FIELD___eventID = 2;

    NANOCLR_NATIVE_DECLARE(NativeReadRespondWithValue___VOID__U2__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeReadRespondWithProtocolError___VOID__U2__U1);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattReadRequestedEventArgs
{
    static const int FIELD___eventID = 1;
    static const int FIELD___session = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattWriteRequestedEventArgs
{
    static const int FIELD___eventID = 1;
    static const int FIELD___session = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalDescriptor
{
    static const int FIELD_STATIC__GattLocalDescriptorIndex = 0;

    static const int FIELD___descriptorId = 1;
    static const int FIELD___charactisic = 2;
    static const int FIELD___writeProtectionLevel = 3;
    static const int FIELD___readProtectionLevel = 4;
    static const int FIELD___uuid = 5;
    static const int FIELD___staticValue = 6;
    static const int FIELD__ReadRequested = 7;
    static const int FIELD__WriteRequested = 8;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattClientNotificationResult
{
    static const int FIELD___result = 1;
    static const int FIELD___status = 2;
    static const int FIELD___client = 3;
    static const int FIELD___bytesSent = 4;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattSubscribedClient
{
    static const int FIELD___session = 1;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattSession
{
    static const int FIELD___deviceId = 1;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattWriteRequest
{
    static const int FIELD___option = 1;
    static const int FIELD___offset = 2;
    static const int FIELD___value = 3;
    static const int FIELD___eventID = 4;

    NANOCLR_NATIVE_DECLARE(NativeWriteGetData___SZARRAY_U1__U2);
    NANOCLR_NATIVE_DECLARE(NativeWriteRespond___VOID__U2);
    NANOCLR_NATIVE_DECLARE(NativeWriteRespondWithProtocolError___VOID__U2__U1);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalCharacteristic
{
    static const int FIELD_STATIC__GattLocalCharacteristicIndex = 1;

    static const int FIELD___characteristicId = 1;
    static const int FIELD___characteristicUuid = 2;
    static const int FIELD___writeProtectionLevel = 3;
    static const int FIELD___readProtectionLevel = 4;
    static const int FIELD___properties = 5;
    static const int FIELD___descriptors = 6;
    static const int FIELD___subscribedClients = 7;
    static const int FIELD___userDescription = 8;
    static const int FIELD___userDescriptionDescriptor = 9;
    static const int FIELD___presentationFormats = 10;
    static const int FIELD___presentationFormatsDescriptors = 11;
    static const int FIELD___staticValue = 12;
    static const int FIELD__ReadRequested = 13;
    static const int FIELD__WriteRequested = 14;
    static const int FIELD__SubscribedClientsChanged = 15;

    NANOCLR_NATIVE_DECLARE(NativeNotifyClient___I4__U2__U2__SZARRAY_U1);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothEventListener
{
    static const int FIELD_STATIC___characteristicMap = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DataReader
{
    static const int FIELD___buffer = 1;
    static const int FIELD___currentReadPosition = 2;

    //--//
};

struct
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalCharacteristicParameters
{
    static const int FIELD___writeProtectionLevel = 1;
    static const int FIELD___readProtectionLevel = 2;
    static const int FIELD___userDescription = 3;
    static const int FIELD___properties = 4;
    static const int FIELD___staticValue = 5;
    static const int FIELD___presentationFormats = 6;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalCharacteristicResult
{
    static const int FIELD___characteristic = 1;
    static const int FIELD___error = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalDescriptorResult
{
    static const int FIELD___error = 1;
    static const int FIELD___descriptor = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalService
{
    static const int FIELD___serviceUuid = 1;
    static const int FIELD___characteristics = 2;

    //--//
};

struct
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProviderAdvertisingParameters
{
    static const int FIELD___deviceName = 1;
    static const int FIELD___isDiscoverable = 2;
    static const int FIELD___isConnectable = 3;
    static const int FIELD___serviceData = 4;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider
{
    static const int FIELD_STATIC___bluetoothEventManager = 3;

    static const int FIELD___service = 1;
    static const int FIELD___status = 2;
    static const int FIELD___deviceName = 3;
    static const int FIELD___isDiscoverable = 4;
    static const int FIELD___isConnectable = 5;
    static const int FIELD___serviceData = 6;

    NANOCLR_NATIVE_DECLARE(NativeInitService___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeStartAdvertising___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeStopAdvertising___VOID);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProviderResult
{
    static const int FIELD___serviceProvider = 1;
    static const int FIELD___error = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Spp_SppReceivedDataEventArgs
{
    static const int FIELD___data = 1;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Spp_NordicSpp
{
    static const int FIELD__ServiceUUID = 1;
    static const int FIELD__RxCharacteristicUUID = 2;
    static const int FIELD__TxCharacteristicUUID = 3;
    static const int FIELD___serviceProvider = 4;
    static const int FIELD___txCharacteristic = 5;
    static const int FIELD___isConnected = 6;
    static const int FIELD__ReceivedData = 7;
    static const int FIELD__ConnectedEvent = 8;

    //--//
};

struct Library_sys_dev_ble_native_ThisAssembly
{
    static const int FIELD_STATIC__GitCommitDate = 4;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Device_Bluetooth;

#endif //_SYS_DEV_BLE_NATIVE_H_
