#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)

############################################
# network layer from Azure RTOS and NetX Duo
if(RTOS_AZURERTOS_CHECK)

    FetchContent_GetProperties(azure_rtos_netxduo)

    # set include directories for nanoFramework network
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/_common/NetX)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/_common/include)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/AzureRTOS/${TARGET_BOARD})
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/AzureRTOS)
    
    # source files for nanoFramework Networking
    set(NF_Network_SRCS

        # platform specific
        target_network.cpp
        targetHAL_network.cpp
    )

    if(AZURERTOS_NETXDUO_REQUIRED)
        #NetX Duo
        list(APPEND NF_Network_SRCS sockets_netx.cpp)
        list(APPEND NF_Network_SRCS nf_netxduo.c)
        list(APPEND NF_Network_SRCS netx_sockets.cpp)
        list(APPEND NF_Network_SRCS netx_sockets_functions.cpp )
        list(APPEND NF_Network_SRCS netx_thread.c)

        # list(APPEND NF_Network_SRCS nxd_dns.c)
        # list(APPEND NF_Network_SRCS nxd_sntp_client.c)

    endif()

    if("${ETHERNET_DRIVER}" STREQUAL "LAN8742")

        list(APPEND NF_Network_SRCS ssl_stubs.cpp)

        # add driver files to network sources
        list(APPEND
            NF_Network_SRCS
                # nx_stm32_eth_driver.c
                # nx_stm32_phy_driver.c
                nx_chibios_eth_driver.c
                nx_chibios_phy_driver.c
                # lan8742.c

                # mx_eth_init.c
                nx_driver_stm32l4.c

                # stm32f7xx_hal_eth_extra.c
                # stm32${TARGET_SERIES_SHORT_LOWER}xx_hal_msp_eth.c
        )

        # need this name in lower case
        string(TOLOWER "${ETHERNET_DRIVER}" ETHERNET_DRIVER_1)

        # add includes too
        list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo/drivers/ethernet)
        # list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo/drivers/ethernet/lan8742)
        list(APPEND NF_Network_INCLUDE_DIRS ${CHIBIOS_HAL_INCLUDE_DIRS})

        SET(NETWORK_DRIVER_LOCATION ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo/drivers/chibios)
        SET(NX_THREAD_LOCATION ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo)

        # add exception to compiler warnings as errors
        SET_SOURCE_FILES_PROPERTIES( ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo/drivers/ethernet/lan8742/lan8742.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
        SET_SOURCE_FILES_PROPERTIES( ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo/drivers/ethernet/nx_stm32_eth_driver.c PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter -Wno-shadow")
        SET_SOURCE_FILES_PROPERTIES( ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo/drivers/ethernet/lan8742/nx_stm32_phy_driver.c PROPERTIES COMPILE_FLAGS -Wno-int-conversion)

        FetchContent_GetProperties(azure_rtos)

        # need to replace some declarations and functions in STM32F4 HAL Cube package because they have a newer implementation 
        # in Azure RTOS package
        if("${TARGET_SERIES}" STREQUAL "STM32F7xx")

            FetchContent_GetProperties(stm32f7_hal_driver)

            set(ETH_INCLUDE_FILE ${stm32f7_hal_driver_SOURCE_DIR}/Inc/stm32f7xx_hal_eth.h)
            set(ETH_SOURCE_FILE ${stm32f7_hal_driver_SOURCE_DIR}/Src/stm32f7xx_hal_eth.c)
        else()
            message(FATAL_ERROR "Network support for NetX Duo is not implemented for ${TARGET_SERIES}.")
        endif()

        # need to read the supplied files and rename the call
        file(READ
            "${ETH_INCLUDE_FILE}"
            ETH_INCLUDE_FILE_CONTENTS)
        file(READ
            "${ETH_SOURCE_FILE}"
            ETH_SOURCE_FILE_CONTENTS)

        string(REPLACE
            "HAL_ETH_WritePHYRegister"
            "HAL_ETH_WritePHYRegister_DUMMY_FOR_NANO_BUILD"
            ETH_INCLUDE_FILE_FINAL_CONTENTS
            "${ETH_INCLUDE_FILE_CONTENTS}")

        string(REPLACE
            "HAL_ETH_ReadPHYRegister"
            "HAL_ETH_ReadPHYRegister_DUMMY_FOR_NANO_BUILD"
            ETH_INCLUDE_FILE_FINAL_CONTENTS_2
            "${ETH_INCLUDE_FILE_FINAL_CONTENTS}")

        string(REPLACE
            "HAL_ETH_WritePHYRegister"
            "HAL_ETH_WritePHYRegister_DUMMY_FOR_NANO_BUILD"
            ETH_SOURCE_FILE_FINAL_CONTENTS
            "${ETH_SOURCE_FILE_CONTENTS}")

        string(REPLACE
            "HAL_ETH_ReadPHYRegister"
            "HAL_ETH_ReadPHYRegister_DUMMY_FOR_NANO_BUILD"
            ETH_SOURCE_FILE_FINAL_CONTENTS_2
            "${ETH_SOURCE_FILE_FINAL_CONTENTS}")

        file(WRITE 
            ${ETH_INCLUDE_FILE} 
            "${ETH_INCLUDE_FILE_FINAL_CONTENTS_2}")

        file(WRITE 
            ${ETH_SOURCE_FILE} 
            "${ETH_SOURCE_FILE_FINAL_CONTENTS_2}")

    elseif("${WIFI_DRIVER}" STREQUAL "ISM43362")

        # add driver files to network sources
        list(APPEND
            NF_Network_SRCS
  
            ssl_stubs.cpp

            nf_wireless.cpp

            nx_driver_stm32l4.c
            # sockets_ISM43362.cpp
            # ISM43362_sockets.cpp
            # ISM43362_sockets_functions.cpp

            # ssl.cpp
            # ssl_ISM43362.cpp

            es_wifi.c
            wifi.c
        )

        if(${CHIBIOS_HAL_REQUIRED})
            # driver specific to ChibiOS HAL
            list(APPEND
                NF_Network_SRCS
                es_wifi_io_chibios.c
            )
        else()
            # drivers specific to STM HAL
            list(APPEND
                NF_Network_SRCS
                es_wifi_io_stm.c
            )
        endif()

        # need this name in lower case
        string(TOLOWER "${WIFI_DRIVER}" WIFI_DRIVER_1)

        SET(NETWORK_DRIVER_LOCATION ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/drivers/wifi/inventek)

        # add includes too
        list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/drivers/wifi/inventek)
        list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_nanoCLR/System.Device.Spi)
        list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_nanoCLR/Windows.Devices.Spi)
        list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/System.Device.Spi)

    endif()

    # add source files 
    foreach(SRC_FILE ${NF_Network_SRCS})

        set(NF_Network_SRC_FILE SRC_FILE-NOTFOUND)

        find_file(NF_Network_SRC_FILE ${SRC_FILE}
            PATHS

                # ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets
                ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
                # ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS
                
                ${NX_THREAD_LOCATION}
                ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/_common
                ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/_common/NetX

                ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/CubeMX
                ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo

                # drivers
                ${NETWORK_DRIVER_LOCATION}
                ${NETWORK_DRIVER_LOCATION}/..

                # ST LAN8742
                ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo/drivers/ethernet
                ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo/drivers/ethernet/${ETHERNET_DRIVER_1}
                ${TARGET_BASE_LOCATION}/common/CubeMX

                # ISM43362
                # ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/netxduo/drivers/wifi/inventek

                # ${azure_rtos_netxduo_SOURCE_DIR}/addons/dns
                # ${azure_rtos_netxduo_SOURCE_DIR}/addons/sntp

                ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}
    
            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${NF_Network_SRC_FILE}")
        endif()

        list(APPEND NF_Network_SOURCES ${NF_Network_SRC_FILE})

    endforeach()

    # # source files for security layer
    # set(NF_Network_Security_SRCS

    #     ssl.cpp
    #     ssl_accept_internal.cpp
    #     ssl_add_cert_auth_internal.cpp
    #     ssl_close_socket_internal.cpp
    #     ssl_connect_internal.cpp
    #     ssl_decode_private_key_internal.cpp
    #     ssl_exit_context_internal.cpp
    #     ssl_generic.cpp
    #     ssl_generic_init_internal.cpp
    #     ssl_initialize_internal.cpp
    #     ssl_parse_certificate_internal.cpp
    #     ssl_available_internal.cpp
    #     ssl_read_internal.cpp
    #     ssl_uninitialize_internal.cpp
    #     ssl_write_internal.cpp
    # )

    # if(NF_FEATURE_DEBUGGER)
    #     list(APPEND NF_Network_SRCS Sockets_debugger.cpp)
    # endif()

    # if(USE_SECURITY_MBEDTLS_OPTION)

    #     foreach(SRC_FILE ${NF_Network_Security_SRCS})

    #         set(NF_Network_SRC_FILE SRC_FILE-NOTFOUND)

    #         find_file(NF_Network_SRC_FILE ${SRC_FILE}
    #             PATHS 

    #                 ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
    #                 ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS
        
    #             CMAKE_FIND_ROOT_PATH_BOTH
    #         )

    #         if (BUILD_VERBOSE)
    #             message("${SRC_FILE} >> ${NF_Network_SRC_FILE}")
    #         endif()
        
    #         list(APPEND NF_Network_SOURCES ${NF_Network_SRC_FILE})

    #     endforeach()

    # endif()

    # need to replace this define here which is conflicting with another from STM32 HAL
    set(BSD_INCLUDE_FILE ${azure_rtos_netxduo_SOURCE_DIR}/addons/BSD/nxd_bsd.h)

    # need to read the supplied files and rename the call
    file(READ
        "${BSD_INCLUDE_FILE}"
        BSD_INCLUDE_FILE_CONTENTS)

    string(REPLACE
        "#define ERROR"
        "#define NX_ERROR"
        BSD_INCLUDE_FILE_FINAL_CONTENTS
        "${BSD_INCLUDE_FILE_CONTENTS}")

    file(WRITE 
        ${BSD_INCLUDE_FILE} 
        "${BSD_INCLUDE_FILE_FINAL_CONTENTS}")

    # this requires also replacing it in nxd_bsd.c
    set(BSD_SOURCE_FILE ${azure_rtos_netxduo_SOURCE_DIR}/addons/BSD/nxd_bsd.c)

    file(READ
        "${BSD_SOURCE_FILE}"
        BSD_SOURCE_FILE_CONTENTS)

    string(REPLACE
        "(ERROR)"
        "(NX_ERROR)"
        BSD_SOURCE_FILE_FINAL_CONTENTS
        "${BSD_SOURCE_FILE_CONTENTS}")

    file(WRITE
        ${BSD_SOURCE_FILE}
        "${BSD_SOURCE_FILE_FINAL_CONTENTS}")

