#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

set(NANOCLR_OSX_SOURCES
    CLRStartup.cpp
    FileStore_OSX.cpp
    main.cpp
    Memory.cpp
    ParseOptions_OSX.cpp
    platform_heap.cpp
    targetHAL_Time.cpp
    targetPAL_Events.cpp
    targetPAL_Time.cpp
    targetRandom.cpp
    Target_BlockStorage.cpp
    Target_HAL.cpp
    Various.cpp
    WatchDog.cpp
)

# Keep the output name aligned with existing host naming conventions.
add_executable(nanoFramework.nanoCLR ${NANOCLR_OSX_SOURCES})

target_compile_features(nanoFramework.nanoCLR PRIVATE cxx_std_20)

target_include_directories(
    nanoFramework.nanoCLR
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../Include
)

target_compile_definitions(
    nanoFramework.nanoCLR
    PRIVATE
        NANOCLR_HOST_POSIX=1
        NANOCLR_HOST_OSX=1
        NANOCLR_HOST_WINDOWS=0
        NANO_OSX_ENABLE_SMOKE=$<BOOL:${NANO_OSX_ENABLE_SMOKE}>
        NANOCLR_OSX_VERSION_STRING="${PROJECT_VERSION}"
)

set_target_properties(
    nanoFramework.nanoCLR
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

