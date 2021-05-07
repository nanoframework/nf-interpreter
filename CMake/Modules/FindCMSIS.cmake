#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(cmsis)

# List of the required include paths
list(APPEND CMSIS_INCLUDE_DIRS ${cmsis_SOURCE_DIR}/CMSIS/Core/Include/)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CMSIS DEFAULT_MSG CMSIS_INCLUDE_DIRS)
