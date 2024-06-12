//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include <nanoCRT.h>
#include <nanoPAL.h>

HAL_DblLinkedList<FileSystemVolume> FileSystemVolumeList::s_zombieVolumeList;

//--//

bool FS_MountVolume(const char *rootName, uint32_t deviceFlags, const char *fileSystemDriver)
{
    FileSystemVolume *volume = NULL;
    FILESYSTEM_DRIVER_INTERFACE *fsDriver = NULL;
    STREAM_DRIVER_INTERFACE *streamDriver = NULL;
    uint32_t volumeId;

    if (!rootName)
    {
        return FALSE;
    }

    //--//

    // free up any memory taken up by this block storage device in the zombie list
    // so at any given time, we'll only have one set of FileSystemVolume per block
    // storage device. Here we release (private_free) the memory that was allocated
    // in the previous insertion, later, prior to AddVolume, we'll allocate new memory
    // needed for this insertion.
    FileSystemVolume *current = FileSystemVolumeList::s_zombieVolumeList.FirstValidNode();
    FileSystemVolume *next;

    while (current)
    {
        next = current->Next();

        if (!(next && next->Next()))
        {
            next = NULL;
        }

        // We'll only free the memory of this storage device
        if (hal_stricmp(current->m_rootName, rootName) == 0)
        {
            current->Unlink();

            platform_free(current);
        }

        current = next;
    }

    // find the FSDriver that will mount the block storage
    for (uint32_t i = 0; i < g_InstalledFSCount; i++)
    {
        if (g_AvailableFSInterfaces[i].fsDriver &&
            hal_stricmp(g_AvailableFSInterfaces[i].fsDriver->Name, fileSystemDriver) == 0)
        {
            fsDriver = g_AvailableFSInterfaces[i].fsDriver;
            streamDriver = g_AvailableFSInterfaces[i].streamDriver;
            break;
        }
    }

    // get the next free volume id
    volumeId = FileSystemVolumeList::GetNextFreeVolumeId();

    // allocate the memory for this FileSystemVolume
    volume = (FileSystemVolume *)platform_malloc(sizeof(FileSystemVolume));

    // sanity check
    if (!volume)
    {
        return FALSE;
    }

    // initialize content to 0
    memset(volume, 0, sizeof(FileSystemVolume));

    if (FileSystemVolumeList::AddVolume(
            volume,
            rootName,
            deviceFlags,
            streamDriver,
            fsDriver,
            volumeId,
            (fsDriver) ? TRUE : FALSE))
    {

        // Now we can notify managed code
        PostManagedEvent(EVENT_STORAGE, EVENT_SUBCATEGORY_MEDIAINSERT, 0, (uint32_t)volume->m_volumeId.volumeId);

        // done here
        return TRUE;
    }
    else
    {
        // if for some reason, AddVolume fails, there is not much that can be done
        platform_free(volume);

        return FALSE;
    }
}

void FS_UnmountVolume(const char *rootName)
{
    FileSystemVolume *volume = FileSystemVolumeList::FindVolume(rootName, hal_strlen_s(rootName));

    if (volume)
    {
        // Let FS uninitialize now for this volume. Note this happens before managed stack is informed
        FileSystemVolumeList::RemoveVolume(volume, TRUE);

        // Move the volume into the zombie list rather than free up the memory so all the subsequent Close() from opened
        // handles will complete and/or fail properly
        FileSystemVolumeList::s_zombieVolumeList.LinkAtBack(volume);

        // Notify managed code
        PostManagedEvent(EVENT_STORAGE, EVENT_SUBCATEGORY_MEDIAEJECT, 0, (uint32_t)volume->m_volumeId.volumeId);
    }
}

//--//

void FS_Initialize()
{
    for (uint32_t i = 0; i < g_InstalledFSCount; i++)
    {
        g_AvailableFSInterfaces[i].streamDriver->Initialize();
    }
}

//--//

HAL_DblLinkedList<FileSystemVolume> FileSystemVolumeList::s_volumeList;

//--//

void FileSystemVolumeList::Initialize()
{
    s_volumeList.Initialize();
    s_zombieVolumeList.Initialize();
}

bool FileSystemVolumeList::InitializeVolumes()
{
    FileSystemVolume *volume = s_volumeList.FirstNode();

    if (volume == NULL)
    {
#if defined(PLATFORM_ARM)
        debug_printf("There are no file system volume to initialize");
#endif
        return FALSE;
    }

    bool success = TRUE;

    while (volume->Next())
    {
        if (volume->InitializeVolume())
        {
            // all good
            // volume->m_fsDriver->GetVolumeLabel(&volume->m_volumeId, volume->m_label, ARRAYSIZE(volume->m_label));
        }
        else
        {
            // even if success == FALSE, InitalizeVolume() will still get called
            success = FALSE;
        }

        volume = volume->Next();
    }

    return success;
}

