#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Hardware.Rpi")


# set include directories
list(APPEND nanoFramework.Hardware.Rpi_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(nanoFramework.Hardware.Rpi_SRCS

    nanoFramework_Hardware_Rpi.cpp
    nanoFramework_Hardware_Rpi_nanoFramework_Hardware_Rpi_Pio_PioBlock.cpp
    nanoFramework_Hardware_Rpi_nanoFramework_Hardware_Rpi_Pio_PioBlock_mshl.cpp
    nanoFramework_Hardware_Rpi_nanoFramework_Hardware_Rpi_Pio_PioStateMachine.cpp
    nanoFramework_Hardware_Rpi_nanoFramework_Hardware_Rpi_Pio_PioStateMachine_mshl.cpp
    nanoFramework_Hardware_Rpi_PioIrqDriver.cpp
    nanoFramework_Hardware_Rpi_PioIrqHandlers.c
)

foreach(SRC_FILE ${nanoFramework.Hardware.Rpi_SRCS})

    set(nanoFramework.Hardware.Rpi_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Hardware.Rpi_SRC_FILE ${SRC_FILE}
        PATHS
            ${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Hardware.Rpi_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Hardware.Rpi_SOURCES ${nanoFramework.Hardware.Rpi_SRC_FILE})

endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Hardware.Rpi DEFAULT_MSG nanoFramework.Hardware.Rpi_INCLUDE_DIRS nanoFramework.Hardware.Rpi_SOURCES)
