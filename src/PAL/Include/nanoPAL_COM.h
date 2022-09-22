//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NANOPAL_COM_H
#define NANOPAL_COM_H

#include <nanoWeak.h>
#include <nanoHAL_v2.h>

#define COM_MAX_RETRIES 99
#define NATIVE_PROFILE_PAL_COM()

#ifdef __cplusplus
extern "C"
{
#endif

    bool GenericPort_Initialize(COM_HANDLE comPortNum);
    bool GenericPort_Uninitialize(COM_HANDLE comPortNum);
    uint32_t GenericPort_Write(int portNum, const char *data, size_t size);
    int GenericPort_Read(COM_HANDLE comPortNum, char *data, size_t size);
    bool GenericPort_Flush(COM_HANDLE comPortNum);
    bool GenericPort_IsSslSupported(COM_HANDLE comPortNum);
    bool GenericPort_UpgradeToSsl(COM_HANDLE comPortNum, unsigned int flags);
    bool GenericPort_IsUsingSsl(COM_HANDLE comPortNum);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    void StdioPort_Init(void);
    uint32_t StdioPort_Write(const char *data, size_t size);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
int DebuggerPort_Write(COM_HANDLE comPortNum, const char *data, size_t size);
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    bool DebuggerPort_Initialize(COM_HANDLE comPortNum);
    bool DebuggerPort_Uninitialize(COM_HANDLE comPortNum);
    int DebuggerPort_Write(COM_HANDLE comPortNum, const char *data, size_t size, int maxRetries);
    int DebuggerPort_Read(COM_HANDLE comPortNum, char *data, size_t size);
    bool DebuggerPort_Flush(COM_HANDLE comPortNum);
    bool DebuggerPort_IsSslSupported(COM_HANDLE comPortNum);
    bool DebuggerPort_UpgradeToSsl(COM_HANDLE comPortNum, unsigned int flags);
    bool DebuggerPort_IsUsingSsl(COM_HANDLE comPortNum);

#ifdef __cplusplus
}
#endif

#endif // NANOPAL_COM_H
