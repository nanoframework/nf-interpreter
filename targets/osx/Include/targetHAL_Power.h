//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_POWER_H
#define TARGET_HAL_POWER_H

#include <cstdlib>

inline void CPU_Reset()
{
    std::exit(0);
}

inline bool CPU_IsSoftRebootSupported()
{
    return true;
}

#endif // TARGET_HAL_POWER_H
