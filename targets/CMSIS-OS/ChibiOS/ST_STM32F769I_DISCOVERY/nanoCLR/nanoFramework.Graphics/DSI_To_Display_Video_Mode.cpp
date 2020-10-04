//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// -------------------------------------------------------
// For the STM32F769IDiscovery evaluation board
// This driver is used to drive directly in video mode a LCD TFT using the DSI interface.

#define STM32F769xx
#define UNUSED(X) (void)X  // To avoid gcc/g++ warnings 

#include "stm32f769xx.h"
#include "stm32f7xx.h"
#include "Core_cm7.h"
#include "DisplayInterface.h"
#include <nanoCLR_Interop.h>
#include <stdarg.h> 
#include <stdio.h>

#ifdef __cplusplus
extern "C" { uint32_t HAL_GetTick(void); }
#endif

void WaitForFIFOEmpty();

extern CLR_UINT32 GraphicsVideoFrameBufferBegin;  // Framebuffer set externally

struct DisplayInterface g_DisplayInterface;
DSI_TypeDef* DsiRegister;                         // Structure to help access registers by name
LTDC_TypeDef* LtdcRegister;                       // Structure to help access registers by name
LTDC_Layer_TypeDef* LTDC_Layer_Register;          // Structure to help access registers by name

CLR_UINT32 LcdClock = 27429;                      // LcdClk = 27429 kHz
CLR_UINT32 laneByteClk_kHz = 62500;               // 500 MHz / 8 = 62.5 MHz = 62500 kHz

// Default to landscape
CLR_UINT32 lcd_x_size = 800;
CLR_UINT32 lcd_y_size = 480;

