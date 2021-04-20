//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_spiffs.h>
#include "target_spiffs.h"
#include <nanoHAL_v2.h>

#ifdef SPIFFS_SPI1

bool SPI_Erase_Block(u32_t addr);
bool SPI_Read(uint8_t* pData, uint32_t readAddr, uint32_t size);
bool SPI_Write(uint8_t* pData, uint32_t writeAddr, uint32_t size);

extern uint32_t HAL_GetTick(void);

static const SPIConfig spiConfig = {
  false,
  NULL,
  PAL_PORT(LINE_FLASH_SPI1_CS),
  PAL_PAD(LINE_FLASH_SPI1_CS),
  // CPHA=0, CPOL=0, MSb first
  0,//SPI_CR1_CPOL | SPI_CR1_BR_0,
  // transfer length to 8bit
  SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0
};

#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t writeBuffer[SPIFFS_LOGICAL_PAGE_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t readBuffer[SPIFFS_LOGICAL_PAGE_SIZE];

// initialization of everything required for SPIFFS
// for this target is the SPI driver
uint8_t target_spiffs_init()
{
    // init driver
    spiAcquireBus(&SPID1);
    spiStart(&SPID1, &spiConfig);

    ////////////////////////////////////////////////////////////////////////
    // no need to worry with cache issues at this early stage of the boot //
    ////////////////////////////////////////////////////////////////////////

    // resume from deep power down
    // have to send this to make sure device is functional after sleep
    writeBuffer[0] = RESUME_DEEP_PD_CMD;

    spiSelect(&SPID1);
    spiSend(&SPID1, 1, writeBuffer);
    spiUnselect(&SPID1);

    // sanity check: read device ID and unique ID
    writeBuffer[0] = READ_ID_CMD2;

    spiSelect(&SPID1);
    spiExchange(&SPID1, 4, writeBuffer, readBuffer);
    spiUnselect(&SPID1);

    // constants from ID Definitions table in AT25SF641 datasheet
    ASSERT(readBuffer[1] == AT25SF641_MANUFACTURER_ID);
    ASSERT(readBuffer[2] == AT25SF641_DEVICE_ID1);
    ASSERT(readBuffer[3] == AT25SF641_DEVICE_ID2);

    return 1;
}

