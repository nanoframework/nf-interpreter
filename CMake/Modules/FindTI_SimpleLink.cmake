#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

###################################################################################################################################
# WHEN ADDING A NEW series add the respective name to the list below along with the CMake files with GCC options and source files
###################################################################################################################################

# check if the series name is supported 

set(TI_SIMPLELINK_SUPPORTED_SERIES "CC13x2_26x2" "CC32xx" CACHE INTERNAL "supported TI series names")

list(FIND TI_SIMPLELINK_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_SERIES_NAME_INDEX)
if(TI_SIMPLELINK_SUPPORTED_SERIES EQUAL -1)
    if(TARGET_SERIES_NAME_INDEX EQUAL -1)
        message(FATAL_ERROR "\n\nSorry but the ${TARGET_SERIES} is not supported at this time...\nYou can't wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
    else()
        # series is supported by TI
        set(TARGET_VENDOR "TI" CACHE INTERNAL "target vendor is TI")
    endif()
endif()

# including here the CMake files for the source files specific to the target series
include(TI_SimpleLink_${TARGET_SERIES}_sources)

# and here the GCC options tuned for the target series 
include(TI_SimpleLink_${TARGET_SERIES}_GCC_options)

# # includes for TI_RTOS
# list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/posix/gcc")
# list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/posix/tirtos")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/ti/sysbios")
# list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/ti/sysbios/knl")
# list(APPEND TI_SimpleLink_INCLUDE_DIRS "${TARGET_BASE_LOCATION}")

# # source files for TI_RTOS
# set(TI_RTOS_SRCS

#     # TI_RTOS
#     ClockP_tirtos.c
#     DebugP_tirtos.c
#     HwiP_tirtos.c
#     MutexP_tirtos.c
#     SemaphoreP_tirtos.c
#     SwiP_tirtos.c
#     SystemP_tirtos.c
#     PowerCC32XX_tirtos.c

#     # sys/bios
#     BIOS.c
#     Clock.c
#     Event.c
#     Idle.c
#     Intrinsics.c
#     Mailbox.c
#     Queue.c
#     Semaphore_svc.c
#     Semaphore.c
#     Swi_smp.c
#     Swi.c
#     Task_smp.c
#     Task_svc.c
#     Task.c





#     # posix
#     clock.c
#     mqueue.c
#     pthread_barrier.c
#     pthread_cond.c
#     pthread.c
#     pthread_mutex.c
#     pthread_rwlock.c
#     sched.c
#     semaphore.c
#     sleep.c
#     timer.c

 
#     # HwiPCC32XX_freertos.c
#     # PowerCC32XX_freertos.c
#     # startup_cc32xx_gcc.c

# )

# foreach(SRC_FILE ${TI_RTOS_SRCS})
#     set(TI_RTOS_SCR_FILE SRC_FILE -NOTFOUND)
#     find_file(TI_RTOS_SCR_FILE ${SRC_FILE}
#         PATHS

#         # TI_RTOS
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/posix/tirtos"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/kernel/tirtos/packages/ti/dpl"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/ti/sysbios"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/ti/sysbios/knl"

#         CMAKE_FIND_ROOT_PATH_BOTH
#     )
#     # message("${SRC_FILE} >> ${TI_RTOS_SCR_FILE}") # debug helper
#     list(APPEND TI_SimpleLink_SOURCES ${TI_RTOS_SCR_FILE})
# endforeach()

# # includes for FreeRTOS
# list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/FreeRTOS_Source/include")
# list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/GCC/ARM_CM3")
# list(APPEND TI_SimpleLink_INCLUDE_DIRS "${TARGET_BASE_LOCATION}")

# # source files for FreeRTOS
# set(FreeRTOS_SRCS

#     # FreeRTOS
#     croutine.c
#     event_groups.c
#     list.c
#     queue.c
#     stream_buffer.c
#     tasks.c
#     timers.c
#     port.c
#     heap_4.c
#     clock.c
#     memory.c
#     mqueue.c
#     pthread_barrier.c
#     pthread_cond.c
#     pthread.c
#     pthread_mutex.c
#     pthread_rwlock.c
#     sched.c
#     semaphore.c
#     sleep.c
#     timer.c
#     PTLS.c
#     aeabi_portable.c
#     ClockP_freertos.c
#     DebugP_freertos.c
#     MutexP_freertos.c
#     SemaphoreP_freertos.c
#     SystemP_freertos.c
#     HwiPCC32XX_freertos.c
#     PowerCC32XX_freertos.c
#     startup_cc32xx_gcc.c

# )

# foreach(SRC_FILE ${FreeRTOS_SRCS})
#     set(FreeRTOS_SCR_FILE SRC_FILE -NOTFOUND)
#     find_file(FreeRTOS_SCR_FILE ${SRC_FILE}
#         PATHS

#         # FreeRTOS
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/posix/freertos"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/source"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/kernel/freertos/dpl"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/kernel/freertos/startup"
#         "${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/MemMang"
#         "${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/GCC/ARM_CM3"
#         "${PROJECT_BINARY_DIR}/FreeRTOS_Source"

#         CMAKE_FIND_ROOT_PATH_BOTH
#     )
#     # message("${SRC_FILE} >> ${FreeRTOS_SCR_FILE}") # debug helper
#     list(APPEND TI_SimpleLink_SOURCES ${FreeRTOS_SCR_FILE})
# endforeach()

# include(FindPackageHandleStandardArgs)

# FIND_PACKAGE_HANDLE_STANDARD_ARGS(TI_SimpleLink DEFAULT_MSG TI_SimpleLink_INCLUDE_DIRS TI_SimpleLink_SOURCES)
