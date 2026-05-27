//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Hardware.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

__nfweak HRESULT CLR_HW_Hardware::CreateInstance()
{
    NATIVE_PROFILE_CLR_HARDWARE();
    NANOCLR_HEADER();
    NANOCLR_NOCLEANUP_NOLABEL();
}

__nfweak HRESULT CLR_HW_Hardware::Hardware_Initialize()
{
    NATIVE_PROFILE_CLR_HARDWARE();
    NANOCLR_HEADER();
    NANOCLR_NOCLEANUP_NOLABEL();
}

//--//

__nfweak HRESULT CLR_HW_Hardware::DeleteInstance()
{
    NATIVE_PROFILE_CLR_HARDWARE();
    NANOCLR_HEADER();
    NANOCLR_NOCLEANUP_NOLABEL();
}

__nfweak void CLR_HW_Hardware::Hardware_Cleanup()
{
    NATIVE_PROFILE_CLR_HARDWARE();
}

//--//

__nfweak void CLR_HW_Hardware::PrepareForGC()
{
    NATIVE_PROFILE_CLR_HARDWARE();
}

__nfweak void CLR_HW_Hardware::ProcessActivity()
{
    NATIVE_PROFILE_CLR_HARDWARE();
}

//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//

