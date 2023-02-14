#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
include(binutils.Nordic)

# If board not specified set it now...
if(NOT BOARD)
    set(BOARD "nrf9160dk_nrf9160_ns" CACHE INTERNAL "make global")
endif()

# invoke the Zephyr build
nf_setup_target_build()

target_sources(app PRIVATE main.c )
nf_copy_zephyr_build_artifacts()
