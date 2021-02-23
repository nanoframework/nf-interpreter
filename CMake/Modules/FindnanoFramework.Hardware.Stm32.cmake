#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Hardware.Stm32")


# set include directories
list(APPEND nanoFramework.Hardware.Stm32_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(nanoFramework.Hardware.Stm32_SRCS

    nf_hardware_stm32_native.cpp
    nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory.cpp
    nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power.cpp
    nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_RTC.cpp
    nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities.cpp
)

foreach(SRC_FILE ${nanoFramework.Hardware.Stm32_SRCS})

    set(nanoFramework.Hardware.Stm32_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Hardware.Stm32_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Hardware.Stm32_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Hardware.Stm32_SOURCES ${nanoFramework.Hardware.Stm32_SRC_FILE})
    
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Hardware.Stm32 DEFAULT_MSG nanoFramework.Hardware.Stm32_INCLUDE_DIRS nanoFramework.Hardware.Stm32_SOURCES)
