#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

#
# Custom macro for nordic 9160 support.
# Function taken from v1.x.x version of NCS.
#
function(z_include_directories)
  foreach(arg ${ARGV})
    if(IS_ABSOLUTE ${arg})
      set(path ${arg})
    else()
      set(path ${CMAKE_CURRENT_SOURCE_DIR}/${arg})
    endif()
    target_include_directories(zephyr_interface INTERFACE ${path})
  endforeach()
endfunction()

macro(nordic_add_network)

include(FetchContent)
FetchContent_GetProperties(mbedtls)

# set include directories for nanoFramework network
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Lwip)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp)

if(USE_SECURITY_MBEDTLS_OPTION)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS)
    list(APPEND NF_Network_INCLUDE_DIRS ${mbedtls_SOURCE_DIR}/include)
endif()

if(USE_ENC28J60_DRIVER_OPTION)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Network/Enc28j60)
    list(APPEND NF_Network_Driver_Path ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Network/Enc28j60)
    set(Use_Networking_Extra_Driver TRUE)
endif()

# source files for nanoFramework Networking
set(NF_Network_SRCS "")

set(NF_Network_SRCS

#     #pal Socket
     sockets_lwip.cpp

#     #Lwip 
#     lwIP_Sockets.cpp
#     lwIP_Sockets_functions.cpp 

#     # platform specific
#     Target_Network.cpp
#     targetHAL_Network.cpp
)

# need a conditional include because of ESP32 building network as a library 
if(NOT USE_SECURITY_MBEDTLS_OPTION)
    list(APPEND NF_Network_SRCS ssl_stubs.cpp)
endif()

# source files for security layer
set(NF_Network_Security_SRCS

    ssl.cpp
    ssl_accept_internal.cpp
    ssl_add_cert_auth_internal.cpp
    ssl_close_socket_internal.cpp
    ssl_connect_internal.cpp
    ssl_decode_private_key_internal.cpp
    ssl_exit_context_internal.cpp
    ssl_generic.cpp
    ssl_generic_init_internal.cpp
    ssl_initialize_internal.cpp
    ssl_parse_certificate_internal.cpp
    ssl_available_internal.cpp
    ssl_read_internal.cpp
    ssl_uninitialize_internal.cpp
    ssl_write_internal.cpp
)

if(NF_FEATURE_DEBUGGER)
    list(APPEND NF_Network_SRCS Sockets_debugger.cpp)
endif()

# add Enc28j60source files in two steps
if(USE_ENC28J60_DRIVER_OPTION)

    set(NF_Network_Driver_Srcs
        enc28j60_lwip_config_stubs.cpp
        enc28j60_lwip.cpp
        enc28j60_lwip_driver.cpp
    )

endif()