bool FileSystemVolumeList::UninitializeVolumes()
{
    bool success = TRUE;

    FileSystemVolume *volume = s_volumeList.FirstNode();
    FileSystemVolume *curVolume;

    while (volume->Next())
    {
        // even if success == FALSE, UninitalizeVolume() will still get called
        success = volume->UninitializeVolume() && success;
        curVolume = volume;

        volume = volume->Next();

        curVolume->Unlink();
    }

    return success;
}

bool FileSystemVolumeList::AddVolume(
    FileSystemVolume *fsv,
    const char *rootName,
    uint32_t deviceFlags,
    STREAM_DRIVER_INTERFACE *streamDriver,
    FILESYSTEM_DRIVER_INTERFACE *fsDriver,
    uint32_t volumeId,
    bool init)
{
    bool success = TRUE;

    if (!fsv)
    {
        return FALSE;
    }

    FileSystemVolume *current;
    bool nameOK = true;

    // Detect if this namespace is already taken over or not.
    current = FileSystemVolumeList::GetFirstVolume();

    while (current)
    {
        if (0 == hal_stricmp(current->m_rootName, rootName))
        {
            nameOK = false;
            break;
        }

        current = FileSystemVolumeList::GetNextVolume(*current);
    }

    // if the name is already taken, there is no need to add the volume
    if (nameOK == false)
    {
        return TRUE;
    }

    // initialize the members of the FileSystemVolume
    memcpy(fsv->m_rootName, rootName, FS_NAME_MAXLENGTH);

    fsv->m_deviceFlags = deviceFlags;
    fsv->m_streamDriver = streamDriver;
    fsv->m_fsDriver = fsDriver;
    fsv->m_volumeId.volumeId = volumeId;

    // init the dblinkednode;
    fsv->Initialize();
    if (init)
    {
        success = fsv->InitializeVolume();

        // fsDriver->GetVolumeLabel(&fsv->m_volumeId, fsv->m_label, ARRAYSIZE(fsv->m_label));
    }
    else
    {
        fsv->m_label[0] = 0;
    }

    // only add the volume if initialization was successful, when requested at all
    if (success)
    {
        s_volumeList.LinkAtBack(fsv);
        return TRUE;
    }

    return FALSE;
}

bool FileSystemVolumeList::RemoveVolume(FileSystemVolume *fsv, bool uninit)
{
    if (fsv->IsLinked())
    {
        fsv->Unlink();

        if (uninit)
        {
            fsv->UninitializeVolume();
        }

        return TRUE;
    }

    return FALSE;
}

FileSystemVolume *FileSystemVolumeList::GetFirstVolume()
{
    return s_volumeList.FirstValidNode();
}

FileSystemVolume *FileSystemVolumeList::GetNextVolume(FileSystemVolume &volume)
{
    FileSystemVolume *nextVolume = volume.Next();

    if (nextVolume && nextVolume->Next())
    {
        return nextVolume;
    }

    return NULL;
}

uint32_t FileSystemVolumeList::GetNumVolumes()
{
    return s_volumeList.NumOfNodes();
}

FileSystemVolume *FileSystemVolumeList::FindVolume(const char *rootName, uint32_t rootNameLength)
{
    FileSystemVolume *volume = GetFirstVolume();
    char root[FS_NAME_MAXLENGTH];

    memcpy(root, rootName, rootNameLength);
    root[rootNameLength] = 0;

    while (volume)
    {
        if (hal_stricmp(volume->m_rootName, root) == 0)
        {
            // Make sure we match the entire namespace string by checking for null terminator
            // In case rootName is a substring of m_rootName (i.e. rootName = "ROOT", m_rootName == "ROOT1")
            if (volume->m_rootName[rootNameLength] == '\0')
            {
                return volume;
            }
        }

        volume = FileSystemVolumeList::GetNextVolume(*volume);
    }

    return NULL;
}

FileSystemVolume *FileSystemVolumeList::FindVolume(uint32_t volumeId)
{
    FileSystemVolume *volume = GetFirstVolume();

    while (volume)
    {
        if (volume->m_volumeId.volumeId == volumeId)
        {
            return volume;
        }

        volume = FileSystemVolumeList::GetNextVolume(*volume);
    }

    return NULL;
}

uint32_t FileSystemVolumeList::GetNextFreeVolumeId()
{
    FileSystemVolume *volume = GetFirstVolume();
    int64_t maxVolumeId = -1;

    while (volume)
    {
        if (volume->m_volumeId.volumeId > maxVolumeId)
        {
            maxVolumeId = volume->m_volumeId.volumeId;
        }

        volume = FileSystemVolumeList::GetNextVolume(*volume);
    }

    return maxVolumeId + 1;
}

bool FileSystemVolumeList::Contains(FileSystemVolume *fsv)
{
    FileSystemVolume *volume = GetFirstVolume();

    while (volume)
    {
        if (volume == fsv)
        {
            return TRUE;
        }

        volume = FileSystemVolumeList::GetNextVolume(*volume);
    }

    return FALSE;
}
