
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _ESP32_RMT_NATIVE_H_
#define _ESP32_RMT_NATIVE_H_

extern void RmtDeleteSyncManager();

enum class RmtChannelType : uint8_t
{
    Unknown = 0,
    Transmit,
    Receive
};

struct RmtChannelInfo
{
    RmtChannelType type = RmtChannelType::Unknown;

    bool isBlockingReceiving = false;
    bool isContinuousReceiving = false;

    // TX fields
    rmt_encoder_handle_t txEncoder = nullptr;
    std::vector<uint8_t> txBuffer;

    // RX fields
    QueueHandle_t rxQueue = nullptr;
    uint8_t *rxBuffer = nullptr;
    CLR_INT32 rxBufferSize = 0;
};

#endif //_ESP32_RMT_NATIVE_H_
