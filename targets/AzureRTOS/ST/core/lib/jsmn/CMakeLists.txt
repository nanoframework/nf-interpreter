# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

set(TARGET jsmn)

add_library(${TARGET} OBJECT
    jsmn.c
)

target_include_directories(${TARGET}
    PUBLIC
        src
)

target_compile_definitions(${TARGET}
    PUBLIC
        JSMN_HEADER
)
