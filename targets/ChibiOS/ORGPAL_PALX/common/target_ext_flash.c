//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// W25Q512 QSPI flash driver implementation for ORGPAL_PALX (STM32F769, QSPI).
//
// Implements the W25Q512_* API declared in target_ext_flash.h.
// Used by both the littlefs driver (target_littlefs.c) and the MCUboot
// flash map porting layer (mcuboot_flash_map.c).
//
// Extracted from target_littlefs.c.  The public QSPI_Read / QSPI_Write /
// QSPI_Erase_Block functions that were previously in that file are replaced
// by the W25Q512_* API.  W25Q512_Erase() unifies 4 kB sector erase (littlefs)
// and 64 kB block erase (MCUboot) behind a single entry point.

#include <ch.h>
#include <hal.h>
#include <hal_stm32_qspi.h>
#include <nanoHAL_v2.h>

#include "target_ext_flash.h"

// W25Q512 command bytes and device constants (internal to this translation unit)
#ifndef HAL_QSPI_TIMEOUT_DEFAULT_VALUE
#define HAL_QSPI_TIMEOUT_DEFAULT_VALUE ((uint32_t)4000)
#endif
#define HAL_QPSI_TIMEOUT_CONFIG_COMMAND ((uint32_t)10)

#define RESET_ENABLE_CMD       0x66U
#define RESET_MEMORY_CMD       0x99U
#define READ_ID_CMD            0x90U
#define READ_ID_CMD2           0x9FU
#define QUAD_OUT_FAST_READ_CMD 0x6BU
#define WRITE_ENABLE_CMD       0x06U
#define READ_STATUS_REG1_CMD   0x05U
#define READ_STATUS_REG2_CMD   0x35U
#define WRITE_STATUS_REG2_CMD  0x31U
#define PAGE_PROG_CMD          0x02U
#define QUAD_IN_FAST_PROG_CMD  0x32U
#define SECTOR_ERASE_CMD       0x20U
#define BLOCK_ERASE_CMD        0xD8U

#define W25Q512_DUMMY_CYCLES_READ      8U
#define W25Q512_DUMMY_CYCLES_READ_QUAD 8U
#define W25Q512_BULK_ERASE_MAX_TIME    10000U
#define W25Q512_BLOCK_ERASE_MAX_TIME   3000U
#define W25Q512_SECTOR_ERASE_MAX_TIME  800U

#define W25Q512_SR_WIP      ((uint8_t)0x01)
#define W25Q512_SR_WREN     ((uint8_t)0x02)
#define W25Q512_SR_BLOCKPR  ((uint8_t)0x5C)
#define W25Q512_SR_PRBOTTOM ((uint8_t)0x20)
#define W25Q512_SR2_QE      ((uint8_t)0x02)

#define W25Q512_MANUFACTURER_ID ((uint8_t)0xEF)
#define W25Q512_DEVICE_ID1      ((uint8_t)0x40)
#define W25Q512_DEVICE_ID2      ((uint8_t)0x20)

///////////////
// Static helpers (internal to this translation unit)

static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_EnterMemory_QPI(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);
static uint8_t QSPI_ReadChipID(QSPI_HandleTypeDef *hqspi, uint8_t *buffer);

///////////////
// Public API

bool W25Q512_Init(void)
{
    // Configure MPU for QSPI memory region.
    // Required on STM32F7 to avoid QSPI BUSY flag issue with indirect access.
    // https://community.st.com/t5/stm32-mcus-products/qspi-flag-qspi-flag-busy-sometimes-stays-set/td-p/365865
    HAL_MPU_Disable();

    MPU_Region_InitTypeDef MPU_InitStruct;
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.BaseAddress = 0x90000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_64MB;
    MPU_InitStruct.SubRegionDisable = 0x0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

    // Initialise QSPI peripheral.
    QSPID1.Init.ClockPrescaler = 1;
    QSPID1.Init.FifoThreshold = 4;
    QSPID1.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    QSPID1.Init.FlashSize = POSITION_VAL(W25Q512_FLASH_SIZE) - 1;
    QSPID1.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE;
    QSPID1.Init.ClockMode = QSPI_CLOCK_MODE_0;
    QSPID1.Init.FlashID = QSPI_FLASH_ID_1;
    QSPID1.Init.DualFlash = QSPI_DUALFLASH_DISABLE;

    qspiStart(&QSPID1);

    if (HAL_QSPI_Init(&QSPID1) != HAL_OK)
    {
        return false;
    }

    if (QSPI_ResetMemory(&QSPID1) != QSPI_OK)
    {
        return false;
    }

    if (QSPI_EnterMemory_QPI(&QSPID1) != QSPI_OK)
    {
        return false;
    }

    // Verify JEDEC ID (6-byte read; check first 3 bytes).
    uint8_t dataBuffer[6];
    if (QSPI_ReadChipID(&QSPID1, dataBuffer) != QSPI_OK)
    {
        return false;
    }

    ASSERT(dataBuffer[0] == W25Q512_MANUFACTURER_ID);
    ASSERT(dataBuffer[1] == W25Q512_DEVICE_ID1);
    ASSERT(dataBuffer[2] == W25Q512_DEVICE_ID2);

    // W25Q512 datasheet: Time Delay Before Write Instruction > 5 ms.
    chThdSleepMilliseconds(10);

    return true;
}

