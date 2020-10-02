#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# set include directories
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/include/lwip)
list(APPEND nanoFramework.Networking.Sntp_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND nanoFramework.Networking.Sntp_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp")


# source files
set(nanoFramework.Networking.Sntp_SRCS

    # source files of the assembly
    nf_networking_sntp_nanoFramework_Networking_Sntp.cpp
    nf_networking_sntp.cpp    
)

foreach(SRC_FILE ${nanoFramework.Networking.Sntp_SRCS})
    set(nanoFramework.Networking.Sntp_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.Networking.Sntp_SRC_FILE ${SRC_FILE}
        PATHS

            # path for source files of this module
            ${PROJECT_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.Networking.Sntp_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.Networking.Sntp_SOURCES ${nanoFramework.Networking.Sntp_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Networking.Sntp DEFAULT_MSG nanoFramework.Networking.Sntp_INCLUDE_DIRS nanoFramework.Networking.Sntp_SOURCES)
