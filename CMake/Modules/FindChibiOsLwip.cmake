
execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip-1.4.1_patched.7z
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/
    )


# List of the required lwIp include files.
list(APPEND LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various)
list(APPEND LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various/lwip_bindings)
list(APPEND LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/include)
list(APPEND LWIP_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/include/ipv4)


set(LWIP_SRCS
# bindings
lwipthread.c
sys_arch.c

# netif
etharp.c

# core
dhcp.c
dns.c
init.c
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
udp.c

# ipv4
autoip.c
icmp.c
igmp.c
inet.c
inet_chksum.c
ip.c
ip_addr.c
ip_frag.c
def.c
timers.c

# api
api_lib.c
api_msg.c
err.c
netbuf.c
netdb.c
netifapi.c
sockets.c
tcpip.c
)


foreach(SRC_FILE ${LWIP_SRCS})
set(LWIP_SRC_FILE SRC_FILE -NOTFOUND)
find_file(LWIP_SRC_FILE ${SRC_FILE}
    PATHS 
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various/lwip_bindings
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various/lwip_bindings/arch
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/netif
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/core
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/core/ipv4
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip/src/api

    CMAKE_FIND_ROOT_PATH_BOTH
)
# message("${SRC_FILE} >> ${LWIP_SRC_FILE}") # debug helper
list(APPEND LWIP_SOURCES ${LWIP_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Debugger DEFAULT_MSG LWIP_INCLUDE_DIRS LWIP_SOURCES)