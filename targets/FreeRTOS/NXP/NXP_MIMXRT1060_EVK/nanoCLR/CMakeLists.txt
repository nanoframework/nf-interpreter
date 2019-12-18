#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append nanoCLR source files
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.c")

# make var global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")
