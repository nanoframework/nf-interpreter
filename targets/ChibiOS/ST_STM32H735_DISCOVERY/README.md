## Configuration of ChibiOS, HAL and MCU

The board definition files (`board.c` and `board.h`) are copied, unchanged, from ChibiOS `os/hal/boards/ST_STM32H735IG_DISCOVERY`.

For memory maps, uuid etc. the reference document used was the STM32H723/733, STM32H725/735 and STM32H730 reference manual (RM0468).

In _halconf.h_ (in both nanoBooter and nanoCLR folders), when compared with a default ChibiOS file:
- HAL_USE_SERIAL to TRUE
- SERIAL_DEFAULT_BITRATE to 921600

In _mcuconf.h_ (in both nanoBooter and nanoCLR folders), when compared with the default file available from ChibiOS (`testhal/STM32/multi/WSPI-MFS/cfg/stm32h735ig_discovery`):
- STM32_SERIAL_USE_USART3 to TRUE (ST-LINK virtual COM port, used for the wire protocol)
- STM32_NOCACHE_RBAR and STM32_NOCACHE_RASR changed to mark SRAM1 + SRAM2 (0x30000000, 32k) as non cacheable
- STM32_I2C4SEL set to HSI kernel clock (I2C timings are computed for 64MHz). I2C1/2/3/5 are not used because ChibiOS 21.11 only supports PCLK1 as their kernel clock
- STM32_I2C_USE_DMA set to FALSE (I2C4 can only use BDMA, which can't reach AXI SRAM)

NOTE: this configuration uses the Serial port through the onboard ST Link USB connection.

## Flash layout

The STM32H735 has 1MB of flash in 8 sectors of 128kB each.

| Sector | Address    | Usage               |
|--------|------------|---------------------|
| 0      | 0x08000000 | nanoBooter          |
| 1      | 0x08020000 | configuration block |
| 2-6    | 0x08040000 | nanoCLR             |
| 7      | 0x080E0000 | deployment          |

Flash is programmed in flash words of 256 bits with ECC, which means that a flash word can be programmed only once after an erase.

## RAM layout

- AXI SRAM (320kB): data, BSS, C heap and CLR managed heap.
- SRAM1 + SRAM2 (32kB): non cacheable area and Ethernet DMA buffers.
- DTCM (128kB): stacks (DTCM is not accessible by the DMAs).

## ADC configurations

The following ADC channels (and respective GPIO pins) are available to the managed API, in the respective index:
- PC0  ADC1 INP10 (Arduino A0)
- PA0  ADC1 INP16 (Arduino D3)
- PF11 ADC1 INP2 (STMOD+)

## Peripherals

- COM1: USART1, TX PB14 (Arduino D1), RX PB15 (Arduino D0)
- I2C4: SCL PF14, SDA PF15 (STMOD+, shared with on-board touch panel and audio codec)
- SPI5: SCK PF7, MISO PF8, MOSI PF9 (STMOD+)
- DAC1 channel 2: PA5 (STMOD+)
- PWM: TIM1, TIM3, TIM4, TIM5, TIM8
- 1-Wire: UART4, TX PA0 (Arduino D3)
- SD card: SDMMC1
- Ethernet: RMII with LAN8742A PHY

## Not supported (yet)

- Graphics (LTDC parallel RGB display, HyperRAM on OCTOSPI2)
- CAN (FDCAN)
- External OCTOSPI NOR flash

## Floating point

The build is set to add support for double-precision floating point (the Cortex-M7 FPU in STM32H7 has double-precision support).
Meaning that `System.Math` API supports both the `float` and the `double` overloads.
