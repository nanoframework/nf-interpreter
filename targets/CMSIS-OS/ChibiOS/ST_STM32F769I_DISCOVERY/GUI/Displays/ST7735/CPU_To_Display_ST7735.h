
#include "nanoCLR_Types.h"

struct CPU_To_Display_ST7735
{
    static bool InitializeCommunicationsChannel();
    static void SendCommandToDisplay(CLR_UINT8 c);
    static void SendDataToDisplay(CLR_UINT16 value);
    static void SendDataToDisplay(const CLR_UINT16 data[], CLR_UINT16 count);
};
