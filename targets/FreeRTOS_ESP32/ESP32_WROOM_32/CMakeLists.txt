#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.ESP32)

ENABLE_LANGUAGE(ASM)

# add packages
NF_ADD_COMMON_PACKAGES()
NF_ADD_PLATFORM_PACKAGES()

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

#if(NANOCLR_GRAPHICS)
#add_subdirectory("Graphics")
#endif()

#######################
# nanoCLR executable

add_executable(
    ${NANOCLR_PROJECT_NAME}.elf
)

NF_ADD_PLATFORM_SOURCES(${NANOCLR_PROJECT_NAME})

# include directories for nanoCLR
NF_ADD_PLATFORM_INCLUDE_DIRECTORIES(${NANOCLR_PROJECT_NAME})

# Build the networking components as a separate library
# This is done this way to stop "Createprocess: file no found" errors in linker when object input file is greater than 32k
if(USE_NETWORKING_OPTION)
    add_library(NetworkLib STATIC ${NF_Networking_SOURCES}
                                  ${TARGET_ESP32_NETWORK_SOURCES}
                                  ${TARGET_LWIP_SOURCES}
                                  ${mbedTLS_SOURCES}
    )
endif()

# Add Library for all NanoFramework Api
add_library(NanoApiLib STATIC ${NF_NativeAssemblies_SOURCES} )

# Add link flags
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--start-group ")

if(USE_SECURITY_MBEDTLS_OPTION)
    add_dependencies(${NANOCLR_PROJECT_NAME}.elf NetworkLib)
endif()

add_dependencies(${NANOCLR_PROJECT_NAME}.elf NanoApiLib)

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

    # Add psram workarounds for newlib
    if( "${LIBSHORT}" STREQUAL "newlib" )
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " ${ESP32_IDF_PATH}/components/newlib/lib/libc-psram-workaround.a " )
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " ${ESP32_IDF_PATH}/components/newlib/lib/libm-psram-workaround.a " )
    endif()

    set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -l${LIBSHORT} " )
    
    if( "${LIBSHORT}" STREQUAL "app_update" )
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -u esp_app_desc " )
    endif()

    if( "${LIBSHORT}" STREQUAL "bt" )
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -L${ESP32_IDF_PATH}/components/bt/lib -lbtdm_app " )
    endif()

    # Add options into LINK_FLAGS in same position as IDF link
    if( "${LIBSHORT}" STREQUAL "cxx" )
        # Cxx Library needs an undefine
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -u __cxa_guard_dummy -u __cxx_fatal_exception  " )
    endif()

    if( "${LIBSHORT}" STREQUAL "esp32" )
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " ${ESP32_IDF_PATH}/components/esp32/libhal.a  " )
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -L ${ESP32_IDF_PATH}/components/esp32/lib -lcore -lrtc -lnet80211  -lpp -lwpa -lsmartconfig -lcoexist -lwps -lwpa2 -lespnow -lphy -lmesh")
	
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -L  ${CMAKE_CURRENT_SOURCE_DIR} -T esp32_out.ld ")
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -L ${ESP32_IDF_PATH}/components/esp32/ld -u ld_include_panic_highint_hdl -T esp32.project.ld -T esp32.rom.ld -T esp32.peripherals.ld -T esp32.rom.libgcc.ld   ")
    endif()


    if( "${LIBSHORT}" STREQUAL "freertos" )
        # Cxx Library needs an undefine
        set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--undefined=uxTopUsedPriority " )
    endif()

endforeach( IDF_libraries )

if(USE_NETWORKING_OPTION)
    set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -L${CMAKE_CURRENT_BINARY_DIR} -lNetworkLib " )
endif()

set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--whole-archive  -L${CMAKE_CURRENT_BINARY_DIR} -lNanoApiLib -Wl,--no-whole-archive " )

set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -lgcc -lstdc++ -lgcov -Wl,--end-group -Wl,-EL  ")

