//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_HAL_Interface.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// The functions bellow are the ones that need to be ported to new channels/HALs when required
//
// Please check the reference implementation provided for ChibiOS in:
// RTOS\ChibiOS\WireProtocol_HAL_Interface.c
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// provided as weak to be replaced by actual implementation by HAL interface
__nfweak bool WP_ReceiveBytes(uint8_t* ptr, unsigned short* size)
{
    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by HAL interface
__nfweak bool WP_TransmitMessage(WP_Message* message)
{
    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by HAL interface
__nfweak void WP_CheckAvailableIncomingData()
{
}

