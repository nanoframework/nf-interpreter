#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(esp32_idf)

include(binutils.ESP32)

# app_trace
macro(add_app_trace)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/app_trace/include)

    set(app_trace_srcs

        app_trace.c
        app_trace_util.c
        host_file_io.c
        gcov_rtio.c
    )

    # TODO check option
    if(CONFIG_SYSVIEW_ENABLE)
    endif()

    # TODO check option
    if(CONFIG_HEAP_TRACING_TOHOST)
    endif()

    set(app_trace_locations

        ${esp32_idf_SOURCE_DIR}/components/app_trace
        ${esp32_idf_SOURCE_DIR}/components/app_trace/gcov
    )

    nf_find_esp32_files_at_location("${app_trace_srcs}" "${app_trace_locations}")

endmacro()

# app_update
macro(add_app_update)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/app_update/include)

    set(app_update_srcs

        esp_ota_ops.c
        esp_app_desc.c
    )

    nf_find_esp32_files_at_location("${app_update_srcs}" "${esp32_idf_SOURCE_DIR}/components/app_update")

endmacro()

# bootloader_support
macro(add_bootloader_support)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/bootloader_support/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/bootloader_support/include_bootloader)

    set(bootloader_support_srcs

        bootloader_common.c
        bootloader_common_loader.c
        bootloader_clock_init.c
        bootloader_flash.c
        bootloader_mem.c
        bootloader_random.c
        bootloader_random_${TARGET_SERIES_SHORT}.c
        bootloader_utility.c
        esp_image_format.c
        flash_encrypt.c
        secure_boot.c
        flash_partitions.c
        flash_qio_mode.c
        bootloader_flash_config_${TARGET_SERIES_SHORT}.c
        bootloader_efuse_${TARGET_SERIES_SHORT}.c
        bootloader_sha.c
    )

    set(bootloader_support_locations

        ${esp32_idf_SOURCE_DIR}/components/bootloader_support/src
        ${esp32_idf_SOURCE_DIR}/components/bootloader_support/src/idf
        ${esp32_idf_SOURCE_DIR}/components/bootloader_support/src/${TARGET_SERIES_SHORT}
    )

    nf_find_esp32_files_at_location("${bootloader_support_srcs}" "${bootloader_support_locations}")

endmacro()

# driver
macro(add_driver)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/include)

    set(driver_srcs

        gpio.c
        i2c.c
        i2s.c
        ledc.c
        pcnt.c
        periph_ctrl.c
        rmt.c
        rtc_io.c
        rtc_module.c
        sdspi_crc.c
        sdspi_host.c
        sdspi_transaction.c
        sigmadelta.c
        spi_common.c
        spi_master.c
        spi_slave.c
        spi_bus_lock.c
        timer.c
        twai.c
        uart.c
    )

    nf_find_esp32_files_at_location("${driver_srcs}" "${esp32_idf_SOURCE_DIR}/components/driver")

    if(${TARGET_SERIES} STREQUAL "ESP32")
        # TODO
    elseif(${TARGET_SERIES} STREQUAL "ESP32_S2")

        list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/esp32s2/include)

        set(driver_esp32s2_srcs

            adc_common.c
            dac_common.c
            dedic_gpio.c
            spi_slave_hd.c
            touch_sensor_common.c
            rtc_tempsensor.c
            touch_sensor.c
            adc.c
            adc2_init_cal.c
            dac.c    
        )

        set(driver_esp32s2_locations

            ${esp32_idf_SOURCE_DIR}/components/driver
            ${esp32_idf_SOURCE_DIR}/components/driver/${TARGET_SERIES_SHORT}
        )
        
        nf_find_esp32_files_at_location("${driver_esp32s2_srcs}" "${driver_esp32s2_locations}")

    elseif(${TARGET_SERIES} STREQUAL "ESP32_C3")
        # TODO
    endif()

endmacro()

# efuse
macro(add_efuse)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/efuse/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/efuse/private_include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/efuse/${TARGET_SERIES_SHORT}/include)

    set(efuse_srcs

        esp_efuse_api.c
        esp_efuse_fields.c
        esp_efuse_utility.c
        esp_efuse_table.c
    )

    set(efuse_locations

        ${esp32_idf_SOURCE_DIR}/components/efuse/src
        ${esp32_idf_SOURCE_DIR}/components/efuse/${TARGET_SERIES_SHORT}
    )

    nf_find_esp32_files_at_location("${efuse_srcs}" "${efuse_locations}")

endmacro()

