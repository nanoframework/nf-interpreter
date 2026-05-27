////
// Copyright (c) .NET Foundation and Contributors.
// See LICENSE file in the project root for full license information.
////

using System;

namespace nanoFramework.Tools.NanoProfiler.Extensions
{
    public static class ProfilerCommandExtensions
    {
        public static string ToCommandName(this uint type)
        {
            switch (type)
            {
                case Packets.Commands.c_Profiling_Timestamp:
                    return "Timestamp";

                case Packets.Commands.c_Profiling_Memory_Layout:
                    return "Memory Layout";

                case Packets.Commands.c_Profiling_HeapDump_Start:
                    return "HeapDump Start";

                case Packets.Commands.c_Profiling_HeapDump_Stop:
                    return "HeapDump Stop";

                case Packets.Commands.c_Profiling_HeapDump_Root:
                    return "HeapDump Root";

                case Packets.Commands.c_Profiling_HeapDump_Object:
                    return "HeapDump Object";

                case Packets.Commands.c_Profiling_Calls_Call:
                    return "Calls Call";

                case Packets.Commands.c_Profiling_Calls_Return:
                    return "Calls Return";

                case Packets.Commands.c_Profiling_Calls_CtxSwitch:
                    return "Calls CtxSwitch";

                case Packets.Commands.c_Profiling_Allocs_Alloc:
                    return "Allocs Alloc";

                case Packets.Commands.c_Profiling_Allocs_Relloc:
                    return "Allocs Relloc";

                case Packets.Commands.c_Profiling_Allocs_Delete:
                    return "Allocs Delete";

                case Packets.Commands.c_Profiling_GarbageCollect_Begin:
                    return "GC Begin";

                case Packets.Commands.c_Profiling_GarbageCollect_End:
                    return "GC End";

                case Packets.Commands.c_Profiling_HeapCompact_Begin:
                    return "HeapCompaction Begin";

                case Packets.Commands.c_Profiling_HeapCompact_End:
                    return "HeapCompaction End";

                default:
                    throw new ApplicationException("Unable to decode Command type.");
            }
        }
    }
}
