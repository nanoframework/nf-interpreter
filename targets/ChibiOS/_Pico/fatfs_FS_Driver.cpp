//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// The shared driver uses a different output-buffer name when there is no data cache.
#define outputBuffer fatfs_outputBuffer
#include "../_FatFs/fatfs_FS_Driver.cpp"