bool W25Q512_Erase(uint32_t addr, bool use_64k)
{
    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = use_64k ? BLOCK_ERASE_CMD : SECTOR_ERASE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.Address = addr;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (QSPI_WriteEnable(&QSPID1) != QSPI_OK)
    {
        return false;
    }

    if (HAL_QSPI_Command(&QSPID1, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    uint32_t timeout = use_64k ? W25Q512_BLOCK_ERASE_MAX_TIME : W25Q512_SECTOR_ERASE_MAX_TIME;
    if (QSPI_AutoPollingMemReady(&QSPID1, timeout) != QSPI_OK)
    {
        return false;
    }

    return true;
}

bool W25Q512_Read(uint8_t *pData, uint32_t readAddr, uint32_t size)
{
    QSPI_CommandTypeDef s_command;
    HAL_StatusTypeDef status;
    uint8_t retryCounter = 3;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = QUAD_OUT_FAST_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.Address = readAddr;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_4_LINES;
    s_command.DummyCycles = W25Q512_DUMMY_CYCLES_READ_QUAD;
    s_command.NbData = size;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

config_command:
    status = HAL_QSPI_Command(&QSPID1, &s_command, HAL_QPSI_TIMEOUT_CONFIG_COMMAND);
    if (status != HAL_OK)
    {
        if (retryCounter-- == 0)
        {
            return false;
        }
        // Clear QSPI busy bit and retry.
        QSPID1.State = HAL_QSPI_STATE_BUSY;
        HAL_QSPI_Abort(&QSPID1);
        goto config_command;
    }

    // Set S# timing for Read command.
    MODIFY_REG(QSPID1.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_2_CYCLE);

    status = HAL_QSPI_Receive(&QSPID1, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    // Restore S# timing for non-Read commands.
    MODIFY_REG(QSPID1.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_5_CYCLE);

    return status == HAL_OK;
}

bool W25Q512_Write(uint8_t *pData, uint32_t writeAddr, uint32_t size)
{
    QSPI_CommandTypeDef s_command;
    uint32_t current_size, current_addr, end_addr;

    current_size = W25Q512_PAGE_SIZE - (writeAddr % W25Q512_PAGE_SIZE);
    if (current_size > size)
    {
        current_size = size;
    }

    current_addr = writeAddr;
    end_addr = writeAddr + size;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = PAGE_PROG_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    do
    {
        s_command.Address = current_addr;
        s_command.NbData = current_size;

        if (QSPI_WriteEnable(&QSPID1) != QSPI_OK)
        {
            return false;
        }

        if (HAL_QSPI_Command(&QSPID1, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return false;
        }

        if (HAL_QSPI_Transmit(&QSPID1, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return false;
        }

        if (QSPI_AutoPollingMemReady(&QSPID1, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
        {
            return false;
        }

        current_addr += current_size;
        pData += current_size;
        current_size = ((current_addr + W25Q512_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q512_PAGE_SIZE;
    } while (current_addr < end_addr);

    return true;
}

bool W25Q512_EraseChip(void)
{
    // Erase sector by sector to allow watchdog resets between operations.
    for (uint32_t i = 0; i < W25Q512_FLASH_SIZE / W25Q512_SECTOR_SIZE; i++)
    {
        if (!W25Q512_Erase(i * W25Q512_SECTOR_SIZE, false))
        {
            return false;
        }
        Watchdog_Reset();
    }
    return true;
}

///////////////
// Static helpers

static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = RESET_ENABLE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    s_command.Instruction = RESET_MEMORY_CMD;
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    if (QSPI_AutoPollingMemReady(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

static uint8_t QSPI_EnterMemory_QPI(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;
    uint8_t reg[] = {0};

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = READ_STATUS_REG2_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 1;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    if (HAL_QSPI_Receive(hqspi, &reg[0], HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    if (QSPI_WriteEnable(hqspi) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    s_command.Instruction = WRITE_STATUS_REG2_CMD;
    MODIFY_REG(reg[0], 0, W25Q512_SR2_QE);

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    if (HAL_QSPI_Transmit(hqspi, &reg[0], HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // Read back to confirm Quad Enable bit was set.
    s_command.Instruction = READ_STATUS_REG2_CMD;
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    if (HAL_QSPI_Receive(hqspi, &reg[0], HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return (reg[0] & W25Q512_SR2_QE) ? QSPI_OK : QSPI_ERROR;
}

static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = WRITE_ENABLE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    s_config.Match = W25Q512_SR_WREN;
    s_config.Mask = W25Q512_SR_WREN;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = READ_STATUS_REG1_CMD;
    s_command.DataMode = QSPI_DATA_1_LINE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef sConfig;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = READ_STATUS_REG1_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.Match = 0;
    sConfig.Mask = W25Q512_SR_WIP;
    sConfig.MatchMode = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval = 0x10;
    sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &sConfig, Timeout) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

static uint8_t QSPI_ReadChipID(QSPI_HandleTypeDef *hqspi, uint8_t *buffer)
{
    QSPI_CommandTypeDef s_command;

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = READ_ID_CMD2;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = 6;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    if (HAL_QSPI_Receive(hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}
