//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <Core.h>
#include <nanoCLR_Types.h>
#include <nanoCLR_FileStream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_FileStream::CreateInstance(CLR_RT_HeapBlock &ref, const char *path, int bufferSize)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_BinaryBlob *blob;
    uint32_t blobSize = sizeof(CLR_RT_FileStream);
    CLR_RT_FileStream *fs;

    char rootNameBuffer[FS_NAME_MAXLENGTH + 1];
    char relativePathBuffer[FS_MAX_PATH_LENGTH + 1];
    char *rootName = rootNameBuffer;
    char *relativePath = relativePathBuffer;
    uint32_t rootNameLength = -1;
    int inputBufferSize = 0;
    int outputBufferSize = 0;

    FileSystemVolume *driver;
    STREAM_DRIVER_DETAILS *sdd;

    NANOCLR_CHECK_HRESULT(CLR_RT_FileStream::SplitFilePath(path, rootName, rootNameLength, relativePath));

    // Retrieve appropriate driver that handles this root name
    if ((driver = FileSystemVolumeList::FindVolume(rootName, rootNameLength)) == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_DRIVER);
    }

    if (!(driver->ValidateStreamDriver()))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_DRIVER);
    }

    sdd = driver->DriverDetails();

    if (sdd->bufferingStrategy == SYSTEM_BUFFERED_IO)
    {
        if (bufferSize == 0)
        {
            inputBufferSize = (sdd->inputBufferSize > 0) ? sdd->inputBufferSize : FS_DEFAULT_BUFFER_SIZE;
            outputBufferSize = (sdd->outputBufferSize > 0) ? sdd->outputBufferSize : FS_DEFAULT_BUFFER_SIZE;
        }
        else
        {
            inputBufferSize = bufferSize;
            outputBufferSize = bufferSize;
        }

        blobSize += inputBufferSize + outputBufferSize;
    }

    NANOCLR_CHECK_HRESULT(
        CLR_RT_HeapBlock_BinaryBlob::CreateInstance(ref, blobSize, NULL, CLR_RT_FileStream::RelocationHandler, 0));

    blob = ref.DereferenceBinaryBlob();
    fs = (CLR_RT_FileStream *)blob->GetData();

    // Clear the memory
    CLR_RT_Memory::ZeroFill(fs, blobSize);

    fs->m_driver = driver;
    fs->m_driverDetails = sdd;

    switch (sdd->bufferingStrategy)
    {
        // I/O is asynchronous from a PAL level buffer: the runtime will alocate the necesary memory
        case SYSTEM_BUFFERED_IO:
        {
            uint8_t *inputBuffer = (uint8_t *)&(fs[1]);
            uint8_t *outputBuffer = inputBuffer + inputBufferSize;

            NANOCLR_CHECK_HRESULT(fs->AssignStorage(inputBuffer, inputBufferSize, outputBuffer, outputBufferSize));
            break;
        }
        // I/O is asynchronous from a HAL level or HW buffer: the runtime will just handle the existing memory
        case DRIVER_BUFFERED_IO:
            if ((sdd->inputBuffer == NULL && sdd->canRead) || (sdd->outputBuffer == NULL && sdd->canWrite))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_DRIVER);
            }

            NANOCLR_CHECK_HRESULT(
                fs->AssignStorage(sdd->inputBuffer, sdd->inputBufferSize, sdd->outputBuffer, sdd->outputBufferSize));
            break;

        case SYNC_IO:
        case DIRECT_IO:
        default:
            // all other buffering strategies are not supported
            break;
    }

    NANOCLR_CHECK_HRESULT(driver->Open(relativePath, fs->m_handle));

    NANOCLR_NOCLEANUP();
}

void CLR_RT_FileStream::Relocate()
{
    NATIVE_PROFILE_CLR_IO();

    if (m_driverDetails->bufferingStrategy == SYSTEM_BUFFERED_IO)
    {
        CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_dataIn);
        CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_dataOut);
    }
}

