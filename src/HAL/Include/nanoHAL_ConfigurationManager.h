//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_CONFIG_MANAGER_H_
#define _NANOHAL_CONFIG_MANAGER_H_ 1

#include <nanoWeak.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.DeviceConfiguration.DeviceConfigurationOption (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum DeviceConfigurationOption
{
    // Network configuration block
    DeviceConfigurationOption_Network = 1,

    // Wireless 802.11 Network configuration block
    DeviceConfigurationOption_Wireless80211Network = 2,

    // Wireless Network as AP configuration block
    DeviceConfigurationOption_WirelessNetworkAP = 3,

    // All configuration blocks
    DeviceConfigurationOption_All = 255,

} DeviceConfigurationOption;


// network interface configuration struct
// declared with a flexible array member to allow N config blocks totaly independent of compilation
typedef struct HAL_CONFIGURATION_NETWORK
{
    // count of the configs elements
    uint8_t Count;

    // pointer to the network interface configuration
    HAL_Configuration_NetworkInterface* Configs[];

} HAL_CONFIGURATION_NETWORK;

// network wireless interface configuration struct
// declared with a flexible array member to allow N config blocks totaly independent of compilation
typedef struct HAL_CONFIGURATION_NETWORK_WIRELESS80211
{
    // count of the configs elements
    uint8_t Count;

    // pointer to the wireless network interface configuration
    HAL_Configuration_Wireless80211NetworkInterface* Configs[];

} HAL_CONFIGURATION_NETWORK_WIRELESS80211;

// target configuration storage struct
// the memory allocation for these will have to be done as required according to the number and type of blocks found in memory
typedef struct HAL_TARGET_CONFIGURATION
{
    HAL_CONFIGURATION_NETWORK* NetworkInterfaceConfigs;
    HAL_CONFIGURATION_NETWORK_WIRELESS80211* NetworkWireless80211InterfaceConfigs;

} HAL_TARGET_CONFIGURATION;


// declaration of Target configuration union as external, has to be provided at target level
extern HAL_TARGET_CONFIGURATION g_TargetConfiguration;


#ifdef __cplusplus
extern "C" {
#endif

// Initialization of configuration manager
void ConfigurationManager_Initialize();

// Enumerates the configuration blocks
// This is provided as a separate function to allow it to be called when there is a need to re-enumerate the blocks
// for example when a new block is added from the debug or from the managed application
void ConfigurationManager_EnumerateConfigurationBlocks();

// GetConfigurationBlock() is defined in targetHAL_ConfigurationManager.cpp at target level because the target 
// needs to be free to implement the storage of the configuration block as they see fit
__nfweak bool ConfigurationManager_GetConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex);

// StoreConfigurationBlock() is defined in targetHAL_ConfigurationManager.cpp at target level because the target 
// needs to be free to implement the storage of the configuration block as they see fit
__nfweak bool ConfigurationManager_StoreConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex, uint32_t blockSize);

// UpdateConfigurationBlock() is defined in targetHAL_ConfigurationManager.cpp at target level because the target 
// needs to be free to implement the storage of the configuration block as they see fit
__nfweak bool ConfigurationManager_UpdateConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex);

// helper functions

// function that sweeps a memory region searching for network configuration blocks
HAL_CONFIGURATION_NETWORK* ConfigurationManager_FindNetworkConfigurationBlocks(uint32_t startAddress, uint32_t storageSize);

// function that sweeps a memory region searching for wireless network configuration blocks
HAL_CONFIGURATION_NETWORK_WIRELESS80211* ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(uint32_t startAddress, uint32_t storageSize);


#ifdef __cplusplus
}
#endif

#endif //_NANOHAL_CONFIG_MANAGER_H_
