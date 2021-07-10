//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include "nanoCLR_native.h"
#include <WireProtocol_HAL_interface.h>

uint8_t WP_ReceiveBytes(uint8_t *ptr, uint32_t *size)
{
    if (*size)
    {
        auto toRead = *size > gWireReceiveBuffer.size() ? gWireReceiveBuffer.size() : *size;
        if (toRead)
        {
            std::memcpy(ptr, gWireReceiveBuffer.data(), toRead);
            *size -= toRead;

            gWireReceiveBuffer.erase(gWireReceiveBuffer.begin(), gWireReceiveBuffer.begin() + toRead);

            return true;
        }
    }

    return false;
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    auto data = CLR_RT_Buffer(sizeof(message->m_header) + message->m_header.m_size);
    std::memcpy(data.data(), &message->m_header, sizeof(message->m_header));
    if (message->m_header.m_size)
    {
        std::memcpy(
            &data[sizeof(message->m_header)],
            message->m_payload,
            message->m_header.m_size);
    }
    gWireTransmitBuffer.insert(gWireTransmitBuffer.end(), data.begin(), data.end());
    return true;
}

void WP_CheckAvailableIncomingData()
{
}
