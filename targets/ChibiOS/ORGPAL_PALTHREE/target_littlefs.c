//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <target_littlefs.h>
#include <hal_littlefs.h>

#if defined(LFS_SPI1) && !defined(LFS_QSPI)
#define LFS_CACHE_SIZE AT25SF641_PAGE_SIZE
#elif defined(LFS_QSPI) && !defined(LFS_SPI1)
#define LFS_CACHE_SIZE W25Q128_PAGE_SIZE
#elif defined(LFS_SPI1) && defined(LFS_QSPI)
#if AT25SF641_PAGE_SIZE > W25Q128_PAGE_SIZE
#define LFS_CACHE_SIZE AT25SF641_PAGE_SIZE
#else
#define LFS_CACHE_SIZE W25Q128_PAGE_SIZE
#endif
#else
#error "Exactly one of LFS_SPI1 or LFS_QSPI must be defined"
#endif

#if CACHE_LINE_SIZE > 0
CC_ALIGN_DATA(CACHE_LINE_SIZE)
uint8_t lfs_inputBuffer[CACHE_SIZE_ALIGN(uint8_t, LFS_CACHE_SIZE)] __attribute__((section(".nocache")));
CC_ALIGN_DATA(CACHE_LINE_SIZE)
uint8_t lfs_outputBuffer[CACHE_SIZE_ALIGN(uint8_t, LFS_CACHE_SIZE)] __attribute__((section(".nocache")));
#else
uint8_t lfs_inputBuffer[];
uint8_t lfs_outputBuffer[];
#endif

int32_t lfs_inputBufferSize = LFS_CACHE_SIZE;
int32_t lfs_outputBufferSize = LFS_CACHE_SIZE;

#ifdef LFS_SPI1

static const SPIConfig spiConfig = {
    .circular = false,
    .slave = false,
    .data_cb = NULL,
    .error_cb = NULL,
    // CPHA=0, CPOL=0, MSb first
    .cr1 = 0U, // SPI_CR1_CPOL | SPI_CR1_BR_0,
    // transfer length to 8bit
    .cr2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0};

#if CACHE_LINE_SIZE > 0
CC_ALIGN_DATA(CACHE_LINE_SIZE)
uint8_t dataBuffer_0[CACHE_SIZE_ALIGN(uint8_t, AT25SF641_PAGE_SIZE)] __attribute__((section(".nocache")));
#else
uint8_t dataBuffer_0[AT25SF641_PAGE_SIZE];
#endif

#ifdef DEBUG
#if CACHE_LINE_SIZE > 0
CC_ALIGN_DATA(CACHE_LINE_SIZE)
uint8_t tempBuffer[CACHE_SIZE_ALIGN(uint8_t, AT25SF641_PAGE_SIZE)] __attribute__((section(".nocache")));
#else
uint8_t tempBuffer[AT25SF641_PAGE_SIZE];
#endif
#endif

///////////////
// Definitions
#define CS_SELECT   palClearPad(PAL_PORT(LINE_FLASH_SPI1_CS), PAL_PAD(LINE_FLASH_SPI1_CS))
#define CS_UNSELECT palSetPad(PAL_PORT(LINE_FLASH_SPI1_CS), PAL_PAD(LINE_FLASH_SPI1_CS))

///////////////
// declarations
static bool SPI_Erase_Block(uint32_t addr, bool largeBlock);
static bool SPI_Read(uint8_t *pData, uint32_t readAddr, uint32_t size);
static bool SPI_Write(const uint8_t *pData, uint32_t writeAddr, uint32_t size);
static bool SPI_WaitOnBusy();

extern uint32_t HAL_GetTick(void);

