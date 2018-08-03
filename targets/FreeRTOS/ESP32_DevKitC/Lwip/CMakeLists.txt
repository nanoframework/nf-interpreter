#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# We build Lwip library from sources so as to include patches required for nanoFramework


# Add current source path for lwipopts.h and sdkconfig.h
list(APPEND TARGET_ESP32_IDF_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")


list(APPEND COMPONENT_PATH "${ESP32_IDF_PATH}/components")

# Lwip Library
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/api/api_lib.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nf_api_msg.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/api/err.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/api/netbuf.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/api/netdb.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/api/netifapi.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/api/pppapi.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nf_sockets.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/api/tcpip.c" )

file( GLOB TARGET_LWIP_APPS1_SOURCES "${COMPONENT_PATH}/lwip/apps/*.c" )
file( GLOB TARGET_LWIP_APPS2_SOURCES "${COMPONENT_PATH}/lwip/apps/sntp/*.c" )
file( GLOB TARGET_LWIP_APPS3_SOURCES "${COMPONENT_PATH}/lwip/apps/ping/*.c" )
file( GLOB TARGET_LWIP_CORE_SOURCES "${COMPONENT_PATH}/lwip/core/*.c" )
file( GLOB TARGET_LWIP_CORE_IP4_SOURCES "${COMPONENT_PATH}/lwip/core/ipv4/*.c" )
file( GLOB TARGET_LWIP_CORE_IP6_SOURCES "${COMPONENT_PATH}/lwip/core/ipv6/*.c" )
file( GLOB TARGET_LWIP_NETIF_SOURCES "${COMPONENT_PATH}/lwip/netif/*.c" )

list(APPEND TARGET_LWIP_PORT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nf_sys_arch.c" )
list(APPEND TARGET_LWIP_PORT_SOURCES "${COMPONENT_PATH}/lwip/port/netif/ethernetif.c" )
list(APPEND TARGET_LWIP_PORT_SOURCES "${COMPONENT_PATH}/lwip/port/netif/wlanif.c" )
list(APPEND TARGET_LWIP_PORT_SOURCES "${COMPONENT_PATH}/lwip/port/debug/lwip_debug.c" )
list(APPEND TARGET_LWIP_PORT_SOURCES "${COMPONENT_PATH}/lwip/port/vfs_lwip.c" )

list(APPEND TARGET_LWIP_SOURCES ${TARGET_LWIP_API_SOURCES}  ${TARGET_LWIP_APPS1_SOURCES}  ${TARGET_LWIP_APPS2_SOURCES}  ${TARGET_LWIP_APPS3_SOURCES}  ${TARGET_LWIP_CORE_SOURCES} ${TARGET_LWIP_CORE_IP4_SOURCES} ${TARGET_LWIP_CORE_IP6_SOURCES} ${TARGET_LWIP_NETIF_SOURCES}  ${TARGET_LWIP_PORT_SOURCES} )
#list( APPEND CMAKE_C_FLAGS " -Wno-address -Wno-unused-variable -Wno-unused-but-set-variable " )
#add_library( lwip STATIC ${TARGET_LWIP_SOURCES}  )
#target_include_directories( lwip PUBLIC  ${TARGET_ESP32_IDF_INCLUDES} )

# make var global
set(TARGET_LWIP_SOURCES ${TARGET_LWIP_SOURCES} CACHE INTERNAL "make global")
set(TARGET_ESP32_IDF_INCLUDES ${TARGET_ESP32_IDF_INCLUDES} CACHE INTERNAL "make global")
