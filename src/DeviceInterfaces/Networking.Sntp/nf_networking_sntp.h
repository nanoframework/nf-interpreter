//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NF_NETWORKING_SNTP_H
#define NF_NETWORKING_SNTP_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <nanoHAL_Time.h>

extern "C"
{
#ifdef DeviceFamily_CC3220
#include <targetSimpleLinkCC32xx_Sntp.h>
#elif defined(_WIN32)
// empty on purpose
#else
#include <apps/sntp.h>
#endif
}

struct Library_nf_networking_sntp_nanoFramework_Networking_Sntp
{
    NANOCLR_NATIVE_DECLARE(Start___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(Stop___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(UpdateNow___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(get_IsStarted___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_Server1___STATIC__STRING);
    NANOCLR_NATIVE_DECLARE(set_Server1___STATIC__VOID__STRING);
    NANOCLR_NATIVE_DECLARE(get_Server2___STATIC__STRING);
    NANOCLR_NATIVE_DECLARE(set_Server2___STATIC__VOID__STRING);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Sntp;

#endif // NF_NETWORKING_SNTP_H
