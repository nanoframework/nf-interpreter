//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_native_target.h"

/* Select USB1 PLL (480 MHz) as master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_DIVIDER (5U)
/* Get frequency of lpi2c clock */
#define LPI2C_CLOCK_FREQUENCY ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (LPI2C_CLOCK_SOURCE_DIVIDER + 1U))

typedef struct
{
    lpi2c_master_config_t masterConfig;
    uint8_t slaveAddress;
    uint8_t *txBuffer;
    int txSize;
    uint8_t *rxBuffer;
    int rxSize;
} i2c_structure_t;

i2c_structure_t i2c1, i2c2, i2c3;

uint8_t I2C1_DeviceCounter;
uint8_t I2C2_DeviceCounter;
uint8_t I2C3_DeviceCounter;

static LPI2C_Type *i2cBaseAddress[] = LPI2C_BASE_PTRS;

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

i2c_structure_t *I2C_ChoosePeripheralStructure(uint8_t busIndex)
{
    if (busIndex == 1)
    {
        return &i2c1;
    }
    else if (busIndex == 2)
    {
        return &i2c2;
    }
    else if (busIndex == 3)
    {
        return &i2c3;
    }
    else
    {
        return nullptr;
    }
}

void I2C_InitPins(uint8_t busIndex)
{
    switch (busIndex)
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

void I2C_InitClocks(void)
{
    CLOCK_EnableClock(kCLOCK_Iomuxc);
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 0);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 5);
}

