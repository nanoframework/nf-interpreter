#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

idf_component_register(
    SRCS "boot_msg.c"
    INCLUDE_DIRS ${CMAKE_BINARY_DIR}/../targets/ESP32
)

target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=bootloader_print_banner")
