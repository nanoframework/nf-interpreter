//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL_FileOperation.h>
#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <ff.h>

void CombinePathAndName(char *outpath, const char *path1, const char *path2)
{
    strcat(outpath, path1);

    // Add "\" to path if required
    if (outpath[hal_strlen_s(outpath) - 1] != '\\')
    {
        strcat(outpath, "\\");
    }
    strcat(outpath, path2);
}

//
//  Converts from windows type path       "c:\folder\folder\file.ext"
//  to linux type path used in ESP32 VFS  "/c/folder/folder/file.exe
//  where /c is the mount point
////////////////////////////////////////////
// MAKE SURE TO FREE THE RETURNED POINTER //
////////////////////////////////////////////
//
char *ConvertToVfsPath(const char *filepath)
{
    char *startPath = NULL;
    char *path = NULL;

    int pathlen = hal_strlen_s(filepath);

    /////////////////////////////////
    // MAKE SURE TO FREE THIS POINTER
    startPath = (char *)platform_malloc(pathlen + 1);

    // sanity check for successfull malloc
    if (startPath == NULL)
    {
        // failed to allocate memory
        return NULL;
    }

    path = startPath;
    hal_strcpy_s(path, pathlen + 1, filepath);

    if (hal_strlen_s(path) >= 2)
    {
        // Map  Drive: -> /C
        char drive = *path;
        *path++ = '/';
        *path++ = drive;

        // Convert '\' to '/'
        while (*path)
        {
            if (*path == '\\')
                *path++ = '/';
            else
                path++;
        }
    }

    return startPath;
}