# Copilot Cloud Agent Instructions for nf-interpreter

## Project Overview

This is the **nf-interpreter** repository — the C/C++ firmware for **.NET nanoFramework**, a managed-code runtime (CLR) for resource-constrained embedded devices. It produces two binaries per target board: **nanoBooter** (bootloader, optional) and **nanoCLR** (the CLR that executes .NET managed applications).

The project targets multiple RTOS platforms (ESP32/ESP-IDF, ChibiOS, FreeRTOS, TI SimpleLink, Eclipse ThreadX) and MCU architectures (ARM Cortex-M, Xtensa, RISC-V).

## Repository Structure

```
src/
├── CLR/              # Common Language Runtime engine
│   ├── Core/         # Heap, GC, type system, execution engine
│   ├── Include/      # Core CLR headers (nanoCLR_Runtime.h, nanoCLR_Types.h, etc.)
│   ├── CorLib/       # Core library native implementations
│   ├── Debugger/     # Debug engine
│   ├── Startup/      # CLR initialization
│   ├── Messaging/    # Wire protocol messaging
│   ├── WireProtocol/ # Debug/deployment wire protocol
│   ├── Helpers/      # Utilities (Base64, nanoprintf, ring buffer)
│   └── Diagnostics/  # Diagnostic support
├── HAL/              # Hardware Abstraction Layer (low-level HW interfaces)
│   └── Include/      # HAL headers (nanoHAL.h, nanoHAL_Boot.h, etc.)
├── PAL/              # Platform Abstraction Layer (OS/platform abstraction)
│   ├── BlockStorage/ # Storage management
│   ├── COM/          # Communications (sockets)
│   ├── Events/       # Event management
│   └── ...
├── DeviceInterfaces/ # Device interface definitions
├── System.Device.*/  # Native implementations for managed device APIs
│                     # (Gpio, I2c, Spi, Pwm, Adc, Dac, I2s, Wifi, etc.)
└── nanoFramework.*/  # Native implementations for nanoFramework-specific APIs
                      # (Graphics, Collections, Security, CAN, OneWire, etc.)

targets/              # Per-RTOS and per-board implementations
├── ESP32/            # ESP-IDF based targets (ESP32, S2, S3, C3, C5, C6, H2, P4)
│   └── defconfig/    # Board defconfig files (Kconfig fragments)
├── ChibiOS/          # ChibiOS RTOS targets (STM32 boards)
├── ThreadX/          # Eclipse ThreadX targets (STM32, Nordic, SiliconLabs, etc.)
├── FreeRTOS/         # FreeRTOS targets (NXP)
└── TI_SimpleLink/    # TI SimpleLink targets

CMake/                # CMake toolchain files, presets, and build modules
├── Modules/          # Per-target GCC options and source file lists
├── toolchain.*.cmake # Cross-compilation toolchain files
└── binutils.*.cmake  # Platform-specific build utilities

config/               # User-local config templates (git-ignored when copied)
Kconfig*              # Kconfig configuration system (feature flags, APIs, RTOS, etc.)
```

## Build System

### Prerequisites

- **CMake 3.31+** and **Ninja** build system.
- Cross-compilation toolchains (ARM GCC, Xtensa GCC, RISC-V GCC) depending on target.
- Platform SDKs (ESP-IDF v5.5.4, ChibiOS, ThreadX, etc.).
- Python 3 with `kconfiglib>=14.1.0` (from `requirements.txt`).
- **Dev containers** are the recommended way to build. Pre-built images:
  - `ghcr.io/nanoframework/dev-container-all` (all platforms)
  - `ghcr.io/nanoframework/dev-container-esp32`
  - `ghcr.io/nanoframework/dev-container-chibios`
  - `ghcr.io/nanoframework/dev-container-azure-rtos` (ThreadX)
  - `ghcr.io/nanoframework/dev-container-freertos-nxp`
  - `ghcr.io/nanoframework/dev-container-ti`

### Building Firmware

The build uses **CMake presets** combined with a **Kconfig** configuration system:

```bash
# 1. Configure (pick a target preset, e.g. ESP_WROVER_KIT)
cmake --preset=ESP_WROVER_KIT -DCMAKE_BUILD_TYPE=Debug

# 2. Build
cmake --build build
```

Each target preset is defined in `targets/<RTOS>/CMakePresets.json` and references a `defconfig` file that sets all Kconfig options (APIs, features, RTOS selection) for that board.

### Key Build Concepts

