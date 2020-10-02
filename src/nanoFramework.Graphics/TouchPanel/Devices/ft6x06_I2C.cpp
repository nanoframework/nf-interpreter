//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// For a  FT6206 by Focal Tech. ( Originally developed for the STM32F79iDiscovery board

#ifndef _ft6x06_H_
#define _ft6x06_H_ 1

#include "TouchDevice.h"
#include "TouchInterface.h"

struct TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

#define FT6206_ID_VALUE     0x11
#define FT6206_CHIP_ID_REG  0xA8

#define FT6206_GMODE_REG                (CLR_UINT8)0xA4
#define FT6206_G_MODE_INTERRUPT_TRIGGER (CLR_UINT8)0x01
#define FT6206_G_MODE_INTERRUPT_POLLING (CLR_UINT8)0x00
#define FT6206_G_MODE_INTERRUPT_MASK    (CLR_UINT8)0x03
#define FT6206_G_MODE_INTERRUPT_SHIFT   (CLR_UINT8)0x00
#define FT6206_TD_STAT_REG              (CLR_UINT8)0x02
#define FT6206_TD_STAT_MASK             (CLR_UINT8)0x0F
#define FT6206_MSB_MASK                 (CLR_UINT8)0x0F
#define FT6206_LSB_MASK                 (CLR_UINT8)0xFF

#define FT6206_P1_XH_REG                (CLR_UINT8)0x03
#define FT6206_P2_XH_REG                (CLR_UINT8)0x09

#define I2C_MEMADD_SIZE_8BIT            (0x00000001U)

bool TouchDevice::Initialize()
{
   // Read the FT6206 ID for confirmation
    CLR_UINT8 chipIdRegister;
    chipIdRegister = FT6206_CHIP_ID_REG;
    CLR_UINT8* id;
    CLR_UINT16 numberValuesExpected = 1;
    id = g_TouchInterface.Write_Read(&chipIdRegister, 1, numberValuesExpected);
    bool result = (*id == FT6206_ID_VALUE);
    return result;
}

bool TouchDevice::Enable(GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc)
{
    // Connect the Interrupt Service routine to the FT6206 interrupt pin
    // G3# - PI13  ( GPIOI_LCD_INT )

    GPIO_PIN FT6206_Interrupt = GPIOI_LCD_INT;
    GpioPinDriveMode driveMode = GpioPinDriveMode_Input;
    GPIO_INT_EDGE IntEdge = GPIO_INT_EDGE_HIGH;
    void* ISR_Param = NULL;
    
    if (CPU_GPIO_ReservePin(GPIOI_LCD_INT, true))
    {
        if (CPU_GPIO_EnableInputPin(FT6206_Interrupt, 0, touchIsrProc, ISR_Param, IntEdge, driveMode))
        {
            // Configure the FT6206 device to generate IT on given INT pin connected to MCU as EXTI.
            uint8_t regValue = 0;
            regValue = (FT6206_G_MODE_INTERRUPT_TRIGGER & (FT6206_G_MODE_INTERRUPT_MASK >> FT6206_G_MODE_INTERRUPT_SHIFT)) << FT6206_G_MODE_INTERRUPT_SHIFT;
            CLR_UINT8 values[2] = { FT6206_GMODE_REG ,regValue };
            g_TouchInterface.Write_Read(values, 2, 0);
        }
    }
    return TRUE;
}

bool TouchDevice::Disable()
{
    // Configure the FT6206 device to stop generating IT on the given INT pin connected to MCU as EXTI.
    CLR_UINT8 regValue = (FT6206_G_MODE_INTERRUPT_POLLING & (FT6206_G_MODE_INTERRUPT_MASK >> FT6206_G_MODE_INTERRUPT_SHIFT)) << FT6206_G_MODE_INTERRUPT_SHIFT;
    CLR_UINT8 values[2] = { FT6206_GMODE_REG ,regValue };
    g_TouchInterface.Write_Read(values, 2, 0);


    return true;
}

TouchPointDevice TouchDevice::GetPoint()
{
    TouchPointDevice t;
    CLR_UINT8 regAddress;
    regAddress = FT6206_P1_XH_REG;
    CLR_UINT8* dataxy;
    CLR_UINT16 numberValuesExpected = 4;

    dataxy = g_TouchInterface.Write_Read(&regAddress, 1, numberValuesExpected);
    t.x = ((dataxy[0] & FT6206_MSB_MASK) << 8) | (dataxy[1] & FT6206_LSB_MASK);
    t.y = ((dataxy[2] & FT6206_MSB_MASK) << 8) | (dataxy[3] & FT6206_LSB_MASK);

    return t;
}


#endif // _ft6x06_H_

