## MXChip IoT DevKit AZ3166 ##

![STM STM32F4DISCOVERY board](stm32f4_discovery.jpg)

The board it's a MXChip IoT DevKit AZ3166 from MXChip. The board can be purchased from various sources such as SeedStudio and should be about 35 EUR. Further information on the AZ3166 board can be found in several online resources. Follows the relevant ones:
- [MXChip web site](http://mxchip.com/az3166)
- [Microsoft Azure IoT Kit](https://microsoft.github.io/azure-iot-developer-kit/)
- [Seeed Studio](https://www.seeedstudio.com/AZ3166-IOT-Developer-Kit-p-2922.html)

Some basic information abstracted from MXChip web site:

- EMW3166 Wi-Fi module with:
  - STM32F412RG 100MHz Cortex-M4F core microcontroller with 1MB Flash memory, 256KB SRAM
  - BCM43362 based Wi-Fi SoC
- DAP Link emulator
- Micro USB
- 3.3V DC-DC, maximum current 1.5A
- Codec, with microphone and earphone socket
- OLED, 128x64
- 2 user button
- 1 RGB light
- 3 working status indicator
- Security encryption chip
- Infrared emitter
- Connecting finger extension interface

### Flashing and debugging

This board has one mini USB connector exposing the embedded ST-Link interface that is used for flashing the nanoFramework firmware and for performing debugging on the nanoCLR code.
The second USB connector (a micro USB one) is used to connect the device with Visual Studio allowing to deploy and debug your C# managed applications.
