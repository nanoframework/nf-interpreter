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

typedef enum __nfpack BluetoothLEAdvertisementDataSectionType
{
    BluetoothLEAdvertisementDataSectionType_AdvertisingInterval = 26,
    BluetoothLEAdvertisementDataSectionType_Appearance = 25,
    BluetoothLEAdvertisementDataSectionType_CompleteLocalName = 9,
    BluetoothLEAdvertisementDataSectionType_CompleteList128uuid = 7,
    BluetoothLEAdvertisementDataSectionType_CompleteList16uuid = 3,
    BluetoothLEAdvertisementDataSectionType_CompleteList32uuid = 5,
    BluetoothLEAdvertisementDataSectionType_Flags = 1,
    BluetoothLEAdvertisementDataSectionType_IncompleteList128uuid = 6,
    BluetoothLEAdvertisementDataSectionType_IncompleteList16uuid = 2,
    BluetoothLEAdvertisementDataSectionType_IncompleteList32uuid = 4,
    BluetoothLEAdvertisementDataSectionType_ManufacturerSpecificData = 255,
    BluetoothLEAdvertisementDataSectionType_PeripheralConnectionIntervalRange = 18,
    BluetoothLEAdvertisementDataSectionType_PublicTargetAddress = 23,
    BluetoothLEAdvertisementDataSectionType_RandomTargetAddress = 24,
    BluetoothLEAdvertisementDataSectionType_ServiceData128bitUuid = 33,
    BluetoothLEAdvertisementDataSectionType_ServiceData16bitUuid = 22,
    BluetoothLEAdvertisementDataSectionType_ServiceData32bitUuid = 32,
    BluetoothLEAdvertisementDataSectionType_ServiceSolicitation128BitUuids = 21,
    BluetoothLEAdvertisementDataSectionType_ServiceSolicitation16BitUuids = 20,
    BluetoothLEAdvertisementDataSectionType_ServiceSolicitation32BitUuids = 31,
    BluetoothLEAdvertisementDataSectionType_ShortenedLocalName = 8,
    BluetoothLEAdvertisementDataSectionType_TxPowerLevel = 10,
} BluetoothLEAdvertisementDataSectionType;

typedef enum __nfpack BluetoothLEAdvertisementFlags
{
    BluetoothLEAdvertisementFlags_None = 0,
    BluetoothLEAdvertisementFlags_LimitedDiscoverableMode = 1,
    BluetoothLEAdvertisementFlags_GeneralDiscoverableMode = 2,
    BluetoothLEAdvertisementFlags_ClassicNotSupported = 4,
    BluetoothLEAdvertisementFlags_DualModeControllerCapable = 8,
    BluetoothLEAdvertisementFlags_DualModeHostCapable = 16,
} BluetoothLEAdvertisementFlags;

typedef enum __nfpack BluetoothLEAdvertisementPublisherStatus
{
    BluetoothLEAdvertisementPublisherStatus_Created = 0,
    BluetoothLEAdvertisementPublisherStatus_Waiting = 1,
    BluetoothLEAdvertisementPublisherStatus_Started = 2,
    BluetoothLEAdvertisementPublisherStatus_Stopping = 3,
    BluetoothLEAdvertisementPublisherStatus_Stopped = 4,
    BluetoothLEAdvertisementPublisherStatus_Aborted = 5,
} BluetoothLEAdvertisementPublisherStatus;

typedef enum __nfpack BluetoothLEAdvertisementType
{
    BluetoothLEAdvertisementType_ConnectableUndirected = 0,
    BluetoothLEAdvertisementType_ConnectableDirected = 1,
    BluetoothLEAdvertisementType_ScannableUndirected = 2,
    BluetoothLEAdvertisementType_NonConnectableUndirected = 3,
    BluetoothLEAdvertisementType_ScanResponse = 4,
} BluetoothLEAdvertisementType;