// target specific implementation of hal_lfs_erase
int32_t hal_lfs_erase_0(const struct lfs_config *c, lfs_block_t block)
{
    Watchdog_Reset();

    uint32_t addr = block * c->block_size;

    if (!SPI_Erase_Block(addr, false))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_read
int32_t hal_lfs_read_0(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t addr = block * c->block_size + off;

    if (!SPI_Read(buffer, addr, size))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_prog
int32_t hal_lfs_prog_0(
    const struct lfs_config *c,
    lfs_block_t block,
    lfs_off_t off,
    const void *buffer,
    lfs_size_t size)
{
    uint32_t addr = block * c->block_size + off;

    if (!SPI_Write(buffer, addr, size))
    {
        return LFS_ERR_IO;
    }

#ifdef DEBUG

    memset(tempBuffer, 0xBB, size);

    // read back and compare
    SPI_Read(tempBuffer, addr, size);
    ASSERT(memcmp(buffer, tempBuffer, size) == 0);

#endif

    return LFS_ERR_OK;
}

// target specific implementation of chip erase
bool hal_lfs_erase_chip_0()
{
    // need to do this one one block at a time to avoid watchdog reset
    for (uint32_t i = 0; i < AT25SF641_FLASH_SIZE / AT25SF641_SECTOR_SIZE; i++)
    {
        if (!SPI_Erase_Block(i * AT25SF641_SECTOR_SIZE, true))
        {
            return false;
        }

        // reset watchdog
        Watchdog_Reset();
    }

    return true;
}

static bool SPI_WaitOnBusy()
{
    uint32_t tickstart = HAL_GetTick();

    // clear read buffer
    memset(dataBuffer_0, 0xFF, 1);

    dataBuffer_0[0] = READ_STATUS_REG1_CMD;
    cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

    CS_SELECT;

    // send read status register 1
    spiSend(&SPID1, 1, dataBuffer_0);

    while (true)
    {
        // read register value
        spiReceive(&SPID1, 1, dataBuffer_0);
        cacheBufferInvalidate(dataBuffer_0, sizeof(dataBuffer_0));

        if (!(dataBuffer_0[0] & AT25SF641_SR_BUSY))
        {
            // BuSY bit is cleared
            break;
        }

        if ((HAL_GetTick() - tickstart) > HAL_SPI_TIMEOUT_DEFAULT_VALUE)
        {
            // operation timeout

            // unselect SPI
            CS_UNSELECT;

            return false;
        }
    }

    CS_UNSELECT;

    return true;
}

static bool SPI_Erase_Block(uint32_t addr, bool largeBlock)
{
    // send write enable
    dataBuffer_0[0] = WRITE_ENABLE_CMD;
    cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

    CS_SELECT;
    spiSend(&SPID1, 1, dataBuffer_0);
    CS_UNSELECT;

    // send block erase
    dataBuffer_0[0] = largeBlock ? BLOCK_ERASE_CMD : SECTOR_ERASE_CMD;
    dataBuffer_0[1] = (uint8_t)(addr >> 16);
    dataBuffer_0[2] = (uint8_t)(addr >> 8);
    dataBuffer_0[3] = (uint8_t)addr;

    // // flush DMA buffer to ensure cache coherency
    // // (only required for Cortex-M7)
    // cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

    CS_SELECT;
    spiSend(&SPID1, 4, dataBuffer_0);
    CS_UNSELECT;

    // wait for erase operation to complete
    return SPI_WaitOnBusy();
}

static bool SPI_Read(uint8_t *pData, uint32_t readAddr, uint32_t size)
{
    // send read page command
    dataBuffer_0[0] = READ_CMD;
    dataBuffer_0[1] = (uint8_t)(readAddr >> 16);
    dataBuffer_0[2] = (uint8_t)(readAddr >> 8);
    dataBuffer_0[3] = (uint8_t)readAddr;

    // // flush DMA buffer to ensure cache coherency
    // // (only required for Cortex-M7)
    // cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

    CS_SELECT;
    spiSend(&SPID1, 4, dataBuffer_0);

    // clear read buffer
    memset(dataBuffer_0, 0xDD, size);

    spiReceive(&SPID1, size, dataBuffer_0);
    CS_UNSELECT;

    // invalidate cache
    // (only required for Cortex-M7)
    cacheBufferInvalidate(dataBuffer_0, sizeof(dataBuffer_0));

    // copy to pointer
    memcpy(pData, dataBuffer_0, size);

    return true;
}

static bool SPI_Write(const uint8_t *pData, uint32_t writeAddr, uint32_t size)
{
    uint32_t writeSize;
    uint32_t address = writeAddr;

    // perform paged program
    while (size > 0)
    {
        // send write enable
        dataBuffer_0[0] = WRITE_ENABLE_CMD;
        cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

        CS_SELECT;
        spiSend(&SPID1, 1, dataBuffer_0);
        CS_UNSELECT;

        // calculate write size
        writeSize = __builtin_fmin(AT25SF641_PAGE_SIZE - (address % AT25SF641_PAGE_SIZE), size);

        // send write page
        dataBuffer_0[0] = PAGE_PROG_CMD;
        dataBuffer_0[1] = (uint8_t)(address >> 16);
        dataBuffer_0[2] = (uint8_t)(address >> 8);
        // adjust address if writeSize is the full page
        dataBuffer_0[3] = (uint8_t)(writeSize == W25Q128_PAGE_SIZE ? address & 0xFFFFFFF0 : address);

        // // flush DMA buffer to ensure cache coherency
        // // (only required for Cortex-M7)
        // cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

        CS_SELECT;
        spiSend(&SPID1, 4, dataBuffer_0);

        // copy from buffer
        memcpy(dataBuffer_0, pData, writeSize);

        // // flush DMA buffer to ensure cache coherency
        // // (only required for Cortex-M7)
        // cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

        spiSend(&SPID1, writeSize, dataBuffer_0);
        CS_UNSELECT;

        // wait for operation to complete
        SPI_WaitOnBusy();

        address += writeSize;
        pData += writeSize;
        size -= writeSize;
    }

    return true;
}

#endif // LFS_SPI1

#ifdef LFS_QSPI

#include <hal_stm32_qspi.h>

#define HAL_QPSI_TIMEOUT_CONFIG_COMMAND ((uint32_t)10)

#ifdef DEBUG
uint8_t tempBuffer[W25Q128_PAGE_SIZE];
#endif

///////////////
// declarations

static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_EnterMemory_QPI(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);
static uint8_t QSPI_ReadChipID(QSPI_HandleTypeDef *hqspi, uint8_t *buffer);

static uint8_t QSPI_Read(uint8_t *pData, uint32_t readAddr, uint32_t size);
static uint8_t QSPI_Write(uint8_t *pData, uint32_t writeAddr, uint32_t size);
static uint8_t QSPI_Erase_Block(uint32_t blockAddress);
static bool QSPI_Erase_Chip();

// target specific implementation of hal_lfs_erase
int32_t hal_lfs_erase_1(const struct lfs_config *c, lfs_block_t block)
{
    uint32_t ret;
    uint32_t addr = block * c->block_size;

    ret = QSPI_Erase_Block(addr);
    if (ret != QSPI_OK)
    {
        __NOP();
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_read
int32_t hal_lfs_read_1(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t ret;
    uint32_t addr = block * c->block_size + off;

    ret = QSPI_Read((uint8_t *)buffer, addr, size);
    if (ret != QSPI_OK)
    {
        __NOP();
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_prog
int32_t hal_lfs_prog_1(
    const struct lfs_config *c,
    lfs_block_t block,
    lfs_off_t off,
    const void *buffer,
    lfs_size_t size)
{
    uint32_t ret;
    uint32_t addr = block * c->block_size + off;

    ret = QSPI_Write((uint8_t *)buffer, addr, size);

    if (ret != QSPI_OK)
    {
        __NOP();
        return LFS_ERR_IO;
    }

#ifdef DEBUG

    memset(tempBuffer, 0xBB, size);

    // read back and compare
    QSPI_Read(tempBuffer, (block * c->block_size + off), size);
    for (lfs_size_t i = 0; i < size; i++)
    {
        ASSERT(((const uint8_t *)buffer)[i] == tempBuffer[i]);
    }

#endif

    return LFS_ERR_OK;
}

// target specific implementation of chip erase
bool hal_lfs_erase_chip_1()
{
    return QSPI_Erase_Chip() == TRUE;
}

static bool QSPI_Erase_Chip()
{
    // need to do this one one block at a time to avoid watchdog reset
    for (uint32_t i = 0; i < W25Q128_FLASH_SIZE / W25Q128_SECTOR_SIZE; i++)
    {
        if (QSPI_Erase_Block(i * W25Q128_SECTOR_SIZE) != QSPI_OK)
        {
            return FALSE;
        }

        // reset watchdog
        Watchdog_Reset();
    }

    return TRUE;
}

static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;

    // Initialize the Mode Bit Reset command
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = RESET_ENABLE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    // Send the command
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // Send the SW reset command
    s_command.Instruction = RESET_MEMORY_CMD;
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // Configure automatic polling mode to wait the memory is ready
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

    // Initialize the read volatile configuration register command
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

    // Configure the command
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // Reception of the data
    if (HAL_QSPI_Receive(hqspi, &reg[0], HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // Enable write operations
    if (QSPI_WriteEnable(hqspi) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    // Update status register 2 (with quad enable bit)
    s_command.Instruction = WRITE_STATUS_REG2_CMD;
    MODIFY_REG(reg[0], 0, W25Q128_SR2_QE);

    // write status register 2
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // Transmission of the data
    if (HAL_QSPI_Transmit(hqspi, &reg[0], HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // read status register for confirmation
    s_command.Instruction = READ_STATUS_REG2_CMD;

    // Configure the command
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // Reception of the data
    if (HAL_QSPI_Receive(hqspi, &reg[0], HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    if (reg[0] & W25Q128_SR2_QE)
    {
        return QSPI_OK;
    }
    else
    {
        return QSPI_ERROR;
    }
}

static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    // Enable write operations
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

    // Configure automatic polling mode to wait for write enabling
    s_config.Match = W25Q128_SR_WREN;
    s_config.Mask = W25Q128_SR_WREN;
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

    // Configure automatic polling mode to wait for memory ready
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
    sConfig.Mask = W25Q128_SR_WIP;
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

    // Configure automatic polling mode to wait for memory ready
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

    // Reception of the data
    if (HAL_QSPI_Receive(hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

uint8_t QSPI_Read(uint8_t *pData, uint32_t readAddr, uint32_t size)
{
    QSPI_CommandTypeDef s_command;
    HAL_StatusTypeDef status;
    uint8_t retryCounter = 3;

    // Initialize the read command
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = QUAD_OUT_FAST_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.Address = readAddr;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_4_LINES;
    s_command.DummyCycles = W25Q128_DUMMY_CYCLES_READ_QUAD;
    s_command.NbData = size;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

config_command:

    // Configure the command
    status = HAL_QSPI_Command(&QSPID1, &s_command, HAL_QPSI_TIMEOUT_CONFIG_COMMAND);

    if (status != HAL_OK)
    {
        // try to clear busy bit, if retry counter not exceeded
        if (retryCounter-- == 0)
        {
            __NOP();
            return QSPI_ERROR;
        }
        else
        {
            // clear QSPI Busy bit
            // https://community.st.com/t5/stm32-mcus-products/qspi-flag-qspi-flag-busy-sometimes-stays-set/td-p/365865
            QSPID1.State = HAL_QSPI_STATE_BUSY;
            status = HAL_QSPI_Abort(&QSPID1);

            goto config_command;
        }
    }

    // Set S# timing for Read command
    MODIFY_REG(QSPID1.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_2_CYCLE);

    // Reception of the data
    status = HAL_QSPI_Receive(&QSPID1, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (status != HAL_OK)
    {
        __NOP();
    }

    // Restore S# timing for nonRead commands
    MODIFY_REG(QSPID1.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_5_CYCLE);

    return status == HAL_OK ? QSPI_OK : QSPI_ERROR;
}

uint8_t QSPI_Write(uint8_t *pData, uint32_t writeAddr, uint32_t size)
{
    QSPI_CommandTypeDef s_command;
    uint32_t end_addr, current_size, current_addr;

    // Calculation of the size between the write address and the end of the page
    current_size = W25Q128_PAGE_SIZE - (writeAddr % (W25Q128_PAGE_SIZE));

    // Check if the size of the data is less than the remaining place in the page
    if (current_size > size)
    {
        current_size = size;
    }

    // Initialize the adress variables
    current_addr = writeAddr;
    end_addr = writeAddr + size;

    // Initialize the program command
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

    // Perform the write page by page
    do
    {
        s_command.Address = current_addr;
        s_command.NbData = current_size;

        // Enable write operations
        if (QSPI_WriteEnable(&QSPID1) != QSPI_OK)
        {
            return QSPI_ERROR;
        }

        // Configure the command
        if (HAL_QSPI_Command(&QSPID1, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return QSPI_ERROR;
        }

        // Transmission of the data
        if (HAL_QSPI_Transmit(&QSPID1, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return QSPI_ERROR;
        }

        // Configure automatic polling mode to wait for end of program
        if (QSPI_AutoPollingMemReady(&QSPID1, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
        {
            return QSPI_ERROR;
        }

        // Update the address and size variables for next page programming
        current_addr += current_size;
        pData += current_size;
        current_size = ((current_addr + W25Q128_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128_PAGE_SIZE;
    } while (current_addr < end_addr);

    return QSPI_OK;
}

uint8_t QSPI_Erase_Block(uint32_t blockAddress)
{
    QSPI_CommandTypeDef s_command;

    // Initialize the erase command
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = SECTOR_ERASE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.Address = blockAddress;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    // Enable write operations
    if (QSPI_WriteEnable(&QSPID1) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    // Send the command
    if (HAL_QSPI_Command(&QSPID1, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // Configure automatic polling mode to wait for end of erase
    if (QSPI_AutoPollingMemReady(&QSPID1, W25Q128_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

#endif // LFS_QSPI

int8_t target_lfs_init()
{
#ifdef LFS_SPI1

    // init driver
    spiAcquireBus(&SPID1);
    spiStart(&SPID1, &spiConfig);

    ////////////////////////////////////////////////////////////////////////
    // no need to worry with cache issues at this early stage of the boot //
    ////////////////////////////////////////////////////////////////////////

    // resume from deep power down
    // have to send this to make sure device is functional after sleep
    dataBuffer_0[0] = RESUME_DEEP_PD_CMD;

    CS_SELECT;
    spiSend(&SPID1, 1, dataBuffer_0);
    CS_UNSELECT;

    // sanity check: read device ID and unique ID
    dataBuffer_0[0] = READ_ID_CMD2;

    // from AT25SF641 datasheet: need time for chip to become fully responsive
    chThdSleepMilliseconds(10);

    CS_SELECT;
    spiSend(&SPID1, 1, dataBuffer_0);
    spiReceive(&SPID1, 3, dataBuffer_0);
    CS_UNSELECT;

    // constants from ID Definitions table in AT25SF641 datasheet
    ASSERT(dataBuffer_0[0] == AT25SF641_MANUFACTURER_ID);
    ASSERT(dataBuffer_0[1] == AT25SF641_DEVICE_ID1);
    ASSERT(dataBuffer_0[2] == AT25SF641_DEVICE_ID2);

#endif // LFS_SPI1

#ifdef LFS_QSPI

    uint8_t dataBuffer[3];

    // need to adjust MPU in order to use QSPI memory, despite using indirect access
    // this is a know issue with STM32F7
    // https://community.st.com/t5/stm32-mcus-products/qspi-flag-qspi-flag-busy-sometimes-stays-set/td-p/365865

    // need to disable MPU in order to change regions
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

    // Enable the MPU
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

    // QSPI initialization
    QSPID1.Init.ClockPrescaler = 1;
    QSPID1.Init.FifoThreshold = 4;
    QSPID1.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    // OK to use the W25Q128_FLASH_SIZE for this instance
    QSPID1.Init.FlashSize = POSITION_VAL(W25Q128_FLASH_SIZE) - 1;
    QSPID1.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE;
    QSPID1.Init.ClockMode = QSPI_CLOCK_MODE_0;
    QSPID1.Init.FlashID = QSPI_FLASH_ID_1;
    QSPID1.Init.DualFlash = QSPI_DUALFLASH_DISABLE;

    // init driver
    qspiStart(&QSPID1);

    if (HAL_QSPI_Init(&QSPID1) != HAL_OK)
    {
        return QSPI_ERROR;
    }

    // QSPI memory reset
    if (QSPI_ResetMemory(&QSPID1) != QSPI_OK)
    {
        return QSPI_NOT_SUPPORTED;
    }

    // Put QSPI memory in QPI mode
    if (QSPI_EnterMemory_QPI(&QSPID1) != QSPI_OK)
    {
        return QSPI_NOT_SUPPORTED;
    }

    // from W25Q128 datasheet: Time Delay Before Write Instruction is >5ms
    chThdSleepMilliseconds(10);

    // sanity check: read device ID and unique ID
    // this instruction requires a buffer with 6 positions
    if (QSPI_ReadChipID(&QSPID1, dataBuffer) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    // constants from ID Definitions table in W25Q128 datasheet
    ASSERT(dataBuffer[0] == W25Q128_MANUFACTURER_ID);
    ASSERT(dataBuffer[1] == W25Q128_DEVICE_ID1);
    ASSERT(dataBuffer[2] == W25Q128_DEVICE_ID2);

#endif // LFS_QSPI

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_sync
int32_t hal_lfs_sync_0(const struct lfs_config *c)
{
    (void)c;

    __DSB();

    return 0;
}

// target specific implementation of hal_lfs_sync
int32_t hal_lfs_sync_1(const struct lfs_config *c)
{
    (void)c;

    __DSB();

    return 0;
}
