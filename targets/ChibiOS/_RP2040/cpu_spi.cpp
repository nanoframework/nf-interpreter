//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include <LaunchCLR.h>
#include <string.h>
#include <targetPAL.h>

#include <sys_dev_spi_native_target.h>

/////////////////////////////////////////////////////
// SPI PAL structs for RP2040 (2 buses: SPI0, SPI1)
/////////////////////////////////////////////////////
#if RP_SPI_USE_SPI0
NF_PAL_SPI SPI0_PAL;
#endif
#if RP_SPI_USE_SPI1
NF_PAL_SPI SPI1_PAL;
#endif

// RP2040 peripheral clock is always 125 MHz (from peri_clk)
#define RP_SPI_PERI_CLK 125000000

// Tidy up after completing transfer
static void CompleteTransfer(NF_PAL_SPI *palSpi)
{
    spiUnselect(palSpi->Driver);
    spiReleaseBus(palSpi->Driver);
}

// Callback used when an async operation completes
static void SpiCallback(SPIDriver *spip)
{
    (void)spip;

    NATIVE_INTERRUPT_START

    NF_PAL_SPI *palSpi = NULL;

#if RP_SPI_USE_SPI0
    if (spip == &SPID0)
    {
        palSpi = &SPI0_PAL;
    }
#endif
#if RP_SPI_USE_SPI1
    if (spip == &SPID1)
    {
        palSpi = &SPI1_PAL;
    }
#endif

    if (palSpi == NULL)
    {
        NATIVE_INTERRUPT_END
        return;
    }

    if (palSpi->SequentialTxRx)
    {
        palSpi->SequentialTxRx = false;
        spiStartReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
    }
    else
    {
        CompleteTransfer(palSpi);

        if (palSpi->ChipSelect >= 0)
        {
            CPU_GPIO_TogglePinState(palSpi->ChipSelect);
        }

        if (palSpi->Callback)
        {
            palSpi->Callback(palSpi->BusIndex);
        }
    }

    NATIVE_INTERRUPT_END
};

// Compute RP2040 SPI baud rate registers (SSPCPSR and SSPCR0.SCR)
// RP2040 SPI (PL022): freq = peri_clk / (CPSDVSR * (1 + SCR))
// CPSDVSR must be even, 2-254; SCR is 0-255
static void ComputeBaudRate(int32_t requestedFrequency, int32_t &actualFrequency, uint32_t &sspcpsr, uint32_t &sspcr0)
{
    if (requestedFrequency <= 0)
    {
        // Default to max frequency (62.5 MHz)
        requestedFrequency = RP_SPI_PERI_CLK / 2;
    }

    // Find best CPSDVSR and SCR combination
    uint32_t bestCpsdvsr = 2;
    uint32_t bestScr = 0;
    int32_t bestFreq = RP_SPI_PERI_CLK / 2;

    for (uint32_t cpsdvsr = 2; cpsdvsr <= 254; cpsdvsr += 2)
    {
        uint32_t scr = (RP_SPI_PERI_CLK / (cpsdvsr * requestedFrequency)) - 1;
        if (scr > 255)
            scr = 255;

        int32_t freq = RP_SPI_PERI_CLK / (cpsdvsr * (1 + scr));

        if (freq <= requestedFrequency && freq > bestFreq)
        {
            bestFreq = freq;
            bestCpsdvsr = cpsdvsr;
            bestScr = scr;
        }
        else if (bestFreq > requestedFrequency && freq <= requestedFrequency)
        {
            bestFreq = freq;
            bestCpsdvsr = cpsdvsr;
            bestScr = scr;
        }

        if (freq <= requestedFrequency)
            break;
    }

    actualFrequency = bestFreq;
    sspcpsr = bestCpsdvsr;
    sspcr0 = (bestScr << 8); // SCR is bits [15:8] of SSPCR0
}