typedef enum __nfpack BluetoothLEScanningMode
{
    BluetoothLEScanningMode_Passive = 0,
    BluetoothLEScanningMode_Active = 1,
} BluetoothLEScanningMode;

typedef enum __nfpack BluetoothAddressType
{
    BluetoothAddressType_Public = 0,
    BluetoothAddressType_Random = 1,
    BluetoothAddressType_Unspecified = 2,
} BluetoothAddressType;

typedef enum __nfpack BluetoothConnectionStatus
{
    BluetoothConnectionStatus_Disconnected = 0,
    BluetoothConnectionStatus_Connected = 1,
} BluetoothConnectionStatus;

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
    BluetoothEventType_AdvertisementDiscovered = 4,
    BluetoothEventType_ScanningComplete = 5,
    BluetoothEventType_ConnectComplete = 6,
    BluetoothEventType_ConnectionDisconnected = 7,
    BluetoothEventType_ServiceDiscovered = 8,
    BluetoothEventType_ServiceDiscoveryComplete = 9,
    BluetoothEventType_CharacteristicDiscovered = 10,
    BluetoothEventType_CharacteristicDiscoveryComplete = 11,
    BluetoothEventType_DescriptorDiscovered = 12,
    BluetoothEventType_DescriptorDiscoveryComplete = 13,
    BluetoothEventType_AttributeReadValueComplete = 14,
    BluetoothEventType_AttributeWriteValueComplete = 15,
    BluetoothEventType_AttributeValueChanged = 16,
    BluetoothEventType_ClientConnected = 17,
    BluetoothEventType_ClientDisconnected = 18,
    BluetoothEventType_ClientSessionChanged = 19,
    BluetoothEventType_PassKeyActions = 20,
    BluetoothEventType_PassKeyActionsNumericComparison = 21,
    BluetoothEventType_AuthenticationComplete = 22,
} BluetoothEventType;

typedef enum __nfpack BluetoothLEAdvertisementWatcherStatus
{
    BluetoothLEAdvertisementWatcherStatus_Created = 0,
    BluetoothLEAdvertisementWatcherStatus_Started = 1,
    BluetoothLEAdvertisementWatcherStatus_Stopping = 2,
    BluetoothLEAdvertisementWatcherStatus_Stopped = 3,
    BluetoothLEAdvertisementWatcherStatus_Aborted = 4,
} BluetoothLEAdvertisementWatcherStatus;

typedef enum __nfpack BluetoothLEDevice_readWriteValueResult
{
    BluetoothLEDevice_readWriteValueResult_success = 0,
    BluetoothLEDevice_readWriteValueResult_acessDenied = 1,
    BluetoothLEDevice_readWriteValueResult_failure = 2,
} BluetoothLEDevice_readWriteValueResult;

typedef enum __nfpack BluetoothNanoDevice_Mode
{
    BluetoothNanoDevice_Mode_NotRunning = 0,
    BluetoothNanoDevice_Mode_Server = 1,
    BluetoothNanoDevice_Mode_Client = 2,
} BluetoothNanoDevice_Mode;

typedef enum __nfpack DevicePairingIOCapabilities
{
    DevicePairingIOCapabilities_DisplayOnly = 0,
    DevicePairingIOCapabilities_DisplayYesNo = 1,
    DevicePairingIOCapabilities_KeyboardOnly = 2,
    DevicePairingIOCapabilities_NoInputNoOutput = 3,
    DevicePairingIOCapabilities_KeyboardDisplay = 4,
} DevicePairingIOCapabilities;

typedef enum __nfpack DevicePairingKinds
{
    DevicePairingKinds_None = 0,
    DevicePairingKinds_ConfirmOnly = 1,
    DevicePairingKinds_DisplayPin = 2,
    DevicePairingKinds_ProvidePin = 4,
    DevicePairingKinds_ConfirmPinMatch = 8,
} DevicePairingKinds;

