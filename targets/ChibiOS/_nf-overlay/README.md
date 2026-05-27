# **nanoFramework** overlay for ChibiOS

This folder contains code that _extends_ ChibiOS features/functionalities for the purpose of **nanoFramework**.
ChibiOS core code is architecture to include seamless integration of third party code (HAL drivers, boards, etc). That is explained [here](http://wiki.chibios.org/dokuwiki/doku.php?id=chibios:community:guides:community_overlay).
The structure below this folder mimics the structure and organization of ChibiOS Community overlay.
Note that this is designed in a way that in order to use a specific driver, the only thing that a developer has to do is enable the corresponding define in the HAL configuration file at the target folder e.g. HAL_NF_USE_STM32_FLASH in order to enable the flash driver.


## Adding components to **nanoFramework** overlay

When adding HAL components, drivers, boards or any other code to the **nanoFramework** overlay make sure you follow the following guide.

- Add the required source code to the appropriate folder below this one. Follow the existing structure and make sure it follows the one established by ChibiOS. It makes it easier to maintain and for newcomers to better understand it. In particular try to follow (if it makes sense) the design approach of providing a 'main' driver and a low level driver.
- Add a compiler define following the pattern HAL_USE_NNNN for the component and add any required includes in [hal_community.h](os/hal/include/hal_community.h).
- If it requires an initialization function add a call to it wrapped in the appropriate compiler define in [hal_community.c](os/hal/src/hal_community.c).
- Add an entry for each source file (both main and low level driver) and include directories to the CMake module [FindChibiOSnfOverlay.cmake](../../../../CMake/Modules/FindChibiOSnfOverlay.cmake).


## **nanoFramework** overlay components

Follows a list of available components in **nanoFramework** overlay.

- [Driver for internal flash of STM32 parts](stm32-internal-flash-driver.md) (supports F0, F4 and F7 series)
