//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Capabilites.h>

// Returns the platform capabilities coded as an uint32_t.
// Implemented as "weak" to allow it to be replaced with "hard" implementation at platform level.
__nfweak uint32_t GetPlatformCapabilities()
{
    return 0;
}

// Returns the target capabilities coded as an uint32_t.
// Implemented as "weak" to allow it to be replaced with "hard" implementation at target level.
__nfweak uint32_t GetTargetCapabilities()
{
    return 0;
}
