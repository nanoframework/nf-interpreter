#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework Core, CoreLib and CLR startup

# include directories for Core
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Core)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Include)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/HAL/Include)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Include)

# include directories for CoreLib
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/CorLib)

# CLR startup
list(APPEND NF_CoreCLR_INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/src/CLR/Startup)

# others
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_include)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Diagnostics)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Debugger)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/nanoprintf)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/Base64)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Collections)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp)

list(APPEND NF_CoreCLR_INCLUDE_DIRS ${TARGET_BASE_LOCATION})
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${TARGET_BASE_LOCATION}/nanoCLR)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD})
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR)

# source files for nanoFramework Core, CoreLib and CLR startup
set(NF_CoreCLR_SRCS

    # Core
    Cache.cpp
    Checks.cpp
    CLR_RT_DblLinkedList.cpp
    CLR_RT_HeapBlock.cpp
    CLR_RT_HeapBlock_Array.cpp
    CLR_RT_HeapBlock_ArrayList.cpp
    CLR_RT_HeapBlock_BinaryBlob.cpp
    CLR_RT_HeapBlock_Delegate.cpp
    CLR_RT_HeapBlock_Delegate_List.cpp
    CLR_RT_HeapBlock_Finalizer.cpp
    CLR_RT_HeapBlock_Lock.cpp
    CLR_RT_HeapBlock_LockRequest.cpp
    CLR_RT_HeapBlock_Node.cpp
    CLR_RT_HeapBlock_String.cpp
    CLR_RT_HeapBlock_Timer.cpp
    CLR_RT_HeapBlock_WaitForObject.cpp
    CLR_RT_HeapCluster.cpp
    CLR_RT_Interop.cpp
    CLR_RT_Memory.cpp
    CLR_RT_ObjectToEvent_Destination.cpp
    CLR_RT_ObjectToEvent_Source.cpp
    CLR_RT_RuntimeMemory.cpp
    CLR_RT_StackFrame.cpp
    CLR_RT_SystemAssembliesTable.cpp
    CLR_RT_UnicodeHelper.cpp
    Core.cpp
    Execution.cpp
    GarbageCollector.cpp
    GarbageCollector_Compaction.cpp
    GarbageCollector_ComputeReachabilityGraph.cpp
    GarbageCollector_Info.cpp
    Interpreter.cpp
    Random.cpp
    Streams.cpp
    StringTable.cpp
    Thread.cpp
    TypeSystemLookup.cpp
    StringTableData.cpp
    TypeSystem.cpp
    nanoHAL_SystemInformation.cpp
    Various.cpp

    # CoreLib
    corlib_native_System_AppDomain.cpp
    corlib_native_System_Array.cpp
    corlib_native_System_Attribute.cpp
    corlib_native_System_BitConverter.cpp
    corlib_native_System_Collections_ArrayList.cpp
    corlib_native_System_Convert.cpp
    corlib_native_System_DateTime.cpp
    corlib_native_System_Delegate.cpp
    corlib_native_System_Diagnostics_Debug.cpp
    corlib_native_System_Diagnostics_Debugger.cpp
    corlib_native_System_Double.cpp
    corlib_native_System_Enum.cpp
    corlib_native_System_Exception.cpp
    corlib_native_System_GC.cpp
    corlib_native_System_Globalization_CultureInfo.cpp
    corlib_native_System_Globalization_DateTimeFormat.cpp
    corlib_native_System_Guid.cpp
    corlib_native_System_MarshalByRefObject.cpp
    corlib_native_System_MathInternal.cpp
    corlib_native_System_MulticastDelegate.cpp
    corlib_native_System_Number.cpp
    corlib_native_System_Object.cpp
    corlib_native_System_Random.cpp
    corlib_native_System_Reflection_Binder.cpp
    corlib_native_System_Reflection_MemberInfo.cpp
    corlib_native_System_Runtime_CompilerServices_RuntimeHelpers.cpp
    corlib_native_System_Runtime_Remoting_RemotingServices.cpp
    corlib_native_System_String.cpp
    corlib_native_System_Threading_AutoResetEvent.cpp
    corlib_native_System_Threading_Interlocked.cpp
    corlib_native_System_Threading_ManualResetEvent.cpp
    corlib_native_System_Threading_Monitor.cpp
    corlib_native_System_Threading_SpinWait.cpp
    corlib_native_System_Threading_Thread.cpp
    corlib_native_System_Threading_Timer.cpp
    corlib_native_System_Threading_WaitHandle.cpp
    corlib_native_System_TimeSpan.cpp
    corlib_native_System_ValueType.cpp
    corlib_native_System_WeakReference.cpp
    corlib_native.cpp

    # CLR startup
    CLRStartup.cpp

    # Runtime.Native
    nf_rt_native.cpp
    nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo.cpp
    nf_rt_native_nanoFramework_Runtime_Native_GC.cpp
    nf_rt_native_nanoFramework_Runtime_Native_ExecutionConstraint.cpp
    nf_rt_native_nanoFramework_Runtime_Native_Power.cpp
    nf_rt_native_nanoFramework_Runtime_Native_Rtc_stubs.cpp
    nf_rt_native_System_Environment.cpp
    
    # Core stubs
    RPC_stub.cpp
    BinaryFormatter_stub.cpp

    # CLR stubs
    Debugger_stub.cpp
    
    # Helpers
    nanoprintf.c

    # HAL
    nanoHAL_Time.cpp
    nanoHAL_Watchdog.c
    nanoHAL_SystemEvents.c
    
    # PAL
    nanoPAL_Events.cpp
    nanoPAL_NativeDouble.cpp
    nanoPAL_Network_stubs.cpp
    nanoPAL_PerformanceCounters_stubs.cpp

    # PAL stubs
    Async_stubs.cpp
    COM_stubs.c
    GenericPort_stubs.c
)

