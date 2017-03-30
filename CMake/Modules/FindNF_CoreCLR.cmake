#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework Core, CoreLib and CLR startup

# include directories for Core
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)

# include directories for CoreLib
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/CorLib)

# CLR startup
list(APPEND NF_CoreCLR_INCLUDE_DIRS  ${PROJECT_SOURCE_DIR}/src/CLR/Startup)

# others
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Diagnostics)
list(APPEND NF_CoreCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Debugger)


# source files for nanoFramework Core, CoreLib and CLR startup
set(NF_CoreCLR_SRCS

    # Core
    Cache.cpp
    Checks.cpp
    CLR_RT_DblLinkedList.cpp
    CLR_RT_HeapBlock.cpp
    CLR_RT_HeapBlock_Array.cpp
    CLR_RT_HeapBlock_BinaryBlob.cpp
    CLR_RT_HeapBlock_Delegate.cpp
    CLR_RT_HeapBlock_Delegate_List.cpp
    CLR_RT_HeapBlock_Finalizer.cpp
    CLR_RT_HeapBlock_Lock.cpp
    CLR_RT_HeapBlock_LockRequest.cpp
    CLR_RT_HeapBlock_Node.cpp
    CLR_RT_HeapBlock_Stack.cpp
    CLR_RT_HeapBlock_String.cpp
    CLR_RT_HeapBlock_Timer.cpp
    CLR_RT_HeapBlock_WaitForObject.cpp
    CLR_RT_HeapCluster.cpp
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
    Interpreter.cpp
    Streams.cpp
    StringTable.cpp
    Thread.cpp
    TypeSystemLookup.cpp
    StringTableData.cpp
    TypeSystem.cpp
    nanoSupport_CRC32.c
    nanoHAL_SystemInformation.cpp
    
    # TODO FIXME need to investigate how this file can be automated 
    CLR_RT_InteropAssembliesTable.cpp

    # CoreLib
    CorLib_Native_System_DateTime.cpp
    CorLib_Native_System_Exception.cpp
    CorLib_Native_System_String.cpp
    CorLib_Native_System_Threading_Thread.cpp
    CorLib_Native_System_TimeSpan.cpp

    # CLR startup
    CLRStartup.cpp

    # Core stubs
    Hardware_stub.cpp
    Heap_Persistence_stub.cpp
    InterruptHandler_stub.cpp
    IOPort_stub.cpp
    RPC_stub.cpp
    BinaryFormatter_stub.cpp

    # CLR stubs
    Debugger_stub.cpp
    Diagnostics_stub.cpp
    Messaging_stub.cpp
    
    # HAL
    HAL_Time.cpp

    # PAL
    nanoPAL_BlockStorage.c

    # PAL stubs
    COM_stubs.c

)

foreach(SRC_FILE ${NF_CoreCLR_SRCS})
    set(NF_CoreCLR_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(NF_CoreCLR_SRC_FILE ${SRC_FILE}
        PATHS 
            
            # Core
            ${PROJECT_SOURCE_DIR}/src/CLR/Core

            # CoreLib
            ${PROJECT_SOURCE_DIR}/src/CLR/CorLib

            # CLR startup
            ${PROJECT_SOURCE_DIR}/src/CLR/Startup

            # Core stubs
            ${PROJECT_SOURCE_DIR}/src/CLR/Core/Hardware
            ${PROJECT_SOURCE_DIR}/src/CLR/Core/HeapPersistence
            ${PROJECT_SOURCE_DIR}/src/CLR/Core/InterruptHandler
            ${PROJECT_SOURCE_DIR}/src/CLR/Core/IOPort
            ${PROJECT_SOURCE_DIR}/src/CLR/Core/RPC
            ${PROJECT_SOURCE_DIR}/src/CLR/Core/Serialization

            # CLR stubs
            ${PROJECT_SOURCE_DIR}/src/CLR/Debugger
            ${PROJECT_SOURCE_DIR}/src/CLR/Diagnostics
            ${PROJECT_SOURCE_DIR}/src/CLR/Messaging
            
            # HAL
            ${PROJECT_SOURCE_DIR}/src/HAL

            # PAL
            ${PROJECT_SOURCE_DIR}/src/PAL
            ${PROJECT_SOURCE_DIR}/src/PAL/BlockStorage

            # PAL stubs
            ${PROJECT_SOURCE_DIR}/src/PAL/COM

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${NF_CoreCLR_SRC_FILE}") # debug helper
    list(APPEND NF_CoreCLR_SOURCES ${NF_CoreCLR_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_CoreCLR DEFAULT_MSG NF_CoreCLR_INCLUDE_DIRS NF_CoreCLR_SOURCES)
