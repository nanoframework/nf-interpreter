//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include "nanoCLR_native.h"
#include <WireProtocol_HAL_interface.h>

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    if (*size)
    {
        auto readed = 0;
        if (WireProtocolReceiveCallback)
        {
            readed = WireProtocolReceiveCallback(*ptr, *size);
        }
        *ptr += readed;
        *size -= readed;
    }
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    auto data = CLR_RT_Buffer(sizeof(message->m_header) + message->m_header.m_size);
    std::memcpy(data.data(), &message->m_header, sizeof(message->m_header));
    if (message->m_header.m_size)
    {
        std::memcpy(&data[sizeof(message->m_header)], message->m_payload, message->m_header.m_size);
    }

    if (WireProtocolTransmitCallback)
    {
        WireProtocolTransmitCallback(&data.front(), data.size());
    }

    return data.size();
}

void WP_CheckAvailableIncomingData()
{
}
