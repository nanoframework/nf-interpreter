# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(SOURCES
    inventek/es_wifi.cpp
    inventek/es_wifi_io.cpp
    inventek/wifi.cpp

    nx_wifi.cpp
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
        ../..
        ${CMAKE_SOURCE_DIR}/src/PAL/Include
        ${CMAKE_SOURCE_DIR}/src/HAL/Include
        ${CMAKE_SOURCE_DIR}/src/CLR/Include
        ${TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}/targets/${RTOS}
        ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}
        ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR
        ${BASE_PATH_FOR_PLATFORM}/_common/include
        ${BASE_PATH_FOR_PLATFORM}/ST/_include
)
