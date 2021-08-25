//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOHAL_BOOT_H
#define NANOHAL_BOOT_H

#include <nanoCLR_Headers.h>
#include <nanoPackStruct.h>
#include <nanoVersion.h>
#include <target_os.h>

#ifndef TARGET_HAS_NANOBOOTER
#error "Can't find definition for TARGET_HAS_NANOBOOTER. Check the inclusion of 'target_os.h'"
#endif

// magic value to mark the limits of the boot clipboard data area
#define BOOTCLIPBOARD_MAGIC_MARKER 0x4F41A583

typedef enum BootRequest_Options
{
    BootRequest_Invalid = 0,
    BootRequest_NanoBooter = 1,
    BootRequest_ProprietaryBootloader = 2,
} BootRequest_Options;

typedef enum BootExecution_Options
{
    BootExecution_Unknown = 0,
    BootExecution_OK = 1,
    BootRequest_Error = 2,
} BootExecution_Options;

// This struct holds the information to be passed to and from nanoCLR and nanoBooter
// It's placed at a known fixed RAM address so both apps know where to look for it.
typedef struct __nfpack BootClipboard
{
    // to be hold the magic marker value
    uint32_t StartMarker;

    BootRequest_Options BootRequest;
    uint32_t BootParameters;
    BootExecution_Options BootExecution;
    uint32_t ErrorCode;

    VersionInfo BooterVersion;
    VersionInfo CLRVersion;

    // padding to keep the struct aligned
    // and keep space available for future expansion
    uint8_t padding[14];

    // to be hold the magic marker value
    uint32_t EndMarker;
} BootClipboard;

extern BootClipboard g_BootClipboard;

#ifdef __cplusplus
extern "C"
{
#endif

    void InitBootClipboard();

    // Launches the proprietary bootloader of the platform.
    // Returns false in case it's not supported (which is considered the default).
    // In case of a successfull call it won't return.
    // Implemented as "weak" to allow it to be replaced with "hard" implementation at platform level.
    bool LaunchProprietaryBootloader();

    // Returns true if the there is a request to remain in nanoBooter
    bool IsToRemainInBooter();

    // Request to launch nanoBooter
    // Returns false in case it's not supported (which is considered the default).
    bool RequestToLaunchNanoBooter();

    // Request to launch proprietary bootloader
    // Returns false in case it's not supported (which is considered the default).
    bool RequestToLaunchProprietaryBootloader();

    // Report successfull nanoBooter execution
    void ReportSuccessfullNanoBooter();

#ifdef __cplusplus
}
#endif

#endif // NANOHAL_BOOT_H
