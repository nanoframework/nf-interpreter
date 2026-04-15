#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Convenience alias - repo root is three levels up from targets/posix/nanoCLR
set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# ─── POSIX-specific platform glue ────────────────────────────────────────────
# main.cpp is NOT part of the shared library; it is built as a separate thin
# test harness (nanoFramework.nanoCLR.test) that links against the .dylib.
set(NANOCLR_POSIX_SOURCES
    CLRStartup.cpp
    FileStore_POSIX.cpp
    Memory.cpp
    nanoCLR_native_posix.cpp
    platform_heap.cpp
    posix_headers_shim_test.cpp
    posix_stubs.cpp
    targetHAL_Time.cpp
    targetPAL_Events.cpp
    targetPAL_Time.cpp
    targetRandom.cpp
    Target_BlockStorage.cpp
    Target_HAL.cpp
    Various.cpp
    WatchDog.cpp
)

# ─── CLR Core ─────────────────────────────────────────────────────────────────
set(NF_CLR_CORE_SRCS
    ${REPO_ROOT}/src/CLR/Core/Cache.cpp
    ${REPO_ROOT}/src/CLR/Core/Checks.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_DblLinkedList.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_Array.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_ArrayList.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_BinaryBlob.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_Delegate.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_Delegate_List.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_Finalizer.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_Lock.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_LockRequest.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_Node.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_Queue.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_Stack.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_String.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_Timer.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapBlock_WaitForObject.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_HeapCluster.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_Interop.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_Memory.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_ObjectToEvent_Destination.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_ObjectToEvent_Source.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_RuntimeMemory.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_StackFrame.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_SystemAssembliesTable.cpp
    ${REPO_ROOT}/src/CLR/Core/CLR_RT_UnicodeHelper.cpp
    ${REPO_ROOT}/src/CLR/Core/Core.cpp
    ${REPO_ROOT}/src/CLR/Core/Execution.cpp
    ${REPO_ROOT}/src/CLR/Core/GarbageCollector.cpp
    ${REPO_ROOT}/src/CLR/Core/GarbageCollector_Compaction.cpp
    ${REPO_ROOT}/src/CLR/Core/GarbageCollector_ComputeReachabilityGraph.cpp
    ${REPO_ROOT}/src/CLR/Core/GarbageCollector_Info.cpp
    ${REPO_ROOT}/src/CLR/Core/Interpreter.cpp
    ${REPO_ROOT}/src/CLR/Core/Random.cpp
    ${REPO_ROOT}/src/CLR/Core/Streams.cpp
    ${REPO_ROOT}/src/CLR/Core/StringTable.cpp
    ${REPO_ROOT}/src/CLR/Core/StringTableData.cpp
    ${REPO_ROOT}/src/CLR/Core/Thread.cpp
    ${REPO_ROOT}/src/CLR/Core/TypeSystem.cpp
    ${REPO_ROOT}/src/CLR/Core/TypeSystemLookup.cpp
    ${REPO_ROOT}/src/CLR/Core/Various.cpp
    ${REPO_ROOT}/src/HAL/nanoHAL_SystemInformation.cpp
    # Sub-directories
    ${REPO_ROOT}/src/CLR/Core/Hardware/Hardware.cpp
    ${REPO_ROOT}/src/CLR/Core/InterruptHandler/InterruptHandler.cpp
    ${REPO_ROOT}/src/CLR/Core/NativeEventDispatcher/NativeEventDispatcher.cpp
    ${REPO_ROOT}/src/CLR/Core/RPC/RPC_stub.cpp
    ${REPO_ROOT}/src/CLR/Core/nanoSupport_CRC32.c
)

