//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_PAL_FILE_OPERATION_H
#define TARGET_PAL_FILE_OPERATION_H

#ifdef __cplusplus
extern "C"
{
#endif
    void CombinePathAndName(char *outpath, const char *path1, const char *path2);
    char *ConvertToVfsPath(const char *filepath);
#ifdef __cplusplus
}
#endif

#endif  // TARGET_PAL_FILE_OPERATION_H