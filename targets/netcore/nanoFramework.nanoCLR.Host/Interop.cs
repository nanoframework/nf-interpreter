using System.Runtime.InteropServices;

namespace nanoFramework.nanoCLR.Host
{
    public class Interop
    {
        public const string NativeLibraryName = "nanoFramework.nanoCLR.Native";

        public delegate int ConfigureDelegate();

        public delegate void DebugPrintDelegate([MarshalAs(UnmanagedType.LPStr)] string message);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void NanoClr_Run(NanoClrSettings nanoClrSettings);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl, PreserveSig = false)]
        public static extern void NanoClr_ReferenceAssembly(string assemblyName, byte[] data, int length);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl, PreserveSig = false)]
        public static extern void NanoClr_ReferenceAssemblySet(byte[] data, int length);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl, PreserveSig = false)]
        public static extern void NanoClr_Resolve();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void NanoClr_SetConfigureCallback([MarshalAs(UnmanagedType.FunctionPtr)] ConfigureDelegate configureCallback);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void NanoClr_SetDebugPrintCallback([MarshalAs(UnmanagedType.FunctionPtr)] DebugPrintDelegate debugPrintCallback);
    }
}
