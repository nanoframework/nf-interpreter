//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef NANOCLR_FILESTREAM_H
#define NANOCLR_FILESTREAM_H

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <nanoCLR_Types.h>
#include <nanoCLR_Interop.h>
// #include <nanoCLR_ErrorCodes.h>
// #include <nanoSupport.h>
// #include <nanoWeak.h>

struct CLR_RT_FileStream
{
  private:
    FileSystemVolume *m_driver;
    STREAM_DRIVER_DETAILS *m_driverDetails;
    uint8_t *m_dataIn;
    uint8_t *m_dataOut;
    int m_dataInSize;
    int m_dataOutSize;
    void *m_handle;

    //--//

  public:
    static HRESULT CreateInstance(CLR_RT_HeapBlock &ref, const char *path, int bufferSize);

    static void RelocationHandler(CLR_RT_HeapBlock_BinaryBlob *ptr);

    static HRESULT SplitFilePath(const char *fullPath, char *&rootName, uint32_t &rootNameLength, char *&relativePath);

    //--//

    void Relocate();

    //--//

    HRESULT AssignStorage(uint8_t *storageIn, size_t sizeIn, uint8_t *storageOut, size_t sizeOut);

    HRESULT Read(uint8_t *buffer, int count, int *bytesRead);
    HRESULT Write(uint8_t *buffer, int count, int *bytesWritten);
    HRESULT Seek(int64_t offset, uint32_t origin, int64_t *position);

    HRESULT Flush();

    HRESULT GetLength(int64_t *length);
    HRESULT SetLength(int64_t length);

    HRESULT Close();

    bool CanRead()
    {
        return m_driverDetails->canRead;
    }
    bool CanWrite()
    {
        return m_driverDetails->canWrite;
    }
    bool CanSeek()
    {
        return m_driverDetails->canSeek;
    }

    int GetReadTimeout()
    {
        return m_driverDetails->readTimeout;
    }
    int GetWriteTimeout()
    {
        return m_driverDetails->writeTimeout;
    }

    FS_BUFFERING_STRATEGY GetBufferingStrategy()
    {
        return m_driverDetails->bufferingStrategy;
    }
};

#endif // NANOCLR_FILESTREAM_H
