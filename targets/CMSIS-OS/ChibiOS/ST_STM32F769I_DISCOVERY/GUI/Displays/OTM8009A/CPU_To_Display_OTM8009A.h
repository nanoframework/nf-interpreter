
#include "nanoCLR_Types.h"
#include "display_driver.h"

struct CPU_To_Display_OTM8009A
{
    static bool InitializeCommunicationsChannel();
    static void SendCommandToDisplay(CLR_UINT8 value);
    static void SendCommandToDisplay(CLR_UINT8 command, CLR_UINT8 value);
    static void SendDataToDisplay(const CLR_UINT16 data[], CLR_UINT16 count);
    static void SendDataToDisplay(CLR_UINT8 * pParams, int numberOfParameters);
    static void SetWindowSize(SCREEN_ORIENTATION Orientation, CLR_UINT16 x1_x2, CLR_UINT16 y1, CLR_UINT16 y2);
    static void DrawPixel(CLR_UINT16 Xpos, CLR_UINT16 Ypos, CLR_UINT32 RGB_Code);
    static void Clear();
};
void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);


