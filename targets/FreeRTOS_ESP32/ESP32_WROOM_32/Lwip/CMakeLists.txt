#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# We build Lwip library from sources so as to include patches required for nanoFramework

list(APPEND COMPONENT_PATH "${ESP32_IDF_PATH}/components")

# Add current source path for lwipopts.h and sdkconfig.h
list(APPEND TARGET_ESP32_IDF_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/lwip/lwip/src/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/lwip/port/esp32/include")



# Lwip Library
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/api/api_lib.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nf_api_msg.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/api/err.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/api/netbuf.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/api/netdb.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/api/netifapi.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/netif/ppp/pppapi.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nf_sockets.c" )
list( APPEND TARGET_LWIP_API_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/api/tcpip.c" )

file( GLOB TARGET_LWIP_APPS1_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/apps/*.c" )
file( GLOB TARGET_LWIP_APPS2_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/apps/sntp/*.c" )
file( GLOB TARGET_LWIP_APPS3_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/apps/ping/*.c" )
file( GLOB TARGET_LWIP_APPS4_SOURCES "${COMPONENT_PATH}/lwip/apps/dhcpserver/dhcpserver.c" )
file( GLOB TARGET_LWIP_CORE_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/core/*.c" )
file( GLOB TARGET_LWIP_CORE_IP4_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/core/ipv4/*.c" )
file( GLOB TARGET_LWIP_CORE_IP6_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/core/ipv6/*.c" )
file( GLOB TARGET_LWIP_NETIF_SOURCES "${COMPONENT_PATH}/lwip/lwip/src/netif/*.c" )

list(APPEND TARGET_LWIP_PORT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nf_sys_arch.c" )
list(APPEND TARGET_LWIP_PORT_SOURCES "${COMPONENT_PATH}/lwip/port/esp32/netif/ethernetif.c" )
list(APPEND TARGET_LWIP_PORT_SOURCES "${COMPONENT_PATH}/lwip/port/esp32/netif/wlanif.c" )
list(APPEND TARGET_LWIP_PORT_SOURCES "${COMPONENT_PATH}/lwip/port/esp32/debug/lwip_debug.c" )
list(APPEND TARGET_LWIP_PORT_SOURCES "${COMPONENT_PATH}/lwip/port/esp32/vfs_lwip.c" )

list(APPEND TARGET_LWIP_SOURCES ${TARGET_LWIP_API_SOURCES}  ${TARGET_LWIP_APPS1_SOURCES}  ${TARGET_LWIP_APPS2_SOURCES}  ${TARGET_LWIP_APPS3_SOURCES} ${TARGET_LWIP_APPS4_SOURCES}  ${TARGET_LWIP_CORE_SOURCES} ${TARGET_LWIP_CORE_IP4_SOURCES} ${TARGET_LWIP_CORE_IP6_SOURCES} ${TARGET_LWIP_NETIF_SOURCES}  ${TARGET_LWIP_PORT_SOURCES} )
#list( APPEND CMAKE_C_FLAGS " -Wno-address -Wno-unused-variable -Wno-unused-but-set-variable " )
#add_library( lwip STATIC ${TARGET_LWIP_SOURCES}  )
#target_include_directories( lwip PUBLIC  ${TARGET_ESP32_IDF_INCLUDES} )

# make var global
set(TARGET_LWIP_SOURCES ${TARGET_LWIP_SOURCES} CACHE INTERNAL "make global")
set(TARGET_ESP32_IDF_INCLUDES ${TARGET_ESP32_IDF_INCLUDES} CACHE INTERNAL "make global")
