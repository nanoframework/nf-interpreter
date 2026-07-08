# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Repository Is

**nf-interpreter** is the C/C++ firmware for [.NET nanoFramework](https://www.nanoframework.net/) — a lightweight managed-code CLR for resource-constrained embedded devices. Each supported board produces two binaries: **nanoBooter** (bootloader) and **nanoCLR** (the .NET runtime).

## Build System

**Prerequisites:** CMake 3.31+, Ninja, Python 3 with `kconfiglib>=14.1.0`, and a cross-compiler toolchain appropriate for the target (ARM GCC, Xtensa GCC, RISC-V GCC), plus the relevant RTOS SDK. Local builds are fully supported when these tools are installed; dev containers (`ghcr.io/nanoframework/dev-container-*`) are the easiest path but not the only one.

**Before starting a build, ask the user** whether they want to build locally (assuming tools are installed) or use a dev container. Do not assume one approach.

```bash
# Configure for a specific target (out-of-source is mandatory — in-source is blocked)
cmake --preset=ESP_WROVER_KIT -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build
```

Available presets are defined in `CMakePresets.json` and per-RTOS `targets/<RTOS>/CMakePresets.json`.

**Switching targets:** The build folder must be deleted or emptied before reconfiguring for a different target. The `build/_deps` subdirectory may be preserved — it caches downloaded SDKs and avoids redundant re-downloads.

**ESP32 `sdkconfig`:** ESP-IDF generates a `sdkconfig` file at the repository root. When building an ESP32 target for the first time, or switching to a different ESP32 variant, delete this file first so the new target's defconfig is applied cleanly. This is not needed for non-ESP32 platforms.

**nanoCLR Win32:** `targets/netcore/nanoCLR.sln` is built with Visual Studio — it is a mixed solution of VCx projects covering HAL, PAL, CLR, C# wrappers, and a CLI tool.

**There is no test runner.** Validation happens via CI (Azure Pipelines) and on-target testing. Do not look for `ctest` or a test suite.

For netcore (and win32) target those are VC++ projects linking to the same library source files.

## Repository Architecture

```
src/
  CLR/                  # Runtime engine: heap, GC, type system, execution engine, debugger, wire protocol
  HAL/                  # Hardware Abstraction Layer: low-level HW interfaces, interrupts, timers
  PAL/                  # Platform Abstraction Layer: OS abstraction, block storage, COM, events, filesystem, networking
  DeviceInterfaces/     # Interface definitions for hardware peripherals
  System.Device.*/      # Native implementations for managed APIs (GPIO, I2C, SPI, PWM, ADC, DAC, WiFi, serial, USB)
  nanoFramework.*/      # Native implementations for nanoFramework APIs (Graphics, Security, CAN, OneWire, etc.)
  System.IO.*/          # File system, ports, runtime serialization

targets/
  ESP32/                # ESP-IDF targets: ESP32, S2, S3, C3, C5, C6, H2, P4 and board variants
  ChibiOS/              # ChibiOS RTOS: STM32 family, Raspberry Pi RP2040
  FreeRTOS/             # FreeRTOS: NXP i.MX.RT1060
  ThreadX/              # Eclipse ThreadX: STM32, Nordic, Silicon Labs
  TI_SimpleLink/        # TI CC-series
  win32/                # Win32 virtual device (for development/testing without hardware) deprecated, replaced by netcore version
  netcore/              # Windows virtual device for development/testing without hardware (current verison)

CMake/
  toolchain.*.cmake     # Cross-compiler definitions per architecture
  binutils.*.cmake      # RTOS-specific build utilities
  Modules/              # Per-target GCC options and source file lists

config/                 # Git-ignored local config (copy *.TEMPLATE.* files to use)
install-scripts/        # PowerShell scripts to install toolchains
```

## Feature & API Configuration (Kconfig)

The build uses **Kconfig** (Python `kconfiglib`) for feature toggles. The root `Kconfig` file sources `Kconfig.rtos`, `Kconfig.features`, `Kconfig.apis`, `Kconfig.networking`, etc. Each board target has a `defconfig` file in `targets/<RTOS>/defconfig/`.

Key config namespaces:
- `CONFIG_RTOS_*` — selects the RTOS
- `CONFIG_API_*` — enables managed API namespaces (e.g., `CONFIG_API_SYSTEM_DEVICE_GPIO=y`)
- `CONFIG_NF_FEATURE_*` — feature flags (debugger, SD card, LittleFS, etc.)
- `CONFIG_NF_BUILD_RTM=y` — release mode (optimizes size, suppresses debug output)

Override via `config/user-kconfig.conf` (copy from `config/user-kconfig.conf.TEMPLATE`).

## Code Conventions

**Language standards:** C++23 (`stdcpp23`) for `.cpp`, C23 (`stdc23`) for `.c`. HAL-level code tends to be C; CLR, PAL, and device API code is C++.

**Formatting:** Enforced by `.clang-format` — Microsoft style, 120-char columns, 4-space indent (no tabs), Allman braces, right-aligned pointers (`int *ptr`).

**File encoding:** UTF-8 with BOM + CRLF for most files. Kconfig files must **not** have BOM. Shell scripts use LF.

**Naming:**
- Global variables: `g_` prefix (e.g., `g_CLR_RT_ExecutionEngine`)
- Static fields: `s_` prefix
- CLR types: `CLR_` prefix (e.g., `CLR_UINT32`, `CLR_RT_HeapBlock`)
- Constants: `c_` prefix
- HAL/PAL functions: `nanoHAL_` / `nanoPAL_` prefixes
- Macros: `SCREAMING_SNAKE_CASE`

**License header** required in every source file:
```cpp
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
```

## CI/CD

- **Azure Pipelines** (`azure-pipelines.yml`): Full build matrix across all reference targets on main/develop/release branches.
- **GitHub Actions** (`.github/workflows/`): Dev container smoke tests.
- Issues are tracked in the central [nanoframework/Home](https://github.com/nanoframework/Home) repository.

## Pull Requests

- Link issues using exactly one of: `Fixes`, `Closes`, or `Resolves` followed by `nanoFramework/Home#NNNN`.
- PRs must pass `.clang-format` checks.
- See `.github/PULL_REQUEST_TEMPLATE.md` for the change-type checklist.
