#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# grab the board name from the target name
# need to strip out the leading 'TI_' like in 'TI_CC1312R1_LAUNCHXL'

string(REPLACE "TI_" "" TARGET_NAME_SHORT "${TARGET_NAME}")

# includes for SimpleLink CC13x2_26x2 SDK
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/boards/${TARGET_NAME_SHORT}")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/devices/cc13x2_cc26x2")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/devices/cc13x2_cc26x2/driverlib")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/net")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/bsd")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/kernel/tirtos/packages")

# the sources files are pulled into the build in the target CMakelists through the XDS tools command line
