//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_system_resourcemanager.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObjectInternal___OBJECT__I2,
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObjectInternal___OBJECT__I2__I4__I4,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::FindResource___STATIC__I4__STRING__SystemReflectionAssembly,
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum,
    Library_nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum,
    Library_nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum__I4__I4,
    nullptr,
    nullptr,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_ResourceManager =
{
    "nanoFramework.ResourceManager",
    0x874533DF,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