# esp_common
macro(add_esp_common)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_common/include)

    set(esp_common_srcs

        brownout.c
        esp_err.c
        esp_err_to_name.c
        freertos_hooks.c
        mac_addr.c
        stack_check.c
        task_wdt.c
        int_wdt.c
    )

    nf_find_esp32_files_at_location("${esp_common_srcs}" "${esp32_idf_SOURCE_DIR}/components/esp_common/src")

    if(NOT ${TARGET_SERIES} STREQUAL "ESP32_C3")
        list(APPEND ESP32_IDF_SOURCES ${esp32_idf_SOURCE_DIR}/components/esp_common/src/dbg_stubs.c)
    endif()

endmacro()

# esp_ipc
macro(add_esp_ipc)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_ipc/include)

    set(esp_ipc_srcs

        ipc.c
    )

    nf_find_esp32_files_at_location("${esp_ipc_srcs}" "${esp32_idf_SOURCE_DIR}/components/esp_ipc")

endmacro()

# esp_hw_support
macro(add_esp_hw_support)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_hw_support/include)

    set(esp_hw_support_srcs

        cpu_util.c
        clk_ctrl_os.c
    )

    nf_find_esp32_files_at_location("${esp_hw_support_srcs}" "${esp32_idf_SOURCE_DIR}/components/esp_hw_support")

    if(${TARGET_SERIES} STREQUAL "ESP32")
        # TODO
    elseif(${TARGET_SERIES} STREQUAL "ESP32_S2")

        list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_hw_support/port/esp32s2/include)
        list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_hw_support/port/esp32s2/private_include)

        set(esp_hw_support_esp32s2_srcs

            rtc_clk.c
            rtc_clk_init.c
            rtc_init.c
            rtc_pm.c
            rtc_sleep.c
            rtc_time.c
            rtc_wdt.c
            regi2c_ctrl.c
        )

        nf_find_esp32_files_at_location("${esp_hw_support_esp32s2}" "${esp32_idf_SOURCE_DIR}/components/esp_hw_support/port/esp32s2")

    elseif(${TARGET_SERIES} STREQUAL "ESP32_C3")
        # TODO
    endif()

endmacro()

# esp_pm
macro(add_esp_pm)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_pm/include)

    set(esp_pm_srcs

        pm_locks.c
        pm_trace.c
        pm_impl.c
    )

    nf_find_esp32_files_at_location("${esp_pm_srcs}" "${esp32_idf_SOURCE_DIR}/components/esp_pm")

endmacro()

# esp_ringbuf
macro(add_esp_ringbuf)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_ringbuf/include)

    set(esp_ringbuf_srcs

        ringbuf.c
    )

    nf_find_esp32_files_at_location("${esp_ringbuf_srcs}" "${esp32_idf_SOURCE_DIR}/components/esp_ringbuf")

endmacro()

# esp_rom
macro(add_esp_rom)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_rom/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_rom/${TARGET_SERIES_SHORT})
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_rom/include/${TARGET_SERIES_SHORT})

    set(esp_rom_srcs

        esp_rom_crc.c
        esp_rom_sys.c
        esp_rom_uart.c
    )

    # TODO check option
    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        list(APPEND sources "patches/esp_rom_longjmp.S")
    endif()

    nf_find_esp32_files_at_location("${esp_rom_srcs}" "${esp32_idf_SOURCE_DIR}/components/esp_rom/patches")

    if(${TARGET_SERIES} STREQUAL "ESP32")
        # TODO
    elseif(${TARGET_SERIES} STREQUAL "ESP32_S2")

        set(esp_rom_esp32s2_srcs

            usb_descriptors.c
        )
        
        nf_find_esp32_files_at_location("${esp_rom_esp32s2_srcs}" "${esp32_idf_SOURCE_DIR}/components/esp_rom/esp32s2")

    elseif(${TARGET_SERIES} STREQUAL "ESP32_C3")
        # TODO
    endif()

endmacro()

