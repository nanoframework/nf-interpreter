# MXChip IoT DevKit AZ3166

The MXChip IoT DevKit AZ3166 (AZ3166) is a compact development kit built around the EMW3166 Wi‑Fi module. It was designed to simplify cloud‑connected IoT development and includes a range of peripherals (OLED, audio, sensors and debug interface). The kit has been discontinued (EOL) in retail channels, but documentation and datasheets remain available.

## References

- [EMW3166 datasheet (MXCHIP)](https://www.mouser.com/datasheet/2/744/DS0047EN_EMW3166-1223059.pdf)
- [Azure IoT DevKit docs](https://microsoft.github.io/azure-iot-developer-kit/)
- [Seeed Studio AZ3166 product page](https://www.seeedstudio.com/AZ3166-IOT-Developer-Kit-p-2922.html)

## Board specs

| Item | Details |
|---|---|
| Module | EMW3166 Wi‑Fi module (see datasheet) |
| MCU | Cortex‑M4, STM32F412RG @ 100MHz Cortex-M4F core microcontroller with 1MB Flash memory, 256KB SRAM |
| Flash | 2 MB on‑board SPI flash |
| Wireless | 802.11 b/g/n (on‑module BCM43362 based Wi-Fi SoC ) |
| Display | 128×64 OLED |
| Audio | On‑board codec, microphone and headphone socket |
| Debug | DAPLink / SWD (via micro USB) |
| USB | Micro USB (power, debug, DFU) |
| Power | 3.3 V DC‑DC (max ~1.5 A) |
| I/O | User buttons, RGB LED, status LEDs, multiple GPIOs and sensors |

## Key features

- EMW3166 Wi‑Fi module with integrated MCU and RF
- On‑board 128×64 OLED display
- Audio codec with mic/headphone support
- DAPLink debug interface accessible via micro USB
- Multiple sensors and expansion headers for prototyping

## Flashing and debugging

You can flash and debug the board via the on‑board DAPLink interface exposed on the micro USB connector. Common tools and workflows:

- Use STM32CubeProgrammer (or DAPLink/SEGGER tools) over the DAPLink interface to program and debug the MCU.
- Use the project's existing build/flashing tasks (see top-level `tasks` or the `nanoff` tool) to flash `nanoCLR` images.