// Return the NF_PAL structure for busIndex (0-based)
NF_PAL_SPI *GetNfPalfromBusIndex(uint8_t busIndex)
{
    switch (busIndex)
    {
#if RP_SPI_USE_SPI0
        case 0:
            return &SPI0_PAL;
#endif
#if RP_SPI_USE_SPI1
        case 1:
            return &SPI1_PAL;
#endif
        default:
            return NULL;
    }
}

// Build low-level SPI configuration for RP2040 PL022
void GetSPIConfig(SPI_DEVICE_CONFIGURATION &config, SPI_WRITE_READ_SETTINGS &wrc, SPIConfig *llConfig)
{
    int32_t actualFrequency;
    uint32_t sspcpsr, sspcr0;

    ComputeBaudRate(config.Clock_RateHz, actualFrequency, sspcpsr, sspcr0);

    // SSPCR0: SCR field (from baud rate), data size bits, SPI mode
    // Data Size Select (DSS): bits [3:0], value = datasize - 1
    if (wrc.Bits16ReadWrite)
    {
        sspcr0 |= (16 - 1); // 16-bit frames
    }
    else
    {
        sspcr0 |= (8 - 1); // 8-bit frames
    }

    // SPI mode: SPO (bit 6) = CPOL, SPH (bit 7) = CPHA
    switch (config.Spi_Mode)
    {
        case SpiMode_Mode1:
            sspcr0 |= (1 << 7); // CPHA=1
            break;
        case SpiMode_Mode2:
            sspcr0 |= (1 << 6); // CPOL=1
            break;
        case SpiMode_Mode3:
            sspcr0 |= (1 << 6) | (1 << 7); // CPOL=1, CPHA=1
            break;
        default: // Mode0
            break;
    }

    llConfig->SSPCR0 = sspcr0;
    llConfig->SSPCPSR = sspcpsr;
    llConfig->end_cb = SpiCallback;
}

