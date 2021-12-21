//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOHAL_CONFIG_MANAGER_H
#define NANOHAL_CONFIG_MANAGER_H

#include <nanoCLR_Headers.h>
#include <nanoHAL_Network.h>

#ifdef __cplusplus
extern "C"
{
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.DeviceConfiguration.DeviceConfigurationOption (in managed
    // code) !!! //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef enum DeviceConfigurationOption
    {
        // Network configuration block
        DeviceConfigurationOption_Network = 1,

        // Wireless 802.11 Network configuration block
        DeviceConfigurationOption_Wireless80211Network = 2,

        // Wireless Network as AP configuration block
        DeviceConfigurationOption_WirelessNetworkAP = 3,

        // X509 CA Root Certificates bundle block
        DeviceConfigurationOption_X509CaRootBundle = 4,

        // X509 Device Certificates
        DeviceConfigurationOption_X509DeviceCertificates = 5,

        // All configuration blocks
        DeviceConfigurationOption_All = 255,

    } DeviceConfigurationOption;

    // network interface configuration struct
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_NETWORK
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the network interface configuration
        HAL_Configuration_NetworkInterface *Configs[];

    } HAL_CONFIGURATION_NETWORK;

    // network wireless interface configuration struct
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_NETWORK_WIRELESS80211
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the wireless network interface configuration
        HAL_Configuration_Wireless80211 *Configs[];

    } HAL_CONFIGURATION_NETWORK_WIRELESS80211;

    // network wireless interface configuration struct
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_NETWORK_WIRELESSAP
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the wireless network interface configuration
        HAL_Configuration_WirelessAP *Configs[];

    } HAL_CONFIGURATION_NETWORK_WIRELESSAP;

    // certificate store struct
    // device certificate(s)
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_X509_CERTIFICATE
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the certificates
        HAL_Configuration_X509CaRootBundle *Certificates[];

    } HAL_CONFIGURATION_X509_CERTIFICATE;

    // device certificates
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the certificates
        HAL_Configuration_X509DeviceCertificate *Certificates[];

    } HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE;

    // target configuration storage struct
    // the memory allocation for these will have to be done as required according to the number and type of blocks found
    // in memory
    typedef struct HAL_TARGET_CONFIGURATION
    {
        HAL_CONFIGURATION_NETWORK *NetworkInterfaceConfigs;
        HAL_CONFIGURATION_NETWORK_WIRELESS80211 *Wireless80211Configs;
        HAL_CONFIGURATION_NETWORK_WIRELESSAP *WirelessAPConfigs;
        HAL_CONFIGURATION_X509_CERTIFICATE *CertificateStore;
        HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *DeviceCertificates;

    } HAL_TARGET_CONFIGURATION;

    // Initialization of configuration manager
    void ConfigurationManager_Initialize();

    // Enumerates the configuration blocks
    // This is provided as a separate function to allow it to be called when there is a need to re-enumerate the blocks
    // for example when a new block is added from the debug or from the managed application
    void ConfigurationManager_EnumerateConfigurationBlocks();

    // GetConfigurationBlock() is defined in targetHAL_ConfigurationManager.cpp at target level because the target
    // needs to be free to implement the storage of the configuration block as they see fit
    bool ConfigurationManager_GetConfigurationBlock(
        void *configurationBlock,
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex);

    // StoreConfigurationBlock() is defined in targetHAL_ConfigurationManager.cpp at target level because the target
    // needs to be free to implement the storage of the configuration block as they see fit
    // X509 certs can be large and are usually received in chunks. In this case, the block size parameter is the size of
    // the current chunck.
    bool ConfigurationManager_StoreConfigurationBlock(
        void *configurationBlock,
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex,
        uint32_t blockSize,
        uint32_t offset,
        bool done);

    // UpdateConfigurationBlock() is defined in targetHAL_ConfigurationManager.cpp at target level because the target
    // needs to be free to implement the storage of the configuration block as they see fit
    bool ConfigurationManager_UpdateConfigurationBlock(
        void *configurationBlock,
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex);

    // Checks if two configuration blocks are the same. Usefull to prevent going through the store process when the
    // content is the same. Default implementation provided for memory addreasable implementations. defined as weak so
    // targets are free to implement the storage of the configuration block as they see fit
    bool ConfigurationManager_CheckExistingConfigurationBlock(
        void *existingConfigBlock,
        void *newConfigBlock,
        uint32_t existingConfigBlockSize,
        uint32_t newConfigBlockSize);

    //  Default initialisation for wireless config block
    void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *pconfig, uint32_t configurationIndex);

    //  Default initialisation for wireless AP config block
    void InitialiseWirelessAPDefaultConfig(HAL_Configuration_WirelessAP *pconfig, uint32_t configurationIndex);

    //  Default initialisation for Network interface config blocks
    // returns FALSE if it's not possible to create a default config block
    bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *pconfig, uint32_t configurationIndex);

    // helper functions

    // function that sweeps a memory region searching for network configuration blocks
    void *ConfigurationManager_FindNetworkConfigurationBlocks(uint32_t startAddress, uint32_t endAddress);

    // function that sweeps a memory region searching for wireless network configuration blocks
    void *ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(uint32_t startAddress, uint32_t endAddress);

    // function that sweeps a memory region searching for X509 certificates configuration blocks
    void *ConfigurationManager_FindX509CertificateConfigurationBlocks(uint32_t startAddress, uint32_t endAddress);

    // function that sweeps a memory region searching for X509 device certificates configuration blocks
    void *ConfigurationManager_FindX509DeviceCertificatesConfigurationBlocks(
        uint32_t startAddress,
        uint32_t endAddress);

    // gets the HAL_Configuration_Wireless80211 configuration block that has the specified Id, if that exists
    // defined as weak needs to be free to implement the storage of the configuration block as they see fit
    HAL_Configuration_Wireless80211 *ConfigurationManager_GetWirelessConfigurationFromId(uint32_t configurationId);

    // gets the HAL_Configuration_WirelessAP configuration block that has the specified Id, if that exists
    // memory is allocated for the configuration block, has to be free by the caller
    // defined as weak to allow replacement at platform/target level to allow different storage management
    HAL_Configuration_WirelessAP *ConfigurationManager_GetWirelessAPConfigurationFromId(uint32_t configurationId);

    // gets the HAL_Configuration_X509CaRootBundle certificate store, if that exists
    // memory is allocated for the configuration block, has to be free by the caller
    // defined as weak to allow replacement at platform/target level to allow different storage management
    HAL_Configuration_X509CaRootBundle *ConfigurationManager_GetCertificateStore();

    // gets the HAL_Configuration_X509DeviceCertificate device certificate, if that exists
    // memory is allocated for the configuration block, has to be free by the caller
    // defined as weak to allow replacement at platform/target level to allow different storage management
    HAL_Configuration_X509DeviceCertificate *ConfigurationManager_GetDeviceCertificate();

#ifdef __cplusplus
}
#endif

// declaration of Target configuration union as external, has to be provided at target level
extern HAL_TARGET_CONFIGURATION g_TargetConfiguration;

#endif // NANOHAL_CONFIG_MANAGER_H
