using System;
using System.Runtime.InteropServices;

namespace nanoFramework.nanoCLR.Host
{
    public class Interop
    {
        public const string NativeLibraryName = "nanoFramework.nanoCLR.Native";

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void nanoclr_run(int argc, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)] string[] argv);
    }
}
