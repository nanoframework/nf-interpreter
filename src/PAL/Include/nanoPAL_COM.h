//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOPAL_COM_H_
#define _NANOPAL_COM_H_ 1

#include <nanoWeak.h>
#include <nanoHAL_v2.h>

#define COM_MAX_RETRIES  99;
#define NATIVE_PROFILE_PAL_COM()

#ifdef __cplusplus
extern "C" {
#endif

__nfweak bool DebuggerPort_Initialize( COM_HANDLE comPortNum );
__nfweak bool DebuggerPort_Uninitialize( COM_HANDLE comPortNum );
__nfweak int DebuggerPort_Write( COM_HANDLE comPortNum, const char* data, size_t size);
__nfweak int DebuggerPort_Read( COM_HANDLE comPortNum, char* data, size_t size );
__nfweak bool DebuggerPort_Flush( COM_HANDLE comPortNum );
__nfweak bool DebuggerPort_IsSslSupported( COM_HANDLE comPortNum );
__nfweak bool DebuggerPort_UpgradeToSsl( COM_HANDLE comPortNum, unsigned int flags );
__nfweak bool DebuggerPort_IsUsingSsl( COM_HANDLE comPortNum );

#ifdef __cplusplus
}
#endif

#endif // _NANOPAL_COMDECL_H_
