//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Runtime.InteropServices;

namespace nanoFramework.nanoCLR.Host.Interop
{
    internal class Native
    {
        internal const uint ClrOk = 0;
        internal const uint ClrErrorFail = 0xFF000000;
        private const string NativeLibraryName = "nanoFramework.nanoCLR.Native";

        internal delegate uint ConfigureDelegate();

        internal delegate void DebugPrintDelegate([MarshalAs(UnmanagedType.LPStr)] string message);

        internal delegate void WireTransmitDelegate([MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)]
            byte[] data, int length);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void NanoClr_Run(NanoClrSettings nanoClrSettings);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl,
            PreserveSig = false)]
        internal static extern void NanoClr_LoadAssembly(string assemblyName, byte[] data, int length);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl,
            PreserveSig = false)]
        internal static extern void NanoClr_LoadAssembliesSet(byte[] data, int length);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl,
            PreserveSig = false)]
        internal static extern void NanoClr_Resolve();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void NanoClr_SetConfigureCallback(
            [MarshalAs(UnmanagedType.FunctionPtr)] ConfigureDelegate configureCallback);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void NanoClr_SetDebugPrintCallback(
            [MarshalAs(UnmanagedType.FunctionPtr)] DebugPrintDelegate debugPrintCallback);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void NanoClr_WireOpen();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void NanoClr_WireClose();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int NanoClr_WireReceive(byte[] data, int length);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void NanoClr_WireTransmit(byte[] data, int length);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int NanoClr_WireBytesAvailable();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void NanoClr_WireProcess();
    }
}
