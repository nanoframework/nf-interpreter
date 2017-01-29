//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <TinyCLR_Hardware.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_HW_Hardware::CreateInstance()
{
    NATIVE_PROFILE_CLR_HARDWARE();
    TINYCLR_HEADER();
    TINYCLR_NOCLEANUP_NOLABEL();
}

HRESULT CLR_HW_Hardware::Hardware_Initialize()
{
    NATIVE_PROFILE_CLR_HARDWARE();
    TINYCLR_HEADER();
    TINYCLR_NOCLEANUP_NOLABEL();
}

//--//

HRESULT CLR_HW_Hardware::DeleteInstance()
{
    NATIVE_PROFILE_CLR_HARDWARE();
    TINYCLR_HEADER();
    TINYCLR_NOCLEANUP_NOLABEL();
}

void CLR_HW_Hardware::Hardware_Cleanup()
{
    NATIVE_PROFILE_CLR_HARDWARE();
}

//--//

void CLR_HW_Hardware::PrepareForGC()
{
    NATIVE_PROFILE_CLR_HARDWARE();
}

void CLR_HW_Hardware::ProcessActivity()
{
    NATIVE_PROFILE_CLR_HARDWARE();
}

//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//