typedef enum __nfpack DevicePairingProtectionLevel
{
    DevicePairingProtectionLevel_Default = 0,
    DevicePairingProtectionLevel_None = 1,
    DevicePairingProtectionLevel_Encryption = 2,
    DevicePairingProtectionLevel_EncryptionAndAuthentication = 3,
} DevicePairingProtectionLevel;

typedef enum __nfpack DevicePairingResultStatus
{
    DevicePairingResultStatus_Paired = 0,
    DevicePairingResultStatus_NotReadyToPair = 1,
    DevicePairingResultStatus_NotPaired = 2,
    DevicePairingResultStatus_AlreadyPaired = 3,
    DevicePairingResultStatus_ConnectionRejected = 4,
    DevicePairingResultStatus_TooManyConnections = 5,
    DevicePairingResultStatus_HardwareFailure = 6,
    DevicePairingResultStatus_AuthenticationTimeout = 7,
    DevicePairingResultStatus_AuthenticationNotAllowed = 8,
    DevicePairingResultStatus_AuthenticationFailure = 9,
    DevicePairingResultStatus_NoSupportedProfiles = 10,
    DevicePairingResultStatus_ProtectionLevelCouldNotBeMet = 11,
    DevicePairingResultStatus_AccessDenied = 12,
    DevicePairingResultStatus_InvalidCeremonyData = 13,
    DevicePairingResultStatus_PairingCanceled = 14,
    DevicePairingResultStatus_OperationAlreadyInProgress = 15,
    DevicePairingResultStatus_RequiredHandlerNotRegistered = 16,
    DevicePairingResultStatus_RejectedByHandler = 17,
    DevicePairingResultStatus_RemoteDeviceHasAssociation = 18,
    DevicePairingResultStatus_Failed = 19,
} DevicePairingResultStatus;

typedef enum __nfpack DeviceUnpairingResultStatus
{
    DeviceUnpairingResultStatus_Unpaired = 0,
    DeviceUnpairingResultStatus_AlreadyUnpaired = 1,
    DeviceUnpairingResultStatus_OperationAlreadyInProgress = 2,
    DeviceUnpairingResultStatus_AccessDenied = 3,
    DeviceUnpairingResultStatus_Failed = 4,
} DeviceUnpairingResultStatus;

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

typedef enum __nfpack GattClientCharacteristicConfigurationDescriptorValue
{
    GattClientCharacteristicConfigurationDescriptorValue_None = 0,
    GattClientCharacteristicConfigurationDescriptorValue_Notify = 1,
    GattClientCharacteristicConfigurationDescriptorValue_Indicate = 2,
} GattClientCharacteristicConfigurationDescriptorValue;

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
    Utilities_GattNativeDescriptorUuid_NumberDigitals = 10505,
    Utilities_GattNativeDescriptorUuid_ValueTriggerSetting = 10506,
    Utilities_GattNativeDescriptorUuid_EssConfiguration = 10507,
    Utilities_GattNativeDescriptorUuid_EssMeasurement = 10508,
    Utilities_GattNativeDescriptorUuid_EssTriggerSetting = 10509,
    Utilities_GattNativeDescriptorUuid_TimeTriggerSetting = 10510,
} Utilities_GattNativeDescriptorUuid;

