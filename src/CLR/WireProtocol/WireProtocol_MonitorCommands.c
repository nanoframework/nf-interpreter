//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_MonitorCommands.h"
#include <target_board.h>

////////////////////////////////////////////////////

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_Ping(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_OemInfo(WP_Message *message)
{
    if ((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_OemInfo_Reply cmdReply;

        bool fOK = NanoBooter_GetReleaseInfo(&cmdReply.m_releaseInfo) == true;

        WP_ReplyToCommand(message, fOK, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_ReadMemory(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_WriteMemory(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_Reboot(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_EraseMemory(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_QueryConfiguration(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_UpdateConfiguration(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_CheckMemory(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_MemoryMap(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_FlashSectorMap(WP_Message *message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_TargetInfo(WP_Message *message)
{
    if ((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_TargetInfo_Reply cmdReply;

        bool fOK = nanoBooter_GetTargetInfo(&cmdReply.m_TargetInfo) == true;

        WP_ReplyToCommand(message, fOK, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

//////////////////////////////////////////////////////////////////////
// helper functions

// provided as weak to be replaced by actual implementation by application
__nfweak int nanoBooter_GetTargetInfo(TargetInfo *targetInfo)
{
#if (TARGET_HAS_NANOBOOTER == TRUE)

    BootClipboard *data = &g_BootClipboard;

    targetInfo->BooterVersion.usMajor = data->BooterVersion.usMajor;
    targetInfo->BooterVersion.usMinor = data->BooterVersion.usMinor;
    targetInfo->BooterVersion.usBuild = data->BooterVersion.usBuild;
    targetInfo->BooterVersion.usRevision = data->BooterVersion.usRevision;

    targetInfo->ClrVersion.usMajor = data->CLRVersion.usMajor;
    targetInfo->ClrVersion.usMinor = data->CLRVersion.usMinor;
    targetInfo->ClrVersion.usBuild = data->CLRVersion.usBuild;
    targetInfo->ClrVersion.usRevision = data->CLRVersion.usRevision;

#else

    targetInfo->BooterVersion.usMajor = 0;
    targetInfo->BooterVersion.usMinor = 0;
    targetInfo->BooterVersion.usBuild = 0;
    targetInfo->BooterVersion.usRevision = 0;

    targetInfo->ClrVersion.usMajor = VERSION_MAJOR;
    targetInfo->ClrVersion.usMinor = VERSION_MINOR;
    targetInfo->ClrVersion.usBuild = VERSION_BUILD;
    targetInfo->ClrVersion.usRevision = VERSION_REVISION;

#endif

    memcpy(&targetInfo->InfoString, OEMSYSTEMINFOSTRING, ARRAYSIZE(OEMSYSTEMINFOSTRING));
    memcpy(&targetInfo->TargetName, TARGETNAMESTRING, ARRAYSIZE(TARGETNAMESTRING));
    memcpy(&targetInfo->PlatformName, PLATFORMNAMESTRING, ARRAYSIZE(PLATFORMNAMESTRING));
    memcpy(&targetInfo->PlatformInfoString, TARGETINFOSTRING, ARRAYSIZE(TARGETINFOSTRING));

    return true;
}

__nfweak int NanoBooter_GetReleaseInfo(ReleaseInfo *releaseInfo)
{
    releaseInfo->version.usMajor = VERSION_MAJOR;
    releaseInfo->version.usMinor = VERSION_MINOR;
    releaseInfo->version.usBuild = VERSION_BUILD;
    releaseInfo->version.usRevision = VERSION_REVISION;

    memcpy(&releaseInfo->InfoString, OEMSYSTEMINFOSTRING, ARRAYSIZE(OEMSYSTEMINFOSTRING));
    memcpy(&releaseInfo->TargetName, TARGETNAMESTRING, ARRAYSIZE(TARGETNAMESTRING));
    memcpy(&releaseInfo->PlatformName, PLATFORMNAMESTRING, ARRAYSIZE(PLATFORMNAMESTRING));
    memcpy(&releaseInfo->PlatformInfoString, TARGETINFOSTRING, ARRAYSIZE(TARGETINFOSTRING));

    return true;
}