void DisplayInterface::Initialize(DisplayInterfaceConfig& config)
{
    (void)config;
    
    DsiRegister = DSI;          // Base address of DSI Host/Wrapper registers
    LtdcRegister = LTDC;        // Base addres of LTDC registers
    LTDC_Layer_Register = (LTDC_Layer_TypeDef*)((CLR_UINT32)LTDC + 0x84U); // Base address of layer registers (layer 0)

    // Toggle Hardware Reset of the DSI LCD using its XRES signal (active low)
    palWritePad(GPIOJ, GPIOJ_DSI_RESET, PAL_LOW);
    OS_DELAY(20);
    palWritePad(GPIOJ, GPIOJ_DSI_RESET, PAL_HIGH);
    OS_DELAY(10);

    // LCD MspInit
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_LTDCEN);  // Enable the LTDC clock
    RCC->APB2RSTR |= RCC_APB2RSTR_LTDCRST;      // Toggle Sw reset of LTDC IP
    RCC->APB2RSTR &= ~RCC_APB2RSTR_LTDCRST;     // LTDC release reset
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_DSIEN);   // Enable DSI Host and wrapper clocks
    RCC->APB2RSTR |= RCC_APB2RSTR_DSIRST;       // Soft Reset the DSI Host and wrapper
    RCC->APB2RSTR &= ~RCC_APB2RSTR_DSIRST;

    __NVIC_SetPriority(LTDC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0));     // NVIC configuration for LTDC interrupt that is now enabled
    NVIC_EnableIRQ(LTDC_IRQn);
    __NVIC_SetPriority(DSI_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0));      // NVIC configuration for DSI interrupt that is now enabled
    NVIC_EnableIRQ(DSI_IRQn);

    CLEAR_BIT(DsiRegister->WCR, DSI_WCR_DSIEN);                  // Disable the DSI wrapper
    CLEAR_BIT(DsiRegister->CR, DSI_CR_EN);                       // Disable the DSI host
    DsiRegister->PCTLR &= ~(DSI_PCTLR_CKE | DSI_PCTLR_DEN);      // D-PHY clock and digital disable
    CLEAR_BIT(DsiRegister->WRPCR, DSI_WRPCR_PLLEN);              // Turn off the DSI PLL
    CLEAR_BIT(DsiRegister->WRPCR, DSI_WRPCR_REGEN);              // Disable the regulator

    SET_BIT(DsiRegister->WRPCR, DSI_WRPCR_REGEN);             // Turn on the regulator and enable the DSI PLL
    uint32_t tickstart = HAL_GetTick();
    while ((DsiRegister->WISR & DSI_WISR_RRS) == 0U)          // Wait until the regulator is ready
    {
        if ((HAL_GetTick() - tickstart) > ((uint32_t)1000U))
        {
            // what to do here ?
        }
    }
    // Set the PLL division factors
    DsiRegister->WRPCR &= ~(DSI_WRPCR_PLL_NDIV | DSI_WRPCR_PLL_IDF | DSI_WRPCR_PLL_ODF);
    CLR_UINT32 PLLNDIV = 100;
    CLR_UINT32 PLLIDF = 0x00000005U;
    CLR_UINT32 PLLODF = 0x00000000U;
    DsiRegister->WRPCR |= ((PLLNDIV << 2U) | (PLLIDF << 11U) | (PLLODF << 16U));
    SET_BIT(DsiRegister->WRPCR, DSI_WRPCR_PLLEN);

    tickstart = HAL_GetTick();
    while ((DsiRegister->WISR & DSI_WISR_PLLLS) == 0U)                           // Enable the DSI PLL and Wait until ready
    {
        if ((HAL_GetTick() - tickstart) > ((uint32_t)1000U))
        {
            // What to do here?
        }
    }
    //************************** Set the PHY parameters **************************

    DsiRegister->PCTLR |= (DSI_PCTLR_CKE | DSI_PCTLR_DEN);              // D-PHY clock and digital enable
    DsiRegister->CLCR &= ~(DSI_CLCR_DPCC | DSI_CLCR_ACR);               // Clock lane configuration
    CLR_UINT32 AutomaticClockLaneControl = 0;                            // Disable automatic clock land control
    DsiRegister->CLCR |= (DSI_CLCR_DPCC | AutomaticClockLaneControl);   // AutomaticClockLaneControl is disabled
    DsiRegister->PCONFR &= ~DSI_PCONFR_NL;                              // Configure the number of active data lanes
    DsiRegister->PCONFR |= 1U;                                          // Set number of Lanes to 2

    //*********************** Set the DSI clock parameters ***********************

    // Set the TX escape clock division factor 
    DsiRegister->CCR &= ~DSI_CCR_TXECKDIV;
    DsiRegister->CCR |= laneByteClk_kHz / 15620; //TXEscapeCkdiv = f(LaneByteClk)/15.62 = 4

    // Calculate the bit period in high-speed mode in unit of 0.25 ns (UIX4) 
    // The equation is : UIX4 = IntegerPart( (1000/F_PHY_Mhz) * 4 )          
    //           Where : F_PHY_Mhz = (NDIV * HSE_Mhz) / (IDF * ODF)                    
    uint32_t tempIDF = (PLLIDF > 0U) ? PLLIDF : 1U;
    DsiRegister->WPCR[0U] &= ~DSI_WPCR0_UIX4;
    DsiRegister->WPCR[0U] |= (4000000U * tempIDF * ((1UL << (0x3U & PLLODF)))) / ((((uint32_t)25000000) / 1000U) * PLLNDIV);

    // Disable all error interrupts and reset the Error Mask 
    DsiRegister->IER[0U] = 0U;
    DsiRegister->IER[1U] = 0U;

    // Set Timing parameters of LTDC depending based on  landscape 800x480
    CLR_UINT32 VSA = 1;              // Vertical start active time in units of lines
    CLR_UINT32 VBP = 15;             // Vertical Back Porch time in units of lines            `
    CLR_UINT32 VFP = 16;             // Vertical Front Porch time in units of lines
    CLR_UINT32 HSA = 2;              // Horizontal start active time in units of lcdClk
    CLR_UINT32 HBP = 34;             // Horizontal Back Porch time in units of lcdClk
    CLR_UINT32 HFP = 34;             // Horizontal Front Porch time in units of lcdClk
    CLR_UINT32 HACT = lcd_x_size;    // Horizontal Active time in units of lcdClk = imageSize X in pixels to display
    CLR_UINT32 VACT = lcd_y_size;    // Vertical Active time in units of lines = imageSize Y in pixels to display

    // Configure DSI Video mode timings with settings set above 

    //  Select video mode by resetting CMDM and DSIM bits 
    DsiRegister->MCR &= ~DSI_MCR_CMDM;
    DsiRegister->WCFGR &= ~DSI_WCFGR_DSIM;

    // Configure the video mode transmission type 
    DsiRegister->VMCR &= ~DSI_VMCR_VMT;
    DsiRegister->VMCR |= 2U;                  // Dsi Mode Video burst ie : one LgP per line;

    // Configure the video packet size 
    DsiRegister->VPCR &= ~DSI_VPCR_VPSIZE;
    DsiRegister->VPCR |= HACT;                // Display dependent

    // Set the chunks number to be transmitted through the DSI link 
    DsiRegister->VCCR &= ~DSI_VCCR_NUMC;
    DsiRegister->VCCR |= 0;                   // zero chunks

    // Set the size of the null packet 
    DsiRegister->VNPCR &= ~DSI_VNPCR_NPSIZE;
    DsiRegister->VNPCR |= 0xFFF;

    // Select the virtual channel for the LTDC interface traffic 
    DsiRegister->LVCIDR &= ~DSI_LVCIDR_VCID;
    DsiRegister->LVCIDR |= 0;                 // LCD Display OTM8009A DSI Virtual Channel  ID

    // Configure the polarity of control signals 
    DsiRegister->LPCR &= ~(DSI_LPCR_DEP | DSI_LPCR_VSP | DSI_LPCR_HSP);
    DsiRegister->LPCR |= (0x00000000U | 0x00000000U | 0x00000000U);       // Vertical, Horizontal active high, DSI data enable active high

    // Select the color coding for the host 
    DsiRegister->LCOLCR &= ~DSI_LCOLCR_COLC;
    DsiRegister->LCOLCR |= 0x00000000U;      // RGB565

    // Select the color coding for the wrapper 
    DsiRegister->WCFGR &= ~DSI_WCFGR_COLMUX;
    DsiRegister->WCFGR |= (0x00000000U << 1U);                  // RGB565

    // Set the Horizontal Synchronization Active (HSA) in lane byte clock cycles 
    DsiRegister->VHSACR &= ~DSI_VHSACR_HSA;
    DsiRegister->VHSACR |= (HSA * laneByteClk_kHz) / LcdClock;

    // Set the Horizontal Back Porch (HBP) in lane byte clock cycles 
    DsiRegister->VHBPCR &= ~DSI_VHBPCR_HBP;
    DsiRegister->VHBPCR |= (HBP * laneByteClk_kHz) / LcdClock;

    // Set the total line time (HLINE=HSA+HBP+HACT+HFP) in lane byte clock cycles 
    DsiRegister->VLCR &= ~DSI_VLCR_HLINE;
    DsiRegister->VLCR |= ((HACT + HSA + HBP + HFP) * laneByteClk_kHz) / LcdClock;  // Value depending on display orientation choice portrait/landscape

    // Set the Vertical Synchronization Active (VSA) 
    DsiRegister->VVSACR &= ~DSI_VVSACR_VSA;
    DsiRegister->VVSACR |= VSA;

    // Set the Vertical Back Porch (VBP)
    DsiRegister->VVBPCR &= ~DSI_VVBPCR_VBP;
    DsiRegister->VVBPCR |= VBP;

    // Set the Vertical Front Porch (VFP)
    DsiRegister->VVFPCR &= ~DSI_VVFPCR_VFP;
    DsiRegister->VVFPCR |= VFP;

    // Set the Vertical Active period
    DsiRegister->VVACR &= ~DSI_VVACR_VA;
    DsiRegister->VVACR |= VACT;

    // Configure the command transmission mode 
    DsiRegister->VMCR &= ~DSI_VMCR_LPCE;
    DsiRegister->VMCR |= DSI_VMCR_LPCE;

    // Low power largest packet size 
    DsiRegister->LPMCR &= ~DSI_LPMCR_LPSIZE;
    DsiRegister->LPMCR |= ((16) << 16U);         // Largest packet size possible to transmit in LP mode in VSA, VBP, VFP regions 

    // Low power VACT largest packet size 
    DsiRegister->LPMCR &= ~DSI_LPMCR_VLPSIZE;
    DsiRegister->LPMCR |= 0;                     // Only useful when sending LP packets is allowed while streaming is active in video mode 

    // Enable LP transition in HFP period 
    DsiRegister->VMCR &= ~DSI_VMCR_LPHFPE;
    DsiRegister->VMCR |= DSI_VMCR_LPHFPE;        // Allow sending LP commands during HFP period

    // Enable LP transition in HBP period 
    DsiRegister->VMCR &= ~DSI_VMCR_LPHBPE;
    DsiRegister->VMCR |= DSI_VMCR_LPHBPE;        // Allow sending LP commands during HBP period

    // Enable LP transition in VACT period 
    DsiRegister->VMCR &= ~DSI_VMCR_LPVAE;
    DsiRegister->VMCR |= DSI_VMCR_LPVAE;         // Allow sending LP commands during VACT period

    // Enable LP transition in VFP period 
    DsiRegister->VMCR &= ~DSI_VMCR_LPVFPE;
    DsiRegister->VMCR |= DSI_VMCR_LPVFPE;        // Allow sending LP commands during VFP period

    // Enable LP transition in VBP period 
    DsiRegister->VMCR &= ~DSI_VMCR_LPVBPE;
    DsiRegister->VMCR |= DSI_VMCR_LPVBPE;        // Allow sending LP commands during VBP period

    // Enable LP transition in vertical sync period 
    DsiRegister->VMCR &= ~DSI_VMCR_LPVSAE;
    DsiRegister->VMCR |= DSI_VMCR_LPVSAE;        // Allow sending LP commands during VSync = VSA period

    // Enable the request for an acknowledge response at the end of a frame 
    DsiRegister->VMCR &= ~DSI_VMCR_FBTAAE;
    DsiRegister->VMCR |= 0;

    //************************End DSI Initialization******************************

    //***********************LTDC Initialization**********************************
    RCC->CR &= ~RCC_CR_PLLSAION; // Disable PLLSAI Clock
    tickstart = HAL_GetTick();
    while (((RCC->CR & RCC_CR_PLLSAIRDY) == RCC_CR_PLLSAIRDY) != RESET) // Wait till PLLSAI is disabled
    {
        if ((HAL_GetTick() - tickstart) > 100U)
        {
            // What to do here?
        }
    }
    // Read PLLSAIP and PLLSAIQ value from PLLSAICFGR register (these value are not needed for LTDC configuration) 
    RCC->PLLSAICFGR = (384 << RCC_PLLSAICFGR_PLLSAIN_Pos) |
        (((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIP) >> RCC_PLLSAICFGR_PLLSAIP_Pos) << RCC_PLLSAICFGR_PLLSAIP_Pos) |
        (((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> RCC_PLLSAICFGR_PLLSAIQ_Pos) << RCC_PLLSAICFGR_PLLSAIQ_Pos) |
        (7 << RCC_PLLSAICFGR_PLLSAIR_Pos);

    MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_PLLSAIDIVR, ((uint32_t)0x00000000U));
    RCC->CR |= RCC_CR_PLLSAION;                                         // Enable PLLSAI Clock
    tickstart = HAL_GetTick();                                          // Get Start Tick
    while (((RCC->CR & RCC_CR_PLLSAIRDY) == RCC_CR_PLLSAIRDY) == RESET) // Wait till PLLSAI is ready
    {
        if ((HAL_GetTick() - tickstart) > 100U)
        {
            // what to do here?
        }
    }
    // To avoid any synchronization issue, the DSI shall be started after enabling the LTDC 

    // Configure the HS, VS, DE and PC polarity 
    LtdcRegister->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
    LtdcRegister->GCR |= (LTDC_GCR_HSPOL | LTDC_GCR_VSPOL); //  PCPolarity = 0, DEPolarity =  0;

    // Set Synchronization size 
    LtdcRegister->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
    LtdcRegister->SSCR |= (((HSA - 1) << 16U) | (VSA - 1U));

    // Set Accumulated Back porch 
    LtdcRegister->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
    LtdcRegister->BPCR |= (((HSA + HBP - 1) << 16U) | (VSA + VBP - 1U));

    // Set Accumulated Active Width 
    LtdcRegister->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
    LtdcRegister->AWCR |= (((lcd_x_size + HSA + HBP - 1) << 16U) | (VSA + VBP + VACT - 1U));

    // Set Total Width 
    LtdcRegister->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
    LtdcRegister->TWCR |= (((lcd_x_size + HSA + HBP + HFP - 1) << 16U) | (VSA + VBP + VACT + VFP - 1U));

    // Set the background color value 
    LtdcRegister->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
    LtdcRegister->BCCR |= 0;                                        // White background

    LtdcRegister->IER |= (LTDC_IER_TERRIE | LTDC_IER_FUIE);         // Enable the Transfer Error and FIFO underrun interrupts
    LtdcRegister->GCR |= LTDC_GCR_LTDCEN;                           // Enable LTDC by setting LTDCEN bit

    // Start DSI
    SET_BIT(DsiRegister->CR, DSI_CR_EN);                            // Enable the DSI host
    SET_BIT(DsiRegister->WCR, DSI_WCR_DSIEN);                       // Enable the DSI wrapper

    // Layer Init
    CLR_UINT32 PixelFormat = 0x00000002U;                           //  PIXEL FORMAT RGB565;
    CLR_UINT32 Alpha = 255;
    CLR_UINT32 BlendingFactor1 = 0x00000600U;                       // Blending factor : Cte Alpha x Pixel Alpha
    CLR_UINT32 BlendingFactor2 = 0x00000007U;                       // Blending factor : Cte Alpha x Pixel Alpha

    //LCD - TFT Display layer x Controller
    // Configure the horizontal start and stop position
    LTDC_Layer_Register->WHPCR &= ~LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS;
    LTDC_Layer_Register->WHPCR = ((((LtdcRegister->BPCR & LTDC_BPCR_AHBP) >> 16U) + 1U) | ((lcd_x_size + ((LtdcRegister->BPCR & LTDC_BPCR_AHBP) >> 16U)) << 16U));

    // Configure the vertical start and stop position
    LTDC_Layer_Register->WVPCR &= ~LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS;
    LTDC_Layer_Register->WVPCR = (((LtdcRegister->BPCR & LTDC_BPCR_AVBP) + 1U) | ((lcd_y_size + (LtdcRegister->BPCR & LTDC_BPCR_AVBP)) << 16U));

    // Specify the pixel format
    LTDC_Layer_Register->PFCR &= ~LTDC_LxPFCR_PF;
    LTDC_Layer_Register->PFCR = PixelFormat;

    // Configure the default color values
    LTDC_Layer_Register->DCCR &= ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);
    LTDC_Layer_Register->DCCR = 0;;

    // Specify the constant alpha value
    LTDC_Layer_Register->CACR &= ~(LTDC_LxCACR_CONSTA);
    LTDC_Layer_Register->CACR = Alpha;

    // Specify the blending factors
    LTDC_Layer_Register->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
    LTDC_Layer_Register->BFCR = BlendingFactor1 | BlendingFactor2;

    // Configure the color frame buffer start address
    LTDC_Layer_Register->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
    LTDC_Layer_Register->CFBAR = (uint32_t)&GraphicsVideoFrameBufferBegin;

    // Configure the color frame buffer pitch in byte
    LTDC_Layer_Register->CFBLR &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
    LTDC_Layer_Register->CFBLR = (((lcd_x_size * 2U) << 16U) | ((lcd_x_size * 2U) + 3U));

    // Configure the frame buffer line number
    LTDC_Layer_Register->CFBLNR &= ~(LTDC_LxCFBLNR_CFBLNBR);
    LTDC_Layer_Register->CFBLNR = lcd_y_size;

    // Enable LTDC_Layer by setting LEN bit
    LTDC_Layer_Register->CR |= (uint32_t)LTDC_LxCR_LEN;
    LtdcRegister->SRCR = LTDC_SRCR_IMR;      // Set the Immediate Reload type

    ClearFrameBuffer();

    return;
}
void DisplayInterface::GetTransferBuffer(CLR_UINT8*& TransferBuffer, CLR_UINT32& TransferBufferSize)
{
    UNUSED(TransferBuffer);
    UNUSED(TransferBufferSize);
    return;
}
void DisplayInterface::ClearFrameBuffer()
{
    CLR_UINT32* frameBuffer = (CLR_UINT32*)&GraphicsVideoFrameBufferBegin;
    CLR_INT32 DataCount32 = (lcd_x_size * lcd_y_size * 2) / 4;
    for (CLR_INT32 i = 0; i < DataCount32; i++)
    {
        // Note: Write out 4 byte ints measured to be faster than bytes (* 3.5-4 times)
        *frameBuffer = 0;
        frameBuffer++;
    }
}
void DisplayInterface::WriteToFrameBuffer(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount)
{
    UNUSED(command);
    CLR_UINT32* frameBuffer = (CLR_UINT32*)&GraphicsVideoFrameBufferBegin;
    CLR_UINT32* source = (CLR_UINT32*)&data[0];
    CLR_INT32 DataCount32 = dataCount / 4;
    for (CLR_INT32 i = 0; i < DataCount32; i++)
    {
        // Note: Write out 4 byte ints measured to be faster than bytes (* 3.5-4 times)
        *frameBuffer = *source;
        frameBuffer++;
        source++;
    }
    return;
}
void DisplayInterface::DisplayBacklight(bool on)
{
    uint8_t OTM8009A_CMD_DISPOFF = 0x28; // Display Off command
    uint8_t OTM8009A_CMD_DISPON = 0x29;  // Display On command
    uint8_t cmd;
    if (on)
        cmd = OTM8009A_CMD_DISPOFF;
    else
        cmd = OTM8009A_CMD_DISPON;
    (void)cmd;
    // DSI_IO_WriteCmd(&hdsi_discovery, DisplayVirtualChannel, 1, &cmd);
}

