//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Runtime.InteropServices;

namespace nanoFramework.nanoCLR.Host
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct nanoCLRSettings
    {
        public ushort MaxContextSwitches { get; set; }

        public bool WaitForDebugger { get; set; }

        public bool EnterDebuggerLoopAfterExit { get; set; }

        public bool PerformGarbageCollection { get; set; }

        public bool PerformHeapCompaction { get; set; }

        public static nanoCLRSettings Default = new()
        {
            MaxContextSwitches = 50,
            WaitForDebugger = false,
            EnterDebuggerLoopAfterExit = false,
            PerformGarbageCollection = false,
            PerformHeapCompaction = false,
        };
    }
}
