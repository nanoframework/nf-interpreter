//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <Core.h>
#include <nanoCLR_Types.h>
#include <nanoCLR_Stream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_FileStream::CreateInstance(CLR_RT_HeapBlock &ref, const char *path, int bufferSize)
{
    (void)ref;
    (void)path;
    (void)bufferSize;

    NANOCLR_FEATURE_STUB_RETURN();
}

void CLR_RT_FileStream::Relocate()
{
}

void CLR_RT_FileStream::RelocationHandler(CLR_RT_HeapBlock_BinaryBlob *ptr)
{
    (void)ptr;
}

HRESULT CLR_RT_FileStream::SplitFilePath(
    const char *fullPath,
    const char **nameSpace,
    uint32_t *nameSpaceLength,
    const char **relativePath)
{
    (void)fullPath;
    (void)nameSpace;
    (void)nameSpaceLength;
    (void)relativePath;

    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FileStream::AssignStorage(uint8_t *bufferIn, size_t sizeIn, uint8_t *bufferOut, size_t sizeOut)
{
    (void)bufferIn;
    (void)sizeIn;
    (void)bufferOut;
    (void)sizeOut;

    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FileStream::Read(uint8_t *buffer, int count, int *bytesRead)
{
    (void)buffer;
    (void)count;
    (void)bytesRead;

    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FileStream::Write(uint8_t *buffer, int count, int *bytesWritten)
{
    (void)buffer;
    (void)count;
    (void)bytesWritten;

    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FileStream::Seek(int64_t offset, uint32_t origin, int64_t *position)
{
    (void)offset;
    (void)origin;
    (void)position;

    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FileStream::Close()
{
    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FileStream::Flush()
{
    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FileStream::GetLength(int64_t *length)
{
    (void)length;

    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FileStream::SetLength(int64_t length)
{
    (void)length;

    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

HRESULT CLR_RT_FindFile::CreateInstance(CLR_RT_HeapBlock &ref, const char *path, const char *searchPattern)
{
    (void)ref;
    (void)path;
    (void)searchPattern;
        
    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FindFile::GetNext(FS_FILEINFO **fi, bool *found)
{
    (void)fi;
    (void)found;

    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FindFile::CreateFilenameString(CLR_RT_HeapBlock &ref)
{
    (void)ref;

    NANOCLR_FEATURE_STUB_RETURN();
}

HRESULT CLR_RT_FindFile::Close()
{
    NANOCLR_FEATURE_STUB_RETURN();
}
