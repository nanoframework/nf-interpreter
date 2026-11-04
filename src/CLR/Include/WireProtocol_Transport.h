//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIREPROTOCOL_TRANSPORT_H
#define WIREPROTOCOL_TRANSPORT_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "WireProtocol.h"

// Wire Protocol channel dispatcher.

#if defined(__cplusplus)
extern "C"
{
#endif

    // maximum length of the authentication token
#define WP_AUTH_TOKEN_MAX_LENGTH 64

    typedef enum WP_ActiveChannel
    {
        WP_ActiveChannel_None = 0,
        WP_ActiveChannel_Serial = 1,
        WP_ActiveChannel_Network = 2,
    } WP_ActiveChannel;

    typedef enum WP_AuthResult
    {
        // token accepted (or no authentication required)
        WP_AuthResult_Ok = 0,

        // wrong token
        WP_AuthResult_Failed = 1,

        // wrong token and the maximum number of attempts was reached: the connection has to be dropped
        WP_AuthResult_Dropped = 2,

    } WP_AuthResult;

    ///
    /// @brief Gets the channel that is currently used by the Wire Protocol.
    ///
    WP_ActiveChannel WP_Transport_GetActiveChannel(void);

    //////////////////////////////////////////
    // platform serial channel (implemented by the platform HAL)

    void WP_Serial_ReceiveBytes(uint8_t **ptr, uint32_t *size);
    uint8_t WP_Serial_TransmitMessage(WP_Message *message);
    void WP_Serial_CheckAvailableIncomingData(void);

    //////////////////////////////////////////
    // transmission lock (has to be implemented by the platform HAL when the network channel is enabled)

    ///
    /// @brief Takes the lock that serializes the transmission of messages. Messages are sent from the CLR thread and
    /// from the receiver thread.
    ///
    void WP_Transport_Lock(void);

    ///
    /// @brief Releases the transmission lock.
    ///
    void WP_Transport_Unlock(void);

    //////////////////////////////////////////
    // authentication (implemented in WireProtocol_Auth.c, used by the network channel and by the debugger)

    ///
    /// @brief Sets the token required to authenticate a network session. An empty token (or NULL) disables the
    /// authentication.
    ///
    void WP_Auth_SetToken(const char *token);

    ///
    /// @brief To be called when a session starts or ends. Resets the authentication state.
    ///
    /// @param isNetwork true if the new session is over the network channel.
    ///
    void WP_Auth_SessionChanged(bool isNetwork);

    ///
    /// @brief Checks if there is a token set (the device requires authentication on network sessions).
    ///
    bool WP_Auth_HasToken(void);

    ///
    /// @brief Checks if the current session requires authentication (network session and a token is set).
    ///
    bool WP_Auth_IsRequired(void);

    ///
    /// @brief Checks if the current session is allowed to execute commands.
    ///
    bool WP_Auth_IsAuthenticated(void);

    ///
    /// @brief Verifies the token provided by the debugger.
    ///
    WP_AuthResult WP_Auth_Verify(const uint8_t *token, uint32_t length);

    ///
    /// @brief Requests the channel to drop the current network client, if any.
    ///
    void WP_Transport_DropClient(void);

#if CONFIG_NF_WP_CHANNEL_NETWORK && !defined(I_AM_NANOBOOTER)

    //////////////////////////////////////////
    // network channel (WireProtocol_Network.cpp)

    ///
    /// @brief Starts the network channel: creates the listener (when the network is up) and the discovery announcer.
    /// Safe to call more than once. If the network isn't ready, the listener is created later, from WP_Network_Poll().
    ///
    bool WP_Network_Initialize(void);

    ///
    /// @brief Stops the network channel announcing that the device is gone.
    ///
    void WP_Network_Uninitialize(void);

    ///
    /// @brief Closes the listener and the client. The listener is created again from WP_Network_Poll().
    ///
    void WP_Network_Stop(void);

    ///
    /// @brief Runs the network channel state machine (accept, takeover, discovery). Called from the receiver thread.
    ///
    /// @param waitMs time to wait for a new connection when there is no client connected.
    ///
    void WP_Network_Poll(uint32_t waitMs);

    ///
    /// @brief Receives bytes from the connected client. Waits for a short time if nothing is available.
    ///
    void WP_Network_Receive(uint8_t **ptr, uint32_t *size);

    ///
    /// @brief Sends bytes to the connected client.
    ///
    bool WP_Network_Send(const uint8_t *data, uint32_t size);

    ///
    /// @brief true if there is a client connected.
    ///
    bool WP_Network_IsConnected(void);

    ///
    /// @brief Closes the client connection and doesn't accept another for holdOffMs.
    ///
    void WP_Network_DropClient(uint32_t holdOffMs);

#endif

#if defined(__cplusplus)
}
#endif

#endif // WIREPROTOCOL_TRANSPORT_H
