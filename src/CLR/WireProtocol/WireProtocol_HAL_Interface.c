//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

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
__attribute__((weak)) bool WP_ReceiveBytes(uint8_t* ptr, uint16_t* size)
{
    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by HAL interface
__attribute__((weak)) bool WP_TransmitMessage(WP_Message* message)
{
    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by HAL interface
__attribute__((weak)) void WP_CheckAvailableIncomingData()
{
}