typedef enum __nfpack Utilities_UuidType
{
    Utilities_UuidType_Uuid16 = 0,
    Utilities_UuidType_Uuid32 = 1,
    Utilities_UuidType_Uuid128 = 2,
} Utilities_UuidType;

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Buffer
{
    static const int FIELD___buffer = 1;
    static const int FIELD___length = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisementDataSection
{
    static const int FIELD___dataType = 1;
    static const int FIELD___buffer = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DataReader
{
    static const int FIELD___buffer = 1;
    static const int FIELD___currentReadPosition = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisement
{
    static const int FIELD___flags = 1;
    static const int FIELD___localName = 2;
    static const int FIELD___serviceUuids = 3;
    static const int FIELD___dataSections = 4;
    static const int FIELD___manufacturerData = 5;
    static const int FIELD___isConnectable = 6;
    static const int FIELD___isDiscovable = 7;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisementBytePattern
{
    static const int FIELD___dataType = 1;
    static const int FIELD___data = 2;
    static const int FIELD___offset = 3;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisementFilter
{
    static const int FIELD___advertisement = 1;
    static const int FIELD___bytePatterns = 2;

    //--//
};

struct
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisementPublisherStatusChangedEventArgs
{
    static const int FIELD___error = 1;
    static const int FIELD___status = 2;
    static const int FIELD___selectedTransmitPowerLevelInDBm = 3;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothNanoDevice
{
    static const int FIELD_STATIC___deviceName = 0;
    static const int FIELD_STATIC___appearance = 1;
    static const int FIELD_STATIC___mode = 2;

    NANOCLR_NATIVE_DECLARE(NativeInitilise___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(
        NativeSetOperationMode___STATIC__VOID__nanoFrameworkDeviceBluetoothBluetoothNanoDeviceMode__STRING__U2);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothDeviceId
{
    static const int FIELD___id = 1;

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

struct
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattSessionStatusChangedEventArgs
{
    static const int FIELD___status = 1;
    static const int FIELD___bluetoothError = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattSession
{
    static const int FIELD___deviceId = 1;
    static const int FIELD___maxMtuSize = 2;
    static const int FIELD__SessionStatusChanged = 3;
    static const int FIELD__MaxPduSizeChanged = 4;

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
    static const int FIELD_STATIC__GattLocalCharacteristicIndex = 3;

    static const int FIELD___characteristicId = 1;
    static const int FIELD___descriptorNextID = 2;
    static const int FIELD___characteristicUuid = 3;
    static const int FIELD___writeProtectionLevel = 4;
    static const int FIELD___readProtectionLevel = 5;
    static const int FIELD___properties = 6;
    static const int FIELD___descriptors = 7;
    static const int FIELD___subscribedClients = 8;
    static const int FIELD___userDescription = 9;
    static const int FIELD___userDescriptionDescriptor = 10;
    static const int FIELD___presentationFormats = 11;
    static const int FIELD___presentationFormatsDescriptors = 12;
    static const int FIELD___staticValue = 13;
    static const int FIELD__ReadRequested = 14;
    static const int FIELD__WriteRequested = 15;
    static const int FIELD__SubscribedClientsChanged = 16;

    NANOCLR_NATIVE_DECLARE(NativeNotifyClient___I4__U2__U2__SZARRAY_U1);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalService
{
    static const int FIELD___serviceUuid = 1;
    static const int FIELD___characteristics = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairingResult
{
    static const int FIELD___protectionLevelUsed = 1;
    static const int FIELD___status = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothEventSesssion
{
    static const int FIELD__type = 3;
    static const int FIELD__connectionHandle = 4;
    static const int FIELD__status = 5;
    static const int FIELD__data = 6;
    static const int FIELD__data32 = 7;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairingEventArgs
{
    static const int FIELD___connectionHandle = 1;
    static const int FIELD___status = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairingRequestedEventArgs
{
    static const int FIELD___pairing = 1;
    static const int FIELD___connectionHandle = 2;
    static const int FIELD___kind = 3;
    static const int FIELD___pin = 4;

    NANOCLR_NATIVE_DECLARE(NativeAcceptYesNo___U2__U2__nanoFrameworkDeviceBluetoothDevicePairingKinds__I4);
    NANOCLR_NATIVE_DECLARE(NativeAcceptPasskey___U2__U2__nanoFrameworkDeviceBluetoothDevicePairingKinds__I4);
    NANOCLR_NATIVE_DECLARE(
        NativeAcceptCredentials___U2__U2__nanoFrameworkDeviceBluetoothDevicePairingKinds__SZARRAY_U1__SZARRAY_U1);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DevicePairing
{
    static const int FIELD___device = 1;
    static const int FIELD___server = 2;
    static const int FIELD___completedEvent = 3;
    static const int FIELD___canPair = 4;
    static const int FIELD___isPaired = 5;
    static const int FIELD___isAuthenticated = 6;
    static const int FIELD___isSecure = 7;
    static const int FIELD___ioCapabilities = 8;
    static const int FIELD___protectionLevel = 9;
    static const int FIELD___bondingAllowed = 10;
    static const int FIELD___pairingStatus = 11;
    static const int FIELD___outOfBand = 12;
    static const int FIELD__PairingRequested = 13;
    static const int FIELD__PairingComplete = 14;

    NANOCLR_NATIVE_DECLARE(NativeStartPair___U2__U2);
    NANOCLR_NATIVE_DECLARE(NativeSetPairAttributes___VOID);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEServer
{
    static const int FIELD_STATIC___instance = 4;
    static const int FIELD_STATIC___lock = 5;
    static const int FIELD_STATIC___bluetoothEventManager = 6;
    static const int FIELD_STATIC___services = 7;

    static const int FIELD___disposedValue = 1;
    static const int FIELD___pairing = 2;
    static const int FIELD___session = 3;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisementPublisher
{
    static const int FIELD_STATIC___advertisementInstance = 8;

    static const int FIELD___advertisement = 1;
    static const int FIELD___status = 2;
    static const int FIELD___useExtendedAdvertisement = 3;
    static const int FIELD___includeTransmitPowerLevel = 4;
    static const int FIELD___isAnonymous = 5;
    static const int FIELD___preferredTransmitPowerLevelInDBm = 6;
    static const int FIELD___dataNotFitInAdvertisement = 7;
    static const int FIELD__StatusChanged = 8;

    NANOCLR_NATIVE_DECLARE(NativeStartLegacyAdvertising___BOOLEAN__SZARRAY_U1__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeStartExtendedAdvertising___BOOLEAN__I2__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeStopAdvertising___VOID);
    NANOCLR_NATIVE_DECLARE(NativeIsExtendedAdvertisingAvailable___BOOLEAN);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisementReceivedEventArgs
{
    static const int FIELD___bluetoothAddress = 1;
    static const int FIELD___bluetoothAddressType = 2;
    static const int FIELD___advertisement = 3;
    static const int FIELD___advertisementType = 4;
    static const int FIELD___rawSignalStrengthInDBm = 5;
    static const int FIELD___timestamp = 6;
    static const int FIELD___rawAdvertData = 7;

    NANOCLR_NATIVE_DECLARE(NativeCreateFromEvent___BOOLEAN__I4);

    //--//
};

struct
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEAdvertisementWatcherStoppedEventArgs
{
    static const int FIELD___error = 1;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Advertisement_BluetoothLEManufacturerData
{
    static const int FIELD___data = 1;
    static const int FIELD___companyId = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothAddress
{
    static const int FIELD___address = 1;
    static const int FIELD___addressType = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothEventCentral
{
    static const int FIELD__type = 3;
    static const int FIELD__connectionHandle = 4;
    static const int FIELD__status = 5;
    static const int FIELD__serviceHandle = 6;
    static const int FIELD__characteristicHandle = 7;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattDeviceServicesResult
{
    static const int FIELD___protocolError = 1;
    static const int FIELD___services = 2;
    static const int FIELD___status = 3;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattCharacteristicsResult
{
    static const int FIELD___protocolError = 1;
    static const int FIELD___characteristics = 2;
    static const int FIELD___status = 3;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattReadResult
{
    static const int FIELD___status = 1;
    static const int FIELD___protocolError = 2;
    static const int FIELD___buffer = 3;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattWriteResult
{
    static const int FIELD___status = 1;
    static const int FIELD___protocolError = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattDescriptorsResult
{
    static const int FIELD___protocolError = 1;
    static const int FIELD___descriptors = 2;
    static const int FIELD___status = 3;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattValueChangedEventArgs
{
    static const int FIELD___buffer = 1;
    static const int FIELD___timeStammp = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattDescriptor
{
    static const int FIELD___attributeHandle = 1;
    static const int FIELD___uuid = 2;
    static const int FIELD___protectionLevel = 3;
    static const int FIELD___device = 4;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattCharacteristic
{
    static const int FIELD___service = 1;
    static const int FIELD___attributeHandle = 2;
    static const int FIELD___lastHandle = 3;
    static const int FIELD___uuid = 4;
    static const int FIELD___descriptors = 5;
    static const int FIELD___presentationFormats = 6;
    static const int FIELD___userDescription = 7;
    static const int FIELD___characteristicProperties = 8;
    static const int FIELD___cccdDescriptorHandle = 9;
    static const int FIELD___protectionLevel = 10;
    static const int FIELD___completedEvent = 11;
    static const int FIELD__ValueChanged = 12;

    NANOCLR_NATIVE_DECLARE(NativeStartDiscoveryDescriptors___U2__U2__U2__U2);
    NANOCLR_NATIVE_DECLARE(
        NativeUpdateDescriptor___VOID__U2__U2__U2__nanoFrameworkDeviceBluetoothGenericAttributeProfileGattDescriptor);
    NANOCLR_NATIVE_DECLARE(NativeReadEventData___SZARRAY_U1__U2__U2);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattDeviceService
{
    static const int FIELD___attributeHandle = 1;
    static const int FIELD___device = 2;
    static const int FIELD___characteristics = 3;
    static const int FIELD___uuid = 4;
    static const int FIELD___startHandle = 5;
    static const int FIELD___endHandle = 6;
    static const int FIELD___eventComplete = 7;

    NANOCLR_NATIVE_DECLARE(NativeDiscoverCharacteristics___U2__U2__U2__U2__U2);
    NANOCLR_NATIVE_DECLARE(
        NativeUpdateCharacteristic___VOID__U2__U2__U2__nanoFrameworkDeviceBluetoothGenericAttributeProfileGattCharacteristic);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEDevice
{
    static const int FIELD_STATIC___routingHandle = 9;

    static const int FIELD___connectionStatus = 1;
    static const int FIELD___bluetoothAddress = 2;
    static const int FIELD___addressType = 3;
    static const int FIELD___name = 4;
    static const int FIELD___appearance = 5;
    static const int FIELD___connectionHandle = 6;
    static const int FIELD___genericAccessRead = 7;
    static const int FIELD___services = 8;
    static const int FIELD___attributes = 9;
    static const int FIELD___pairing = 10;
    static const int FIELD___disposed = 11;
    static const int FIELD___eventComplete = 12;
    static const int FIELD___completedEvent = 13;
    static const int FIELD__lockObj = 14;
    static const int FIELD___eventValue = 15;
    static const int FIELD___eventStatus = 16;
    static const int FIELD__ConnectionStatusChanged = 17;
    static const int FIELD__GattServicesChanged = 18;

    NANOCLR_NATIVE_DECLARE(NativeConnect___U2__U8__nanoFrameworkDeviceBluetoothBluetoothAddressType__U2);
    NANOCLR_NATIVE_DECLARE(NativeDisconnect___VOID__U2);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID__U2);
    NANOCLR_NATIVE_DECLARE(NativeDiscoverServices___U2__U2);
    NANOCLR_NATIVE_DECLARE(
        NativeUpdateService___VOID__U2__nanoFrameworkDeviceBluetoothGenericAttributeProfileGattDeviceService);
    NANOCLR_NATIVE_DECLARE(NativeStartReadValue___U2__U2__U2);
    NANOCLR_NATIVE_DECLARE(NativeReadValue___SZARRAY_U1__U2__U2);
    NANOCLR_NATIVE_DECLARE(NativeStartWriteValue___U2__U2__U2__BOOLEAN__SZARRAY_U1__U2);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothEventServer
{
    static const int FIELD__type = 3;
    static const int FIELD__id = 4;
    static const int FIELD__characteristicId = 5;
    static const int FIELD__descriptorId = 6;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothEventScan
{
    static const int FIELD__type = 3;
    static const int FIELD__id = 4;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothSignalStrengthFilter__ScanItem
{
    static const int FIELD__Rssi = 1;
    static const int FIELD__InRange = 2;
    static const int FIELD__OutRangeTime = 3;
    static const int FIELD__Active = 4;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothSignalStrengthFilter
{
    static const int FIELD___inRangeThresholdInDBm = 1;
    static const int FIELD___outOfRangeThresholdInDBm = 2;
    static const int FIELD___outOfRangeTimeout = 3;
    static const int FIELD___scanCheck = 4;
    static const int FIELD___scanResults = 5;
    static const int FIELD___scanResultsLock = 6;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothLEAdvertisementWatcher
{
    static const int FIELD___status = 1;
    static const int FIELD___scanningMode = 2;
    static const int FIELD___advertisementFilter = 3;
    static const int FIELD___signalStrengthFilter = 4;
    static const int FIELD__Received = 5;
    static const int FIELD__Stopped = 6;

    NANOCLR_NATIVE_DECLARE(NativeStartAdvertisementWatcher___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeStopAdvertisementWatcher___VOID);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothEventListener
{
    static const int FIELD_STATIC___server = 10;
    static const int FIELD_STATIC___characteristicMap = 11;
    static const int FIELD_STATIC___leDeviceMap = 12;
    static const int FIELD_STATIC___watcher = 13;
    static const int FIELD_STATIC___watcherEvent = 14;
    static const int FIELD_STATIC___watcherQueue = 15;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_DeviceUnpairingResult
{
    static const int FIELD___status = 1;

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

struct
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattReadClientCharacteristicConfigurationDescriptorResult
{
    static const int FIELD___config = 1;
    static const int FIELD___protocolError = 2;
    static const int FIELD___status = 3;

    //--//
};

struct
    Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProviderAdvertisingParameters
{
    static const int FIELD___isDiscoverable = 1;
    static const int FIELD___isConnectable = 2;
    static const int FIELD___serviceData = 3;
    static const int FIELD___advertisement = 4;
    static const int FIELD___customAdvertisement = 5;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Utilities
{
    static const int FIELD_STATIC__baseUuid = 16;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider
{
    static const int FIELD___status = 1;
    static const int FIELD___service = 2;
    static const int FIELD___publisher = 3;

    NANOCLR_NATIVE_DECLARE(NativeInitializeServiceConfig___BOOLEAN__SystemCollectionsArrayList);
    NANOCLR_NATIVE_DECLARE(NativeDisposeServiceConfig___VOID);

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProviderResult
{
    static const int FIELD___serviceProvider = 1;
    static const int FIELD___error = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_PasswordCredential
{
    static const int FIELD___username = 1;
    static const int FIELD___password = 2;

    //--//
};

struct Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_Security_DeviceBonding
{
    NANOCLR_NATIVE_DECLARE(IsBonded___STATIC__BOOLEAN__U8);
    NANOCLR_NATIVE_DECLARE(DeleteAllBonds___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(DeleteBondForPeer___STATIC__VOID__U8__nanoFrameworkDeviceBluetoothBluetoothAddressType);
    NANOCLR_NATIVE_DECLARE(Count___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(GetBondInformationAt___STATIC__U8__I4);

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

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Device_Bluetooth;

#include "sys_ble.h"

// clang-format on

#endif //_SYS_DEV_BLE_NATIVE_H_
