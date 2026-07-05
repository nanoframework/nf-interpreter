//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <cmsis_os.h>
#include <nanoHAL_v2.h>
#include <hal.h>
#include <hal_nf_community.h>
#include <WireProtocol.h>
#include <Debugger.h>
#include <WireProtocol_MonitorCommands.h>
#include <target_board.h>

// Vendor-abstracted flash operations
#if (HAL_NF_USE_STM32_FLASH == TRUE)
#define nf_TargetFlashWrite stm32FlashWrite
#define nf_TargetFlashErase stm32FlashErase
#else
// Forward declarations for target-provided flash operations
int nf_TargetFlashWrite(uint32_t startAddress, uint32_t length, const uint8_t *buffer);
int nf_TargetFlashErase(uint32_t address);
#endif

int AccessMemory(uint32_t location, uint32_t lengthInBytes, uint8_t *buffer, int32_t mode, uint32_t *errorCode)
{
    // reset error code
    *errorCode = AccessMemoryErrorCode_NoError;

    switch (mode)
    {
        case AccessMemory_Write:
            // use FLASH driver to perform write operation
            return nf_TargetFlashWrite(location, lengthInBytes, buffer);

        case AccessMemory_Erase:
            // erase using FLASH driver
            return nf_TargetFlashErase(location);

        case AccessMemory_Check:
            // compute CRC32 of the memory segment
            *(unsigned int *)buffer = SUPPORT_ComputeCRC((uint32_t *)location, lengthInBytes, 0);
            // done here
            return true;

        case AccessMemory_Read:
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
            // because ChibiOS relies on CMSIS it's recommended to make use of the CMSIS API
            NVIC_SystemReset();
        }
    }

    return true;
}
