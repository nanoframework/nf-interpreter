//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native_target.h"

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiController::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint32_t spiCount = CPU_SPI_PortsCount();

        // Length of string requird to return availble names
        uint32_t spiMaxStringLength = spiCount * 5;
        char *deviceSelectorString = (char *)platform_malloc(spiMaxStringLength + 1);
        deviceSelectorString[0] = 0; // terminate in case no buses

        char busNum[2] = "1";

        uint32_t spiMap = CPU_SPI_PortsMap();
        while (spiMap > 0)
        {
            if (spiMap & 0x01)
            {
                strcat(deviceSelectorString, "SPI");
                strcat(deviceSelectorString, busNum);
                if (spiMap > 1)
                {
                    strcat(deviceSelectorString, ",");
                }
            }
            spiMap >>= 1;
            busNum[0] += 1;
        }

        // because the caller is expecting a result to be returned
        // we need set a return result in the stack argument using the appropriate SetResult according to the variable
        // type (a string here)
        NANOCLR_CHECK_HRESULT(stack.SetResult_String(deviceSelectorString));

        platform_free(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP();
}
