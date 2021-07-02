using System.Runtime.InteropServices;
using System.Xml;

namespace nanoFramework.nanoCLR.Host
{
    [StructLayout(LayoutKind.Sequential)]
    public struct NanoClrSettings
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