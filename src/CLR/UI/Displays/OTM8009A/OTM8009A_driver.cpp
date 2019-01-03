
#include "nanohal.h"
#include "nanoCLR_Types.h"
#include "OTM8009A.h"
#include  "hal.h"

#include  "nanohal.h"

OTM8009A_Driver g_OTM8009A_Driver;


void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
    if (NbrParams <= 1)
    {
        HAL_DSI_ShortWrite(&hdsi_discovery, LCD_OTM8009A_ID, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]);
    }
    else
    {
        HAL_DSI_LongWrite(&hdsi_discovery, LCD_OTM8009A_ID, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams);
    }
}
bool OTM8009A_Driver::SetWindow(CLR_UINT16 x1, CLR_UINT16 y1, CLR_UINT16 x2, CLR_UINT16 y2)
{
    CLR_UINT16 x1_x2;
    CLR_UINT16 Addr1, Addr2;

    switch (g_OTM8009A_Driver.lcd_orientation)
    {
    default:
        // Invalid! Fall through to portrait mode
    case PORTRAIT:
        Addr1 = x1;
        Addr2 = y1;
        x1_x2 = (CLR_UINT16)((x2 << 8) + x1);   // pack X-Values into one word            
        break;
    case PORTRAIT180:
        Addr1 = (CLR_UINT16)(SCREEN_SIZE_SHORTER_SIDE - 1 - x1);
        Addr2 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - y1);
        x1_x2 = (CLR_UINT16)((Addr1 << 8) + (SCREEN_SIZE_SHORTER_SIDE - 1 - x2));    // pack X-Values into one word
        y1 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - y2);
        y2 = Addr2;
        break;
    case LANDSCAPE:
        Addr1 = (CLR_UINT16)(SCREEN_SIZE_SHORTER_SIDE - 1 - y1);
        Addr2 = x1;
        x1_x2 = (CLR_UINT16)((Addr1 << 8) + (SCREEN_SIZE_SHORTER_SIDE - 1 - y2));    // pack X-Values into one word
        y1 = x1;
        y2 = x2;
        break;
    case LANDSCAPE180:
        Addr1 = y1;
        Addr2 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - x1);    // pack X-Values into one word
        x1_x2 = (CLR_UINT16)((y2 << 8) + y1);
        y1 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - x2);
        y2 = Addr2;
        break;
    }

    // SET Windows colstart,rowstart,

    return TRUE;
}
bool OTM8009A_Driver::Initialize()
{
    g_OTM8009A_Driver.m_cursor = 0;

    // Initializes the LCD KoD display part by communication in DSI mode in Video Mode
    // with IC Display Driver OTM8009A (see IC Driver specification for more information).
    uint32_t ColorCoding;
    uint32_t orientation;

    // Enable CMD2 to access vendor specific commands, enter in command 2 mode and set EXTC to enable address shift function (0x00)
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(3, (uint8_t *)lcdRegData1);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2); // Enter ORISE Command 2 =  Shift address to 0x80 */
    DSI_IO_WriteCmd(2, (uint8_t *)lcdRegData2);

    /////////////////////////////////////////////////////////////////////
    /* SD_PCH_CTRL - 0xC480h - 129th parameter - Default 0x00          */
    /* Set SD_PT                                                       */
    /* -> Source output level during porch and non-display area to GND */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData3);
    osDelay(10);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData4);       /* Not documented */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData5);       /* Not documented */
    osDelay(10);
    /////////////////////////////////////////////////////////////////////

    // PWR_CTRL4 - 0xC4B0h - 178th parameter - Default 0xA8
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData6);           // Set gvdd_en_test -> enable GVDD test mode !!!
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData7);

    // PWR_CTRL2 - 0xC590h - 146th parameter - Default 0x79
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData8);          // Set pump 4 vgh voltage from 15.0v down to 13.0v
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData9);           // Set pump 5 vgh voltage from -12.0v downto -9.0v

    // P_DRV_M - 0xC0B4h - 181th parameter - Default 0x00
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData10);          // -> Column inversion 
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData11);

    // VCOMDC - 0xD900h - 1st parameter - Default 0x39h
    // VCOM Voltage settings from -1.0000v downto -1.2625v
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData12);

    // Oscillator adjustment for Idle/Normal mode (LPDT only) set to 65Hz (default is 60Hz)
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData13);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData14);

    // Video mode internal
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData15);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData16);

    // PWR_CTRL2 - 0xC590h - 147h parameter - Default 0x00
    // Set pump 4&5 x6  ONLY VALID when PUMP4_EN_ASDM_HV = "0"
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData17);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData18);

    // PWR_CTRL2 - 0xC590h - 150th parameter - Default 0x33h
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData19);      // Change pump4 clock ratio from 1 line to 1/2 line
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData9);

    /* GVDD/NGVDD settings */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(2, (uint8_t *)lcdRegData5);

    // PWR_CTRL2 - 0xC590h - 149th parameter - Default 0x33h
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData20);      // Rewrite the default value !                          
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData21);

    /* Panel display timing Setting 3 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData22);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData23);

    /* Power control 1 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData24);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData25);

    /* Source driver precharge */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData13);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData26);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData15);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData27);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData28);
    DSI_IO_WriteCmd(2, (uint8_t *)lcdRegData6);

    /* GOAVST */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd(6, (uint8_t *)lcdRegData7);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData29);
    DSI_IO_WriteCmd(14, (uint8_t *)lcdRegData8);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData30);
    DSI_IO_WriteCmd(14, (uint8_t *)lcdRegData9);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData31);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData10);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData32);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData46);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData11);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData33);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData12);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData29);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData13);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData30);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData14);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData31);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData15);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData32);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData16);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData34);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData17);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData35);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData18);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData19);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData33);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData20);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData29);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData21);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData30);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData22);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData31);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData23);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData32);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData24);

    /////////////////////////////////////////////////////////////////////////////
    /* PWR_CTRL1 - 0xc580h - 130th parameter - default 0x00 */
    /* Pump 1 min and max DM                                */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData13);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData47);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData48);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData49);
    /////////////////////////////////////////////////////////////////////////////

    /* Exit CMD2 mode */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(3, (uint8_t *)lcdRegData25);

    /*************************************************************************** */
    /* Standard DCS Initialization TO KEEP CAN BE DONE IN HSDT                   */
    /*************************************************************************** */

    /* NOP - goes back to DCS std command ? */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);

    /* Gamma correction 2.2+ table (HSDT possible) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(16, (uint8_t *)lcdRegData3);

    /* Gamma correction 2.2- table (HSDT possible) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(16, (uint8_t *)lcdRegData4);

    /* Send Sleep Out command to display : no parameter */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData36);

    /* Wait for sleep out exit */
    OTM8009A_IO_Delay(120);

    switch (ColorCoding)
    {
    case OTM8009A_FORMAT_RBG565:
        DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData37);  /* Set Pixel color format to RGB565 */
        break;
    case OTM8009A_FORMAT_RGB888:
        DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData38);  /* Set Pixel color format to RGB888 */
        break;
    default:
        break;
    }

    // Send command to configure display in landscape orientation mode. By default the orientation mode is portrait
    if (orientation == OTM8009A_ORIENTATION_LANDSCAPE)
    {
        DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData39);
        DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData27);
        DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData28);
    }

    // CABC : Content Adaptive Backlight Control section start >> */
    //  Note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData40);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData42);      // defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture]
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData43);      // defaut is 0 (lowest Brightness), 0xFF is highest Brightness
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData41);      // defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on

    // CABC : Content Adaptive Backlight Control section end << */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData44);      // Send Command Display On */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);       //  NOP command */

    // Send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData45);      // DSI host from LTDC incoming pixels in video mode */

    return 0;

    osDelay(50);
    Clear();


    return TRUE;
}
bool OTM8009A_Driver::Uninitialize()
{
    Clear();
    return TRUE;
}
void OTM8009A_Driver::PowerSave(bool On)
{
    return;
}
void OTM8009A_Driver::Clear()
{
    // Clear OTM8009A display

    // example::    SetWindow(0, 0, LCD_HW_WIDTH - 1, LCD_HW_HEIGHT - 1);
    //reset the cursor pos to the begining
    g_OTM8009A_Driver.m_cursor = 0;
}
void OTM8009A_Driver::BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    ASSERT((x >= 0) && ((x + width) <= LCD_SCREEN_WIDTH));
    ASSERT((y >= 0) && ((y + height) <= LCD_SCREEN_HEIGHT));

    CLR_UINT16 * StartOfLine_src = (CLR_UINT16*)&data[0];
    SetWindow(x, y, (x + width - 1), (y + height - 1));

    CLR_UINT16 offset = (y * g_OTM8009A_Config.Width) + x;
    StartOfLine_src += offset;

    while (height--)
    {
        CLR_UINT16 * src;
        int      Xcnt;
        src = StartOfLine_src;
        Xcnt = width;
        while (Xcnt--)
        {
            // SendDataWord(*src++);

        }
        StartOfLine_src += g_OTM8009A_Config.Width;
    }
}
void OTM8009A_Driver::BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta)
{
    _ASSERTE(width == LCD_GetWidth());
    _ASSERTE(height == LCD_GetHeight());
    _ASSERTE(widthInWords == width / PixelsPerWord());

    BitBltEx(0, 0, width, height, data);
}
void OTM8009A_Driver::WriteChar(unsigned char c, int row, int col)
{
    // MUNEEB - Font_Width refers to font 8x8 or 8x15...so limiting it to 8x8.  tinyclr.proj defines that we are using 8x8
    //int width = Font_Width();
    //int height = Font_Height();
    int width = 8;
    int height = 8;

    // convert to LCD pixel coordinates
    row *= height;
    col *= width;

    if (row > (g_OTM8009A_Config.Height - height)) return;
    if (col > (g_OTM8009A_Config.Width - width)) return;

    const CLR_UINT8* font = Font_GetGlyph(c);
    // MUNEEB - SEE NOTE ABOVE
    //CLR_UINT16 data[height*width];
    CLR_UINT16 data[8 * 8];
    int i = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            CLR_UINT16 val = LCD_COLOR_BKG;
            // the font data is mirrored
            if ((font[y] & (1 << (width - x - 1))) != 0) val |= LCD_COLOR_TEXT;
            data[i] = val;
            i++;
        }
    }
    SetWindow(col, row, col + width - 1, row + height - 1);
    for (i = 0; i < width*height; i++)
    {
        //////// SendDataWord(data[i]);
    }
}
void OTM8009A_Driver::WriteFormattedChar(unsigned char c)
{
    if (c < 32)
    {
        switch (c)
        {
        case '\b':                      /* backspace, clear previous char and move cursor back */
            if ((g_OTM8009A_Driver.m_cursor % TextColumns()) > 0)
            {
                g_OTM8009A_Driver.m_cursor--;
                WriteChar(' ', g_OTM8009A_Driver.m_cursor / TextColumns(), g_OTM8009A_Driver.m_cursor % TextColumns());
            }
            break;

        case '\f':                      /* formfeed, clear screen and home cursor */
            //Clear();
            g_OTM8009A_Driver.m_cursor = 0;
            break;

        case '\n':                      /* newline */
            g_OTM8009A_Driver.m_cursor += TextColumns();
            g_OTM8009A_Driver.m_cursor -= (g_OTM8009A_Driver.m_cursor % TextColumns());
            break;

        case '\r':                      /* carriage return */
            g_OTM8009A_Driver.m_cursor -= (g_OTM8009A_Driver.m_cursor % TextColumns());
            break;

        case '\t':                      /* horizontal tab */
            g_OTM8009A_Driver.m_cursor += (Font_TabWidth() - ((g_OTM8009A_Driver.m_cursor % TextColumns()) % Font_TabWidth()));
            // deal with line wrap scenario
            if ((g_OTM8009A_Driver.m_cursor % TextColumns()) < Font_TabWidth())
            {
                // bring the cursor to start of line
                g_OTM8009A_Driver.m_cursor -= (g_OTM8009A_Driver.m_cursor % TextColumns());
            }
            break;

        case '\v':                      /* vertical tab */
            g_OTM8009A_Driver.m_cursor += TextColumns();
            break;

        default:
            DEBUG_TRACE2(TRACE_ALWAYS, "Unrecognized control character in LCD_WriteChar: %2u (0x%02x)\r\n", (unsigned int)c, (unsigned int)c);
            break;
        }
    }
    else
    {
        WriteChar(c, g_OTM8009A_Driver.m_cursor / TextColumns(), g_OTM8009A_Driver.m_cursor % TextColumns());
        g_OTM8009A_Driver.m_cursor++;
    }

    if (g_OTM8009A_Driver.m_cursor >= (TextColumns() * TextRows()))
    {
        g_OTM8009A_Driver.m_cursor = 0;
    }
} //done
CLR_UINT32 OTM8009A_Driver::PixelsPerWord()
{
    return ((8 * sizeof(CLR_UINT32)) / g_OTM8009A_Config.BitsPerPixel);
} //done
CLR_UINT32 OTM8009A_Driver::TextRows()
{
    return (g_OTM8009A_Config.Height / Font_Height());
} //done
CLR_UINT32 OTM8009A_Driver::TextColumns()
{
    return (g_OTM8009A_Config.Width / Font_Width());
} //done
CLR_UINT32 OTM8009A_Driver::WidthInWords()
{
    return ((g_OTM8009A_Config.Width + (PixelsPerWord() - 1)) / PixelsPerWord());
} //done
CLR_UINT32 OTM8009A_Driver::SizeInWords()
{
    return (WidthInWords() * g_OTM8009A_Config.Height);
} //done
CLR_UINT32 OTM8009A_Driver::SizeInBytes()
{
    return (SizeInWords() * sizeof(CLR_UINT32));
} //done



