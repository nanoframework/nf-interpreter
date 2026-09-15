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

typedef Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_ImageInfo ImageInfo;
typedef Library_corlib_native_System_Version Version;

// Number of flash slots reported per image (primary + secondary).
static const uint8_t c_Ifu_SlotsPerImage = 2;

HRESULT Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::
    GetStatus___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateStatus__nanoFrameworkRuntimeInFieldUpdateImageType(
        CLR_RT_StackFrame &stack)
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
        // Derive active/confirmed/pending/rollback-pending purely from boot_swap_type_multi(),
        int swapType = boot_swap_type_multi(image);
        bool isRollbackPending = (swapType == BOOT_SWAP_TYPE_REVERT);

        for (uint8_t slot = 0; slot < c_Ifu_SlotsPerImage; slot++, entryRef++)
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*entryRef, imageInfoTypeDef));

            CLR_RT_HeapBlock *pImageInfo = entryRef->Dereference();
            if (pImageInfo == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            Ifu_ReadSlotSnapshot(image, slot, snapshot);

            bool isActive = false;
            bool isConfirmed = false;
            bool isPending = false;

            if (slot == SlotId_Primary)
            {
                // the primary slot holds the running image
                isActive = true;

                if (swapType != BOOT_SWAP_TYPE_REVERT)
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

            pImageInfo[ImageInfo::FIELD__Image].SetInteger((CLR_INT32)image);
            pImageInfo[ImageInfo::FIELD__Slot].SetInteger((CLR_INT32)slot);
            pImageInfo[ImageInfo::FIELD__HasValidHeader].SetBoolean(snapshot.TlvValid);
            pImageInfo[ImageInfo::FIELD__IsBootable].SetBoolean(snapshot.HeaderValid && snapshot.Bootable);
            pImageInfo[ImageInfo::FIELD__IsActive].SetBoolean(isActive);
            pImageInfo[ImageInfo::FIELD__IsConfirmed].SetBoolean(isConfirmed);
            pImageInfo[ImageInfo::FIELD__IsPending].SetBoolean(isPending);
            pImageInfo[ImageInfo::FIELD__IsRollbackPending].SetBoolean(isRollbackPending);

            if (snapshot.HeaderValid)
            {
                CLR_RT_HeapBlock &versionField = pImageInfo[ImageInfo::FIELD__Version];

                NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(versionField, versionTypeDef));

                CLR_RT_HeapBlock *pVersion = versionField.Dereference();
                if (pVersion == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                pVersion[Version::FIELD___Major].SetInteger((CLR_INT32)snapshot.MajorVersion);
                pVersion[Version::FIELD___Minor].SetInteger((CLR_INT32)snapshot.MinorVersion);
                pVersion[Version::FIELD___Build].SetInteger((CLR_INT32)snapshot.BuildNumber);
                pVersion[Version::FIELD___Revision].SetInteger((CLR_INT32)snapshot.RevisionNumber);
            }

            // ImageHash is only populated when the TLV SHA-256 entry was actually found.
            if (snapshot.HasHash)
            {
                CLR_RT_HeapBlock &hashField = pImageInfo[ImageInfo::FIELD__ImageHash];

                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                    hashField,
                    sizeof(snapshot.Hash),
                    g_CLR_RT_WellKnownTypes.UInt8));

                memcpy(hashField.DereferenceArray()->GetFirstElement(), snapshot.Hash, sizeof(snapshot.Hash));
            }
        }
    }

    NANOCLR_SET_AND_LEAVE(S_OK);

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

// Resolve the flash area ID for a (image+slot) pair.
int Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::Ifu_GetFlashAreaId(uint8_t imageIndex, uint8_t slotIndex)
{
    if (slotIndex == SlotId_Secondary)
    {
        return FLASH_AREA_IMAGE_SECONDARY(imageIndex);
    }

    return FLASH_AREA_IMAGE_PRIMARY(imageIndex);
}

// Read the image header and (best-effort) SHA-256 digest from a slot.
void Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager::Ifu_ReadSlotSnapshot(uint8_t imageIndex, uint8_t slotIndex, Ifu_SlotSnapshot &snapshot)
{
    memset(&snapshot, 0, sizeof(snapshot));

    int faId = Ifu_GetFlashAreaId(imageIndex, slotIndex);
    if (faId == FLASH_SLOT_DOES_NOT_EXIST)
    {
        return;
    }

    const struct flash_area *fa = NULL;
    if (flash_area_open((uint8_t)faId, &fa) != 0 || fa == NULL)
    {
        return;
    }

    struct image_header hdr;

    if (flash_area_read(fa, 0, &hdr, sizeof(hdr)) == 0 && hdr.ih_magic == IMAGE_MAGIC)
    {
        snapshot.HeaderValid = true;

        snapshot.MajorVersion = hdr.ih_ver.iv_major;
        snapshot.MinorVersion = hdr.ih_ver.iv_minor;
        snapshot.RevisionNumber = hdr.ih_ver.iv_revision;
        snapshot.BuildNumber = hdr.ih_ver.iv_build_num;

        if (!(hdr.ih_flags & IMAGE_F_NON_BOOTABLE))
        {
            snapshot.Bootable = true;
        }

        // Locate the unprotected TLV area and extract the SHA-256 digest (best-effort).
        uint32_t tlvOff = (uint32_t)hdr.ih_hdr_size + hdr.ih_img_size + hdr.ih_protect_tlv_size;
        struct image_tlv_info tlvInfo;

        if (flash_area_read(fa, tlvOff, &tlvInfo, sizeof(tlvInfo)) == 0 && tlvInfo.it_magic == IMAGE_TLV_INFO_MAGIC)
        {
            snapshot.TlvValid = true;

            uint32_t pos = tlvOff + sizeof(tlvInfo);
            uint32_t end = tlvOff + tlvInfo.it_tlv_tot;

            while (pos + sizeof(struct image_tlv) <= end)
            {
                struct image_tlv tlv;
                if (flash_area_read(fa, pos, &tlv, sizeof(tlv)) != 0)
                {
                    break;
                }

                pos += sizeof(tlv);

                if (tlv.it_type == IMAGE_TLV_SHA256 && tlv.it_len == sizeof(snapshot.Hash))
                {
                    if (flash_area_read(fa, pos, snapshot.Hash, sizeof(snapshot.Hash)) == 0)
                    {
                        snapshot.HasHash = true;
                    }
                    break;
                }

                pos += tlv.it_len;
            }
        }
    }

    flash_area_close(fa);
}
