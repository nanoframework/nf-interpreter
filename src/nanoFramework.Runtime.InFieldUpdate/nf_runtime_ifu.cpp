//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_runtime_ifu.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetStatus___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateStatus__nanoFrameworkRuntimeInFieldUpdateImageType,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetPrimaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetSecondaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetImageList___STATIC__SZARRAY_nanoFrameworkRuntimeInFieldUpdateImageInfo,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::EraseSecondaryImage___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateImageType,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::StartUpdateSession___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSession__nanoFrameworkRuntimeInFieldUpdateImageType__I4,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::ResumeUpdateSession___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSession__nanoFrameworkRuntimeInFieldUpdateImageType__I4__SZARRAY_U1,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::StoreImageChunk___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateUpdateSession__SZARRAY_U1__I4__I4,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::CompleteUpdateSession___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSessionResult__nanoFrameworkRuntimeInFieldUpdateUpdateSession,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::AbortUpdateSession___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateUpdateSession__BOOLEAN,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetUpdateSessionOwner___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSessionOwner__nanoFrameworkRuntimeInFieldUpdateImageType,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetLastSessionError___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSessionResult,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::ConfirmDeploymentImage___STATIC__BOOLEAN,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestDeploymentRevert___STATIC__BOOLEAN,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestClrRevert___STATIC__BOOLEAN,
    Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestReboot___STATIC__VOID,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_InFieldUpdate =
{
    "nanoFramework.Runtime.InFieldUpdate",
    0xFD280147,
    method_lookup,
    { 100, 0, 0, 2 }
};

// clang-format on