/**
  * @brief  BSP LCD Reset
  *         Hw reset the LCD DSI activating its XRES signal (active low for some time)
  *         and desactivating it later.
  */


void BSP_LCD_Reset(void)
{
    GPIO_InitTypeDef  gpio_init_structure;

    __HAL_RCC_GPIOJ_CLK_ENABLE();

    /* Configure the GPIO on PJ15 */
    gpio_init_structure.Pin = GPIO_PIN_15;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

    /* Activate XRES active low */
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_RESET);

    wait_ms(20); /* wait 20 ms */

    /* Desactivate XRES */
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_SET);

    /* Wait for 10ms after releasing XRES before sending commands */
    wait_ms(10);
}

#define LCD_DSI_ID              0x11
#define LCD_DSI_ADDRESS         TS_I2C_ADDRESS
#define LCD_DSI_ID_REG          0xA8

static DSI_VidCfgTypeDef hdsivideo_handle;
#define ABS(X)                 ((X) > 0 ? (X) : -(X))
#define POLY_X(Z)              ((int32_t)((Points + (Z))->X))
#define POLY_Y(Z)              ((int32_t)((Points + (Z))->Y))


DMA2D_HandleTypeDef hdma2d_discovery;
LTDC_HandleTypeDef  hltdc_discovery;
DSI_HandleTypeDef hdsi_discovery;
uint32_t lcd_x_size = OTM8009A_800X480_WIDTH;
uint32_t lcd_y_size = OTM8009A_800X480_HEIGHT;

