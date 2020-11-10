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

extern i2c_structure_t i2c1, i2c2, i2c3;

uint8_t I2C1_DeviceCounter;
uint8_t I2C2_DeviceCounter;
uint8_t I2C3_DeviceCounter;

static LPI2C_Type *i2cBaseAddress[] = LPI2C_BASE_PTRS;

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

// need to declare these as external
// TODO move them here after Windows.Devices.I2c is removed
extern i2c_structure_t *I2C_ChoosePeripheralStructure(uint8_t busIndex);
extern void I2C_InitPins(uint8_t busIndex);
extern void I2C_InitClocks(void);
extern status_t I2C_WriteProcedure(uint8_t busIndex, i2c_structure_t *pI2Cx);
extern status_t I2C_ReadProcedure(uint8_t busIndex, i2c_structure_t *pI2Cx);
extern status_t I2cTransferStatusCheck(status_t i2cStatus);
extern void I2C_ClearBuffers(i2c_structure_t *pI2Cx);

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        i2c_structure_t *pI2Cx;
        uint8_t busIndex;

        // Get a pointer to the managed object instance and check that it's not NULL
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

        // Get a pointer to the managed object instance and check that it's not NULL
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
        CLR_RT_HeapBlock_Array *writeBuffer = NULL;
        CLR_RT_HeapBlock_Array *readBuffer = NULL;
        CLR_RT_HeapBlock *result;
        CLR_RT_HeapBlock *pConfig;

        // Get a pointer to the managed object instance and check that it's not NULL
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
        if (writeSpanByte != NULL)
        {
            // get buffer
            writeBuffer = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
            if (writeBuffer != NULL)
            {
                pI2Cx->txBuffer = writeBuffer->GetFirstElement();
                pI2Cx->txSize = writeBuffer->m_numOfElements;
            }
        }

        if (writeBuffer == NULL)
        {
            pI2Cx->txBuffer = NULL;
            pI2Cx->txSize = 0;
        }

        readSpanByte = stack.Arg2().Dereference();
        if (readSpanByte != NULL)
        {
            // get buffer
            readBuffer = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
            if (readBuffer != NULL)
            {
                pI2Cx->rxBuffer = readBuffer->GetFirstElement();
                pI2Cx->rxSize = readBuffer->m_numOfElements;
            }
        }

        if (readBuffer == NULL)
        {
            pI2Cx->rxBuffer = NULL;
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
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult));
        result = top.Dereference();
        FAULT_ON_NULL(result);

        uint32_t transferResult = I2cTransferStatusCheck(i2cStatus);
        result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)transferResult);
        result[I2cTransferResult::FIELD___bytesTransferred].SetInteger((CLR_UINT32)(pI2Cx->txSize + pI2Cx->rxSize));
    }
    NANOCLR_NOCLEANUP();
}
