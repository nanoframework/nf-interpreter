#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# grab the board name from the target name
# need to strip out the leading 'TI_' like in 'TI_CC3220SF_LAUNCHXL'

string(REPLACE "TI_" "" TARGET_NAME_SHORT "${TARGET_NAME}")

# includes for SimpleLink CC32xx SDK
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/posix/gcc")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/boards/${TARGET_NAME_SHORT}")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/devices/cc32xx")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/devices/cc32xx/driverlib")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/net")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/bsd")

# includes for nanoFramework SSL driver
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets/ssl")

# source file for nanoFramework SSL driver
list(APPEND TI_SimpleLink_SOURCES ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl/ssl.cpp)

# the sources files are pulled into the build in the target CMakelists through the XDS tools command line