// Performs a read/write operation on 8-bit or 16-bit word data.
HRESULT CPU_SPI_nWrite_nRead(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &wrc,
    uint8_t *writeBuffer,
    int32_t writeSize,
    uint8_t *readBuffer,
    int32_t readSize)
{
    NANOCLR_HEADER();
    {
        NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;
        bool sync = (wrc.callback == 0);

        palSpi->BufferIs16bits = wrc.Bits16ReadWrite;
        palSpi->Callback = wrc.callback;

        if (writeBuffer != NULL)
        {
            palSpi->WriteSize = writeSize;
            palSpi->WriteBuffer = writeBuffer;
        }

        if (readBuffer != NULL)
        {
            palSpi->ReadSize = readSize;
            palSpi->ReadBuffer = readBuffer;
        }

        palSpi->BusIndex = sdev.Spi_Bus;

        GetSPIConfig(sdev, wrc, &palSpi->Configuration);

        if (sync)
        {
            palSpi->Configuration.end_cb = NULL;
        }

        spiAcquireBus(palSpi->Driver);
        spiStart(palSpi->Driver, &palSpi->Configuration);
        spiSelect(palSpi->Driver);

        palSpi->ChipSelect = wrc.DeviceChipSelect;
        if (wrc.DeviceChipSelect >= 0)
        {
            CPU_GPIO_SetPinState(wrc.DeviceChipSelect, (GpioPinValue)wrc.ChipSelectActiveState);
        }

        if (sync)
        {
            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                if (wrc.fullDuplex)
                {
                    spiExchange(
                        palSpi->Driver,
                        palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                        palSpi->WriteBuffer,
                        palSpi->ReadBuffer);
                }
                else
                {
                    spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                    spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
            }
            else
            {
                if (palSpi->ReadSize != 0)
                {
                    spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }

            CompleteTransfer(palSpi);

            if (wrc.DeviceChipSelect >= 0)
            {
                CPU_GPIO_SetPinState(wrc.DeviceChipSelect, (GpioPinValue)!wrc.ChipSelectActiveState);
            }
        }
        else
        {
            if (wrc.DeviceChipSelect >= 0)
            {
                CPU_GPIO_SetPinState(wrc.DeviceChipSelect, (GpioPinValue)wrc.ChipSelectActiveState);
            }

            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                if (wrc.fullDuplex)
                {
                    palSpi->SequentialTxRx = false;
                    spiStartExchange(
                        palSpi->Driver,
                        palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                        palSpi->WriteBuffer,
                        palSpi->ReadBuffer);
                }
                else
                {
                    palSpi->SequentialTxRx = true;
                    spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }
            else
            {
                palSpi->SequentialTxRx = false;

                if (palSpi->ReadSize != 0)
                {
                    spiStartReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }

            NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
        }
    }

    NANOCLR_NOCLEANUP();
}

SPI_OP_STATUS CPU_SPI_OP_Status(uint8_t busIndex, uint32_t deviceHandle)
{
    (void)busIndex;

    NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;

    switch (palSpi->Driver->state)
    {
        default:
        case SPI_UNINIT:
        case SPI_STOP:
        case SPI_READY:
            return SPI_OP_READY;
        case SPI_ACTIVE:
            return SPI_OP_RUNNING;
        case SPI_COMPLETE:
            return SPI_OP_COMPLETE;
    }
}

bool CPU_SPI_Initialize(uint8_t busIndex, const SPI_DEVICE_CONFIGURATION &spiDeviceConfig)
{
    switch (busIndex)
    {
#if RP_SPI_USE_SPI0
        case 0:
            if (SPI0_PAL.Driver == NULL)
            {
                ConfigPins_SPI0(spiDeviceConfig);
                SPI0_PAL.Driver = &SPID0;
                SPI0_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
#if RP_SPI_USE_SPI1
        case 1:
            if (SPI1_PAL.Driver == NULL)
            {
                ConfigPins_SPI1(spiDeviceConfig);
                SPI1_PAL.Driver = &SPID1;
                SPI1_PAL.ChipSelect = spiDeviceConfig.DeviceChipSelect;
            }
            break;
#endif
        default:
            return false;
    }

    return true;
}

bool CPU_SPI_Uninitialize(uint8_t busIndex)
{
    switch (busIndex)
    {
#if RP_SPI_USE_SPI0
        case 0:
            spiStop(&SPID0);
            SPI0_PAL.Driver = NULL;
            spiReleaseBus(&SPID0);
            break;
#endif
#if RP_SPI_USE_SPI1
        case 1:
            spiStop(&SPID1);
            SPI1_PAL.Driver = NULL;
            spiReleaseBus(&SPID1);
            break;
#endif
        default:
            return false;
    }

    return true;
}

uint32_t CPU_SPI_PortsMap()
{
    uint32_t map = 0;

#if RP_SPI_USE_SPI0
    map |= 0x01;
#endif
#if RP_SPI_USE_SPI1
    map |= 0x02;
#endif

    return map;
}

HRESULT CPU_SPI_Add_Device(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, uint32_t &handle)
{
    if (spiDeviceConfig.BusConfiguration == SpiBusConfiguration_Simplex)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    handle = (uint32_t)GetNfPalfromBusIndex(spiDeviceConfig.Spi_Bus);

    return S_OK;
}

void CPU_SPI_GetPins(uint32_t busIndex, GPIO_PIN &clk, GPIO_PIN &miso, GPIO_PIN &mosi)
{
    (void)busIndex;

    clk = (GPIO_PIN)-1;
    miso = (GPIO_PIN)-1;
    mosi = (GPIO_PIN)-1;
}

HRESULT CPU_SPI_MinClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // RP2040: min freq = 125MHz / (254 * 256) ≈ 1923 Hz
    *frequency = RP_SPI_PERI_CLK / (254 * 256);

    return S_OK;
}

HRESULT CPU_SPI_MaxClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // RP2040: max freq = 125MHz / 2 = 62.5 MHz
    *frequency = RP_SPI_PERI_CLK / 2;

    return S_OK;
}

uint32_t CPU_SPI_ChipSelectLineCount(uint32_t busIndex)
{
    (void)busIndex;

    return MAX_SPI_DEVICES;
}
