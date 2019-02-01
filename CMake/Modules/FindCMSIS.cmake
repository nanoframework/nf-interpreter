#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# List of the required include paths
list(APPEND CMSIS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/CMSIS_Source/CMSIS/Core/include/)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CMSIS DEFAULT_MSG CMSIS_INCLUDE_DIRS)
