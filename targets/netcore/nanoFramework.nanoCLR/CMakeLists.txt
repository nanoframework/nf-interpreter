#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.25)

# ---------------------------------------------------------------------------
# Version handling — BUILD_VERSION comes from CI or caller.
# ---------------------------------------------------------------------------
if(NOT DEFINED BUILD_VERSION)
    set(BUILD_VERSION "9.99.999.9999")
endif()

string(REPLACE "." ";" _VERSION_PARTS "${BUILD_VERSION}")
list(LENGTH _VERSION_PARTS _VERSION_PARTS_LEN)

if(_VERSION_PARTS_LEN GREATER_EQUAL 1)
    list(GET _VERSION_PARTS 0 NBGV_VERSION_MAJOR)
else()
    set(NBGV_VERSION_MAJOR 9)
endif()
if(_VERSION_PARTS_LEN GREATER_EQUAL 2)
    list(GET _VERSION_PARTS 1 NBGV_VERSION_MINOR)
else()
    set(NBGV_VERSION_MINOR 99)
endif()
if(_VERSION_PARTS_LEN GREATER_EQUAL 3)
    list(GET _VERSION_PARTS 2 NBGV_BUILD_NUMBER)
else()
    set(NBGV_BUILD_NUMBER 999)
endif()
if(_VERSION_PARTS_LEN GREATER_EQUAL 4)
    list(GET _VERSION_PARTS 3 TARGET_BUILD_COUNTER)
else()
    set(TARGET_BUILD_COUNTER 9999)
endif()

project(nanoFramework_nanoCLR_native
    VERSION "${NBGV_VERSION_MAJOR}.${NBGV_VERSION_MINOR}.${NBGV_BUILD_NUMBER}"
    LANGUAGES C CXX
)

# ---------------------------------------------------------------------------
# Repository root — all src/** and targets/** paths resolve from here.
# ---------------------------------------------------------------------------
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)

# ---------------------------------------------------------------------------
# littlefs — external clone required on all platforms.
# The vcxproj uses the LITTLEFS_PATH environment variable.
# ---------------------------------------------------------------------------
if(NOT DEFINED LITTLEFS_PATH AND DEFINED ENV{LITTLEFS_PATH})
    set(LITTLEFS_PATH "$ENV{LITTLEFS_PATH}")
endif()
if(NOT DEFINED LITTLEFS_PATH)
    message(WARNING
        "LITTLEFS_PATH is not set. "
        "Set -DLITTLEFS_PATH=<path> to a littlefs repository clone. "
        "Build will proceed but littlefs sources will not be found.")
endif()

# ---------------------------------------------------------------------------
# Source files — faithfully mirrors the vcxproj <ClCompile> items.
# ---------------------------------------------------------------------------

# Sources local to targets/netcore/nanoFramework.nanoCLR/
set(NANOCLR_NETCORE_LOCAL_SOURCES
    CLRStartup.cpp
    Device_BlockStorage.cpp
    dllmain.cpp
    nanoCLR_native.cpp
    platform_BlockStorage.cpp
    Target_BlockStorage.cpp
    Target_BlockStorage_TransientFlashDriver.cpp
    target_FileSystem.cpp
    Target_HAL.cpp
    target_littlefs.cpp
    WireProtocol_HAL_Interface.cpp
)

# stdafx.cpp (PCH creator) — Windows/MSVC only.
if(WIN32)
    list(APPEND NANOCLR_NETCORE_LOCAL_SOURCES
        stdafx.cpp
    )
endif()

# Sources reused from targets/win32/nanoCLR/ (vcxproj references them
# via relative paths like ..\..\win32\nanoCLR\<file>).
set(NANOCLR_WIN32_SHARED_SOURCES
    ${REPO_ROOT}/targets/win32/nanoCLR/Various.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/base64.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/targetPAL_Events.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/FileStore_Win32.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/Generated/CLR_RT_InteropAssembliesTable.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/Memory.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/PAL/COMDirector_stubs.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/PAL/ConfigHelper_stubs.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/PAL/time_stubs.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/PAL/Watchdog_stubs.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/platform_heap.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/targetPAL_Time.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/targetRandom.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/targetHAL_Time.cpp
    ${REPO_ROOT}/targets/win32/nanoCLR/WatchDog.cpp
)

