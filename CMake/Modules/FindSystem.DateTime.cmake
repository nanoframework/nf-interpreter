#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#


# set include directories
# CoreCLR
list(APPEND System.DateTime_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/System.DateTime")
# native code directory
list(APPEND System.DateTime_INCLUDE_DIRS "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.DateTime")

# source files
set(System.DateTime_SRCS

    # System.DateTime
    nf_dt_native.cpp
    nf_dt_native_System_DateTime.cpp
    nf_dt_native_System_Globalization_DateTimeFormat.cpp
    nf_dt_native_nanoFramework_DateTime_RTC_stubs.cpp

    # native part of System.DateTime
    nf_dt_native_nanoFramework_DateTime_RTC.cpp
)

foreach(SRC_FILE ${System.DateTime_SRCS})
    set(System.DateTime_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(System.DateTime_SRC_FILE ${SRC_FILE}
        PATHS 

            # System.DateTime
            "${PROJECT_SOURCE_DIR}/src/CLR/System.DateTime"

            # native part of System.DateTime
            "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.DateTime"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${System.DateTime_SRC_FILE}") # debug helper
    list(APPEND System.DateTime_SOURCES ${System.DateTime_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.DateTime DEFAULT_MSG System.DateTime_INCLUDE_DIRS System.DateTime_SOURCES)
