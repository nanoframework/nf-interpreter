#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(simplelinkcc13x2_26x2sdk)

# grab the board name from the target name
# need to strip out the leading 'TI_' like in 'TI_CC1312R1_LAUNCHXL'

string(REPLACE "TI_" "" TARGET_NAME_SHORT "${TARGET_NAME}")

# includes for SimpleLink CC13x2_26x2 SDK
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR})
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source)
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/devices)
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/drivers)
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/boards/${TARGET_NAME_SHORT})
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/devices/cc13x2_cc26x2)
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/devices/cc13x2_cc26x2/driverlib)
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/net)
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/bsd)
list(APPEND TI_SimpleLink_INCLUDE_DIRS ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/kernel/tirtos/packages)

# the sources files are pulled into the build in the target CMakelists through the XDS tools command line