// target specific implementation of hal_spiffs_erase
s32_t hal_spiffs_erase(u32_t addr, u32_t size)
{
    uint32_t i = 0;

    // how many sectors need to be erased?
    uint32_t erase_count = (size + SPIFFS_ERASE_BLOCK_SIZE - 1) / SPIFFS_ERASE_BLOCK_SIZE;

    for (i = 0; i < erase_count; i++)
    {
        Watchdog_Reset();

        if( !SPI_Erase_Block(addr) )
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
    if( !SPI_Read(dst, addr, size))
    {
        return SPIFFS_ERROR;
    }

    return SPIFFS_SUCCESS;
}

// target specific implementation of hal_spiffs_write
s32_t hal_spiffs_write(u32_t addr, u32_t size, u8_t *src)
{
    if( !SPI_Write(src, addr, size) )
    {
        return SPIFFS_ERROR;
    }

    return SPIFFS_SUCCESS;
}

bool SPI_WaitOnBusy()
{
    uint32_t tickstart = HAL_GetTick();

    writeBuffer[0] = READ_STATUS_REG1_CMD;

    spiSelect(&SPID1);

    // send read status register 1
    spiSend(&SPID1, 1, writeBuffer);

    while(true)
    {
        // read register value
        spiReceive(&SPID1, 1, readBuffer);

        if( !(readBuffer[0] & AT25SF641_SR_BUSY) )
        {
            // BuSY bit is cleared
            break;          
        }

        if((HAL_GetTick() - tickstart) > HAL_SPI_TIMEOUT_DEFAULT_VALUE)
        {
            // operation timeout
            
            // unselect SPI
            spiUnselect(&SPID1);

            return false;
        }
    }

    spiUnselect(&SPID1);

    return true;
}

bool SPI_Erase_Block(u32_t addr)
{
    // send write enable
    writeBuffer[0] = WRITE_ENABLE_CMD;

    spiSelect(&SPID1);
    spiSend(&SPID1, 1, writeBuffer);
    spiUnselect(&SPID1);

    // send block erase
    writeBuffer[0] = SECTOR_ERASE_CMD;
    writeBuffer[1] = (uint8_t)(addr >> 16);
    writeBuffer[2] = (uint8_t)(addr >> 8);
    writeBuffer[3] = (uint8_t)addr;

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(writeBuffer, 4);

    spiSelect(&SPID1);
    spiSend(&SPID1, 4, writeBuffer);
    spiUnselect(&SPID1);

    // wait for erase operation to complete
    return SPI_WaitOnBusy();
}

bool SPI_Read(uint8_t* pData, uint32_t readAddr, uint32_t size)
{
    // send read page command
    writeBuffer[0] = READ_CMD;
    writeBuffer[1] = (uint8_t)(readAddr >> 16);
    writeBuffer[2] = (uint8_t)(readAddr >> 8);
    writeBuffer[3] = (uint8_t)readAddr;

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(writeBuffer, 4);

    spiSelect(&SPID1);
    spiSend(&SPID1, 4, writeBuffer);

    // clear read buffer
    memset(readBuffer, 0, SPIFFS_LOGICAL_PAGE_SIZE);

    spiReceive(&SPID1, size, readBuffer);
    spiUnselect(&SPID1);

    // invalidate cache
    // (only required for Cortex-M7)
    cacheBufferInvalidate(readBuffer, size);

    // copy to pointer
    memcpy(pData, readBuffer, size);

    return true;
}

bool SPI_Write(uint8_t* pData, uint32_t writeAddr, uint32_t size)
{
    // send write enable
    writeBuffer[0] = WRITE_ENABLE_CMD;

    spiSelect(&SPID1);
    spiSend(&SPID1, 1, writeBuffer);
    spiUnselect(&SPID1);

    // send write page
    writeBuffer[0] = PAGE_PROG_CMD;
    writeBuffer[1] = (uint8_t)(writeAddr >> 16);
    writeBuffer[2] = (uint8_t) (writeAddr >> 8);
    writeBuffer[3] = (uint8_t)writeAddr;

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(writeBuffer, 4);

    spiSelect(&SPID1);
    spiSend(&SPID1, 4, writeBuffer);

    // copy from buffer
    memcpy(writeBuffer, pData, size);

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(writeBuffer, size);

    spiSend(&SPID1, size, writeBuffer);
    spiUnselect(&SPID1);

    // wait for erase operation to complete
    SPI_WaitOnBusy();

    // send write disable
    writeBuffer[0] = WRITE_DISABLE_CMD;
    cacheBufferFlush(writeBuffer, 1);

    spiSelect(&SPID1);
    spiSend(&SPID1, 1, writeBuffer);
    spiUnselect(&SPID1);

    return true;
}

#endif // SPIFFS_SPI1


#ifdef SPIFFS_QSPI

#include <hal_stm32_qspi.h>

static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi);
// static uint8_t QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_EnterMemory_QPI(QSPI_HandleTypeDef *hqspi);
// static uint8_t QSPI_ExitMemory_QPI(QSPI_HandleTypeDef *hqspi);
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
    uint8_t device_id[6];
    memset(device_id, 0, sizeof(device_id));
   
    /* QSPI initialization */
    QSPID1.Init.ClockPrescaler     = 1;
    QSPID1.Init.FifoThreshold      = 4;
    QSPID1.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE; 
    QSPID1.Init.FlashSize          = POSITION_VAL(SPIFFS_TOTAL_SIZE) - 1;
    QSPID1.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE;
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

    // not used with this memory chip
    // /* Set the QSPI memory in 4-bytes address mode */
    // if(QSPI_EnterFourBytesAddress(&QSPID1) != QSPI_OK)
    // {
    //     return QSPI_NOT_SUPPORTED;
    // }

    // sanity check: read device ID and unique ID
    // this instruction requires a buffer with 6 positions
    if( QSPI_ReadChipID( &QSPID1, device_id ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }

    // constants from ID Definitions table in W25Q128 datasheet
    ASSERT(device_id[0] == W25Q128_MANUFACTURER_ID);
    ASSERT(device_id[1] == W25Q128_DEVICE_ID1);
    ASSERT(device_id[2] == W25Q128_DEVICE_ID2);

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

    /* Initialize the Mode Bit Reset command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
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

    /* Send the SW reset command */
    s_command.Instruction       = RESET_MEMORY_CMD;
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

static uint8_t QSPI_EnterMemory_QPI( QSPI_HandleTypeDef *hqspi )
{
    QSPI_CommandTypeDef s_command;
    uint8_t reg[] = { 0 };
    
    /* Initialize the read volatile configuration register command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_STATUS_REG2_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
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
    if (HAL_QSPI_Receive(hqspi, &reg[0], HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }

    /* Enable write operations */
    if (QSPI_WriteEnable(hqspi) != QSPI_OK)
    {
      return QSPI_ERROR;
    }

    /* Update status register 2 (with quad enable bit) */
    s_command.Instruction = WRITE_STATUS_REG2_CMD;
    MODIFY_REG(reg[0], 0, W25Q128_SR2_QE);

    /* write status register 2 */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }

    /* Transmission of the data */
    if (HAL_QSPI_Transmit(hqspi, &reg[0], HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // read status register for confirmation
    s_command.Instruction = READ_STATUS_REG2_CMD;
    
    /* Configure the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(hqspi, &reg[0], HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }

    if(reg[0] & W25Q128_SR2_QE)
    {
        return QSPI_OK;
    }
    else
    {
        return QSPI_ERROR;
    }
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

// static uint8_t QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi)
// {
//     (void)hqspi;
//     // QSPI_CommandTypeDef s_command;
//     // uint8_t reg1;

//     // /* Initialize the command */
//     // s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
//     // s_command.Instruction       = ENTER_4_BYTE_ADDR_MODE_CMD;
//     // s_command.AddressMode       = QSPI_ADDRESS_NONE;
//     // s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//     // s_command.DataMode          = QSPI_DATA_NONE;
//     // s_command.DummyCycles       = 0;
//     // s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//     // s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//     // s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//     // /* Enable write operations */
//     // if (QSPI_WriteEnable(hqspi) != QSPI_OK)
//     // {
//     //   return QSPI_ERROR;
//     // }

//     // /* Send the command */
//     // if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//     // {
//     //     return QSPI_ERROR;
//     // }

//     // /* Configure automatic polling mode to wait the memory is ready */
//     // if (QSPI_AutoPollingMemReady(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
//     // {
//     //     return QSPI_ERROR;
//     // }

//     return QSPI_OK;
// }

static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef     s_command;
    QSPI_AutoPollingTypeDef s_config;
    
    /* Enable write operations */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
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
    s_config.Match           = W25Q128_SR_WREN;
    s_config.Mask            = W25Q128_SR_WREN;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
    
    s_command.Instruction    = READ_STATUS_REG1_CMD;
    s_command.DataMode       = QSPI_DATA_1_LINE;
    
    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }
    
    return QSPI_OK;
}

static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
    QSPI_CommandTypeDef     s_command;
    QSPI_AutoPollingTypeDef sConfig;

    /* Configure automatic polling mode to wait for memory ready */  
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_STATUS_REG1_CMD; /* same value on both memory types */
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.Match           = 0;
    sConfig.Mask            = W25Q128_SR_WIP; /* same value on both memory types */
    sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval        = 0x10;
    sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &sConfig, Timeout) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

