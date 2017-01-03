cmake_minimum_required(VERSION 3.0)
ENABLE_LANGUAGE(ASM)

include_directories(
     ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(MBEDOS REQUIRED)
include_directories(${MBEDOS_INCLUDE_DIRS})

# pass the RTOS option to the compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -include mbed_config.h ${MBEDOS_COMPILE_OPTIONS} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include mbed_config.h ${MBEDOS_COMPILE_OPTIONS} ")

set(PROJECT_SOURCES
    main.cpp
    
    WireProtocol.cpp
)

add_executable(
    # executables for project, project sources 
    ${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES}
    
    # sources for mBed RTOS
    ${MBEDOS_SOURCES}
 )

MBED_SET_LINKER_OPTIONS(${CMAKE_PROJECT_NAME})

MBED_ADD_HEX_BIN_DUMP_TARGETS(${CMAKE_PROJECT_NAME})

MBED_PRINT_SIZE_OF_TARGETS(${CMAKE_PROJECT_NAME})

# generate map file
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(S19_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.s19)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)
set(DUMP_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.lst)

# copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
file(COPY "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}" DESTINATION "${PROJECT_SOURCE_DIR}/build/")

# generate HEX, BIN and LST files as needed
if(CMAKE_BUILD_TYPE EQUAL "Release")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${PROJECT_NAME}> ${S19_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
            COMMENT "Generate HEX and BIN files fro deployment")
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${PROJECT_NAME}> ${S19_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
            COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:${PROJECT_NAME}> > ${DUMP_FILE}
            COMMENT "Generate HEX and BIN files for deployment, LST file for debug")
endif()
