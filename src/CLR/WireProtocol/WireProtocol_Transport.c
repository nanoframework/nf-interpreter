//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Wire Protocol channel dispatcher.
// Forwards WP_ReceiveBytes() and WP_TransmitMessage() to the channel in use: serial or the network.

#include <nanoWeak.h>
#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_Transport.h>

#if CONFIG_NF_WP_CHANNEL_NETWORK && !defined(I_AM_NANOBOOTER)
#define WP_NETWORK_ENABLED 1
#else
#define WP_NETWORK_ENABLED 0
#endif

// serial is used unless this is a nanoCLR with the network channel and the serial channel disabled
#if WP_NETWORK_ENABLED && !CONFIG_NF_WP_CHANNEL_SERIAL
#define WP_SERIAL_ENABLED 0
#else
#define WP_SERIAL_ENABLED 1
#endif

static volatile WP_ActiveChannel s_activeChannel = WP_ActiveChannel_None;

#if WP_NETWORK_ENABLED

static void SwitchChannel(WP_ActiveChannel channel)
{
    if (s_activeChannel != channel)
    {
        s_activeChannel = channel;

        // anything received so far belongs to the previous channel
        WP_Message_ResetReception();

        // new session, authentication starts over
        WP_Auth_SessionChanged(channel == WP_ActiveChannel_Network);
    }
}

#endif

WP_ActiveChannel WP_Transport_GetActiveChannel(void)
{
    return s_activeChannel;
}

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
#if WP_NETWORK_ENABLED

    // drives the network state machine (accept, takeover, discovery)
    // when the network is the only channel this is also where the thread waits for a client
    WP_Network_Poll(WP_SERIAL_ENABLED ? 0 : 100);

    if (WP_Network_IsConnected())
    {
        SwitchChannel(WP_ActiveChannel_Network);

        WP_Network_Receive(ptr, size);

        return;
    }

    // client is gone (or never was), back to serial
    SwitchChannel(WP_SERIAL_ENABLED ? WP_ActiveChannel_Serial : WP_ActiveChannel_None);

#else

    s_activeChannel = WP_ActiveChannel_Serial;

#endif

#if WP_SERIAL_ENABLED

    WP_Serial_ReceiveBytes(ptr, size);

#else

    // nothing to read from: the network channel is the only one and there is no client
    // (the poll above waited for a connection)
    (void)ptr;
    (void)size;

#endif
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
#if WP_NETWORK_ENABLED

    uint8_t result = false;

    WP_Transport_Lock();

    if (s_activeChannel == WP_ActiveChannel_Network)
    {
        result = WP_Network_Send((const uint8_t *)&message->m_header, sizeof(message->m_header));

        if (result && message->m_header.m_size && message->m_payload)
        {
            result = WP_Network_Send(message->m_payload, message->m_header.m_size);
        }
    }
#if WP_SERIAL_ENABLED
    else
    {
        result = WP_Serial_TransmitMessage(message);
    }
#endif

    WP_Transport_Unlock();

    return result;

#else

    return WP_Serial_TransmitMessage(message);

#endif
}

void WP_CheckAvailableIncomingData()
{
#if WP_SERIAL_ENABLED
    WP_Serial_CheckAvailableIncomingData();
#endif
}

void WP_Transport_DropClient(void)
{
#if WP_NETWORK_ENABLED
    // wait a while before accepting another client
    WP_Network_DropClient(1000);
#endif
}

// provided as weak to be replaced by actual implementation by the platform HAL, when required
__nfweak void WP_Serial_CheckAvailableIncomingData(void)
{
}