static uint8_t QSPI_ReadChipID(QSPI_HandleTypeDef *hqspi, uint8_t* buffer)
{
    QSPI_CommandTypeDef     s_command;

    /* Configure automatic polling mode to wait for memory ready */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_ID_CMD2;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.NbData            = 6;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

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
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_OUT_FAST_READ_CMD; /* same value on both memory types */
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = readAddr;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = W25Q128_DUMMY_CYCLES_READ_QUAD;
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
    MODIFY_REG(QSPID1.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_2_CYCLE);
    
    /* Reception of the data */
    if (HAL_QSPI_Receive(&QSPID1, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    /* Restore S# timing for nonRead commands */
    MODIFY_REG(QSPID1.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_5_CYCLE);
    
    return QSPI_OK;
}

uint8_t QSPI_Write(uint8_t* pData, uint32_t writeAddr, uint32_t size)
{
    QSPI_CommandTypeDef s_command;
    uint32_t end_addr, current_size, current_addr;
    
    /* Calculation of the size between the write address and the end of the page */
    current_size = W25Q128_PAGE_SIZE - (writeAddr % (W25Q128_PAGE_SIZE));
    
    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > size)
    {
        current_size = size;
    }

    /* Initialize the adress variables */
    current_addr = writeAddr;
    end_addr = writeAddr + size;

    /* Initialize the program command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_IN_FAST_PROG_CMD; /* same value on both memory types */
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
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
        current_size = ((current_addr + W25Q128_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128_PAGE_SIZE;
    }
    while (current_addr < end_addr);
    
    return QSPI_OK;
}

uint8_t QSPI_Erase_Block(uint32_t blockAddress)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = SECTOR_ERASE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
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
    if (QSPI_AutoPollingMemReady(&QSPID1, W25Q128_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

#endif // SPIFFS_QSPI
