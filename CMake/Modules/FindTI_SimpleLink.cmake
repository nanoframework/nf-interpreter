#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

###################################################################################################################################
# WHEN ADDING A NEW series add the respective name to the list below along with the CMake files with GCC options and source files
###################################################################################################################################

# check if the series name is supported 

set(TI_SIMPLELINK_SUPPORTED_SERIES "CC32xx" CACHE INTERNAL "supported TI series names")

list(FIND TI_SIMPLELINK_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_SERIES_NAME_INDEX)
if(TI_SIMPLELINK_SUPPORTED_SERIES EQUAL -1)
    if(TARGET_SERIES_NAME_INDEX EQUAL -1)
        message(FATAL_ERROR "\n\nSorry but the ${TARGET_SERIES} is not supported at this time...\nYou can wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
    else()
        # series is supported by TI
        set(TARGET_VENDOR "TI" CACHE INTERNAL "target vendor is TI")
    endif()
endif()

# including here the CMake files for the source files specific to the target series
include(TI_SimpleLink_${TARGET_SERIES}_sources)

# and here the GCC options tuned for the target series 
include(TI_SimpleLink_${TARGET_SERIES}_GCC_options)

# includes for FreeRTOS
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/FreeRTOS_Source/lib/include")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/FreeRTOS_Source/lib/include/private")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/FreeRTOS_Source/lib/FreeRTOS/portable/GCC/ARM_CM3")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${TARGET_BASE_LOCATION}")

# source files for FreeRTOS
set(FreeRTOS_SRCS

    # FreeRTOS
    event_groups.c
    list.c
    queue.c
    stream_buffer.c
    tasks.c
    timers.c
    port.c
    heap_4.c
    clock.c
    memory.c
    mqueue.c
    pthread_barrier.c
    pthread_cond.c
    pthread.c
    pthread_mutex.c
    pthread_rwlock.c
    sched.c
    semaphore.c
    sleep.c
    timer.c
    PTLS.c
    aeabi_portable.c
    ClockP_freertos.c
    DebugP_freertos.c
    MutexP_freertos.c
    SemaphoreP_freertos.c
    SystemP_freertos.c
    HwiPCC32XX_freertos.c
    PowerCC32XX_freertos.c
    startup_cc32xx_gcc.c

)

foreach(SRC_FILE ${FreeRTOS_SRCS})
    set(FreeRTOS_SCR_FILE SRC_FILE -NOTFOUND)
    find_file(FreeRTOS_SCR_FILE ${SRC_FILE}
        PATHS

        # FreeRTOS
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/posix/freertos"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/source"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/kernel/freertos/dpl"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/kernel/freertos/startup"
        "${PROJECT_BINARY_DIR}/FreeRTOS_Source/lib/FreeRTOS/portable/MemMang"
        "${PROJECT_BINARY_DIR}/FreeRTOS_Source/lib/FreeRTOS/portable/GCC/ARM_CM3"
        "${PROJECT_BINARY_DIR}/FreeRTOS_Source/lib/FreeRTOS"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${FreeRTOS_SCR_FILE}") # debug helper
    list(APPEND TI_SimpleLink_SOURCES ${FreeRTOS_SCR_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(TI_SimpleLink DEFAULT_MSG TI_SimpleLink_INCLUDE_DIRS TI_SimpleLink_SOURCES)
