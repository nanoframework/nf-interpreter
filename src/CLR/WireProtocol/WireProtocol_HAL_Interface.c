//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_HAL_Interface.h"
#include "WireProtocol_Transport.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// The functions below are the ones that need to be ported to new channels/HALs when required
//
// Please check the reference implementation provided for ChibiOS in:
// RTOS\ChibiOS\WireProtocol_HAL_Interface.c
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// provided as weak to be replaced by actual implementation by HAL interface
__nfweak void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    (void)(ptr);
    (void)(size);
}

// provided as weak to be replaced by actual implementation by HAL interface
__nfweak uint8_t WP_TransmitMessage(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by HAL interface
__nfweak void WP_CheckAvailableIncomingData()
{
}

// provided as weak to be replaced by actual implementation by the platform when it has a channel that can be dropped
// (the targets with the Wire Protocol channel dispatcher have this one defined there)
__nfweak void WP_Transport_DropClient(void)
{
}