void DisplayInterface::SendCommand(CLR_UINT8 NbrParams, ...)
{
    va_list ap;
    va_start(ap, NbrParams);
    volatile CLR_UINT8 command;
    const int DSI_DCS_SHORT_PKT_WRITE_P1 = 0x00000015U;     // DCS short write, one parameter
    const int DSI_DCS_LONG_PKT_WRITE = 0x00000039U;         // DCS long write, multiple parameters
    const uint32_t LCD_OTM8009A_ID = 0;                     // LCD Display OTM8009A DSI Virtual Channel  ID

    WaitForFIFOEmpty();
    switch (NbrParams)
    {
    case 1:
    {
        command = va_arg(ap, int);
        DsiRegister->GHCR = (DSI_DCS_SHORT_PKT_WRITE_P1 | (LCD_OTM8009A_ID << 6U) | (command << 8U));
        break;
    }
    case 2:
    {
        command = va_arg(ap, int);
        volatile CLR_UINT8 parameter = va_arg(ap, int);
        DsiRegister->GHCR = (DSI_DCS_SHORT_PKT_WRITE_P1 | (LCD_OTM8009A_ID << 6U) | (command << 8U) | (parameter << 16U));
        break;
    }
    default:
    {

        WaitForFIFOEmpty();

        // Set the DCS code on payload byte 1, and the other parameters on the write FIFO command
        uint32_t fifoword = va_arg(ap, int);
        uint32_t nbBytes = (NbrParams < 3U) ? NbrParams : 3U;
        for (CLR_UINT8 count = 0U; count < nbBytes; count++)
        {
            CLR_UINT8 parameter = va_arg(ap, int);
            fifoword |= parameter << (8U + (8U * count));
        }
        DsiRegister->GPDR = fifoword;

        uint32_t uicounter = NbrParams - nbBytes;
        while (uicounter != 0U)                             // Set the Next parameters on the write FIFO command
        {
            nbBytes = (uicounter < 4U) ? uicounter : 4U;
            fifoword = 0U;
            for (CLR_UINT8 count = 0U; count < nbBytes; count++)
            {
                CLR_UINT8 parameter = va_arg(ap, int);
                fifoword |= parameter << (8U * count);
            }
            DsiRegister->GPDR = fifoword;

            uicounter -= nbBytes;
        }

        DsiRegister->GHCR = (DSI_DCS_LONG_PKT_WRITE | (LCD_OTM8009A_ID << 6U) | (((NbrParams + 1U) & 0x00FFU) << 8U) | ((((NbrParams + 1U) & 0xFF00U) >> 8U) << 16U)); // Update the DSI packet header with new information 

    }
    }
}
void WaitForFIFOEmpty()
{
    uint32_t tickstart = HAL_GetTick();
    while ((DsiRegister->GPSR & DSI_GPSR_CMDFE) == 0U) // Wait for Command FIFO Empty
    {
        if ((HAL_GetTick() - tickstart) > ((uint32_t)1000U))
        {
            // Hardware problem?. What to do here ?
        }
    }
}



