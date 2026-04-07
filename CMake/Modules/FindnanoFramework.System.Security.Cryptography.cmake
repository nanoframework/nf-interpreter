#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.System.Security.Cryptography)


# set include directories
list(APPEND nanoFramework.System.Security.Cryptography_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.System.Security.Cryptography_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.System.Security.Cryptography_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.System.Security.Cryptography_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.System.Security.Cryptography_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.System.Security.Cryptography_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.System.Security.Cryptography)

# source files
set(nanoFramework.System.Security.Cryptography_SRCS

    nf_sys_sec_cryptography.cpp

    
    nf_sys_sec_cryptography_System_Security_Cryptography_Aes.cpp
    nf_sys_sec_cryptography_System_Security_Cryptography_HMACSHA256.cpp
    nf_sys_sec_cryptography_System_Security_Cryptography_HMACSHA512.cpp

)

foreach(SRC_FILE ${nanoFramework.System.Security.Cryptography_SRCS})

    set(nanoFramework.System.Security.Cryptography_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.System.Security.Cryptography_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.System.Security.Cryptography

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.System.Security.Cryptography_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.System.Security.Cryptography_SOURCES ${nanoFramework.System.Security.Cryptography_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.System.Security.Cryptography DEFAULT_MSG nanoFramework.System.Security.Cryptography_INCLUDE_DIRS nanoFramework.System.Security.Cryptography_SOURCES)
