//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Types.h>

//////////////////////////////////////////////////////////////////////////
// this is required to include the NanoPrintf implementation
// only once in the build, so using it here as this is part of all builds
#define NANOPRINTF_IMPLEMENTATION 1
#include <nanoprintf.h>
//////////////////////////////////////////////////////////////////////////

bool CLR_SafeSprintfV(char *&szBuffer, size_t &iBuffer, const char *format, va_list arg)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();

    int chars = vsnprintf(szBuffer, iBuffer, format, arg);
    bool fRes = (chars >= 0);

    if (fRes == false)
        chars = (int)iBuffer;

    szBuffer += chars;
    szBuffer[0] = 0;
    iBuffer -= chars;

    return fRes;
}

bool CLR_SafeSprintf(char *&szBuffer, size_t &iBuffer, const char *format, ...)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    va_list arg;
    bool fRes;

    va_start(arg, format);

    fRes = CLR_SafeSprintfV(szBuffer, iBuffer, format, arg);

    va_end(arg);

    return fRes;
}
