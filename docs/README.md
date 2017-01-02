# Documents Index

This repo includes several documents that explain both high-level and low-level
concepts of the **nano Framework**. These are very useful for contributors,
to get context that can be very difficult to acquire from just reading code.

## Introduction

> This project is in early stages of its development.

The nano Framework is an open source platform that enables writing managed code
applications for embedded devices. Developers can use familiar environment (IDE)
such as Visual Studio and .NET (C#) knowledge to quickly write applications without
having to worry about the low level hardware intricacies of each microcontroller.

The nano Framework is based on [.NET Micro Framework Interpreter](https://github.com/NETMF/netmf-interpreter) (NETMF)
with several key differences to address the most important NETMF shortcomings
(which are the build system, PAL and the overall porting process).

The nano Framework provides architecture neutral libraries for its core components

* Bootloader (optional),
* Core Language Runtime (CLR),
* Framework Class Library (FCL).

By leveraging the configuration and build system support of existing real-time
operating systems (RTOS), such as mbed-os, FreeRTOS, ChibiOS or NuttX, along
with HAL and PAL drivers (where available), the nano Framework can be easily
used on vast majority of hardware boards with minimal changes.

> The nano Framework initial implementation targets ARM Cortex-M architecture
and requires CMSIS-compatible RTOS. The support for other systems and architectures
is under development.

## Getting Started

<!--- [??????](https://?????)-->

## Project Documentation

- [Developer Guide](project-documentation/developer-guide.md)
- [Project priorities](project-documentation/project-priorities.md)
- [Contributing to **nano Framework**](project-documentation/contributing.md)
- [Contributing Workflow](project-documentation/contributing-workflow.md)

## Build **nano Framework**

- [Building **nano Framework**](building/build-instructions.md)

## Testing and Debugging **nano Framework**

- [Debugging **nano Framework** with VS Code](building/vscode-debug-instructions.md)

## CMake Documentation

- [CMake configuration files](cmake-documentation/README.md)
