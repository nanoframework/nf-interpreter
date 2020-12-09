//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Boot.h>
#include <nanoHAL_Capabilites.h>

#if (TARGET_HAS_NANOBOOTER == TRUE)
BootClipboard __attribute__((section(".boot_clipboard"))) g_BootClipboard;
#endif

// Performs check and initialization (if required) of the boot clipboard area
void InitBootClipboard()
{

#if (TARGET_HAS_NANOBOOTER == TRUE)

    BootClipboard *data = &g_BootClipboard;

    // sanity check
    if (data->StartMarker != BOOTCLIPBOARD_MAGIC_MARKER || data->EndMarker != BOOTCLIPBOARD_MAGIC_MARKER)
    {
        // no markers found, consider area invalid or not properly initialized
        memset(data, 0, sizeof(BootClipboard));

        // set markers
        data->StartMarker = BOOTCLIPBOARD_MAGIC_MARKER;
        data->EndMarker = BOOTCLIPBOARD_MAGIC_MARKER;
    }

    // set the version according to the build running
#ifdef I_AM_NANOBOOTER

    // building nanoBooter, set version
    data->BooterVersion.usMajor = VERSION_MAJOR;
    data->BooterVersion.usMinor = VERSION_MINOR;
    data->BooterVersion.usBuild = VERSION_BUILD;
    data->BooterVersion.usRevision = VERSION_REVISION;
#endif

#ifdef I_AM_NANOCLR

    // building nanoBooter, set version
    data->CLRVersion.usMajor = VERSION_MAJOR;
    data->CLRVersion.usMinor = VERSION_MINOR;
    data->CLRVersion.usBuild = VERSION_BUILD;
    data->CLRVersion.usRevision = VERSION_REVISION;
#endif

#endif // TARGET_HAS_NANOBOOTER
}

// Returns true if the there is a request to remain in nanoBooter
inline bool IsToRemainInBooter()
{
#if (TARGET_HAS_NANOBOOTER == TRUE)
    return (g_BootClipboard.BootRequest == BootRequest_NanoBooter);
#else
    return false;
#endif
}

// Request to launch nanoBooter
// Returns false in case it's not supported (which is considered the default).
inline bool RequestToLaunchNanoBooter()
{
#if (TARGET_HAS_NANOBOOTER == TRUE)
    if (Target_HasNanoBooter())
    {
        g_BootClipboard.BootRequest = BootRequest_NanoBooter;
        return true;
    }
#endif

    return false;
}

// Request to launch proprietary bootloader
// Returns false in case it's not supported (which is considered the default).
inline bool RequestToLaunchProprietaryBootloader()
{
#if (TARGET_HAS_NANOBOOTER == TRUE)
    if (Target_HasProprietaryBooter())
    {
        g_BootClipboard.BootRequest = BootRequest_ProprietaryBootloader;
        return true;
    }
#endif

    return false;
}

// Report successfull nanoBooter execution
void ReportSuccessfullNanoBooter()
{
#if (TARGET_HAS_NANOBOOTER == TRUE)
    // clear boot request
    g_BootClipboard.BootRequest = BootRequest_Invalid;
    // set execution code
    g_BootClipboard.BootExecution = BootExecution_OK;
    // clear error
    g_BootClipboard.ErrorCode = 0;
#endif
}

// Calls the proprietary bootloader of the platform.
// Returns false in case it's not supported (which is considered the default).
// In case of a successfull call it won't return.
// Implemented as "weak" to allow it to be replaced with "hard" implementation at platform level.
__nfweak bool LaunchProprietaryBootloader()
{
    return false;
}
