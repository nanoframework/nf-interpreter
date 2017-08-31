![nanoFramework logo](https://github.com/nanoframework/Home/blob/master/resources/logo/nanoFramework-repo-logo.png)

-----

# **nanoFramework** Interpreter

Welcome to the **nanoFramework** Interpreter repository!

This repo contains:

* **nanoFramework** agnostic blocks
  * [CLR](src/CLR)
  * [HAL](src/HAL)
  * [PAL](src/PAL)
* Target reference for CMSIS OS
  * [ChibiOS](targets/CMSIS-OS/ChibiOS)
    * Reference target boards
      * [MBN QUAIL](targets/CMSIS-OS/ChibiOS/MBN_QUAIL)
      * [ST NUCLEO64 F091RC](targets/CMSIS-OS/ChibiOS/ST_NUCLEO64_F091RC)
      * [ST NUCLEO144 F746ZG](targets/CMSIS-OS/ChibiOS/ST_NUCLEO144_F746ZG)
      * [ST STM32F4 DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F4_DISCOVERY)
      * [ST STM32F429I DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F429I_DISCOVERY)
      * [ST STM32F769I DISCOVERY](targets/CMSIS-OS/ChibiOS/ST_STM32F769I_DISCOVERY)
    * ChibiOS overlay for **nanoFramework**
      * [STM32 NUCLEO144 F746ZG (board)](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/boards/ST_NUCLEO144_F746ZG)
      * [STM32 Flash driver](targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_flash)
* Target reference for other OSes
  * [Win32 OS (test project only at this time)](targets/os/win32)
* [CMake files for the build system](CMake)


## Build status

| Component | Build Status |
|:-|---|
| nanoBooter | [![Build Status](https://travis-ci.org/nanoframework/nf-interpreter.svg?branch=master)](https://travis-ci.org/nanoframework/nf-interpreter) |
| nanoCLR | [![Build Status](https://travis-ci.org/nanoframework/nf-interpreter.svg?branch=master)](https://travis-ci.org/nanoframework/nf-interpreter) |
| Win32 test project | [![Build status](https://ci.appveyor.com/api/projects/status/94fldjinqji4w977?svg=true)](https://ci.appveyor.com/project/nfbot/nf-interpreter) |


## Feedback and documentation

For documentation, providing feedback, issues and finding out how to contribute please refer to the [Home repo](https://github.com/nanoframework/Home).

Join our Slack community by filling in this [invite form](https://nanoframework.wordpress.com/slack-invite-form/).


## Credits

The list of contributors to this project can be found at [CONTRIBUTORS](CONTRIBUTORS.md).


## License

The nanoFramework Interpreter is licensed under the [Apache 2.0 license](http://www.apache.org/licenses/LICENSE-2.0).


## Code of Conduct
This project has adopted the code of conduct defined by the [Contributor Covenant](http://contributor-covenant.org/)
to clarify expected behavior in our community.
