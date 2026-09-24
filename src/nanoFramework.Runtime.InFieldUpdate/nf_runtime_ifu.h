//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_RUNTIME_IFU_H
#define NF_RUNTIME_IFU_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>
#include <MCUboot_ImageSlotInfo.h>

// MCUboot_UpdateSession.h includes THIS header for the managed UpdateSessionResult/UpdateSessionOwner
// enums, so it cannot be included from here. The helpers below only take a session by reference,
// which an incomplete type satisfies; the .cpp includes the session header for the full definition.
struct Ifu_Session;

typedef enum __nfpack ImageType
{
    ImageType_NanoClr = 0,
    ImageType_Deployment = 1,
} ImageType;

typedef enum __nfpack SlotId
{
    SlotId_Primary = 0,
    SlotId_Secondary = 1,
} SlotId;

typedef enum __nfpack UpdateSessionOwner
{
    UpdateSessionOwner_None = 0,
    UpdateSessionOwner_Managed = 1,
    UpdateSessionOwner_WireProtocol = 2,
    UpdateSessionOwner_Native = 3,
} UpdateSessionOwner;

typedef enum __nfpack UpdateSessionResult
{
    UpdateSessionResult_Success = 0,
    UpdateSessionResult_Busy = 1,
    UpdateSessionResult_BadToken = 2,
    UpdateSessionResult_BadOffset = 3,
    UpdateSessionResult_TooLarge = 4,
    UpdateSessionResult_FlashError = 5,
    UpdateSessionResult_BadMagic = 6,
    UpdateSessionResult_SwapInFlight = 7,
    UpdateSessionResult_NoImage = 8,
    UpdateSessionResult_HeaderMismatch = 9,
    UpdateSessionResult_Incomplete = 10,
    UpdateSessionResult_BadTlv = 11,
    UpdateSessionResult_HashMismatch = 12,
    UpdateSessionResult_BadArgument = 13,
    UpdateSessionResult_NoSlot = 14,
} UpdateSessionResult;

typedef enum __nfpack UpdateStatus
{
    UpdateStatus_Confirmed = 0,
    UpdateStatus_Testing = 1,
    UpdateStatus_TestPending = 2,
    UpdateStatus_PermanentPending = 3,
    UpdateStatus_RollbackPending = 4,
    UpdateStatus_Unknown = 255,
} UpdateStatus;

struct Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_ImageInfo
{
    // renamed backing field '<Image>k__BackingField'
    static const int FIELD__Image = 1;
    // renamed backing field '<Slot>k__BackingField'
    static const int FIELD__Slot = 2;
    // renamed backing field '<Version>k__BackingField'
    static const int FIELD__Version = 3;
    // renamed backing field '<ImageHash>k__BackingField'
    static const int FIELD__ImageHash = 4;
    // renamed backing field '<HasValidHeader>k__BackingField'
    static const int FIELD__HasValidHeader = 5;
    // renamed backing field '<IsActive>k__BackingField'
    static const int FIELD__IsActive = 6;
    // renamed backing field '<IsConfirmed>k__BackingField'
    static const int FIELD__IsConfirmed = 7;
    // renamed backing field '<IsPending>k__BackingField'
    static const int FIELD__IsPending = 8;
    // renamed backing field '<IsBootable>k__BackingField'
    static const int FIELD__IsBootable = 9;
    // renamed backing field '<IsRollbackPending>k__BackingField'
    static const int FIELD__IsRollbackPending = 10;

    //--//
};

struct Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_ImageInfoExtensions
{
    static const int FIELD_STATIC__Headers = 0;

    //--//
};

struct Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager
{
    NANOCLR_NATIVE_DECLARE(
        GetStatus___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateStatus__nanoFrameworkRuntimeInFieldUpdateImageType);
    NANOCLR_NATIVE_DECLARE(
        GetPrimaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType);
    NANOCLR_NATIVE_DECLARE(
        GetSecondaryImageInfo___STATIC__nanoFrameworkRuntimeInFieldUpdateImageInfo__nanoFrameworkRuntimeInFieldUpdateImageType);
    NANOCLR_NATIVE_DECLARE(GetImageList___STATIC__SZARRAY_nanoFrameworkRuntimeInFieldUpdateImageInfo);
    NANOCLR_NATIVE_DECLARE(EraseSecondaryImage___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateImageType);
    NANOCLR_NATIVE_DECLARE(
        StartUpdateSession___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSession__nanoFrameworkRuntimeInFieldUpdateImageType__I4);
    NANOCLR_NATIVE_DECLARE(
        ResumeUpdateSession___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSession__nanoFrameworkRuntimeInFieldUpdateImageType__I4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(
        StoreImageChunk___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateUpdateSession__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(
        CompleteUpdateSession___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSessionResult__nanoFrameworkRuntimeInFieldUpdateUpdateSession);
    NANOCLR_NATIVE_DECLARE(
        AbortUpdateSession___STATIC__BOOLEAN__nanoFrameworkRuntimeInFieldUpdateUpdateSession__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(
        GetUpdateSessionOwner___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSessionOwner__nanoFrameworkRuntimeInFieldUpdateImageType);
    NANOCLR_NATIVE_DECLARE(GetLastSessionError___STATIC__nanoFrameworkRuntimeInFieldUpdateUpdateSessionResult);
    NANOCLR_NATIVE_DECLARE(ConfirmDeploymentImage___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(RequestDeploymentRevert___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(RequestClrRevert___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(RequestReboot___STATIC__VOID);

    //--//

    static HRESULT Ifu_PopulateImageInfo(
        CLR_RT_HeapBlock &destSlot,
        uint8_t imageIndex,
        uint8_t slotIndex,
        const Ifu_SlotSnapshot &snapshot,
        CLR_RT_TypeDef_Index imageInfoTypeDef,
        CLR_RT_TypeDef_Index versionTypeDef);
    static HRESULT Ifu_GetImageInfoForSlot(CLR_RT_StackFrame &stack, uint8_t slotIndex);
    static bool Ifu_IsPrimaryConfirmed(uint8_t imageIndex);
    static HRESULT Ifu_OpenSession(CLR_RT_StackFrame &stack, bool resume);
    static HRESULT Ifu_PopulateSession(CLR_RT_HeapBlock &destSlot, const Ifu_Session &session, bool resumed);
    static HRESULT Ifu_UpdateSessionHeader(CLR_RT_HeapBlock *pSession, const struct image_header &hdr);
    static HRESULT Ifu_ReadSessionArg(
        CLR_RT_HeapBlock &arg,
        uint8_t &imageIndex,
        uint32_t &token,
        CLR_RT_HeapBlock *&pSession);
};

struct Library_nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateSession
{
    // renamed backing field '<Image>k__BackingField'
    static const int FIELD__Image = 1;
    // renamed backing field '<Token>k__BackingField'
    static const int FIELD__Token = 2;
    // renamed backing field '<TotalLength>k__BackingField'
    static const int FIELD__TotalLength = 3;
    // renamed backing field '<NextOffset>k__BackingField'
    static const int FIELD__NextOffset = 4;
    // renamed backing field '<IsResumed>k__BackingField'
    static const int FIELD__IsResumed = 5;
    // renamed backing field '<Version>k__BackingField'
    static const int FIELD__Version = 6;
    // renamed backing field '<HeaderSize>k__BackingField'
    static const int FIELD__HeaderSize = 7;
    // renamed backing field '<ImageSize>k__BackingField'
    static const int FIELD__ImageSize = 8;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_InFieldUpdate;

#endif // NF_RUNTIME_IFU_H
