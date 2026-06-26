#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Hardware.Pico")


# set include directories
list(APPEND nanoFramework.Hardware.Pico_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(nanoFramework.Hardware.Pico_SRCS

    nanoFramework_hardware_pico_native.cpp
    nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock.cpp
    nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine.cpp
    nanoFramework_hardware_pico_native_PioIrqDriver.cpp
    nanoFramework_hardware_pico_native_PioIrqHandlers.c
)

foreach(SRC_FILE ${nanoFramework.Hardware.Pico_SRCS})

    set(nanoFramework.Hardware.Pico_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Hardware.Pico_SRC_FILE ${SRC_FILE}
        PATHS
            ${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Hardware.Pico_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Hardware.Pico_SOURCES ${nanoFramework.Hardware.Pico_SRC_FILE})

endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Hardware.Pico DEFAULT_MSG nanoFramework.Hardware.Pico_INCLUDE_DIRS nanoFramework.Hardware.Pico_SOURCES)
