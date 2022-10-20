#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.Nordic)

if(NOT BOARD)
    # If board not specified set it now...
    set(BOARD "nrf9160_ard0022Bns" CACHE INTERNAL "make global")
endif()

# invoke the zephyr build.
nf_setup_target_build()

target_sources(app PRIVATE main.c )

nf_copy_build_artifacts()
