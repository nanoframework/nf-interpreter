#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(esp32_idf)

include(ESP32_utils.cmake)

# set(ESP32_SRCS

# )

# foreach(SRC_FILE ${ESP32_SRCS})

#     set(ESP32_SRC_FILE SRC_FILE-NOTFOUND)

#     find_file(ESP32_SRC_FILE ${SRC_FILE}
#         PATHS 
        
#             ${esp32_idf_SOURCE_DIR}/


#         CMAKE_FIND_ROOT_PATH_BOTH
#     )

#     if (BUILD_VERBOSE)
#         message("${SRC_FILE} >> ${ESP32_SRC_FILE}")
#     endif()

#     list(APPEND ESP32_IDF_SOURCES ${ESP32_SRC_FILE})

# endforeach()