static uint32_t  ActiveLayer = LTDC_ACTIVE_LAYER_BACKGROUND;  //Default Active LTDC Layer in which drawing is made is LTDC Layer Background
static LCD_DrawPropTypeDef DrawProp[LTDC_MAX_LAYER_NUMBER];  // Current Drawing Layer properties variable
static void LL_ConvertLineToARGB8888(void * pSrc, void *pDst, uint32_t xSize, uint32_t ColorMode);
static uint16_t LCD_IO_GetID(void);

//  Initializes the DSI LCD.
//       - DSI PLL ititialization
//       - DSI ititialization
//       - LTDC ititialization
//       - OTM8009A LCD Display IC Driver ititialization
//  orientation: LCD orientation, can be LCD_ORIENTATION_PORTRAIT or LCD_ORIENTATION_LANDSCAPE
uint8_t BSP_LCD_InitEx(LCD_OrientationTypeDef orientation)
{
    DSI_PLLInitTypeDef dsiPllInit;
    static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
    uint32_t LcdClock = 27429; /*!< LcdClk = 27429 kHz */
    uint16_t read_id = 0;

    uint32_t laneByteClk_kHz = 0;
    uint32_t                   VSA; /*!< Vertical start active time in units of lines */
    uint32_t                   VBP; /*!< Vertical Back Porch time in units of lines */
    uint32_t                   VFP; /*!< Vertical Front Porch time in units of lines */
    uint32_t                   VACT; /*!< Vertical Active time in units of lines = imageSize Y in pixels to display */
    uint32_t                   HSA; /*!< Horizontal start active time in units of lcdClk */
    uint32_t                   HBP; /*!< Horizontal Back Porch time in units of lcdClk */
    uint32_t                   HFP; /*!< Horizontal Front Porch time in units of lcdClk */
    uint32_t                   HACT; /*!< Horizontal Active time in units of lcdClk = imageSize X in pixels to display */

// Toggle Hardware Reset of the DSI LCD using its XRES signal (active low)
    BSP_LCD_Reset();

    // Call first MSP Initialize only in case of first initialization
    // This will set IP blocks LTDC, DSI and DMA2D
    //      - out of reset
    //      - clocked
    //      - NVIC IRQ related to IP blocks enabled
    // 
    BSP_LCD_MspInit();

    // DSI Initialization
    hdsi_discovery.Instance = DSI;          // Base address of DSI Host/Wrapper registers to be set before calling De-Init
    HAL_DSI_DeInit(&(hdsi_discovery));
    dsiPllInit.PLLNDIV = 100;
    dsiPllInit.PLLIDF = DSI_PLL_IN_DIV5;
    dsiPllInit.PLLODF = DSI_PLL_OUT_DIV1;
    laneByteClk_kHz = 62500;                // 500 MHz / 8 = 62.5 MHz = 62500 kHz
    hdsi_discovery.Init.NumberOfLanes = DSI_TWO_DATA_LANES;     // Set number of Lanes
    hdsi_discovery.Init.TXEscapeCkdiv = laneByteClk_kHz / 15620;        // TXEscapeCkdiv = f(LaneByteClk)/15.62 = 4
    HAL_DSI_Init(&(hdsi_discovery), &(dsiPllInit));
    // Timing parameters for all Video modes, Set Timing parameters of LTDC depending on its chosen orientation

    if (orientation == LCD_ORIENTATION_PORTRAIT)
    {
        lcd_x_size = OTM8009A_480X800_WIDTH;  /* 480 */
        lcd_y_size = OTM8009A_480X800_HEIGHT; /* 800 */
    }
    else
    {
        /* lcd_orientation == LCD_ORIENTATION_LANDSCAPE */
        lcd_x_size = OTM8009A_800X480_WIDTH;  /* 800 */
        lcd_y_size = OTM8009A_800X480_HEIGHT; /* 480 */
    }

    HACT = lcd_x_size;
    VACT = lcd_y_size;

    /* The following values are same for portrait and landscape orientations */
    VSA = OTM8009A_480X800_VSYNC;        /* 12  */
    VBP = OTM8009A_480X800_VBP;          /* 12  */
    VFP = OTM8009A_480X800_VFP;          /* 12  */
    HSA = OTM8009A_480X800_HSYNC;        /* 63  */
    HBP = OTM8009A_480X800_HBP;          /* 120 */
    HFP = OTM8009A_480X800_HFP;          /* 120 */

    hdsivideo_handle.VirtualChannelID = LCD_OTM8009A_ID;
    hdsivideo_handle.ColorCoding = LCD_DSI_PIXEL_DATA_FMT_RBG888;
    hdsivideo_handle.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
    hdsivideo_handle.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
    hdsivideo_handle.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
    hdsivideo_handle.Mode = DSI_VID_MODE_BURST; /* Mode Video burst ie : one LgP per line */
    hdsivideo_handle.NullPacketSize = 0xFFF;
    hdsivideo_handle.NumberOfChunks = 0;
    hdsivideo_handle.PacketSize = HACT; /* Value depending on display orientation choice portrait/landscape */
    hdsivideo_handle.HorizontalSyncActive = (HSA * laneByteClk_kHz) / LcdClock;
    hdsivideo_handle.HorizontalBackPorch = (HBP * laneByteClk_kHz) / LcdClock;
    hdsivideo_handle.HorizontalLine = ((HACT + HSA + HBP + HFP) * laneByteClk_kHz) / LcdClock; /* Value depending on display orientation choice portrait/landscape */
    hdsivideo_handle.VerticalSyncActive = VSA;
    hdsivideo_handle.VerticalBackPorch = VBP;
    hdsivideo_handle.VerticalFrontPorch = VFP;
    hdsivideo_handle.VerticalActive = VACT; /* Value depending on display orientation choice portrait/landscape */

    /* Enable or disable sending LP command while streaming is active in video mode */
    hdsivideo_handle.LPCommandEnable = DSI_LP_COMMAND_ENABLE; /* Enable sending commands in mode LP (Low Power) */

    /* Largest packet size possible to transmit in LP mode in VSA, VBP, VFP regions */
    /* Only useful when sending LP packets is allowed while streaming is active in video mode */
    hdsivideo_handle.LPLargestPacketSize = 16;

    /* Largest packet size possible to transmit in LP mode in HFP region during VACT period */
    /* Only useful when sending LP packets is allowed while streaming is active in video mode */
    hdsivideo_handle.LPVACTLargestPacketSize = 0;

    /* Specify for each region of the video frame, if the transmission of command in LP mode is allowed in this region */
    /* while streaming is active in video mode                                                                         */
    hdsivideo_handle.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;   /* Allow sending LP commands during HFP period */
    hdsivideo_handle.LPHorizontalBackPorchEnable = DSI_LP_HBP_ENABLE;   /* Allow sending LP commands during HBP period */
    hdsivideo_handle.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;  /* Allow sending LP commands during VACT period */
    hdsivideo_handle.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;   /* Allow sending LP commands during VFP period */
    hdsivideo_handle.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;   /* Allow sending LP commands during VBP period */
    hdsivideo_handle.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE; /* Allow sending LP commands during VSync = VSA period */

    /* Configure DSI Video mode timings with settings set above */
    HAL_DSI_ConfigVideoMode(&(hdsi_discovery), &(hdsivideo_handle));

    /*************************End DSI Initialization*******************************/


    /************************LTDC Initialization***********************************/

      /* Timing Configuration */
    hltdc_discovery.Init.HorizontalSync = (HSA - 1);
    hltdc_discovery.Init.AccumulatedHBP = (HSA + HBP - 1);
    hltdc_discovery.Init.AccumulatedActiveW = (lcd_x_size + HSA + HBP - 1);
    hltdc_discovery.Init.TotalWidth = (lcd_x_size + HSA + HBP + HFP - 1);

    /* Initialize the LCD pixel width and pixel height */
    hltdc_discovery.LayerCfg->ImageWidth = lcd_x_size;
    hltdc_discovery.LayerCfg->ImageHeight = lcd_y_size;

    /** LCD clock configuration
      * Note: The following values should not be changed as the PLLSAI is also used
      *      to clock the USB FS
      * PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz
      * PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 384 Mhz
      * PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 384 MHz / 7 = 54.85 MHz
      * LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 54.85 MHz / 2 = 27.429 MHz
      */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 7;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Background value */
    hltdc_discovery.Init.Backcolor.Blue = 0;
    hltdc_discovery.Init.Backcolor.Green = 0;
    hltdc_discovery.Init.Backcolor.Red = 0;
    hltdc_discovery.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hltdc_discovery.Instance = LTDC;

    /* Get LTDC Configuration from DSI Configuration */
    HAL_LTDC_StructInitFromVideoConfig(&(hltdc_discovery), &(hdsivideo_handle));

    /* Initialize the LTDC */
    HAL_LTDC_Init(&hltdc_discovery);

    /* Enable the DSI host and wrapper after the LTDC initialization
       To avoid any synchronization issue, the DSI shall be started after enabling the LTDC */
    HAL_DSI_Start(&hdsi_discovery);

#if !defined(DATA_IN_ExtSDRAM)
    /* Initialize the SDRAM */
    BSP_SDRAM_Init();
#endif /* DATA_IN_ExtSDRAM */

    /* Initialize the font */
    BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

    /************************End LTDC Initialization*******************************/


    /***********************OTM8009A Initialization********************************/

      /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)
      *  depending on configuration set in 'hdsivideo_handle'.
      */
    OTM8009A_Init(OTM8009A_FORMAT_RGB888, orientation);

    /***********************End OTM8009A Initialization****************************/

    return LCD_OK;
}

