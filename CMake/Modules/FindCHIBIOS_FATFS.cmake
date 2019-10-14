#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/fatfs-0.13_patched.7z
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/
)


# List of the required FatFs include files.
list(APPEND CHIBIOS_FATFS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/fatfs/src)


set(FATFS_SRCS
    # bindings
    fatfs_syscall.c

    # fatfs
    ff.c
    ffunicode.c
)

foreach(SRC_FILE ${FATFS_SRCS})
    set(FATFS_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(FATFS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various/fatfs_bindings
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/fatfs/src

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${FATFS_SRC_FILE}") # debug helper
    list(APPEND CHIBIOS_FATFS_SOURCES ${FATFS_SRC_FILE})
endforeach()

# fatfs_diskio is hacked because of USB Host, so we need to use the source from the appropriate location
if(NF_FEATURE_HAS_USB_MSD)
    # get it from ChibiOS contribution
    list(APPEND CHIBIOS_FATFS_SOURCES ${PROJECT_BINARY_DIR}/ChibiOS-Contrib_Source/os/various/fatfs_bindings/fatfs_diskio.c)
else()
    # get it from standard ChibiOS
    list(APPEND CHIBIOS_FATFS_SOURCES ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various/fatfs_bindings/fatfs_diskio.c)
endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CHIBIOS_FATFS DEFAULT_MSG CHIBIOS_FATFS_INCLUDE_DIRS CHIBIOS_FATFS_SOURCES)

# setup target to unzip ChibiOS external filesystem components
add_custom_target( CHIBIOS_FILESYSTEM_COMPONENTS ALL )

add_custom_command(TARGET CHIBIOS_FILESYSTEM_COMPONENTS
PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/fatfs-0.13_patched.7z
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/
    DEPENDS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/fatfs-0.13_patched.7z

    VERBATIM
)

# this depends on ChibiOS target being already downloaded
add_dependencies(CHIBIOS_FILESYSTEM_COMPONENTS ChibiOS)
