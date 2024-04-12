//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoPAL.h>

// //--//

// void FS_MountVolume( LPCSTR nameSpace, UINT32 serialNumber, UINT32 deviceFlags, BlockStorageDevice* blockStorageDevice )
// {
// }

// void FS_UnmountVolume( BlockStorageDevice* blockStorageDevice )
// {
// }

// //--//

__nfweak void FS_Initialize()
{
}

// //--//

// HAL_DblLinkedList<FileSystemVolume> FileSystemVolumeList::s_volumeList;

// //--//

void FileSystemVolumeList::Initialize()
{
}

__nfweak bool FileSystemVolumeList::InitializeVolumes()
{
    return TRUE;
}

__nfweak bool FileSystemVolumeList::UninitializeVolumes()
{
    return TRUE;
}

// BOOL FileSystemVolumeList::AddVolume( FileSystemVolume* fsv, LPCSTR nameSpace, UINT32 serialNumber, UINT32 deviceFlags,
//                                       STREAM_DRIVER_INTERFACE* streamDriver, FILESYSTEM_DRIVER_INTERFACE* fsDriver,
//                                       BlockStorageDevice* blockStorageDevice, UINT32 volumeId, BOOL init )
// {
//     return TRUE;
// }

// BOOL FileSystemVolumeList::RemoveVolume( FileSystemVolume* fsv, BOOL uninit )
// {
//     return TRUE;
// }

// FileSystemVolume* FileSystemVolumeList::GetFirstVolume()
// { 
//     return NULL;
// }

// FileSystemVolume* FileSystemVolumeList::GetNextVolume( FileSystemVolume& volume )
// { 
//     return NULL;
// }

// UINT32 FileSystemVolumeList::GetNumVolumes()
// {
//     return 0;
// }

// FileSystemVolume* FileSystemVolumeList::FindVolume( LPCSTR nameSpace, UINT32 nameSpaceLength )
// {
//     return NULL;
// }

// BOOL FileSystemVolumeList::Contains( FileSystemVolume* fsv )
// {
//     return FALSE;
// }