#if defined(USE_LCD_HDMI)
/**
  * @brief  Initializes the DSI for HDMI monitor.
  * The ititialization is done as below:
  *     - DSI PLL ititialization
  *     - DSI ititialization
  *     - LTDC ititialization
  *     - DSI-HDMI ADV7533 adapter device ititialization
  * @param  format : HDMI format could be HDMI_FORMAT_720_480 or HDMI_FORMAT_720_576
  * @retval LCD state
  */
uint8_t BSP_LCD_HDMIInitEx(uint8_t format)
{
    /************************ADV7533 Initialization********************************/

    /* Initialize the ADV7533 HDMI Bridge
    *  depending on configuration set in 'hdsivideo_handle'.
    */
    adv7533ConfigTypeDef adv7533_config;

    adv7533_config.DSI_LANES = 2;
    adv7533_config.HACT = HDMI_Format[format].HACT;
    adv7533_config.HSYNC = HDMI_Format[format].HSYNC;
    adv7533_config.HBP = HDMI_Format[format].HBP;
    adv7533_config.HFP = HDMI_Format[format].HFP;
    adv7533_config.VACT = HDMI_Format[format].VACT;
    adv7533_config.VSYNC = HDMI_Format[format].VSYNC;
    adv7533_config.VBP = HDMI_Format[format].VBP;
    adv7533_config.VFP = HDMI_Format[format].VFP;

    ADV7533_Init();
    ADV7533_Configure(&adv7533_config);
    ADV7533_PowerOn();

    /************************ Update hdmi_x_size and hdmi_y_size *****************/
    lcd_x_size = HDMI_Format[format].HACT;
    lcd_y_size = HDMI_Format[format].VACT;

    /***********************End ADV7533 Initialization****************************/
    DSI_PLLInitTypeDef dsiPllInit;
    DSI_PHY_TimerTypeDef dsiPhyInit;
    static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

    /* Call first MSP Initialize only in case of first initialization
    * This will set IP blocks LTDC and DSI
    * - out of reset
    * - clocked
    * - NVIC IRQ related to IP blocks enabled
    */
    BSP_LCD_MspInit();

    /*************************DSI Initialization***********************************/

      /* Base address of DSI Host/Wrapper registers to be set before calling De-Init */
    hdsi_discovery.Instance = DSI;

    HAL_DSI_DeInit(&(hdsi_discovery));

    /* Configure the DSI PLL */
    dsiPllInit.PLLNDIV = HDMI_PLLConfig[format].NDIV;
    dsiPllInit.PLLIDF = HDMI_PLLConfig[format].IDF;
    dsiPllInit.PLLODF = HDMI_PLLConfig[format].ODF;

    /* Set number of Lanes */
    hdsi_discovery.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
    /* Set the TX escape clock division ratio */
    hdsi_discovery.Init.TXEscapeCkdiv = HDMI_PLLConfig[format].TXEscapeCkdiv;
    /* Disable the automatic clock lane control (the ADV7533 must be clocked) */
    hdsi_discovery.Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;

    /* Init the DSI */
    HAL_DSI_Init(&hdsi_discovery, &dsiPllInit);

    /* Configure the D-PHY Timings */
    dsiPhyInit.ClockLaneHS2LPTime = 0x14;
    dsiPhyInit.ClockLaneLP2HSTime = 0x14;
    dsiPhyInit.DataLaneHS2LPTime = 0x0A;
    dsiPhyInit.DataLaneLP2HSTime = 0x0A;
    dsiPhyInit.DataLaneMaxReadTime = 0x00;
    dsiPhyInit.StopWaitTime = 0x0;
    HAL_DSI_ConfigPhyTimer(&hdsi_discovery, &dsiPhyInit);

    /* Virutal channel used by the ADV7533 */
    hdsivideo_handle.VirtualChannelID = HDMI_ADV7533_ID;

    /* Timing parameters for Video modes
       Set Timing parameters of DSI depending on its chosen format */
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

    /* Enable or disable sending LP command while streaming is active in video mode */
    hdsivideo_handle.LPCommandEnable = DSI_LP_COMMAND_DISABLE; /* Enable sending commands in mode LP (Low Power) */

    /* Largest packet size possible to transmit in LP mode in VSA, VBP, VFP regions */
    /* Only useful when sending LP packets is allowed while streaming is active in video mode */
    hdsivideo_handle.LPLargestPacketSize = 4;

    /* Largest packet size possible to transmit in LP mode in HFP region during VACT period */
    /* Only useful when sending LP packets is allowed while streaming is active in video mode */
    hdsivideo_handle.LPVACTLargestPacketSize = 4;

    /* Specify for each region, if the going in LP mode is allowed */
    /* while streaming is active in video mode                     */
    hdsivideo_handle.LPHorizontalFrontPorchEnable = DSI_LP_HFP_DISABLE;
    hdsivideo_handle.LPHorizontalBackPorchEnable = DSI_LP_HBP_DISABLE;
    hdsivideo_handle.LPVerticalActiveEnable = DSI_LP_VACT_DISABLE;
    hdsivideo_handle.LPVerticalFrontPorchEnable = DSI_LP_VFP_DISABLE;
    hdsivideo_handle.LPVerticalBackPorchEnable = DSI_LP_VBP_DISABLE;
    hdsivideo_handle.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_DISABLE;

    /* No acknoledge at the end of a frame */
    hdsivideo_handle.FrameBTAAcknowledgeEnable = DSI_FBTAA_DISABLE;

    /* Configure DSI Video mode timings with settings set above */
    HAL_DSI_ConfigVideoMode(&hdsi_discovery, &hdsivideo_handle);

    /* Enable the DSI host and wrapper : but LTDC is not started yet at this stage */
    HAL_DSI_Start(&hdsi_discovery);

    /*************************End DSI Initialization*******************************/


    /************************LTDC Initialization***********************************/

      /* LTDC clock configuration */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = HDMI_PLLConfig[format].PLLSAIN;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = HDMI_PLLConfig[format].PLLSAIR;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Base address of LTDC registers to be set before calling De-Init */
    hltdc_discovery.Instance = LTDC;

    HAL_LTDC_DeInit(&(hltdc_discovery));

    /* Timing Configuration */
    hltdc_discovery.Init.HorizontalSync = (HDMI_Format[format].HSYNC - 1);
    hltdc_discovery.Init.AccumulatedHBP = (HDMI_Format[format].HSYNC + HDMI_Format[format].HBP - 1);
    hltdc_discovery.Init.AccumulatedActiveW = (HDMI_Format[format].HACT + HDMI_Format[format].HSYNC + HDMI_Format[format].HBP - 1);
    hltdc_discovery.Init.TotalWidth = (HDMI_Format[format].HACT + HDMI_Format[format].HSYNC + HDMI_Format[format].HBP + HDMI_Format[format].HFP - 1);
    hltdc_discovery.Init.VerticalSync = (HDMI_Format[format].VSYNC - 1);
    hltdc_discovery.Init.AccumulatedVBP = (HDMI_Format[format].VSYNC + HDMI_Format[format].VBP - 1);
    hltdc_discovery.Init.AccumulatedActiveH = (HDMI_Format[format].VACT + HDMI_Format[format].VSYNC + HDMI_Format[format].VBP - 1);
    hltdc_discovery.Init.TotalHeigh = (HDMI_Format[format].VACT + HDMI_Format[format].VSYNC + HDMI_Format[format].VBP + HDMI_Format[format].VFP - 1);

    /* background value */
    hltdc_discovery.Init.Backcolor.Blue = 0x00;
    hltdc_discovery.Init.Backcolor.Green = 0xFF;
    hltdc_discovery.Init.Backcolor.Red = 0xFF;

    /* Polarity */
    hltdc_discovery.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hltdc_discovery.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hltdc_discovery.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hltdc_discovery.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

    /* Initialize & Start the LTDC */
    HAL_LTDC_Init(&hltdc_discovery);

#if !defined(DATA_IN_ExtSDRAM)
    /* Initialize the SDRAM */
    BSP_SDRAM_Init();
#endif /* DATA_IN_ExtSDRAM */

    /* Initialize the font */
    BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
    /************************End LTDC Initialization*******************************/

    return LCD_OK;
}
#endif /* USE_LCD_HDMI */