# append CRC32, if not already included with Wire Protocol
if(NOT WireProtocol_FOUND)
    list(APPEND NF_CoreCLR_SRCS nanoSupport_CRC32.c)
endif()

# include System.Reflection API files depending on build option
if(NF_FEATURE_SUPPORT_REFLECTION)
    list(APPEND NF_CoreCLR_SRCS corlib_native_System_Reflection_Assembly.cpp)
    list(APPEND NF_CoreCLR_SRCS corlib_native_System_Reflection_ConstructorInfo.cpp)
    list(APPEND NF_CoreCLR_SRCS corlib_native_System_Reflection_FieldInfo.cpp)
    list(APPEND NF_CoreCLR_SRCS corlib_native_System_Reflection_MethodBase.cpp)
    list(APPEND NF_CoreCLR_SRCS corlib_native_System_Reflection_PropertyInfo.cpp)
    list(APPEND NF_CoreCLR_SRCS corlib_native_System_Reflection_RuntimeFieldInfo.cpp)
    list(APPEND NF_CoreCLR_SRCS corlib_native_System_Reflection_RuntimeMethodInfo.cpp)
    list(APPEND NF_CoreCLR_SRCS corlib_native_System_RuntimeType.cpp)
    list(APPEND NF_CoreCLR_SRCS corlib_native_System_Type.cpp)
endif()

# include Collection support files depending on build option
if(API_nanoFramework.System.Collections)
    list(APPEND NF_CoreCLR_SRCS CLR_RT_HeapBlock_Queue.cpp)
    list(APPEND NF_CoreCLR_SRCS CLR_RT_HeapBlock_Stack.cpp)
endif()

# need a conditional include because of ESP32 building network as a library 
if(NOT USE_SECURITY_MBEDTLS_OPTION)
    list(APPEND NF_CoreCLR_SRCS base64.c)
endif()