- **Target presets** inherit from toolchain presets (in `CMake/*.json`) and user config (in `config/`).
- **defconfig files** are Kconfig fragments specifying board features (e.g., `targets/ESP32/defconfig/ESP_WROVER_KIT_defconfig`).
- **User local config**: Copy `config/user-prefs.TEMPLATE.json` → `config/user-prefs.json` and `config/user-tools-repos.TEMPLATE.json` → `config/user-tools-repos.json`, then `config/user-kconfig.conf.TEMPLATE` → `config/user-kconfig.conf` for local overrides. These are git-ignored.
- **Out-of-source builds are mandatory** — building in the source tree is a fatal error.
- **Local builds are supported** when the developer machine has the required toolchains (ARM GCC, Xtensa GCC, RISC-V GCC) and RTOS SDKs installed. The dev containers are the easiest path, but not the only one.
- **Before initiating a build**, check with the user whether they want to build locally (assuming tools are installed) or use a dev container. Do not assume one approach — ask first.
- The cloud agent environment itself does not have embedded cross-compilers pre-installed, so CI builds run in Azure Pipelines and GitHub Actions dev container workflows.
- **Switching targets**: When switching to a different target, the build folder must be cleaned (deleted or emptied) before reconfiguring. Exception: the `_deps` subdirectory inside the build folder may be preserved — it caches downloaded SDKs and repositories and can safely be reused across builds to avoid redundant downloads.
- **ESP32 `sdkconfig` at repo root**: The ESP-IDF build system generates and reads a `sdkconfig` file at the repository root. When building an ESP32 target for the first time, or switching to a different ESP32 target, delete this file if it already exists so the new target's defconfig is applied cleanly. This is not required for non-ESP32 platforms.

### No In-Repo Unit Tests

There is no standalone test suite or test runner in this repository. Testing is done through:
- **Azure Pipelines CI** (`azure-pipelines.yml`): Full build matrix across all platforms.
- **GitHub Actions smoke tests** (`.github/workflows/devcontainer-smoketest.yaml`): Build selected targets in dev containers.
- **Runtime testing** on actual hardware or the Win32 virtual device.

## Coding Conventions

### Language Standards

- **C++20** (`stdcpp20`) for C++ files (`.cpp`).
- **C17** (`stdc17`) for C files (`.c`).
- HAL-level code tends to be C; CLR, PAL, and device API code is C++.

### Formatting

- A `.clang-format` config enforces the **Microsoft** style variant:
  - **120-character column limit**.
  - **4-space indentation** (no tabs).
  - **Allman brace style** (opening brace on its own line after functions, classes, control statements, etc.).
  - **Pointer alignment**: right (`int *ptr`).
  - Short functions are never on a single line.
- An `.editorconfig` enforces: UTF-8 BOM, CRLF line endings for most files, LF for `.sh`, 4-space indent (2 for XML/YAML).

### License Headers

Every source file must start with:

```cpp
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
```

Some files ported from the original .NET Micro Framework also include:
```cpp
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
```

### Naming Conventions