# ─── CLR CoreLib ──────────────────────────────────────────────────────────────
file(GLOB NF_CORLIB_SRCS ${REPO_ROOT}/src/CLR/CorLib/*.cpp)

# ─── CLR Startup ─────────────────────────────────────────────────────────────
# We use targets/posix/nanoCLR/CLRStartup.cpp (already in NANOCLR_POSIX_SOURCES)
# instead of the common embedded src/CLR/Startup/CLRStartup.cpp, because the
# POSIX version adds virtual-device-style assembly loading from memory buffers.
set(NF_CLR_STARTUP_SRCS)

# ─── Stubs (Debugger / Messaging / Diagnostics) ───────────────────────────────
set(NF_CLR_STUB_SRCS
    ${REPO_ROOT}/src/CLR/Debugger/Debugger_stub.cpp
    ${REPO_ROOT}/src/CLR/Messaging/Messaging_stub.cpp
    ${REPO_ROOT}/src/CLR/Diagnostics/Info_Safeprintf.cpp
    ${REPO_ROOT}/src/CLR/Diagnostics/Diagnostics_stub.cpp
    ${REPO_ROOT}/src/CLR/Core/Serialization/BinaryFormatter_stub.cpp
    ${REPO_ROOT}/src/CLR/Helpers/nanoprintf/nanoprintf.c
    ${REPO_ROOT}/src/CLR/Helpers/NanoRingBuffer/nanoRingBuffer.c
    ${REPO_ROOT}/src/CLR/Helpers/Base64/base64.c
)

# ─── System.Device.Gpio ──────────────────────────────────────────────────────────
set(NF_SYSTEM_DEVICE_GPIO_SRCS
    ${REPO_ROOT}/src/System.Device.Gpio/sys_dev_gpio_native.cpp
    ${REPO_ROOT}/src/System.Device.Gpio/sys_dev_gpio_native_System_Device_Gpio_GpioController_stubs.cpp
    ${REPO_ROOT}/src/System.Device.Gpio/sys_dev_gpio_native_System_Device_Gpio_GpioPin_stubs.cpp
)

# ─── System.Device.I2c ────────────────────────────────────────────────────────────
set(NF_SYSTEM_DEVICE_I2C_SRCS
    ${REPO_ROOT}/src/System.Device.I2c/sys_dev_i2c_native.cpp
    ${REPO_ROOT}/src/System.Device.I2c/sys_dev_i2c_native_System_Device_I2c_I2cDevice_stubs.cpp
)

# ─── System.Device.Spi ────────────────────────────────────────────────────────────
set(NF_SYSTEM_DEVICE_SPI_SRCS
    ${REPO_ROOT}/src/System.Device.Spi/sys_dev_spi_native.cpp
    ${REPO_ROOT}/src/System.Device.Spi/sys_dev_spi_native_System_Device_Spi_SpiDevice_stubs.cpp
    ${REPO_ROOT}/src/System.Device.Spi/sys_dev_spi_native_System_Device_Spi_SpiBusInfo_stubs.cpp
)

# ─── System.IO.Ports ─────────────────────────────────────────────────────────────
set(NF_SYSTEM_IO_PORTS_SRCS
    ${REPO_ROOT}/src/System.IO.Ports/sys_io_ser_native.cpp
    ${REPO_ROOT}/src/System.IO.Ports/sys_io_ser_native_System_IO_Ports_SerialPort__.cpp
    ${REPO_ROOT}/src/System.IO.Ports/sys_io_ser_native_System_IO_Ports_SerialPort_stubs.cpp
)

# ─── System.Math ────────────────────────────────────────────────────────────
set(NF_SYSTEM_MATH_SRCS
    ${REPO_ROOT}/src/CLR/System.Math/nf_native_system_math.cpp
    ${REPO_ROOT}/src/CLR/System.Math/nf_native_system_math_System_Math.cpp
)

# ─── System.Runtime.Serialization ───────────────────────────────────────────────
set(NF_RUNTIME_SERIALIZATION_SRCS
    ${REPO_ROOT}/src/System.Runtime.Serialization/nf_system_runtime_serialization.cpp
    ${REPO_ROOT}/src/System.Runtime.Serialization/nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter.cpp
)

# ─── nanoFramework.ResourceManager ────────────────────────────────────────────────
set(NF_RESOURCE_MANAGER_SRCS
    ${REPO_ROOT}/src/nanoFramework.ResourceManager/nf_system_resourcemanager.cpp
    ${REPO_ROOT}/src/nanoFramework.ResourceManager/nf_system_resourcemanager_System_Resources_ResourceManager.cpp
    ${REPO_ROOT}/src/nanoFramework.ResourceManager/nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility.cpp
)

# ─── nanoFramework.System.IO.Hashing ───────────────────────────────────────────────
set(NF_SYS_IO_HASHING_SRCS
    ${REPO_ROOT}/src/nanoFramework.System.IO.Hashing/nf_sys_io_hashing.cpp
    ${REPO_ROOT}/src/nanoFramework.System.IO.Hashing/nf_sys_io_hashing_System_IO_Hashing_Crc32_stubs.cpp
)

# ─── nanoFramework.Runtime.Events ───────────────────────────────────────────────────
set(NF_RUNTIME_EVENTS_SRCS
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_EventSink.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate.cpp
)

# ─── System.Text ────────────────────────────────────────────────────────────
set(NF_SYSTEM_TEXT_SRCS
    ${REPO_ROOT}/src/nanoFramework.System.Text/nf_system_text.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Text/nf_system_text_System_Text_UTF8Decoder.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Text/nf_system_text_System_Text_UTF8Encoding.cpp
)

# ─── System.Collections ─────────────────────────────────────────────────────
set(NF_SYSTEM_COLLECTIONS_SRCS
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Hashtable.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Hashtable__HashtableEnumerator.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Queue.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Stack.cpp
)

# ─── Runtime.Native ───────────────────────────────────────────────────────────
set(NF_RUNTIME_NATIVE_SRCS
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_GC.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_ExecutionConstraint.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_Power.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_Rtc_stubs.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_System_Environment.cpp
)

# ─── HAL / PAL stubs ──────────────────────────────────────────────────────────
set(NF_HAL_PAL_STUB_SRCS
    ${REPO_ROOT}/src/HAL/nanoHAL_Time.cpp
    ${REPO_ROOT}/src/HAL/nanoHAL_Watchdog.c
    ${REPO_ROOT}/src/HAL/nanoHAL_SystemEvents.c
    ${REPO_ROOT}/src/PAL/AsyncProcCall/AsyncCompletions.cpp
    ${REPO_ROOT}/src/PAL/AsyncProcCall/AsyncContinuations.cpp
    ${REPO_ROOT}/src/PAL/COM/COM_stubs.c
    ${REPO_ROOT}/src/PAL/COM/GenericPort_stubs.c
    ${REPO_ROOT}/src/PAL/Double/nanoPAL_NativeDouble.cpp
    ${REPO_ROOT}/src/PAL/FileSystem/nanoPAL_FileSystem_stubs.cpp
    ${REPO_ROOT}/src/PAL/Profiler/nanoPAL_PerformanceCounters_stubs.cpp
    ${REPO_ROOT}/src/PAL/nanoPAL_Network_stubs.cpp
)

list(APPEND NANOCLR_POSIX_SOURCES
    ${NF_CLR_CORE_SRCS}
    ${NF_CORLIB_SRCS}
    ${NF_CLR_STARTUP_SRCS}
    ${NF_CLR_STUB_SRCS}
    ${NF_SYSTEM_DEVICE_GPIO_SRCS}
    ${NF_SYSTEM_DEVICE_I2C_SRCS}
    ${NF_SYSTEM_DEVICE_SPI_SRCS}
    ${NF_SYSTEM_IO_PORTS_SRCS}
    ${NF_SYSTEM_MATH_SRCS}
    ${NF_RUNTIME_SERIALIZATION_SRCS}
    ${NF_RESOURCE_MANAGER_SRCS}
    ${NF_SYS_IO_HASHING_SRCS}
    ${NF_RUNTIME_EVENTS_SRCS}
    ${NF_SYSTEM_TEXT_SRCS}
    ${NF_SYSTEM_COLLECTIONS_SRCS}
    ${NF_RUNTIME_NATIVE_SRCS}
    ${NF_HAL_PAL_STUB_SRCS}
)

# ─── Shared library target ───────────────────────────────────────────────────
# Produces nanoFramework.nanoCLR.dylib (macOS) / .so (Linux) loaded by the
# .NET nanoclr global tool via P/Invoke.
add_library(nanoFramework.nanoCLR SHARED ${NANOCLR_POSIX_SOURCES})

target_compile_features(nanoFramework.nanoCLR PRIVATE cxx_std_20)

target_include_directories(
    nanoFramework.nanoCLR
    PRIVATE
    # POSIX-specific headers first so they shadow any Windows-only variants
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
    # Shared CLR, HAL and PAL headers
    ${REPO_ROOT}/src/CLR/Include
    ${REPO_ROOT}/src/CLR/Core
    ${REPO_ROOT}/src/CLR/Core/Hardware
    ${REPO_ROOT}/src/CLR/Core/InterruptHandler
    ${REPO_ROOT}/src/CLR/Core/NativeEventDispatcher
    ${REPO_ROOT}/src/CLR/Core/RPC
    ${REPO_ROOT}/src/CLR/CorLib
    ${REPO_ROOT}/src/CLR/Startup
    ${REPO_ROOT}/src/CLR/Diagnostics
    ${REPO_ROOT}/src/CLR/Debugger
    ${REPO_ROOT}/src/CLR/Helpers/NanoRingBuffer
    ${REPO_ROOT}/src/CLR/Helpers/nanoprintf
    ${REPO_ROOT}/src/CLR/Helpers/Base64
    ${REPO_ROOT}/src/HAL/Include
    ${REPO_ROOT}/src/PAL/Include
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events
    ${REPO_ROOT}/src/nanoFramework.System.Collections
    ${REPO_ROOT}/src/nanoFramework.System.Text
    ${REPO_ROOT}/src/CLR/System.Math
    ${REPO_ROOT}/src/System.Runtime.Serialization
    ${REPO_ROOT}/src/nanoFramework.ResourceManager
    ${REPO_ROOT}/src/nanoFramework.System.IO.Hashing
    ${REPO_ROOT}/src/System.Device.Gpio
    ${REPO_ROOT}/src/System.Device.I2c
    ${REPO_ROOT}/src/System.Device.Spi
    ${REPO_ROOT}/src/System.IO.Ports
)

# Determine platform-specific flags.
if(APPLE)
    set(NANOCLR_HOST_APPLE 1)
    set(NANOCLR_HOST_LINUX 0)
    set(NANOCLR_PLATFORM_NAME "macOS")
else()
    set(NANOCLR_HOST_APPLE 0)
    set(NANOCLR_HOST_LINUX 1)
    set(NANOCLR_PLATFORM_NAME "Linux")
endif()

target_compile_definitions(
    nanoFramework.nanoCLR
    PRIVATE
    PLATFORM_POSIX_HOST=1
    NANOCLR_HOST_POSIX=1
    NANOCLR_HOST_APPLE=${NANOCLR_HOST_APPLE}
    NANOCLR_HOST_LINUX=${NANOCLR_HOST_LINUX}
    NANOCLR_HOST_WINDOWS=0
    NANO_POSIX_ENABLE_SMOKE=$<BOOL:${NANO_POSIX_ENABLE_SMOKE}>
    NANOCLR_POSIX_VERSION_STRING="${PROJECT_VERSION}"
    NANOCLR_PLATFORM_NAME="${NANOCLR_PLATFORM_NAME}"
)

# Hide all symbols by default; only NANOCLRNATIVE_API-marked symbols are exported.
target_compile_options(nanoFramework.nanoCLR PRIVATE -fvisibility=hidden)

# GCC (Linux) enables -Wmaybe-uninitialized with optimizations on CLR core code that
# is intentionally written to rely on NANOCLR_HEADER/NANOCLR_SET_AND_LEAVE macros.
# Suppress this warning to avoid false-positive errors, matching the pattern used by
# other embedded targets (e.g. ThreadX_EFM32GG11_GCC_options.cmake).
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(nanoFramework.nanoCLR PRIVATE -Wno-maybe-uninitialized)
endif()

set_target_properties(
    nanoFramework.nanoCLR
    PROPERTIES
    MACOSX_RPATH ON
    OUTPUT_NAME "nanoFramework.nanoCLR"
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# ─── Thin test harness (links against the shared library) ────────────────────
add_executable(nanoFramework.nanoCLR.test main.cpp)
target_compile_features(nanoFramework.nanoCLR.test PRIVATE cxx_std_20)
target_link_libraries(nanoFramework.nanoCLR.test PRIVATE nanoFramework.nanoCLR)
target_include_directories(
    nanoFramework.nanoCLR.test
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
    ${REPO_ROOT}/src/CLR/Include
    ${REPO_ROOT}/src/HAL/Include
    ${REPO_ROOT}/src/PAL/Include
)
target_compile_definitions(
    nanoFramework.nanoCLR.test
    PRIVATE
    PLATFORM_POSIX_HOST=1
    NANOCLR_HOST_POSIX=1
    NANOCLR_HOST_APPLE=${NANOCLR_HOST_APPLE}
    NANOCLR_HOST_LINUX=${NANOCLR_HOST_LINUX}
    NANOCLR_HOST_WINDOWS=0
    NANOCLR_POSIX_VERSION_STRING="${PROJECT_VERSION}"
    NANOCLR_PLATFORM_NAME="${NANOCLR_PLATFORM_NAME}"
)
set_target_properties(
    nanoFramework.nanoCLR.test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
