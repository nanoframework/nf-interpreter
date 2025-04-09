//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// For a  FT6206 by Focal Tech. ( Originally developed for the STM32F79iDiscovery board

#ifndef stmpe811qtr_H
#define stmpe811qtr_H

#include "TouchDevice.h"
#include "TouchInterface.h"

struct TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

#define STMPE811QTR_ID_VALUE    0x11
#define STMPE811QTR_CHIP_ID_REG 0xA8

#define STMPE811QTR_GMODE_REG                (CLR_UINT8)0xA4
#define STMPE811QTR_G_MODE_INTERRUPT_TRIGGER (CLR_UINT8)0x01
#define STMPE811QTR_G_MODE_INTERRUPT_POLLING (CLR_UINT8)0x00
#define STMPE811QTR_G_MODE_INTERRUPT_MASK    (CLR_UINT8)0x03
#define STMPE811QTR_G_MODE_INTERRUPT_SHIFT   (CLR_UINT8)0x00
#define STMPE811QTR_TD_STAT_REG              (CLR_UINT8)0x02
#define STMPE811QTR_TD_STAT_MASK             (CLR_UINT8)0x0F
#define STMPE811QTR_MSB_MASK                 (CLR_UINT8)0x0F
#define STMPE811QTR_LSB_MASK                 (CLR_UINT8)0xFF

#define STMPE811QTR_P1_XH_REG (CLR_UINT8)0x03
#define STMPE811QTR_P2_XH_REG (CLR_UINT8)0x09

#define I2C_MEMADD_SIZE_8BIT (0x00000001U)

bool TouchDevice::Initialize()
{
    /*
        // Read the ID for confirmation
        CLR_UINT8 chipIdRegister;
        chipIdRegister = STMPE811QTR_CHIP_ID_REG;
        CLR_UINT8 *id;
        CLR_UINT16 numberValuesExpected = 1;
        id = g_TouchInterface.Write_Read(&chipIdRegister, 1, numberValuesExpected);
        bool result = (*id == STMPE811QTR_ID_VALUE);
        return result;
    */
    return false;
}

bool TouchDevice::Enable(GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc)
{
    (void)touchIsrProc;

    /*
        // Connect the Interrupt Service routine to the interrupt pin
        // G3# - PI13  ( GPIOI_LCD_INT )
        GPIO_PIN STMPE811QTR_Interrupt = GPIOI_LCD_INT;
        PinMode driveMode = PinMode_Input;
        GPIO_INT_EDGE IntEdge = GPIO_INT_EDGE_HIGH;
        void *ISR_Param = nullptr;
        if (CPU_GPIO_ReservePin(GPIOI_LCD_INT, true))
        {
            if (CPU_GPIO_EnableInputPin(STMPE811QTR_Interrupt, 0, touchIsrProc, ISR_Param, IntEdge, driveMode))
            {
                // Configure the  device to generate IT on given INT pin connected to MCU as EXTI.
                uint8_t regValue = 0;
                regValue =
                    (STMPE811QTR_G_MODE_INTERRUPT_TRIGGER & (STMPE811QTR_G_MODE_INTERRUPT_MASK >>
       STMPE811QTR_G_MODE_INTERRUPT_SHIFT))
                    << STMPE811QTR_G_MODE_INTERRUPT_SHIFT;
                CLR_UINT8 values[2] = {STMPE811QTR_GMODE_REG, regValue};
                g_TouchInterface.Write_Read(values, 2, 0);
            }
        }
    */
    return TRUE;
}

bool TouchDevice::Disable()
{
    /*
        // Configure the  device to stop generating IT on the given INT pin connected to MCU as EXTI.
        CLR_UINT8 regValue =
            (STMPE811QTR_G_MODE_INTERRUPT_POLLING & (STMPE811QTR_G_MODE_INTERRUPT_MASK >>
       STMPE811QTR_G_MODE_INTERRUPT_SHIFT))
            << STMPE811QTR_G_MODE_INTERRUPT_SHIFT;
        CLR_UINT8 values[2] = {STMPE811QTR_GMODE_REG, regValue};
        g_TouchInterface.Write_Read(values, 2, 0);
    */
    return true;
}

TouchPointDevice TouchDevice::GetPoint()
{
    TouchPointDevice t;
    /*
        CLR_UINT8 regAddress;
        regAddress = STMPE811QTR_P1_XH_REG;
        CLR_UINT8 *dataxy;
        CLR_UINT16 numberValuesExpected = 4;

        dataxy = g_TouchInterface.Write_Read(&regAddress, 1, numberValuesExpected);
        t.x = ((dataxy[0] & STMPE811QTR_MSB_MASK) << 8) | (dataxy[1] & STMPE811QTR_LSB_MASK);
        t.y = ((dataxy[2] & STMPE811QTR_MSB_MASK) << 8) | (dataxy[3] & STMPE811QTR_LSB_MASK);
    */
    return t;
}

#endif // stmpe811qtr_H
