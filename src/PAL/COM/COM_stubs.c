//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_COM.h>

__nfweak bool DebuggerPort_Initialize(COM_HANDLE comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak bool DebuggerPort_Uninitialize(COM_HANDLE comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak int DebuggerPort_Write(COM_HANDLE comPortNum, const char* data, size_t size, int maxRetries)
{
    (void)maxRetries;

    NATIVE_PROFILE_PAL_COM();

    // minimum implementation is calling generic port to be able to use SWO (if available)
    GenericPort_Write( comPortNum, data, size );

    return 0;
}

__nfweak int DebuggerPort_Read(COM_HANDLE comPortNum, char* data, size_t size)
{
    (void)comPortNum;
    (void)data;
    (void)size;

    NATIVE_PROFILE_PAL_COM();
    return 0;
}

__nfweak bool DebuggerPort_Flush(COM_HANDLE comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak bool DebuggerPort_IsSslSupported(COM_HANDLE comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return false;
}

__nfweak bool DebuggerPort_UpgradeToSsl(COM_HANDLE comPortNum, unsigned int flags) 
{
    (void)comPortNum;
    (void)flags;

    NATIVE_PROFILE_PAL_COM();
    return false; 
}

__nfweak bool DebuggerPort_IsUsingSsl(COM_HANDLE comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return false;
}
