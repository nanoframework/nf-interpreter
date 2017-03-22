//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"
#include <crtdbg.h>
#include <nanoHAL.h>
#include <nanoPAL.h>

#define NATIVE_PROFILE_PAL_COM()

//////////////////////////////////////////////////////////////////////////////////

bool DebuggerPort_Initialize( COM_HANDLE ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return true;
}

bool DebuggerPort_Uninitialize( COM_HANDLE ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return true;
}


int DebuggerPort_Write( COM_HANDLE ComPortNum, const char* Data, size_t size, int maxRetries )
{
    NATIVE_PROFILE_PAL_COM();
    return 0;
}


int DebuggerPort_Read( COM_HANDLE ComPortNum, char* Data, size_t size )
{
    NATIVE_PROFILE_PAL_COM();
    return 0;
}

bool DebuggerPort_Flush( COM_HANDLE ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return true;
}

bool DebuggerPort_IsSslSupported( COM_HANDLE ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return FALSE;
}

bool DebuggerPort_UpgradeToSsl( COM_HANDLE ComPortNum, unsigned int flags ) 
{ 
    NATIVE_PROFILE_PAL_COM();
    return FALSE; 
}

bool DebuggerPort_IsUsingSsl( COM_HANDLE ComPortNum )
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
