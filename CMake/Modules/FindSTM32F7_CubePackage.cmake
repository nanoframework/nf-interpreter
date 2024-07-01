#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(stm32f7_hal_driver)
FetchContent_GetProperties(cmsis_device_f7)
FetchContent_GetProperties(cmsis_core)

# set include directories
list(APPEND STM32F7_CubePackage_INCLUDE_DIRS ${cmsis_device_f7_SOURCE_DIR}/Include)
list(APPEND STM32F7_CubePackage_INCLUDE_DIRS ${stm32f7_hal_driver_SOURCE_DIR}/Inc)
list(APPEND STM32F7_CubePackage_INCLUDE_DIRS ${cmsis_core_SOURCE_DIR}/Include)
list(APPEND STM32F7_CubePackage_INCLUDE_DIRS ${TARGET_BASE_LOCATION})

# source files
set(STM32F7_CubePackage_SRCS

    # add HAL files here as required
    stm32f7xx_hal_cortex.c

    # littlefs
    stm32f7xx_hal_dma.c
    stm32f7xx_hal_qspi.c
)

# add exception to compiler warnings as errors
SET_SOURCE_FILES_PROPERTIES(${stm32f7_hal_driver_SOURCE_DIR}/Src/stm32f7xx_hal_eth.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

foreach(SRC_FILE ${STM32F7_CubePackage_SRCS})

    set(STM32F7_CubePackage_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(STM32F7_CubePackage_SRC_FILE ${SRC_FILE}
        PATHS 

            ${stm32f7_hal_driver_SOURCE_DIR}/Src
            ${TARGET_BASE_LOCATION}/common/CubeMX

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${STM32F7_CubePackage_SRC_FILE}")
    endif()

    list(APPEND STM32F7_CubePackage_SOURCES ${STM32F7_CubePackage_SRC_FILE})
endforeach()

list(REMOVE_DUPLICATES STM32F7_CubePackage_INCLUDE_DIRS)

# remove duplicated defines that clash with ChibiOS defines (when used along with)
if(${CHIBIOS_HAL_REQUIRED})

    set(HAL_INCLUDE_FILE ${stm32f7_hal_driver_SOURCE_DIR}/Inc/Legacy/stm32_hal_legacy.h)

    # need to read the supplied files and rename the call
    file(READ
        "${HAL_INCLUDE_FILE}"
        HAL_INCLUDE_FILE_CONTENTS)

    string(REPLACE
        "KR_KEY_"
        "KR_KEY__DUMMY_FOR_NANO_BUILD"
        HAL_INCLUDE_FILE_FINAL_CONTENTS
        "${HAL_INCLUDE_FILE_CONTENTS}")
        
    file(WRITE 
        ${HAL_INCLUDE_FILE} 
        "${HAL_INCLUDE_FILE_FINAL_CONTENTS}")

endif()
 
include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32F7_CubePackage DEFAULT_MSG STM32F7_CubePackage_INCLUDE_DIRS STM32F7_CubePackage_SOURCES)