/**
  * @brief  Gets the LCD X size.
  * @retval Used LCD X size
  */
uint32_t BSP_LCD_GetXSize(void)
{
    return (lcd_x_size);
}

/**
  * @brief  Gets the LCD Y size.
  * @retval Used LCD Y size
  */
uint32_t BSP_LCD_GetYSize(void)
{
    return (lcd_y_size);
}

/**
  * @brief  Set the LCD X size.
  * @param  imageWidthPixels : uint32_t image width in pixels unit
  * @retval None
  */
void BSP_LCD_SetXSize(uint32_t imageWidthPixels)
{
    hltdc_discovery.LayerCfg[ActiveLayer].ImageWidth = imageWidthPixels;
}

/**
  * @brief  Set the LCD Y size.
  * @param  imageHeightPixels : uint32_t image height in lines unit
  */
void BSP_LCD_SetYSize(uint32_t imageHeightPixels)
{
    hltdc_discovery.LayerCfg[ActiveLayer].ImageHeight = imageHeightPixels;
}


/**
  * @brief  Initializes the LCD layers.
  * @param  LayerIndex: Layer foreground or background
  * @param  FB_Address: Layer frame buffer
  * @retval None
  */
void BSP_LCD_LayerDefaultInit(uint16_t LayerIndex, uint32_t FB_Address)
{
    LCD_LayerCfgTypeDef  Layercfg;

    /* Layer Init */
    Layercfg.WindowX0 = 0;
    Layercfg.WindowX1 = BSP_LCD_GetXSize();
    Layercfg.WindowY0 = 0;
    Layercfg.WindowY1 = BSP_LCD_GetYSize();
    Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    Layercfg.FBStartAdress = FB_Address;
    Layercfg.Alpha = 255;
    Layercfg.Alpha0 = 0;
    Layercfg.Backcolor.Blue = 0;
    Layercfg.Backcolor.Green = 0;
    Layercfg.Backcolor.Red = 0;
    Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    Layercfg.ImageWidth = BSP_LCD_GetXSize();
    Layercfg.ImageHeight = BSP_LCD_GetYSize();

    HAL_LTDC_ConfigLayer(&hltdc_discovery, &Layercfg, LayerIndex);

    DrawProp[LayerIndex].BackColor = LCD_COLOR_WHITE;
    DrawProp[LayerIndex].pFont = &Font24;
    DrawProp[LayerIndex].TextColor = LCD_COLOR_BLACK;
}


/**
  * @brief  Selects the LCD Layer.
  * @param  LayerIndex: Layer foreground or background
  */
void BSP_LCD_SelectLayer(uint32_t LayerIndex)
{
    ActiveLayer = LayerIndex;
}

/**
  * @brief  Sets an LCD Layer visible
  * @param  LayerIndex: Visible Layer
  * @param  State: New state of the specified layer
  *          This parameter can be one of the following values:
  *            @arg  ENABLE
  *            @arg  DISABLE
  */
void BSP_LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State)
{
    if (State == ENABLE)
    {
        __HAL_LTDC_LAYER_ENABLE(&(hltdc_discovery), LayerIndex);
    }
    else
    {
        __HAL_LTDC_LAYER_DISABLE(&(hltdc_discovery), LayerIndex);
    }
    __HAL_LTDC_RELOAD_CONFIG(&(hltdc_discovery));

}

