//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOPAL_FILESYSTEM_H
#define NANOPAL_FILESYSTEM_H

#ifdef VIRTUAL_DEVICE
// #include <stdafx.h>
#include <nanoCLR_Headers.h>
#include <nf_errors_exceptions.h>
#include <nanoCLR_PlatformDef.h>
#else
#include <nanoHAL.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    struct FileSystemVolume;
    struct VOLUME_ID;

////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.IO.NativeIO (in managed code) !!! //
////////////////////////////////////////////////////////////////////

// All these length includes the NULL termination at the end of the string

// To maintain compatibility with the desktop, the max "relative" path we can allow. 2 is the MAX_DRIVE, i.e. "C:", "D:"
// ... etc
#define FS_MAX_PATH_LENGTH     (260 - 2)
#define FS_MAX_FILENAME_LENGTH 256
// As required by desktop, the longest directory path is MAX_PATH - 12 (size of an 8.3 file name)
#define FS_MAX_DIRECTORY_LENGTH (FS_MAX_PATH_LENGTH - 12)

#define FS_NAME_DEFAULT_LENGTH 7
#define FS_NAME_MAXLENGTH      (FS_NAME_DEFAULT_LENGTH + 1)

#define FS_LABEL_DEFAULT_LENGTH 11
#define FS_LABEL_MAXLENGTH      (FS_LABEL_DEFAULT_LENGTH + 1)

#define FS_DEFAULT_BUFFER_SIZE 512
#define FS_DEFAULT_TIMEOUT     (-1)

#define SEEKORIGIN_BEGIN   0
#define SEEKORIGIN_CURRENT 1
#define SEEKORIGIN_END     2

    struct FS_FILEINFO
    {
        uint32_t Attributes;
        int64_t Size;
        // This will point to a buffer of size FileNameSize
        void *FileName;
        // This is the size of the buffer, determined by the size of the base path given in FindOpen
        uint32_t FileNameSize;
    };

    enum FS_BUFFERING_STRATEGY
    {
        // I/O is synchronous and does not require buffering
        SYNC_IO = 1,
        // I/O is asynchronous from the managed application heap
        DIRECT_IO = 2,
        // I/O is asynchronous from a PAL level buffer
        SYSTEM_BUFFERED_IO = 3,
        // I/O is asynchronous from a HAL level or HW buffer
        DRIVER_BUFFERED_IO = 4,
    };

    struct STREAM_DRIVER_DETAILS
    {
        FS_BUFFERING_STRATEGY bufferingStrategy;
        uint8_t *inputBuffer;
        uint8_t *outputBuffer;
        int inputBufferSize;
        int outputBufferSize;
        bool canRead;
        bool canWrite;
        bool canSeek;
        int readTimeout;
        int writeTimeout;
    };

    //--//

    typedef bool (*FS_LOADMEDIA)(const void * /*pointer to media*/);
    typedef HRESULT (*FS_FORMAT)(const VOLUME_ID * /*volume*/, const char * /*volumeLabel*/, uint32_t /*parameters*/);
    typedef HRESULT (
        *FS_GETSIZEINFO)(const VOLUME_ID * /*volume*/, int64_t * /*totalSize*/, int64_t * /*totalFreeSpace*/);
    typedef HRESULT (*FS_FLUSHALL)(const VOLUME_ID * /*volume*/);
    typedef HRESULT (
        *FS_GETVOLUMELABEL)(const VOLUME_ID * /*volume*/, char * /*volumeLabel*/, int32_t /*volumeLabelLen*/);

    typedef HRESULT (*FS_FINDOPEN)(const VOLUME_ID * /*volume*/, const char * /*path*/, void *& /*findHandle*/);
    typedef HRESULT (*FS_FINDNEXT)(void * /*findHandle*/, FS_FILEINFO * /*findData*/, bool * /*found*/);
    typedef HRESULT (*FS_FINDCLOSE)(void * /*findHandle*/);

    typedef HRESULT (*FS_GETFILEINFO)(
        const VOLUME_ID * /*volume*/,
        const char * /*path*/,
        FS_FILEINFO * /*fileInfo*/,
        bool * /*found*/);

    typedef HRESULT (*FS_CREATEDIRECTORY)(const VOLUME_ID * /*volume*/, const char * /*path*/);
    typedef HRESULT (*FS_MOVE)(const VOLUME_ID * /*volume*/, const char * /*oldPath*/, const char * /*newPath*/);
    typedef HRESULT (*FS_DELETE)(const VOLUME_ID * /*volume*/, const char * /*path*/, bool /*recursive*/);
    typedef HRESULT (*FS_GETATTRIBUTES)(const VOLUME_ID * /*volume*/, const char * /*path*/, uint32_t * /*attributes*/);
    typedef HRESULT (*FS_SETATTRIBUTES)(const VOLUME_ID * /*volume*/, const char * /*path*/, uint32_t /*attributes*/);

    //--//

