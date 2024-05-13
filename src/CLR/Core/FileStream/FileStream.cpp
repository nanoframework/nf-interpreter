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

    char *rootName;
    char *relativePath;
    uint32_t rootNameLength = 0;
    uint32_t *rootNameLengthP = &rootNameLength;

    int inputBufferSize = 0;
    int outputBufferSize = 0;

    FileSystemVolume *driver;
    STREAM_DRIVER_DETAILS *sdd;

    NANOCLR_CHECK_HRESULT(CLR_RT_FileStream::SplitFilePath(path, rootName, rootNameLengthP, relativePath));

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
    uint32_t *&rootNameLength,
    char *&relativePath)
{

    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    static const char root[] = "\\";
    static const char root1[] = "/";

    bool isForwardSlash = false;

    const char *c = fullPath;
    uint32_t rootLen = 0;

    if (!fullPath || !rootName || !rootNameLength || !relativePath)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

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

    *rootNameLength = rootLen;

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

//--//

HRESULT CLR_RT_FindFile::CreateInstance(CLR_RT_HeapBlock &ref, const char *path, const char *searchPattern)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_BinaryBlob *blob;
    CLR_RT_FindFile *ff;
    uint32_t size;
    uint32_t i = 0, j;
    uint32_t fullPathBufferSize;

    char *rootName;
    char *relativePath;
    uint32_t rootNameLength = 0;
    uint32_t *rootNameLengthP = &rootNameLength;


    // We will support only the "*" search pattern for now
    if (hal_strncmp_s(searchPattern, "*", 2) != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    NANOCLR_CHECK_HRESULT(CLR_RT_FileStream::SplitFilePath(path, rootName, rootNameLengthP, relativePath));

    // '\' before the namespace
    fullPathBufferSize = FS_MAX_PATH_LENGTH + rootNameLength + 1;

    size = sizeof(CLR_RT_FindFile) + fullPathBufferSize * sizeof(char);

    NANOCLR_CHECK_HRESULT(
        CLR_RT_HeapBlock_BinaryBlob::CreateInstance(ref, size, NULL, CLR_RT_FindFile::RelocationHandler, 0));

    blob = ref.DereferenceBinaryBlob();
    ff = (CLR_RT_FindFile *)blob->GetData();

    // Clear the memory
    CLR_RT_Memory::ZeroFill(ff, size);

    /// Retrieve appropriate driver that handles this namespace.
    if ((ff->m_driver = FileSystemVolumeList::FindVolume(rootName, rootNameLength)) == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }

    // Validate all the find related function are present in the driver
    if (!(ff->m_driver->ValidateFind()))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    ff->m_fullPath = (void *)(&ff[1]);
    ff->m_fullPathBufferSize = fullPathBufferSize;

    // Copy the '\' and root name
    for (i = 0; i < rootNameLength + 1; i++)
    {
        ((char*)ff->m_fullPath)[i] = path[i];
    }

    // need to remove the '\' from the root name
    i--;

    // Copy the rest of the path from the relative path
    for (j = 0; i < fullPathBufferSize && relativePath[j] != 0; i++, j++)
    {
        ((char *)ff->m_fullPath)[i] = relativePath[j];
    }

    // Make sure we always ends with '\\'
    if (((char *)ff->m_fullPath)[i - 1] != '\\')
    {
        ((char *)ff->m_fullPath)[i++] = '\\';
        ((char *)ff->m_fullPath)[i] = 0;
    }

    ff->m_fi.FileName = &(((char *)ff->m_fullPath)[i]);
    ff->m_fi.FileNameSize = fullPathBufferSize - i;

    NANOCLR_CHECK_HRESULT(ff->m_driver->FindOpen(relativePath, ff->m_handle));

    NANOCLR_NOCLEANUP();
}

void CLR_RT_FindFile::Relocate()
{
    NATIVE_PROFILE_CLR_IO();

    CLR_RT_GarbageCollector::Heap_Relocate((void **)&(m_fi.FileName));
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&(m_fullPath));
}

void CLR_RT_FindFile::RelocationHandler(CLR_RT_HeapBlock_BinaryBlob *ptr)
{
    NATIVE_PROFILE_CLR_IO();
    CLR_RT_FindFile *pThis = (CLR_RT_FindFile *)ptr->GetData();

    pThis->Relocate();
}

HRESULT CLR_RT_FindFile::GetNext(FS_FILEINFO **fi, bool *found)
{
    *fi = &m_fi;

    return m_driver->FindNext(m_handle, &m_fi, found);
}

HRESULT CLR_RT_FindFile::CreateFilenameString(CLR_RT_HeapBlock &ref)
{
    // join found filename with the path
    char buffer[FS_MAX_PATH_LENGTH];
    char *bufferP = buffer;
    size_t bufferSize = FS_MAX_PATH_LENGTH;

    CLR_SafeSprintf(bufferP, bufferSize, "%s%s", m_fullPath, m_fi.FileName);

    return CLR_RT_HeapBlock_String::CreateInstance(ref, buffer, hal_strlen_s(buffer));
}

HRESULT CLR_RT_FindFile::Close()
{
    return m_driver->FindClose(m_handle);
}