# Set Includes & compile definition for Network library
if(USE_NETWORKING_OPTION)
    target_include_directories(NetworkLib PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${NF_CoreCLR_INCLUDE_DIRS}
        ${NF_Networking_INCLUDE_DIRS}
        ${mbedTLS_INCLUDE_DIRS}
        ${TARGET_ESP32_IDF_INCLUDES}
    )

    target_compile_definitions(NetworkLib PUBLIC "-DPLATFORM_ESP32 " )
    set (EXTRA_LIBS ${EXTRA_LIBS} NetworkLib)
    target_link_libraries(${NANOCLR_PROJECT_NAME}.elf ${EXTRA_LIBS})
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

    # Inhibit warnings so we can compile 3rd party code (Lwip)
    set_target_properties(NetworkLib PROPERTIES COMPILE_FLAGS " -w " )
endif()

 target_include_directories(NanoApiLib PUBLIC
    # directories for nanoFramework libraries
    ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${NF_CoreCLR_INCLUDE_DIRS}

    ${TARGET_ESP32_IDF_INCLUDES}

    # includes for nanoFramework APIs
    ${NF_NativeAssemblies_INCLUDE_DIRS}
)

# set platform for NanoClr
target_compile_definitions(${NANOCLR_PROJECT_NAME}.elf PUBLIC "-DPLATFORM_ESP32 " )

# set platform for NanoApiLib
# ESP32 has FPU
target_compile_definitions(NanoApiLib PUBLIC "-DPLATFORM_ESP32 -DUSE_FPU=TRUE ")


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
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.map")
# Verbose linking, uncomment to enable
#set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf    APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--verbose ")

# add dependency for networking
if(USE_NETWORKING_OPTION)
    add_dependencies(${NANOCLR_PROJECT_NAME}.elf NetworkLib)
endif()

# need to tweak this in case this path variable is empty
if(NOT EXECUTABLE_OUTPUT_PATH)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
endif()

# Create bin file from elf
set(GEN_ESP32_BIN_PATH ${EXECUTABLE_OUTPUT_PATH} )
set(NANOCLR_BIN_FILE ${GEN_ESP32_BIN_PATH}/${NANOCLR_PROJECT_NAME}.bin)
set(ESPTOOL_PY_FILE ${ESP32_IDF_PATH}/components/esptool_py/esptool/esptool.py )
set(ESPTOOL_PY_FLAGS "--chip" "esp32" "elf2image" "--flash_mode" "dio" "--flash_freq" "40m" "--flash_size" "4MB" )
# message("ESPTOOL_PY_FLAGS:${ESPTOOL_PY_FLAGS}" )

add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
    COMMAND ${ESPTOOL_PY_FILE} ${ESPTOOL_PY_FLAGS} -o ${NANOCLR_BIN_FILE}   ${EXECUTABLE_OUTPUT_PATH}/${NANOCLR_PROJECT_NAME}.elf
    COMMENT "Generate nanoCLR BIN file for deployment" )
    
# Create partition tables
set(ESP32_PARTITION_TABLE_UTILITY ${ESP32_IDF_PATH}/components/partition_table/gen_esp32part.py )

add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
    COMMAND ${ESP32_PARTITION_TABLE_UTILITY} 
    --flash-size 16MB 
    ${CMAKE_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/IDF/partitions_nanoclr_16mb.csv
    ${GEN_ESP32_BIN_PATH}/partitions_16mb.bin
    COMMENT "Generate ESP32 partition table for 16MB flash" )

add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
    COMMAND ${ESP32_PARTITION_TABLE_UTILITY} 
    --flash-size 8MB 
    ${CMAKE_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/IDF/partitions_nanoclr_8mb.csv
    ${GEN_ESP32_BIN_PATH}/partitions_8mb.bin
    COMMENT "Generate ESP32 partition table for 8MB flash" )

add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
    COMMAND ${ESP32_PARTITION_TABLE_UTILITY} 
    --flash-size 4MB 
    ${CMAKE_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/IDF/partitions_nanoclr_4mb.csv
    ${GEN_ESP32_BIN_PATH}/partitions_4mb.bin
    COMMENT "Generate Esp32 partition table for 4MB flash" )

add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
    COMMAND ${ESP32_PARTITION_TABLE_UTILITY} 
    --flash-size 2MB 
    ${CMAKE_SOURCE_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/IDF/partitions_nanoclr_2mb.csv
    ${GEN_ESP32_BIN_PATH}/partitions_2mb.bin
    COMMENT "Generate Esp32 partition table for 2MB flash" )
