//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <Debugger.h>
#include <WireProtocol_MonitorCommands.h>
#include <Target_BlockStorage_MXCFlashDriver.h>
#include <target_board.h>

#include <mxc_device.h>

int AccessMemory(uint32_t location, uint32_t lengthInBytes, uint8_t *buffer, int32_t mode, uint32_t *errorCode)
{
    // reset error code
    *errorCode = AccessMemoryErrorCode_NoError;

    switch (mode)
    {
        case AccessMemory_Write:
            // use FLASH driver to perform write operation
            // this requires that HAL_USE_MXC_FLASH is set to TRUE on halconf_nf.h
            return MXCFlashDriver_Write(NULL, location, lengthInBytes, buffer, true);

        case AccessMemory_Erase:
            // erase using FLASH driver
            // this requires that HAL_USE_MXC_FLASH is set to TRUE on halconf_nf.h
            return MXCFlashDriver_EraseBlock(NULL, location);

        case AccessMemory_Check:
            // compute CRC32 of the memory segment
            *(unsigned int *)buffer = SUPPORT_ComputeCRC((uint32_t *)location, lengthInBytes, 0);
            // done here
            return true;

        case AccessMemory_Read:
            // use FLASH driver to perform read operation
            // this requires that HAL_USE_MXC_FLASH is set to TRUE on halconf_nf.h
            MXCFlashDriver_Read(NULL, location, lengthInBytes, buffer);
            return true;

        default:
            // default return is FALSE
            return false;
    }
}

////////////////////////////////////////////////////

int Monitor_Reboot(WP_Message *message)
{
    Monitor_Reboot_Command *cmd = (Monitor_Reboot_Command *)message->m_payload;

    WP_ReplyToCommand(message, true, false, NULL, 0);

    if (cmd != NULL)
    {
        if (Monitor_Reboot_c_EnterProprietaryBooter == (cmd->m_flags & Monitor_Reboot_c_EnterProprietaryBooter))
        {
            // request to load proprietary bootloader
            // OK to call directly as this will launch the bootloader only if the target has support for it
            LaunchProprietaryBootloader();
        }
        else
        {
            // RESET CPU to load nanoCLR
            NVIC_SystemReset();
        }
    }

    return true;
}
