# Kconfig - nRF91 PCA20035 board configuration
#
# Copyright (c) 2019 Nordic Semiconductor ASA
# Copyright (c) Ericsson AB 2020, all rights reserved
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic

if(CONFIG_BOARD_NRF9160_ARD0022B AND NOT DEFINED CONFIG_MCUBOOT)
	zephyr_library()
	zephyr_library_sources(board_secure.c)
endif()

if(CONFIG_BOARD_NRF9160_ARD0022BNS)
	zephyr_library()
	zephyr_library_sources(board_nonsecure.c)
endif()
