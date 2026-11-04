//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Authentication of the Wire Protocol sessions over the network channel.
// The debugger has to present the token that is set in the device, otherwise only Ping and Authenticate are accepted.
// Sessions over the serial transport don't require authentication.

#include <WireProtocol_Transport.h>

#if !defined(I_AM_NANOBOOTER)

#include <string.h>

// number of wrong tokens that are accepted before the connection is dropped
#define WP_AUTH_MAX_FAILURES 3

static char s_token[WP_AUTH_TOKEN_MAX_LENGTH];
static uint32_t s_tokenLength = 0;

static volatile bool s_networkSession = false;
static volatile bool s_authenticated = false;
static uint32_t s_failures = 0;

void WP_Auth_SetToken(const char *token)
{
    uint32_t length = 0;

    if (token != NULL)
    {
        length = (uint32_t)strlen(token);

        if (length > WP_AUTH_TOKEN_MAX_LENGTH)
        {
            length = WP_AUTH_TOKEN_MAX_LENGTH;
        }

        memcpy(s_token, token, length);
    }

    s_tokenLength = length;
}

void WP_Auth_SessionChanged(bool isNetwork)
{
    s_networkSession = isNetwork;
    s_authenticated = false;
    s_failures = 0;
}

bool WP_Auth_HasToken(void)
{
    return s_tokenLength > 0;
}

bool WP_Auth_IsRequired(void)
{
    return s_networkSession && s_tokenLength > 0;
}

bool WP_Auth_IsAuthenticated(void)
{
    return !WP_Auth_IsRequired() || s_authenticated;
}

WP_AuthResult WP_Auth_Verify(const uint8_t *token, uint32_t length)
{
    // always go through the whole buffer, regardless of where the first difference is, so the time taken doesn't
    // reveal how much of the token is right
    uint32_t difference = length ^ s_tokenLength;

    for (uint32_t i = 0; i < WP_AUTH_TOKEN_MAX_LENGTH; i++)
    {
        uint8_t received = (token != NULL && i < length) ? token[i] : 0;
        uint8_t expected = (i < s_tokenLength) ? (uint8_t)s_token[i] : 0;

        difference |= (uint32_t)(received ^ expected);
    }

    if (length > WP_AUTH_TOKEN_MAX_LENGTH)
    {
        difference |= 1;
    }

    if (difference == 0)
    {
        s_authenticated = true;
        s_failures = 0;

        return WP_AuthResult_Ok;
    }

    s_authenticated = false;

    if (++s_failures >= WP_AUTH_MAX_FAILURES)
    {
        s_failures = 0;

        return WP_AuthResult_Dropped;
    }

    return WP_AuthResult_Failed;
}

#endif // !I_AM_NANOBOOTER
