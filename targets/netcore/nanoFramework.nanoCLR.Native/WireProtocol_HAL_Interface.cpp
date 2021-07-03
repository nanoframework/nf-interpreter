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
    if (gWireTransmitCallback != NULL)
    {
        auto data = std::vector<byte>(sizeof(message->m_header) + message->m_header.m_size);
        std::memcpy(data.data(), (const uint8_t *)&message->m_header, sizeof(message->m_header));
        if (message->m_header.m_size)
        {
            std::memcpy(&data[sizeof(message->m_header)], (const uint8_t *)message->m_payload, message->m_header.m_size);
        }
        gWireTransmitCallback(data.data(), data.size());
    }
    return true;
}

void WP_CheckAvailableIncomingData()
{
}
