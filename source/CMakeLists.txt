cmake_minimum_required(VERSION 3.0)
ENABLE_LANGUAGE(ASM)

find_package(STM32CMSIS REQUIRED)
find_package(STM32HAL COMPONENTS gpio tim REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${STM32CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
)

if(RTOS_FREERTOS_CHECK)
   
    find_package(STM32FREERTOS REQUIRED)
    include_directories(${STM32FREERTOS_INCLUDE_DIR})

    # pass the RTOS option to the compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${RTOS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${RTOS}")

elseif(RTOS_MBED_RTOS_CHECK)

    find_package(MBEDRTOS REQUIRED)
    include_directories(${MBEDRTOS_INCLUDE_DIRS})

    # pass the RTOS option to the compiler flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${RTOS} -DTOOLCHAIN_GCC -DTARGET_${TARGET_CHIP} ${MBEDRTOS_COMPILE_OPTIONS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${RTOS} -DTOOLCHAIN_GCC -DTARGET_${TARGET_CHIP} ${MBEDRTOS_COMPILE_OPTIONS}")

elseif(RTX_RTOS_CHECK)

    find_package(RTXRTOS REQUIRED)
    include_directories(${RTXRTOS_INCLUDE_DIRS})

    # pass the RTOS option to the compiler flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${RTOS} -DTOOLCHAIN_GCC -DTARGET_${TARGET_CHIP} -D__CMSIS_RTOS ${RTXRTOS_COMPILE_OPTIONS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${RTOS} -DTOOLCHAIN_GCC -DTARGET_${TARGET_CHIP} -D__CMSIS_RTOS ${RTXRTOS_COMPILE_OPTIONS}")

elseif(RTX_RTOS2_CHECK)

    find_package(RTXRTOS2 REQUIRED)
    include_directories(${RTXRTOS2_INCLUDE_DIRS})

    # pass the RTOS option to the compiler flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${RTOS} -DTOOLCHAIN_GCC -DTARGET_${TARGET_CHIP} -D__CMSIS_RTOS ${RTXRTOS2_COMPILE_OPTIONS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${RTOS} -DTOOLCHAIN_GCC -DTARGET_${TARGET_CHIP} -D__CMSIS_RTOS ${RTXRTOS2_COMPILE_OPTIONS}")
message("fglas: ${CMAKE_C_FLAGS}")

else()

    # pass the NO RTOS option to the compiler flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNOROTS ${RTXRTOS_COMPILE_OPTIONS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOROTS ${RTXRTOS_COMPILE_OPTIONS}")

endif()

set(PROJECT_SOURCES
    main.c
    $<$<BOOL:${RTOS_MBED_RTOS_CHECK}>:RTX_Conf_CM_MBED.c>
    $<$<BOOL:${RTX_RTOS_CHECK}>:RTX_Conf_CM_RTX.c>
    $<$<BOOL:${RTX_RTOS2_CHECK}>:RTX_Config_RTX2.c>
)

add_executable(
    # executables for project, project sources, STM32 CMSIS sources and STM32 HAL libraries
    ${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES} ${STM32CMSIS_SOURCES} ${STM32HAL_SOURCES} 
    
    # sources for FreeRTOS and STM32 FreeRTOS port
    $<$<BOOL:${RTOS_FREERTOS_CHECK}>:${STM32FREERTOS_SOURCES}>
    
    # sources for mBed RTOS
    $<$<BOOL:${RTOS_MBED_RTOS_CHECK}>:${MBEDRTOS_SOURCES}>
    
    # sources for RTX RTOS
    $<$<BOOL:${RTX_RTOS_CHECK}>:${RTXRTOS_SOURCES}>
    
    # sources for RTX RTOS2
    $<$<BOOL:${RTX_RTOS2_CHECK}>:${RTXRTOS2_SOURCES}>
)

STM32_SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME})

STM32_ADD_HEX_BIN_DUMP_TARGETS(${CMAKE_PROJECT_NAME})

STM32_PRINT_SIZE_OF_TARGETS(${CMAKE_PROJECT_NAME})

# generate map file
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(S19_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.s19)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)
set(DUMP_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.lst)

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
