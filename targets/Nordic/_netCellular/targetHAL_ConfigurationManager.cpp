//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <Target_BlockStorage_ZephyrFlashDriver.h>

extern uint32_t ip4_addr;
extern uint32_t ip4_dns1_addr;
extern uint32_t ip4_dns2_addr;

uint32_t GetExistingConfigSize()
{
    uint32_t currentConfigSize = 0;

    currentConfigSize =
        g_TargetConfiguration.NetworkInterfaceConfigs->Count * sizeof(HAL_Configuration_NetworkInterface);
    currentConfigSize += g_TargetConfiguration.Wireless80211Configs->Count * sizeof(HAL_Configuration_Wireless80211);

    return currentConfigSize;
}

// Need these alignment attributes given the 'odd' size of the strutures.
__attribute__((aligned(32))) HAL_CONFIGURATION_NETWORK spfNetwork;
__attribute__((aligned(32))) HAL_CONFIGURATION_NETWORK_WIRELESS80211 spfWireless;
__attribute__((aligned(32))) HAL_CONFIGURATION_NETWORK_WIRELESSAP spfWirelessAP;
__attribute__((aligned(32))) HAL_CONFIGURATION_X509_CERTIFICATE spfCertificateStore;
__attribute__((aligned(32))) HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE spfDeviceCertificates;

// #define STATOC_ALLOC 1
#ifdef STATOC_ALLOC
HAL_Configuration_NetworkInterface nrf9160_lte;
#else
HAL_Configuration_NetworkInterface *nrf9160_lte;
#endif

// initialization of configuration manager
// provided as weak so it can be replaced at target level, if required because of the target implementing the storage
// with a mechanism other then saving to flash
__nfweak void ConfigurationManager_Initialize()
{
    // init g_TargetConfiguration
    memset(&g_TargetConfiguration, 0, sizeof(HAL_TARGET_CONFIGURATION));

    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
};

void dumpNetInfo(HAL_Configuration_NetworkInterface *net1, HAL_Configuration_NetworkInterface *net2)
{
#if 1
    if (net2 != 0)
    {
        printk("AutomaticDNS         %d  %d\n", net1->AutomaticDNS, net2->AutomaticDNS);
        printk("InterfaceType        %d  %d\n", net1->InterfaceType, net2->InterfaceType);
        printk("IPv4Address          %x  %x\n", net1->IPv4Address, net2->IPv4Address);
        printk("IPv4DNSAddress1      %x  %x\n", net1->IPv4DNSAddress1, net2->IPv4DNSAddress1);
        printk("IPv4GatewayAddress   %x  %x\n", net1->IPv4GatewayAddress, net2->IPv4GatewayAddress);
    }
    else
    {
        printk("AutomaticDNS         %d\n", net1->AutomaticDNS);
        printk("InterfaceType        %d\n", net1->InterfaceType);
        printk("IPv4Address          %x\n", net1->IPv4Address);
        printk("IPv4DNSAddress1      %x\n", net1->IPv4DNSAddress1);
        printk("IPv4GatewayAddress   %x\n", net1->IPv4GatewayAddress);
    }
#endif
}