# Sources from src/ compiled directly (not via static-lib project references).
set(NANOCLR_SRC_SOURCES
    ${REPO_ROOT}/src/PAL/FileSystem/nanoPAL_FileSystem.cpp
    ${REPO_ROOT}/src/CLR/Core/NativeEventDispatcher/NativeEventDispatcher.cpp
    ${REPO_ROOT}/src/HAL/nanoHAL_Time.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_EventSink.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate.cpp
    ${REPO_ROOT}/src/PAL/AsyncProcCall/AsyncCompletions.cpp
    ${REPO_ROOT}/src/PAL/AsyncProcCall/AsyncContinuations.cpp
    ${REPO_ROOT}/src/PAL/Double/nanoPAL_NativeDouble.cpp
)

# ---------------------------------------------------------------------------
# Sources for libraries previously linked as static .lib via project
# references in nanoFramework.nanoCLR.vcxproj. We compile them straight
# into the DLL so we don't need to build/install separate .lib files.
#
# Mirrors the <ClCompile> entries in each *.vcxproj. Stub variants
# (Debugger_stub, Hardware_stub, Messaging_stub, RPC_stub,
# Diagnostics_stub, InterruptHandler_stub, NativeEventDispatcher_stub,
# Serialization_stub, FileStream_stub) are intentionally NOT included:
# in nanoCLR_native.cpp the "FEATURE_ALL" branch links the real libs,
# and the vcxproj marks the stub references with
# LinkLibraryDependencies=false.
# ---------------------------------------------------------------------------

# CLR Core (src/CLR/Core/Core.vcxproj) — minus NativeEventDispatcher.cpp,
# already in NANOCLR_SRC_SOURCES above.
set(NANOCLR_CLR_CORE_SOURCES
    ${REPO_ROOT}/src/CLR/Helpers/nanoprintf/nanoprintf.c
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
    ${REPO_ROOT}/src/CLR/Core/nanoSupport_CRC32.c
    ${REPO_ROOT}/src/CLR/Core/Streams.cpp
    ${REPO_ROOT}/src/CLR/Core/StringTable.cpp
    ${REPO_ROOT}/src/CLR/Core/Thread.cpp
    ${REPO_ROOT}/src/CLR/Core/TypeSystem.cpp
    ${REPO_ROOT}/src/CLR/Core/TypeSystemLookup.cpp
    ${REPO_ROOT}/src/CLR/Core/Various.cpp
)

