#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Runtime.Serialization)


# set include directories
list(APPEND System.Runtime.Serialization_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Core)
list(APPEND System.Runtime.Serialization_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Include)
list(APPEND System.Runtime.Serialization_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/HAL/Include)
list(APPEND System.Runtime.Serialization_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Include)
list(APPEND System.Runtime.Serialization_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.Runtime.Serialization_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/System.Runtime.Serialization)

# source files
set(System.Runtime.Serialization_SRCS

    nf_system_runtime_serialization.cpp

    nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter.cpp

)

foreach(SRC_FILE ${System.Runtime.Serialization_SRCS})

    set(System.Runtime.Serialization_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(System.Runtime.Serialization_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${CMAKE_SOURCE_DIR}/src/System.Runtime.Serialization

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${System.Runtime.Serialization_SRC_FILE}")
    endif()

    list(APPEND System.Runtime.Serialization_SOURCES ${System.Runtime.Serialization_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Runtime.Serialization DEFAULT_MSG System.Runtime.Serialization_INCLUDE_DIRS System.Runtime.Serialization_SOURCES)