#ifdef CreateDirectory
#undef CreateDirectory
#endif

    struct FILESYSTEM_DRIVER_INTERFACE
    {
        FS_FINDOPEN FindOpen;
        FS_FINDNEXT FindNext;
        FS_FINDCLOSE FindClose;

        FS_GETFILEINFO GetFileInfo;

        FS_CREATEDIRECTORY CreateDirectory;
        FS_MOVE Move;
        FS_DELETE Delete;

        FS_GETATTRIBUTES GetAttributes;
        FS_SETATTRIBUTES SetAttributes;

        FS_FORMAT Format;
        FS_LOADMEDIA LoadMedia;
        FS_GETSIZEINFO GetSizeInfo;
        FS_FLUSHALL FlushAll;
        FS_GETVOLUMELABEL GetVolumeLabel;

        const char *Name;
        uint32_t Flags;
    };

    //--//

    typedef void (*STREAM_INITIALIZE)();
    typedef bool (*STREAM_INITIALIZEVOLUME)(const VOLUME_ID * /*volume*/, const char * /*path*/);
    typedef bool (*STREAM_UNINITIALIZEVOLUME)(const VOLUME_ID * /*volume*/);
    typedef STREAM_DRIVER_DETAILS *(*STREAM_DRIVERDETAILS)(const VOLUME_ID * /*volume*/);
    typedef HRESULT (*STREAM_OPEN)(const VOLUME_ID * /*volume*/, const char * /*path*/, void *& /*handle*/);
    typedef HRESULT (*STREAM_CLOSE)(void * /*handle*/);
    typedef HRESULT (*STREAM_READ)(void * /*handle*/, uint8_t * /*buffer*/, int /*count*/, int * /*readCount*/);
    typedef HRESULT (*STREAM_WRITE)(void * /*handle*/, uint8_t * /*buffer*/, int /*count*/, int * /*writtenCount*/);
    typedef HRESULT (*STREAM_FLUSH)(void * /*handle*/);
    typedef HRESULT (*STREAM_SEEK_)(
        void * /*handle*/,
        int64_t /*offset*/,
        uint32_t /*origin*/,
        int64_t * /*position*/); // STREAM_SEEK conflicts with some windows header, use STREAM_SEEK_ instead
    typedef HRESULT (*STREAM_GETLENGTH)(void * /*handle*/, int64_t * /*length*/);
    typedef HRESULT (*STREAM_SETLENGTH)(void * /*handle*/, int64_t /*length*/);

    struct STREAM_DRIVER_INTERFACE
    {
        STREAM_INITIALIZE Initialize;
        STREAM_INITIALIZEVOLUME InitializeVolume;
        STREAM_UNINITIALIZEVOLUME UninitializeVolume;
        STREAM_DRIVERDETAILS DriverDetails;
        STREAM_OPEN Open;
        STREAM_CLOSE Close;
        STREAM_READ Read;
        STREAM_WRITE Write;
        STREAM_FLUSH Flush;
        STREAM_SEEK_ Seek;
        STREAM_GETLENGTH GetLength;
        STREAM_SETLENGTH SetLength;
    };

//--//

