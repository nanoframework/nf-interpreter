#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(lwip)
FetchContent_GetProperties(chibios)

# List of the required lwIp include files.
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_Lwip)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/various)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/various/lwip_bindings)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${lwip_SOURCE_DIR}/src/include)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${lwip_SOURCE_DIR}/src/include/lwip)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${lwip_SOURCE_DIR}/src/include/netif)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp)

set(LWIP_SRCS

    # sys_arch.c

    # core
    init.c
	def.c
	dns.c
	inet_chksum.c
	ip.c
	mem.c
	memp.c
	netif.c
	pbuf.c
	raw.c
	stats.c
	sys.c
	tcp.c
	tcp_in.c
	tcp_out.c
	timeouts.c
    udp.c
    
    # ipv4
    autoip.c
	dhcp.c
	etharp.c
	icmp.c
	igmp.c
	ip4_frag.c
	ip4.c
	ip4_addr.c

    # ip6
    dhcp6.c
	ethip6.c
	icmp6.c
	inet6.c
	ip6.c
	ip6_addr.c
	ip6_frag.c
	mld6.c
	nd6.c

    # api
    api_lib.c
	# api_msg.c
	err.c
	netbuf.c
	netdb.c
	netifapi.c
	# sockets.c
	tcpip.c

    # api patched files for nanoframework
    nf_api_msg.c
    nf_sockets.c
	nf_sys_arch.c
    
	# bindings
	nf_lwipthread.c

    #extras
    evtimer.c

    # netif
    ethernet.c
    slipif.c
)

if(NF_NETWORKING_SNTP)
    list(APPEND LWIP_SRCS sntp.c)
endif()

foreach(SRC_FILE ${LWIP_SRCS})

    set(LWIP_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(LWIP_SRC_FILE ${SRC_FILE}
        PATHS 
            ${chibios_SOURCE_DIR}/os/various
            ${lwip_SOURCE_DIR}/src/core
            ${lwip_SOURCE_DIR}/src/core/ipv4
            ${lwip_SOURCE_DIR}/src/core/ipv6
            ${lwip_SOURCE_DIR}/src/api
            ${lwip_SOURCE_DIR}/src/netif

            ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_Lwip

            # APPS:
            ${lwip_SOURCE_DIR}/src/apps/sntp

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${LWIP_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_LWIP_SOURCES ${LWIP_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CHIBIOS_LWIP DEFAULT_MSG CHIBIOS_LWIP_INCLUDE_DIRS CHIBIOS_LWIP_SOURCES)
