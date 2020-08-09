#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# extract LwIP source files
execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xf ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip-2.1.2.7z
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/
)

# List of the required lwIp include files.
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/Lwip)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various/lwip_bindings)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/include)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/include/lwip)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/include/netif)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/include/posix)
list(APPEND CHIBIOS_LWIP_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp)

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
	
	# platform implementations
	platform_sys_arch.c

    #extras
    evtimer.c

    # netif
    ethernet.c
    slipif.c

    # 6LoWPAN
    # lowpan6.c

    # PPP
    auth.c
	ccp.c
	chap-md5.c
	chap_ms.c
	chap-new.c
	demand.c
	eap.c
	ecp.c
	eui64.c
	fsm.c
	ipcp.c
	ipv6cp.c
	lcp.c
	magic.c
	mppe.c
	multilink.c
	ppp.c
	pppapi.c
	pppcrypt.c
	pppoe.c
	pppol2tp.c
	pppos.c
	upap.c
	utils.c
    vj.c
    
    # PPP SSL
	arc4.c
	des.c
	md4.c
	md5.c
	sha1.c

    # APPS!

    # SNMPv2c agent
    # snmp_asn1.c
	# snmp_core.c
	# snmp_mib2.c
	# snmp_mib2_icmp.c
	# snmp_mib2_interfaces.c
	# snmp_mib2_ip.c
	# snmp_mib2_snmp.c
	# snmp_mib2_system.c
	# snmp_mib2_tcp.c
	# snmp_mib2_udp.c
	# snmp_msg.c
	# snmpv3.c
	# snmp_netconn.c
	# snmp_pbuf_stream.c
	# snmp_raw.c
	# snmp_scalar.c
	# snmp_table.c
	# snmp_threadsync.c
	# snmp_traps.c
	# snmpv3_mbedtls.c
	# snmpv3_dummy.c

    # http server
    # fs.c
    # httpd.c

    # iperf server
    # lwiperf.c

	# SNTP client
	# this one is added below if NF_NETWORKING_SNTP option is ON
    # sntp.c

    # MDNS responder
    mdns.c

    # NetBIOS server
    # netbiosns.c

    # TFTP server
    # tftp_server.c

    # MQTT client
    # mqtt.c
)

if(NF_NETWORKING_SNTP)
    list(APPEND LWIP_SRCS sntp.c)
endif()

foreach(SRC_FILE ${LWIP_SRCS})
    set(LWIP_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(LWIP_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/core
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/core/ipv4
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/core/ipv6
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/api
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/netif
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/netif/ppp
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/netif/ppp/polarssl

            ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/Lwip
            ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/Lwip

            # APPS:
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/apps/snmp
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/apps/httpd
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/apps/lwiperf
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/apps/sntp
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/apps/mdns
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/apps/netbiosns
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/apps/tftp
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/apps/mqtt

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${LWIP_SRC_FILE}") # debug helper
    list(APPEND CHIBIOS_LWIP_SOURCES ${LWIP_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CHIBIOS_LWIP DEFAULT_MSG CHIBIOS_LWIP_INCLUDE_DIRS CHIBIOS_LWIP_SOURCES)

# setup target to unzip ChibiOS external network components
add_custom_target( CHIBIOS_NETWORK_COMPONENTS ALL )

add_custom_command(TARGET CHIBIOS_NETWORK_COMPONENTS
PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar xf ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip-2.1.2.7z
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/
    DEPENDS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip-2.1.2.7z

    VERBATIM
)

# this depends on ChibiOS target being already downloaded
add_dependencies(CHIBIOS_NETWORK_COMPONENTS ChibiOS)
