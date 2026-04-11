# Raspberry Pi Pico (RP2040) — nanoFramework Target

## Overview

This target supports the **Raspberry Pi Pico** board with the RP2040 microcontroller running ChibiOS/RT.

- **MCU:** RP2040 — Dual-core ARM Cortex-M0+ @ 125 MHz
- **RAM:** 264 KB SRAM
- **Flash:** 2 MB XIP (execute-in-place)
- **RTOS:** ChibiOS/RT (single-core)

## Flashing

The RP2040 uses **UF2 (USB Flashing Format)**. To flash:

1. Hold the **BOOTSEL** button on the Pico while plugging in USB
2. The Pico appears as a USB mass storage device (`RPI-RP2`)
3. Drag and drop the `nanoCLR.uf2` file onto the drive
4. The Pico reboots automatically and runs nanoFramework

The `nanoCLR.uf2` file contains both **nanoBooter** and **nanoCLR** combined.

## Memory Layout

| Region | Start Address | Size | Description |
|--------|--------------|------|-------------|
| nanoBooter | `0x10000000` | 64 KB | Boot loader |
| Config | `0x10010000` | 32 KB | Configuration block |
| nanoCLR | `0x10018000` | 928 KB | CLR runtime + managed assemblies |
| Deployment | `0x10100000` | 1 MB | Managed application deployment |

## Pin Mapping

### GPIO

All 30 GPIO pins (GP0–GP29) are available. Pin numbers in nanoFramework correspond directly to RP2040 GPIO numbers.

```csharp
// Example: Blink the on-board LED (GP25 on Pico)
var led = new GpioController();
var pin = led.OpenPin(25, PinMode.Output);
pin.Write(PinValue.High);
```

### SPI Bus Configuration

| Bus | Function | GPIO | Pico Pin |
|-----|----------|------|----------|
| SPI0 | SCK | GP18 | 24 |
| SPI0 | MOSI | GP19 | 25 |
| SPI0 | MISO | GP16 | 21 |
| SPI1 | SCK | GP10 | 14 |
| SPI1 | MOSI | GP11 | 15 |
| SPI1 | MISO | GP12 | 16 |

```csharp
// SPI0 example
var settings = new SpiConnectionSettings(0, chipSelectPin);  // bus 0 = SPI0
settings.ClockFrequency = 1_000_000;
var spi = SpiDevice.Create(settings);
```

### I2C Bus Configuration

| Bus | Function | GPIO | Pico Pin |
|-----|----------|------|----------|
| I2C0 | SDA | GP4 | 6 |
| I2C0 | SCL | GP5 | 7 |
| I2C1 | SDA | GP6 | 9 |
| I2C1 | SCL | GP7 | 10 |

```csharp
// I2C0 example (bus index 0)
var settings = new I2cConnectionSettings(0, 0x48);  // bus 0 = I2C0
var device = I2cDevice.Create(settings);
```

> **Note:** I2C buses are 0-indexed on RP2040. Bus 0 = I2C0, Bus 1 = I2C1.

### PWM

RP2040 has 8 PWM slices with 2 channels each (A and B). The pin-to-PWM mapping follows a simple formula:

- **Slice** = GPIO number / 2
- **Channel** = GPIO number % 2 (0 = A, 1 = B)

| GPIO | Slice | Channel | GPIO | Slice | Channel |
|------|-------|---------|------|-------|---------|
| GP0 | 0 | A | GP1 | 0 | B |
| GP2 | 1 | A | GP3 | 1 | B |
| GP4 | 2 | A | GP5 | 2 | B |
| GP6 | 3 | A | GP7 | 3 | B |
| GP8 | 4 | A | GP9 | 4 | B |
| GP10 | 5 | A | GP11 | 5 | B |
| GP12 | 6 | A | GP13 | 6 | B |
| GP14 | 7 | A | GP15 | 7 | B |
| GP16 | 0 | A | GP17 | 0 | B |
| GP18 | 1 | A | GP19 | 1 | B |
| GP20 | 2 | A | GP21 | 2 | B |
| GP22 | 3 | A | GP23 | 3 | B |
| GP24 | 4 | A | GP25 | 4 | B |
| GP26 | 5 | A | GP27 | 5 | B |
| GP28 | 6 | A | GP29 | 6 | B |

```csharp
// PWM on GP25 (on-board LED) — Slice 4, Channel B
var pwm = PwmChannel.CreateUnstarted(4, 25);  // timer=4 (slice), pin=25
pwm.Frequency = 1000;
pwm.DutyCycle = 0.5;
pwm.Start();
```

> **Note:** Two GPIOs sharing the same slice share the same frequency but can have different duty cycles.

### ADC

| Channel | GPIO | Pico Pin | Description |
|---------|------|----------|-------------|
| 0 | GP26 | 31 | ADC0 |
| 1 | GP27 | 32 | ADC1 |
| 2 | GP28 | 34 | ADC2 |
| 3 | GP29 | — | ADC3 (VSYS/3 on Pico board) |
| 4 | — | — | Internal temperature sensor |

```csharp
// Read ADC channel 0 (GP26)
var adc = new AdcController();
var channel = adc.OpenChannel(0);
int value = channel.ReadValue();  // 0–4095 (12-bit)

// Read internal temperature sensor (channel 4)
var tempChannel = adc.OpenChannel(4);
int raw = tempChannel.ReadValue();
// Temperature (°C) ≈ 27 - (raw * 3.3 / 4096 - 0.706) / 0.001721
```

- **Resolution:** 12-bit (0–4095)
- **Reference voltage:** 3.3V (internal)

## Pico Board Pinout Reference

```
         ┌──────────────────┐
    GP0  │ 1             40 │  VBUS
    GP1  │ 2             39 │  VSYS
    GND  │ 3             38 │  GND
    GP2  │ 4             37 │  3V3_EN
    GP3  │ 5             36 │  3V3
    GP4  │ 6    ┌─────┐  35 │  ADC_VREF
    GP5  │ 7    │ USB │  34 │  GP28 (ADC2)
    GND  │ 8    └─────┘  33 │  GND
    GP6  │ 9             32 │  GP27 (ADC1)
    GP7  │10             31 │  GP26 (ADC0)
    GP8  │11             30 │  RUN
    GP9  │12             29 │  GP22
    GND  │13             28 │  GND
    GP10 │14             27 │  GP21
    GP11 │15             26 │  GP20
    GP12 │16             25 │  GP19
    GP13 │17             24 │  GP18
    GND  │18             23 │  GND
    GP14 │19             22 │  GP17
    GP15 │20             21 │  GP16
         └──────────────────┘

    On-board LED: GP25
```

## Known Limitations

- **No hardware TRNG:** RP2040 lacks a true random number generator. `USE_RNG` is disabled.
- **No FPU:** Cortex-M0+ has no floating-point unit. Floating-point operations use software emulation.
- **Single GPIO bank:** All 30 pins are in one bank (`IOPORT1`).
- **ADC channel 3 (GP29):** On the standard Pico board, this pin is connected to VSYS/3 (power supply monitoring). It can still be used as a general ADC input on custom boards.
- **SPI/I2C pin flexibility:** The default pin assignments are for the standard Pico board. RP2040 supports flexible pin routing — custom boards may use different GPIO assignments.

## Build Configuration

Key CMake preset options (`CMakePresets.json`):

```json
{
  "API_System.Device.Gpio": "ON",
  "API_System.Device.Spi": "ON",
  "API_System.Device.I2c": "ON",
  "API_System.Device.Pwm": "ON",
  "API_System.Device.Adc": "ON",
  "USE_RNG": "OFF"
}
```