void CLR_RT_FileStream::RelocationHandler(CLR_RT_HeapBlock_BinaryBlob *ptr)
{
    NATIVE_PROFILE_CLR_IO();

    CLR_RT_FileStream *pThis = (CLR_RT_FileStream *)ptr->GetData();

    pThis->Relocate();
}

HRESULT CLR_RT_FileStream::SplitFilePath(
    const char *fullPath,
    char *&rootName,
    uint32_t &rootNameLength,
    char *&relativePath)
{

    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    static const char root[] = "\\";
    static const char root1[] = "/";

    bool isForwardSlash = false;

    const char *c = fullPath;
    uint32_t rootLen = 0;

    rootName = const_cast<char *>(c);
    // handle both '\' and '/' as path separator
    while ((*c != '\\') && (*c != '/') && (*c != 0))
    {
        if (*c == '/')
        {
            isForwardSlash = true;
        }

        c++;
        rootLen++;
    }

    // relative path always have to start with a '\' (or '/') (*c will be '\0' only when fullPath is \<root>)
    relativePath =
        (*c == 0) ? (isForwardSlash ? const_cast<char *>(root1) : const_cast<char *>(root)) : const_cast<char *>(c);

    // Invalid paths should be taken care of by Path.NormalizePath() in the managed code
    if (rootLen >= FS_NAME_MAXLENGTH || hal_strlen_s(relativePath) >= FS_MAX_PATH_LENGTH)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_PATH_TOO_LONG);
    }

    rootNameLength = rootLen;

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_FileStream::AssignStorage(uint8_t *bufferIn, size_t sizeIn, uint8_t *bufferOut, size_t sizeOut)
{
    NANOCLR_HEADER();

    m_dataIn = bufferIn;
    m_dataInSize = sizeIn;
    m_dataOut = bufferOut;
    m_dataOutSize = sizeOut;

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT CLR_RT_FileStream::Read(uint8_t *buffer, int count, int *bytesRead)
{
    NANOCLR_HEADER();

    switch (m_driverDetails->bufferingStrategy)
    {
        case SYNC_IO:
        case DIRECT_IO:
            NANOCLR_CHECK_HRESULT(m_driver->Read(m_handle, buffer, count, bytesRead));

            break;

        case SYSTEM_BUFFERED_IO:
        case DRIVER_BUFFERED_IO:
            NANOCLR_CHECK_HRESULT(m_driver->Read(m_handle, m_dataIn, MIN(count, m_dataInSize), bytesRead));

            if (*bytesRead > 0)
            {
                memcpy(buffer, m_dataIn, *bytesRead);
            }

            break;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_FileStream::Write(uint8_t *buffer, int count, int *bytesWritten)
{
    NANOCLR_HEADER();

    switch (m_driverDetails->bufferingStrategy)
    {
        case SYNC_IO:
        case DIRECT_IO:
            NANOCLR_CHECK_HRESULT(m_driver->Write(m_handle, buffer, count, bytesWritten));

            break;

        case SYSTEM_BUFFERED_IO:
        case DRIVER_BUFFERED_IO:
            count = MIN(count, m_dataOutSize);

            memcpy(m_dataOut, buffer, count);

            NANOCLR_CHECK_HRESULT(m_driver->Write(m_handle, m_dataOut, count, bytesWritten));

            break;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_FileStream::Seek(int64_t offset, uint32_t origin, int64_t *position)
{
    return m_driver->Seek(m_handle, offset, origin, position);
}

HRESULT CLR_RT_FileStream::Close()
{
    return m_driver->Close(m_handle);
}

HRESULT CLR_RT_FileStream::Flush()
{
    return m_driver->Flush(m_handle);
}

HRESULT CLR_RT_FileStream::GetLength(int64_t *length)
{
    return m_driver->GetLength(m_handle, length);
}

HRESULT CLR_RT_FileStream::SetLength(int64_t length)
{
    return m_driver->SetLength(m_handle, length);
}
