#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.0)
ENABLE_LANGUAGE(ASM)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/target_common.h" @ONLY)

configure_file("${CMAKE_SOURCE_DIR}/CMake/ESP32_target_os.h.in"
               "${CMAKE_BINARY_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/target_os.h" @ONLY)


set(NANOCLR_PROJECT_NAME "nanoCLR")


find_package(WireProtocol REQUIRED)

# packages for nanoFramework libraries
# mandatory 
find_package(NF_CoreCLR REQUIRED)

#######################################
# optional

# nF feature: debugger
if(NF_FEATURE_DEBUGGER)
    find_package(NF_Debugger REQUIRED)
    find_package(NF_Diagnostics REQUIRED)
endif()

# nF feature: Networking Stack, LWIP included in IDF libs
if(USE_NETWORKING_OPTION)
    find_package(NF_NETWORKING REQUIRED)
endif()

# RTC (real time clock) (default is OFF so RTC is NOT included)
option(NF_FEATURE_RTC "option to use hardware RTC")
if(NF_FEATURE_RTC)
    set(HAL_USE_RTC_OPTION TRUE CACHE INTERNAL "NF feature RTC")
else()    
    set(HAL_USE_RTC_OPTION FALSE CACHE INTERNAL "NF feature RTC")
endif()

#######################################

add_subdirectory("common")
add_subdirectory("Lwip")  # lwip must be before IDF
add_subdirectory("IDF")
add_subdirectory("Network")
add_subdirectory("nanoCLR")

# Build the networking components as a separate library
# This is done this way to stop "Createprocess: file no found" errors in linker when object input file is greater than 32k
if(USE_NETWORKING_OPTION)
add_library(NetworkLib STATIC "${NF_Networking_SOURCES}" "${TARGET_ESP32_NETWORK_SOURCES}"  "${TARGET_LWIP_SOURCES}" )
endif()


#######################
# nanoCLR executable

add_executable(
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf

    "${CMAKE_CURRENT_SOURCE_DIR}/target_common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c"

    ${COMMON_PROJECT_SOURCES}
    ${TARGET_ESP32_COMMON_SOURCES}
    
    ${NANOCLR_PROJECT_SOURCES}

    ${TARGET_ESP32_NANOCLR_SOURCES}

    ${WireProtocol_SOURCES}

    # sources for nanoFramework libraries
    "${NF_CoreCLR_SOURCES}"
    "${NF_Debugger_SOURCES}"
    "${NF_Diagnostics_SOURCES}"
    
	# sources for nanoFramework Network sockets & LWIP
#    "${NF_Networking_SOURCES}"
#    "${TARGET_ESP32_NETWORK_SOURCES}"  
#    "${TARGET_LWIP_SOURCES}"
    
    # # sources for nanoFramework APIs
    "${TARGET_NANO_APIS_SOURCES}"
)



# Add link flags
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--start-group ")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS "  ${ESP32_IDF_PATH}/components/esp32/libhal.a ")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS "  -L ${ESP32_IDF_PATH}/components/esp32/lib -lcore -lrtc -lnet80211 -lmesh -lpp -lwpa -lsmartconfig -lcoexist -lwps -lwpa2 -lphy ")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -L  ${CMAKE_CURRENT_SOURCE_DIR} -T esp32_out.ld ")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -L ${ESP32_IDF_PATH}/components/esp32/ld -u ld_include_panic_highint_hdl -T esp32.common.ld -T esp32.rom.ld -T esp32.peripherals.ld -T esp32.rom.spiram_incompatible_fns.ld ")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS "  ${ESP32_IDF_PATH}/components/newlib/lib/libc.a ")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS "  ${ESP32_IDF_PATH}/components/newlib/lib/libm.a ")

#message( "project libs:${PROJECT_LINK_LIBS} " )
set(LIBPATHSAVE "")
foreach( IDF_libraries ${PROJECT_LINK_LIBS} )
    get_filename_component( LIBNAME ${IDF_libraries} NAME_WE )
    get_filename_component( LIBPATH ${IDF_libraries} DIRECTORY )
  
    string( SUBSTRING ${LIBNAME} 3 -1 LIBSHORT )
    if( NOT "${LIBPATHSAVE}" STREQUAL ${LIBPATH} )
        set(LIBPATHSAVE ${LIBPATH})
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS " -L${LIBPATH} " )
    endif() 
    set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -l${LIBSHORT} " )
    
    # Add options into LINK_FLAGS in same position as IDF link
    if( "${LIBSHORT}" STREQUAL "cxx" )
        # Cxx Library needs an undefine
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -u __cxa_guard_dummy " )
        endif()   
    if( "${LIBSHORT}" STREQUAL "freertos" )
        # Cxx Library needs an undefine
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--undefined=uxTopUsedPriority " )
        endif()   
endforeach( IDF_libraries )

if(USE_NETWORKING_OPTION)
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -L${CMAKE_CURRENT_BINARY_DIR} -lNetworkLib " )
endif()

set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -lgcc -lstdc++ -Wl,--end-group -Wl,-EL  ")

#message( "wireprotol :${WireProtocol_INCLUDE_DIRS}")

# include common directories
include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/src/CLR/Core
    ${PROJECT_SOURCE_DIR}/src/CLR/Include
    ${PROJECT_SOURCE_DIR}/src/CLR/Debugger
    ${PROJECT_SOURCE_DIR}/src/HAL/Include
    ${PROJECT_SOURCE_DIR}/src/PAL/Include
    ${PROJECT_SOURCE_DIR}/src/CLR/Runtime.Native

    ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl

    ${WireProtocol_INCLUDE_DIRS}

    ${TARGET_ESP32_IDF_INCLUDES}
)

