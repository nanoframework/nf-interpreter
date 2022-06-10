#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append common source files
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device_BlockStorage$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-DEBUG>.c)

# this needs to be added here, otherewise the stack and heap arrays are not allocated
list(APPEND COMMON_PROJECT_SOURCES ${gecko_sdk_SOURCE_DIR}/platform/common/toolchain/src/sl_memory.c)

# make var global
set(COMMON_PROJECT_SOURCES ${COMMON_PROJECT_SOURCES} CACHE INTERNAL "make global")
