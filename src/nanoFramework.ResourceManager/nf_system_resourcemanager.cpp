//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_system_resourcemanager.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObjectInternal___OBJECT__I2,
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObjectInternal___OBJECT__I2__I4__I4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::FindResource___STATIC__I4__STRING__mscorlibSystemReflectionAssembly,
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__mscorlibSystemEnum,
    Library_nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__mscorlibSystemEnum,
    Library_nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__mscorlibSystemEnum__I4__I4,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_ResourceManager =
{
    "nanoFramework.ResourceManager", 
    0xCDF9E76C,
    method_lookup,
    ////////////////////////////////////////////////////////////////////////////////////
    // check if the version bellow matches the one in AssemblyNativeVersion attribute //
    ////////////////////////////////////////////////////////////////////////////////////
    { 100, 0, 0, 0 }
};
