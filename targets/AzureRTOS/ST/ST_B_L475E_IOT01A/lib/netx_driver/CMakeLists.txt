# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(SOURCES
    inventek/es_wifi.c
    inventek/es_wifi_io.c
    inventek/wifi.c

    nx_wifi.c
)

SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/inventek/es_wifi_io.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/inventek/wifi_io.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/inventek/wifi.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/nx_wifi.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

set(PROJECT netx_driver)

add_library(${PROJECT} OBJECT
    ${SOURCES}
)

target_link_libraries(${PROJECT}
    PUBLIC
        azrtos::threadx
        azrtos::netxduo
        #stm32cubel4
)

target_include_directories(${PROJECT}
    PUBLIC
        inventek
        ..\..
)