# CLR static-lib companions (vcxproj per folder, all linked when
# NF_CLR_FEATURE_ALL is set).
set(NANOCLR_CLR_LIB_SOURCES
    ${REPO_ROOT}/src/CLR/Debugger/Debugger.cpp
    ${REPO_ROOT}/src/CLR/Debugger/Debugger_full.cpp
    ${REPO_ROOT}/src/CLR/Diagnostics/Info.cpp
    ${REPO_ROOT}/src/CLR/Diagnostics/Info_Safeprintf.cpp
    ${REPO_ROOT}/src/CLR/Diagnostics/Profile.cpp
    ${REPO_ROOT}/src/CLR/Diagnostics/Profiler.cpp
    ${REPO_ROOT}/src/CLR/Messaging/Messaging.cpp
    # targetHAL_time.cpp provides an extern "C" stub for
    # HAL_Time_CurrentSysTicks needed by the .c WireProtocol files.
    # The matching C++ definition (real impl) lives in
    # targets/win32/nanoCLR/targetHAL_Time.cpp — different linkage,
    # different mangled name, both are required.
    ${REPO_ROOT}/src/CLR/WireProtocol/targetHAL_time.cpp
    ${REPO_ROOT}/src/CLR/WireProtocol/WireProtocol_App_Interface.c
    # WireProtocol_HAL_Interface.c is intentionally NOT included — the
    # local netcore WireProtocol_HAL_Interface.cpp already provides the
    # WP_* symbols (with the actual host-side implementations using the
    # WireProtocolReceiveCallback / WireProtocolTransmitCallback hooks).
    ${REPO_ROOT}/src/CLR/WireProtocol/WireProtocol_Message.c
    ${REPO_ROOT}/src/CLR/WireProtocol/WireProtocol_MonitorCommands.c
    ${REPO_ROOT}/src/CLR/Core/Hardware/Hardware.cpp
    ${REPO_ROOT}/src/CLR/Core/InterruptHandler/InterruptHandler.cpp
    ${REPO_ROOT}/src/CLR/Core/RPC/CLR_RT_HeapBlock_EndPoint.cpp
    ${REPO_ROOT}/src/CLR/Core/Serialization/BinaryFormatter.cpp
    ${REPO_ROOT}/src/CLR/Core/FileStream/FileStream.cpp
    ${REPO_ROOT}/src/CLR/Startup/CLRStartup.cpp
    ${REPO_ROOT}/src/CLR/System.Math/nf_native_system_math.cpp
    ${REPO_ROOT}/src/CLR/System.Math/nf_native_system_math_System_Math.cpp
)

# CorLib (src/CLR/CorLib/CorLib.vcxproj).
set(NANOCLR_CLR_CORLIB_SOURCES
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_AppDomain.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Array.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Attribute.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_BitConverter.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Collections_ArrayList.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Convert.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_DateTime.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Delegate.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Diagnostics_Debug.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Diagnostics_Debugger.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Double.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Enum.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Exception.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_GC.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Globalization_CultureInfo.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Globalization_DateTimeFormat.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Guid.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_MarshalByRefObject.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_MathInternal.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_MulticastDelegate.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Number.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Object.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Random.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Reflection_Assembly.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Reflection_Binder.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Reflection_ConstructorInfo.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Reflection_FieldInfo.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Reflection_MemberInfo.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Reflection_MethodBase.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Reflection_PropertyInfo.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Reflection_RuntimeFieldInfo.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Reflection_RuntimeMethodInfo.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_RuntimeType.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Runtime_CompilerServices_RuntimeHelpers.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Runtime_Remoting_RemotingServices.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_String.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Threading_AutoResetEvent.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Threading_Interlocked.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Threading_ManualResetEvent.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Threading_Monitor.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Threading_SpinWait.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Threading_Thread.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Threading_Timer.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Threading_WaitHandle.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_TimeSpan.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_Type.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_ValueType.cpp
    ${REPO_ROOT}/src/CLR/CorLib/corlib_native_System_WeakReference.cpp
)

