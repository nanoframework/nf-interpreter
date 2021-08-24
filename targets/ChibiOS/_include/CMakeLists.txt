#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append include directory for target ChibiOS
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD})

# make var global
set(TARGET_CHIBIOS_COMMON_INCLUDE_DIRS ${TARGET_CHIBIOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

# add headers with target platform definitions and SWO
if(IS_COMMUNITY_TARGET)

    configure_file(${CMAKE_SOURCE_DIR}/CMake/ChibiOS_target_os.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_os.h @ONLY)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/swo.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/swo.h @ONLY)
    
else()

    configure_file(${CMAKE_SOURCE_DIR}/CMake/ChibiOS_target_os.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_os.h @ONLY)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/swo.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/swo.h @ONLY)

endif()