- **Global variables**: `g_` prefix (e.g., `g_CLR_RT_ExecutionEngine`, `g_BootClipboard`).
- **Static fields (C#)**: `s_` prefix.
- **CLR types**: `CLR_` prefix (e.g., `CLR_UINT32`, `CLR_RT_HeapBlock`, `CLR_RT_ExecutionEngine`).
- **HAL types/functions**: `nanoHAL_` prefix.
- **PAL types/functions**: `nanoPAL_` prefix.
- **Boolean config symbols**: `NF_FEATURE_*`, `NF_BUILD_*`, `API_*` (Kconfig naming).
- **Constants**: `c_` prefix (e.g., `c_CLR_RT_Trace_Info`).
- **Macros**: `SCREAMING_SNAKE_CASE`.
- File names use a mix of CamelCase and snake_case depending on the layer.

### Code Style Notes

- Use `HRESULT` return values with `NANOCLR_CHECK_HRESULT` / `NANOCLR_SET_AND_LEAVE` error handling patterns.
- Prefer `CLR_RT_*` type aliases over raw C types for CLR code.
- Platform-conditional code uses `#if defined(VIRTUAL_DEVICE)` for Win32 vs embedded differences.
- Include guards are generally not used; `#pragma once` is not consistently used either — follow existing file patterns.

## Kconfig Configuration System

The project uses **Kconfig** (via Python `kconfiglib`) for feature and API configuration:

- **Root**: `Kconfig` sources `Kconfig.rtos`, `Kconfig.build`, `Kconfig.features`, `Kconfig.apis`, etc.
- **RTOS selection**: `CONFIG_RTOS_ESP32`, `CONFIG_RTOS_CHIBIOS`, `CONFIG_RTOS_FREERTOS`, `CONFIG_RTOS_TI_SIMPLELINK`, `CONFIG_RTOS_THREADX`.
- **API toggles**: `CONFIG_API_SYSTEM_DEVICE_GPIO=y`, `CONFIG_API_SYSTEM_NET=y`, etc.
- **Feature flags**: `CONFIG_NF_FEATURE_DEBUGGER`, `CONFIG_NF_FEATURE_HAS_SDCARD`, `CONFIG_NF_FEATURE_USE_LITTLEFS`, etc.
- **Build modes**: `CONFIG_NF_BUILD_RTM=y` for release builds (suppresses debug output).

When modifying Kconfig files, respect the naming conventions:
- `NF_FEATURE_HAS_*` — hardware capability is present.
- `NF_FEATURE_USE_*` — software feature/stack is enabled.
- `API_*` — managed API is included in the firmware.

## Adding or Modifying Target Boards

Each target board has:
1. A directory under `targets/<RTOS>/<BOARD>/` with `CMakeLists.txt`, `nanoBooter/`, `nanoCLR/`, and `common/` subdirectories.
2. A `defconfig` file (Kconfig fragment) specifying enabled features and APIs.
3. A CMake preset entry in `targets/<RTOS>/CMakePresets.json`.
4. Source file lists in `CMake/Modules/` (e.g., `*_sources.cmake`, `*_GCC_options.cmake`).

## CI/CD

- **Primary CI**: Azure Pipelines (`azure-pipelines.yml`) — builds firmware for all reference targets on `main`, `develop*`, and `release-*` branches.
- **GitHub Actions**: Dev container smoke tests (`.github/workflows/devcontainer-smoketest.yaml`) — builds selected targets in Docker containers, triggered on PRs that modify `.devcontainer/` or `.github/workflows/`.
- **Nightly builds**: `azure-pipelines-nightly.yml`.
- Issues are tracked in the central `nanoframework/Home` repository. Link format: `nanoFramework/Home#NNNN`.

## PR Guidelines

Follow the PR template at `.github/PULL_REQUEST_TEMPLATE.md`:
- Provide a description, motivation/context, and testing details.
- **Linking issues**: Use exactly one of the accepted verbs for GitHub mapping (`Fixes`, `Closes`, or `Resolves`) followed by the issue reference. All issues are tracked in the **Home** repository, so the pattern is always `Fixes/Closes/Resolves nanoFramework/Home#NNNN`. The template lists all three verbs — remove the ones that don't apply, leaving only the one verb and the correct issue number.
- Sometimes a PR may not fully resolve an issue but only addresses part of it. In that case, link the issue in the description but do not use the closing verbs in the issue reference (e.g., "Progress towards nanoFramework/REPO-NAME#NNNN" or "Related to nanoFramework/REPO-NAME#NNNN") so the issue remains open until fully resolved.
- **Change type checkboxes**: Tick **only** the boxes that genuinely describe the change. Each checkbox has a description of what it covers. The categories are mutually exclusive in intent:
  - `Improvement`, `Bug fix`, `New feature`, `Breaking change` — for changes to **source code and algorithms only**.
  - `Config and build` — for changes to the build system, CMake, pipelines, or Kconfig; **not** for source code bugs or features.
  - `Dev Containers` — for changes to dev container definitions only.
  - `Dependencies/declarations` — for dependency updates or assembly declaration changes only.
  - `Documentation` — for documentation-only changes.
  - Do **not** tick multiple categories from different groups (e.g., do not tick both `Bug fix` and `Config and build` for a pipeline fix — only `Config and build` applies).
  - Do **not** remove check box that aren't ticked — the template is designed to cover all change types, so if a box doesn't apply, simply leave it unticked but do not delete it.
- Ensure code follows the project style (`.clang-format`).
- Contributing guidelines are at `https://github.com/nanoframework/.github/blob/main/CONTRIBUTING.md`.

## Common Pitfalls and Workarounds

1. **Local builds require toolchains**: This repo requires embedded cross-compilers (ARM GCC, Xtensa, RISC-V) and RTOS SDKs. Local builds are possible when these tools are installed on the developer machine. If the tools are not available, the dev containers provide a ready-to-use environment. The cloud agent environment itself does not have these tools pre-installed. When a build is needed, **ask the user** whether to build locally or use a dev container before proceeding.
2. **Out-of-source builds only**: Never run CMake in the source root; always use a separate build directory.
3. **No test runner**: There is no `make test` or `ctest` to run. Validation happens through CI builds and on-target testing.
4. **Kconfig Python dependency**: The Kconfig system requires `kconfiglib` (`pip install kconfiglib`). This is needed for `cmake --preset` to work.
5. **Submodules**: The repo has a submodule for community targets (`targets-community`). Clone with `--recurse-submodules` or run `git submodule update --init`.
6. **File encoding**: Most files use UTF-8 with BOM and CRLF line endings (see `.editorconfig`). Kconfig files must NOT have BOM.
7. **ESP32 targets**: ESP-IDF v5.5.4 is the pinned SDK version. The ESP32 build integrates with the IDF build system (idf.py/CMake).
