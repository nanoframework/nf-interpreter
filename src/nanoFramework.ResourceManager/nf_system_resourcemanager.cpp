//
// Copyright (c) .NET Foundation and Contributors
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
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::FindResource___STATIC__I4__STRING__SystemReflectionAssembly,
    Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum,
    Library_nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum,
    Library_nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum__I4__I4,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_ResourceManager =
{
    "nanoFramework.ResourceManager",
    0xDCD7DF4D,
    method_lookup,
    { 100, 0, 0, 1 }
};
