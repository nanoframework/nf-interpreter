//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

//__________________________________________________________________________________ 
// Code extracted from online mbed compiler example "/DISCO-F769NI_TOUCHSCREEN_demo/
//__________________________________________________________________________________ 


#ifdef __cplusplus
extern "C" {
#endif

#include "STM32F7LcdSupport.h"
#include "Core_cm7.h"

    // __________________________________________________
    // Supporting code for the STMf32769-Discovery board
    // __________________________________________________

    DSI_HandleTypeDef hdsi_discovery;
    static DSI_VidCfgTypeDef hdsivideo_handle;
    __IO uint32_t uwTick;
    static ft6x06_handle_TypeDef ft6x06_handle = { FT6206_I2C_NOT_INITIALIZED, 0, 0 };
    static TS_DrvTypeDef* ts_driver;
    static uint8_t  ts_orientation;
    uint8_t  I2C_Address = 0;
    static I2C_HandleTypeDef hI2cAudioHandler = { 0 };
    uint8_t ft6x06[FT6x06_MAX_INSTANCE] = { 0 };
    LTDC_HandleTypeDef  hltdc_discovery;
    static LCD_DrawPropTypeDef DrawProp[LTDC_MAX_LAYER_NUMBER];
    static FMC_SDRAM_TimingTypeDef Timing;
    SDRAM_HandleTypeDef sdramHandle;
    static FMC_SDRAM_CommandTypeDef Command;

    void STM32F769i_Discovery_LCD_INIT(uint32_t FB_Address)
    {
        uint16_t LayerIndex = 0;
        // Toggle Hardware Reset of the DSI LCD using  its XRES signal (active low)
        BSP_LCD_Reset();
        BSP_LCD_MspInit();
        BSP_LCD_InitEx(LCD_ORIENTATION_PORTRAIT);
        BSP_LCD_LayerDefaultInit(LayerIndex, FB_Address);
        BSP_TS_Init();
    }
    void BSP_LCD_MspInit(void)
    {
        __HAL_RCC_LTDC_CLK_ENABLE();             // Enable the LTDC clock
        __HAL_RCC_LTDC_FORCE_RESET();            // Toggle Sw reset of LTDC IP
        __HAL_RCC_LTDC_RELEASE_RESET();
        __HAL_RCC_DMA2D_CLK_ENABLE();            // Enable the DMA2D clock
        __HAL_RCC_DMA2D_FORCE_RESET();           // Toggle Sw reset of DMA2D IP
        __HAL_RCC_DMA2D_RELEASE_RESET();
        __HAL_RCC_DSI_CLK_ENABLE();              // Enable DSI Host and wrapper clocks
        __HAL_RCC_DSI_FORCE_RESET();             // Soft Reset the DSI Host and wrapper
        __HAL_RCC_DSI_RELEASE_RESET();

        uint32_t prioritygroup = __NVIC_GetPriorityGrouping();
        NVIC_SetPriority(LTDC_IRQn, NVIC_EncodePriority(prioritygroup, 3, 0));  // NVIC configuration for LTDC interrupt that is now enabled
        __NVIC_EnableIRQ(LTDC_IRQn);
        __NVIC_SetPriority(DMA2D_IRQn, NVIC_EncodePriority(prioritygroup, 3, 0));  // NVIC configuration for DMA2D interrupt that is now enabled
        __NVIC_EnableIRQ(DMA2D_IRQn);
        __NVIC_SetPriority(DSI_IRQn, NVIC_EncodePriority(prioritygroup, 3, 0));  // NVIC configuration for DSI interrupt that is now enabled
        __NVIC_EnableIRQ(DSI_IRQn);
    }
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
    uint8_t BSP_LCD_InitEx()
    {

        /*************************DSI Initialization***********************************/
        DSI_PLLInitTypeDef dsiPllInit;
        static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
        LTDC_HandleTypeDef  hltdc_discovery;


        uint32_t LcdClock = 27429; /*!< LcdClk = 27429 kHz */
        uint32_t laneByteClk_kHz = 0;
        /* Base address of DSI Host/Wrapper registers to be set before calling De-Init */
        hdsi_discovery.Instance = DSI;

        HAL_DSI_DeInit(&(hdsi_discovery));

        dsiPllInit.PLLNDIV = 100;
        dsiPllInit.PLLIDF = DSI_PLL_IN_DIV5;
        dsiPllInit.PLLODF = DSI_PLL_OUT_DIV1;
        laneByteClk_kHz = 62500; /* 500 MHz / 8 = 62.5 MHz = 62500 kHz */

        hdsi_discovery.Init.NumberOfLanes = DSI_TWO_DATA_LANES;          /* Set number of Lanes */
        hdsi_discovery.Init.TXEscapeCkdiv = laneByteClk_kHz / 15620;      /* TXEscapeCkdiv = f(LaneByteClk)/15.62 = 4 */
        HAL_DSI_Init(&(hdsi_discovery), &(dsiPllInit));
        hdsivideo_handle.ColorCoding = LCD_DSI_PIXEL_DATA_FMT_RBG565;
        hdsivideo_handle.VirtualChannelID = LCD_OTM8009A_ID;
        hdsivideo_handle.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
        hdsivideo_handle.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
        hdsivideo_handle.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
        hdsivideo_handle.Mode = DSI_VID_MODE_BURST;                     /* Mode Video burst ie : one LgP per line */
        hdsivideo_handle.NullPacketSize = 0xFFF;
        hdsivideo_handle.NumberOfChunks = 0;
        hdsivideo_handle.PacketSize = 800;                              /* Value depending on display orientation choice portrait/landscape */
        hdsivideo_handle.HorizontalSyncActive = (OTM8009A_480X800_HSYNC * laneByteClk_kHz) / LcdClock;
        hdsivideo_handle.HorizontalBackPorch = (OTM8009A_480X800_HBP * laneByteClk_kHz) / LcdClock;
        hdsivideo_handle.HorizontalLine = ((800 + OTM8009A_480X800_HSYNC + OTM8009A_480X800_HBP + OTM8009A_480X800_HFP) * laneByteClk_kHz) / LcdClock; /* Value depending on display orientation choice portrait/landscape */
        hdsivideo_handle.VerticalSyncActive = OTM8009A_480X800_VSYNC;
        hdsivideo_handle.VerticalBackPorch = OTM8009A_480X800_VBP;
        hdsivideo_handle.VerticalFrontPorch = OTM8009A_480X800_VFP;
        hdsivideo_handle.VerticalActive = 480;                          /* Value depending on display orientation choice portrait/landscape */

        /* Enable or disable sending LP command while streaming is active in video mode */
        hdsivideo_handle.LPCommandEnable = DSI_LP_COMMAND_ENABLE; /* Enable sending commands in mode LP (Low Power) */
        hdsivideo_handle.LPLargestPacketSize = 16;
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
        hltdc_discovery.Init.HorizontalSync = (OTM8009A_480X800_HSYNC - 1);
        hltdc_discovery.Init.AccumulatedHBP = (OTM8009A_480X800_HSYNC + OTM8009A_480X800_HBP - 1);
        hltdc_discovery.Init.AccumulatedActiveW = (800 + OTM8009A_480X800_HSYNC + OTM8009A_480X800_HBP - 1);
        hltdc_discovery.Init.TotalWidth = (800 + OTM8009A_480X800_HSYNC + OTM8009A_480X800_HBP + OTM8009A_480X800_HFP - 1);


        hltdc_discovery.LayerCfg->ImageWidth = 800;         // Initialize the LCD pixel width and pixel height
        hltdc_discovery.LayerCfg->ImageHeight = 480;

        /** LCD clock configuration
          * Note: The following values should not be changed as the PLLSAI is also used to clock the USB FS
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

        //////#if !defined(DATA_IN_ExtSDRAM)
        //////        /* Initialize the SDRAM */
        //////        BSP_SDRAM_Init();
        //////#endif /* DATA_IN_ExtSDRAM */


        return LCD_OK;
    }
    void BSP_LCD_LayerDefaultInit(uint32_t LayerIndex, uint32_t FB_Address)
    {
        LCD_LayerCfgTypeDef  Layercfg;

        /* Layer Init */
        Layercfg.WindowX0 = 0;
        Layercfg.WindowX1 = 800;
        Layercfg.WindowY0 = 0;
        Layercfg.WindowY1 = 480;
        Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
        Layercfg.FBStartAdress = FB_Address;
        Layercfg.Alpha = 255;
        Layercfg.Alpha0 = 0;
        Layercfg.Backcolor.Blue = 0;
        Layercfg.Backcolor.Green = 0;
        Layercfg.Backcolor.Red = 0;
        Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
        Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
        Layercfg.ImageWidth = 800;
        Layercfg.ImageHeight = 480;

        HAL_LTDC_ConfigLayer(&hltdc_discovery, &Layercfg, LayerIndex);

        DrawProp[LayerIndex].BackColor = LCD_COLOR_WHITE;
        //     DrawProp[LayerIndex].pFont = &Font24;
        DrawProp[LayerIndex].TextColor = LCD_COLOR_BLACK;
    }
    uint8_t BSP_TS_Init(void)
    {
        uint8_t ts_status = TS_OK;
        uint8_t ts_id1, ts_id2 = 0;
        /* Note : I2C_Address is un-initialized here, but is not used at all in init function */
        /* but the prototype of Init() is like that in template and should be respected       */

        /* Initialize the communication channel to sensor (I2C) if necessary */
        /* that is initialization is done only once after a power up         */
        ft6x06_ts_drv.Init(I2C_Address);

        ts_id1 = ft6x06_ts_drv.ReadID(TS_I2C_ADDRESS);
        if (ts_id1 != FT6206_ID_VALUE)
        {
            ts_id2 = ft6x06_ts_drv.ReadID(TS_I2C_ADDRESS_A02);
            I2C_Address = TS_I2C_ADDRESS_A02;
        }
        else
        {
            I2C_Address = TS_I2C_ADDRESS;
        }

        /* Scan FT6xx6 TouchScreen IC controller ID register by I2C Read       */
        /* Verify this is a FT6206 or FT6336G, otherwise this is an error case */
        if ((ts_id1 == FT6206_ID_VALUE) || (ts_id2 == FT6206_ID_VALUE))
        {
            /* Found FT6206 : Initialize the TS driver structure */
            ts_driver = &ft6x06_ts_drv;

            /* Get LCD chosen orientation */
            if (800 < 480)
            {
                ts_orientation = TS_SWAP_NONE;
            }
            else
            {
                ts_orientation = TS_SWAP_XY | TS_SWAP_Y;
            }

            if (ts_status == TS_OK)
            {
                /* Software reset the TouchScreen */
                ts_driver->Reset(I2C_Address);

                /* Calibrate, Configure and Start the TouchScreen driver */
                ts_driver->Start(I2C_Address);

            } /* of if(ts_status == TS_OK) */
        }
        else
        {
            ts_status = TS_DEVICE_NOT_FOUND;
        }

        return (ts_status);
    }

    HAL_StatusTypeDef HAL_DSI_DeInit(DSI_HandleTypeDef* hdsi)
    {
        if (hdsi == NULL)                                             // Check the DSI handle allocation
        {
            return HAL_ERROR;
        }
        hdsi->State = HAL_DSI_STATE_BUSY;                             // Change DSI peripheral state
        __HAL_DSI_WRAPPER_DISABLE(hdsi);                              // Disable the DSI wrapper
        __HAL_DSI_DISABLE(hdsi);
        hdsi->Instance->PCTLR &= ~(DSI_PCTLR_CKE | DSI_PCTLR_DEN);    // D-PHY clock and digital disable
        __HAL_DSI_PLL_DISABLE(hdsi);                                  // Turn off the DSI PLL
        __HAL_DSI_REG_DISABLE(hdsi);                                  // Disable the regulator
        HAL_DSI_MspDeInit(hdsi);                                      // DeInit the low level hardware
        hdsi->ErrorCode = HAL_DSI_ERROR_NONE;                         // Initialise the error code
        hdsi->State = HAL_DSI_STATE_RESET;                            // Initialize the DSI state
        __HAL_UNLOCK(hdsi);                                           // Release Lock
        return HAL_OK;
    }
    void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* hsdram)
    {
        UNUSED(hsdram);
        GPIO_InitTypeDef gpio_init_structure;

        /* Enable FMC clock */
        __HAL_RCC_FMC_CLK_ENABLE();

        /* Enable GPIOs clock */
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        __HAL_RCC_GPIOI_CLK_ENABLE();

        /* Common GPIO configuration */
        gpio_init_structure.Mode = GPIO_MODE_AF_PP;
        gpio_init_structure.Pull = GPIO_PULLUP;
        gpio_init_structure.Speed = GPIO_SPEED_FAST;
        gpio_init_structure.Alternate = GPIO_AF12_FMC;

        /* GPIOD configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
            GPIO_PIN_14 | GPIO_PIN_15;


        HAL_GPIO_Init(GPIOD, &gpio_init_structure);

        /* GPIOE configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | \
            GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | \
            GPIO_PIN_15;

        HAL_GPIO_Init(GPIOE, &gpio_init_structure);

        /* GPIOF configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | \
            GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | \
            GPIO_PIN_15;

        HAL_GPIO_Init(GPIOF, &gpio_init_structure);

        /* GPIOG configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
            GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
        HAL_GPIO_Init(GPIOG, &gpio_init_structure);

        /* GPIOH configuration */
        gpio_init_structure.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | \
            GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | \
            GPIO_PIN_15;
        HAL_GPIO_Init(GPIOH, &gpio_init_structure);

        /* GPIOI configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | \
            GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
        HAL_GPIO_Init(GPIOI, &gpio_init_structure);
    }
    HAL_StatusTypeDef FMC_SDRAM_Timing_Init(FMC_SDRAM_TypeDef* Device, FMC_SDRAM_TimingTypeDef* Timing, uint32_t Bank)
    {
        uint32_t tmpr1 = 0;
        uint32_t tmpr2 = 0;


        /* Set SDRAM device timing parameters */
        if (Bank != FMC_SDRAM_BANK2)
        {
            tmpr1 = Device->SDTR[FMC_SDRAM_BANK1];

            /* Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits */
            tmpr1 &= ((uint32_t)~(FMC_SDTR1_TMRD | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
                FMC_SDTR1_TRC | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
                FMC_SDTR1_TRCD));

            tmpr1 |= (uint32_t)(((Timing->LoadToActiveDelay) - 1) | \
                (((Timing->ExitSelfRefreshDelay) - 1) << 4) | \
                (((Timing->SelfRefreshTime) - 1) << 8) | \
                (((Timing->RowCycleDelay) - 1) << 12) | \
                (((Timing->WriteRecoveryTime) - 1) << 16) | \
                (((Timing->RPDelay) - 1) << 20) | \
                (((Timing->RCDDelay) - 1) << 24));
            Device->SDTR[FMC_SDRAM_BANK1] = tmpr1;
        }
        else /* FMC_Bank2_SDRAM */
        {
            tmpr1 = Device->SDTR[FMC_SDRAM_BANK1];

            /* Clear TRC and TRP bits */
            tmpr1 &= ((uint32_t)~(FMC_SDTR1_TRC | FMC_SDTR1_TRP));

            tmpr1 |= (uint32_t)((((Timing->RowCycleDelay) - 1) << 12) | \
                (((Timing->RPDelay) - 1) << 20));

            tmpr2 = Device->SDTR[FMC_SDRAM_BANK2];

            /* Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits */
            tmpr2 &= ((uint32_t)~(FMC_SDTR1_TMRD | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
                FMC_SDTR1_TRC | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
                FMC_SDTR1_TRCD));

            tmpr2 |= (uint32_t)(((Timing->LoadToActiveDelay) - 1) | \
                (((Timing->ExitSelfRefreshDelay) - 1) << 4) | \
                (((Timing->SelfRefreshTime) - 1) << 8) | \
                (((Timing->WriteRecoveryTime) - 1) << 16) | \
                (((Timing->RCDDelay) - 1) << 24));

            Device->SDTR[FMC_SDRAM_BANK1] = tmpr1;
            Device->SDTR[FMC_SDRAM_BANK2] = tmpr2;
        }

        return HAL_OK;
    }
    HAL_StatusTypeDef FMC_SDRAM_Init(FMC_SDRAM_TypeDef* Device, FMC_SDRAM_InitTypeDef* Init)
    {
        uint32_t tmpr1 = 0;
        uint32_t tmpr2 = 0;



        /* Set SDRAM bank configuration parameters */
        if (Init->SDBank != FMC_SDRAM_BANK2)
        {
            tmpr1 = Device->SDCR[FMC_SDRAM_BANK1];

            /* Clear NC, NR, MWID, NB, CAS, WP, SDCLK, RBURST, and RPIPE bits */
            tmpr1 &= ((uint32_t)~(FMC_SDCR1_NC | FMC_SDCR1_NR | FMC_SDCR1_MWID | \
                FMC_SDCR1_NB | FMC_SDCR1_CAS | FMC_SDCR1_WP | \
                FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE));

            tmpr1 |= (uint32_t)(Init->ColumnBitsNumber | \
                Init->RowBitsNumber | \
                Init->MemoryDataWidth | \
                Init->InternalBankNumber | \
                Init->CASLatency | \
                Init->WriteProtection | \
                Init->SDClockPeriod | \
                Init->ReadBurst | \
                Init->ReadPipeDelay
                );
            Device->SDCR[FMC_SDRAM_BANK1] = tmpr1;
        }
        else /* FMC_Bank2_SDRAM */
        {
            tmpr1 = Device->SDCR[FMC_SDRAM_BANK1];

            /* Clear SDCLK, RBURST, and RPIPE bits */
            tmpr1 &= ((uint32_t)~(FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE));

            tmpr1 |= (uint32_t)(Init->SDClockPeriod | \
                Init->ReadBurst | \
                Init->ReadPipeDelay);

            tmpr2 = Device->SDCR[FMC_SDRAM_BANK2];

            /* Clear NC, NR, MWID, NB, CAS, WP, SDCLK, RBURST, and RPIPE bits */
            tmpr2 &= ((uint32_t)~(FMC_SDCR1_NC | FMC_SDCR1_NR | FMC_SDCR1_MWID | \
                FMC_SDCR1_NB | FMC_SDCR1_CAS | FMC_SDCR1_WP | \
                FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE));

            tmpr2 |= (uint32_t)(Init->ColumnBitsNumber | \
                Init->RowBitsNumber | \
                Init->MemoryDataWidth | \
                Init->InternalBankNumber | \
                Init->CASLatency | \
                Init->WriteProtection);

            Device->SDCR[FMC_SDRAM_BANK1] = tmpr1;
            Device->SDCR[FMC_SDRAM_BANK2] = tmpr2;
        }

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_SDRAM_Init(SDRAM_HandleTypeDef* hsdram, FMC_SDRAM_TimingTypeDef* Timing)
    {
        /* Check the SDRAM handle parameter */
        if (hsdram == NULL)
        {
            return HAL_ERROR;
        }

        if (hsdram->State == HAL_SDRAM_STATE_RESET)
        {
            /* Allocate lock resource and initialize it */
            hsdram->Lock = HAL_UNLOCKED;
            //#if (USE_HAL_SDRAM_REGISTER_CALLBACKS == 1)
            //            if (hsdram->MspInitCallback == NULL)
            //            {
            //                hsdram->MspInitCallback = HAL_SDRAM_MspInit;
            //            }
            //            hsdram->RefreshErrorCallback = HAL_SDRAM_RefreshErrorCallback;
            //            hsdram->DmaXferCpltCallback = HAL_SDRAM_DMA_XferCpltCallback;
            //            hsdram->DmaXferErrorCallback = HAL_SDRAM_DMA_XferErrorCallback;
            //
            //            /* Init the low level hardware */
            //            hsdram->MspInitCallback(hsdram);
            //#else
                        /* Initialize the low level hardware (MSP) */
            HAL_SDRAM_MspInit(hsdram);
            //#endif
        }

        /* Initialize the SDRAM controller state */
        hsdram->State = HAL_SDRAM_STATE_BUSY;

        /* Initialize SDRAM control Interface */
        FMC_SDRAM_Init(hsdram->Instance, &(hsdram->Init));

        /* Initialize SDRAM timing Interface */
        FMC_SDRAM_Timing_Init(hsdram->Instance, Timing, hsdram->Init.SDBank);

        /* Update the SDRAM controller state */
        hsdram->State = HAL_SDRAM_STATE_READY;

        return HAL_OK;
    }
    HAL_StatusTypeDef FMC_SDRAM_SendCommand(FMC_SDRAM_TypeDef* Device, FMC_SDRAM_CommandTypeDef* Command, uint32_t Timeout)
    {
        UNUSED(Timeout);
        __IO uint32_t tmpr = 0;
        /* Check the parameters */
#define IS_FMC_SDRAM_DEVICE(__INSTANCE__) ((__INSTANCE__) == FMC_SDRAM_DEVICE)
#define IS_FMC_COMMAND_MODE(__COMMAND__) (((__COMMAND__) == FMC_SDRAM_CMD_NORMAL_MODE)      || \
                                          ((__COMMAND__) == FMC_SDRAM_CMD_CLK_ENABLE)       || \
                                          ((__COMMAND__) == FMC_SDRAM_CMD_PALL)             || \
                                          ((__COMMAND__) == FMC_SDRAM_CMD_AUTOREFRESH_MODE) || \
                                          ((__COMMAND__) == FMC_SDRAM_CMD_LOAD_MODE)        || \
                                          ((__COMMAND__) == FMC_SDRAM_CMD_SELFREFRESH_MODE) || \
                                          ((__COMMAND__) == FMC_SDRAM_CMD_POWERDOWN_MODE))
#define IS_FMC_COMMAND_TARGET(__TARGET__) (((__TARGET__) == FMC_SDRAM_CMD_TARGET_BANK1) || \
                                           ((__TARGET__) == FMC_SDRAM_CMD_TARGET_BANK2) || \
                                           ((__TARGET__) == FMC_SDRAM_CMD_TARGET_BANK1_2)) 										  
