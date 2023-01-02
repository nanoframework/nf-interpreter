# Copyright (c) .NET Foundation and Contributors
# Portions Copyright (c) 2018 Nordic Semiconductor ASA
# Portions Copyright (c) Ericsson AB 2020, all rights reserved
# See LICENSE file in the project root for full license information.


board_runner_args(nrfjprog "--nrf-family=NRF91" "--softreset")
board_runner_args(jlink "--device=cortex-m33" "--speed=4000")
include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