# esp_system
macro(add_esp_system)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_system/include)

    set(esp_system_srcs

        intr_alloc.c
        esp_async_memcpy.c
        panic.c
        system_api.c
        startup.c
        system_time.c
        sleep_modes.c
        
        cpu_start.c
        panic_handler.c
    )

    set(esp_system_locations

        ${esp32_idf_SOURCE_DIR}/components/esp_system/
        ${esp32_idf_SOURCE_DIR}/components/esp_system/port
    )

    nf_find_esp32_files_at_location("${esp_system_srcs}" "${esp_system_locations}")

    if(${TARGET_SERIES} STREQUAL "ESP32")
        # TODO
    elseif(${TARGET_SERIES} STREQUAL "ESP32_S2")

        set(esp_system_esp32s2_srcs

            async_memcpy_impl_cp_dma.c
            dport_panic_highint_hdl.S
            clk.c
            reset_reason.c

            panic_arch.c
            panic_handler_asm.S
        )

        set(esp_system_esp32s2_locations

            ${esp32_idf_SOURCE_DIR}/components/esp_system/port/soc/${TARGET_SERIES_SHORT}
            ${esp32_idf_SOURCE_DIR}/components/esp_system/port/arch/xtensa
        )
        
        nf_find_esp32_files_at_location("${esp_system_esp32s2_srcs}" "${esp_system_esp32s2_locations}")

    elseif(${TARGET_SERIES} STREQUAL "ESP32_C3")
        # TODO
    endif()

    # TODO check option
    if(CONFIG_ESP_CONSOLE_USB_CDC)
        target_sources(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/usb_console.c")
    endif()

endmacro()

# esp_timer
macro(add_esp_timer)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_timer/include)

    set(esp_timer_srcs

        esp_timer.c
        ets_timer_legacy.c
    )

    nf_find_esp32_files_at_location("${esp_timer_srcs}" "${esp32_idf_SOURCE_DIR}/components/esp_timer/src")

    # TODO check option
    if(CONFIG_ESP_TIMER_IMPL_FRC2)
        list(APPEND ESP32_IDF_SOURCES ${esp32_idf_SOURCE_DIR}/components/esp_timer/src/esp_timer_impl_frc_legacy.c)
    elseif(CONFIG_ESP_TIMER_IMPL_TG0_LAC)
        list(APPEND ESP32_IDF_SOURCES ${esp32_idf_SOURCE_DIR}/components/esp_timer/src/esp_timer_impl_lac.c)
    elseif(CONFIG_ESP_TIMER_IMPL_SYSTIMER)
        list(APPEND ESP32_IDF_SOURCES ${esp32_idf_SOURCE_DIR}/components/esp_timer/src/esp_timer_impl_systimer.c)
    endif()

endmacro()

# add_espcoredump
macro(add_espcoredump)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/espcoredump/include)

    set(espcoredump_srcs

        core_dump_common.c
        core_dump_checksum.c
        core_dump_flash.c
        core_dump_uart.c
        core_dump_elf.c
        core_dump_binary.c
    )

    nf_find_esp32_files_at_location("${espcoredump_srcs}" "${esp32_idf_SOURCE_DIR}/components/espcoredump/src")

    # TODO check option
    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        set(srcs ${srcs} port/xtensa/core_dump_port.c)
        set(priv_includes ${priv_includes} "include_core_dump/port/xtensa")
    elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
        set(srcs ${srcs} port/riscv/core_dump_port.c)
        set(priv_includes ${priv_includes} "include_core_dump/port/riscv")
    endif()
  
endmacro()

# hal
macro(add_hal)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/hal/include)

    set(hal_srcs

        wdt_hal_iram.c
        mpu_hal.c
        cpu_hal.c
        rmt_hal.c
        rtc_io_hal.c
        spi_hal.c
        spi_hal_iram.c
        spi_slave_hal.c
        spi_slave_hal_iram.c
        i2s_hal.c
        sigmadelta_hal.c
        timer_hal.c
        ledc_hal.c
        ledc_hal_iram.c
        i2c_hal.c
        i2c_hal_iram.c
        gpio_hal.c
        uart_hal.c
        uart_hal_iram.c
        spi_flash_hal.c
        spi_flash_hal_iram.c
        soc_hal.c
        interrupt_controller_hal.c
        sha_hal.c
        aes_hal.c
        twai_hal.c
        twai_hal_iram.c
        adc_hal.c
    )

    nf_find_esp32_files_at_location("${hal_srcs}" "${esp32_idf_SOURCE_DIR}/components/hal")

    if(${TARGET_SERIES} STREQUAL "ESP32")
        # TODO
    elseif(${TARGET_SERIES} STREQUAL "ESP32_S2")

        list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/hal/esp32s2/include)

        set(hal_esp32s2_srcs

            dac_hal.c
            pcnt_hal.c
            spi_flash_hal_gpspi.c
            spi_slave_hd_hal.c
            touch_sensor_hal.c
            adc_hal.c
            brownout_hal.c
            cp_dma_hal.c
            systimer_hal.c
            touch_sensor_hal.c
            dac_hal.c
            interrupt_descriptor_table.c
            usb_hal.c
            usbh_hal.c
        )

        set(hal_esp32s2_locations

            ${esp32_idf_SOURCE_DIR}/components/hal
            ${esp32_idf_SOURCE_DIR}/components/hal/${TARGET_SERIES_SHORT}
        )
        
        nf_find_esp32_files_at_location("${hal_esp32s2_srcs}" "${hal_esp32s2_locations}")

    elseif(${TARGET_SERIES} STREQUAL "ESP32_C3")
        # TODO
    endif()

