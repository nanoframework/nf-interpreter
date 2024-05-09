//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef LITTLEFS_FS_DRIVER_H
#define LITTLEFS_FS_DRIVER_H

#include <nanoCLR_Headers.h>
//#include <target_littlefs.h>
#include <nanoHAL_v2.h>
#include <lfs.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct LITTLEFS_FileHandle
{
    lfs_t *fs;
    lfs_file_t file;
    struct lfs_attr attr;
    struct lfs_file_config fileConfig;
    uint32_t nanoAttributes;
};

struct LITTLEFS_FindFileHandle
{
    lfs_t *fs;
    lfs_file_t file;
    lfs_dir_t dir;
    lfs_soff_t offset;
};

struct LITTLEFS_FS_Driver
{
    static void Initialize();
    static bool InitializeVolume( const VOLUME_ID *volume );
    static bool UnInitializeVolume( const VOLUME_ID *volume );
    static HRESULT Format( const VOLUME_ID *volume, LPCSTR volumeLabel, uint32_t parameters );
    static HRESULT GetSizeInfo( const VOLUME_ID *volume, int64_t* totalSize, int64_t* totalFreeSpace );
    static HRESULT FlushAll( const VOLUME_ID* volume );
    static HRESULT GetVolumeLabel( const VOLUME_ID* volume, const char* volumeLabel, int32_t volumeLabelLen );

    //--//
    static bool LoadMedia( const void *driverInterface );
    static STREAM_DRIVER_DETAILS *DriverDetails( const VOLUME_ID *volume );
    
    static HRESULT Open( const VOLUME_ID *volume, const char * path, uint32_t *handle );
    static HRESULT Close( uint32_t handle );
    static HRESULT Read( uint32_t handle, uint8_t *buffer, int size, int *readsize );
    static HRESULT Write( uint32_t handle, uint8_t *buffer, int size, int *writesize );
    static HRESULT Flush( uint32_t handle );
    static HRESULT Seek( uint32_t handle, int64_t offset, uint32_t Flag, int64_t *position );
    static HRESULT GetLength( uint32_t handle, int64_t *size );
    static HRESULT SetLength( uint32_t handle, int64_t size );
    
    //--//
    static HRESULT FindOpen(const VOLUME_ID *volume, const char * fileSpec, uint32_t *findHandle);
    static HRESULT FindNext(uint32_t handle, FS_FILEINFO *fi, bool *fileFound);
    static HRESULT FindClose(uint32_t handle);
    static HRESULT GetFileInfo( const VOLUME_ID* volume, const char * path, FS_FILEINFO* fileInfo, bool* found );
    static HRESULT GetAttributes(const VOLUME_ID *volume, const char * path, uint32_t *attributes);
    static HRESULT SetAttributes(const VOLUME_ID *volume, const char * path, uint32_t attributes);
    static HRESULT CreateDirectory(const VOLUME_ID *volume, const char * path);
    static HRESULT Move(const VOLUME_ID *volume, const char * oldPath, const char * newPath);
    static HRESULT Delete(const VOLUME_ID *volume, const char * path, bool recursive);

    //--//
};

#ifdef __cplusplus
}
#endif

#endif // LITTLEFS_FS_DRIVER_H
