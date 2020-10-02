//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_DEVICES_ONEWIRE_NATIVE_H_
#define _NF_DEVICES_ONEWIRE_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoHAL.h>

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

#endif  //_NF_DEVICES_ONEWIRE_NATIVE_H_
