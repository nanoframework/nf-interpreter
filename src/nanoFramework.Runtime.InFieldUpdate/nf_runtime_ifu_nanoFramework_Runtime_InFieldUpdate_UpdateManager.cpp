//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_runtime_ifu.h"
#include <flash_map_backend/flash_map_backend.h>
#include <sysflash/sysflash.h>


HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetStatus___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateStatus__nanoFrameworkRuntimeInFieldUpdateImageType( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetPrimaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetSecondaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::GetImageList___STATIC__SZARRAY_nanoFrameworkRuntimeInFieldUpdateImageInfo( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::EraseSecondaryImage___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateImageType( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    uint8_t imageIndex = (uint8_t)stack.Arg0().NumericByRef().s4;
    bool success = false;

    int faId = FLASH_AREA_IMAGE_SECONDARY(imageIndex);
    const struct flash_area *fa = NULL;

    if (faId != FLASH_SLOT_DOES_NOT_EXIST && flash_area_open((uint8_t)faId, &fa) == 0 && fa != NULL)
    {
        if (flash_area_erase(fa, 0, fa->fa_size) == 0)
        {
            success = true;
        }

        flash_area_close(fa);
    }

    stack.SetResult_Boolean(success);

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::StoreImageChunk___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateImageType__SZARRAY_U1__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::ConfirmDeploymentImage___STATIC__BOOLEAN( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestDeploymentRevert___STATIC__BOOLEAN( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestClrRevert___STATIC__BOOLEAN( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestReboot___STATIC__VOID( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}