/**
  * @brief  Configures the transparency.
  * @param  LayerIndex: Layer foreground or background.
  * @param  Transparency: Transparency
  *           This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF
  */
void BSP_LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency)
{

    HAL_LTDC_SetAlpha(&(hltdc_discovery), Transparency, LayerIndex);

}

/**
  * @brief  Sets an LCD layer frame buffer address.
  * @param  LayerIndex: Layer foreground or background
  * @param  Address: New LCD frame buffer value
  */
void BSP_LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address)
{

    HAL_LTDC_SetAddress(&(hltdc_discovery), Address, LayerIndex);

}

/**
  * @brief  Sets display window.
  * @param  LayerIndex: Layer index
  * @param  Xpos: LCD X position
  * @param  Ypos: LCD Y position
  * @param  Width: LCD window width
  * @param  Height: LCD window height
  */
void BSP_LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    /* Reconfigure the layer size */
    HAL_LTDC_SetWindowSize(&(hltdc_discovery), Width, Height, LayerIndex);

    /* Reconfigure the layer position */
    HAL_LTDC_SetWindowPosition(&(hltdc_discovery), Xpos, Ypos, LayerIndex);

}

/**
  * @brief  Configures and sets the color keying.
  * @param  LayerIndex: Layer foreground or background
  * @param  RGBValue: Color reference
  */
void BSP_LCD_SetColorKeying(uint32_t LayerIndex, uint32_t RGBValue)
{
    /* Configure and Enable the color Keying for LCD Layer */
    HAL_LTDC_ConfigColorKeying(&(hltdc_discovery), RGBValue, LayerIndex);
    HAL_LTDC_EnableColorKeying(&(hltdc_discovery), LayerIndex);
}

/**
  * @brief  Disables the color keying.
  * @param  LayerIndex: Layer foreground or background
  */
void BSP_LCD_ResetColorKeying(uint32_t LayerIndex)
{
    /* Disable the color Keying for LCD Layer */
    HAL_LTDC_DisableColorKeying(&(hltdc_discovery), LayerIndex);
}

/**
  * @brief  Sets the LCD text color.
  * @param  Color: Text color code ARGB(8-8-8-8)
  */
void BSP_LCD_SetTextColor(uint32_t Color)
{
    DrawProp[ActiveLayer].TextColor = Color;
}

/**
  * @brief  Gets the LCD text color.
  * @retval Used text color.
  */
uint32_t BSP_LCD_GetTextColor(void)
{
    return DrawProp[ActiveLayer].TextColor;
}

/**
  * @brief  Sets the LCD background color.
  * @param  Color: Layer background color code ARGB(8-8-8-8)
  */
void BSP_LCD_SetBackColor(uint32_t Color)
{
    DrawProp[ActiveLayer].BackColor = Color;
}

/**
  * @brief  Gets the LCD background color.
  * @retval Used background color
  */
uint32_t BSP_LCD_GetBackColor(void)
{
    return DrawProp[ActiveLayer].BackColor;
}

/**
  * @brief  Sets the LCD text font.
  * @param  fonts: Layer font to be used
  */
void BSP_LCD_SetFont(sFONT *fonts)
{
    DrawProp[ActiveLayer].pFont = fonts;
}

/**
  * @brief  Gets the LCD text font.
  * @retval Used layer font
  */
sFONT *BSP_LCD_GetFont(void)
{
    return DrawProp[ActiveLayer].pFont;
}

/**
  * @brief  Reads an LCD pixel.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @retval RGB pixel color
  */
uint32_t BSP_LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
    uint32_t ret = 0;

    if (hltdc_discovery.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
    {
        /* Read data value from SDRAM memory */
        ret = *(__IO uint32_t*) (hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress + (4 * (Ypos*BSP_LCD_GetXSize() + Xpos)));
    }
    else if (hltdc_discovery.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
    {
        /* Read data value from SDRAM memory */
        ret = (*(__IO uint32_t*) (hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress + (4 * (Ypos*BSP_LCD_GetXSize() + Xpos))) & 0x00FFFFFF);
    }
    else if ((hltdc_discovery.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
        (hltdc_discovery.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
        (hltdc_discovery.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))
    {
        /* Read data value from SDRAM memory */
        ret = *(__IO uint16_t*) (hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos*BSP_LCD_GetXSize() + Xpos)));
    }
    else
    {
        /* Read data value from SDRAM memory */
        ret = *(__IO uint8_t*) (hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos*BSP_LCD_GetXSize() + Xpos)));
    }

    return ret;
}

/**
  * @brief  Clears the whole currently active layer of LTDC.
  * @param  Color: Color of the background
  */
void BSP_LCD_Clear(uint32_t Color)
{
    /* Clear the LCD */
    LL_FillBuffer(ActiveLayer, (uint32_t *)(hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress), BSP_LCD_GetXSize(), BSP_LCD_GetYSize(), 0, Color);
}

/**
  * @brief  Clears the selected line in currently active layer.
  * @param  Line: Line to be cleared
  */
void BSP_LCD_ClearStringLine(uint32_t Line)
{
    uint32_t color_backup = DrawProp[ActiveLayer].TextColor;
    DrawProp[ActiveLayer].TextColor = DrawProp[ActiveLayer].BackColor;

    /* Draw rectangle with background color */
    BSP_LCD_FillRect(0, (Line * DrawProp[ActiveLayer].pFont->Height), BSP_LCD_GetXSize(), DrawProp[ActiveLayer].pFont->Height);

    DrawProp[ActiveLayer].TextColor = color_backup;
    BSP_LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  Displays one character in currently active layer.
  * @param  Xpos: Start column address
  * @param  Ypos: Line where to display the character shape.
  * @param  Ascii: Character ascii code
  *           This parameter must be a number between Min_Data = 0x20 and Max_Data = 0x7E
  */
void BSP_LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
    DrawChar(Xpos, Ypos, &DrawProp[ActiveLayer].pFont->table[(Ascii - ' ') *\
        DrawProp[ActiveLayer].pFont->Height * ((DrawProp[ActiveLayer].pFont->Width + 7) / 8)]);
}

/**
  * @brief  Displays characters in currently active layer.
  * @param  Xpos: X position (in pixel)
  * @param  Ypos: Y position (in pixel)
  * @param  Text: Pointer to string to display on LCD
  * @param  Mode: Display mode
  *          This parameter can be one of the following values:
  *            @arg  CENTER_MODE
  *            @arg  RIGHT_MODE
  *            @arg  LEFT_MODE
  */
