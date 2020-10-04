//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_DEVICES_ONEWIRE_NATIVE_TARGET_H_
#define _NF_DEVICES_ONEWIRE_NATIVE_TARGET_H_

#include <nf_devices_onewire_native.h>

// struct with parameters for 1-Wire working thread
struct OneWireFindStruct
{
    bool DoReset;
    bool AlarmOnly;
    bool FindFirst;
};

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  ONEWIRE_UNINIT,                /* Not initialized.                           */
  ONEWIRE_STOP,                  /* Stopped.                                   */
  ONEWIRE_READY,                 /* Ready.                                     */
  ONEWIRE_ACTIVE                 /* Generating random number.                  */
} oneWireState;

//character to send on the UART to mimic 1-Wire bus signals
#define IWIRE_WR0           0x00    // 1-Wire write 0 time slot
#define IWIRE_WR1           0xFF    // 1-Wire write 1 time slot
#define IWIRE_RD            0xFF    // 1-Wire read time slot

// ROM commands
#define SEARCH_ROM               0xF0    /* search ROM */
#define COND_SEARCH_ROM          0xEC    /* conditional search ROM */

#endif  //_NF_DEVICES_ONEWIRE_NATIVE_TARGET_H_
