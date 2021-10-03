# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(TARGET app_common)

set(SOURCES
    azure_iot_mqtt/azure_iot_mqtt.c
    azure_iot_mqtt/azure_iot_dps_mqtt.c
    azure_iot_mqtt/hmac_sha256.c
    azure_iot_mqtt/sas_token.c
    azure_iot_mqtt/sha256.c

    azure_iot_nx/azure_iot_nx_client.c
    azure_iot_nx/nx_azure_iot_pnp_helpers.c

    azure_iot_cert.c
    azure_iot_ciphersuites.c
    json_utils.c
    sntp_client.c
)

# Allow to disable the common networking component
if(NOT DEFINED DISABLE_COMMON_NETWORK) 
    list(APPEND SOURCES
        networking.c
    )
endif()

# Allow to disable the newlib stubbing
if(NOT DEFINED DISABLE_NEWLIB_STUB) 
    list(APPEND SOURCES
        newlib_nano.c
    )
    SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/newlib_nano.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
endif()

SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/sntp_client.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/azure_iot_mqtt/azure_iot_dps_mqtt.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/azure_iot_mqtt/azure_iot_mqtt.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/azure_iot_mqtt/azure_iot_nx_client.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_LIST_DIR}/azure_iot_nx/azure_iot_nx_client.c" PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

add_library(${TARGET} OBJECT
    ${SOURCES}
)

target_include_directories(${TARGET}
    PUBLIC
        .
        azure_iot_mqtt
        azure_iot_nx
)

target_link_libraries(${TARGET}
    azrtos::threadx
    azrtos::netxduo
    jsmn
)