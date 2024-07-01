//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include <nanoCLR_FileStream.h>

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::_ctor___VOID__STRING__I4(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String *hbPath;
    int32_t bufferSize;

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_RT_HeapBlock *pArgs = &(stack.Arg1());

    hbPath = pArgs[0].DereferenceString();
    FAULT_ON_NULL(hbPath);

    bufferSize = pArgs[1].NumericByRef().s4;

    if (bufferSize < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_CHECK_HRESULT(CLR_RT_FileStream::CreateInstance(pThis[FIELD___fs], hbPath->StringText(), bufferSize));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Read___I4__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(ReadWriteHelper(stack, TRUE));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Write___I4__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(ReadWriteHelper(stack, FALSE));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Seek___I8__I8__U4(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_FileStream *fs;
    int64_t offset;
    uint32_t origin;
    int64_t position = 0;

    CLR_RT_HeapBlock *pArgs = &(stack.Arg1());

    offset = pArgs[0].NumericByRef().s8;
    origin = pArgs[1].NumericByRef().u4;

    NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));

    // Arugment Validation
    if (origin > SEEKORIGIN_END)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_CHECK_HRESULT(fs->Seek(offset, origin, &position));

    stack.SetResult_I8(position);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Flush___VOID(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_FileStream *fs;

    NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));

    NANOCLR_CHECK_HRESULT(fs->Flush());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::GetLength___I8(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_FileStream *fs;
    int64_t length = 0;

    NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));

    NANOCLR_CHECK_HRESULT(fs->GetLength(&length));

    stack.SetResult_I8(length);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::SetLength___VOID__I8(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_FileStream *fs;
    int64_t length;

    CLR_RT_HeapBlock *pArgs = &(stack.Arg1());

    length = pArgs[0].NumericByRef().s8;

    NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));

    if (length < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    NANOCLR_CHECK_HRESULT(fs->SetLength(length));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::
    GetStreamProperties___VOID__BYREF_BOOLEAN__BYREF_BOOLEAN__BYREF_BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_FileStream *fs;
    CLR_RT_HeapBlock *hbCanRead;
    CLR_RT_HeapBlock *hbCanWrite;
    CLR_RT_HeapBlock *hbCanSeek;

    CLR_RT_HeapBlock *pArgs = &(stack.Arg1());

    NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));

    hbCanRead = pArgs[0].Dereference();
    FAULT_ON_NULL(hbCanRead);
    hbCanWrite = pArgs[1].Dereference();
    FAULT_ON_NULL(hbCanWrite);
    hbCanSeek = pArgs[2].Dereference();
    FAULT_ON_NULL(hbCanSeek);

    hbCanRead->SetBoolean(fs->CanRead() != 0);
    hbCanWrite->SetBoolean(fs->CanWrite() != 0);
    hbCanSeek->SetBoolean(fs->CanSeek() != 0);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Close___VOID(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_FileStream *fs;
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    hr = GetFileStream(stack, fs);

    if (FAILED(hr))
    {
        if (hr == CLR_E_OBJECT_DISPOSED)
        {
            hr = S_OK;
        }

        NANOCLR_LEAVE();
    }

    NANOCLR_CHECK_HRESULT(fs->Close());

    NANOCLR_CLEANUP();

    pThis[FIELD___fs].SetObjectReference(NULL);

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::ReadWriteHelper(CLR_RT_StackFrame &stack, bool isRead)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *bufHB = NULL;
    CLR_RT_FileStream *fs;
    CLR_RT_HeapBlock *nativeFileStreamHB = NULL;

    uint8_t *buffer;
    int32_t bufferLength;
    int32_t offset;
    int32_t count;
    CLR_RT_HeapBlock *timeoutHB;
    int64_t *timeoutTicks;
    int32_t bytesProcessed = 0;
    bool fRes;

    bufHB = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(bufHB);
    buffer = bufHB->GetFirstElement();
    bufferLength = (int32_t)bufHB->m_numOfElements;
    offset = stack.Arg2().NumericByRef().s4;
    count = stack.Arg3().NumericByRef().s4;
    timeoutHB = &(stack.Arg4());

    NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));

    // Argument Validation
    if (offset < 0 || count < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    if (bufferLength - offset < count)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (timeoutHB->NumericByRef().s4 == 0)
    {
        // no timeout set, just use default timeout
        CLR_INT64 timeout = (isRead) ? fs->GetReadTimeout() : fs->GetWriteTimeout();
        timeoutHB->SetInteger((CLR_INT64)((timeout != 0) ? timeout : FS_DEFAULT_TIMEOUT));
    }

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(*timeoutHB, timeoutTicks));

    buffer += offset;

    // I/O is synchronous and does not require buffering or pinning
    if (fs->GetBufferingStrategy() == SYNC_IO)
    {
        while (count > 0)
        {
            int processed;

            if (isRead)
            {
                NANOCLR_CHECK_HRESULT(fs->Read(buffer, count, &processed));
            }
            else
            {
                NANOCLR_CHECK_HRESULT(fs->Write(buffer, count, &processed));
            }

            if (processed == 0)
            {
                break;
            }

            bytesProcessed += processed;
            buffer += processed;
            count -= processed;
        }
    }
    else
    {
        // Push "bytesProcessed" onto the eval stack.
        if (stack.m_customState == 1)
        {
            stack.PushValueI4(0);

            switch (fs->GetBufferingStrategy())
            {
                case DIRECT_IO:
                    fs->AssignStorage(buffer, count, NULL, 0);
                    bufHB->Pin();

                    break;

                case SYSTEM_BUFFERED_IO:
                    nativeFileStreamHB = stack.This()[FIELD___fs].Dereference();
                    nativeFileStreamHB->Pin();

                    break;

                // nothing to do for the other cases
                default:
                    break;
            }

            stack.m_customState = 2;
        }

        bytesProcessed = stack.m_evalStack[1].NumericByRef().s4;

        buffer += bytesProcessed;
        count -= bytesProcessed;

        fRes = true;

        while (fRes && count > 0)
        {
            int processed;

            if (isRead)
            {
                NANOCLR_CHECK_HRESULT(fs->Read(buffer, count, &processed));
            }
            else
            {
                NANOCLR_CHECK_HRESULT(fs->Write(buffer, count, &processed));
            }

            if (processed == 0)
            {
                stack.m_evalStack[1].NumericByRef().s4 = bytesProcessed;

                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_IO, fRes));
            }
            else if (processed < 0)
            {
                // we've reached the end of the stream
                break;
            }
            else
            {
                buffer += processed;
                bytesProcessed += processed;
                count -= processed;
            }
        }

        // bytesProcessed
        stack.PopValue();
        // Timeout
        stack.PopValue();
    }

    stack.SetResult_I4(bytesProcessed);

    NANOCLR_CLEANUP();

    // we need to clean up if this is not rescheduled
    if (hr != CLR_E_THREAD_WAITING)
    {
        if (bufHB && bufHB->IsPinned())
        {
            bufHB->Unpin();
        }
        else if (nativeFileStreamHB && nativeFileStreamHB->IsPinned())
        {
            nativeFileStreamHB->Unpin();
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::GetFileStream(
    CLR_RT_StackFrame &stack,
    CLR_RT_FileStream *&fs)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_BinaryBlob *blob;
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    blob = pThis[FIELD___fs].DereferenceBinaryBlob();

    if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    fs = (CLR_RT_FileStream *)blob->GetData();

    NANOCLR_NOCLEANUP();
}