# include configuration manager file
if(NF_FEATURE_HAS_CONFIG_BLOCK)
    # feature enabled, full support
    list(APPEND NF_CoreCLR_SRCS nanoHAL_ConfigurationManager.c)
else()
    # feature disabled, stubs only
    list(APPEND NF_CoreCLR_SRCS nanoHAL_ConfigurationManager_stubs.c)
endif()

foreach(SRC_FILE ${NF_CoreCLR_SRCS})

    set(NF_CoreCLR_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(NF_CoreCLR_SRC_FILE ${SRC_FILE}
        PATHS 
            
            # Core
            ${CMAKE_SOURCE_DIR}/src/CLR/Core

            # CoreLib
            ${CMAKE_SOURCE_DIR}/src/CLR/CorLib

            # CLR startup
            ${CMAKE_SOURCE_DIR}/src/CLR/Startup

            # Runtime.Native
            ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native

            # Core stubs
            ${CMAKE_SOURCE_DIR}/src/CLR/Core/Hardware
            ${CMAKE_SOURCE_DIR}/src/CLR/Core/InterruptHandler
            ${CMAKE_SOURCE_DIR}/src/CLR/Core/NativeEventDispatcher
            ${CMAKE_SOURCE_DIR}/src/CLR/Core/RPC
            ${CMAKE_SOURCE_DIR}/src/CLR/Core/Serialization

            # CLR stubs
            ${CMAKE_SOURCE_DIR}/src/CLR/Debugger
            ${CMAKE_SOURCE_DIR}/src/CLR/Diagnostics
            ${CMAKE_SOURCE_DIR}/src/CLR/Messaging
            
            # Helpers
            ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/nanoprintf
            ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/Base64

            # HAL
            ${CMAKE_SOURCE_DIR}/src/HAL

            # PAL
            ${CMAKE_SOURCE_DIR}/src/PAL
            ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage
            ${CMAKE_SOURCE_DIR}/src/PAL/Double
            ${CMAKE_SOURCE_DIR}/src/PAL/Events

            # PAL stubs
            ${CMAKE_SOURCE_DIR}/src/PAL/AsyncProcCall
            ${CMAKE_SOURCE_DIR}/src/PAL/COM
            ${CMAKE_SOURCE_DIR}/src/PAL/Profiler

            # target
            ${TARGET_BASE_LOCATION}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${NF_CoreCLR_SRC_FILE}")
    endif()

    list(APPEND NF_CoreCLR_SOURCES ${NF_CoreCLR_SRC_FILE})
    
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_CoreCLR DEFAULT_MSG NF_CoreCLR_INCLUDE_DIRS NF_CoreCLR_SOURCES)


# macro to be called from binutils to add Core CLR library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_add_lib_coreclr)

    # parse arguments
    cmake_parse_arguments(NFALC "" "" "EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})

    # add this has a library
    set(LIB_NAME NF_CoreCLR)

    add_library(
        ${LIB_NAME} STATIC 
            ${NF_CoreCLR_SOURCES}
            ${NF_Diagnostics_SOURCES})   

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            ${NF_CoreCLR_INCLUDE_DIRS}
            ${NFALC_EXTRA_INCLUDES})   

    # TODO can be removed later
    if(RTOS_ESP32_CHECK)

        nf_common_compiler_definitions(TARGET ${LIB_NAME} BUILD_TARGET ${NANOCLR_PROJECT_NAME})

        # this is the only one different
        target_compile_definitions(
            ${LIB_NAME} PUBLIC
            -DPLATFORM_ESP32
            ${NFALC_EXTRA_COMPILER_DEFINITIONS}
        )

    else() 
        nf_set_compile_options(TARGET ${LIB_NAME} BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_compile_definitions(TARGET ${LIB_NAME} EXTRA_COMPILE_DEFINITIONS ${NFALC_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_link_options(TARGET ${LIB_NAME})
    endif()

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