status_t I2C_WriteProcedure(uint8_t busIndex, i2c_structure_t *pI2Cx)
{
    status_t i2cStatus = kStatus_Fail;
    i2cStatus = LPI2C_MasterStart(i2cBaseAddress[busIndex], pI2Cx->slaveAddress, kLPI2C_Write);
    if (i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    i2cStatus = LPI2C_MasterSend(i2cBaseAddress[busIndex], pI2Cx->txBuffer, pI2Cx->txSize);
    if (i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    LPI2C_MasterStop(i2cBaseAddress[busIndex]);
    return i2cStatus;
}

status_t I2C_ReadProcedure(uint8_t busIndex, i2c_structure_t *pI2Cx)
{
    status_t i2cStatus = kStatus_Fail;
    i2cStatus = LPI2C_MasterStart(i2cBaseAddress[busIndex], pI2Cx->slaveAddress, kLPI2C_Write);
    if (i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    i2cStatus = LPI2C_MasterSend(i2cBaseAddress[busIndex], pI2Cx->txBuffer, pI2Cx->txSize);
    if (i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    i2cStatus = LPI2C_MasterStart(i2cBaseAddress[busIndex], pI2Cx->slaveAddress, kLPI2C_Read);
    if (i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    i2cStatus = LPI2C_MasterReceive(i2cBaseAddress[busIndex], pI2Cx->rxBuffer, pI2Cx->rxSize);
    if (i2cStatus != kStatus_Success)
    {
        return i2cStatus;
    }
    i2cStatus = LPI2C_MasterStop(i2cBaseAddress[busIndex]);
    return i2cStatus;
}

status_t I2cTransferStatusCheck(status_t i2cStatus)
{
    if (i2cStatus == kStatus_Success)
    {
        return I2cTransferStatus_FullTransfer;
    }
    else if (i2cStatus == kStatus_LPI2C_Nak)
    {
        return I2cTransferStatus_SlaveAddressNotAcknowledged;
    }
    else if (i2cStatus == kStatus_LPI2C_PinLowTimeout)
    {
        return I2cTransferStatus_ClockStretchTimeout;
    }
    else if (i2cStatus == kStatus_LPI2C_NoTransferInProgress)
    {
        return I2cTransferStatus_PartialTransfer;
    }
    else
    {
        return I2cTransferStatus_UnknownError;
    }
}

void I2C_ClearBuffers(i2c_structure_t *pI2Cx)
{
    pI2Cx->txBuffer = 0;
    pI2Cx->txSize = 0;
    pI2Cx->rxBuffer = 0;
    pI2Cx->rxSize = 0;
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        i2c_structure_t *pI2Cx;
        uint8_t busIndex;

        // Get a pointer to the managed object instance and check that it's not nullptr
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Get a pointer to the managed I2C connectionSettings object instance
        CLR_RT_HeapBlock *pConfig = pThis[FIELD___connectionSettings].Dereference();

        // Get bus index
        busIndex = (uint8_t)pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        pI2Cx = I2C_ChoosePeripheralStructure(busIndex);

        I2C_InitPins(busIndex);
        I2C_InitClocks();

        switch (busIndex)
        {
            case 1:
                // increase device counter
                I2C1_DeviceCounter++;
                break;

            case 2:
                // increase device counter
                I2C2_DeviceCounter++;
                break;

            case 3:
                // increase device counter
                I2C3_DeviceCounter++;
                break;
        }

        pI2Cx->slaveAddress = pConfig[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;
        LPI2C_MasterGetDefaultConfig(&(pI2Cx->masterConfig));
        LPI2C_MasterInit(i2cBaseAddress[busIndex], &(pI2Cx->masterConfig), LPI2C_CLOCK_FREQUENCY);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint8_t busIndex;
        bool disposeController = false;

        CLR_RT_HeapBlock *pConfig;

        // Get a pointer to the managed object instance and check that it's not nullptr
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a pointer to the managed I2C connectionSettings object instance
        pConfig = pThis[FIELD___connectionSettings].Dereference();

        // Get bus index
        busIndex = (uint8_t)pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        switch (busIndex)
        {
            case 1:
                // decrease device counter
                I2C1_DeviceCounter--;

                if (I2C1_DeviceCounter == 0)
                {
                    // no more devices on the bus
                    disposeController = true;
                }
                break;

            case 2:
                // decrease device counter
                I2C2_DeviceCounter--;

                if (I2C2_DeviceCounter == 0)
                {
                    // no more devices on the bus
                    disposeController = true;
                }

                break;

            case 3:
                // decrease device counter
                I2C3_DeviceCounter--;

                if (I2C3_DeviceCounter == 0)
                {
                    // no more devices on the bus
                    disposeController = true;
                }
                break;
        }

        if (disposeController)
        {
            // get bus index
            busIndex = (uint8_t)pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

            LPI2C_MasterDeinit(i2cBaseAddress[busIndex]);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::
    NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint8_t busIndex;
        i2c_structure_t *pI2Cx;
        status_t i2cStatus = kStatus_Fail;

        CLR_RT_HeapBlock *writeSpanByte;
        CLR_RT_HeapBlock *readSpanByte;
        CLR_RT_HeapBlock_Array *writeBuffer = nullptr;
        CLR_RT_HeapBlock_Array *readBuffer = nullptr;
        CLR_RT_HeapBlock *result;
        CLR_RT_HeapBlock *pConfig;

        // Get a pointer to the managed object instance and check that it's not nullptr
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        // get a pointer to the managed I2C connectionSettings object instance
        pConfig = pThis[FIELD___connectionSettings].Dereference();

        // Get bus index
        busIndex = (uint8_t)pConfig[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        pI2Cx = I2C_ChoosePeripheralStructure(busIndex);

        I2C_ClearBuffers(pI2Cx);

        // dereference the write and read SpanByte from the arguments
        writeSpanByte = stack.Arg1().Dereference();
        if (writeSpanByte != nullptr)
        {
            // get buffer
            writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
            if (writeBuffer != nullptr)
            {
                pI2Cx->txBuffer = writeBuffer->GetFirstElement();
                pI2Cx->txSize = writeBuffer->m_numOfElements;
            }
        }

        if (writeBuffer == nullptr)
        {
            pI2Cx->txBuffer = nullptr;
            pI2Cx->txSize = 0;
        }

        readSpanByte = stack.Arg2().Dereference();
        if (readSpanByte != nullptr)
        {
            // get buffer
            readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
            if (readBuffer != nullptr)
            {
                pI2Cx->rxBuffer = readBuffer->GetFirstElement();
                pI2Cx->rxSize = readBuffer->m_numOfElements;
            }
        }

        if (readBuffer == nullptr)
        {
            pI2Cx->rxBuffer = nullptr;
            pI2Cx->rxSize = 0;
        }

        // Start writing/reading procedure
        if (pI2Cx->rxSize != 0 && pI2Cx->txSize != 0)
        {
            i2cStatus = I2C_ReadProcedure(busIndex, pI2Cx);
            if (i2cStatus != kStatus_Success)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }
        else if (pI2Cx->txSize != 0)
        {
            i2cStatus = I2C_WriteProcedure(busIndex, pI2Cx);
            if (i2cStatus != kStatus_Success)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }

        // Create return object
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.I2cTransferResult));
        result = top.Dereference();
        FAULT_ON_NULL(result);

        uint32_t transferResult = I2cTransferStatusCheck(i2cStatus);
        result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)transferResult);
        result[I2cTransferResult::FIELD___bytesTransferred].SetInteger((CLR_UINT32)(pI2Cx->txSize + pI2Cx->rxSize));
    }
    NANOCLR_NOCLEANUP();
}
