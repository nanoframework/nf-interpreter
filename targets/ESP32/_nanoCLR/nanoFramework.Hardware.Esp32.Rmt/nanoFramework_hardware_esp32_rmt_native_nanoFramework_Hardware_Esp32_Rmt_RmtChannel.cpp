//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"

// reduce line lengths
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel RmtChannel;

// Map for registered channels
std::map<rmt_channel_handle_t, RmtChannelInfo> RmtChannel::registeredRmtChannels;

// Uncomment to dump RMT channel info on ADD, ERASE channel
// #define DUMP_RMT_CHANNELS

#ifdef DUMP_RMT_CHANNELS
void DumpRegisteredRmtChannels(const char *text)
{
    ets_printf("---- Registered RMT Channels Dump (%s) ----\n", text);

    if (RmtChannel::registeredRmtChannels.empty())
    {
        ets_printf("No registered RMT channels.\n");
        return;
    }

    for (auto &entry : RmtChannel::registeredRmtChannels)
    {
        rmt_channel_handle_t handle = entry.first;
        const RmtChannelInfo &info = entry.second;

        ets_printf("Channel Handle: %p\n", handle);

        switch (info.type)
        {
            case RmtChannelType::Transmit:
                ets_printf("  Type: Transmit\n");
                ets_printf("  TX Encoder: %p\n", info.txEncoder);
                ets_printf("  TX Buffer Size: %d bytes\n", (int)info.txBuffer.size());
                break;

            case RmtChannelType::Receive:
                ets_printf("  Type: Receive\n");
                ets_printf("  RX Queue: %p\n", info.rxQueue);
                ets_printf("  RX Buffer: %p\n", info.rxBuffer);
                ets_printf("  RX Buffer Size: %d bytes\n", info.rxBufferSize);
                break;

            default:
                ets_printf("  Type: Unknown\n");
                break;
        }

        ets_printf("--------------------------------------\n");
    }
}
#endif

static void DestroyChannel(rmt_channel_handle_t handle, RmtChannelInfo &info)
{
    // ignore any errors, as we are cleaning up and the channel may already be disabled or deleted
    rmt_disable(handle);
    rmt_del_channel(handle);

    if (info.type == RmtChannelType::Transmit)
    {
        if (info.txEncoder)
            rmt_del_encoder(info.txEncoder);

        info.txBuffer.clear();
    }
    else if (info.type == RmtChannelType::Receive)
    {
        if (info.rxQueue)
            vQueueDelete(info.rxQueue);

        if (info.rxBuffer)
            platform_free(info.rxBuffer);
    }
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::UninitializeAll()
{
    RmtDeleteSyncManager();

    for (auto &entry : registeredRmtChannels)
    {
        rmt_channel_handle_t handle = entry.first;
        RmtChannelInfo &info = entry.second;

        DestroyChannel(handle, info);
    }

    registeredRmtChannels.clear();
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::
    AddRegisteredTxChannel(rmt_channel_handle_t handle, int bufferSize, rmt_encoder_handle_t encoder)
{
    RmtChannelInfo info;
    info.type = RmtChannelType::Transmit;
    info.txEncoder = encoder;
    info.txBuffer.resize(bufferSize);

    registeredRmtChannels.emplace(handle, std::move(info));

#ifdef DUMP_RMT_CHANNELS
    DumpRegisteredRmtChannels("add Tx");
#endif
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::
    AddRegisteredRxChannel(rmt_channel_handle_t handle, QueueHandle_t queue, uint8_t *buffer, CLR_INT32 bufferSize)
{
    RmtChannelInfo info;
    info.type = RmtChannelType::Receive;
    info.rxQueue = queue;
    info.rxBuffer = buffer;
    info.rxBufferSize = bufferSize;
    info.isBlockingReceiving = false;
    info.isContinuousReceiving = false;

    registeredRmtChannels.emplace(handle, std::move(info));

#ifdef DUMP_RMT_CHANNELS
    DumpRegisteredRmtChannels("add Rx");
#endif
}

//
//  Check if channel is present in map
//  return true if present (valid)
//
bool Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::CheckChannel(
    rmt_channel_handle_t handle)
{
    if (registeredRmtChannels.find(handle) == registeredRmtChannels.end())
    {
        return false;
    }

    return true;
}

RmtChannelInfo *Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::
    FindRegisteredRmtChannel(rmt_channel_handle_t handle, RmtChannelType expectedType)
{
    auto it = registeredRmtChannels.find(handle);
    if (it == registeredRmtChannels.end())
    {
        return nullptr;
    }

    RmtChannelInfo &info = it->second;

    if (info.type != expectedType)
    {
        return nullptr;
    }

    return &info;
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::EraseChannel(
    rmt_channel_handle_t handle)
{
    auto it = registeredRmtChannels.find(handle);
    if (it == registeredRmtChannels.end())
    {
        return;
    }

    RmtChannelInfo &info = it->second;

    DestroyChannel(handle, info);

    registeredRmtChannels.erase(handle);

#ifdef DUMP_RMT_CHANNELS
    DumpRegisteredRmtChannels("Erase channel");
#endif
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::
    RmtMapEspErrToClrErr(esp_err_t err)
{
    HRESULT hr = 0;

    switch (err)
    {
        case ESP_OK:
            hr = S_OK;
            break;
        case ESP_ERR_TIMEOUT:
            hr = CLR_E_TIMEOUT;
            break;
        case ESP_ERR_INVALID_ARG:
            hr = CLR_E_INVALID_PARAMETER;
            break;
        case ESP_ERR_NOT_SUPPORTED:
            hr = CLR_E_NOT_SUPPORTED;
            break;
        case ESP_ERR_INVALID_STATE:
            hr = CLR_E_INVALID_OPERATION;
            break;
        case ESP_ERR_NO_MEM:
            hr = CLR_E_OUT_OF_MEMORY;
            break;
        case ESP_ERR_NOT_FOUND: // RMT channels are used up
            hr = CLR_E_NOT_FOUND;
            break;
        default:
            hr = CLR_E_FAIL;
            break;
    }
    return hr;
}