# nanoFramework / DeviceInterfaces / System.* sources, mirroring the
# remaining ProjectReference entries in nanoFramework.nanoCLR.vcxproj.
set(NANOCLR_FRAMEWORK_SOURCES
    # Networking.Sntp (stubs-only variant referenced by netcore vcxproj)
    ${REPO_ROOT}/src/DeviceInterfaces/Networking.Sntp/nf_networking_sntp.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/Networking.Sntp/nf_networking_sntp_nanoFramework_Networking_Sntp_stubs.cpp

    # System.Net (stubs-only variant)
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_IPAddress_stubs.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_IPGlobalProperties_stubs.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_NetworkInterface_stubs.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration_stubs.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration_stubs.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Security_CertificateManager_stubs.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Security_SslNative_stubs.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Sockets_NativeSocket_stubs.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2_stubs.cpp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net/sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate_stubs.cpp

    # nanoFramework.ResourceManager
    ${REPO_ROOT}/src/nanoFramework.ResourceManager/nf_system_resourcemanager.cpp
    ${REPO_ROOT}/src/nanoFramework.ResourceManager/nf_system_resourcemanager_System_Resources_ResourceManager.cpp
    ${REPO_ROOT}/src/nanoFramework.ResourceManager/nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility.cpp

    # nanoFramework.Runtime.Native
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_System_Environment.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_ExecutionConstraint.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_GC.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_Power.cpp
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_Rtc_stubs.cpp

    # nanoFramework.System.Collections
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Hashtable.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Hashtable__HashtableEnumerator.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Queue.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Stack.cpp

    # nanoFramework.System.IO.Hashing
    ${REPO_ROOT}/src/nanoFramework.System.IO.Hashing/nf_sys_io_hashing.cpp
    ${REPO_ROOT}/src/nanoFramework.System.IO.Hashing/nf_sys_io_hashing_System_IO_Hashing_Crc32_stubs.cpp

    # nanoFramework.System.Text
    ${REPO_ROOT}/src/nanoFramework.System.Text/nf_system_text.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Text/nf_system_text_System_Text_UTF8Decoder.cpp
    ${REPO_ROOT}/src/nanoFramework.System.Text/nf_system_text_System_Text_UTF8Encoding.cpp

    # System.Device.Gpio (stubs-only variant)
    ${REPO_ROOT}/src/System.Device.Gpio/sys_dev_gpio_native.cpp
    ${REPO_ROOT}/src/System.Device.Gpio/sys_dev_gpio_native_System_Device_Gpio_GpioController_stubs.cpp
    ${REPO_ROOT}/src/System.Device.Gpio/sys_dev_gpio_native_System_Device_Gpio_GpioPin_stubs.cpp

    # System.Device.I2c (stubs-only variant)
    ${REPO_ROOT}/src/System.Device.I2c/sys_dev_i2c_native.cpp
    ${REPO_ROOT}/src/System.Device.I2c/sys_dev_i2c_native_System_Device_I2c_I2cDevice_stubs.cpp

    # System.Device.Spi (stubs-only variant)
    ${REPO_ROOT}/src/System.Device.Spi/sys_dev_spi_native.cpp
    ${REPO_ROOT}/src/System.Device.Spi/sys_dev_spi_native_System_Device_Spi_SpiBusInfo_stubs.cpp
    ${REPO_ROOT}/src/System.Device.Spi/sys_dev_spi_native_System_Device_Spi_SpiDevice_stubs.cpp

    # System.IO.FileSystem (real variant — netcore-only)
    ${REPO_ROOT}/src/System.IO.FileSystem/nf_sys_io_filesystem.cpp
    ${REPO_ROOT}/src/System.IO.FileSystem/nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard_stubs.cpp
    ${REPO_ROOT}/src/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_Directory.cpp
    ${REPO_ROOT}/src/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_DriveInfo.cpp
    ${REPO_ROOT}/src/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_NativeFileStream.cpp
    ${REPO_ROOT}/src/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_NativeFindFile.cpp
    ${REPO_ROOT}/src/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_NativeIO.cpp

    # System.IO.Ports (stubs-only variant)
    ${REPO_ROOT}/src/System.IO.Ports/sys_io_ser_native.cpp
    ${REPO_ROOT}/src/System.IO.Ports/sys_io_ser_native_System_IO_Ports_SerialPort_stubs.cpp

    # System.Runtime.Serialization
    ${REPO_ROOT}/src/System.Runtime.Serialization/nf_system_runtime_serialization.cpp
    ${REPO_ROOT}/src/System.Runtime.Serialization/nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter.cpp
)

# littlefs (targets/netcore/littlefs/littlefs.vcxproj). The two lfs.c
# files come from the external clone pointed to by LITTLEFS_PATH.
set(NANOCLR_LITTLEFS_SOURCES
    ${REPO_ROOT}/targets/netcore/littlefs/hal_littlefs.c
    ${REPO_ROOT}/targets/netcore/littlefs/littlefs_FS_Driver.cpp
)
if(DEFINED LITTLEFS_PATH)
    list(APPEND NANOCLR_LITTLEFS_SOURCES
        ${LITTLEFS_PATH}/lfs.c
        ${LITTLEFS_PATH}/lfs_util.c
    )
