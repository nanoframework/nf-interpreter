#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Hardware.Esp32.Rmt")


# set include directories
list(APPEND nanoFramework.Hardware.Esp32.Rmt_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Core")
list(APPEND nanoFramework.Hardware.Esp32.Rmt_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Include")
list(APPEND nanoFramework.Hardware.Esp32.Rmt_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND nanoFramework.Hardware.Esp32.Rmt_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Include")
list(APPEND nanoFramework.Hardware.Esp32.Rmt_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")

# source files
set(nanoFramework.Hardware.Esp32.Rmt_SRCS

nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel.cpp
nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel.cpp
    
)

foreach(SRC_FILE ${nanoFramework.Hardware.Esp32.Rmt_SRCS})
    set(nanoFramework.Hardware.Esp32.Rmt_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.Hardware.Esp32.Rmt_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.Hardware.Esp32.Rmt_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.Hardware.Esp32.Rmt_SOURCES ${nanoFramework.Hardware.Esp32.Rmt_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Hardware.Esp32.Rmt DEFAULT_MSG nanoFramework.Hardware.Esp32.Rmt_INCLUDE_DIRS nanoFramework.Hardware.Esp32.Rmt_SOURCES)
