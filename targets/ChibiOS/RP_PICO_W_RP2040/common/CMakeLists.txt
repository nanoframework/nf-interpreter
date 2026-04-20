#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append source files required for ChibiOS interface (usbcfg.c from shared _RP2040)
list(APPEND COMMON_PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/targets/ChibiOS/_RP2040/common/usbcfg.c")
list(APPEND COMMON_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Device_BlockStorage$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-DEBUG>.c")
list(APPEND COMMON_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager.cpp")

# make var global
set(COMMON_PROJECT_SOURCES ${COMMON_PROJECT_SOURCES} CACHE INTERNAL "make global")
