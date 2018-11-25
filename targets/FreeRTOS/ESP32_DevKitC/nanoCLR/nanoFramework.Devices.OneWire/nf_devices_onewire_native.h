//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_DEVICES_ONEWIRE_NATIVE_H_
#define _NF_DEVICES_ONEWIRE_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
//#include <hal.h>

struct Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___serialNumber = 2;

    NANOCLR_NATIVE_DECLARE(TouchReset___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(TouchBit___BOOLEAN__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(TouchByte___U1__U1);
    NANOCLR_NATIVE_DECLARE(WriteByte___U1__U1);
    NANOCLR_NATIVE_DECLARE(ReadByte___U1);
    NANOCLR_NATIVE_DECLARE(FindFirstDevice___BOOLEAN__BOOLEAN__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(FindNextDevice___BOOLEAN__BOOLEAN__BOOLEAN);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Devices_OneWire;

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

#endif  //_NF_DEVICES_ONEWIRE_NATIVE_H_
