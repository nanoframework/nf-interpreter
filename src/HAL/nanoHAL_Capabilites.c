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

// Information on whether updating a configuration block requires previous erase.
// Implemented as "weak" to allow it to be replaced with "hard" implementation at target level.
__nfweak bool Target_ConfigUpdateRequiresErase()
{
    return true;
}

// Information on whether the target has a proprietary bootloader
// Implemented as "weak" to allow it to be replaced with "hard" implementation at target level.
__nfweak bool Target_HasProprietaryBooter()
{
    return false;
}

// Information on whether the target is capable of IFU
// Implemented as "weak" to allow it to be replaced with "hard" implementation at target level.
__nfweak bool Target_IFUCapable()
{
    return false;
}

inline bool Target_HasNanoBooter()
{
#if (TARGET_HAS_NANOBOOTER == TRUE)
    return true;
#else
    return false;
#endif
}
