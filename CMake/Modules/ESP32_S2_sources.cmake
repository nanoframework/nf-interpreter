#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# include(ESP32_IDF_components)

# include(FetchContent)
# FetchContent_GetProperties(esp32_idf)

# set(ESP32_S2_SRCS

#     # esp32s2
#     cache_err_int.c
#     clk.c
#     crosscore_int.c
#     dport_access.c
#     esp_crypto_lock.c
#     esp_ds.c
#     esp_hmac.c
#     hw_random.c
#     memprot.c
#     spiram_psram.c
#     spiram.c
#     system_api_esp32s2.c
# )

# foreach(SRC_FILE ${ESP32_S2_SRCS})

#     set(ESP32_S2_SRC_FILE SRC_FILE-NOTFOUND)

#     find_file(ESP32_S2_SRC_FILE ${SRC_FILE}
#         PATHS 
        
#             ${esp32_idf_SOURCE_DIR}/components/esp32s2


#         CMAKE_FIND_ROOT_PATH_BOTH
#     )

#     if (BUILD_VERBOSE)
#         message("${SRC_FILE} >> ${ESP32_S2_SRC_FILE}")
#     endif()

#     list(APPEND ESP32_IDF_SOURCES ${ESP32_S2_SRC_FILE})

# endforeach()

# # add the components
# add_esp_timer()
# add_esp_common()
# add_espcoredump()
# add_soc()
# add_esp_ipc()
# add_esp_pm()
# add_esp_hw_support()
# add_efuse()
# add_spi_flash()
# add_app_update()
# add_app_trace()
# add_bootloader_support()
# add_hal()
# add_driver()
# add_esp_ringbuf()
# add_esp_system()
# add_log()
# add_vfs()
# add_pthread()
# add_nvs_flash()
# add_xtensa()
# add_esp_rom()
