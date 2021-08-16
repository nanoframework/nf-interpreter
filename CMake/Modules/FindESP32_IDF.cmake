#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(esp32_idf)

include(binutils.ESP32)

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${TARGET_SERIES_SHORT})
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${TARGET_SERIES_SHORT}/include)

# including here the CMake files for the source files specific to the target series
include(${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(${TARGET_SERIES}_GCC_options)

############
# freertos #
############

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/include)

if(${TARGET_SERIES} STREQUAL "ESP32_C3")

    # including RISC-V specific files

    set(FREERTOS_SRCS

        # port specific
        port.c
        portasm.S
    
        # commmon
        port_common.c
        croutine.c
        event_groups.c
        list.c
        queue.c
        tasks.c
        timers.c
        stream_buffer.c
        FreeRTOS-openocd.c
        freertos_v8_compat.c
    )

    foreach(SRC_FILE ${FREERTOS_SRCS})

        set(FREERTOS_SRC_FILE SRC_FILE -NOTFOUND)

        find_file(FREERTOS_SRC_FILE ${SRC_FILE}
            PATHS 

            ${esp32_idf_SOURCE_DIR}/components/freertos
            ${esp32_idf_SOURCE_DIR}/components/freertos/port
            ${esp32_idf_SOURCE_DIR}/components/freertos/port/riscv

            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${FREERTOS_SRC_FILE}")
        endif()

        list(APPEND ESP32_IDF_SOURCES ${FREERTOS_SRC_FILE})

    endforeach()

    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/port/riscv/include/freertos)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/port/riscv)

else()

    # including Xtensa specific files
    set(FREERTOS_SRCS

        # port specific
        port.c
        portasm.S
        xtensa_context.S
        xtensa_init.c
        xtensa_overlay_os_hook.c
        xtensa_vector_defaults.S
        xtensa_vectors.S
    
        # commmon
        port_common.c
        croutine.c
        event_groups.c
        list.c
        queue.c
        tasks.c
        timers.c
        stream_buffer.c
        FreeRTOS-openocd.c
        freertos_v8_compat.c

    )

    foreach(SRC_FILE ${FREERTOS_SRCS})

        set(FREERTOS_SRC_FILE SRC_FILE -NOTFOUND)

        find_file(FREERTOS_SRC_FILE ${SRC_FILE}
            PATHS

                ${esp32_idf_SOURCE_DIR}/components/freertos
                ${esp32_idf_SOURCE_DIR}/components/freertos/port
                ${esp32_idf_SOURCE_DIR}/components/freertos/port/xtensa


            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${FREERTOS_SRC_FILE}")
        endif()

        list(APPEND ESP32_IDF_SOURCES ${FREERTOS_SRC_FILE})

    endforeach()
    
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/port/xtensa/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/port/xtensa)

endif()

# TODO check option
if(CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)
    list(APPEND srcs "port/xtensa/xtensa_loadstore_handler.S")
endif()



include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(ESP32_IDF DEFAULT_MSG ESP32_IDF_INCLUDE_DIRS ESP32_IDF_SOURCES)