// Enumerates the configuration blocks from the configuration flash sector
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
__nfweak void ConfigurationManager_EnumerateConfigurationBlocks()
{
    printk("ConfigurationManager_EnumerateConfigurationBlocks++\n");
    // Spoof up a single network config and no certs.
    if (g_TargetConfiguration.NetworkInterfaceConfigs == 0)
    {
#if 1

#ifdef STATIC_ALLOC
        memcpy(&nrf9160_lte.Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(nrf9160_lte.Marker));
        nrf9160_lte.InterfaceType = NetworkInterfaceType_Ethernet;
        nrf9160_lte.StartupAddressMode = AddressMode_DHCP;
        nrf9160_lte.AutomaticDNS = 1;
        nrf9160_lte.SpecificConfigId = UINT32_MAX;
        nrf9160_lte.IPv4Address = 0;
        nrf9160_lte.IPv4DNSAddress1 = 0;
        nrf9160_lte.IPv4DNSAddress2 = 0;
        // printk ("0addr of blk %x\n", (uint32_t)&nrf9160_lte);
#else
        nrf9160_lte = (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));
        memset(nrf9160_lte, 0, sizeof(HAL_Configuration_NetworkInterface));

        memcpy(&nrf9160_lte->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(nrf9160_lte->Marker));
        nrf9160_lte->InterfaceType = NetworkInterfaceType_Ethernet;
        nrf9160_lte->StartupAddressMode = AddressMode_DHCP;
        nrf9160_lte->AutomaticDNS = 1;
        nrf9160_lte->SpecificConfigId = UINT32_MAX;
        nrf9160_lte->IPv4Address = 0;
        nrf9160_lte->IPv4DNSAddress1 = 0;
        nrf9160_lte->IPv4DNSAddress2 = 0;
        // printk ("0addr of blk %x\n", (uint32_t)nrf9160_lte);
#endif

#else
        spfNetwork.Configs[0] =
            (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));
        memset(spfNetwork.Configs, 0, sizeof(HAL_Configuration_NetworkInterface));
        memcpy(
            &spfNetwork.Configs[0]->Marker,
            c_MARKER_CONFIGURATION_NETWORK_V1,
            sizeof(spfNetwork.Configs[0]->Marker));
        // TODO:: Fix the network type
        spfNetwork.Configs[0]->InterfaceType = NetworkInterfaceType_Ethernet;
        spfNetwork.Configs[0]->StartupAddressMode = AddressMode_DHCP;
#endif
        spfNetwork.Count = 1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

#ifdef STATIC_ALLOC
        spfNetwork.Configs[0] = &nrf9160_lte;
#else
        spfNetwork.Configs[0] = nrf9160_lte;
#endif
#pragma GCC diagnostic pop
        g_TargetConfiguration.NetworkInterfaceConfigs = &spfNetwork;

        spfWireless.Count = 0;
        g_TargetConfiguration.Wireless80211Configs = &spfWireless;

        spfWirelessAP.Count = 0;
        g_TargetConfiguration.WirelessAPConfigs = &spfWirelessAP;

        spfCertificateStore.Count = 0;
        g_TargetConfiguration.CertificateStore = &spfCertificateStore;

        spfDeviceCertificates.Count = 0;
        g_TargetConfiguration.DeviceCertificates = &spfDeviceCertificates;
    }
    dumpNetInfo(spfNetwork.Configs[0], 0);

    // printk ("ConfigurationManager_EnumerateConfigurationBlocks-- blk %x\n", (uint32_t)spfNetwork.Configs[0]);
}

// Gets the network configuration block from the configuration flash sector
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
static int cnt = 0;
__nfweak bool ConfigurationManager_GetConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    int sizeOfBlock = 0;
    uint8_t *blockAddress = NULL;

    printk(
        "ConfigurationManager_GetConfigurationBlock++  cfg %d idx:%d cnt:%d\n",
        configuration,
        configurationIndex,
        cnt);

    // printk ("ConfigurationManager_GetConfigurationBlock   count %d idx %d %x\n",
    //         g_TargetConfiguration.NetworkInterfaceConfigs->Count, configurationIndex,
    //         (uint32_t)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[0]);

    // validate if the requested block exists
    // Count has to be non zero
    // requested Index has to exist (array index starts at zero, so need to add one)
    if (configuration == DeviceConfigurationOption_Network)
    {
        if (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
        {
            // the requested config block is beyond the available count
            return false;
        }
        // See if it's the 9160 modem.
        if (configurationIndex == 0)
        {
            // if (cnt++ > 2)
            {
                // Update the ip info.
#ifdef STATIC_ALLOC
                nrf9160_lte.IPv4Address = ip4_addr;
                nrf9160_lte.IPv4DNSAddress1 = ip4_dns1_addr;
                nrf9160_lte.IPv4DNSAddress2 = ip4_dns2_addr;
#else
                nrf9160_lte->IPv4Address = ip4_addr;
                nrf9160_lte->IPv4NetMask = 0xffffffff;
                nrf9160_lte->IPv4DNSAddress1 = ip4_dns1_addr;
                nrf9160_lte->IPv4DNSAddress2 = ip4_dns2_addr;
                // The LTE network doesn't supply a gateway address.
                nrf9160_lte->IPv4GatewayAddress = ip4_addr;
#endif
            }

            // set block size
            sizeOfBlock = sizeof(HAL_Configuration_NetworkInterface);

            // get block address
            blockAddress = (uint8_t *)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex];

            dumpNetInfo(g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex], 0);
        }
    }
    else if (configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        if (g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count)
        {
            return FALSE;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_Wireless80211);

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if (g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
            return FALSE;
        }

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        sizeOfBlock += ((HAL_Configuration_X509CaRootBundle *)blockAddress)->CertificateSize;
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if (g_TargetConfiguration.DeviceCertificates->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.DeviceCertificates->Count)
        {
            return FALSE;
        }

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex];

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
        sizeOfBlock += ((HAL_Configuration_X509DeviceCertificate *)blockAddress)->CertificateSize;
    }
    else
    {
        printk("Unknown configuration %d requested.\n", configuration);
        return false;
    }
    // copy the config block content to the pointer in the argument
    memcpy(configurationBlock, blockAddress, sizeOfBlock);

    // printk ("ConfigurationManager_GetConfigurationBlock--  %x\n", (uint32_t)blockAddress);
    return TRUE;
}

