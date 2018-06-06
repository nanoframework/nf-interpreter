//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_COM.h>

__nfweak bool GenericPort_Initialize(int comPortNum)
{
    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak bool GenericPort_Uninitialize(int comPortNum)
{
    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak uint32_t GenericPort_Write(int comPortNum, const char* data, size_t size)
{
    NATIVE_PROFILE_PAL_COM();
    return 0;
}

__nfweak int GenericPort_Read(int comPortNum, char* data, size_t size)
{
    NATIVE_PROFILE_PAL_COM();
    return 0;
}

__nfweak bool GenericPort_Flush(int comPortNum)
{
    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak bool GenericPort_IsSslSupported(int comPortNum)
{
    NATIVE_PROFILE_PAL_COM();
    return false;
}

__nfweak bool GenericPort_UpgradeToSsl(int comPortNum, unsigned int flags) 
{ 
    NATIVE_PROFILE_PAL_COM();
    return false; 
}

__nfweak bool GenericPort_IsUsingSsl(int comPortNum)
{
    NATIVE_PROFILE_PAL_COM();
    return false;
}
