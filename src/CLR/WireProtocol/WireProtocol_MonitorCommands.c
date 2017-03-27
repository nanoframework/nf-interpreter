//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_MonitorCommands.h"

////////////////////////////////////////////////////

// provided as weak to be replaced by actual implementation by application
__nfweak bool Monitor_Ping(WP_Message* message)
{
   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak bool Monitor_OemInfo(WP_Message* message)
{
   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak bool Monitor_WriteMemory(WP_Message* message)
{
   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak bool Monitor_Reboot(WP_Message* message)
{
   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak bool Monitor_EraseMemory(WP_Message* message)
{
   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak bool Monitor_CheckMemory(WP_Message* message)
{
   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak bool Monitor_MemoryMap(WP_Message* message)
{
   // default to false
   return false;
}

// provided as weak to be replaced by actual implementation by application
__nfweak bool Monitor_FlashSectorMap(WP_Message* message)
{
   // default to false
   return false;
}
