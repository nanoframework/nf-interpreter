//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_runtime_ifu.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetStatus___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateStatus__nanoFrameworkRuntimeInFieldUpdateImageType,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetPrimaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetSecondaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetImageList___STATIC__SZARRAY_nanoFrameworkRuntimeInFieldUpdateImageInfo,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::EraseSecondaryImage___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateImageType,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::StoreImageChunk___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateImageType__SZARRAY_U1__I4__I4,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::ConfirmDeploymentImage___STATIC__BOOLEAN,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestDeploymentRevert___STATIC__BOOLEAN,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestClrRevert___STATIC__BOOLEAN,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestReboot___STATIC__VOID,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_InFieldUpdate =
{
    "nanoFramework.Runtime.InFieldUpdate",
    0x4089B617,
    method_lookup,
    { 100, 0, 0, 1 }
};

// clang-format on
