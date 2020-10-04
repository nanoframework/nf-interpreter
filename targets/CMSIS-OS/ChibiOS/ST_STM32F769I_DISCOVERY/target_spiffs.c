//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_spiffs.h>
#include <hal_stm32_qspi.h>
#include "target_spiffs.h"
#include <nanoHAL_v2.h>

static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_EnterMemory_QPI(QSPI_HandleTypeDef *hqspi);
// static uint8_t QSPI_ExitMemory_QPI(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_OutDrvStrengthCfg(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_AutoPollingMemReady  (QSPI_HandleTypeDef *hqspi, uint32_t Timeout);
static uint8_t QSPI_ReadChipID(QSPI_HandleTypeDef *hqspi, uint8_t* buffer);

uint8_t QSPI_Read(uint8_t* pData, uint32_t readAddr, uint32_t size);
uint8_t QSPI_Write(uint8_t* pData, uint32_t writeAddr, uint32_t size);
uint8_t QSPI_Erase_Block(uint32_t blockAddress);

// initialization of everything required for SPIFFS
// for this target is the QSPI driver
uint8_t target_spiffs_init()
{
    uint8_t device_id[3] = { 0, 0, 0 };
   
    /* QSPI initialization */
    /* QSPI freq = SYSCLK /(1 + ClockPrescaler) = 216 MHz/(1+1) = 108 Mhz */
    QSPID1.Init.ClockPrescaler     = 1;   /* QSPI freq = 216 MHz/(1+1) = 108 Mhz */
    QSPID1.Init.FifoThreshold      = 16;
    QSPID1.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE; 
    QSPID1.Init.FlashSize          = POSITION_VAL(MX25L512_FLASH_SIZE) - 1;
    QSPID1.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE; /* Min 30ns for nonRead */
    QSPID1.Init.ClockMode          = QSPI_CLOCK_MODE_0;
    QSPID1.Init.FlashID            = QSPI_FLASH_ID_1;
    QSPID1.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;

    // init driver
    qspiStart(&QSPID1);

    if (HAL_QSPI_Init(&QSPID1) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* QSPI memory reset */
    if(QSPI_ResetMemory( &QSPID1 ) != QSPI_OK)
    {
        return QSPI_NOT_SUPPORTED;
    }

    /* Put QSPI memory in QPI mode */
    if(QSPI_EnterMemory_QPI( &QSPID1 )!=QSPI_OK )
    {
        return QSPI_NOT_SUPPORTED;
    }

    /* Set the QSPI memory in 4-bytes address mode */
    if(QSPI_EnterFourBytesAddress(&QSPID1) != QSPI_OK)
    {
        return QSPI_NOT_SUPPORTED;
    }

    /* Configuration of the dummy cycles on QSPI memory side */
    if (QSPI_DummyCyclesCfg(&QSPID1) != QSPI_OK)
    {
        return QSPI_NOT_SUPPORTED;
    }
    
    /* Configuration of the Output driver strength on memory side */
    if( QSPI_OutDrvStrengthCfg( &QSPID1 ) != QSPI_OK )
    {
        return QSPI_NOT_SUPPORTED;
    }

    HAL_Delay(10);

    // sanity check: read device ID and unique ID
    if( QSPI_ReadChipID( &QSPID1, device_id ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }
    
    // // invalidate cache over read buffer to ensure that content from DMA is read
    // // (only required for Cortex-M7)
    // cacheBufferInvalidate(device_id, 3);

    // constants from ID Definitions table in MX25L51245G datasheet
    ASSERT(device_id[0] == MX25L512_MANUFACTURER_ID);
    ASSERT(device_id[1] == MX25L512_DEVICE_ID_MEM_TYPE);
    ASSERT(device_id[2] == MX25L512_DEVICE_ID_MEM_CAPACITY);

    return QSPI_OK;
}

// target specific implementation of hal_spiffs_erase
s32_t hal_spiffs_erase(u32_t addr, u32_t size)
{
    uint32_t i = 0;

    // how many sectors need to be erased?
    uint32_t erase_count = (size + SPIFFS_ERASE_BLOCK_SIZE - 1) / SPIFFS_ERASE_BLOCK_SIZE;

    for (i = 0; i < erase_count; i++)
    {
        if( QSPI_Erase_Block(addr) != QSPI_OK)
        {
            return SPIFFS_ERROR;
        }

        // adjust sector address
        addr += i * SPIFFS_ERASE_BLOCK_SIZE;
    }    

    return SPIFFS_SUCCESS;
}

// target specific implementation of hal_spiffs_read
s32_t hal_spiffs_read(u32_t addr, u32_t size, u8_t *dst)
{
    if(QSPI_Read(dst, addr, size) != QSPI_OK)
    {
        return SPIFFS_ERROR;
    }

    return SPIFFS_SUCCESS;
}

// target specific implementation of hal_spiffs_write
s32_t hal_spiffs_write(u32_t addr, u32_t size, u8_t *src)
{
    if( QSPI_Write(src, addr, size) != QSPI_OK)
    {
        return SPIFFS_ERROR;
    }

    return SPIFFS_SUCCESS;
}

static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef      s_command;
    QSPI_AutoPollingTypeDef  s_config;
    uint8_t                  reg;

    /* Send command RESET command in QPI mode (QUAD I/Os) */
    /* Initialize the reset enable command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = RESET_ENABLE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Send the reset memory command */
    s_command.Instruction = RESET_MEMORY_CMD;
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }  

    /* Send command RESET command in SPI mode */
    /* Initialize the reset enable command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = RESET_ENABLE_CMD;
    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    /* Send the reset memory command */
    s_command.Instruction = RESET_MEMORY_CMD;
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* After reset CMD, 1000ms requested if QSPI memory SWReset occured during full chip erase operation */
    HAL_Delay( 1000 );

    /* Configure automatic polling mode to wait the WIP bit=0 */
    s_config.Match           = 0;
    s_config.Mask            = MX25L512_SR_WIP;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction     = READ_STATUS_REG_CMD;
    s_command.DataMode        = QSPI_DATA_1_LINE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Initialize the reading of status register */
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.NbData            = 1;

    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Enable write operations, command in 1 bit */
    /* Enable write operations */
    s_command.Instruction       = WRITE_ENABLE_CMD;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 0;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for write enabling */  
    s_config.Match           = MX25L512_SR_WREN;
    s_config.Mask            = MX25L512_SR_WREN;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction    = READ_STATUS_REG_CMD;
    s_command.DataMode       = QSPI_DATA_1_LINE;
    s_command.NbData         = 0;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Update the configuration register with new dummy cycles */
    s_command.Instruction       = WRITE_STATUS_CFG_REG_CMD;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.NbData            = 1;

    /* Enable the Quad IO on the QSPI memory (Non-volatile bit) */
    reg |= MX25L512_SR_QUADEN;

    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Transmission of the data */
    if (HAL_QSPI_Transmit(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* 40ms  Write Status/Configuration Register Cycle Time */
    HAL_Delay( 40 );

    // read back register to check MX25L512_SR_QUADEN 
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 1;

    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Reception of the data */
    if (HAL_QSPI_Receive(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    if(reg & MX25L512_SR_QUADEN)
    {
        return QSPI_OK;
    }
    else
    {
        return QSPI_ERROR;
    }    
}

static uint8_t QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;
    uint8_t reg[2];
    
    /* Initialize the reading of status register */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 1;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    
    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Reception of the data */
    if (HAL_QSPI_Receive(hqspi, &(reg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Initialize the reading of configuration register */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = READ_CFG_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 1;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    
    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Reception of the data */
    if (HAL_QSPI_Receive(hqspi, &(reg[1]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Enable write operations */
    if (QSPI_WriteEnable(hqspi) != QSPI_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Update the configuration register with new dummy cycles */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = WRITE_STATUS_CFG_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 2;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    
    /* MX25L512_DUMMY_CYCLES_READ_QUAD = 3 for 10 cycles in QPI mode */
    MODIFY_REG( reg[1], MX25L512_CR_NB_DUMMY, (MX25L512_DUMMY_CYCLES_READ_QUAD << POSITION_VAL(MX25L512_CR_NB_DUMMY)));
    
    /* Configure the write volatile configuration register command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Transmission of the data */
    if (HAL_QSPI_Transmit(hqspi, &(reg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* 40ms  Write Status/Configuration Register Cycle Time */
    HAL_Delay( 40 );  
    
    return QSPI_OK;
}

static uint8_t QSPI_EnterMemory_QPI( QSPI_HandleTypeDef *hqspi )
{
    QSPI_CommandTypeDef      s_command;
    QSPI_AutoPollingTypeDef  s_config;

    /* Initialize the QPI enable command */
    /* QSPI memory is supported to be in SPI mode, so CMD on 1 LINE */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;  
    s_command.Instruction       = ENTER_QUAD_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait the QUADEN bit=1 and WIP bit=0 */
    s_config.Match           = MX25L512_SR_QUADEN;
    s_config.Mask            = MX25L512_SR_QUADEN|MX25L512_SR_WIP;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.DataMode          = QSPI_DATA_4_LINES;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

// static uint8_t QSPI_ExitMemory_QPI( QSPI_HandleTypeDef *hqspi)
// {
//     QSPI_CommandTypeDef      s_command;

//     /* Initialize the QPI enable command */
//     /* QSPI memory is supported to be in QPI mode, so CMD on 4 LINES */
//     s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;  
//     s_command.Instruction       = EXIT_QUAD_CMD;
//     s_command.AddressMode       = QSPI_ADDRESS_NONE;
//     s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//     s_command.DataMode          = QSPI_DATA_NONE;
//     s_command.DummyCycles       = 0;
//     s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//     s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//     s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//     /* Send the command */
//     if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//     {
//         return QSPI_ERROR;
//     }

//     return QSPI_OK;
// }

static uint8_t QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = ENTER_4_BYTE_ADDR_MODE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if (QSPI_WriteEnable(hqspi) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait the memory is ready */  
    if (QSPI_AutoPollingMemReady(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
   
    //HAL_QSPI_Abort(&QSPID1);

    QSPI_CommandTypeDef     s_command;
    QSPI_AutoPollingTypeDef s_config;
    
    /* Enable write operations */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = WRITE_ENABLE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for write enabling */  
    s_config.Match           = MX25L512_SR_WREN;
    s_config.Mask            = MX25L512_SR_WREN;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
    
    s_command.Instruction    = READ_STATUS_REG_CMD;
    s_command.DataMode       = QSPI_DATA_4_LINES;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    return QSPI_OK;
}

static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
    QSPI_CommandTypeDef     s_command;
    QSPI_AutoPollingTypeDef s_config;

    /* Configure automatic polling mode to wait for memory ready */  
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    s_config.Match           = 0;
    s_config.Mask            = MX25L512_SR_WIP;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, Timeout) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

static uint8_t QSPI_OutDrvStrengthCfg( QSPI_HandleTypeDef *hqspi )
{
    QSPI_CommandTypeDef s_command;
    uint8_t reg[2];

    /* Initialize the reading of status register */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 1;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(hqspi, &(reg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Initialize the reading of configuration register */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = READ_CFG_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 1;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(hqspi, &(reg[1]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Enable write operations */
    if (QSPI_WriteEnable(&QSPID1) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Update the configuration register with new output driver strength */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = WRITE_STATUS_CFG_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 2;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Set Output Strength of the QSPI memory 15 ohms */
    MODIFY_REG( reg[1], MX25L512_CR_ODS, (MX25L512_CR_ODS_15 << POSITION_VAL(MX25L512_CR_ODS)));

    /* Configure the write volatile configuration register command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Transmission of the data */
    if (HAL_QSPI_Transmit(hqspi, &(reg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

static uint8_t QSPI_ReadChipID(QSPI_HandleTypeDef *hqspi, uint8_t* buffer)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the reading of status register */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = MULTIPLE_IO_READ_ID_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 3;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

uint8_t QSPI_Read(uint8_t* pData, uint32_t readAddr, uint32_t size)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the read command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = QSPI_READ_4_BYTE_ADDR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
    s_command.Address           = readAddr;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = MX25L512_DUMMY_CYCLES_READ_QUAD_IO;
    s_command.NbData            = size;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    
    /* Configure the command */
    if (HAL_QSPI_Command(&QSPID1, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Set S# timing for Read command */
    MODIFY_REG(QSPID1.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_1_CYCLE);

    /* Reception of the data */
    if (HAL_QSPI_Receive(&QSPID1, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    // // invalidate cache over read buffer to ensure that content from DMA is read
    // // (only required for Cortex-M7)
    // cacheBufferInvalidate(pData, size);

    /* Restore S# timing for nonRead commands */
    MODIFY_REG(QSPID1.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_4_CYCLE);

SET_BIT(QSPID1.Instance->CR, QUADSPI_CR_ABORT);

    return QSPI_OK;
}

uint8_t QSPI_Write(uint8_t* pData, uint32_t writeAddr, uint32_t size)
{
    QSPI_CommandTypeDef s_command;
    uint32_t end_addr, current_size, current_addr;

    /* Calculation of the size between the write address and the end of the page */
    current_size = MX25L512_PAGE_SIZE - (writeAddr % MX25L512_PAGE_SIZE);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > size)
    {
        current_size = size;
    }

    /* Initialize the address variables */
    current_addr = writeAddr;
    end_addr = writeAddr + size;

    /* Initialize the program command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = PAGE_PROG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    
    /* Perform the write page by page */
    do
    {
        s_command.Address = current_addr;
        s_command.NbData  = current_size;

        /* Enable write operations */
        if (QSPI_WriteEnable(&QSPID1) != QSPI_OK)
        {
            return QSPI_ERROR;
        }
        
        /* Configure the command */
        if (HAL_QSPI_Command(&QSPID1, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return QSPI_ERROR;
        }
        
        // // flush DMA buffer to ensure cache coherency
        // // (only required for Cortex-M7)
        // cacheBufferFlush(pData, current_size);

        /* Transmission of the data */
        if (HAL_QSPI_Transmit(&QSPID1, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return QSPI_ERROR;
        }
        
        /* Configure automatic polling mode to wait for end of program */  
        if (QSPI_AutoPollingMemReady(&QSPID1, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
        {
            return QSPI_ERROR;
        }
        
        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        pData += current_size;
        current_size = ((current_addr + MX25L512_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : MX25L512_PAGE_SIZE;

    }
    while (current_addr < end_addr);
    
    return QSPI_OK;
}

uint8_t QSPI_Erase_Block(uint32_t blockAddress)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction       = SECTOR_ERASE_4_BYTE_ADDR_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
    s_command.Address           = blockAddress;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if (QSPI_WriteEnable(&QSPID1) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Send the command */
    if (HAL_QSPI_Command(&QSPID1, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for end of erase */  
    if (QSPI_AutoPollingMemReady(&QSPID1, MX25L512_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}