void DumpBuf (uint8_t *buf, int size)
{
    int i;
    int cnt = 0;
    char sz[64];
    char sz1[32];
    while (cnt < size)
    {
        sz[0]='\0';
        sz1[0]='\0';
        for (i = 0; (i < 16) && (cnt < size); i++)
        {
            sprintf (&sz[hal_strlen_s(sz)], "%02x ", buf[cnt]);
            if ((buf[cnt] >=' ') && (buf[cnt] < 0x7e))
            {
                sz1[i] = buf[cnt];
            }
            else
            {
                sz1[i] = '.';
            }
            cnt++;
        }
        printk ("%s    %s\n", sz, sz1);
    }
}

// Stores the configuration block to the configuration flash sector
// NOTE: because inserting or removing a configuration block it's very 'RAM expensive' we choose not to support
// those operations the host debugger will have to be used to manage these operations on the device configuration
// collection it's implemented with 'weak' attribute so it can be replaced at target level if a different
// persistance mechanism is used
__nfweak bool ConfigurationManager_StoreConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t blockSize,
    uint32_t offset,
    bool done)
{
    // ByteAddress storageAddress = 0;
    // bool requiresEnumeration = FALSE;
    bool success = FALSE;
    printk("ConfigurationManager_StoreConfigurationBlock++  idx: %d ptr %p blksiz: %d off %d\n", configurationIndex, configurationBlock, blockSize, offset);

    if (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 ||
        (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
    {
        printk("ConfigurationManager_StoreConfigurationBlock-- fail 1\n");
        // the requested config block is beyond the available count
        return false;
    }
    // We only support the 9160 modem for now
    if (configurationIndex != 0)
    {
        printk("ConfigurationManager_StoreConfigurationBlock-- fail 2\n");
        // the requested config block is beyond the available count
        return false;
    }

    printk ("################################ >>>\n");
    HAL_CONFIGURATION_NETWORK *pnet;
    HAL_CONFIGURATION_NETWORK_WIRELESS80211 *p802;
    // HAL_CONFIGURATION_NETWORK_WIRELESSAP *pAP;
    HAL_CONFIGURATION_X509_CERTIFICATE *pCACert;
    HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *pDevCert;
    uint32_t start, end;
    start = (uint32_t)configurationBlock+offset;
    end = start+blockSize;
    {
        pnet = (HAL_CONFIGURATION_NETWORK *)ConfigurationManager_FindNetworkConfigurationBlocks(start, end);
        if (pnet && (pnet->Count > 0))
            printk ("NetworkConfiguration at %p  cnt %d\n", pnet, pnet->Count);
        platform_free (pnet);

        p802 = (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(start, end);
        if (p802 && (p802->Count > 0))
            printk ("NetworkWireless80211Configuration at %p cnt %d\n", p802, p802->Count);
        platform_free (p802);

        // pAP = (HAL_CONFIGURATION_NETWORK_WIRELESSAP *)ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(start, end);
        // if (pAP && (pAP->Count > 0))
        //     printk ("NetworkWirelessAPConfiguration at %p cnt %d\n", pAP, pAP->Count);
        // platform_free (pAP);

        pCACert = (HAL_CONFIGURATION_X509_CERTIFICATE *)ConfigurationManager_FindX509CertificateConfigurationBlocks(start, end);
        if (pCACert && (pCACert->Count > 0))
        {
            printk ("X509CertificateConfiguration at %p ", pCACert);
            printk ("cnt %d  ", pCACert->Count);
            if ((pCACert->Count > 0) && (pCACert->Certificates))
            {
                printk ("size %d\n", pCACert->Certificates[0]->CertificateSize);
                DumpBuf (pCACert->Certificates[0]->Certificate, pCACert->Certificates[0]->CertificateSize);
            }
            printk ("\n");
        }
        platform_free (pCACert);
        pDevCert = (HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)ConfigurationManager_FindX509DeviceCertificatesConfigurationBlocks(start, end);
        if (pDevCert && (pDevCert->Count > 0))
        {
            printk ("DeviceCertificatesConfiguration at %p ", pDevCert);
            printk ("cnt %d  ", pDevCert->Count);
            if ((pDevCert->Count > 0) && (pDevCert->Certificates))
            {
                printk ("size %d", pDevCert->Certificates[0]->CertificateSize);
            }
            printk ("\n");
        }
        platform_free (pDevCert);
    }
    printk ("################################ <<<\n");
    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
        {
            printk("ConfigurationManager_StoreConfigurationBlock-- 2\n");
            HAL_Configuration_NetworkInterface *newcfg = (HAL_Configuration_NetworkInterface *)configurationBlock;
            dumpNetInfo(newcfg, nrf9160_lte);
            printk("Store configuration \"DeviceConfigurationOption_Network\" not supported at this time.");


            printk("ConfigurationManager_StoreConfigurationBlock-- 3\n");
            success = TRUE;
        }
        case DeviceConfigurationOption_X509CaRootBundle:
            printk("ConfigurationManager_StoreConfigurationBlock  DeviceConfigurationOption_X509CaRootBundle\n");
            success = TRUE;
            break;
        case DeviceConfigurationOption_X509DeviceCertificates:
            printk("ConfigurationManager_StoreConfigurationBlock  DeviceConfigurationOption_X509DeviceCertificates\n");
            success = TRUE;
            break;
        case DeviceConfigurationOption_All:
            printk("ConfigurationManager_StoreConfigurationBlock  DeviceConfigurationOption_All\n");
            success = TRUE;
            break;

        default:
            printk("Unknown configuration %d\n", configuration);
            break;
    }
    printk("ConfigurationManager_StoreConfigurationBlock-- %d\n", success);
    return success;
}

// Updates a configuration block in the configuration flash sector
// The flash sector has to be erased before writing the updated block
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism
// is used
__nfweak bool ConfigurationManager_UpdateConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    bool success = FALSE;

    printk("ConfigurationManager_UpdateConfigurationBlock++\n");
    if (configuration == DeviceConfigurationOption_Network)
    {
        if (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
        {
            // the requested config block is beyond the available count
            return false;
        }
        // See if it's the 9160 modem.
        if (configurationIndex == 0)
        {
            HAL_Configuration_NetworkInterface *newcfg = (HAL_Configuration_NetworkInterface *)configurationBlock;
            dumpNetInfo(newcfg, nrf9160_lte);
        }
        success = TRUE;
    }
    return success;
}

//  Default initialisation for wireless config block
// it's implemented with 'weak' attribute so it can be replaced at target level if different configurations are
// intended
__nfweak void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *pconfig, uint32_t configurationIndex)
{
    (void)pconfig;
    (void)configurationIndex;
    printk("InitialiseWirelessDefaultConfig++\n");

    // currently empty as no Zephyr target has Wireless 802.11 interface
}

//  Default initialisation for Network interface config blocks
// it's implemented with 'weak' attribute so it can be replaced at target level if different configurations are
// intended
__nfweak bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *pconfig, uint32_t configurationIndex)
{
    (void)pconfig;
    (void)configurationIndex;
    printk("InitialiseNetworkDefaultConfig++\n");

    // can't create a "default" network config because we are lacking definition of a MAC address
    return FALSE;
}