endif()

set_source_files_properties(
    ${REPO_ROOT}/src/CLR/WireProtocol/targetHAL_time.cpp
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)

# ---------------------------------------------------------------------------
# SHARED library target — produces:
#   Windows: nanoFramework.nanoCLR.dll
#   macOS:   nanoFramework.nanoCLR.dylib
#   Linux:   nanoFramework.nanoCLR.so
# ---------------------------------------------------------------------------
add_library(nanoFramework.nanoCLR SHARED
    ${NANOCLR_NETCORE_LOCAL_SOURCES}
    ${NANOCLR_WIN32_SHARED_SOURCES}
    ${NANOCLR_SRC_SOURCES}
    ${NANOCLR_CLR_CORE_SOURCES}
    ${NANOCLR_CLR_LIB_SOURCES}
    ${NANOCLR_CLR_CORLIB_SOURCES}
    ${NANOCLR_FRAMEWORK_SOURCES}
    ${NANOCLR_LITTLEFS_SOURCES}
)

set_target_properties(nanoFramework.nanoCLR PROPERTIES
    OUTPUT_NAME "nanoFramework.nanoCLR"
    PREFIX ""                                         # no "lib" prefix on any platform
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" # DLLs go to RUNTIME on Windows
)

# ---------------------------------------------------------------------------
# C++ / C standard — matches nf.props: stdcpp20 / stdc17
# ---------------------------------------------------------------------------
target_compile_features(nanoFramework.nanoCLR PRIVATE cxx_std_20)
set_target_properties(nanoFramework.nanoCLR PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
)

# ---------------------------------------------------------------------------
# Include directories — union of vcxproj + nf.props, de-duplicated.
# ---------------------------------------------------------------------------
target_include_directories(nanoFramework.nanoCLR PRIVATE
    # Local / platform headers
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${REPO_ROOT}/targets/win32/Include

    # netcore self-reference (nf.props adds this at multiple depths)
    ${REPO_ROOT}/targets/netcore/nanoFramework.nanoCLR

    # CLR / HAL / PAL / Runtime headers
    ${REPO_ROOT}/src/CLR/Core
    ${REPO_ROOT}/src/CLR/CorLib
    ${REPO_ROOT}/src/CLR/Helpers/Base64
    ${REPO_ROOT}/src/CLR/Helpers/nanoprintf
    ${REPO_ROOT}/src/CLR/Include
    ${REPO_ROOT}/src/HAL/Include
    ${REPO_ROOT}/src/PAL/Include
    ${REPO_ROOT}/src/nanoFramework.Runtime.Events

    # Headers from each previously-static-lib folder. Each lib compiled
    # in isolation in MSBuild, so its .cpp files include their own
    # headers via a bare path (e.g. "Hardware.h"). When we compile the
    # same .cpp files from the root target, we have to put these dirs
    # on the include path.
    ${REPO_ROOT}/src/CLR/Debugger
    ${REPO_ROOT}/src/CLR/Diagnostics
    ${REPO_ROOT}/src/CLR/Messaging
    ${REPO_ROOT}/src/CLR/Startup
    ${REPO_ROOT}/src/CLR/System.Math
    ${REPO_ROOT}/src/CLR/WireProtocol
    ${REPO_ROOT}/src/CLR/Core/FileStream
    ${REPO_ROOT}/src/CLR/Core/Hardware
    ${REPO_ROOT}/src/CLR/Core/InterruptHandler
    ${REPO_ROOT}/src/CLR/Core/NativeEventDispatcher
    ${REPO_ROOT}/src/CLR/Core/RPC
    ${REPO_ROOT}/src/CLR/Core/Serialization
    ${REPO_ROOT}/src/DeviceInterfaces/Networking.Sntp
    ${REPO_ROOT}/src/DeviceInterfaces/System.Net
    ${REPO_ROOT}/src/nanoFramework.ResourceManager
    ${REPO_ROOT}/src/nanoFramework.Runtime.Native
    ${REPO_ROOT}/src/nanoFramework.System.Collections
    ${REPO_ROOT}/src/nanoFramework.System.IO.Hashing
    ${REPO_ROOT}/src/nanoFramework.System.Text
    ${REPO_ROOT}/src/System.Device.Gpio
    ${REPO_ROOT}/src/System.Device.I2c
    ${REPO_ROOT}/src/System.Device.Spi
    ${REPO_ROOT}/src/System.IO.FileSystem
    ${REPO_ROOT}/src/System.IO.Ports
    ${REPO_ROOT}/src/System.Runtime.Serialization

    # littlefs — local config header + external repo
    ${REPO_ROOT}/targets/netcore/littlefs
)

