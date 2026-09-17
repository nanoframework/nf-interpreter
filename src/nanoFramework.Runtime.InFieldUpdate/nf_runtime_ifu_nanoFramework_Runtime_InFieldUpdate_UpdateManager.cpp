//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_runtime_ifu.h"
#include <flash_map_backend/flash_map_backend.h>
#include <sysflash/sysflash.h>
#include <mcuboot_config/mcuboot_config.h>
#include <bootutil/bootutil_public.h>
#include <bootutil/image.h>
#include <nanoHAL_v2.h>

typedef Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_ImageInfo ImageInfo;
typedef Library_corlib_native_System_Version Version;

// Number of flash slots reported per image (primary + secondary).
static const uint8_t c_Ifu_SlotsPerImage = 2;

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    GetStatus___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateStatus__nanoFrameworkRuntimeInFieldUpdateImageType(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t imageIndex = (uint8_t)stack.Arg0().NumericByRef().s4;
    UpdateStatus status;

    int swapType = boot_swap_type_multi(imageIndex);

    switch (swapType)
    {
        case BOOT_SWAP_TYPE_TEST:
            status = UpdateStatus_TestPending;
            break;

        case BOOT_SWAP_TYPE_PERM:
            status = UpdateStatus_PermanentPending;
            break;

        case BOOT_SWAP_TYPE_REVERT:
            status = UpdateStatus_RollbackPending;
            break;

        case BOOT_SWAP_TYPE_NONE:
            status = Ifu_IsPrimaryConfirmed(imageIndex) ? UpdateStatus_Confirmed : UpdateStatus_Testing;
            break;

        default:
            status = UpdateStatus_Unknown;
            break;
    }

    stack.SetResult_I4((CLR_INT32)status);

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    GetPrimaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Ifu_GetImageInfoForSlot(stack, SlotId_Primary));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    GetSecondaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Ifu_GetImageInfoForSlot(stack, SlotId_Secondary));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    GetImageList___STATIC__SZARRAY_nanoFrameworkRuntimeInFieldUpdateImageInfo(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // number of MCUboot images on this target (CLR + optional deployment)
    const uint8_t imageCount = (uint8_t)MCUBOOT_IMAGE_NUMBER;
    const uint8_t entryCount = (uint8_t)(imageCount * c_Ifu_SlotsPerImage);

    CLR_RT_TypeDef_Index imageInfoTypeDef;
    CLR_RT_TypeDef_Index versionTypeDef;
    CLR_RT_HeapBlock *entryRef;
    Ifu_SlotSnapshot snapshot;

    // result array: one ImageInfo entry per (image, slot) combination, always fully populated
    CLR_RT_HeapBlock &top = stack.PushValue();

    if (!g_CLR_RT_TypeSystem.FindTypeDef("ImageInfo", "nanoFramework.Runtime.InFieldUpdate", imageInfoTypeDef))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
    }

    if (!g_CLR_RT_TypeSystem.FindTypeDef("Version", "System", versionTypeDef))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
    }

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, entryCount, imageInfoTypeDef));

    entryRef = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

    for (uint8_t image = 0; image < imageCount; image++)
    {
        for (uint8_t slot = 0; slot < c_Ifu_SlotsPerImage; slot++, entryRef++)
        {
            Ifu_ReadSlotInfo(image, slot, &snapshot);

            NANOCLR_CHECK_HRESULT(
                Ifu_PopulateImageInfo(*entryRef, image, slot, snapshot, imageInfoTypeDef, versionTypeDef));
        }
    }

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    EraseSecondaryImage___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateImageType(CLR_RT_StackFrame &stack)
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

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    StoreImageChunk___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateImageType__SZARRAY_U1__I4__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t imageIndex = (uint8_t)stack.Arg0().NumericByRef().s4;
    CLR_RT_HeapBlock_Array *data = stack.Arg1().DereferenceArray();
    CLR_INT32 offset = stack.Arg2().NumericByRef().s4;
    CLR_INT32 length = stack.Arg3().NumericByRef().s4;
    bool success = false;

    int faId;
    const struct flash_area *fa = NULL;

    FAULT_ON_NULL_ARG(data);

    if (offset < 0 || length < 0 || length > (CLR_INT32)data->m_numOfElements)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    faId = Ifu_FlashAreaId(imageIndex, SlotId_Secondary);

    if (faId != FLASH_SLOT_DOES_NOT_EXIST && flash_area_open((uint8_t)faId, &fa) == 0 && fa != NULL)
    {
        bool ok = true;

        // first chunk: validate the MCUboot header magic and erase the slot
        if (offset == 0)
        {
            uint32_t magic = 0;

            if (length >= (CLR_INT32)sizeof(magic))
            {
                memcpy(&magic, data->GetFirstElement(), sizeof(magic));
            }

            if (magic != IMAGE_MAGIC || flash_area_erase(fa, 0, fa->fa_size) != 0)
            {
                ok = false;
            }
        }

        if (ok && (uint32_t)offset + (uint32_t)length > fa->fa_size)
        {
            ok = false;
        }

        if (ok && length > 0 && flash_area_write(fa, offset, data->GetFirstElement(), length) != 0)
        {
            ok = false;
        }

        success = ok;

        flash_area_close(fa);
    }

    stack.SetResult_Boolean(success);

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    ConfirmDeploymentImage___STATIC__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // need to read the trailer back to report the actual state because a zero return alone does not mean the image is
    // confirmed
    bool success =
        (boot_set_confirmed_multi(ImageType_Deployment) == 0) && Ifu_IsPrimaryConfirmed(ImageType_Deployment);

    stack.SetResult_Boolean(success);

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    RequestDeploymentRevert___STATIC__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    bool success = (boot_swap_type_multi(ImageType_Deployment) == BOOT_SWAP_TYPE_NONE) &&
                   !Ifu_IsPrimaryConfirmed(ImageType_Deployment);

    stack.SetResult_Boolean(success);

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestClrRevert___STATIC__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    Ifu_SlotSnapshot snapshot;
    bool success = false;

    Ifu_ReadSlotInfo(ImageType_NanoClr, SlotId_Secondary, &snapshot);

    if (snapshot.HeaderValid)
    {
        success = (boot_set_pending_multi(ImageType_NanoClr, 0) == 0);
    }

    stack.SetResult_Boolean(success);

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::RequestReboot___STATIC__VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    (void)stack;

    CPU_Reset();

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::Ifu_PopulateImageInfo(
    CLR_RT_HeapBlock &destSlot,
    uint8_t imageIndex,
    uint8_t slotIndex,
    const Ifu_SlotSnapshot &snapshot,
    CLR_RT_TypeDef_Index imageInfoTypeDef,
    CLR_RT_TypeDef_Index versionTypeDef)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pImageInfo = NULL;
    CLR_RT_HeapBlock *pVersion = NULL;

    int swapType = boot_swap_type_multi(imageIndex);
    bool isRollbackPending = (swapType == BOOT_SWAP_TYPE_REVERT);
    bool isActive = false;
    bool isConfirmed = false;
    bool isPending = false;

    if (slotIndex == SlotId_Primary)
    {
        isActive = snapshot.HeaderValid;

        if (swapType == BOOT_SWAP_TYPE_NONE)
        {
            isConfirmed = Ifu_IsPrimaryConfirmed(imageIndex);
        }
        else if (swapType != BOOT_SWAP_TYPE_REVERT)
        {
            isConfirmed = true;
        }
    }
    else
    {
        if (swapType == BOOT_SWAP_TYPE_TEST || swapType == BOOT_SWAP_TYPE_PERM)
        {
            isPending = true;
        }
        else if (swapType == BOOT_SWAP_TYPE_REVERT)
        {
            isConfirmed = true;
        }
    }

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(destSlot, imageInfoTypeDef));

    pImageInfo = destSlot.Dereference();
    if (pImageInfo == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    pImageInfo[ImageInfo::FIELD__Image].SetInteger((CLR_INT32)imageIndex);
    pImageInfo[ImageInfo::FIELD__Slot].SetInteger((CLR_INT32)slotIndex);
    pImageInfo[ImageInfo::FIELD__HasValidHeader].SetBoolean(snapshot.TlvValid);
    pImageInfo[ImageInfo::FIELD__IsBootable].SetBoolean(snapshot.HeaderValid && snapshot.Bootable);
    pImageInfo[ImageInfo::FIELD__IsActive].SetBoolean(isActive);
    pImageInfo[ImageInfo::FIELD__IsConfirmed].SetBoolean(isConfirmed);
    pImageInfo[ImageInfo::FIELD__IsPending].SetBoolean(isPending);
    pImageInfo[ImageInfo::FIELD__IsRollbackPending].SetBoolean(isRollbackPending);

    if (snapshot.HeaderValid)
    {
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(pImageInfo[ImageInfo::FIELD__Version], versionTypeDef));

        pVersion = pImageInfo[ImageInfo::FIELD__Version].Dereference();
        if (pVersion == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        pVersion[Version::FIELD___Major].SetInteger((CLR_INT32)snapshot.MajorVersion);
        pVersion[Version::FIELD___Minor].SetInteger((CLR_INT32)snapshot.MinorVersion);
        pVersion[Version::FIELD___Build].SetInteger((CLR_INT32)snapshot.RevisionNumber);
        pVersion[Version::FIELD___Revision].SetInteger((CLR_INT32)snapshot.BuildNumber);
    }

    // ImageHash is only populated when the TLV SHA-256 entry was actually found.
    if (snapshot.HasHash)
    {
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
            pImageInfo[ImageInfo::FIELD__ImageHash],
            sizeof(snapshot.Hash),
            g_CLR_RT_WellKnownTypes.UInt8));

        memcpy(
            pImageInfo[ImageInfo::FIELD__ImageHash].DereferenceArray()->GetFirstElement(),
            snapshot.Hash,
            sizeof(snapshot.Hash));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::Ifu_GetImageInfoForSlot(
    CLR_RT_StackFrame &stack,
    uint8_t slotIndex)
{
    NANOCLR_HEADER();

    uint8_t imageIndex = (uint8_t)stack.Arg0().NumericByRef().s4;

    CLR_RT_TypeDef_Index imageInfoTypeDef;
    CLR_RT_TypeDef_Index versionTypeDef;
    Ifu_SlotSnapshot snapshot;

    // PushValueAndClear (not plain PushValue) so the result is an explicit null reference when
    // this method leaves it untouched.
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    Ifu_ReadSlotInfo(imageIndex, slotIndex, &snapshot);

    if (snapshot.HeaderValid)
    {
        if (!g_CLR_RT_TypeSystem.FindTypeDef("ImageInfo", "nanoFramework.Runtime.InFieldUpdate", imageInfoTypeDef))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
        }

        if (!g_CLR_RT_TypeSystem.FindTypeDef("Version", "System", versionTypeDef))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
        }

        NANOCLR_CHECK_HRESULT(
            Ifu_PopulateImageInfo(top, imageIndex, slotIndex, snapshot, imageInfoTypeDef, versionTypeDef));
    }

    NANOCLR_NOCLEANUP();
}

// True only when the primary slot's swap-state trailer reads successfully and reports the image
// as confirmed (image_ok == BOOT_FLAG_SET). boot_swap_type_multi() alone returns
// BOOT_SWAP_TYPE_NONE both when the primary image is settled/confirmed and when it is running as
// an unconfirmed test image, so only the trailer's image_ok flag can tell the two apart.
bool Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::Ifu_IsPrimaryConfirmed(
    uint8_t imageIndex)
{
    struct boot_swap_state state;

    int faId = Ifu_FlashAreaId(imageIndex, SlotId_Primary);
    if (faId == FLASH_SLOT_DOES_NOT_EXIST)
    {
        return false;
    }

    if (boot_read_swap_state_by_id(faId, &state) != 0)
    {
        return false;
    }

    return state.image_ok == BOOT_FLAG_SET;
}
