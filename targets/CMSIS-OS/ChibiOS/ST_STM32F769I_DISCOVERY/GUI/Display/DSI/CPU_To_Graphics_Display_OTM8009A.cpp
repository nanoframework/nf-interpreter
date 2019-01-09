
#include "nanoCLR_Types.h"
#include "..\..\..\..\src\CLR\GUI\Devices\Includes\Cpu_To_Graphics_Display.h"

bool CPU_To_Display_OTM8009A::InitializeCpuDisplayDriver()
{
#ifdef HasBeenCoded
    // Base address of DSI Host/Wrapper registers to be set before calling De-Init 
    hdsi_discovery.Instance = DSI;

    HAL_DSI_DeInit(&(hdsi_discovery));

    // Configure the DSI PLL 
    dsiPllInit.PLLNDIV = HDMI_PLLConfig[format].NDIV;
    dsiPllInit.PLLIDF = HDMI_PLLConfig[format].IDF;
    dsiPllInit.PLLODF = HDMI_PLLConfig[format].ODF;

    // Set number of Lanes 
    hdsi_discovery.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
    // Set the TX escape clock division ratio 
    hdsi_discovery.Init.TXEscapeCkdiv = HDMI_PLLConfig[format].TXEscapeCkdiv;
    // Disable the automatic clock lane control (the ADV7533 must be clocked) 
    hdsi_discovery.Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;

    // Init the DSI 
    HAL_DSI_Init(&hdsi_discovery, &dsiPllInit);

    // Configure the D-PHY Timings 
    dsiPhyInit.ClockLaneHS2LPTime = 0x14;
    dsiPhyInit.ClockLaneLP2HSTime = 0x14;
    dsiPhyInit.DataLaneHS2LPTime = 0x0A;
    dsiPhyInit.DataLaneLP2HSTime = 0x0A;
    dsiPhyInit.DataLaneMaxReadTime = 0x00;
    dsiPhyInit.StopWaitTime = 0x0;
    HAL_DSI_ConfigPhyTimer(&hdsi_discovery, &dsiPhyInit);

    // Virutal channel used by the ADV7533 hdsivideo_handle.VirtualChannelID = HDMI_ADV7533_ID
    // Timing parameters for Video modes Set Timing parameters of DSI depending on its chosen format
    hdsivideo_handle.ColorCoding = HDMI_Format[format].RGB_CODING;
    hdsivideo_handle.LooselyPacked = DSI_LOOSELY_PACKED_DISABLE;
    hdsivideo_handle.VSPolarity = DSI_VSYNC_ACTIVE_LOW;
    hdsivideo_handle.HSPolarity = DSI_HSYNC_ACTIVE_LOW;
    hdsivideo_handle.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
    hdsivideo_handle.Mode = DSI_VID_MODE_NB_PULSES;
    hdsivideo_handle.NullPacketSize = HDMI_DSIPacket[format].NullPacketSize;
    hdsivideo_handle.NumberOfChunks = HDMI_DSIPacket[format].NumberOfChunks;
    hdsivideo_handle.PacketSize = HDMI_DSIPacket[format].PacketSize;
    hdsivideo_handle.HorizontalSyncActive = HDMI_Format[format].HSYNC*HDMI_PLLConfig[format].LaneByteClock / HDMI_PLLConfig[format].PCLK;
    hdsivideo_handle.HorizontalBackPorch = HDMI_Format[format].HBP*HDMI_PLLConfig[format].LaneByteClock / HDMI_PLLConfig[format].PCLK;
    hdsivideo_handle.HorizontalLine = (HDMI_Format[format].HACT + HDMI_Format[format].HSYNC + HDMI_Format[format].HBP + HDMI_Format[format].HFP)*HDMI_PLLConfig[format].LaneByteClock / HDMI_PLLConfig[format].PCLK;
    hdsivideo_handle.VerticalSyncActive = HDMI_Format[format].VSYNC;
    hdsivideo_handle.VerticalBackPorch = HDMI_Format[format].VBP;
    hdsivideo_handle.VerticalFrontPorch = HDMI_Format[format].VFP;
    hdsivideo_handle.VerticalActive = HDMI_Format[format].VACT;

    // Enable or disable sending LP command while streaming is active in video mode 
    hdsivideo_handle.LPCommandEnable = DSI_LP_COMMAND_DISABLE; // Enable sending commands in mode LP (Low Power)

    // Largest packet size possible to transmit in LP mode in VSA, VBP, VFP regions
    // Only useful when sending LP packets is allowed while streaming is active in video mode
    hdsivideo_handle.LPLargestPacketSize = 4;

    // Largest packet size possible to transmit in LP mode in HFP region during VACT period
    // Only useful when sending LP packets is allowed while streaming is active in video mode
    hdsivideo_handle.LPVACTLargestPacketSize = 4;

    // Specify for each region, if the going in LP mode is allowed
    // while streaming is active in video mode
    hdsivideo_handle.LPHorizontalFrontPorchEnable = DSI_LP_HFP_DISABLE;
    hdsivideo_handle.LPHorizontalBackPorchEnable = DSI_LP_HBP_DISABLE;
    hdsivideo_handle.LPVerticalActiveEnable = DSI_LP_VACT_DISABLE;
    hdsivideo_handle.LPVerticalFrontPorchEnable = DSI_LP_VFP_DISABLE;
    hdsivideo_handle.LPVerticalBackPorchEnable = DSI_LP_VBP_DISABLE;
    hdsivideo_handle.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_DISABLE;

    // No acknoledge at the end of a frame 
    hdsivideo_handle.FrameBTAAcknowledgeEnable = DSI_FBTAA_DISABLE;

    // Configure DSI Video mode timings with settings set above 
    HAL_DSI_ConfigVideoMode(&hdsi_discovery, &hdsivideo_handle);

    // Enable the DSI host and wrapper : but LTDC is not started yet at this stage
    HAL_DSI_Start(&hdsi_discovery);

#endif

}
void CPU_To_Display_OTM8009A::SendCommandToDisplay(CLR_UINT8 c)
{
    //  HAL_DSI_ShortWrite(&hdsi_discovery, LCD_OTM8009A_ID, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]);
}
void CPU_To_Display_OTM8009A::SendCommandToDisplay(CLR_UINT8 command, CLR_UINT8 value)
{
    //  HAL_DSI_ShortWrite(&hdsi_discovery, LCD_OTM8009A_ID, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]);
}
void CPU_To_Display_OTM8009A::SendDataToDisplay(CLR_UINT16 c)
{
    // HAL_DSI_LongWrite(&hdsi_discovery, LCD_OTM8009A_ID, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams);
}
void CPU_To_Display_OTM8009A::SendDataToDisplay(const CLR_UINT16 data[], CLR_UINT16 count)
{
    // HAL_DSI_LongWrite(&hdsi_discovery, LCD_OTM8009A_ID, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams);
}
void CPU_To_Display_OTM8009A::SendDataToDisplay(CLR_UINT8 * pParams, int numberOfParameters)
{
    // HAL_DSI_LongWrite(&hdsi_discovery, LCD_OTM8009A_ID, DSI_DCS_LONG_PKT_WRITE, numberOfParameters, pParams[numberOfParameters], pParams);
}