if(DEFINED LITTLEFS_PATH)
    target_include_directories(nanoFramework.nanoCLR PRIVATE
        ${LITTLEFS_PATH}
    )
endif()

# ---------------------------------------------------------------------------
# Preprocessor definitions — vcxproj + nf.props, config-aware.
# ---------------------------------------------------------------------------
target_compile_definitions(nanoFramework.nanoCLR PRIVATE
    # From nf.props — always set
    VIRTUAL_DEVICE
    SUPPORT_ANY_BASE_CONVERSION
    NANOCLR_REFLECTION=TRUE
    NANOCLR_SYSTEM_COLLECTIONS=TRUE
    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    LFS_CONFIG=lfs_config.h
    VERSION_MAJOR=${NBGV_VERSION_MAJOR}
    VERSION_MINOR=${NBGV_VERSION_MINOR}
    VERSION_BUILD=${NBGV_BUILD_NUMBER}
    VERSION_REVISION=${TARGET_BUILD_COUNTER}
    $<IF:$<PLATFORM_ID:Windows>,PLATFORMNAMESTRING="WINDOWS",PLATFORMNAMESTRING="OSX">

    # From vcxproj — DLL export macro
    NANOCLRNATIVE_EXPORTS
    _CONSOLE
    VERSION_STRING="${BUILD_VERSION}"

    # Config-dependent
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Debug>:DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# ---------------------------------------------------------------------------
# Platform-specific compiler/linker flags
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(nanoFramework.nanoCLR PRIVATE
        /W3            # WarningLevel = Level3
        /EHsc          # C++ exception handling
        /utf-8         # Unicode charset
        $<$<NOT:$<CONFIG:Debug>>:/Gy>   # FunctionLevelLinking
        $<$<NOT:$<CONFIG:Debug>>:/Oi>   # IntrinsicFunctions
        $<$<NOT:$<CONFIG:Debug>>:/GL>   # WholeProgramOptimization
    )

    target_link_options(nanoFramework.nanoCLR PRIVATE
        /SUBSYSTEM:WINDOWS              # vcxproj uses Windows subsystem (not Console)
        $<$<NOT:$<CONFIG:Debug>>:/OPT:ICF>   # EnableCOMDATFolding
        $<$<NOT:$<CONFIG:Debug>>:/OPT:REF>   # OptimizeReferences
        $<$<NOT:$<CONFIG:Debug>>:/LTCG>      # Link-time code generation
        $<$<CONFIG:Debug>:/INCREMENTAL>      # Debug incremental linking

        # nanoCLR_native.cpp uses #pragma comment(lib, "<Name>.lib") to pull in
        # the static libs that the MSBuild solution builds via project
        # references. In this CMake build we compile the same sources directly
        # into the DLL instead, so tell the linker to ignore those default-lib
        # directives.
        /NODEFAULTLIB:WireProtocol.lib
        /NODEFAULTLIB:Debugger.lib
        /NODEFAULTLIB:Diagnostics.lib
        /NODEFAULTLIB:Hardware.lib
        /NODEFAULTLIB:InterruptHandler.lib
        /NODEFAULTLIB:Messaging.lib
        /NODEFAULTLIB:RPC.lib
        /NODEFAULTLIB:Serialization.lib
    )

    # Precompiled header — stdafx.h / stdafx.cpp. Restrict to C++ TUs:
    # stdafx.h pulls in <mutex>, base64.h (C++), etc., so it can't be
    # consumed by the .c sources (nanoprintf.c, nanoSupport_CRC32.c,
    # WireProtocol_*.c, hal_littlefs.c, lfs.c, lfs_util.c).
    target_precompile_headers(nanoFramework.nanoCLR PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h>"
    )
endif()

if(APPLE)
    # Symbols are exported via extern "C" / __attribute__((visibility("default")))
    # CMake default visibility for SHARED is already correct on macOS.
    set_target_properties(nanoFramework.nanoCLR PROPERTIES
        MACOSX_RPATH ON
    )
endif()

# ---------------------------------------------------------------------------
# Library dependencies
#
# The vcxproj uses ProjectReferences to static-library .vcxproj projects
# and #pragma comment(lib, ...) in nanoCLR_native.cpp.
# When those libraries are ported to CMake targets, link them here.
# ---------------------------------------------------------------------------

# On Windows, kernel32 is linked via #pragma comment(lib, ...) in stdafx.h.
# Make it explicit for CMake:
if(WIN32)
    target_link_libraries(nanoFramework.nanoCLR PRIVATE kernel32)
endif()

# Project references that ARE linked (LinkLibraryDependencies != false):
# TODO: Replace with CMake target names once each library has a CMakeLists.txt
#   - Core              (src/CLR/Core/Core.vcxproj)
#   - FileStream        (src/CLR/Core/FileStream/FileStream.vcxproj)           ** netcore-only **
#   - Hardware          (src/CLR/Core/Hardware/Hardware.vcxproj)
#   - InterruptHandler  (src/CLR/Core/InterruptHandler/InterruptHandler.vcxproj)
#   - RPC               (src/CLR/Core/RPC/RPC.vcxproj)
#   - Serialization     (src/CLR/Core/Serialization/Serialization.vcxproj)
#   - CorLib            (src/CLR/CorLib/CorLib.vcxproj)
#   - Debugger          (src/CLR/Debugger/Debugger.vcxproj)
#   - Diagnostics       (src/CLR/Diagnostics/Diagnostics.vcxproj)
#   - Messaging         (src/CLR/Messaging/Messaging.vcxproj)
#   - StartUp           (src/CLR/StartUp/StartUp.vcxproj)
#   - System.Math       (src/CLR/System.Math/System.Math.vcxproj)
#   - WireProtocol      (src/CLR/WireProtocol/WireProtocol.vcxproj)
#   - Networking.Sntp   (src/DeviceInterfaces/Networking.Sntp/Networking.Sntp.vcxproj)
#   - System.Net        (src/DeviceInterfaces/System.Net/System.Net.vcxproj)
#   - nanoFramework.ResourceManager
#   - Runtime.Native
#   - System.Collections
#   - System.IO.Hashing
#   - System.Text
#   - System.Device.Gpio
#   - System.Device.I2c
#   - System.Device.Spi
#   - System.IO.FileSystem  (src/System.IO.FileSystem/System.IO.FileSystem.vcxproj)  ** netcore-only **
#   - System.IO.Ports
#   - System.Runtime.Serialization
#   - littlefs           (targets/netcore/littlefs/littlefs.vcxproj)              ** netcore-only **

# Stubs referenced but NOT linked (LinkLibraryDependencies=false):
#   - InterruptHandler_stub
#   - NativeEventDispatcher_stub
#   - RPC_stub
#   - Debugger_stub
#   - Diagnostics_stub
#   - Messaging_stub
