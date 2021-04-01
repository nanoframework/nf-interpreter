#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(lwip)

# List of the required lwIp include files.
list(APPEND LWIP_INCLUDE_DIRS ${lwip_SOURCE_DIR}/src/include/)
list(APPEND LWIP_INCLUDE_DIRS ${lwip_SOURCE_DIR}/src/include/lwip)
list(APPEND LWIP_INCLUDE_DIRS ${lwip_SOURCE_DIR}/src/include/netif)
list(APPEND LWIP_INCLUDE_DIRS ${lwip_SOURCE_DIR}/src/include/compat)
list(APPEND LWIP_INCLUDE_DIRS ${lwip_SOURCE_DIR}/src/include/compat/posix)
list(APPEND LWIP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp)
list(APPEND LWIP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/_LwIP)

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
    # evtimer.c

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
	# this one is added bellow if NF_NETWORKING_SNTP option is ON
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
            # ${chibios_SOURCE_DIR}/os/various
            ${lwip_SOURCE_DIR}/src/core
            ${lwip_SOURCE_DIR}/src/core/ipv4
            ${lwip_SOURCE_DIR}/src/core/ipv6
            ${lwip_SOURCE_DIR}/src/api
            ${lwip_SOURCE_DIR}/src/netif
            ${lwip_SOURCE_DIR}/src/netif/ppp
            ${lwip_SOURCE_DIR}/src/netif/ppp/polarssl

			# ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_Lwip
			# TODO: this needs to be changed so it's not platform dependent
			${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/_LwIP

            # APPS:
            ${lwip_SOURCE_DIR}/src/apps/snmp
            ${lwip_SOURCE_DIR}/src/apps/http
            ${lwip_SOURCE_DIR}/src/apps/lwiperf
            ${lwip_SOURCE_DIR}/src/apps/sntp
            ${lwip_SOURCE_DIR}/src/apps/mdns
            ${lwip_SOURCE_DIR}/src/apps/netbiosns
            ${lwip_SOURCE_DIR}/src/apps/tftp
            ${lwip_SOURCE_DIR}/src/apps/mqtt

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
		message("${SRC_FILE} >> ${LWIP_SRC_FILE}")
    endif()

    list(APPEND LWIP_SOURCES ${LWIP_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LWIP DEFAULT_MSG LWIP_INCLUDE_DIRS LWIP_SOURCES)

