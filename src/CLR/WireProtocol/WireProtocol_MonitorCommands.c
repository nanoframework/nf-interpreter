//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_MonitorCommands.h"
#include <nanoPAL_BlockStorage.h>
#include <target_board.h>

#if !defined(_WIN32)

// dummy implementation to allow build of nanoCLR
__nfweak int AccessMemory(uint32_t location, uint32_t lengthInBytes, uint8_t *buffer, int32_t mode, uint32_t *errorCode)
{
    (void)location;
    (void)lengthInBytes;
    (void)buffer;
    (void)mode;
    (void)errorCode;

    return 0;
}

//////////////////////////////////////////////////

int Monitor_Ping(WP_Message *message)
{
    if ((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_Ping_Reply cmdReply;
        cmdReply.Source = Monitor_Ping_c_Ping_Source_NanoBooter;
        cmdReply.Flags = 0;

        // fill in the flags
#if defined(WP_IMPLEMENTS_CRC32)
        cmdReply.Flags |= Monitor_Ping_c_Ping_WPFlag_SupportsCRC32;
#endif

// Wire Protocol packet size
#if (WP_PACKET_SIZE == 512)
        cmdReply.Flags |= Monitor_Ping_c_PacketSize_0512;
#elif (WP_PACKET_SIZE == 256)
        cmdReply.Flags |= Monitor_Ping_c_PacketSize_0256;
#elif (WP_PACKET_SIZE == 128)
        cmdReply.Flags |= Monitor_Ping_c_PacketSize_0128;
#elif (WP_PACKET_SIZE == 1024)
        cmdReply.Flags |= Monitor_Ping_c_PacketSize_1024;
#endif

        if (Target_HasNanoBooter())
        {
            cmdReply.Flags |= Monitor_Ping_c_HasNanoBooter;
        }

        if (Target_HasProprietaryBooter())
        {
            cmdReply.Flags |= Monitor_Ping_c_HasProprietaryBooter;
        }

        if (Target_IFUCapable())
        {
            cmdReply.Flags |= Monitor_Ping_c_IFUCapable;
        }

        if (Target_ConfigUpdateRequiresErase())
        {
            cmdReply.Flags |= Monitor_Ping_c_ConfigBlockRequiresErase;
        }

        WP_ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_OemInfo(WP_Message *message)
{
    Monitor_OemInfo_Reply cmdReply;

    bool fOK = NanoBooter_GetReleaseInfo(&cmdReply.m_releaseInfo) == true;

    WP_ReplyToCommand(message, fOK, false, &cmdReply, sizeof(cmdReply));

    return true;
}

int Monitor_ReadMemory(WP_Message *message)
{
    CLR_DBG_Commands_Monitor_ReadMemory *cmd = (CLR_DBG_Commands_Monitor_ReadMemory *)message->m_payload;

    unsigned char buf[1024];
    unsigned int len = cmd->length;
    if (len > sizeof(buf))
        len = sizeof(buf);
    uint32_t errorCode;

    AccessMemory(cmd->address, len, buf, AccessMemory_Read, &errorCode);

    WP_ReplyToCommand(message, true, false, buf, len);

    return true;
}

int Monitor_WriteMemory(WP_Message *message)
{
    CLR_DBG_Commands_Monitor_WriteMemory *cmd = (CLR_DBG_Commands_Monitor_WriteMemory *)message->m_payload;
    CLR_DBG_Commands_Monitor_WriteMemory_Reply cmdReply;

    AccessMemory(cmd->address, cmd->length, cmd->data, AccessMemory_Write, &cmdReply);

    WP_ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));

    return true;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_Reboot(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

int Monitor_EraseMemory(WP_Message *message)
{
    CLR_DBG_Commands_Monitor_EraseMemory *cmd = (CLR_DBG_Commands_Monitor_EraseMemory *)message->m_payload;
    CLR_DBG_Commands_Monitor_EraseMemory_Reply cmdReply;

    // TODO: not sure if we really need this
    // nanoBooter_OnStateChange( State_MemoryErase, (void*)cmd->m_address );

    AccessMemory(cmd->address, cmd->length, NULL, AccessMemory_Erase, &cmdReply);

    WP_ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));

    return true;
}

int Monitor_QueryConfiguration(WP_Message *message)
{
    bool success = false;

    // include handling of configuration block only if feature is available
#if (HAS_CONFIG_BLOCK == TRUE)

    Monitor_QueryConfiguration_Command *cmd = (Monitor_QueryConfiguration_Command *)message->m_payload;
    int size = 0;

    HAL_Configuration_NetworkInterface configNetworkInterface;
    HAL_Configuration_Wireless80211 configWireless80211NetworkInterface;

    switch ((DeviceConfigurationOption)cmd->Configuration)
    {
        case DeviceConfigurationOption_Network:

            if (ConfigurationManager_GetConfigurationBlock(
                    (void *)&configNetworkInterface,
                    (DeviceConfigurationOption)cmd->Configuration,
                    cmd->BlockIndex) == true)
            {
                size = sizeof(HAL_Configuration_NetworkInterface);
                success = true;

                WP_ReplyToCommand(message, success, false, (uint8_t *)&configNetworkInterface, size);
            }
            break;

        case DeviceConfigurationOption_Wireless80211Network:

            if (ConfigurationManager_GetConfigurationBlock(
                    (void *)&configWireless80211NetworkInterface,
                    (DeviceConfigurationOption)cmd->Configuration,
                    cmd->BlockIndex) == true)
            {
                size = sizeof(HAL_Configuration_Wireless80211);
                success = true;

                WP_ReplyToCommand(message, success, false, (uint8_t *)&configWireless80211NetworkInterface, size);
            }
            break;

        case DeviceConfigurationOption_WirelessNetworkAP:
            // TODO missing implementation for now
            break;

        default:
            break;
    }

    if (!success)
    {
        WP_ReplyToCommand(message, success, false, NULL, size);
    }

#else

    (void)message;

#endif // (HAS_CONFIG_BLOCK == TRUE)

    return success;
}

int Monitor_UpdateConfiguration(WP_Message *message)
{
    bool success = false;

    // include handling of configuration block only if feature is available
#if (HAS_CONFIG_BLOCK == TRUE)

    Monitor_UpdateConfiguration_Command *cmd = (Monitor_UpdateConfiguration_Command *)message->m_payload;
    Monitor_UpdateConfiguration_Reply cmdReply;

    switch ((DeviceConfigurationOption)cmd->Configuration)
    {
        case DeviceConfigurationOption_Network:
        case DeviceConfigurationOption_Wireless80211Network:
        case DeviceConfigurationOption_X509CaRootBundle:
        case DeviceConfigurationOption_X509DeviceCertificates:
        case DeviceConfigurationOption_All:
            if (ConfigurationManager_StoreConfigurationBlock(
                    cmd->Data,
                    (DeviceConfigurationOption)cmd->Configuration,
                    cmd->BlockIndex,
                    cmd->Length,
                    cmd->Offset,
                    cmd->Done) == true)
            {
                cmdReply.ErrorCode = 0;
                success = true;
            }
            else
            {
                cmdReply.ErrorCode = 100;
            }
            break;

        default:
            cmdReply.ErrorCode = 10;
    }

    WP_ReplyToCommand(message, success, false, &cmdReply, sizeof(cmdReply));

#else

    (void)message;

#endif // (HAS_CONFIG_BLOCK == TRUE)

    return success;
}

int Monitor_CheckMemory(WP_Message *message)
{
    bool ret = false;

    CLR_DBG_Commands_Monitor_CheckMemory *cmd = (CLR_DBG_Commands_Monitor_CheckMemory *)message->m_payload;
    CLR_DBG_Commands_Monitor_CheckMemory_Reply cmdReply;
    uint32_t errorCode;

    ret = AccessMemory(cmd->address, cmd->length, (uint8_t *)&cmdReply, AccessMemory_Check, &errorCode);

    WP_ReplyToCommand(message, ret, false, &cmdReply, sizeof(cmdReply));

    return ret;
}

int Monitor_MemoryMap(WP_Message *message)
{
    MemoryMap_Range map[2];

    map[0].m_address = HalSystemConfig.RAM1.Base;
    map[0].m_length = HalSystemConfig.RAM1.Size;
    map[0].m_flags = Monitor_MemoryMap_c_RAM;

    map[1].m_address = HalSystemConfig.FLASH1.Base;
    map[1].m_length = HalSystemConfig.FLASH1.Size;
    map[1].m_flags = Monitor_MemoryMap_c_FLASH;

    WP_ReplyToCommand(message, true, false, map, sizeof(map));

    return true;
}

int Monitor_FlashSectorMap(WP_Message *message)
{
    if ((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Flash_BlockRegionInfo *pData = NULL;

        unsigned int rangeCount = 0;
        unsigned int rangeIndex = 0;

        for (int cnt = 0; cnt < 2; cnt++)
        {
            BlockStorageDevice *device = BlockStorageList_GetFirstDevice();

            if (device == NULL)
            {
                WP_ReplyToCommand(message, true, false, NULL, 0);
                return false;
            }

            if (cnt == 1)
            {
                uint32_t allocationSize = rangeCount * sizeof(struct Flash_BlockRegionInfo);

                pData = (Flash_BlockRegionInfo *)platform_malloc(allocationSize);

                if (pData == NULL)
                {
                    WP_ReplyToCommand(message, true, false, NULL, 0);
                    return false;
                }

                // clear memory
                memset(pData, 0, allocationSize);
            }

            DeviceBlockInfo *deviceInfo = BlockStorageDevice_GetDeviceInfo(device);

            for (unsigned int i = 0; i < deviceInfo->NumRegions; i++)
            {
                const BlockRegionInfo *pRegion = &deviceInfo->Regions[i];

                for (unsigned int j = 0; j < pRegion->NumBlockRanges; j++)
                {

                    if (cnt == 0)
                    {
                        rangeCount++;
                    }
                    else
                    {
                        pData[rangeIndex].StartAddress =
                            BlockRegionInfo_BlockAddress(pRegion, pRegion->BlockRanges[j].StartBlock);
                        pData[rangeIndex].NumBlocks = BlockRange_GetBlockCount(pRegion->BlockRanges[j]);
                        pData[rangeIndex].BytesPerBlock = pRegion->BytesPerBlock;
                        pData[rangeIndex].Flags = pRegion->BlockRanges[j].RangeType & BlockRange_USAGE_MASK;
                        // add the media attributes to the flags
                        pData[rangeIndex].Flags |= pRegion->Attributes & BlockRegionAttributes_MASK;

                        rangeIndex++;
                    }
                }
            }
        }

        WP_ReplyToCommand(message, true, false, (void *)pData, rangeCount * sizeof(struct Flash_BlockRegionInfo));

        platform_free(pData);
    }

    return true;
}

int Monitor_TargetInfo(WP_Message *message)
{
    Monitor_TargetInfo_Reply cmdReply;

    bool fOK = nanoBooter_GetTargetInfo(&cmdReply.m_TargetInfo) == true;

    WP_ReplyToCommand(message, fOK, false, &cmdReply, sizeof(cmdReply));

    return true;
}

//////////////////////////////////////////////////////////////////////
// helper functions

int nanoBooter_GetTargetInfo(TargetInfo *targetInfo)
{
#if (TARGET_HAS_NANOBOOTER == TRUE)

    BootClipboard *data = &g_BootClipboard;

    targetInfo->BooterVersion.usMajor = data->BooterVersion.usMajor;
    targetInfo->BooterVersion.usMinor = data->BooterVersion.usMinor;
    targetInfo->BooterVersion.usBuild = data->BooterVersion.usBuild;
    targetInfo->BooterVersion.usRevision = data->BooterVersion.usRevision;

    targetInfo->ClrVersion.usMajor = data->CLRVersion.usMajor;
    targetInfo->ClrVersion.usMinor = data->CLRVersion.usMinor;
    targetInfo->ClrVersion.usBuild = data->CLRVersion.usBuild;
    targetInfo->ClrVersion.usRevision = data->CLRVersion.usRevision;

#else

    targetInfo->BooterVersion.usMajor = 0;
    targetInfo->BooterVersion.usMinor = 0;
    targetInfo->BooterVersion.usBuild = 0;
    targetInfo->BooterVersion.usRevision = 0;

    targetInfo->ClrVersion.usMajor = VERSION_MAJOR;
    targetInfo->ClrVersion.usMinor = VERSION_MINOR;
    targetInfo->ClrVersion.usBuild = VERSION_BUILD;
    targetInfo->ClrVersion.usRevision = VERSION_REVISION;

#endif

    memcpy(&targetInfo->InfoString, OEMSYSTEMINFOSTRING, ARRAYSIZE(OEMSYSTEMINFOSTRING));
    memcpy(&targetInfo->TargetName, TARGETNAMESTRING, ARRAYSIZE(TARGETNAMESTRING));
    memcpy(&targetInfo->PlatformName, PLATFORMNAMESTRING, ARRAYSIZE(PLATFORMNAMESTRING));
    memcpy(&targetInfo->PlatformInfoString, TARGETINFOSTRING, ARRAYSIZE(TARGETINFOSTRING));

    return true;
}

int NanoBooter_GetReleaseInfo(ReleaseInfo *releaseInfo)
{
    releaseInfo->version.usMajor = VERSION_MAJOR;
    releaseInfo->version.usMinor = VERSION_MINOR;
    releaseInfo->version.usBuild = VERSION_BUILD;
    releaseInfo->version.usRevision = VERSION_REVISION;

    memcpy(&releaseInfo->InfoString, OEMSYSTEMINFOSTRING, ARRAYSIZE(OEMSYSTEMINFOSTRING));
    memcpy(&releaseInfo->TargetName, TARGETNAMESTRING, ARRAYSIZE(TARGETNAMESTRING));
    memcpy(&releaseInfo->PlatformName, PLATFORMNAMESTRING, ARRAYSIZE(PLATFORMNAMESTRING));
    memcpy(&releaseInfo->PlatformInfoString, TARGETINFOSTRING, ARRAYSIZE(TARGETINFOSTRING));

    return true;
}

#endif // WIN32
