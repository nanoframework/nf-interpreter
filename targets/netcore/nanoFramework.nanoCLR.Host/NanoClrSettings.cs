using System.Runtime.InteropServices;

namespace nanoFramework.nanoCLR.Host
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct NanoClrSettings
    {
        public ushort MaxContextSwitches { get; set; }

        public bool WaitForDebugger { get; set; }

        public bool EnterDebuggerLoopAfterExit { get; set; }

        public static NanoClrSettings Default = new()
        {
            MaxContextSwitches = 50,
            WaitForDebugger = false,
            EnterDebuggerLoopAfterExit = false,
        };
    }
}
