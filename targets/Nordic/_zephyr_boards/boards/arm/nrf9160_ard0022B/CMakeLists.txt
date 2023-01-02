# Copyright (c) .NET Foundation and Contributors
# Portions Copyright (c) 2018 Nordic Semiconductor ASA
# Portions Copyright (c) Ericsson AB 2020, all rights reserved
# See LICENSE file in the project root for full license information.

# Kconfig - nRF91 PCA20035 board configuration

if(CONFIG_BOARD_NRF9160_ARD0022B AND NOT DEFINED CONFIG_MCUBOOT)
	zephyr_library()
	zephyr_library_sources(board_secure.c)
endif()

if(CONFIG_BOARD_NRF9160_ARD0022B_NS)
	zephyr_library()
	zephyr_library_sources(board_nonsecure.c)
endif()
