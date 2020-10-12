//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"
#include <crtdbg.h>
#include <nanoHAL.h>
#include <nanoPAL.h>

#define NATIVE_PROFILE_PAL_COM()

//////////////////////////////////////////////////////////////////////////////////

__nfweak bool DebuggerPort_Initialize( COM_HANDLE ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak bool DebuggerPort_Uninitialize( COM_HANDLE ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak int DebuggerPort_Write( COM_HANDLE ComPortNum, const char* Data, size_t size )
{
    NATIVE_PROFILE_PAL_COM();
    return DebuggerPort_Write( ComPortNum, Data, size, COM_MAX_RETRIES );
}

__nfweak int DebuggerPort_Write( COM_HANDLE ComPortNum, const char* Data, size_t size, int maxRetries )
{   
    NATIVE_PROFILE_PAL_COM();
    return 0;
}


__nfweak int DebuggerPort_Read( COM_HANDLE ComPortNum, char* Data, size_t size )
{
    NATIVE_PROFILE_PAL_COM();
    return 0;
}

__nfweak bool DebuggerPort_Flush( COM_HANDLE ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak bool DebuggerPort_IsSslSupported( COM_HANDLE ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return FALSE;
}

__nfweak bool DebuggerPort_UpgradeToSsl( COM_HANDLE ComPortNum, unsigned int flags ) 
{ 
    NATIVE_PROFILE_PAL_COM();
    return FALSE; 
}

__nfweak bool DebuggerPort_IsUsingSsl( COM_HANDLE ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////

void CPU_InitializeCommunication()
{
    NATIVE_PROFILE_PAL_COM();
}

void CPU_UninitializeCommunication()
{
    NATIVE_PROFILE_PAL_COM();
}


void CPU_ProtectCommunicationGPIOs( bool On )
{
    NATIVE_PROFILE_PAL_COM();
}
