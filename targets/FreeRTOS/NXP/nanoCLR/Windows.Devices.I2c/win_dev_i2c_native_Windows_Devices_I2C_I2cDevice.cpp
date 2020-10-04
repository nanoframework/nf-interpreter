//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include "win_dev_i2c_native_target.h"

/* Select USB1 PLL (480 MHz) as master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_DIVIDER (5U)
/* Get frequency of lpi2c clock */
#define LPI2C_CLOCK_FREQUENCY ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (LPI2C_CLOCK_SOURCE_DIVIDER + 1U))


typedef struct 
{
    lpi2c_master_config_t masterConfig;
    uint8_t  slaveAddress; 
    uint8_t *txBuffer;
    int      txSize;
    uint8_t *rxBuffer;
    int      rxSize;
}i2c_structure_t;
 i2c_structure_t i2c1, i2c2, i2c3;


static LPI2C_Type *i2cBaseAddress[] = LPI2C_BASE_PTRS;

typedef Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings I2cConnectionSettings;



static i2c_structure_t *I2C_ChoosePeripheralStructure(uint8_t busIndex)
{
    if(busIndex == 1)
    {
        return &i2c1;
    }
    else if(busIndex == 2)
    {
        return &i2c2;
    }
    else if(busIndex == 3)
    {
        return &i2c3;
    }
    else
    {
        return NULL;
    }
} 


static void I2C_InitPins(uint8_t busIndex)
{
    switch(busIndex)
    {
        case 1:
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL, 1U);        
	        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA, 1U);                                           
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL, 0xC8B0U);                                     
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA, 0xC8B0U);
            break;

        case 2:
            IOMUXC_SetPinMux(IOMUXC_GPIO_B0_04_LPI2C2_SCL, 1U);        
	        IOMUXC_SetPinMux(IOMUXC_GPIO_B0_05_LPI2C2_SDA, 1U);                                           
            IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_04_LPI2C2_SCL, 0xC8B0U);                                     
            IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_05_LPI2C2_SDA, 0xC8B0U);
            break;

        case 3:
            IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_22_LPI2C3_SCL, 1U);        
	        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_06_LPI2C3_SDA, 1U);                                           
            IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_22_LPI2C3_SCL, 0xC8B0U);                                     
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_06_LPI2C3_SDA, 0xC8B0U);
            break;
    }
                                                                                      
}


static void I2C_InitClocks(void)
{
    CLOCK_EnableClock(kCLOCK_Iomuxc);
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 0);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 5);
}


