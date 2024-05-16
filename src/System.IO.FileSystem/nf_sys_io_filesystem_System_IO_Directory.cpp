//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::NativeGetChildren___STATIC__SZARRAY_STRING__STRING__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    FileSystemVolume *driver = NULL;
    char *path;
    bool isDirectory;
    char rootNameBuffer[FS_NAME_MAXLENGTH + 1];
    char relativePathBuffer[FS_MAX_PATH_LENGTH + 1];
    char *rootName = rootNameBuffer;
    char *relativePath = relativePathBuffer;
    uint32_t rootNameLength = -1;
    bool found = false;
    bool *foundP = &found;
    void *findHandle = NULL;
    int32_t itemsCount = 0;
    FS_FILEINFO fileData = {0};

    CLR_RT_HeapBlock *pathEntry;
    CLR_RT_HeapBlock_String *hbPath;

    CLR_RT_HeapBlock &top = stack.PushValue();

    hbPath = stack.Arg0().DereferenceString();
    FAULT_ON_NULL(hbPath);

    NANOCLR_CHECK_HRESULT(
        CLR_RT_FileStream::SplitFilePath(hbPath->StringText(), rootName, rootNameLength, relativePath));

    NANOCLR_CHECK_HRESULT(Library_nf_sys_io_filesystem_System_IO_NativeIO::FindVolume(stack.Arg0(), driver, path));

    isDirectory = stack.Arg1().NumericByRef().u1 != 0;

    // 1st pass to get the count of items
    driver->FindOpen(path, findHandle);

    do
    {
        memset(&fileData, 0, sizeof(FS_FILEINFO));

        // find the next entry, until you don't find any more
        driver->FindNext(findHandle, &fileData, foundP);

        // do nothing here, just iterate to count the items
        if (found)
        {
            if (isDirectory && ((fileData.Attributes & FileAttributes::FileAttributes_Directory) ==
                                FileAttributes::FileAttributes_Directory))
            {
                itemsCount++;
            }
            else if (
                !isDirectory && ((fileData.Attributes & FileAttributes::FileAttributes_Directory) !=
                                 FileAttributes::FileAttributes_Directory))
            {
                itemsCount++;
            }
        }

        // free memory for the file name
        if (fileData.FileName != NULL)
        {
            platform_free(fileData.FileName);
        }

    } while (found);

    // close find handle
    driver->FindClose(findHandle);

    // create an array of files paths <String>
    NANOCLR_CHECK_HRESULT(
        CLR_RT_HeapBlock_Array::CreateInstance(top, (CLR_UINT32)itemsCount, g_CLR_RT_WellKnownTypes.m_String));

    // 2nd pass to fill the array
    driver->FindOpen(path, findHandle);

    pathEntry = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

    do
    {
        memset(&fileData, 0, sizeof(FS_FILEINFO));

        // find the next entry, until you don't find any more
        driver->FindNext(findHandle, &fileData, foundP);

        // do nothing here, just iterate to count the items
        if (found)
        {
            if ((isDirectory && ((fileData.Attributes & FileAttributes::FileAttributes_Directory) ==
                                 FileAttributes::FileAttributes_Directory)) ||

                !isDirectory && ((fileData.Attributes & FileAttributes::FileAttributes_Directory) !=
                                 FileAttributes::FileAttributes_Directory))
            {
                // compose file path
                char workingPath[FS_MAX_PATH_LENGTH];
                hal_snprintf(workingPath, rootNameLength + 1, "%s", rootName);
                hal_snprintf(
                    workingPath,
                    FS_MAX_PATH_LENGTH,
                    "%s%s%s",
                    workingPath,
                    path,
                    (const char *)fileData.FileName);

                // set file full path in array of strings
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*pathEntry, workingPath));

                // move the file array pointer to the next item in the array
                pathEntry++;
            }
        }

        // free memory for the file name
        if (fileData.FileName != NULL)
        {
            platform_free(fileData.FileName);
        }

    } while (found);

    NANOCLR_CLEANUP();

    // close find handle
    driver->FindClose(findHandle);

    // free memory for the file name
    if (fileData.FileName != NULL)
    {
        platform_free(fileData.FileName);
    }

    NANOCLR_CLEANUP_END();
}

void Library_nf_sys_io_filesystem_System_IO_Directory::CombinePaths(char *outpath, const char *path1, const char *path2)
{
    strcat(outpath, path1);

    // Add "\" to path if required
    if (outpath[hal_strlen_s(outpath) - 1] != '\\')
    {
        strcat(outpath, "\\");
    }

    strcat(outpath, path2);
}
