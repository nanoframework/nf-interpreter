//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#if defined(FEATURE_TRACE_TO_STDIO)

#include <nanoPAL_COM.h>
#include <nanoHAL_v2.h>
#include "..\..\..\build\_deps\chibios-src-b859f10\os\hal\include\hal_channels.h"
#include "..\..\..\targets\AzureRTOS\ChibiOS\osal.h"
#include "..\..\..\build\_deps\chibios-src-b859f10\os\hal\include\hal.h"
#if (HAL_USE_SERIAL == TRUE)
#include <serialcfg.h>
#endif

__nfweak bool GenericPort_Initialize(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak bool GenericPort_Uninitialize(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak uint32_t GenericPort_Write(int comPortNum, const char* data, size_t size)
{
    (void)comPortNum;
    (void)data;
    (void)size;

    NATIVE_PROFILE_PAL_COM();

    /*size_t writeResult = */chnWriteTimeout(&SERIAL_DRIVER_STDIO, (const uint8_t*)data, size, TIME_INFINITE);

    return 0;
}

__nfweak int GenericPort_Read(int comPortNum, char* data, size_t size)
{
    (void)comPortNum;
    (void)data;
    (void)size;

    NATIVE_PROFILE_PAL_COM();
    return 0;
}

__nfweak bool GenericPort_Flush(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return true;
}

__nfweak bool GenericPort_IsSslSupported(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return false;
}

__nfweak bool GenericPort_UpgradeToSsl(int comPortNum, unsigned int flags) 
{
    (void)comPortNum;
    (void)flags;

    NATIVE_PROFILE_PAL_COM();
    return false; 
}

__nfweak bool GenericPort_IsUsingSsl(int comPortNum)
{
    (void)comPortNum;

    NATIVE_PROFILE_PAL_COM();
    return false;
}
#endif