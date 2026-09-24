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
#include <nanoHAL_Time.h>
#include <MCUboot_UpdateSession.h>

typedef Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_ImageInfo ImageInfo;
typedef Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateSession UpdateSession;
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

    // refused while an update session is open on the image by any writer
    stack.SetResult_Boolean(Ifu_EraseSecondary(imageIndex) == UpdateSessionResult_Success);

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    StartUpdateSession___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSession__nanoFrameworkRuntimeInFieldUpdateImageType__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Ifu_OpenSession(stack, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    ResumeUpdateSession___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSession__nanoFrameworkRuntimeInFieldUpdateImageType__I4__SZARRAY_U1(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Ifu_OpenSession(stack, true));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    StoreImageChunk___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateUpdateSession__SZARRAY_U1__I4__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pSession = NULL;
    CLR_RT_HeapBlock_Array *data = stack.Arg1().DereferenceArray();
    CLR_INT32 offset = stack.Arg2().NumericByRef().s4;
    CLR_INT32 count = stack.Arg3().NumericByRef().s4;
    uint8_t imageIndex = 0;
    uint32_t token = 0;
    uint32_t nextOffset = 0;
    bool firstChunk = false;
    struct image_header hdr;
    UpdateSessionResult result = UpdateSessionResult_Success;

    NANOCLR_CHECK_HRESULT(Ifu_ReadSessionArg(stack.Arg0(), imageIndex, token, pSession));

    FAULT_ON_NULL_ARG(data);

    if (offset < 0 || count < 0 || (CLR_UINT32)offset + (CLR_UINT32)count > data->m_numOfElements)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    firstChunk = (pSession[UpdateSession::FIELD__NextOffset].NumericByRef().s4 == 0) && count >= (CLR_INT32)sizeof(hdr);

    if (firstChunk)
    {
        memcpy(&hdr, data->GetFirstElement() + offset, sizeof(hdr));
    }

    result = Ifu_SessionWrite(imageIndex, token, data->GetFirstElement() + offset, (uint32_t)count, &nextOffset);

    if (result == UpdateSessionResult_Success)
    {
        pSession[UpdateSession::FIELD__NextOffset].SetInteger((CLR_INT32)nextOffset);

        if (firstChunk && count > 0)
        {
            NANOCLR_CHECK_HRESULT(Ifu_UpdateSessionHeader(pSession, hdr));
        }
    }

    stack.SetResult_Boolean(result == UpdateSessionResult_Success);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    CompleteUpdateSession___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSessionResult__nanoFrameworkRuntimeInFieldUpdateUpdateSession(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pSession = NULL;
    uint8_t imageIndex = 0;
    uint32_t token = 0;
    Ifu_Session snapshot;
    UpdateSessionResult result;

    NANOCLR_CHECK_HRESULT(Ifu_ReadSessionArg(stack.Arg0(), imageIndex, token, pSession));

    memset(&snapshot, 0, sizeof(snapshot));

    result = Ifu_SessionComplete(imageIndex, token, &snapshot);

    if (result == UpdateSessionResult_Success)
    {
        pSession[UpdateSession::FIELD__NextOffset].SetInteger((CLR_INT32)snapshot.NextOffset);

        NANOCLR_CHECK_HRESULT(Ifu_UpdateSessionHeader(pSession, snapshot.Header));
    }

    stack.SetResult_I4((CLR_INT32)result);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    AbortUpdateSession___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateUpdateSession__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pSession = NULL;
    uint8_t imageIndex = 0;
    uint32_t token = 0;
    bool eraseSlot = (stack.Arg1().NumericByRef().u1 != 0);

    NANOCLR_CHECK_HRESULT(Ifu_ReadSessionArg(stack.Arg0(), imageIndex, token, pSession));

    stack.SetResult_Boolean(Ifu_SessionAbort(imageIndex, token, eraseSlot) == UpdateSessionResult_Success);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    GetUpdateSessionOwner___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSessionOwner__nanoFrameworkRuntimeInFieldUpdateImageType(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t imageIndex = (uint8_t)stack.Arg0().NumericByRef().s4;

    stack.SetResult_I4((CLR_INT32)Ifu_SessionOwner(imageIndex));

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    GetLastSessionError___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSessionResult(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_I4((CLR_INT32)Ifu_SessionLastStatus());

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    ConfirmDeploymentImage___STATIC__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // read the trailer back to report the actual state because a zero return alone does not mean the image is confirmed
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

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::Ifu_OpenSession(CLR_RT_StackFrame &stack, bool resume)
{
    NANOCLR_HEADER();

    uint8_t imageIndex = (uint8_t)stack.Arg0().NumericByRef().s4;
    CLR_INT32 totalLength = stack.Arg1().NumericByRef().s4;
    CLR_RT_HeapBlock_Array *expectedHeader = NULL;
    const uint8_t *expected = NULL;
    uint32_t expectedLen = 0;
    Ifu_Session session;
    UpdateSessionResult result;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    if (totalLength <= 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    if (resume)
    {
        expectedHeader = stack.Arg2().DereferenceArray();

        if (expectedHeader != NULL)
        {
            expected = expectedHeader->GetFirstElement();
            expectedLen = expectedHeader->m_numOfElements;
        }
    }

    memset(&session, 0, sizeof(session));

    if (resume)
    {
        result = Ifu_SessionResume(
            imageIndex,
            UpdateSessionOwner_Managed,
            (uint32_t)totalLength,
            expected,
            expectedLen,
            (uint32_t)HAL_Time_CurrentSysTicks(),
            &session);
    }
    else
    {
        result = Ifu_SessionStart(
            imageIndex,
            UpdateSessionOwner_Managed,
            (uint32_t)totalLength,
            (uint32_t)HAL_Time_CurrentSysTicks(),
            &session);
    }

    if (result == UpdateSessionResult_Success)
    {
        NANOCLR_CHECK_HRESULT(Ifu_PopulateSession(top, session, resume));
    }

    NANOCLR_NOCLEANUP();
}

// Create the managed UpdateSession object for an open native session.
HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::Ifu_PopulateSession(
    CLR_RT_HeapBlock &destSlot,
    const Ifu_Session &session,
    bool resumed)
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index sessionTypeDef;
    CLR_RT_HeapBlock *pSession = NULL;

    if (!g_CLR_RT_TypeSystem.FindTypeDef("UpdateSession", "nanoFramework.Runtime.InFieldUpdate", sessionTypeDef))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
    }

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(destSlot, sessionTypeDef));

    pSession = destSlot.Dereference();
    if (pSession == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    pSession[UpdateSession::FIELD__Image].SetInteger((CLR_INT32)session.Image);
    pSession[UpdateSession::FIELD__Token].SetInteger((CLR_UINT32)session.Token);
    pSession[UpdateSession::FIELD__TotalLength].SetInteger((CLR_INT32)session.TotalLength);
    pSession[UpdateSession::FIELD__NextOffset].SetInteger((CLR_INT32)session.NextOffset);
    pSession[UpdateSession::FIELD__IsResumed].SetBoolean(resumed);

    if (session.HeaderKnown)
    {
        NANOCLR_CHECK_HRESULT(Ifu_UpdateSessionHeader(pSession, session.Header));
    }

    NANOCLR_NOCLEANUP();
}

// Fill the header-derived members (Version, HeaderSize, ImageSize) of a managed UpdateSession.
HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::Ifu_UpdateSessionHeader(
    CLR_RT_HeapBlock *pSession,
    const struct image_header &hdr)
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index versionTypeDef;
    CLR_RT_HeapBlock *pVersion = NULL;

    if (!g_CLR_RT_TypeSystem.FindTypeDef("Version", "System", versionTypeDef))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
    }

    NANOCLR_CHECK_HRESULT(
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(pSession[UpdateSession::FIELD__Version], versionTypeDef));

    pVersion = pSession[UpdateSession::FIELD__Version].Dereference();
    if (pVersion == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // same mapping as ImageInfo.Version
    pVersion[Version::FIELD___Major].SetInteger((CLR_INT32)hdr.ih_ver.iv_major);
    pVersion[Version::FIELD___Minor].SetInteger((CLR_INT32)hdr.ih_ver.iv_minor);
    pVersion[Version::FIELD___Build].SetInteger((CLR_INT32)hdr.ih_ver.iv_revision);
    pVersion[Version::FIELD___Revision].SetInteger((CLR_INT32)hdr.ih_ver.iv_build_num);

    pSession[UpdateSession::FIELD__HeaderSize].SetInteger((CLR_INT32)hdr.ih_hdr_size);
    pSession[UpdateSession::FIELD__ImageSize].SetInteger((CLR_INT32)hdr.ih_img_size);

    NANOCLR_NOCLEANUP();
}

// Dereference a managed UpdateSession argument and read the fields the registry needs.
HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::Ifu_ReadSessionArg(
    CLR_RT_HeapBlock &arg,
    uint8_t &imageIndex,
    uint32_t &token,
    CLR_RT_HeapBlock *&pSession)
{
    NANOCLR_HEADER();

    pSession = arg.Dereference();

    FAULT_ON_NULL_ARG(pSession);

    imageIndex = (uint8_t)pSession[UpdateSession::FIELD__Image].NumericByRef().s4;
    token = pSession[UpdateSession::FIELD__Token].NumericByRef().u4;

    NANOCLR_NOCLEANUP();
}