// Storage event bit fields
// 0-7  : Event Sub Category (Insert, Eject etc.)
// 8-15 : Category (always EVENT_STORAGE)
// 16-23: Internal flags.
// 24-31: Driver supplied custom flags.

// TODO
// #define EVENT_STORAGE                 3
#define EVENT_SUBCATEGORY_MEDIAINSERT 1
#define EVENT_SUBCATEGORY_MEDIAEJECT  2

    bool FS_MountVolume(const char *rootName, uint32_t deviceFlags, const char *fileSystemDriver);
    void FS_UnmountVolume(const char *rootName);

    void FS_Initialize();

    struct FILESYSTEM_INTERFACES
    {
        FILESYSTEM_DRIVER_INTERFACE *fsDriver;
        STREAM_DRIVER_INTERFACE *streamDriver;
    };

    extern const size_t g_InstalledFSCount;
    extern FILESYSTEM_INTERFACES g_AvailableFSInterfaces[];

    //--//

    struct VOLUME_ID
    {
        uint32_t volumeId;
    };

    struct FileSystemVolume : public HAL_DblLinkedNode<FileSystemVolume>
    {

      public:
        bool InitializeVolume()
        {
            if (!m_streamDriver || !(m_streamDriver->InitializeVolume))
            {
                return FALSE;
            }

            return m_streamDriver->InitializeVolume(&m_volumeId, m_rootName);
        }

        bool UninitializeVolume()
        {
            // it's we don't have valid stream driver, do nothing
            if (!m_streamDriver || !(m_streamDriver->UninitializeVolume))
            {
                return TRUE;
            }

            return m_streamDriver->UninitializeVolume(&m_volumeId);
        }

        HRESULT Format(const char *volumeLabel, uint32_t parameters)
        {
            if (!m_fsDriver || !(m_fsDriver->Format))
            {
                return CLR_E_NOT_SUPPORTED;
            }

            return m_fsDriver->Format(&m_volumeId, volumeLabel, parameters);
        }

        HRESULT GetSizeInfo(int64_t *totalSize, int64_t *totalFreeSpace)
        {
            if (!m_fsDriver || !(m_fsDriver->GetSizeInfo))
            {
                return CLR_E_NOT_SUPPORTED;
            }

            return m_fsDriver->GetSizeInfo(&m_volumeId, totalSize, totalFreeSpace);
        }

        HRESULT FlushAll()
        {
            if (!m_fsDriver || !(m_fsDriver->FlushAll))
            {
                return CLR_E_NOT_SUPPORTED;
            }

            return m_fsDriver->FlushAll(&m_volumeId);
        }

        HRESULT FindOpen(const char *path, void *&findHandle)
        {
            // Use ValidateFind() to validate, this assert is for debug purpose only
            _ASSERTE(m_fsDriver && m_fsDriver->FindOpen);

            return m_fsDriver->FindOpen(&m_volumeId, path, findHandle);
        }

        HRESULT FindNext(void *findHandle, FS_FILEINFO *findData, bool *found)
        {
            // Use ValidateFind() to validate, this assert is for debug purpose only
            _ASSERTE(m_fsDriver && m_fsDriver->FindNext);

            return m_fsDriver->FindNext(findHandle, findData, found);
        }

        HRESULT FindClose(void *findHandle)
        {
            // Use ValidateFind() to validate, this assert is for debug purpose only
            _ASSERTE(m_fsDriver && m_fsDriver->FindClose);

            return m_fsDriver->FindClose(findHandle);
        }

        HRESULT GetFileInfo(const char *path, FS_FILEINFO *fileInfo, bool *found)
        {
            if (!m_fsDriver || !(m_fsDriver->GetFileInfo))
            {
                return CLR_E_NOT_SUPPORTED;
            }

            return m_fsDriver->GetFileInfo(&m_volumeId, path, fileInfo, found);
        }

        HRESULT CreateDirectory(const char *path)
        {
            if (!m_fsDriver || !(m_fsDriver->CreateDirectory))
            {
                return CLR_E_NOT_SUPPORTED;
            }

            return m_fsDriver->CreateDirectory(&m_volumeId, path);
        }

        HRESULT Move(const char *oldPath, const char *newPath)
        {
            if (!m_fsDriver || !(m_fsDriver->Move))
            {
                return CLR_E_NOT_SUPPORTED;
            }

            return m_fsDriver->Move(&m_volumeId, oldPath, newPath);
        }

        HRESULT Delete(const char *path, bool recursive)
        {
            if (!m_fsDriver || !(m_fsDriver->Delete))
            {
                return CLR_E_NOT_SUPPORTED;
            }

            return m_fsDriver->Delete(&m_volumeId, path, recursive);
        }

        HRESULT GetAttributes(const char *path, uint32_t *attributes)
        {
            if (!m_fsDriver || !(m_fsDriver->GetAttributes))
            {
                return CLR_E_NOT_SUPPORTED;
            }

            return m_fsDriver->GetAttributes(&m_volumeId, path, attributes);
        }

        HRESULT SetAttributes(const char *path, uint32_t attributes)
        {
            if (!m_fsDriver || !(m_fsDriver->SetAttributes))
            {
                return CLR_E_NOT_SUPPORTED;
            }

            return m_fsDriver->SetAttributes(&m_volumeId, path, attributes);
        }

        STREAM_DRIVER_DETAILS *DriverDetails()
        {
            // Use ValidateStreamDriver() to validate, this assert is for debug purpose only
            _ASSERTE(m_streamDriver && m_streamDriver->DriverDetails);

            return m_streamDriver->DriverDetails(&m_volumeId);
        }

        HRESULT Open(const char *path, void *&handle)
        {
            // Use ValidateStreamDriver() to validate, this assert is for debug purpose only
            _ASSERTE(m_streamDriver && m_streamDriver->Open);

            return m_streamDriver->Open(&m_volumeId, path, handle);
        }

        HRESULT Close(void *handle)
        {
            // Use ValidateStreamDriver() to validate, this assert is for debug purpose only
            _ASSERTE(m_streamDriver && m_streamDriver->Close);

            return m_streamDriver->Close(handle);
        }

        HRESULT Read(void *handle, uint8_t *buffer, int count, int *readCount)
        {
            // Use ValidateStreamDriver() to validate, this assert is for debug purpose only
            _ASSERTE(m_streamDriver && m_streamDriver->Read);

            return m_streamDriver->Read(handle, buffer, count, readCount);
        }

        HRESULT Write(void *handle, uint8_t *buffer, int count, int *writtenCount)
        {
            // Use ValidateStreamDriver() to validate, this assert is for debug purpose only
            _ASSERTE(m_streamDriver && m_streamDriver->Write);

            return m_streamDriver->Write(handle, buffer, count, writtenCount);
        }

        HRESULT Flush(void *handle)
        {
            // Use ValidateStreamDriver() to validate, this assert is for debug purpose only
            _ASSERTE(m_streamDriver && m_streamDriver->Flush);

            return m_streamDriver->Flush(handle);
        }

        HRESULT Seek(void *handle, int64_t offset, uint32_t origin, int64_t *position)
        {
            // Use ValidateStreamDriver() to validate, this assert is for debug purpose only
            _ASSERTE(m_streamDriver && m_streamDriver->Seek);

            return m_streamDriver->Seek(handle, offset, origin, position);
        }

        HRESULT GetLength(void *handle, int64_t *length)
        {
            // Use ValidateStreamDriver() to validate, this assert is for debug purpose only
            _ASSERTE(m_streamDriver && m_streamDriver->GetLength);

            return m_streamDriver->GetLength(handle, length);
        }

        HRESULT SetLength(void *handle, int64_t length)
        {
            // Use ValidateStreamDriver() to validate, this assert is for debug purpose only
            _ASSERTE(m_streamDriver && m_streamDriver->SetLength);

            return m_streamDriver->SetLength(handle, length);
        }

        //--//

        bool ValidateStreamDriver()
        {
            STREAM_DRIVER_DETAILS *details;

            if (!m_streamDriver || !(m_streamDriver->DriverDetails))
            {
                // invalid streamDriver, or invalid DriverDetails pointer
                return FALSE;
            }

            details = m_streamDriver->DriverDetails(&m_volumeId);

            // Check for valid stream driver details
            if ((!details) ||
                // Check for valid bufferingStrategy
                (details->bufferingStrategy < SYNC_IO || details->bufferingStrategy > DRIVER_BUFFERED_IO) ||
                // Open, Close, Flush, InitializeVolume, and
                (!(m_streamDriver->Open) || !(m_streamDriver->Close) || !(m_streamDriver->Flush) ||
                 !(m_streamDriver->InitializeVolume) ||
                 //    UninitializeVolume is required on all streams
                 !(m_streamDriver->UninitializeVolume)) ||
                // if the stream can read, Read is required
                (details->canRead && !(m_streamDriver->Read)) ||
                // if the stream can write, Write is required
                (details->canWrite && !(m_streamDriver->Write)) ||
                // if the stream can seek, Seek and GetLength is required
                (details->canSeek && (!(m_streamDriver->Seek) || !(m_streamDriver->GetLength))) ||
                // if the stream can seek AND write, SetLength is required
                (details->canSeek && details->canWrite && (!(m_streamDriver->SetLength))))
            {
                return FALSE;
            }

            return TRUE;
        }

        bool ValidateFind()
        {
            return m_fsDriver && (m_fsDriver->FindOpen) && (m_fsDriver->FindNext) && (m_fsDriver->FindClose);
        }

        //--//

        char m_rootName[FS_NAME_MAXLENGTH];
        char m_label[FS_LABEL_MAXLENGTH];
        uint32_t m_deviceFlags;
        STREAM_DRIVER_INTERFACE *m_streamDriver;
        FILESYSTEM_DRIVER_INTERFACE *m_fsDriver;
        VOLUME_ID m_volumeId;
    };

    struct FileSystemVolumeList
    {
        // initailize the list
        static void Initialize();

        // walk through list of volumes and calls Init() function
        static bool InitializeVolumes();

        // walk through list of volumes and calls Uninit() function
        static bool UninitializeVolumes();

        // add fsv to the list
        // If init=true, the InitializeVolume() will be called.
        static bool AddVolume(
            FileSystemVolume *fsv,
            char const *rootName,
            uint32_t deviceFlags,
            STREAM_DRIVER_INTERFACE *streamDriver,
            FILESYSTEM_DRIVER_INTERFACE *fsDriver,
            uint32_t volumeId,
            bool init);

        // remove fsv from the list
        // uninit = true, UninitializeVolume() will be called.
        static bool RemoveVolume(FileSystemVolume *fsv, bool uninit);

        // returns a pointer to the first volume in the list or NULL if
        // there is none.
        static FileSystemVolume *GetFirstVolume();

        static FileSystemVolume *GetNextVolume(FileSystemVolume &volume);

        // returns number of volumes has been declared in the system
        static uint32_t GetNumVolumes();

        // returns the volume driver for the specified namespace
        static FileSystemVolume *FindVolume(char const *rootName, uint32_t rootNameLength);

        // returns the volume driver for the specified VolumeID
        static FileSystemVolume *FindVolume(uint32_t volumeId);

        // returns true if fsv is in the list, false otherwise
        static bool Contains(FileSystemVolume *fsv);

        // returns the next free volume ID
        static uint32_t GetNextFreeVolumeId();

        //--//

        static HAL_DblLinkedList<FileSystemVolume> s_volumeList;

        // The zombie list is for the file system volumes of a removable media after it's been ejected
        // so it can be cleaned up properly later
        static HAL_DblLinkedList<FileSystemVolume> s_zombieVolumeList;
    };

    //--//

    // Implement this function to add all the built-in, static volumes (i.e. ones that can't be removed) to the system
    // Use FileSystemVolumeList::AddVolume() to add individual volumes
    void FS_AddVolumes();

    // Implement this function to add all the removable volumes *that's inserted in the system at startup* to the system
    // Use FS_MountVolume() to add individual volumes
    void FS_MountRemovableVolumes();

#ifdef __cplusplus
}
#endif

#endif // NANOPAL_FILESYSTEM_H
