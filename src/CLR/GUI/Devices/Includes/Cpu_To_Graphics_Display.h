
#ifndef _DRIVERS_CPU_DISPLAY_DRIVER_H_
#define _DRIVERS_CPU_DISPLAY_DRIVER_H_ 1

#include "nanoCLR_Types.h"
//#include "Display_driver.h"

struct CPU_TO_GRAPHICS_DISPLAY
{
    static bool InitializeCpuDisplayDriver();
    static void SendCommandToDisplay(CLR_UINT8 c);
    static void SendCommandToDisplay(CLR_UINT8 command, CLR_UINT8 value);

    static void SendDataToDisplay(CLR_UINT16 value);
    static void SendDataToDisplay(const CLR_UINT16 data[], CLR_UINT16 count);
    static void SendDataToDisplay(CLR_UINT8 * pParams, int numberOfParameters);
};

#endif