endmacro()

# log
macro(add_log)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/log/include)

    set(log_srcs

        log.c
        log_buffers.c
    )

    nf_find_esp32_files_at_location("${log_srcs}" "${esp32_idf_SOURCE_DIR}/components/log")

endmacro()

# nvs_flash
macro(add_nvs_flash)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/nvs_flash/include)

    set(nvs_flash_srcs

        nvs_api.cpp
        nvs_cxx_api.cpp
        nvs_item_hash_list.cpp
        nvs_page.cpp
        nvs_pagemanager.cpp
        nvs_storage.cpp
        nvs_handle_simple.cpp
        nvs_handle_locked.cpp
        nvs_partition.cpp
        nvs_partition_lookup.cpp
        nvs_partition_manager.cpp
        nvs_types.cpp
    )

    nf_find_esp32_files_at_location("${nvs_flash_srcs}" "${esp32_idf_SOURCE_DIR}/components/nvs_flash/src")

endmacro()

# pthread
macro(add_pthread)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/pthread/include)

    set(pthread_srcs

        pthread.c
        pthread_cond_var.c
        pthread_local_storage.c
    )

    nf_find_esp32_files_at_location("${pthread_srcs}" "${esp32_idf_SOURCE_DIR}/components/pthread")

    # TODO check option
    if(CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=vPortCleanUpTCB")
    endif()


endmacro()

# soc
macro(add_soc)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/soc/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/soc/${TARGET_SERIES_SHORT}/include)

    set(soc_srcs

        lldesc.c
        soc_include_legacy_warn.c
        memory_layout_utils.c
    )

    nf_find_esp32_files_at_location("${soc_srcs}" "${esp32_idf_SOURCE_DIR}/components/soc")

endmacro()

# spi_flash
macro(add_spi_flash)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/spi_flash/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/spi_flash/private_include)

    set(spi_flash_srcs

        cache_utils.c
        flash_mmap.c
        flash_ops.c
        flash_ops_${TARGET_SERIES_SHORT}.c
        partition.c
        spi_flash_rom_patch.c
        spi_flash_chip_drivers.c
        spi_flash_chip_generic.c
        spi_flash_chip_issi.c
        spi_flash_chip_mxic.c
        spi_flash_chip_gd.c
        spi_flash_chip_winbond.c
        spi_flash_chip_boya.c
        memspi_host_driver.c
        esp_flash_api.c
        esp_flash_spi_init.c
        spi_flash_os_func_app.c
        spi_flash_os_func_noos.c
    )

    set(spi_flash_locations

        ${esp32_idf_SOURCE_DIR}/components/spi_flash
        ${esp32_idf_SOURCE_DIR}/components/spi_flash/${TARGET_SERIES_SHORT}
    )
    
    nf_find_esp32_files_at_location("${spi_flash_srcs}" "${spi_flash_locations}")

endmacro()

# vfs
macro(add_vfs)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/vfs/include)

    set(vfs_srcs

        vfs.c
        vfs_uart.c
        vfs_semihost.c
    )

    nf_find_esp32_files_at_location("${vfs_srcs}" "${esp32_idf_SOURCE_DIR}/components/vfs")

    # TODO check option
    if(CONFIG_ESP_CONSOLE_USB_CDC)
        target_sources(${COMPONENT_LIB} PRIVATE "vfs_cdcacm.c")
    endif()

endmacro()

# xtensa
macro(add_xtensa)

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/xtensa/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/xtensa/${TARGET_SERIES_SHORT}/include)

    set(xtensa_srcs

        debug_helpers.c
        debug_helpers_asm.S
        expression_with_stack_xtensa.c
        expression_with_stack_xtensa_asm.S
        eri.c
        trax.c
        xtensa_intr.c
        xtensa_intr_asm.S
        trax_init.c
    )

    if(${TARGET_SERIES} STREQUAL "ESP32_S2")
        list(APPEND xtensa_srcs stdatomic.c)
    endif()

    set(xtensa_locations

        ${esp32_idf_SOURCE_DIR}/components/xtensa
        ${esp32_idf_SOURCE_DIR}/components/xtensa/${TARGET_SERIES_SHORT}
    )

    nf_find_esp32_files_at_location("${xtensa_srcs}" "${xtensa_locations}")

endmacro()