#########################
# network layer from lwIP
else()

    FetchContent_GetProperties(mbedtls)
    FetchContent_GetProperties(esp32_idf)

    # set include directories for nanoFramework network
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Lwip)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp)

    if(USE_SECURITY_MBEDTLS_OPTION)
        list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS)
        list(APPEND NF_Network_INCLUDE_DIRS ${mbedtls_SOURCE_DIR}/include)
    endif()

    if(RTOS_ESP32_CHECK)
        list(APPEND NF_Network_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/include)
        list(APPEND NF_Network_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/include/mbedtls)
        list(APPEND NF_Network_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/mbedtls/mbedtls/include)
    endif()

    if(USE_ENC28J60_DRIVER_OPTION)
        list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Network/Enc28j60)
        list(APPEND NF_Network_Driver_Path ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Network/Enc28j60)
        set(Use_Networking_Extra_Driver TRUE)
    endif()

    # source files for nanoFramework Networking
    set(NF_Network_SRCS

        #pal Socket
        sockets_lwip.cpp

        #Lwip 
        lwIP_Sockets.cpp
        lwIP_Sockets_functions.cpp 

        # platform specific
        Target_Network.cpp
        targetHAL_Network.cpp
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
                ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS
                ${CMAKE_SOURCE_DIR}/src/PAL/Lwip
                ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_common
                ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_Network

                ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}
    
            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${NF_Network_SRC_FILE}")
        endif()

        list(APPEND NF_Network_SOURCES ${NF_Network_SRC_FILE})

    endforeach()

    if(USE_SECURITY_MBEDTLS_OPTION)

        set(NF_Security_Search_Path "${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS")

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
        
            list(APPEND NF_Network_SOURCES ${NF_Network_SRC_FILE})

        endforeach()

        # unset this warning as error, which is required for these source files
        # OK to remove after this issue is fixed upstream https://github.com/Mbed-TLS/mbedtls/issues/9425
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_accept_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_add_cert_auth_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_close_socket_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_connect_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_decode_private_key_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_exit_context_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_generic.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_generic_init_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_initialize_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_parse_certificate_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_available_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_read_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_uninitialize_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)
        set_source_files_properties(${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTLS/ssl_write_internal.cpp PROPERTIES COMPILE_FLAGS -Wno-undef)

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
        
            list(APPEND NF_Network_SOURCES ${NF_Network_SRC_FILE})
            
        endforeach()

    endif()

endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Network DEFAULT_MSG NF_Network_INCLUDE_DIRS NF_Network_SOURCES)

