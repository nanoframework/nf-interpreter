//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_SYSTEM_RESOURCEMANAGER_H_
#define _NF_SYSTEM_RESOURCEMANAGER_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <corlib_native.h>

struct Library_nf_system_resourcemanager_System_Resources_ResourceManager
{
    static const int FIELD___resourceFileId = 1;
    static const int FIELD___assembly = 2;
    static const int FIELD___baseAssembly = 3;
    static const int FIELD___baseName = 4;
    static const int FIELD___cultureName = 5;
    static const int FIELD___rmFallback = 6;

    NANOCLR_NATIVE_DECLARE(GetObjectInternal___OBJECT__I2);
    NANOCLR_NATIVE_DECLARE(GetObjectInternal___OBJECT__I2__I4__I4);
    NANOCLR_NATIVE_DECLARE(FindResource___STATIC__I4__STRING__mscorlibSystemReflectionAssembly);
    NANOCLR_NATIVE_DECLARE(GetObject___STATIC__OBJECT__SystemResourcesResourceManager__mscorlibSystemEnum);

    //--//

    NANOCLR_NATIVE_DECLARE(GetObject___STATIC__OBJECT__SystemResourcesResourceManager__mscorlibSystemEnum__I4__I4);    

};

struct Library_nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility
{
    NANOCLR_NATIVE_DECLARE(GetObject___STATIC__OBJECT__SystemResourcesResourceManager__mscorlibSystemEnum);
    NANOCLR_NATIVE_DECLARE(GetObject___STATIC__OBJECT__SystemResourcesResourceManager__mscorlibSystemEnum__I4__I4);

    //--//

};



extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_ResourceManager;

#endif  //_NF_SYSTEM_RESOURCEMANAGER_H_
