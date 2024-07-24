//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef LITTLEFS_FS_DRIVER_H
#define LITTLEFS_FS_DRIVER_H

#include <nanoCLR_Headers.h>
#include <nanoHAL_v2.h>
#include <esp32_idf.h>
#include <ff.h>

#ifdef __cplusplus
extern "C"
{
#endif

    struct LITTLEFS_FileHandle
    {
        FILE *file;
    };

    struct LITTLEFS_FindFileHandle
    {
        FILE *file;
        DIR *dir;
        char basePath[FS_MAX_DIRECTORY_LENGTH];
    };

    struct LITTLEFS_FS_Driver
    {
        static void Initialize();
        static bool InitializeVolume(const VOLUME_ID *volume, const char *path);
        static bool UnInitializeVolume(const VOLUME_ID *volume);
        static HRESULT Format(const VOLUME_ID *volume, const char *volumeLabel, uint32_t parameters);
        static HRESULT GetSizeInfo(const VOLUME_ID *volume, int64_t *totalSize, int64_t *totalFreeSpace);
        static HRESULT FlushAll(const VOLUME_ID *volume);
        static HRESULT GetVolumeLabel(const VOLUME_ID *volume, char *volumeLabel, int32_t volumeLabelLen);

        //--//
        static bool LoadMedia(const void *driverInterface);
        static STREAM_DRIVER_DETAILS *DriverDetails(const VOLUME_ID *volume);

        static HRESULT Open(const VOLUME_ID *volume, const char *path, void *&handle);
        static HRESULT Close(void *handle);
        static HRESULT Read(void *handle, uint8_t *buffer, int size, int *readsize);
        static HRESULT Write(void *handle, uint8_t *buffer, int size, int *writesize);
        static HRESULT Flush(void *handle);
        static HRESULT Seek(void *handle, int64_t offset, uint32_t Flag, int64_t *position);
        static HRESULT GetLength(void *handle, int64_t *size);
        static HRESULT SetLength(void *handle, int64_t size);

        //--//
        static HRESULT FindOpen(const VOLUME_ID *volume, const char *fileSpec, void *&findHandle);
        static HRESULT FindNext(void *handle, FS_FILEINFO *fi, bool *fileFound);
        static HRESULT FindClose(void *handle);
        static HRESULT GetFileInfo(const VOLUME_ID *volume, const char *path, FS_FILEINFO *fileInfo, bool *found);
        static HRESULT GetAttributes(const VOLUME_ID *volume, const char *path, uint32_t *attributes);
        static HRESULT SetAttributes(const VOLUME_ID *volume, const char *path, uint32_t attributes);
        static HRESULT CreateDirectory(const VOLUME_ID *volume, const char *path);
        static HRESULT Move(const VOLUME_ID *volume, const char *oldPath, const char *newPath);
        static HRESULT Delete(const VOLUME_ID *volume, const char *path, bool recursive);

        //--//
    };

#ifdef __cplusplus
}
#endif

#endif // LITTLEFS_FS_DRIVER_H