static status_t I2C_WriteProcedure(uint8_t busIndex, i2c_structure_t *pI2Cx)
{
    status_t i2cStatus = kStatus_Fail;
    i2cStatus = LPI2C_MasterStart(i2cBaseAddress[busIndex], pI2Cx->slaveAddress, kLPI2C_Write);
    if(i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    i2cStatus = LPI2C_MasterSend(i2cBaseAddress[busIndex], pI2Cx->txBuffer, pI2Cx->txSize);
    if(i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    LPI2C_MasterStop(i2cBaseAddress[busIndex]);
    return i2cStatus;
}


static status_t I2C_ReadProcedure(uint8_t busIndex, i2c_structure_t *pI2Cx)
{
    status_t i2cStatus = kStatus_Fail;
    i2cStatus = LPI2C_MasterStart(i2cBaseAddress[busIndex], pI2Cx->slaveAddress, kLPI2C_Write);
    if(i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    i2cStatus = LPI2C_MasterSend(i2cBaseAddress[busIndex], pI2Cx->txBuffer, pI2Cx->txSize);
    if(i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    i2cStatus = LPI2C_MasterStart(i2cBaseAddress[busIndex], pI2Cx->slaveAddress, kLPI2C_Read);
	if (i2cStatus != kStatus_Success)
	{
		return i2cStatus;
	}
    i2cStatus = LPI2C_MasterReceive(i2cBaseAddress[busIndex], pI2Cx->rxBuffer, pI2Cx->rxSize);
    if(i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    i2cStatus = LPI2C_MasterStop(i2cBaseAddress[busIndex]);
    return i2cStatus;
}


static status_t I2cTransferStatusCheck(status_t i2cStatus)
{
    if(i2cStatus == kStatus_Success)
    {
        return I2cTransferStatus_FullTransfer;
    }
    else if(i2cStatus == kStatus_LPI2C_Nak)
    {
        return I2cTransferStatus_SlaveAddressNotAcknowledged;
    }
    else if(i2cStatus == kStatus_LPI2C_PinLowTimeout)
    {
        return I2cTransferStatus_ClockStretchTimeout;
    }
    else if(i2cStatus == kStatus_LPI2C_NoTransferInProgress)
    {
        return I2cTransferStatus_PartialTransfer;
    }
    else
    {
        return I2cTransferStatus_UnknownError;
    }
}



static void I2C_ClearBuffers(i2c_structure_t *pI2Cx)
{
    pI2Cx->txBuffer = 0;
    pI2Cx->txSize   = 0;
    pI2Cx->rxBuffer = 0;
    pI2Cx->rxSize   = 0;
}


HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        i2c_structure_t *pI2Cx;
        uint8_t busIndex;

        // Get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Get a pointer to the managed I2C connectionSettings object instance
        CLR_RT_HeapBlock *pConfig = pThis[ FIELD___connectionSettings ].Dereference();

        // Get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the I2cDevice() constructor in managed code for details
        busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);
        pI2Cx = I2C_ChoosePeripheralStructure(busIndex);
        I2C_InitPins(busIndex);
        I2C_InitClocks();
        
        
        pI2Cx->slaveAddress = pConfig[ I2cConnectionSettings::FIELD___slaveAddress].NumericByRef().s4;
        LPI2C_MasterGetDefaultConfig(&(pI2Cx->masterConfig));
        LPI2C_MasterInit(i2cBaseAddress[busIndex], &(pI2Cx->masterConfig), LPI2C_CLOCK_FREQUENCY);
    }
    NANOCLR_NOCLEANUP();
}


HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeDispose___VOID__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        uint8_t busIndex;
        bool disposeController = false;

        // Get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();     FAULT_ON_NULL(pThis);

        disposeController = (bool)stack.Arg0().NumericByRef().u1;

        if(disposeController)
        {
            busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);
            LPI2C_MasterDeinit(i2cBaseAddress[busIndex]);
        }
        
    }
    NANOCLR_NOCLEANUP();
}


HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        i2c_structure_t *pI2Cx;
        status_t i2cStatus     = kStatus_Fail;

        CLR_RT_HeapBlock_Array *writeBuffer;
        CLR_RT_HeapBlock_Array *readBuffer;
        CLR_RT_HeapBlock       *result;

        // Get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();  FAULT_ON_NULL(pThis);
        CLR_RT_HeapBlock &top   = stack.PushValueAndClear();

        // Get bus index
        // This is coded with a multiplication, need to perform and int division to get the number
        // See the comments in the I2cDevice() constructor in managed code for details
        uint8_t busIndex = (uint8_t)(pThis[ FIELD___deviceId ].NumericByRef().s4 / 1000);
        pI2Cx = I2C_ChoosePeripheralStructure(busIndex);

        I2C_ClearBuffers(pI2Cx);
        // Dereference the write buffer from the arguments   
        writeBuffer = stack.Arg1().DereferenceArray();
        if(writeBuffer != NULL)
        {
            pI2Cx->txBuffer = writeBuffer->GetFirstElement();
            pI2Cx->txSize   = writeBuffer->m_numOfElements;
        }

        readBuffer = stack.Arg2().DereferenceArray();
        if(readBuffer != NULL)
        {
            pI2Cx->rxBuffer = readBuffer->GetFirstElement();
            pI2Cx->rxSize   = readBuffer->m_numOfElements;
        }

        
        // Start writing/reading procedure
        if(pI2Cx->rxSize != 0 && pI2Cx->txSize != 0)
        {
            i2cStatus = I2C_ReadProcedure(busIndex, pI2Cx);
            if(i2cStatus != kStatus_Success)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }
        else if(pI2Cx->txSize != 0)
        {
            i2cStatus = I2C_WriteProcedure(busIndex, pI2Cx);
            if(i2cStatus != kStatus_Success)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }


        // Create return object
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));
        result = top.Dereference(); FAULT_ON_NULL(result);

        uint32_t transferResult = I2cTransferStatusCheck(i2cStatus);
        result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status ].SetInteger((CLR_UINT32)transferResult);
        result[ Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___bytesTransferred ].SetInteger((CLR_UINT32)(pI2Cx->txSize + pI2Cx->rxSize));
        
    }
    NANOCLR_NOCLEANUP();
}