#define IS_FMC_AUTOREFRESH_NUMBER(__NUMBER__) (((__NUMBER__) > 0) && ((__NUMBER__) <= 16))
#define IS_FMC_MODE_REGISTER(__CONTENT__) ((__CONTENT__) <= 8191)

        assert_param(IS_FMC_SDRAM_DEVICE(Device));
        assert_param(IS_FMC_COMMAND_MODE(Command->CommandMode));
        assert_param(IS_FMC_COMMAND_TARGET(Command->CommandTarget));
        assert_param(IS_FMC_AUTOREFRESH_NUMBER(Command->AutoRefreshNumber));
        assert_param(IS_FMC_MODE_REGISTER(Command->ModeRegisterDefinition));

        /* Set command register */
        tmpr = (uint32_t)((Command->CommandMode) | \
            (Command->CommandTarget) | \
            (((Command->AutoRefreshNumber) - 1) << 5) | \
            ((Command->ModeRegisterDefinition) << 9)
            );

        Device->SDCMR = tmpr;

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_SDRAM_SendCommand(SDRAM_HandleTypeDef* hsdram, FMC_SDRAM_CommandTypeDef* Command, uint32_t Timeout)
    {
        /* Check the SDRAM controller state */
        if (hsdram->State == HAL_SDRAM_STATE_BUSY)
        {
            return HAL_BUSY;
        }

        /* Update the SDRAM state */
        hsdram->State = HAL_SDRAM_STATE_BUSY;

        /* Send SDRAM command */
        FMC_SDRAM_SendCommand(hsdram->Instance, Command, Timeout);

        /* Update the SDRAM controller state state */
        if (Command->CommandMode == FMC_SDRAM_CMD_PALL)
        {
            hsdram->State = HAL_SDRAM_STATE_PRECHARGED;
        }
        else
        {
            hsdram->State = HAL_SDRAM_STATE_READY;
        }

        return HAL_OK;
    }
    HAL_StatusTypeDef FMC_SDRAM_ProgramRefreshRate(FMC_SDRAM_TypeDef* Device, uint32_t RefreshRate)
    {
        /* Check the parameters */
        assert_param(IS_FMC_SDRAM_DEVICE(Device));
#define IS_FMC_REFRESH_RATE(__RATE__) ((__RATE__) <= 8191)
        assert_param(IS_FMC_REFRESH_RATE(RefreshRate));

        /* Set the refresh rate in command register */
        Device->SDRTR |= (RefreshRate << 1);

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_SDRAM_ProgramRefreshRate(SDRAM_HandleTypeDef* hsdram, uint32_t RefreshRate)
    {
        /* Check the SDRAM controller state */
        if (hsdram->State == HAL_SDRAM_STATE_BUSY)
        {
            return HAL_BUSY;
        }

        /* Update the SDRAM state */
        hsdram->State = HAL_SDRAM_STATE_BUSY;

        /* Program the refresh rate */
        FMC_SDRAM_ProgramRefreshRate(hsdram->Instance, RefreshRate);

        /* Update the SDRAM state */
        hsdram->State = HAL_SDRAM_STATE_READY;

        return HAL_OK;
    }
    void BSP_SDRAM_Initialization_sequence(uint32_t RefreshCount)
    {
        __IO uint32_t tmpmrd = 0;
#define SDRAM_TIMEOUT                    ((uint32_t)0xFFFF)

        /* Step 1: Configure a clock configuration enable command */
        Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
        Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
        Command.AutoRefreshNumber = 1;
        Command.ModeRegisterDefinition = 0;

        /* Send the command */
        HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

        /* Step 2: Insert 100 us minimum delay */
        /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
        wait_ms(1);

        /* Step 3: Configure a PALL (precharge all) command */
        Command.CommandMode = FMC_SDRAM_CMD_PALL;
        Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
        Command.AutoRefreshNumber = 1;
        Command.ModeRegisterDefinition = 0;

        /* Send the command */
        HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

        /* Step 4: Configure an Auto Refresh command */
        Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
        Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
        Command.AutoRefreshNumber = 8;
        Command.ModeRegisterDefinition = 0;

        /* Send the command */
        HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

        /* Step 5: Program the external memory mode register */
        tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1 | \
            SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL | \
            SDRAM_MODEREG_CAS_LATENCY_3 | \
            SDRAM_MODEREG_OPERATING_MODE_STANDARD | \
            SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

        Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
        Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
        Command.AutoRefreshNumber = 1;
        Command.ModeRegisterDefinition = tmpmrd;

        /* Send the command */
        HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

        /* Step 6: Set the refresh rate counter */
        /* Set the device refresh rate */
        HAL_SDRAM_ProgramRefreshRate(&sdramHandle, RefreshCount);
    }
    void BSP_SDRAM_MspInit(SDRAM_HandleTypeDef* hsdram, void* Params)
    {

        UNUSED(Params);

        static DMA_HandleTypeDef dma_handle;
        GPIO_InitTypeDef gpio_init_structure;

        /* Enable FMC clock */
        __HAL_RCC_FMC_CLK_ENABLE();

        /* Enable chosen DMAx clock */
        __DMAx_CLK_ENABLE();

        /* Enable GPIOs clock */
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        __HAL_RCC_GPIOI_CLK_ENABLE();

        /* Common GPIO configuration */
        gpio_init_structure.Mode = GPIO_MODE_AF_PP;
        gpio_init_structure.Pull = GPIO_PULLUP;
        gpio_init_structure.Speed = GPIO_SPEED_HIGH;
        gpio_init_structure.Alternate = GPIO_AF12_FMC;

        /* GPIOD configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
            GPIO_PIN_14 | GPIO_PIN_15;


        HAL_GPIO_Init(GPIOD, &gpio_init_structure);

        /* GPIOE configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | \
            GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | \
            GPIO_PIN_15;

        HAL_GPIO_Init(GPIOE, &gpio_init_structure);

        /* GPIOF configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | \
            GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | \
            GPIO_PIN_15;

        HAL_GPIO_Init(GPIOF, &gpio_init_structure);

        /* GPIOG configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
            GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
        HAL_GPIO_Init(GPIOG, &gpio_init_structure);

        /* GPIOH configuration */
        gpio_init_structure.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | \
            GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | \
            GPIO_PIN_15;
        HAL_GPIO_Init(GPIOH, &gpio_init_structure);

        /* GPIOI configuration */
        gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | \
            GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
        HAL_GPIO_Init(GPIOI, &gpio_init_structure);


        dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
        /* Configure common DMA parameters */
        dma_handle.Init.Channel = SDRAM_DMAx_CHANNEL;
        dma_handle.Init.Direction = DMA_MEMORY_TO_MEMORY;
        dma_handle.Init.PeriphInc = DMA_PINC_ENABLE;
        dma_handle.Init.MemInc = DMA_MINC_ENABLE;
        dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        dma_handle.Init.Mode = DMA_NORMAL;
        dma_handle.Init.Priority = DMA_PRIORITY_HIGH;
        dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;
        dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;

        dma_handle.Instance = SDRAM_DMAx_STREAM;

        /* Associate the DMA handle */
        __HAL_LINKDMA(hsdram, hdma, dma_handle);

        /* Deinitialize the stream for new transfer */
        HAL_DMA_DeInit(&dma_handle);

        /* Configure the DMA stream */
        HAL_DMA_Init(&dma_handle);
        uint32_t prioritygroup = __NVIC_GetPriorityGrouping();
        NVIC_SetPriority(SDRAM_DMAx_IRQn, NVIC_EncodePriority(prioritygroup, 0x0F, 0));  /* NVIC configuration for DMA transfer complete interrupt */
        __NVIC_EnableIRQ(SDRAM_DMAx_IRQn);
    }
    uint8_t BSP_SDRAM_Init(void)
    {
        static uint8_t sdramstatus = SDRAM_ERROR;
        /* SDRAM device configuration */
        sdramHandle.Instance = FMC_SDRAM_DEVICE;

        /* Timing configuration for 100Mhz as SDRAM clock frequency (System clock is up to 200Mhz) */
        Timing.LoadToActiveDelay = 2;
        Timing.ExitSelfRefreshDelay = 7;
        Timing.SelfRefreshTime = 4;
        Timing.RowCycleDelay = 7;
        Timing.WriteRecoveryTime = 2;
        Timing.RPDelay = 2;
        Timing.RCDDelay = 2;

        sdramHandle.Init.SDBank = FMC_SDRAM_BANK1;
        sdramHandle.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
        sdramHandle.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
        sdramHandle.Init.MemoryDataWidth = SDRAM_MEMORY_WIDTH;
        sdramHandle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
        sdramHandle.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
        sdramHandle.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
        sdramHandle.Init.SDClockPeriod = SDCLOCK_PERIOD;
        sdramHandle.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
        sdramHandle.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;

        /* SDRAM controller initialization */

        BSP_SDRAM_MspInit(&sdramHandle, NULL); /* __weak function can be rewritten by the application */

        if (HAL_SDRAM_Init(&sdramHandle, &Timing) != HAL_OK)
        {
            sdramstatus = SDRAM_ERROR;
        }
        else
        {
            sdramstatus = SDRAM_OK;
        }

        /* SDRAM initialization sequence */
        BSP_SDRAM_Initialization_sequence(REFRESH_COUNT);

        return sdramstatus;
    }
    void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
    {
        UNUSED(hltdc);
        GPIO_InitTypeDef GPIO_Init_Structure;

        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable the LTDC Clock */
        __HAL_RCC_LTDC_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /******************** LTDC Pins configuration *************************/
        /* Enable GPIOs clock */
        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_GPIOI_CLK_ENABLE();
        __HAL_RCC_GPIOJ_CLK_ENABLE();
        __HAL_RCC_GPIOK_CLK_ENABLE();

        /*** LTDC Pins configuration ***/
        /* GPIOE configuration */
        GPIO_Init_Structure.Pin = GPIO_PIN_4;
        GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
        GPIO_Init_Structure.Pull = GPIO_NOPULL;
        GPIO_Init_Structure.Speed = GPIO_SPEED_FAST;
        GPIO_Init_Structure.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);

        /* GPIOG configuration */
        GPIO_Init_Structure.Pin = GPIO_PIN_12;
        GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
        GPIO_Init_Structure.Alternate = GPIO_AF9_LTDC;
        HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);

        /* GPIOI LTDC alternate configuration */
        GPIO_Init_Structure.Pin = GPIO_PIN_9 | GPIO_PIN_10 | \
            GPIO_PIN_14 | GPIO_PIN_15;
        GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
        GPIO_Init_Structure.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);

        /* GPIOJ configuration */
        GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
            GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
            GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
            GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
        GPIO_Init_Structure.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOJ, &GPIO_Init_Structure);

        /* GPIOK configuration */
        GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
            GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
        GPIO_Init_Structure.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOK, &GPIO_Init_Structure);

        /* LCD_DISP GPIO configuration */
        GPIO_Init_Structure.Pin = GPIO_PIN_12;     /* LCD_DISP pin has to be manually controlled */
        GPIO_Init_Structure.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);

        /* LCD_BL_CTRL GPIO configuration */
        GPIO_Init_Structure.Pin = GPIO_PIN_3;  /* LCD_BL_CTRL pin has to be manually controlled */
        GPIO_Init_Structure.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(GPIOK, &GPIO_Init_Structure);


        /* Assert display enable LCD_DISP pin */
        HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);

        /* Assert backlight LCD_BL_CTRL pin */
        HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

    }
    static uint32_t DMA_CalcBaseAndBitshift(DMA_HandleTypeDef* hdma)
    {
        uint32_t stream_number = (((uint32_t)hdma->Instance & 0xFFU) - 16U) / 24U;

        /* lookup table for necessary bitshift of flags within status registers */
        static const uint8_t flagBitshiftOffset[8U] = { 0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U };
        hdma->StreamIndex = flagBitshiftOffset[stream_number];

        if (stream_number > 3U)
        {
            /* return pointer to HISR and HIFCR */
            hdma->StreamBaseAddress = (((uint32_t)hdma->Instance & (uint32_t)(~0x3FFU)) + 4U);
        }
        else
        {
            /* return pointer to LISR and LIFCR */
            hdma->StreamBaseAddress = ((uint32_t)hdma->Instance & (uint32_t)(~0x3FFU));
        }

        return hdma->StreamBaseAddress;
    }
    static HAL_StatusTypeDef DMA_CheckFifoParam(DMA_HandleTypeDef* hdma)
    {
        HAL_StatusTypeDef status = HAL_OK;
        uint32_t tmp = hdma->Init.FIFOThreshold;

        /* Memory Data size equal to Byte */
        if (hdma->Init.MemDataAlignment == DMA_MDATAALIGN_BYTE)
        {
            switch (tmp)
            {
            case DMA_FIFO_THRESHOLD_1QUARTERFULL:
            case DMA_FIFO_THRESHOLD_3QUARTERSFULL:
                if ((hdma->Init.MemBurst & DMA_SxCR_MBURST_1) == DMA_SxCR_MBURST_1)
                {
                    status = HAL_ERROR;
                }
                break;
            case DMA_FIFO_THRESHOLD_HALFFULL:
                if (hdma->Init.MemBurst == DMA_MBURST_INC16)
                {
                    status = HAL_ERROR;
                }
                break;
            case DMA_FIFO_THRESHOLD_FULL:
                break;
            default:
                break;
            }
        }

        /* Memory Data size equal to Half-Word */
        else if (hdma->Init.MemDataAlignment == DMA_MDATAALIGN_HALFWORD)
        {
            switch (tmp)
            {
            case DMA_FIFO_THRESHOLD_1QUARTERFULL:
            case DMA_FIFO_THRESHOLD_3QUARTERSFULL:
                status = HAL_ERROR;
                break;
            case DMA_FIFO_THRESHOLD_HALFFULL:
                if ((hdma->Init.MemBurst & DMA_SxCR_MBURST_1) == DMA_SxCR_MBURST_1)
                {
                    status = HAL_ERROR;
                }
                break;
            case DMA_FIFO_THRESHOLD_FULL:
                if (hdma->Init.MemBurst == DMA_MBURST_INC16)
                {
                    status = HAL_ERROR;
                }
                break;
            default:
                break;
            }
        }

        /* Memory Data size equal to Word */
        else
        {
            switch (tmp)
            {
            case DMA_FIFO_THRESHOLD_1QUARTERFULL:
            case DMA_FIFO_THRESHOLD_HALFFULL:
            case DMA_FIFO_THRESHOLD_3QUARTERSFULL:
                status = HAL_ERROR;
                break;
            case DMA_FIFO_THRESHOLD_FULL:
                if ((hdma->Init.MemBurst & DMA_SxCR_MBURST_1) == DMA_SxCR_MBURST_1)
                {
                    status = HAL_ERROR;
                }
                break;
            default:
                break;
            }
        }

        return status;
    }
    HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef* hdma)
    {
        DMA_Base_Registers* regs;

        /* Check the DMA peripheral state */
        if (hdma == NULL)
        {
            return HAL_ERROR;
        }

        /* Check the DMA peripheral state */
        if (hdma->State == HAL_DMA_STATE_BUSY)
        {
            /* Return error status */
            return HAL_BUSY;
        }

        /* Check the parameters */
        assert_param(IS_DMA_STREAM_ALL_INSTANCE(hdma->Instance));

        /* Disable the selected DMA Streamx */
        __HAL_DMA_DISABLE(hdma);

        /* Reset DMA Streamx control register */
        hdma->Instance->CR = 0U;

        /* Reset DMA Streamx number of data to transfer register */
        hdma->Instance->NDTR = 0U;

        /* Reset DMA Streamx peripheral address register */
        hdma->Instance->PAR = 0U;

        /* Reset DMA Streamx memory 0 address register */
        hdma->Instance->M0AR = 0U;

        /* Reset DMA Streamx memory 1 address register */
        hdma->Instance->M1AR = 0U;

        /* Reset DMA Streamx FIFO control register */
        hdma->Instance->FCR = (uint32_t)0x00000021U;

        /* Get DMA steam Base Address */
        regs = (DMA_Base_Registers*)DMA_CalcBaseAndBitshift(hdma);

        /* Clear all interrupt flags at correct offset within the register */
        regs->IFCR = 0x3FU << hdma->StreamIndex;

        /* Clean all callbacks */
        hdma->XferCpltCallback = NULL;
        hdma->XferHalfCpltCallback = NULL;
        hdma->XferM1CpltCallback = NULL;
        hdma->XferM1HalfCpltCallback = NULL;
        hdma->XferErrorCallback = NULL;
        hdma->XferAbortCallback = NULL;

        /* Reset the error code */
        hdma->ErrorCode = HAL_DMA_ERROR_NONE;

        /* Reset the DMA state */
        hdma->State = HAL_DMA_STATE_RESET;

        /* Release Lock */
        __HAL_UNLOCK(hdma);

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef* hdma)
    {
        uint32_t tmp = 0U;
        uint32_t tickstart = HAL_GetTick();
        DMA_Base_Registers* regs;

        /* Check the DMA peripheral state */
        if (hdma == NULL)
        {
            return HAL_ERROR;
        }

        /* Check the parameters */
        assert_param(IS_DMA_STREAM_ALL_INSTANCE(hdma->Instance));
        assert_param(IS_DMA_CHANNEL(hdma->Init.Channel));
        assert_param(IS_DMA_DIRECTION(hdma->Init.Direction));
        assert_param(IS_DMA_PERIPHERAL_INC_STATE(hdma->Init.PeriphInc));
        assert_param(IS_DMA_MEMORY_INC_STATE(hdma->Init.MemInc));
        assert_param(IS_DMA_PERIPHERAL_DATA_SIZE(hdma->Init.PeriphDataAlignment));
        assert_param(IS_DMA_MEMORY_DATA_SIZE(hdma->Init.MemDataAlignment));
        assert_param(IS_DMA_MODE(hdma->Init.Mode));
        assert_param(IS_DMA_PRIORITY(hdma->Init.Priority));
        assert_param(IS_DMA_FIFO_MODE_STATE(hdma->Init.FIFOMode));
        /* Check the memory burst, peripheral burst and FIFO threshold parameters only
           when FIFO mode is enabled */
        if (hdma->Init.FIFOMode != DMA_FIFOMODE_DISABLE)
        {
            assert_param(IS_DMA_FIFO_THRESHOLD(hdma->Init.FIFOThreshold));
            assert_param(IS_DMA_MEMORY_BURST(hdma->Init.MemBurst));
            assert_param(IS_DMA_PERIPHERAL_BURST(hdma->Init.PeriphBurst));
        }

        /* Allocate lock resource */
        __HAL_UNLOCK(hdma);

        /* Change DMA peripheral state */
        hdma->State = HAL_DMA_STATE_BUSY;

        /* Disable the peripheral */
        __HAL_DMA_DISABLE(hdma);

        /* Check if the DMA Stream is effectively disabled */
        while ((hdma->Instance->CR & DMA_SxCR_EN) != RESET)
        {
            /* Check for the Timeout */
            if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
            {
                /* Update error code */
                hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

                /* Change the DMA state */
                hdma->State = HAL_DMA_STATE_TIMEOUT;

                return HAL_TIMEOUT;
            }
        }

        /* Get the CR register value */
        tmp = hdma->Instance->CR;

        /* Clear CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC, DIR, CT and DBM bits */
        tmp &= ((uint32_t)~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST | \
            DMA_SxCR_PL | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE | \
            DMA_SxCR_MINC | DMA_SxCR_PINC | DMA_SxCR_CIRC | \
            DMA_SxCR_DIR | DMA_SxCR_CT | DMA_SxCR_DBM));

        /* Prepare the DMA Stream configuration */
        tmp |= hdma->Init.Channel | hdma->Init.Direction |
            hdma->Init.PeriphInc | hdma->Init.MemInc |
            hdma->Init.PeriphDataAlignment | hdma->Init.MemDataAlignment |
            hdma->Init.Mode | hdma->Init.Priority;

        /* the Memory burst and peripheral burst are not used when the FIFO is disabled */
        if (hdma->Init.FIFOMode == DMA_FIFOMODE_ENABLE)
        {
            /* Get memory burst and peripheral burst */
            tmp |= hdma->Init.MemBurst | hdma->Init.PeriphBurst;
        }

        /* Write to DMA Stream CR register */
        hdma->Instance->CR = tmp;

        /* Get the FCR register value */
        tmp = hdma->Instance->FCR;

        /* Clear Direct mode and FIFO threshold bits */
        tmp &= (uint32_t)~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);

        /* Prepare the DMA Stream FIFO configuration */
        tmp |= hdma->Init.FIFOMode;

        /* The FIFO threshold is not used when the FIFO mode is disabled */
        if (hdma->Init.FIFOMode == DMA_FIFOMODE_ENABLE)
        {
            /* Get the FIFO threshold */
            tmp |= hdma->Init.FIFOThreshold;

            /* Check compatibility between FIFO threshold level and size of the memory burst */
            /* for INCR4, INCR8, INCR16 bursts */
            if (hdma->Init.MemBurst != DMA_MBURST_SINGLE)
            {
                if (DMA_CheckFifoParam(hdma) != HAL_OK)
                {
                    /* Update error code */
                    hdma->ErrorCode = HAL_DMA_ERROR_PARAM;

                    /* Change the DMA state */
                    hdma->State = HAL_DMA_STATE_READY;

                    return HAL_ERROR;
                }
            }
        }

        /* Write to DMA Stream FCR */
        hdma->Instance->FCR = tmp;

        /* Initialize StreamBaseAddress and StreamIndex parameters to be used to calculate
           DMA steam Base Address needed by HAL_DMA_IRQHandler() and HAL_DMA_PollForTransfer() */
        regs = (DMA_Base_Registers*)DMA_CalcBaseAndBitshift(hdma);

        /* Clear all interrupt flags */
        regs->IFCR = 0x3FU << hdma->StreamIndex;

        /* Initialize the error code */
        hdma->ErrorCode = HAL_DMA_ERROR_NONE;

        /* Initialize the DMA state */
        hdma->State = HAL_DMA_STATE_READY;

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_LTDC_Init(LTDC_HandleTypeDef* hltdc)
    {
        uint32_t tmp, tmp1;

        /* Check the LTDC peripheral state */
        if (hltdc == NULL)
        {
            return HAL_ERROR;
        }


        /* Check function parameters */
        assert_param(IS_LTDC_ALL_INSTANCE(hltdc->Instance));
        assert_param(IS_LTDC_HSYNC(hltdc->Init.HorizontalSync));
        assert_param(IS_LTDC_VSYNC(hltdc->Init.VerticalSync));
        assert_param(IS_LTDC_AHBP(hltdc->Init.AccumulatedHBP));
        assert_param(IS_LTDC_AVBP(hltdc->Init.AccumulatedVBP));
        assert_param(IS_LTDC_AAH(hltdc->Init.AccumulatedActiveH));
        assert_param(IS_LTDC_AAW(hltdc->Init.AccumulatedActiveW));
        assert_param(IS_LTDC_TOTALH(hltdc->Init.TotalHeigh));
        assert_param(IS_LTDC_TOTALW(hltdc->Init.TotalWidth));
        assert_param(IS_LTDC_HSPOL(hltdc->Init.HSPolarity));
        assert_param(IS_LTDC_VSPOL(hltdc->Init.VSPolarity));
        assert_param(IS_LTDC_DEPOL(hltdc->Init.DEPolarity));
        assert_param(IS_LTDC_PCPOL(hltdc->Init.PCPolarity));

        //#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
        //        if (hltdc->State == HAL_LTDC_STATE_RESET)
        //        {
        //            /* Allocate lock resource and initialize it */
        //            hltdc->Lock = HAL_UNLOCKED;
        //
        //            /* Reset the LTDC callback to the legacy weak callbacks */
        //            hltdc->LineEventCallback = HAL_LTDC_LineEventCallback;    /* Legacy weak LineEventCallback    */
        //            hltdc->ReloadEventCallback = HAL_LTDC_ReloadEventCallback;  /* Legacy weak ReloadEventCallback  */
        //            hltdc->ErrorCallback = HAL_LTDC_ErrorCallback;        /* Legacy weak ErrorCallback        */
        //
        //            if (hltdc->MspInitCallback == NULL)
        //            {
        //                hltdc->MspInitCallback = HAL_LTDC_MspInit;
        //            }
        //            /* Init the low level hardware */
        //            hltdc->MspInitCallback(hltdc);
        //        }
        //#else
        if (hltdc->State == HAL_LTDC_STATE_RESET)
        {
            /* Allocate lock resource and initialize it */
            hltdc->Lock = HAL_UNLOCKED;
            /* Init the low level hardware */
            HAL_LTDC_MspInit(hltdc);
        }
        //#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */

                /* Change LTDC peripheral state */
        hltdc->State = HAL_LTDC_STATE_BUSY;

        /* Configure the HS, VS, DE and PC polarity */
        hltdc->Instance->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
        hltdc->Instance->GCR |= (uint32_t)(hltdc->Init.HSPolarity | hltdc->Init.VSPolarity | \
            hltdc->Init.DEPolarity | hltdc->Init.PCPolarity);

        /* Set Synchronization size */
        hltdc->Instance->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
        tmp = (hltdc->Init.HorizontalSync << 16U);
        hltdc->Instance->SSCR |= (tmp | hltdc->Init.VerticalSync);

        /* Set Accumulated Back porch */
        hltdc->Instance->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
        tmp = (hltdc->Init.AccumulatedHBP << 16U);
        hltdc->Instance->BPCR |= (tmp | hltdc->Init.AccumulatedVBP);

        /* Set Accumulated Active Width */
        hltdc->Instance->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
        tmp = (hltdc->Init.AccumulatedActiveW << 16U);
        hltdc->Instance->AWCR |= (tmp | hltdc->Init.AccumulatedActiveH);

        /* Set Total Width */
        hltdc->Instance->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
        tmp = (hltdc->Init.TotalWidth << 16U);
        hltdc->Instance->TWCR |= (tmp | hltdc->Init.TotalHeigh);

        /* Set the background color value */
        tmp = ((uint32_t)(hltdc->Init.Backcolor.Green) << 8U);
        tmp1 = ((uint32_t)(hltdc->Init.Backcolor.Red) << 16U);
        hltdc->Instance->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
        hltdc->Instance->BCCR |= (tmp1 | tmp | hltdc->Init.Backcolor.Blue);

        /* Enable the Transfer Error and FIFO underrun interrupts */
        __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_TE | LTDC_IT_FU);

        /* Enable LTDC by setting LTDCEN bit */
        __HAL_LTDC_ENABLE(hltdc);

        /* Initialize the error code */
        hltdc->ErrorCode = HAL_LTDC_ERROR_NONE;

        /* Initialize the LTDC state*/
        hltdc->State = HAL_LTDC_STATE_READY;

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_DSI_Start(DSI_HandleTypeDef* hdsi)
    {
        /* Process locked */
        __HAL_LOCK(hdsi);

        /* Enable the DSI host */
        __HAL_DSI_ENABLE(hdsi);

        /* Enable the DSI wrapper */
        __HAL_DSI_WRAPPER_ENABLE(hdsi);

        /* Process unlocked */
        __HAL_UNLOCK(hdsi);

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_LTDC_ConfigLayer(LTDC_HandleTypeDef* hltdc, LTDC_LayerCfgTypeDef* pLayerCfg, uint32_t LayerIdx)
    {

        /* Process locked */
        __HAL_LOCK(hltdc);

        /* Change LTDC peripheral state */
        hltdc->State = HAL_LTDC_STATE_BUSY;

        /* Copy new layer configuration into handle structure */
        hltdc->LayerCfg[LayerIdx] = *pLayerCfg;

        /* Configure the LTDC Layer */
        LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

        /* Set the Immediate Reload type */
        hltdc->Instance->SRCR = LTDC_SRCR_IMR;

        /* Initialize the LTDC state*/
        hltdc->State = HAL_LTDC_STATE_READY;

        /* Process unlocked */
        __HAL_UNLOCK(hltdc);

        return HAL_OK;
    }
    static void LTDC_SetConfig(LTDC_HandleTypeDef* hltdc, LTDC_LayerCfgTypeDef* pLayerCfg, uint32_t LayerIdx)
    {
        uint32_t tmp;
        uint32_t tmp1;
        uint32_t tmp2;

        /* Configure the horizontal start and stop position */
        tmp = ((pLayerCfg->WindowX1 + ((hltdc->Instance->BPCR & LTDC_BPCR_AHBP) >> 16U)) << 16U);
        LTDC_LAYER(hltdc, LayerIdx)->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
        LTDC_LAYER(hltdc, LayerIdx)->WHPCR = ((pLayerCfg->WindowX0 + ((hltdc->Instance->BPCR & LTDC_BPCR_AHBP) >> 16U) + 1U) | tmp);

        /* Configure the vertical start and stop position */
        tmp = ((pLayerCfg->WindowY1 + (hltdc->Instance->BPCR & LTDC_BPCR_AVBP)) << 16U);
        LTDC_LAYER(hltdc, LayerIdx)->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
        LTDC_LAYER(hltdc, LayerIdx)->WVPCR = ((pLayerCfg->WindowY0 + (hltdc->Instance->BPCR & LTDC_BPCR_AVBP) + 1U) | tmp);

        /* Specifies the pixel format */
        LTDC_LAYER(hltdc, LayerIdx)->PFCR &= ~(LTDC_LxPFCR_PF);
        LTDC_LAYER(hltdc, LayerIdx)->PFCR = (pLayerCfg->PixelFormat);

        /* Configure the default color values */
        tmp = ((uint32_t)(pLayerCfg->Backcolor.Green) << 8U);
        tmp1 = ((uint32_t)(pLayerCfg->Backcolor.Red) << 16U);
        tmp2 = (pLayerCfg->Alpha0 << 24U);
        LTDC_LAYER(hltdc, LayerIdx)->DCCR &= ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);
        LTDC_LAYER(hltdc, LayerIdx)->DCCR = (pLayerCfg->Backcolor.Blue | tmp | tmp1 | tmp2);

        /* Specifies the constant alpha value */
        LTDC_LAYER(hltdc, LayerIdx)->CACR &= ~(LTDC_LxCACR_CONSTA);
        LTDC_LAYER(hltdc, LayerIdx)->CACR = (pLayerCfg->Alpha);

        /* Specifies the blending factors */
        LTDC_LAYER(hltdc, LayerIdx)->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
        LTDC_LAYER(hltdc, LayerIdx)->BFCR = (pLayerCfg->BlendingFactor1 | pLayerCfg->BlendingFactor2);

        /* Configure the color frame buffer start address */
        LTDC_LAYER(hltdc, LayerIdx)->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
        LTDC_LAYER(hltdc, LayerIdx)->CFBAR = (pLayerCfg->FBStartAdress);

        if (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
        {
            tmp = 4U;
        }
        else if (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
        {
            tmp = 3U;
        }
        else if ((pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
            (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
            (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB1555) || \
            (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_AL88))
        {
            tmp = 2U;
        }
        else
        {
            tmp = 1U;
        }

        /* Configure the color frame buffer pitch in byte */
        LTDC_LAYER(hltdc, LayerIdx)->CFBLR &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
        LTDC_LAYER(hltdc, LayerIdx)->CFBLR = (((pLayerCfg->ImageWidth * tmp) << 16U) | (((pLayerCfg->WindowX1 - pLayerCfg->WindowX0) * tmp) + 3U));
        /* Configure the frame buffer line number */
        LTDC_LAYER(hltdc, LayerIdx)->CFBLNR &= ~(LTDC_LxCFBLNR_CFBLNBR);
        LTDC_LAYER(hltdc, LayerIdx)->CFBLNR = (pLayerCfg->ImageHeight);

        /* Enable LTDC_Layer by setting LEN bit */
        LTDC_LAYER(hltdc, LayerIdx)->CR |= (uint32_t)LTDC_LxCR_LEN;
    }
    void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
    {
        /* Check the parameters */
        assert_param(IS_GPIO_PIN(GPIO_Pin));
        assert_param(IS_GPIO_PIN_ACTION(PinState));

        if (PinState != GPIO_PIN_RESET)
        {
            GPIOx->BSRR = GPIO_Pin;
        }
        else
        {
            GPIOx->BSRR = (uint32_t)GPIO_Pin << 16;
        }
    }
    HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef* PeriphClkInit)
    {
        uint32_t tickstart = 0;
        uint32_t tmpreg0 = 0;
        uint32_t tmpreg1 = 0;
        uint32_t plli2sused = 0;
        uint32_t pllsaiused = 0;

        /* Check the parameters */
        assert_param(IS_RCC_PERIPHCLOCK(PeriphClkInit->PeriphClockSelection));

        /*-------------------------------------- I2C1 Configuration -----------------------------------*/
        if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C1) == RCC_PERIPHCLK_I2C1)
        {
            /* Check the parameters */
            assert_param(IS_RCC_I2C1CLKSOURCE(PeriphClkInit->I2c1ClockSelection));

            /* Configure the I2C1 clock source */
            __HAL_RCC_I2C1_CONFIG(PeriphClkInit->I2c1ClockSelection);
        }

        /*-------------------------------------- I2C2 Configuration -----------------------------------*/
        if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C2) == RCC_PERIPHCLK_I2C2)
        {
            /* Check the parameters */
            assert_param(IS_RCC_I2C2CLKSOURCE(PeriphClkInit->I2c2ClockSelection));

            /* Configure the I2C2 clock source */
            __HAL_RCC_I2C2_CONFIG(PeriphClkInit->I2c2ClockSelection);
        }

        /*-------------------------------------- I2C3 Configuration -----------------------------------*/
        if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C3) == RCC_PERIPHCLK_I2C3)
        {
            /* Check the parameters */
            assert_param(IS_RCC_I2C3CLKSOURCE(PeriphClkInit->I2c3ClockSelection));

            /* Configure the I2C3 clock source */
            __HAL_RCC_I2C3_CONFIG(PeriphClkInit->I2c3ClockSelection);
        }

        /*-------------------------------------- I2C4 Configuration -----------------------------------*/
        if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2C4) == RCC_PERIPHCLK_I2C4)
        {
            /* Check the parameters */
            assert_param(IS_RCC_I2C4CLKSOURCE(PeriphClkInit->I2c4ClockSelection));

            /* Configure the I2C4 clock source */
            __HAL_RCC_I2C4_CONFIG(PeriphClkInit->I2c4ClockSelection);
        }


        /*-------------------------------------- LTDC Configuration -----------------------------------*/
        if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LTDC) == RCC_PERIPHCLK_LTDC)
        {
            pllsaiused = 1;
        }




        /*-------------------------------------- PLLI2S Configuration ---------------------------------*/
        /* PLLI2S is configured when a peripheral will use it as source clock : SAI1, SAI2, I2S or SPDIF-RX */
        if ((plli2sused == 1) || (PeriphClkInit->PeriphClockSelection == RCC_PERIPHCLK_PLLI2S))
        {
            /* Disable the PLLI2S */
            __HAL_RCC_PLLI2S_DISABLE();

            /* Get Start Tick*/
            tickstart = HAL_GetTick();

            /* Wait till PLLI2S is disabled */
#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5) == 1)? RCC->CR :((((__FLAG__) >> 5) == 2) ? RCC->BDCR :((((__FLAG__) >> 5) == 3)? RCC->CSR :RCC->CIR))) & ((uint32_t)1 << ((__FLAG__) & RCC_FLAG_MASK)))!= 0)? 1 : 0)
            while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY) != RESET)
            {
                if ((HAL_GetTick() - tickstart) > PLLI2S_TIMEOUT_VALUE)
                {
                    /* return in case of Timeout detected */
                    return HAL_TIMEOUT;
                }
            }

            /* check for common PLLI2S Parameters */
            assert_param(IS_RCC_PLLI2SN_VALUE(PeriphClkInit->PLLI2S.PLLI2SN));

            /*----------------- In Case of PLLI2S is selected as source clock for I2S -------------------*/
            if (((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_I2S) == RCC_PERIPHCLK_I2S) && (PeriphClkInit->I2sClockSelection == RCC_I2SCLKSOURCE_PLLI2S)))
            {
                /* check for Parameters */
                assert_param(IS_RCC_PLLI2SR_VALUE(PeriphClkInit->PLLI2S.PLLI2SR));

                /* Read PLLI2SP and PLLI2SQ value from PLLI2SCFGR register (this value is not needed for I2S configuration) */
                tmpreg0 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SP) >> RCC_PLLI2SCFGR_PLLI2SP_Pos);
                tmpreg1 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ) >> RCC_PLLI2SCFGR_PLLI2SQ_Pos);
                /* Configure the PLLI2S division factors */
                /* PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) x (PLLI2SN/PLLM) */
                /* I2SCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SR */
                __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN, tmpreg0, tmpreg1, PeriphClkInit->PLLI2S.PLLI2SR);
            }

            /*----------------- In Case of PLLI2S is selected as source clock for SAI -------------------*/
            if (((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI1) == RCC_PERIPHCLK_SAI1) && (PeriphClkInit->Sai1ClockSelection == RCC_SAI1CLKSOURCE_PLLI2S)) ||
                ((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI2) == RCC_PERIPHCLK_SAI2) && (PeriphClkInit->Sai2ClockSelection == RCC_SAI2CLKSOURCE_PLLI2S)))
            {
                /* Check for PLLI2S Parameters */
                assert_param(IS_RCC_PLLI2SQ_VALUE(PeriphClkInit->PLLI2S.PLLI2SQ));
                /* Check for PLLI2S/DIVQ parameters */
                assert_param(IS_RCC_PLLI2S_DIVQ_VALUE(PeriphClkInit->PLLI2SDivQ));

                /* Read PLLI2SP and PLLI2SR values from PLLI2SCFGR register (this value is not needed for SAI configuration) */
                tmpreg0 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SP) >> RCC_PLLI2SCFGR_PLLI2SP_Pos);
                tmpreg1 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> RCC_PLLI2SCFGR_PLLI2SR_Pos);
                /* Configure the PLLI2S division factors */
                /* PLLI2S_VCO Input  = PLL_SOURCE/PLLM */
                /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN */
                /* SAI_CLK(first level) = PLLI2S_VCO Output/PLLI2SQ */
                __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN, tmpreg0, PeriphClkInit->PLLI2S.PLLI2SQ, tmpreg1);

                /* SAI_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ */
                __HAL_RCC_PLLI2S_PLLSAICLKDIVQ_CONFIG(PeriphClkInit->PLLI2SDivQ);
            }

            /*----------------- In Case of PLLI2S is selected as source clock for SPDIF-RX -------------------*/
            if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SPDIFRX) == RCC_PERIPHCLK_SPDIFRX)
            {
                /* check for Parameters */
                assert_param(IS_RCC_PLLI2SP_VALUE(PeriphClkInit->PLLI2S.PLLI2SP));

                /* Read PLLI2SR value from PLLI2SCFGR register (this value is not needed for SPDIF-RX configuration) */
                tmpreg0 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ) >> RCC_PLLI2SCFGR_PLLI2SQ_Pos);
                tmpreg1 = ((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> RCC_PLLI2SCFGR_PLLI2SR_Pos);
                /* Configure the PLLI2S division factors */
                /* PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) x (PLLI2SN/PLLM) */
                /* SPDIFCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SP */
                __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN, PeriphClkInit->PLLI2S.PLLI2SP, tmpreg0, tmpreg1);
            }

            /*----------------- In Case of PLLI2S is just selected  -----------------*/
            if ((PeriphClkInit->PeriphClockSelection & RCC_PERIPHCLK_PLLI2S) == RCC_PERIPHCLK_PLLI2S)
            {
                /* Check for Parameters */
                assert_param(IS_RCC_PLLI2SP_VALUE(PeriphClkInit->PLLI2S.PLLI2SP));
                assert_param(IS_RCC_PLLI2SR_VALUE(PeriphClkInit->PLLI2S.PLLI2SR));
                assert_param(IS_RCC_PLLI2SQ_VALUE(PeriphClkInit->PLLI2S.PLLI2SQ));

                /* Configure the PLLI2S division factors */
                /* PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input) x (PLLI2SN/PLLI2SM) */
                /* SPDIFRXCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SP */
                __HAL_RCC_PLLI2S_CONFIG(PeriphClkInit->PLLI2S.PLLI2SN, PeriphClkInit->PLLI2S.PLLI2SP, PeriphClkInit->PLLI2S.PLLI2SQ, PeriphClkInit->PLLI2S.PLLI2SR);
            }

            /* Enable the PLLI2S */
            __HAL_RCC_PLLI2S_ENABLE();

            /* Get Start Tick*/
            tickstart = HAL_GetTick();

            /* Wait till PLLI2S is ready */
            while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY) == RESET)
            {
                if ((HAL_GetTick() - tickstart) > PLLI2S_TIMEOUT_VALUE)
                {
                    /* return in case of Timeout detected */
                    return HAL_TIMEOUT;
                }
            }
        }

        /*-------------------------------------- PLLSAI Configuration ---------------------------------*/
        /* PLLSAI is configured when a peripheral will use it as source clock : SAI1, SAI2, LTDC or CK48 */
        if (pllsaiused == 1)
        {
            /* Disable PLLSAI Clock */
            __HAL_RCC_PLLSAI_DISABLE();

            /* Get Start Tick*/
            tickstart = HAL_GetTick();

            /* Wait till PLLSAI is disabled */
            while (__HAL_RCC_PLLSAI_GET_FLAG() != RESET)
            {
                if ((HAL_GetTick() - tickstart) > PLLSAI_TIMEOUT_VALUE)
                {
                    /* return in case of Timeout detected */
                    return HAL_TIMEOUT;
                }
            }

            /* Check the PLLSAI division factors */
            assert_param(IS_RCC_PLLSAIN_VALUE(PeriphClkInit->PLLSAI.PLLSAIN));

            /*----------------- In Case of PLLSAI is selected as source clock for SAI -------------------*/
            if (((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI1) == RCC_PERIPHCLK_SAI1) && (PeriphClkInit->Sai1ClockSelection == RCC_SAI1CLKSOURCE_PLLSAI)) || \
                ((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_SAI2) == RCC_PERIPHCLK_SAI2) && (PeriphClkInit->Sai2ClockSelection == RCC_SAI2CLKSOURCE_PLLSAI)))
            {
                /* check for PLLSAIQ Parameter */
                assert_param(IS_RCC_PLLSAIQ_VALUE(PeriphClkInit->PLLSAI.PLLSAIQ));
                /* check for PLLSAI/DIVQ Parameter */
                assert_param(IS_RCC_PLLSAI_DIVQ_VALUE(PeriphClkInit->PLLSAIDivQ));

                /* Read PLLSAIP value from PLLSAICFGR register (this value is not needed for SAI configuration) */
                tmpreg0 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIP) >> RCC_PLLSAICFGR_PLLSAIP_Pos);
                tmpreg1 = ((RCC->PLLSAICFGR & RCC_PLLI2SCFGR_PLLI2SR) >> RCC_PLLSAICFGR_PLLSAIR_Pos);
                /* PLLSAI_VCO Input  = PLL_SOURCE/PLLM */
                /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN */
                /* SAI_CLK(first level) = PLLSAI_VCO Output/PLLSAIQ */
                __HAL_RCC_PLLSAI_CONFIG(PeriphClkInit->PLLSAI.PLLSAIN, tmpreg0, PeriphClkInit->PLLSAI.PLLSAIQ, tmpreg1);

                /* SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ */
                __HAL_RCC_PLLSAI_PLLSAICLKDIVQ_CONFIG(PeriphClkInit->PLLSAIDivQ);
            }

            /*----------------- In Case of PLLSAI is selected as source clock for CLK48 -------------------*/
            /* In Case of PLLI2S is selected as source clock for CK48 */
            if ((((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_CLK48) == RCC_PERIPHCLK_CLK48) && (PeriphClkInit->Clk48ClockSelection == RCC_CLK48SOURCE_PLLSAIP))
            {
                /* check for Parameters */
                assert_param(IS_RCC_PLLSAIP_VALUE(PeriphClkInit->PLLSAI.PLLSAIP));
                /* Read PLLSAIQ and PLLSAIR value from PLLSAICFGR register (this value is not needed for CK48 configuration) */
                tmpreg0 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> RCC_PLLSAICFGR_PLLSAIQ_Pos);
                tmpreg1 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIR) >> RCC_PLLSAICFGR_PLLSAIR_Pos);

                /* Configure the PLLSAI division factors */
                /* PLLSAI_VCO = f(VCO clock) = f(PLLSAI clock input) x (PLLI2SN/PLLM) */
                /* 48CLK = f(PLLSAI clock output) = f(VCO clock) / PLLSAIP */
                __HAL_RCC_PLLSAI_CONFIG(PeriphClkInit->PLLSAI.PLLSAIN, PeriphClkInit->PLLSAI.PLLSAIP, tmpreg0, tmpreg1);
            }

            /*---------------------------- LTDC configuration -------------------------------*/
            if (((PeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_LTDC) == (RCC_PERIPHCLK_LTDC))
            {
                assert_param(IS_RCC_PLLSAIR_VALUE(PeriphClkInit->PLLSAI.PLLSAIR));
                assert_param(IS_RCC_PLLSAI_DIVR_VALUE(PeriphClkInit->PLLSAIDivR));

                /* Read PLLSAIP and PLLSAIQ value from PLLSAICFGR register (these value are not needed for LTDC configuration) */
                tmpreg0 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ) >> RCC_PLLSAICFGR_PLLSAIQ_Pos);
                tmpreg1 = ((RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIP) >> RCC_PLLSAICFGR_PLLSAIP_Pos);

                /* PLLSAI_VCO Input  = PLL_SOURCE/PLLM */
                /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN */
                /* LTDC_CLK(first level) = PLLSAI_VCO Output/PLLSAIR */
                __HAL_RCC_PLLSAI_CONFIG(PeriphClkInit->PLLSAI.PLLSAIN, tmpreg1, tmpreg0, PeriphClkInit->PLLSAI.PLLSAIR);

                /* LTDC_CLK = LTDC_CLK(first level)/PLLSAIDIVR */
                __HAL_RCC_PLLSAI_PLLSAICLKDIVR_CONFIG(PeriphClkInit->PLLSAIDivR);
            }

            /* Enable PLLSAI Clock */
            __HAL_RCC_PLLSAI_ENABLE();

            /* Get Start Tick*/
            tickstart = HAL_GetTick();

            /* Wait till PLLSAI is ready */
            while (__HAL_RCC_PLLSAI_GET_FLAG() == RESET)
            {
                if ((HAL_GetTick() - tickstart) > PLLSAI_TIMEOUT_VALUE)
                {
                    /* return in case of Timeout detected */
                    return HAL_TIMEOUT;
                }
            }
        }
        return HAL_OK;
    }
    void HAL_DSI_MspDeInit(DSI_HandleTypeDef* hdsi)
    {
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hdsi);
        /* NOTE : This function Should not be modified, when the callback is needed,
                  the HAL_DSI_MspDeInit could be implemented in the user file
         */
    }
    HAL_StatusTypeDef HAL_DSI_ConfigVideoMode(DSI_HandleTypeDef* hdsi, DSI_VidCfgTypeDef* VidCfg)
    {
        /* Process locked */
        __HAL_LOCK(hdsi);

        /* Check the parameters */
        assert_param(IS_DSI_COLOR_CODING(VidCfg->ColorCoding));
        assert_param(IS_DSI_VIDEO_MODE_TYPE(VidCfg->Mode));
        assert_param(IS_DSI_LP_COMMAND(VidCfg->LPCommandEnable));
        assert_param(IS_DSI_LP_HFP(VidCfg->LPHorizontalFrontPorchEnable));
        assert_param(IS_DSI_LP_HBP(VidCfg->LPHorizontalBackPorchEnable));
        assert_param(IS_DSI_LP_VACTIVE(VidCfg->LPVerticalActiveEnable));
        assert_param(IS_DSI_LP_VFP(VidCfg->LPVerticalFrontPorchEnable));
        assert_param(IS_DSI_LP_VBP(VidCfg->LPVerticalBackPorchEnable));
        assert_param(IS_DSI_LP_VSYNC(VidCfg->LPVerticalSyncActiveEnable));
        assert_param(IS_DSI_FBTAA(VidCfg->FrameBTAAcknowledgeEnable));
        assert_param(IS_DSI_DE_POLARITY(VidCfg->DEPolarity));
        assert_param(IS_DSI_VSYNC_POLARITY(VidCfg->VSPolarity));
        assert_param(IS_DSI_HSYNC_POLARITY(VidCfg->HSPolarity));
        /* Check the LooselyPacked variant only in 18-bit mode */
        if (VidCfg->ColorCoding == DSI_RGB666)
        {
            assert_param(IS_DSI_LOOSELY_PACKED(VidCfg->LooselyPacked));
        }

        /* Select video mode by resetting CMDM and DSIM bits */
        hdsi->Instance->MCR &= ~DSI_MCR_CMDM;
        hdsi->Instance->WCFGR &= ~DSI_WCFGR_DSIM;

        /* Configure the video mode transmission type */
        hdsi->Instance->VMCR &= ~DSI_VMCR_VMT;
        hdsi->Instance->VMCR |= VidCfg->Mode;

        /* Configure the video packet size */
        hdsi->Instance->VPCR &= ~DSI_VPCR_VPSIZE;
        hdsi->Instance->VPCR |= VidCfg->PacketSize;

        /* Set the chunks number to be transmitted through the DSI link */
        hdsi->Instance->VCCR &= ~DSI_VCCR_NUMC;
        hdsi->Instance->VCCR |= VidCfg->NumberOfChunks;

        /* Set the size of the null packet */
        hdsi->Instance->VNPCR &= ~DSI_VNPCR_NPSIZE;
        hdsi->Instance->VNPCR |= VidCfg->NullPacketSize;

        /* Select the virtual channel for the LTDC interface traffic */
        hdsi->Instance->LVCIDR &= ~DSI_LVCIDR_VCID;
        hdsi->Instance->LVCIDR |= VidCfg->VirtualChannelID;

        /* Configure the polarity of control signals */
        hdsi->Instance->LPCR &= ~(DSI_LPCR_DEP | DSI_LPCR_VSP | DSI_LPCR_HSP);
        hdsi->Instance->LPCR |= (VidCfg->DEPolarity | VidCfg->VSPolarity | VidCfg->HSPolarity);

        /* Select the color coding for the host */
        hdsi->Instance->LCOLCR &= ~DSI_LCOLCR_COLC;
        hdsi->Instance->LCOLCR |= VidCfg->ColorCoding;

        /* Select the color coding for the wrapper */
        hdsi->Instance->WCFGR &= ~DSI_WCFGR_COLMUX;
        hdsi->Instance->WCFGR |= ((VidCfg->ColorCoding) << 1U);

        /* Enable/disable the loosely packed variant to 18-bit configuration */
        if (VidCfg->ColorCoding == DSI_RGB666)
        {
            hdsi->Instance->LCOLCR &= ~DSI_LCOLCR_LPE;
            hdsi->Instance->LCOLCR |= VidCfg->LooselyPacked;
        }

        /* Set the Horizontal Synchronization Active (HSA) in lane byte clock cycles */
        hdsi->Instance->VHSACR &= ~DSI_VHSACR_HSA;
        hdsi->Instance->VHSACR |= VidCfg->HorizontalSyncActive;

        /* Set the Horizontal Back Porch (HBP) in lane byte clock cycles */
        hdsi->Instance->VHBPCR &= ~DSI_VHBPCR_HBP;
        hdsi->Instance->VHBPCR |= VidCfg->HorizontalBackPorch;

        /* Set the total line time (HLINE=HSA+HBP+HACT+HFP) in lane byte clock cycles */
        hdsi->Instance->VLCR &= ~DSI_VLCR_HLINE;
        hdsi->Instance->VLCR |= VidCfg->HorizontalLine;

        /* Set the Vertical Synchronization Active (VSA) */
        hdsi->Instance->VVSACR &= ~DSI_VVSACR_VSA;
        hdsi->Instance->VVSACR |= VidCfg->VerticalSyncActive;

        /* Set the Vertical Back Porch (VBP)*/
        hdsi->Instance->VVBPCR &= ~DSI_VVBPCR_VBP;
        hdsi->Instance->VVBPCR |= VidCfg->VerticalBackPorch;

        /* Set the Vertical Front Porch (VFP)*/
        hdsi->Instance->VVFPCR &= ~DSI_VVFPCR_VFP;
        hdsi->Instance->VVFPCR |= VidCfg->VerticalFrontPorch;

        /* Set the Vertical Active period*/
        hdsi->Instance->VVACR &= ~DSI_VVACR_VA;
        hdsi->Instance->VVACR |= VidCfg->VerticalActive;

        /* Configure the command transmission mode */
        hdsi->Instance->VMCR &= ~DSI_VMCR_LPCE;
        hdsi->Instance->VMCR |= VidCfg->LPCommandEnable;

        /* Low power largest packet size */
        hdsi->Instance->LPMCR &= ~DSI_LPMCR_LPSIZE;
        hdsi->Instance->LPMCR |= ((VidCfg->LPLargestPacketSize) << 16U);

        /* Low power VACT largest packet size */
        hdsi->Instance->LPMCR &= ~DSI_LPMCR_VLPSIZE;
        hdsi->Instance->LPMCR |= VidCfg->LPVACTLargestPacketSize;

        /* Enable LP transition in HFP period */
        hdsi->Instance->VMCR &= ~DSI_VMCR_LPHFPE;
        hdsi->Instance->VMCR |= VidCfg->LPHorizontalFrontPorchEnable;

        /* Enable LP transition in HBP period */
        hdsi->Instance->VMCR &= ~DSI_VMCR_LPHBPE;
        hdsi->Instance->VMCR |= VidCfg->LPHorizontalBackPorchEnable;

        /* Enable LP transition in VACT period */
        hdsi->Instance->VMCR &= ~DSI_VMCR_LPVAE;
        hdsi->Instance->VMCR |= VidCfg->LPVerticalActiveEnable;

        /* Enable LP transition in VFP period */
        hdsi->Instance->VMCR &= ~DSI_VMCR_LPVFPE;
        hdsi->Instance->VMCR |= VidCfg->LPVerticalFrontPorchEnable;

        /* Enable LP transition in VBP period */
        hdsi->Instance->VMCR &= ~DSI_VMCR_LPVBPE;
        hdsi->Instance->VMCR |= VidCfg->LPVerticalBackPorchEnable;

        /* Enable LP transition in vertical sync period */
        hdsi->Instance->VMCR &= ~DSI_VMCR_LPVSAE;
        hdsi->Instance->VMCR |= VidCfg->LPVerticalSyncActiveEnable;

        /* Enable the request for an acknowledge response at the end of a frame */
        hdsi->Instance->VMCR &= ~DSI_VMCR_FBTAAE;
        hdsi->Instance->VMCR |= VidCfg->FrameBTAAcknowledgeEnable;

        /* Process unlocked */
        __HAL_UNLOCK(hdsi);

        return HAL_OK;
    }
    void HAL_DSI_MspInit()
    {
        /* Prevent unused argument(s) compilation warning */
        //UNUSED(hdsi);
        /* NOTE : This function Should not be modified, when the callback is needed,
                  the HAL_DSI_MspInit could be implemented in the user file
         */
    }
    void HAL_NVIC_DisableIRQ(IRQn_Type IRQn)
    {

        /* Disable interrupt */
        NVIC_DisableIRQ(IRQn);
    }
    void BSP_LCD_MspDeInit(void)
    {
        /** @brief Disable IRQ of LTDC IP */
        HAL_NVIC_DisableIRQ(LTDC_IRQn);

        /** @brief Disable IRQ of DMA2D IP */
        HAL_NVIC_DisableIRQ(DMA2D_IRQn);

        /** @brief Disable IRQ of DSI IP */
        HAL_NVIC_DisableIRQ(DSI_IRQn);


        /** @brief Force and let in reset state LTDC, DMA2D and DSI Host + Wrapper IPs */
        __HAL_RCC_LTDC_FORCE_RESET();
        __HAL_RCC_DMA2D_FORCE_RESET();
        __HAL_RCC_DSI_FORCE_RESET();

        /** @brief Disable the LTDC, DMA2D and DSI Host and Wrapper clocks */
        __HAL_RCC_LTDC_CLK_DISABLE();
        __HAL_RCC_DMA2D_CLK_DISABLE();
        __HAL_RCC_DSI_CLK_DISABLE();
    }
    HAL_StatusTypeDef HAL_DSI_Init(DSI_HandleTypeDef* hdsi, DSI_PLLInitTypeDef* PLLInit)
    {
        uint32_t tickstart;
        uint32_t unitIntervalx4;
        uint32_t tempIDF;

        /* Check the DSI handle allocation */
        if (hdsi == NULL)
        {
            return HAL_ERROR;
        }

        /* Check function parameters */
        assert_param(IS_DSI_PLL_NDIV(PLLInit->PLLNDIV));
        assert_param(IS_DSI_PLL_IDF(PLLInit->PLLIDF));
        assert_param(IS_DSI_PLL_ODF(PLLInit->PLLODF));
        assert_param(IS_DSI_AUTO_CLKLANE_CONTROL(hdsi->Init.AutomaticClockLaneControl));
        assert_param(IS_DSI_NUMBER_OF_LANES(hdsi->Init.NumberOfLanes));

        //#if (USE_HAL_DSI_REGISTER_CALLBACKS == 1)
        //    if (hdsi->State == HAL_DSI_STATE_RESET)
        //    {
        //        /* Reset the DSI callback to the legacy weak callbacks */
        //        hdsi->TearingEffectCallback = HAL_DSI_TearingEffectCallback; /* Legacy weak TearingEffectCallback */
        //        hdsi->EndOfRefreshCallback = HAL_DSI_EndOfRefreshCallback;  /* Legacy weak EndOfRefreshCallback  */
        //        hdsi->ErrorCallback = HAL_DSI_ErrorCallback;         /* Legacy weak ErrorCallback         */
        //
        //        if (hdsi->MspInitCallback == NULL)
        //        {
        //            hdsi->MspInitCallback = HAL_DSI_MspInit;
        //        }
        //        /* Initialize the low level hardware */
        //        hdsi->MspInitCallback(hdsi);
        //    }
        //#else
        if (hdsi->State == HAL_DSI_STATE_RESET)
        {
            /* Initialize the low level hardware */
            HAL_DSI_MspInit();
        }
        //#endif /* USE_HAL_DSI_REGISTER_CALLBACKS */

            /* Change DSI peripheral state */
        hdsi->State = HAL_DSI_STATE_BUSY;

        /**************** Turn on the regulator and enable the DSI PLL ****************/

        /* Enable the regulator */
        __HAL_DSI_REG_ENABLE(hdsi);

        /* Get tick */
        tickstart = HAL_GetTick();

        /* Wait until the regulator is ready */
        while (__HAL_DSI_GET_FLAG(hdsi, DSI_FLAG_RRS) == 0U)
        {
            /* Check for the Timeout */
            if ((HAL_GetTick() - tickstart) > DSI_TIMEOUT_VALUE)
            {
                return HAL_TIMEOUT;
            }
        }

        /* Set the PLL division factors */
        hdsi->Instance->WRPCR &= ~(DSI_WRPCR_PLL_NDIV | DSI_WRPCR_PLL_IDF | DSI_WRPCR_PLL_ODF);
        hdsi->Instance->WRPCR |= (((PLLInit->PLLNDIV) << 2U) | ((PLLInit->PLLIDF) << 11U) | ((PLLInit->PLLODF) << 16U));

        /* Enable the DSI PLL */
        __HAL_DSI_PLL_ENABLE(hdsi);

        /* Get tick */
        tickstart = HAL_GetTick();

        /* Wait for the lock of the PLL */
        while (__HAL_DSI_GET_FLAG(hdsi, DSI_FLAG_PLLLS) == 0U)
        {
            /* Check for the Timeout */
            if ((HAL_GetTick() - tickstart) > DSI_TIMEOUT_VALUE)
            {
                return HAL_TIMEOUT;
            }
        }

        /*************************** Set the PHY parameters ***************************/

        /* D-PHY clock and digital enable*/
        hdsi->Instance->PCTLR |= (DSI_PCTLR_CKE | DSI_PCTLR_DEN);

        /* Clock lane configuration */
        hdsi->Instance->CLCR &= ~(DSI_CLCR_DPCC | DSI_CLCR_ACR);
        hdsi->Instance->CLCR |= (DSI_CLCR_DPCC | hdsi->Init.AutomaticClockLaneControl);

        /* Configure the number of active data lanes */
        hdsi->Instance->PCONFR &= ~DSI_PCONFR_NL;
        hdsi->Instance->PCONFR |= hdsi->Init.NumberOfLanes;

        /************************ Set the DSI clock parameters ************************/

        /* Set the TX escape clock division factor */
        hdsi->Instance->CCR &= ~DSI_CCR_TXECKDIV;
        hdsi->Instance->CCR |= hdsi->Init.TXEscapeCkdiv;

        /* Calculate the bit period in high-speed mode in unit of 0.25 ns (UIX4) */
        /* The equation is : UIX4 = IntegerPart( (1000/F_PHY_Mhz) * 4 )          */
        /* Where : F_PHY_Mhz = (NDIV * HSE_Mhz) / (IDF * ODF)                    */
        tempIDF = (PLLInit->PLLIDF > 0U) ? PLLInit->PLLIDF : 1U;
        unitIntervalx4 = (4000000U * tempIDF * ((1UL << (0x3U & PLLInit->PLLODF)))) / ((HSE_VALUE / 1000U) * PLLInit->PLLNDIV);

        /* Set the bit period in high-speed mode */
        hdsi->Instance->WPCR[0U] &= ~DSI_WPCR0_UIX4;
        hdsi->Instance->WPCR[0U] |= unitIntervalx4;

        /****************************** Error management *****************************/

        /* Disable all error interrupts and reset the Error Mask */
        hdsi->Instance->IER[0U] = 0U;
        hdsi->Instance->IER[1U] = 0U;
        hdsi->ErrorMsk = 0U;

        /* Initialise the error code */
        hdsi->ErrorCode = HAL_DSI_ERROR_NONE;

        /* Initialize the DSI state*/
        hdsi->State = HAL_DSI_STATE_READY;

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_LTDCEx_StructInitFromVideoConfig(LTDC_HandleTypeDef* hltdc, DSI_VidCfgTypeDef* VidCfg)
    {
        /* Retrieve signal polarities from DSI */

        /* The following polarity is inverted:
                           LTDC_DEPOLARITY_AL <-> LTDC_DEPOLARITY_AH */

                           /* Note 1 : Code in line w/ Current LTDC specification */
        hltdc->Init.DEPolarity = (VidCfg->DEPolarity == DSI_DATA_ENABLE_ACTIVE_HIGH) ? LTDC_DEPOLARITY_AL : LTDC_DEPOLARITY_AH;
        hltdc->Init.VSPolarity = (VidCfg->VSPolarity == DSI_VSYNC_ACTIVE_HIGH) ? LTDC_VSPOLARITY_AH : LTDC_VSPOLARITY_AL;
        hltdc->Init.HSPolarity = (VidCfg->HSPolarity == DSI_HSYNC_ACTIVE_HIGH) ? LTDC_HSPOLARITY_AH : LTDC_HSPOLARITY_AL;

        /* Note 2: Code to be used in case LTDC polarities inversion updated in the specification */
        /* hltdc->Init.DEPolarity = VidCfg->DEPolarity << 29;
           hltdc->Init.VSPolarity = VidCfg->VSPolarity << 29;
           hltdc->Init.HSPolarity = VidCfg->HSPolarity << 29; */

           /* Retrieve vertical timing parameters from DSI */
        hltdc->Init.VerticalSync = VidCfg->VerticalSyncActive - 1U;
        hltdc->Init.AccumulatedVBP = VidCfg->VerticalSyncActive + VidCfg->VerticalBackPorch - 1U;
        hltdc->Init.AccumulatedActiveH = VidCfg->VerticalSyncActive + VidCfg->VerticalBackPorch + VidCfg->VerticalActive - 1U;
        hltdc->Init.TotalHeigh = VidCfg->VerticalSyncActive + VidCfg->VerticalBackPorch + VidCfg->VerticalActive + VidCfg->VerticalFrontPorch - 1U;

        return HAL_OK;
    }

    //______________________________________________________________________
    // Touch
    //___________________________________________________________________

    void ft6x06_Init(uint16_t DeviceAddr)
    {
        uint8_t instance;
        uint8_t empty;

        /* Check if device instance already exists */
        instance = ft6x06_GetInstance(DeviceAddr);

        /* To prevent double initialization */
        if (instance == 0xFF)
        {
            /* Look for empty instance */
            empty = ft6x06_GetInstance(0);

            if (empty < FT6x06_MAX_INSTANCE)
            {
                /* Register the current device instance */
                ft6x06[empty] = DeviceAddr;

                /* Initialize IO BUS layer */
                TS_IO_Init();
            }
        }
    }
    uint8_t ft6x06_GetInstance(uint16_t DeviceAddr)
    {
        uint8_t idx = 0;

        /* Check all the registered instances */
        for (idx = 0; idx < FT6x06_MAX_INSTANCE; idx++)
        {
            if (ft6x06[idx] == DeviceAddr)
            {
                return idx;
            }
        }

        return 0xFF;
    }
    void ft6x06_Reset(uint16_t DeviceAddr)
    {
        UNUSED(DeviceAddr);
        /* Do nothing */
        /* No software reset sequence available in FT6206 IC */
    }
    void TS_IO_Init(void)
    {
        I2Cx_Init(&hI2cAudioHandler);
    }
    uint16_t ft6x06_ReadID(uint16_t DeviceAddr)
    {
        /* Initialize I2C link if needed */
        TS_IO_Init();

        /* Return the device ID value */
        return (TS_IO_Read(DeviceAddr, FT6206_CHIP_ID_REG));
    }
    static uint32_t ft6x06_TS_Configure(uint16_t DeviceAddr)
    {
        uint32_t status = FT6206_STATUS_OK;
        UNUSED(DeviceAddr);

        /* Nothing special to be done for FT6206 */

        return(status);
    }
    void ft6x06_TS_Start(uint16_t DeviceAddr)
    {
#if (TS_AUTO_CALIBRATION_SUPPORTED == 1)
        /* Hw Calibration sequence start : should be done once after each power up */
        /* This is called internal calibration of the touch screen                 */
        ft6x06_TS_Calibration(DeviceAddr);
#endif
        /* Minimum static configuration of FT6206 */
        ft6x06_TS_Configure(DeviceAddr);

        /* By default set FT6206 IC in Polling mode : no INT generation on FT6206 for new touch available */
        /* Note TS_INT is active low                                                                      */
        ft6x06_TS_DisableIT(DeviceAddr);
    }
    uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg)
    {
        uint8_t read_value = 0;

        I2Cx_ReadMultiple(&hI2cAudioHandler, Addr, Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&read_value, 1);

        return read_value;
    }
    uint8_t ft6x06_TS_DetectTouch(uint16_t DeviceAddr)
    {
        volatile uint8_t nbTouch = 0;

        /* Read register FT6206_TD_STAT_REG to check number of touches detection */
        nbTouch = TS_IO_Read(DeviceAddr, FT6206_TD_STAT_REG);
        nbTouch &= FT6206_TD_STAT_MASK;

        if (nbTouch > FT6206_MAX_DETECTABLE_TOUCH)
        {
            /* If invalid number of touch detected, set it to zero */
            nbTouch = 0;
        }

        /* Update ft6x06 driver internal global : current number of active touches */
        ft6x06_handle.currActiveTouchNb = nbTouch;

        /* Reset current active touch index on which to work on */
        ft6x06_handle.currActiveTouchIdx = 0;

        return(nbTouch);
    }
    void ft6x06_TS_GetXY(uint16_t DeviceAddr, uint16_t* X, uint16_t* Y)
    {
        uint8_t regAddress = 0;
        uint8_t  dataxy[4];

        if (ft6x06_handle.currActiveTouchIdx < ft6x06_handle.currActiveTouchNb)
        {
            switch (ft6x06_handle.currActiveTouchIdx)
            {
            case 0:
                regAddress = FT6206_P1_XH_REG;
                break;
            case 1:
                regAddress = FT6206_P2_XH_REG;
                break;

            default:
                break;
            }

            /* Read X and Y positions */
            TS_IO_ReadMultiple(DeviceAddr, regAddress, dataxy, sizeof(dataxy));

            /* Send back ready X position to caller */
            *X = ((dataxy[0] & FT6206_MSB_MASK) << 8) | (dataxy[1] & FT6206_LSB_MASK);

            /* Send back ready Y position to caller */
            *Y = ((dataxy[2] & FT6206_MSB_MASK) << 8) | (dataxy[3] & FT6206_LSB_MASK);

            ft6x06_handle.currActiveTouchIdx++;
        }
    }
    static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef* hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart)
    {
        while (__HAL_I2C_GET_FLAG(hi2c, Flag) == Status)
        {
            /* Check for the Timeout */
            if (Timeout != HAL_MAX_DELAY)
            {
                if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
                {
                    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                    hi2c->State = HAL_I2C_STATE_READY;
                    hi2c->Mode = HAL_I2C_MODE_NONE;

                    /* Process Unlocked */
                    __HAL_UNLOCK(hi2c);
                    return HAL_ERROR;
                }
            }
        }
        return HAL_OK;
    }
    static void I2C_TransferConfig(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint8_t Size, uint32_t Mode, uint32_t Request)
    {
        /* Check the parameters */
        assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
        assert_param(IS_TRANSFER_MODE(Mode));
        assert_param(IS_TRANSFER_REQUEST(Request));

        /* update CR2 register */
        MODIFY_REG(hi2c->Instance->CR2, ((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | (I2C_CR2_RD_WRN & (uint32_t)(Request >> (31U - I2C_CR2_RD_WRN_Pos))) | I2C_CR2_START | I2C_CR2_STOP)), \
            (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | (uint32_t)Mode | (uint32_t)Request));
    }
    static HAL_StatusTypeDef I2C_WaitOnTXISFlagUntilTimeout(I2C_HandleTypeDef* hi2c, uint32_t Timeout, uint32_t Tickstart)
    {
        while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) == RESET)
        {
            /* Check if a NACK is detected */
            if (I2C_IsAcknowledgeFailed(hi2c, Timeout, Tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            /* Check for the Timeout */
            if (Timeout != HAL_MAX_DELAY)
            {
                if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
                {
                    hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                    hi2c->State = HAL_I2C_STATE_READY;
                    hi2c->Mode = HAL_I2C_MODE_NONE;

                    /* Process Unlocked */
                    __HAL_UNLOCK(hi2c);

                    return HAL_ERROR;
                }
            }
        }
        return HAL_OK;
    }
    static HAL_StatusTypeDef I2C_RequestMemoryWrite(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart)
    {
        I2C_TransferConfig(hi2c, DevAddress, (uint8_t)MemAddSize, I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);

        /* Wait until TXIS flag is set */
        if (I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        /* If Memory address size is 8Bit */
        if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
        {
            /* Send Memory Address */
            hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
        }
        /* If Memory address size is 16Bit */
        else
        {
            /* Send MSB of Memory Address */
            hi2c->Instance->TXDR = I2C_MEM_ADD_MSB(MemAddress);

            /* Wait until TXIS flag is set */
            if (I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            /* Send LSB of Memory Address */
            hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
        }

        /* Wait until TCR flag is set */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout, Tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t* pData, uint16_t Size, uint32_t Timeout)
    {
        uint32_t tickstart;

        /* Check the parameters */
        assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

        if (hi2c->State == HAL_I2C_STATE_READY)
        {
            if ((pData == NULL) || (Size == 0U))
            {
                hi2c->ErrorCode = HAL_I2C_ERROR_INVALID_PARAM;
                return  HAL_ERROR;
            }

            /* Process Locked */
            __HAL_LOCK(hi2c);

            /* Init tickstart for timeout management*/
            tickstart = HAL_GetTick();

            if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY, tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            hi2c->State = HAL_I2C_STATE_BUSY_TX;
            hi2c->Mode = HAL_I2C_MODE_MEM;
            hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

            /* Prepare transfer parameters */
            hi2c->pBuffPtr = pData;
            hi2c->XferCount = Size;
            hi2c->XferISR = NULL;

            /* Send Slave Address and Memory Address */
            if (I2C_RequestMemoryWrite(hi2c, DevAddress, MemAddress, MemAddSize, Timeout, tickstart) != HAL_OK)
            {
                /* Process Unlocked */
                __HAL_UNLOCK(hi2c);
                return HAL_ERROR;
            }

            /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE */
            if (hi2c->XferCount > MAX_NBYTE_SIZE)
            {
                hi2c->XferSize = MAX_NBYTE_SIZE;
                I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
            }
            else
            {
                hi2c->XferSize = hi2c->XferCount;
                I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
            }

            do
            {
                /* Wait until TXIS flag is set */
                if (I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
                {
                    return HAL_ERROR;
                }

                /* Write data to TXDR */
                hi2c->Instance->TXDR = *hi2c->pBuffPtr;

                /* Increment Buffer pointer */
                hi2c->pBuffPtr++;

                hi2c->XferCount--;
                hi2c->XferSize--;

                if ((hi2c->XferCount != 0U) && (hi2c->XferSize == 0U))
                {
                    /* Wait until TCR flag is set */
                    if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout, tickstart) != HAL_OK)
                    {
                        return HAL_ERROR;
                    }

                    if (hi2c->XferCount > MAX_NBYTE_SIZE)
                    {
                        hi2c->XferSize = MAX_NBYTE_SIZE;
                        I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
                    }
                    else
                    {
                        hi2c->XferSize = hi2c->XferCount;
                        I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
                    }
                }

            } while (hi2c->XferCount > 0U);

            /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
            /* Wait until STOPF flag is reset */
            if (I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            /* Clear STOP Flag */
            __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

            /* Clear Configuration Register 2 */
            I2C_RESET_CR2(hi2c);

            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->Mode = HAL_I2C_MODE_NONE;

            /* Process Unlocked */
            __HAL_UNLOCK(hi2c);

            return HAL_OK;
        }
        else
        {
            return HAL_BUSY;
        }
    }
    void HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_Init)
    {
        uint32_t position = 0x00;
        uint32_t ioposition = 0x00;
        uint32_t iocurrent = 0x00;
        uint32_t temp = 0x00;

        /* Check the parameters */
        assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
        assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
        assert_param(IS_GPIO_MODE(GPIO_Init->Mode));
        assert_param(IS_GPIO_PULL(GPIO_Init->Pull));

        /* Configure the port pins */
        for (position = 0; position < GPIO_NUMBER; position++)
        {
            /* Get the IO position */
            ioposition = ((uint32_t)0x01) << position;
            /* Get the current IO position */
            iocurrent = (uint32_t)(GPIO_Init->Pin) & ioposition;

            if (iocurrent == ioposition)
            {
                /*--------------------- GPIO Mode Configuration ------------------------*/
                /* In case of Alternate function mode selection */
                if ((GPIO_Init->Mode == GPIO_MODE_AF_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
                {
                    /* Check the Alternate function parameter */
                    assert_param(IS_GPIO_AF(GPIO_Init->Alternate));

                    /* Configure Alternate function mapped with the current IO */
                    temp = GPIOx->AFR[position >> 3];
                    temp &= ~((uint32_t)0xF << ((uint32_t)(position & (uint32_t)0x07) * 4));
                    temp |= ((uint32_t)(GPIO_Init->Alternate) << (((uint32_t)position & (uint32_t)0x07) * 4));
                    GPIOx->AFR[position >> 3] = temp;
                }

                /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
                temp = GPIOx->MODER;
                temp &= ~(GPIO_MODER_MODER0 << (position * 2));
                temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2));
                GPIOx->MODER = temp;

                /* In case of Output or Alternate function mode selection */
                if ((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
                    (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
                {
                    /* Check the Speed parameter */
                    assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));
                    /* Configure the IO Speed */
                    temp = GPIOx->OSPEEDR;
                    temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2));
                    temp |= (GPIO_Init->Speed << (position * 2));
                    GPIOx->OSPEEDR = temp;

                    /* Configure the IO Output Type */
                    temp = GPIOx->OTYPER;
                    temp &= ~(GPIO_OTYPER_OT_0 << position);
                    temp |= (((GPIO_Init->Mode & GPIO_OUTPUT_TYPE) >> 4) << position);
                    GPIOx->OTYPER = temp;
                }

                /* Activate the Pull-up or Pull down resistor for the current IO */
                temp = GPIOx->PUPDR;
                temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2));
                temp |= ((GPIO_Init->Pull) << (position * 2));
                GPIOx->PUPDR = temp;

                /*--------------------- EXTI Mode Configuration ------------------------*/
                /* Configure the External Interrupt or event for the current IO */
                if ((GPIO_Init->Mode & EXTI_MODE) == EXTI_MODE)
                {
                    /* Enable SYSCFG Clock */
                    __HAL_RCC_SYSCFG_CLK_ENABLE();

                    temp = SYSCFG->EXTICR[position >> 2];
                    temp &= ~(((uint32_t)0x0F) << (4 * (position & 0x03)));
                    temp |= ((uint32_t)(GPIO_GET_INDEX(GPIOx)) << (4 * (position & 0x03)));
                    SYSCFG->EXTICR[position >> 2] = temp;

                    /* Clear EXTI line configuration */
                    temp = EXTI->IMR;
                    temp &= ~((uint32_t)iocurrent);
                    if ((GPIO_Init->Mode & GPIO_MODE_IT) == GPIO_MODE_IT)
                    {
                        temp |= iocurrent;
                    }
                    EXTI->IMR = temp;

                    temp = EXTI->EMR;
                    temp &= ~((uint32_t)iocurrent);
                    if ((GPIO_Init->Mode & GPIO_MODE_EVT) == GPIO_MODE_EVT)
                    {
                        temp |= iocurrent;
                    }
                    EXTI->EMR = temp;

                    /* Clear Rising Falling edge configuration */
                    temp = EXTI->RTSR;
                    temp &= ~((uint32_t)iocurrent);
                    if ((GPIO_Init->Mode & RISING_EDGE) == RISING_EDGE)
                    {
                        temp |= iocurrent;
                    }
                    EXTI->RTSR = temp;

                    temp = EXTI->FTSR;
                    temp &= ~((uint32_t)iocurrent);
                    if ((GPIO_Init->Mode & FALLING_EDGE) == FALLING_EDGE)
                    {
                        temp |= iocurrent;
                    }
                    EXTI->FTSR = temp;
                }
            }
        }
    }
    static void I2Cx_MspInit(I2C_HandleTypeDef* i2c_handler)
    {
        GPIO_InitTypeDef  gpio_init_structure;

        if (i2c_handler == (I2C_HandleTypeDef*)(&hI2cAudioHandler))
        {
            /*** Configure the GPIOs ***/
            /* Enable GPIO clock */
            DISCOVERY_AUDIO_I2Cx_SCL_GPIO_CLK_ENABLE();
            DISCOVERY_AUDIO_I2Cx_SDA_GPIO_CLK_ENABLE();
            /* Configure I2C Tx as alternate function */
            gpio_init_structure.Pin = DISCOVERY_AUDIO_I2Cx_SCL_PIN;
            gpio_init_structure.Mode = GPIO_MODE_AF_OD;
            gpio_init_structure.Pull = GPIO_NOPULL;
            gpio_init_structure.Speed = GPIO_SPEED_FAST;
            gpio_init_structure.Alternate = DISCOVERY_AUDIO_I2Cx_SCL_AF;
            HAL_GPIO_Init(DISCOVERY_AUDIO_I2Cx_SCL_GPIO_PORT, &gpio_init_structure);

            /* Configure I2C Rx as alternate function */
            gpio_init_structure.Pin = DISCOVERY_AUDIO_I2Cx_SDA_PIN;
            gpio_init_structure.Alternate = DISCOVERY_AUDIO_I2Cx_SDA_AF;
            HAL_GPIO_Init(DISCOVERY_AUDIO_I2Cx_SDA_GPIO_PORT, &gpio_init_structure);

            /*** Configure the I2C peripheral ***/
            /* Enable I2C clock */
            DISCOVERY_AUDIO_I2Cx_CLK_ENABLE();

            /* Force the I2C peripheral clock reset */
            DISCOVERY_AUDIO_I2Cx_FORCE_RESET();

            /* Release the I2C peripheral clock reset */
            DISCOVERY_AUDIO_I2Cx_RELEASE_RESET();

            /* Enable and set I2C1 Interrupt to a lower priority */
            uint32_t prioritygroup = __NVIC_GetPriorityGrouping();
            __NVIC_SetPriority(DISCOVERY_AUDIO_I2Cx_EV_IRQn, NVIC_EncodePriority(prioritygroup, 0x0F, 0));  /* NVIC configuration for DMA transfer complete interrupt */

            __NVIC_EnableIRQ(DISCOVERY_AUDIO_I2Cx_EV_IRQn);

            /* Enable and set I2C1 Interrupt to a lower priority */
            __NVIC_SetPriority(DISCOVERY_AUDIO_I2Cx_ER_IRQn, NVIC_EncodePriority(prioritygroup, 0x0F, 0));  /* NVIC configuration for DMA transfer complete interrupt */
            __NVIC_EnableIRQ(DISCOVERY_AUDIO_I2Cx_ER_IRQn);

        }
        else
        {
            /*** Configure the GPIOs ***/
            /* Enable GPIO clock */
            DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();

            /* Configure I2C Tx as alternate function */
            gpio_init_structure.Pin = DISCOVERY_EXT_I2Cx_SCL_PIN;
            gpio_init_structure.Mode = GPIO_MODE_AF_OD;
            gpio_init_structure.Pull = GPIO_NOPULL;
            gpio_init_structure.Speed = GPIO_SPEED_FAST;
            gpio_init_structure.Alternate = DISCOVERY_EXT_I2Cx_SCL_SDA_AF;
            HAL_GPIO_Init(DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

            /* Configure I2C Rx as alternate function */
            gpio_init_structure.Pin = DISCOVERY_EXT_I2Cx_SDA_PIN;
            HAL_GPIO_Init(DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

            /*** Configure the I2C peripheral ***/
            /* Enable I2C clock */
            DISCOVERY_EXT_I2Cx_CLK_ENABLE();

            /* Force the I2C peripheral clock reset */
            DISCOVERY_EXT_I2Cx_FORCE_RESET();

            /* Release the I2C peripheral clock reset */
            DISCOVERY_EXT_I2Cx_RELEASE_RESET();

            /* Enable and set I2C1 Interrupt to a lower priority */
            uint32_t prioritygroup = __NVIC_GetPriorityGrouping();
            __NVIC_SetPriority(DISCOVERY_EXT_I2Cx_EV_IRQn, NVIC_EncodePriority(prioritygroup, 0x0F, 0));  /* NVIC configuration for DMA transfer complete interrupt */

            __NVIC_EnableIRQ(DISCOVERY_EXT_I2Cx_EV_IRQn);

            /* Enable and set I2C1 Interrupt to a lower priority */
            __NVIC_SetPriority(DISCOVERY_EXT_I2Cx_ER_IRQn, NVIC_EncodePriority(prioritygroup, 0x0F, 0));  /* NVIC configuration for DMA transfer complete interrupt */
            __NVIC_EnableIRQ(DISCOVERY_EXT_I2Cx_ER_IRQn);
        }
    }
    void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
    {
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hi2c);

        /* NOTE : This function should not be modified, when the callback is needed,
                  the HAL_I2C_MspInit could be implemented in the user file
         */
    }
    HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef* hi2c)
    {
        /* Check the I2C handle allocation */
        if (hi2c == NULL)
        {
            return HAL_ERROR;
        }

        /* Check the parameters */
        assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
        assert_param(IS_I2C_OWN_ADDRESS1(hi2c->Init.OwnAddress1));
        assert_param(IS_I2C_ADDRESSING_MODE(hi2c->Init.AddressingMode));
        assert_param(IS_I2C_DUAL_ADDRESS(hi2c->Init.DualAddressMode));
        assert_param(IS_I2C_OWN_ADDRESS2(hi2c->Init.OwnAddress2));
        assert_param(IS_I2C_OWN_ADDRESS2_MASK(hi2c->Init.OwnAddress2Masks));
        assert_param(IS_I2C_GENERAL_CALL(hi2c->Init.GeneralCallMode));
        assert_param(IS_I2C_NO_STRETCH(hi2c->Init.NoStretchMode));

        if (hi2c->State == HAL_I2C_STATE_RESET)
        {
            /* Allocate lock resource and initialize it */
            hi2c->Lock = HAL_UNLOCKED;

            ////////////#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
            ////////////            /* Init the I2C Callback settings */
            ////////////            hi2c->MasterTxCpltCallback = HAL_I2C_MasterTxCpltCallback; /* Legacy weak MasterTxCpltCallback */
            ////////////            hi2c->MasterRxCpltCallback = HAL_I2C_MasterRxCpltCallback; /* Legacy weak MasterRxCpltCallback */
            ////////////            hi2c->SlaveTxCpltCallback = HAL_I2C_SlaveTxCpltCallback;  /* Legacy weak SlaveTxCpltCallback  */
            ////////////            hi2c->SlaveRxCpltCallback = HAL_I2C_SlaveRxCpltCallback;  /* Legacy weak SlaveRxCpltCallback  */
            ////////////            hi2c->ListenCpltCallback = HAL_I2C_ListenCpltCallback;   /* Legacy weak ListenCpltCallback   */
            ////////////            hi2c->MemTxCpltCallback = HAL_I2C_MemTxCpltCallback;    /* Legacy weak MemTxCpltCallback    */
            ////////////            hi2c->MemRxCpltCallback = HAL_I2C_MemRxCpltCallback;    /* Legacy weak MemRxCpltCallback    */
            ////////////            hi2c->ErrorCallback = HAL_I2C_ErrorCallback;        /* Legacy weak ErrorCallback        */
            ////////////            hi2c->AbortCpltCallback = HAL_I2C_AbortCpltCallback;    /* Legacy weak AbortCpltCallback    */
            ////////////            hi2c->AddrCallback = HAL_I2C_AddrCallback;         /* Legacy weak AddrCallback         */
            ////////////
            ////////////            if (hi2c->MspInitCallback == NULL)
            ////////////            {
            ////////////                hi2c->MspInitCallback = HAL_I2C_MspInit; /* Legacy weak MspInit  */
            ////////////            }
            ////////////
            ////////////            /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
            ////////////            hi2c->MspInitCallback(hi2c);
            ////////////#else
                        /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
            HAL_I2C_MspInit(hi2c);
            ////////////#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
        }

        hi2c->State = HAL_I2C_STATE_BUSY;

        /* Disable the selected I2C peripheral */
        __HAL_I2C_DISABLE(hi2c);

        /*---------------------------- I2Cx TIMINGR Configuration ------------------*/
        /* Configure I2Cx: Frequency range */
        hi2c->Instance->TIMINGR = hi2c->Init.Timing & TIMING_CLEAR_MASK;

        /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
        /* Disable Own Address1 before set the Own Address1 configuration */
        hi2c->Instance->OAR1 &= ~I2C_OAR1_OA1EN;

        /* Configure I2Cx: Own Address1 and ack own address1 mode */
        if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
        {
            hi2c->Instance->OAR1 = (I2C_OAR1_OA1EN | hi2c->Init.OwnAddress1);
        }
        else /* I2C_ADDRESSINGMODE_10BIT */
        {
            hi2c->Instance->OAR1 = (I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE | hi2c->Init.OwnAddress1);
        }

        /*---------------------------- I2Cx CR2 Configuration ----------------------*/
        /* Configure I2Cx: Addressing Master mode */
        if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT)
        {
            hi2c->Instance->CR2 = (I2C_CR2_ADD10);
        }
        /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
        hi2c->Instance->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);

        /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
        /* Disable Own Address2 before set the Own Address2 configuration */
        hi2c->Instance->OAR2 &= ~I2C_DUALADDRESS_ENABLE;

        /* Configure I2Cx: Dual mode and Own Address2 */
        hi2c->Instance->OAR2 = (hi2c->Init.DualAddressMode | hi2c->Init.OwnAddress2 | (hi2c->Init.OwnAddress2Masks << 8));

        /*---------------------------- I2Cx CR1 Configuration ----------------------*/
        /* Configure I2Cx: Generalcall and NoStretch mode */
        hi2c->Instance->CR1 = (hi2c->Init.GeneralCallMode | hi2c->Init.NoStretchMode);

        /* Enable the selected I2C peripheral */
        __HAL_I2C_ENABLE(hi2c);

        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        return HAL_OK;
    }
    HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef* hi2c)
    {
        /* Return I2C handle state */
        return hi2c->State;
    }
    static void I2Cx_Init(I2C_HandleTypeDef* i2c_handler)
    {
        if (HAL_I2C_GetState(i2c_handler) == HAL_I2C_STATE_RESET)
        {
            if (i2c_handler == (I2C_HandleTypeDef*)(&hI2cAudioHandler))
            {
                /* Audio and LCD I2C configuration */
                i2c_handler->Instance = DISCOVERY_AUDIO_I2Cx;
            }
            else
            {
                /* External, camera and Arduino connector  I2C configuration */
                i2c_handler->Instance = DISCOVERY_EXT_I2Cx;
            }
            i2c_handler->Init.Timing = DISCOVERY_I2Cx_TIMING;
            i2c_handler->Init.OwnAddress1 = 0;
            i2c_handler->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
            i2c_handler->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
            i2c_handler->Init.OwnAddress2 = 0;
            i2c_handler->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
            i2c_handler->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

            /* Init the I2C */
            I2Cx_MspInit(i2c_handler);
            HAL_I2C_Init(i2c_handler);
        }
    }
    void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
    {
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hi2c);

        /* NOTE : This function should not be modified, when the callback is needed,
                  the HAL_I2C_MspDeInit could be implemented in the user file
         */
    }
    HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef* hi2c)
    {
        /* Check the I2C handle allocation */
        if (hi2c == NULL)
        {
            return HAL_ERROR;
        }

        /* Check the parameters */
        assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));

        hi2c->State = HAL_I2C_STATE_BUSY;

        /* Disable the I2C Peripheral Clock */
        __HAL_I2C_DISABLE(hi2c);

        ////////////////#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
        ////////////////        if (hi2c->MspDeInitCallback == NULL)
        ////////////////        {
        ////////////////            hi2c->MspDeInitCallback = HAL_I2C_MspDeInit; /* Legacy weak MspDeInit  */
        ////////////////        }
        ////////////////
        ////////////////        /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
        ////////////////        hi2c->MspDeInitCallback(hi2c);
        ////////////////#else
                /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
        HAL_I2C_MspDeInit(hi2c);
        //////////////////#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
        hi2c->State = HAL_I2C_STATE_RESET;
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Release Lock */
        __HAL_UNLOCK(hi2c);

        return HAL_OK;
    }
    static void I2Cx_Error(I2C_HandleTypeDef* i2c_handler, uint8_t Addr)
    {
        /* De-initialize the I2C communication bus */
        HAL_I2C_DeInit(i2c_handler);

        /* Re-Initialize the I2C communication bus */
        I2Cx_Init(i2c_handler);

        UNUSED(Addr);
    }
    static HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef* i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t* Buffer, uint16_t Length)
    {
        HAL_StatusTypeDef status = HAL_OK;

        status = HAL_I2C_Mem_Write(i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);

        /* Check the communication status */
        if (status != HAL_OK)
        {
            /* Re-Initiaize the I2C Bus */
            I2Cx_Error(i2c_handler, Addr);
        }
        return status;
    }
    void TS_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t* Buffer, uint16_t Length)
    {
        I2Cx_WriteMultiple(&hI2cAudioHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length);
    }
    void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
    {
        I2Cx_WriteMultiple(&hI2cAudioHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&Value, 1);
    }
    uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t* Buffer, uint16_t Length)
    {
        return I2Cx_ReadMultiple(&hI2cAudioHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length);
    }
    static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef* i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t* Buffer, uint16_t Length)
    {
        HAL_StatusTypeDef status = HAL_OK;

        status = HAL_I2C_Mem_Read(i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);

        /* Check the communication status */
        if (status != HAL_OK)
        {
            /* I2C error occured */
            I2Cx_Error(i2c_handler, Addr);
        }
        return status;
    }
    static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart)
    {
        I2C_TransferConfig(hi2c, DevAddress, (uint8_t)MemAddSize, I2C_SOFTEND_MODE, I2C_GENERATE_START_WRITE);

        /* Wait until TXIS flag is set */
        if (I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        /* If Memory address size is 8Bit */
        if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
        {
            /* Send Memory Address */
            hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
        }
        /* If Memory address size is 16Bit */
        else
        {
            /* Send MSB of Memory Address */
            hi2c->Instance->TXDR = I2C_MEM_ADD_MSB(MemAddress);

            /* Wait until TXIS flag is set */
            if (I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            /* Send LSB of Memory Address */
            hi2c->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
        }

        /* Wait until TC flag is set */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TC, RESET, Timeout, Tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        return HAL_OK;
    }
    static void I2C_Flush_TXDR(I2C_HandleTypeDef* hi2c)
    {
        /* If a pending TXIS flag is set */
        /* Write a dummy data in TXDR to clear it */
        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) != RESET)
        {
            hi2c->Instance->TXDR = 0x00U;
        }

        /* Flush TX register if not empty */
        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == RESET)
        {
            __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_TXE);
        }
    }
    static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef* hi2c, uint32_t Timeout, uint32_t Tickstart)
    {
        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
        {
            /* Wait until STOP Flag is reset */
            /* AutoEnd should be initiate after AF */
            while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET)
            {
                /* Check for the Timeout */
                if (Timeout != HAL_MAX_DELAY)
                {
                    if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
                    {
                        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                        hi2c->State = HAL_I2C_STATE_READY;
                        hi2c->Mode = HAL_I2C_MODE_NONE;

                        /* Process Unlocked */
                        __HAL_UNLOCK(hi2c);

                        return HAL_ERROR;
                    }
                }
            }

            /* Clear NACKF Flag */
            __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

            /* Clear STOP Flag */
            __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

            /* Flush TX register */
            I2C_Flush_TXDR(hi2c);

            /* Clear Configuration Register 2 */
            I2C_RESET_CR2(hi2c);

            hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->Mode = HAL_I2C_MODE_NONE;

            /* Process Unlocked */
            __HAL_UNLOCK(hi2c);

            return HAL_ERROR;
        }
        return HAL_OK;
    }
    static HAL_StatusTypeDef I2C_WaitOnSTOPFlagUntilTimeout(I2C_HandleTypeDef* hi2c, uint32_t Timeout, uint32_t Tickstart)
    {
        while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET)
        {
            /* Check if a NACK is detected */
            if (I2C_IsAcknowledgeFailed(hi2c, Timeout, Tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            /* Check for the Timeout */
            if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
            {
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->Mode = HAL_I2C_MODE_NONE;

                /* Process Unlocked */
                __HAL_UNLOCK(hi2c);

                return HAL_ERROR;
            }
        }
        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t* pData, uint16_t Size, uint32_t Timeout)
    {
        uint32_t tickstart;

        /* Check the parameters */
        assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

        if (hi2c->State == HAL_I2C_STATE_READY)
        {
            if ((pData == NULL) || (Size == 0U))
            {
                hi2c->ErrorCode = HAL_I2C_ERROR_INVALID_PARAM;
                return  HAL_ERROR;
            }

            /* Process Locked */
            __HAL_LOCK(hi2c);

            /* Init tickstart for timeout management*/
            tickstart = HAL_GetTick();

            if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY, tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            hi2c->State = HAL_I2C_STATE_BUSY_RX;
            hi2c->Mode = HAL_I2C_MODE_MEM;
            hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

            /* Prepare transfer parameters */
            hi2c->pBuffPtr = pData;
            hi2c->XferCount = Size;
            hi2c->XferISR = NULL;

            /* Send Slave Address and Memory Address */
            if (I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, Timeout, tickstart) != HAL_OK)
            {
                /* Process Unlocked */
                __HAL_UNLOCK(hi2c);
                return HAL_ERROR;
            }

            /* Send Slave Address */
            /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and generate RESTART */
            if (hi2c->XferCount > MAX_NBYTE_SIZE)
            {
                hi2c->XferSize = MAX_NBYTE_SIZE;
                I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize, I2C_RELOAD_MODE, I2C_GENERATE_START_READ);
            }
            else
            {
                hi2c->XferSize = hi2c->XferCount;
                I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);
            }

            do
            {
                /* Wait until RXNE flag is set */
                if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_RXNE, RESET, Timeout, tickstart) != HAL_OK)
                {
                    return HAL_ERROR;
                }

                /* Read data from RXDR */
                *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->RXDR;

                /* Increment Buffer pointer */
                hi2c->pBuffPtr++;

                hi2c->XferSize--;
                hi2c->XferCount--;

                if ((hi2c->XferCount != 0U) && (hi2c->XferSize == 0U))
                {
                    /* Wait until TCR flag is set */
                    if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout, tickstart) != HAL_OK)
                    {
                        return HAL_ERROR;
                    }

                    if (hi2c->XferCount > MAX_NBYTE_SIZE)
                    {
                        hi2c->XferSize = MAX_NBYTE_SIZE;
                        I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize, I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
                    }
                    else
                    {
                        hi2c->XferSize = hi2c->XferCount;
                        I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
                    }
                }
            } while (hi2c->XferCount > 0U);

            /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
            /* Wait until STOPF flag is reset */
            if (I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            /* Clear STOP Flag */
            __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

            /* Clear Configuration Register 2 */
            I2C_RESET_CR2(hi2c);

            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->Mode = HAL_I2C_MODE_NONE;

            /* Process Unlocked */
            __HAL_UNLOCK(hi2c);

            return HAL_OK;
        }
        else
        {
            return HAL_BUSY;
        }
    }
    void ft6x06_TS_EnableIT(uint16_t DeviceAddr)
    {
        uint8_t regValue = 0;
        regValue = (FT6206_G_MODE_INTERRUPT_TRIGGER & (FT6206_G_MODE_INTERRUPT_MASK >> FT6206_G_MODE_INTERRUPT_SHIFT)) << FT6206_G_MODE_INTERRUPT_SHIFT;

        /* Set interrupt trigger mode in FT6206_GMODE_REG */
        TS_IO_Write(DeviceAddr, FT6206_GMODE_REG, regValue);
    }
    void ft6x06_TS_DisableIT(uint16_t DeviceAddr)
    {
        uint8_t regValue = 0;
        regValue = (FT6206_G_MODE_INTERRUPT_POLLING & (FT6206_G_MODE_INTERRUPT_MASK >> FT6206_G_MODE_INTERRUPT_SHIFT)) << FT6206_G_MODE_INTERRUPT_SHIFT;

        /* Set interrupt polling mode in FT6206_GMODE_REG */
        TS_IO_Write(DeviceAddr, FT6206_GMODE_REG, regValue);
        UNUSED(DeviceAddr);
    }
    uint8_t ft6x06_TS_ITStatus(uint16_t DeviceAddr)
    {
        /* Always return 0 as feature not applicable to FT6206 */
        UNUSED(DeviceAddr);
        return 0;
    }
    void ft6x06_TS_ClearIT(uint16_t DeviceAddr)
    {
        UNUSED(DeviceAddr);
        /* Nothing to be done here for FT6206 */
    }
    static void DSI_ConfigPacketHeader(DSI_TypeDef* DSIx,
        uint32_t ChannelID,
        uint32_t DataType,
        uint32_t Data0,
        uint32_t Data1)
    {
        /* Update the DSI packet header with new information */
        DSIx->GHCR = (DataType | (ChannelID << 6U) | (Data0 << 8U) | (Data1 << 16U));
    }
    static HAL_StatusTypeDef HAL_DSI_ShortWrite(DSI_HandleTypeDef* hdsi,
        uint32_t ChannelID,
        uint32_t Mode,
        uint32_t Param1,
        uint32_t Param2)
    {
        uint32_t tickstart;

        /* Get tick */
        tickstart = HAL_GetTick();

        /* Wait for Command FIFO Empty */
        while ((hdsi->Instance->GPSR & DSI_GPSR_CMDFE) == 0U)
        {
            /* Check for the Timeout */
            if ((HAL_GetTick() - tickstart) > DSI_TIMEOUT_VALUE)
            {
                return HAL_TIMEOUT;
            }
        }

        /* Configure the packet to send a short DCS command with 0 or 1 parameter */
        /* Update the DSI packet header with new information */
        hdsi->Instance->GHCR = (Mode | (ChannelID << 6U) | (Param1 << 8U) | (Param2 << 16U));

        return HAL_OK;
    }
    HAL_StatusTypeDef HAL_DSI_LongWrite(DSI_HandleTypeDef* hdsi,
        uint32_t ChannelID,
        uint32_t Mode,
        uint32_t NbParams,
        uint32_t Param1,
        uint8_t* ParametersTable)
    {
        uint32_t uicounter, nbBytes, count;
        uint32_t tickstart;
        uint32_t fifoword;
        uint8_t* pparams = ParametersTable;

        /* Process locked */
        __HAL_LOCK(hdsi);

        /* Check the parameters */
        assert_param(IS_DSI_LONG_WRITE_PACKET_TYPE(Mode));

        /* Get tick */
        tickstart = HAL_GetTick();

        /* Wait for Command FIFO Empty */
        while ((hdsi->Instance->GPSR & DSI_GPSR_CMDFE) == 0U)
        {
            /* Check for the Timeout */
            if ((HAL_GetTick() - tickstart) > DSI_TIMEOUT_VALUE)
            {
                /* Process Unlocked */
                __HAL_UNLOCK(hdsi);

                return HAL_TIMEOUT;
            }
        }

        /* Set the DCS code on payload byte 1, and the other parameters on the write FIFO command*/
        fifoword = Param1;
        nbBytes = (NbParams < 3U) ? NbParams : 3U;

        for (count = 0U; count < nbBytes; count++)
        {
            fifoword |= (((uint32_t)(*(pparams + count))) << (8U + (8U * count)));
        }
        hdsi->Instance->GPDR = fifoword;

        uicounter = NbParams - nbBytes;
        pparams += nbBytes;
        /* Set the Next parameters on the write FIFO command*/
        while (uicounter != 0U)
        {
            nbBytes = (uicounter < 4U) ? uicounter : 4U;
            fifoword = 0U;
            for (count = 0U; count < nbBytes; count++)
            {
                fifoword |= (((uint32_t)(*(pparams + count))) << (8U * count));
            }
            hdsi->Instance->GPDR = fifoword;

            uicounter -= nbBytes;
            pparams += nbBytes;
        }

        /* Configure the packet to send a long DCS command */
        DSI_ConfigPacketHeader(hdsi->Instance,
            ChannelID,
            Mode,
            ((NbParams + 1U) & 0x00FFU),
            (((NbParams + 1U) & 0xFF00U) >> 8U));

        /* Process unlocked */
        __HAL_UNLOCK(hdsi);

        return HAL_OK;
    }
    void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t* pParams)
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
    void wait_ms(int delay)
    {
        UNUSED(delay);
    }
