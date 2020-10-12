//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_MonitorCommands.h"

////////////////////////////////////////////////////

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_Ping(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_OemInfo(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_ReadMemory(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_WriteMemory(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_Reboot(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_EraseMemory(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_QueryConfiguration(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_UpdateConfiguration(WP_Message* message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_CheckMemory(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_MemoryMap(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak int Monitor_FlashSectorMap(WP_Message* message)
{
   (void)(message);

   // default to false
   return false;
}