/* FIXME

void CPU_To_Display_OTM8009A::SetWindowSize(SCREEN_ORIENTATION Orientation, CLR_UINT16 x1_x2, CLR_UINT16 y1, CLR_UINT16 y2)
{

    // Reconfigure the layer size
   // FIXME: HAL_LTDC_SetWindowSize(&(hltdc_discovery), Width, Height, LayerIndex);

    // Reconfigure the layer position
    // FIXME:  HAL_LTDC_SetWindowPosition(&(hltdc_discovery), Xpos, Ypos, LayerIndex);
}
void CPU_To_Display_OTM8009A::Clear()
{
    // Clear the LCD
   //FIXME: LL_FillBuffer(ActiveLayer, (uint32_t *)(hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress), BSP_LCD_GetXSize(), BSP_LCD_GetYSize(), 0, Color);

}
void CPU_To_Display_OTM8009A::DrawPixel(CLR_UINT16 Xpos, CLR_UINT16 Ypos, CLR_UINT32 RGB_Code)
{
    // Write data value to all SDRAM memory
  //  *(__IO uint32_t*) (hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress + (4 * (Ypos*BSP_LCD_GetXSize() + Xpos))) = RGB_Code;
}
// Fills a buffer.
// LayerIndex: Layer index
// pDst: Pointer to destination buffer
// xSize: Buffer width
// ySize: Buffer height
// OffLine: Offset
// ColorIndex: Color index
void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{
    // Register to memory mode with ARGB8888 as color Mode
    hdma2d_discovery.Init.Mode = DMA2D_R2M;
    hdma2d_discovery.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
    hdma2d_discovery.Init.OutputOffset = OffLine;

    hdma2d_discovery.Instance = DMA2D;

    // DMA2D Initialization
    if (HAL_DMA2D_Init(&hdma2d_discovery) == HAL_OK)
    {
        if (HAL_DMA2D_ConfigLayer(&hdma2d_discovery, LayerIndex) == HAL_OK)
        {
            if (HAL_DMA2D_Start(&hdma2d_discovery, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
            {
                // Polling For DMA transfer
                HAL_DMA2D_PollForTransfer(&hdma2d_discovery, 10);
            }
        }
    }
}
*/