# include directories for nanoCLR
target_include_directories(${NANOCLR_PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR"
     ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR

    # directories for nanoFramework libraries
    "${NF_CoreCLR_INCLUDE_DIRS}"
    "${NF_Debugger_INCLUDE_DIRS}"
	"${NF_Networking_INCLUDE_DIRS}"

    # includes for nanoFramework APIs
    "${TARGET_NANO_APIS_INCLUDES}"
)

# Set Includes & compile definition for Network library
if(USE_NETWORKING_OPTION)
    target_include_directories(NetworkLib PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR"
        "${NF_CoreCLR_INCLUDE_DIRS}"
        "${NF_Networking_INCLUDE_DIRS}"
    )

    target_compile_definitions(NetworkLib PUBLIC "-DPLATFORM_ESP32 " )
    set (EXTRA_LIBS ${EXTRA_LIBS} NetworkLib)
    target_link_libraries(${NANOCLR_PROJECT_NAME}.elf ${EXTRA_LIBS})
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

    # Inhibit warnings so we can compile 3rd party code (Lwip)
    set_target_properties(NetworkLib PROPERTIES COMPILE_FLAGS " -w " )
endif()

# set platform for NanoClr
target_compile_definitions(${NANOCLR_PROJECT_NAME}.elf PUBLIC "-DPLATFORM_ESP32 " )

# build types that have debugging capabilities AND are NOT RTM have to have the define 'NANOCLR_ENABLE_SOURCELEVELDEBUGGING'
if((NOT NF_BUILD_RTM) OR NF_FEATURE_DEBUGGER)
    target_compile_definitions(${NANOCLR_PROJECT_NAME}.elf PUBLIC "-DNANOCLR_ENABLE_SOURCELEVELDEBUGGING ")
endif()

# set compiler definition for using Application Domains feature
if(NF_FEATURE_USE_APPDOMAINS)
    target_compile_definitions(${NANOCLR_PROJECT_NAME}.elf PUBLIC -DNANOCLR_USE_APPDOMAINS)
endif()

# set compiler definition for implementing (or not) CRC32 in Wire Protocol
if(NF_WP_IMPLEMENTS_CRC32)
    target_compile_definitions(${NANOCLR_PROJECT_NAME}.elf PUBLIC -DWP_IMPLEMENTS_CRC32)
endif()

# set compiler definition regarding inclusion of trace messages and checks on CLR
if(NF_PLATFORM_NO_CLR_TRACE)
    target_compile_definitions(${TARGET} PUBLIC -DPLATFORM_NO_CLR_TRACE=1)
endif()

# set compiler definition regarding CLR IL inlining
if(NF_CLR_NO_IL_INLINE)
    target_compile_definitions(${TARGET} PUBLIC -DNANOCLR_NO_IL_INLINE=1)
endif()

# set extra linker flags for DEBUG
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS_DEBUG "")

# set extra linker flags for RELEASE
#set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " -flto ")

# add linker flags set with traditional CMAKE_EXE_LINKER_FLAGS
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

# add linker flags to generate map file
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${PROJECT_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.map")

# Create bin file from elf
set(GEN_ESP32_BIN_PATH ${EXECUTABLE_OUTPUT_PATH} )
set(NANOCLR_BIN_FILE ${GEN_ESP32_BIN_PATH}/${NANOCLR_PROJECT_NAME}.bin)
set(ESPTOOL_PY_FILE ${ESP32_IDF_PATH}/components/esptool_py/esptool/esptool.py )
set(ESPTOOL_PY_FLAGS "--chip esp32 elf2image --flash_mode \"dio\" --flash_freq \"40m\" --flash_size \"4MB\" " )
set(GEN_ESP32PART_PY_FILE ${ESP32_IDF_PATH}/components/partition_table/gen_esp32part.py )

add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
	COMMAND ${ESPTOOL_PY_FILE} ${ESPTOOL_PY_FLAGS} -o ${NANOCLR_BIN_FILE}   ${EXECUTABLE_OUTPUT_PATH}/${NANOCLR_PROJECT_NAME}.elf
	COMMENT "Generate nanoClr BIN file for deployment" )
    
# # Create partition table
add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
	COMMAND ${GEN_ESP32PART_PY_FILE}  --verify    ${PROJECT_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/IDF/partitions_nanoclr_4mb.csv ${GEN_ESP32_BIN_PATH}/partitions_4mb.bin
	COMMAND ${GEN_ESP32PART_PY_FILE}  --verify   ${PROJECT_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/IDF/partitions_nanoclr_2mb.csv ${GEN_ESP32_BIN_PATH}/partitions_2mb.bin
	COMMENT "Generate Esp32 Partition tables for 2MB and 4MB flash" )

# list partition table
add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
	COMMAND ${GEN_ESP32PART_PY_FILE}  ${GEN_ESP32_BIN_PATH}/partitions_4mb.bin
	COMMENT "List flash 4Mb Esp32 Partition tables" )


#message("CMAKE_CXX_FLAGS:${CMAKE_CXX_FLAGS}" )
#message("CMAKE_EXE_LINKER_FLAGS:${CMAKE_EXE_LINKER_FLAGS}" )
#message("CMAKE_EXE_LINKER_FLAGS_DEBUG:${CMAKE_EXE_LINKER_FLAGS_DEBUG}" )
#message("CMAKE_EXE_LINKER_FLAGS_DEBUG_INIT:${CMAKE_EXE_LINKER_FLAGS_DEBUG_INIT}" )
#message("CMAKE_CXX_LINK_EXECUTABLE:${CMAKE_CXX_LINK_EXECUTABLE}" )
#message("CMAKE_C_LINK_EXECUTABLE:${CMAKE_C_LINK_EXECUTABLE}" )
#message("FLAGS:${FLAGS}" )

