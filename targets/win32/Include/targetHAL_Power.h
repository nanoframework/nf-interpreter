//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef TARGET_HAL_POWER_H
#define TARGET_HAL_POWER_H

inline void CPU_Reset()
{
    ::ExitProcess(0);
};
inline bool CPU_IsSoftRebootSupported()
{
    return true;
};

#endif //TARGET_HAL_POWER_H