# macro to be called from binutils to add network library
# BUILD_TARGET parameter to set the target it's building
# optional EXTRA_SOURCES with source files to be added to the library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
# optional EXTRA_COMPILE_OPTIONS with compile options to be added to the library
macro(nf_add_lib_network)

    # parse arguments
    cmake_parse_arguments(NFALN "" "BUILD_TARGET;EXTRA_COMPILE_OPTIONS" "EXTRA_SOURCES;EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})

    if(NOT NFALN_BUILD_TARGET OR "${NFALN_BUILD_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set BUILD_TARGET argument when calling nf_add_lib_network()")
    endif()

    # add this has a library
    set(LIB_NAME NF_Network)

    add_library(
        ${LIB_NAME} STATIC 
            ${NF_Network_SOURCES}
            ${NFALN_EXTRA_SOURCES})

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            ${NF_Network_INCLUDE_DIRS}
            ${NF_CoreCLR_INCLUDE_DIRS}
            ${NFALN_EXTRA_INCLUDES})

    # TODO can be removed later
    if(RTOS_ESP32_CHECK)
        # this is the only one different
        target_compile_definitions(
            ${LIB_NAME} PUBLIC
            -DPLATFORM_ESP32
            ${NFALN_EXTRA_COMPILE_DEFINITIONS}
        )

    else()
        nf_set_compile_options(TARGET ${LIB_NAME} EXTRA_COMPILE_OPTIONS ${NFALN_EXTRA_COMPILE_OPTIONS})
        nf_set_compile_definitions(TARGET ${LIB_NAME} EXTRA_COMPILE_DEFINITIONS ${NFALN_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NFALN_BUILD_TARGET})
        nf_set_link_options(TARGET ${LIB_NAME})
    endif()

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
