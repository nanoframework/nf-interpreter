#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append nanoCLR source files
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoHAL.cpp")

# default Wi-Fi configuration for this target
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ti_drivers_net_wifi_Config.c")

# make var global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")
