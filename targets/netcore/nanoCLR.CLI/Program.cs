using System;
using System.Runtime.InteropServices;

namespace nanoCLR.CLI
{
    class Program
    {
        public const string nativeLibraryName = "nanoFramework.nanoCLR.Native";

        [DllImport(nativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        static extern void nanoclr_run(int argc, [MarshalAsAttribute (UnmanagedType.LPArray, ArraySubType=UnmanagedType.LPWStr)] string[] argv);

        static void Main(string[] args)
        {
            nanoclr_run(args.Length, args);
        }
    }
}
