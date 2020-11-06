//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_CAPABILITIES_H_
#define _NANOHAL_CAPABILITIES_H_ 1

#include <stdint.h>
#include <stdbool.h>
#include <nanoWeak.h>

// ready to use default implementations to be used at platform level

#define GET_PLATFORM_CAPABILITIES(option)                                                                              \
    uint32_t GetPlatformCapabilities()                                                                                 \
    {                                                                                                                  \
        return option;                                                                                                 \
    }

#define GET_TARGET_CAPABILITIES(option)                                                                                \
    uint32_t GetTargetCapabilities()                                                                                   \
    {                                                                                                                  \
        return option;                                                                                                 \
    }

#define TARGET_CONFIG_UPDATE_REQUIRES_ERASE(option)                                                                    \
    bool Target_ConfigUpdateRequiresErase()                                                                            \
    {                                                                                                                  \
        return option;                                                                                                 \
    }

#define TARGET_HAS_NANOBOOTER(option)                                                                                  \
    bool Target_HasNanoBooter()                                                                                        \
    {                                                                                                                  \
        return option;                                                                                                 \
    }

#define TARGET_HAS_PROPRITARY_BOOTER(option)                                                                           \
    bool Target_HasProprietaryBooter()                                                                                 \
    {                                                                                                                  \
        return option;                                                                                                 \
    }

#define TARGET_IFU_CAPABLE(option)                                                                                     \
    bool Target_IFUCapable()                                                                                           \
    {                                                                                                                  \
        return option;                                                                                                 \
    }

#ifdef __cplusplus
extern "C"
{
#endif

    uint32_t GetPlatformCapabilities();

    // Returns the target capabilities coded in a uint32_t.
    // Needs to be provided at target level.
    // Platform implementation should to be declared as "weak" to allow it to be replaced at target level.
    uint32_t GetTargetCapabilities();

    // Information on whether updating a configuration block requires previous erase.
    // This is relevant for targets that store the configuration block in flash,
    // which requires erasing before changing the content.
    // No default implementation provided to make sure the platform/target esplicitly declares it
    bool Target_ConfigUpdateRequiresErase();

    // Information on whether the target implements nano booter
    // No default implementation provided to make sure the platform/target esplicitly declares it
    bool Target_HasNanoBooter();

    // Information on whether the target has a proprietary bootloader
    // No default implementation provided to make sure the platform/target esplicitly declares it
    bool Target_HasProprietaryBooter();

    // Information on whether the target is capable of IFU
    // No default implementation provided to make sure the platform/target esplicitly declares it
    bool Target_IFUCapable();

#ifdef __cplusplus
}
#endif

#endif // _NANOHAL_CAPABILITIES_H_