void BSP_LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode)
{
    uint16_t refcolumn = 1, i = 0;
    uint32_t size = 0, xsize = 0;
    uint8_t  *ptr = Text;

    /* Get the text size */
    while (*ptr++) size++;

    /* Characters number per line */
    xsize = (BSP_LCD_GetXSize() / DrawProp[ActiveLayer].pFont->Width);

    switch (Mode)
    {
    case CENTER_MODE:
    {
        refcolumn = Xpos + ((xsize - size)* DrawProp[ActiveLayer].pFont->Width) / 2;
        break;
    }
    case LEFT_MODE:
    {
        refcolumn = Xpos;
        break;
    }
    case RIGHT_MODE:
    {
        refcolumn = -Xpos + ((xsize - size)*DrawProp[ActiveLayer].pFont->Width);
        break;
    }
    default:
    {
        refcolumn = Xpos;
        break;
    }
    }

    /* Check that the Start column is located in the screen */
    if ((refcolumn < 1) || (refcolumn >= 0x8000))
    {
        refcolumn = 1;
    }

    /* Send the string character by character on LCD */
    while ((*Text != 0) & (((BSP_LCD_GetXSize() - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF) >= DrawProp[ActiveLayer].pFont->Width))
    {
        /* Display one character on LCD */
        BSP_LCD_DisplayChar(refcolumn, Ypos, *Text);
        /* Decrement the column position by 16 */
        refcolumn += DrawProp[ActiveLayer].pFont->Width;

        /* Point on the next character */
        Text++;
        i++;
    }

}

/**
  * @brief  Displays a maximum of 60 characters on the LCD.
  * @param  Line: Line where to display the character shape
  * @param  ptr: Pointer to string to display on LCD
  */
void BSP_LCD_DisplayStringAtLine(uint16_t Line, uint8_t *ptr)
{
    BSP_LCD_DisplayStringAt(0, LINE(Line), ptr, LEFT_MODE);
}

/**
  * @brief  Draws an horizontal line in currently active layer.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Length: Line length
  */
void BSP_LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
    uint32_t  Xaddress = 0;

    /* Get the line address */
    Xaddress = (hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress) + 4 * (BSP_LCD_GetXSize()*Ypos + Xpos);

    /* Write line */
    LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, Length, 1, 0, DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  Draws a vertical line in currently active layer.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Length: Line length
  */
void BSP_LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
    uint32_t  Xaddress = 0;

    /* Get the line address */
    Xaddress = (hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress) + 4 * (BSP_LCD_GetXSize()*Ypos + Xpos);

    /* Write line */
    LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, 1, Length, (BSP_LCD_GetXSize() - 1), DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  Draws an uni-line (between two points) in currently active layer.
  * @param  x1: Point 1 X position
  * @param  y1: Point 1 Y position
  * @param  x2: Point 2 X position
  * @param  y2: Point 2 Y position
  */
void BSP_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
        yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
        curpixel = 0;

    deltax = ABS(x2 - x1);        /* The difference between the x's */
    deltay = ABS(y2 - y1);        /* The difference between the y's */
    x = x1;                       /* Start x off at the first pixel */
    y = y1;                       /* Start y off at the first pixel */

    if (x2 >= x1)                 /* The x-values are increasing */
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else                          /* The x-values are decreasing */
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1)                 /* The y-values are increasing */
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else                          /* The y-values are decreasing */
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay)         /* There is at least one x-value for every y-value */
    {
        xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
        yinc2 = 0;                  /* Don't change the y for every iteration */
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;         /* There are more x-values than y-values */
    }
    else                          /* There is at least one y-value for every x-value */
    {
        xinc2 = 0;                  /* Don't change the x for every iteration */
        yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;         /* There are more y-values than x-values */
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        BSP_LCD_DrawPixel(x, y, DrawProp[ActiveLayer].TextColor);   /* Draw the current pixel */
        num += numadd;                            /* Increase the numerator by the top of the fraction */
        if (num >= den)                           /* Check if numerator >= denominator */
        {
            num -= den;                             /* Calculate the new numerator value */
            x += xinc1;                             /* Change the x as appropriate */
            y += yinc1;                             /* Change the y as appropriate */
        }
        x += xinc2;                               /* Change the x as appropriate */
        y += yinc2;                               /* Change the y as appropriate */
    }
}

/**
  * @brief  Draws a rectangle in currently active layer.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Width: Rectangle width
  * @param  Height: Rectangle height
  */
void BSP_LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    /* Draw horizontal lines */
    BSP_LCD_DrawHLine(Xpos, Ypos, Width);
    BSP_LCD_DrawHLine(Xpos, (Ypos + Height), Width);

    /* Draw vertical lines */
    BSP_LCD_DrawVLine(Xpos, Ypos, Height);
    BSP_LCD_DrawVLine((Xpos + Width), Ypos, Height);
}

/**
  * @brief  Draws a circle in currently active layer.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  Radius: Circle radius
  */
void BSP_LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int32_t   D;    /* Decision Variable */
    uint32_t  CurX; /* Current X Value */
    uint32_t  CurY; /* Current Y Value */

    D = 3 - (Radius << 1);
    CurX = 0;
    CurY = Radius;

    while (CurX <= CurY)
    {
        BSP_LCD_DrawPixel((Xpos + CurX), (Ypos - CurY), DrawProp[ActiveLayer].TextColor);

        BSP_LCD_DrawPixel((Xpos - CurX), (Ypos - CurY), DrawProp[ActiveLayer].TextColor);

        BSP_LCD_DrawPixel((Xpos + CurY), (Ypos - CurX), DrawProp[ActiveLayer].TextColor);

        BSP_LCD_DrawPixel((Xpos - CurY), (Ypos - CurX), DrawProp[ActiveLayer].TextColor);

        BSP_LCD_DrawPixel((Xpos + CurX), (Ypos + CurY), DrawProp[ActiveLayer].TextColor);

        BSP_LCD_DrawPixel((Xpos - CurX), (Ypos + CurY), DrawProp[ActiveLayer].TextColor);

        BSP_LCD_DrawPixel((Xpos + CurY), (Ypos + CurX), DrawProp[ActiveLayer].TextColor);

        BSP_LCD_DrawPixel((Xpos - CurY), (Ypos + CurX), DrawProp[ActiveLayer].TextColor);

        if (D < 0)
        {
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }
}


/**
  * @brief  Draws a bitmap picture loaded in the internal Flash (32 bpp) in currently active layer.
  * @param  Xpos: Bmp X position in the LCD
  * @param  Ypos: Bmp Y position in the LCD
  * @param  pbmp: Pointer to Bmp picture address in the internal Flash
  */
void BSP_LCD_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp)
{
    uint32_t index = 0, width = 0, height = 0, bit_pixel = 0;
    uint32_t Address;
    uint32_t InputColorMode = 0;

    /* Get bitmap data address offset */
    index = *(__IO uint16_t *) (pbmp + 10);
    index |= (*(__IO uint16_t *) (pbmp + 12)) << 16;

    /* Read bitmap width */
    width = *(uint16_t *)(pbmp + 18);
    width |= (*(uint16_t *)(pbmp + 20)) << 16;

    /* Read bitmap height */
    height = *(uint16_t *)(pbmp + 22);
    height |= (*(uint16_t *)(pbmp + 24)) << 16;

    /* Read bit/pixel */
    bit_pixel = *(uint16_t *)(pbmp + 28);

    /* Set the address */
    Address = hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress + (((BSP_LCD_GetXSize()*Ypos) + Xpos)*(4));

    /* Get the layer pixel format */
    if ((bit_pixel / 8) == 4)
    {
        InputColorMode = DMA2D_INPUT_ARGB8888;
    }
    else if ((bit_pixel / 8) == 2)
    {
        InputColorMode = DMA2D_INPUT_RGB565;
    }
    else
    {
        InputColorMode = DMA2D_INPUT_RGB888;
    }

    /* Bypass the bitmap header */
    pbmp += (index + (width * (height - 1) * (bit_pixel / 8)));

    /* Convert picture to ARGB8888 pixel format */
    for (index = 0; index < height; index++)
    {
        /* Pixel format conversion */
        LL_ConvertLineToARGB8888((uint32_t *)pbmp, (uint32_t *)Address, width, InputColorMode);

        /* Increment the source and destination buffers */
        Address += (BSP_LCD_GetXSize() * 4);
        pbmp -= width * (bit_pixel / 8);
    }
}