# add source files in two steps
# 1st pass: common source files
foreach(SRC_FILE ${NF_Network_SRCS})

    set(NF_Network_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(NF_Network_SRC_FILE ${SRC_FILE}
        PATHS

            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets
            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS
            ${CMAKE_SOURCE_DIR}/src/PAL/Lwip
            ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_common
            ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_Network

            ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}
 
        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${NF_Network_SRC_FILE}")
    endif()

    list(APPEND NORDIC_Network_SOURCES ${NF_Network_SRC_FILE})

endforeach()

if(USE_SECURITY_MBEDTLS_OPTION)

    set(NF_Security_Search_Path "${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS")

    # 2nd pass: security files if option is selected 
    foreach(SRC_FILE ${NF_Network_Security_SRCS})

        set(NF_Network_SRC_FILE SRC_FILE-NOTFOUND)

        find_file(NF_Network_SRC_FILE ${SRC_FILE}
            PATHS 

                ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
                ${NF_Security_Search_Path}
    
            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${NF_Network_SRC_FILE}")
        endif()
    
        list(APPEND NORDIC_Network_SOURCES ${NF_Network_SRC_FILE})

    endforeach()

endif()

if(Use_Networking_Extra_Driver)

    # 3rd pass: Any extra driver files 
    foreach(SRC_FILE ${NF_Network_Driver_Srcs})

        set(NF_Network_SRC_FILE SRC_FILE-NOTFOUND)

        find_file(NF_Network_SRC_FILE ${SRC_FILE}
            PATHS 
                ${NF_Network_Driver_Path}
    
            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${NF_Network_Driver_Srcs}")
        endif()
    
        list(APPEND NORDIC_Network_SOURCES ${NF_Network_SRC_FILE})
        
    endforeach()

endif()
endmacro()

#
#
#

# add platform dirs
add_subdirectory($ENV{PLATFORM_NORDIC_ROOT}/_include nordic__include)
add_subdirectory($ENV{PLATFORM_NORDIC_ROOT}/_common nordic__common)
#add_subdirectory(${TARGET_RTOS_ROOT}/_nanoBooter ${TARGET_RTOS_BINDIR}/_nanoBooter)
add_subdirectory($ENV{PLATFORM_NORDIC_ROOT}/_nanoCLR nordic__nanoCLR)


# (binutils.zephyr.cmake) Calls FindPackage for platform specific needs. Nop right now.
nf_add_platform_packages(TARGET ${NANOCLR_PROJECT_NAME})

#######################
#
# Simple build linking sources all together
#
#######################

# zephyr-centric naming of the library
zephyr_library_named(${NANOCLR_PROJECT_NAME}.elf)

nf_common_compiler_definitions(TARGET ${NANOCLR_PROJECT_NAME}.elf BUILD_TARGET ${NANOCLR_PROJECT_NAME})

# (binutils.nordic.cmake) Configures the target_board and target_common include files.
# No sources are added as they are added below
nf_add_platform_sources(${NANOCLR_PROJECT_NAME})

# nordic-centric adding include directories
z_include_directories(${ZEPHYR_CURRENT_LIBRARY} PUBLIC
    ${NF_CoreCLR_INCLUDE_DIRS}
    ${WireProtocol_INCLUDE_DIRS}
    ${NF_NativeAssemblies_INCLUDE_DIRS}
    ${TARGET_NORDIC_NANOCLR_INCLUDE_DIRS}
    ${TARGET_NORDIC_COMMON_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(NF_FEATURE_DEBUGGER)
#zephyr_include_directories(${ZEPHYR_CURRENT_LIBRARY} PUBLIC
z_include_directories(${ZEPHYR_CURRENT_LIBRARY} PUBLIC
    ${NF_Debugger_INCLUDE_DIRS}
)
endif()

if(USE_NETWORKING_OPTION)
    nordic_add_network()
    #zephyr_include_directories(${ZEPHYR_CURRENT_LIBRARY} PUBLIC
    z_include_directories(${ZEPHYR_CURRENT_LIBRARY} PUBLIC
        ${NF_Network_INCLUDE_DIRS}
#        "C:/_dev/ardesco/v1.8.0/modules/lib/matter/third_party/lwip/repo/lwip/src/include"
    )
endif()


# zephyr-centric adding source files
zephyr_library_sources(
    ${NF_CoreCLR_SOURCES} 
    ${WireProtocol_SOURCES}
    ${NF_NativeAssemblies_SOURCES}
    ${TARGET_NORDIC_COMMON_SOURCES}
    ${TARGET_NORDIC_NANOCLR_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/nanoHAL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/target_blockstorage.c
    ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c
    ${COMMON_PROJECT_SOURCES}
    ${NF_HALCore_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/nflib_main.c
    ${TARGET_BASE_LOCATION}/target_common.c
    ${TARGET_BASE_LOCATION}/Device_BlockStorage.c
)
if(NF_FEATURE_DEBUGGER)
    zephyr_library_sources(
        ${NF_Debugger_SOURCES}
        ${NF_Diagnostics_SOURCES}
        )
endif()

if(0)
if(USE_NETWORKING_OPTION)
    zephyr_library_sources(${NF_Network_SOURCES})
endif()
else()
if(USE_NETWORKING_OPTION)
    zephyr_library_sources(
        ${NORDIC_Network_SOURCES}
        ${TARGET_BASE_LOCATION}/../_network/nrf9160_lte.c
        #${TARGET_BASE_LOCATION}/../_network/sockets_Nordic.cpp
        ${TARGET_BASE_LOCATION}/../_network/Nordic_Sockets_functions.cpp
        ${TARGET_BASE_LOCATION}/../_network/targetHAL_ConfigurationManager.cpp

    )
endif()
endif()
