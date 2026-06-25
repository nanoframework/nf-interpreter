#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Hardware.Rp2040")


# set include directories
list(APPEND nanoFramework.Hardware.Rp2040_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(nanoFramework.Hardware.Rp2040_SRCS

    nanoFramework_Hardware_Rp2040.cpp
    nanoFramework_Hardware_Rp2040_nanoFramework_Hardware_Rp2040_Pio_PioBlock.cpp
    nanoFramework_Hardware_Rp2040_nanoFramework_Hardware_Rp2040_Pio_PioBlock_mshl.cpp
    nanoFramework_Hardware_Rp2040_nanoFramework_Hardware_Rp2040_Pio_PioStateMachine.cpp
    nanoFramework_Hardware_Rp2040_nanoFramework_Hardware_Rp2040_Pio_PioStateMachine_mshl.cpp
)

foreach(SRC_FILE ${nanoFramework.Hardware.Rp2040_SRCS})

    set(nanoFramework.Hardware.Rp2040_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Hardware.Rp2040_SRC_FILE ${SRC_FILE}
        PATHS
            ${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Hardware.Rp2040_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Hardware.Rp2040_SOURCES ${nanoFramework.Hardware.Rp2040_SRC_FILE})

endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Hardware.Rp2040 DEFAULT_MSG nanoFramework.Hardware.Rp2040_INCLUDE_DIRS nanoFramework.Hardware.Rp2040_SOURCES)