/**
  * @brief  Switch back on the display if was switched off by previous call of BSP_LCD_DisplayOff().
  *         Exit DSI ULPM mode if was allowed and configured in Dsi Configuration.
  */
void BSP_LCD_DisplayOn(void)
{
    HAL_DSI_ShortWrite(&(hdsi_discovery), hdsivideo_handle.VirtualChannelID, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_DISPON, 0x00);
}

/**
  * @brief  Switch Off the display.
  *         Enter DSI ULPM mode if was allowed and configured in Dsi Configuration.
  */
void BSP_LCD_DisplayOff(void)
{
    HAL_DSI_ShortWrite(&(hdsi_discovery), hdsivideo_handle.VirtualChannelID, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_DISPOFF, 0x00);
}

/**
  * @brief  Set the brightness value
  * @param  BrightnessValue: [00: Min (black), 100 Max]
  */
void BSP_LCD_SetBrightness(uint8_t BrightnessValue)
{
    HAL_DSI_ShortWrite(&hdsi_discovery, LCD_OTM8009A_ID, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_WRDISBV, (uint16_t)(BrightnessValue * 255) / 100);
}

/**
  * @brief  DCS or Generic short/long write command
  * @param  NbrParams: Number of parameters. It indicates the write command mode:
  *                 If inferior to 2, a long write command is performed else short.
  * @param  pParams: Pointer to parameter values table.
  * @retval HAL status
  */

  // Returns the ID of LCD DSI
static uint16_t LCD_IO_GetID(void)
{
    return LCD_DSI_ID;
}

// LTDC, DMA2D and DSI BSP Routines, De-Initializes the BSP LCD Msp
void BSP_LCD_MspDeInit(void)
{
    HAL_NVIC_DisableIRQ(LTDC_IRQn);             // Disable IRQ of LTDC IP */
    HAL_NVIC_DisableIRQ(DMA2D_IRQn);            // Disable IRQ of DMA2D IP */
    HAL_NVIC_DisableIRQ(DSI_IRQn);               // Disable IRQ of DSI IP */
    __HAL_RCC_LTDC_FORCE_RESET();               // Force and let in reset state LTDC, DMA2D and DSI Host + Wrapper IPs */
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_LTDC_CLK_DISABLE();               // Disable the LTDC, DMA2D and DSI Host and Wrapper clocks */
    __HAL_RCC_DMA2D_CLK_DISABLE();
    __HAL_RCC_DSI_CLK_DISABLE();
}

// Initialize the BSP LCD Msp.
void BSP_LCD_MspInit(void)
{
    __HAL_RCC_LTDC_CLK_ENABLE();            // Enable the LTDC clock */
    __HAL_RCC_LTDC_FORCE_RESET();           // Toggle Sw reset of LTDC IP */
    __HAL_RCC_LTDC_RELEASE_RESET();
    __HAL_RCC_DMA2D_CLK_ENABLE();           // Enable the DMA2D clock */
    __HAL_RCC_DMA2D_FORCE_RESET();          // Toggle Sw reset of DMA2D IP */
    __HAL_RCC_DMA2D_RELEASE_RESET();
    __HAL_RCC_DSI_CLK_ENABLE();             // Enable DSI Host and wrapper clocks */
    __HAL_RCC_DSI_FORCE_RESET();            // Soft Reset the DSI Host and wrapper */
    __HAL_RCC_DSI_RELEASE_RESET();
    HAL_NVIC_SetPriority(LTDC_IRQn, 3, 0);  // NVIC configuration for LTDC interrupt that is now enabled */
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
    HAL_NVIC_SetPriority(DMA2D_IRQn, 3, 0); // NVIC configuration for DMA2D interrupt that is now enabled */
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);
    HAL_NVIC_SetPriority(DSI_IRQn, 3, 0);   // NVIC configuration for DSI interrupt that is now enabled */
    HAL_NVIC_EnableIRQ(DSI_IRQn);
}

// Draws a pixel on LCD ,Pixel color in ARGB mode (8-8-8-8)
*/
void BSP_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{
    // Write data value to all SDRAM memory
    *(__IO uint32_t*) (hltdc_discovery.LayerCfg[ActiveLayer].FBStartAdress + (4 * (Ypos*BSP_LCD_GetXSize() + Xpos))) = RGB_Code;
}


/**
  * @brief  Fills a buffer.
  * @param  LayerIndex: Layer index
  * @param  pDst: Pointer to destination buffer
  * @param  xSize: Buffer width
  * @param  ySize: Buffer height
  * @param  OffLine: Offset
  * @param  ColorIndex: Color index
  */
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{
    /* Register to memory mode with ARGB8888 as color Mode */
    hdma2d_discovery.Init.Mode = DMA2D_R2M;
    hdma2d_discovery.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
    hdma2d_discovery.Init.OutputOffset = OffLine;

    hdma2d_discovery.Instance = DMA2D;

    /* DMA2D Initialization */
    if (HAL_DMA2D_Init(&hdma2d_discovery) == HAL_OK)
    {
        if (HAL_DMA2D_ConfigLayer(&hdma2d_discovery, LayerIndex) == HAL_OK)
        {
            if (HAL_DMA2D_Start(&hdma2d_discovery, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
            {
                /* Polling For DMA transfer */
                HAL_DMA2D_PollForTransfer(&hdma2d_discovery, 10);
            }
        }
    }
}

/**
  * @brief  Converts a line to an ARGB8888 pixel format.
  * @param  pSrc: Pointer to source buffer
  * @param  pDst: Output color
  * @param  xSize: Buffer width
  * @param  ColorMode: Input color mode
  */
static void LL_ConvertLineToARGB8888(void *pSrc, void *pDst, uint32_t xSize, uint32_t ColorMode)
{
    /* Configure the DMA2D Mode, Color Mode and output offset */
    hdma2d_discovery.Init.Mode = DMA2D_M2M_PFC;
    hdma2d_discovery.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
    hdma2d_discovery.Init.OutputOffset = 0;

    /* Foreground Configuration */
    hdma2d_discovery.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d_discovery.LayerCfg[1].InputAlpha = 0xFF;
    hdma2d_discovery.LayerCfg[1].InputColorMode = ColorMode;
    hdma2d_discovery.LayerCfg[1].InputOffset = 0;

    hdma2d_discovery.Instance = DMA2D;

    /* DMA2D Initialization */
    if (HAL_DMA2D_Init(&hdma2d_discovery) == HAL_OK)
    {
        if (HAL_DMA2D_ConfigLayer(&hdma2d_discovery, 1) == HAL_OK)
        {
            if (HAL_DMA2D_Start(&hdma2d_discovery, (uint32_t)pSrc, (uint32_t)pDst, xSize, 1) == HAL_OK)
            {
                /* Polling For DMA transfer */
                HAL_DMA2D_PollForTransfer(&hdma2d_discovery, 10);
            }
        }
    }
}
