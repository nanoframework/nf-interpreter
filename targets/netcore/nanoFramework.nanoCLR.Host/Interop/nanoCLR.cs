//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Runtime.InteropServices;

namespace nanoFramework.nanoCLR.Host.Interop
{
    internal class nanoCLR
    {
        internal const uint ClrOk = 0;
        internal const uint ClrErrorFail = 0xFF000000;
        private const string NativeLibraryName = "nanoFramework.nanoCLR";

        internal delegate uint ConfigureDelegate();

        internal delegate void DebugPrintDelegate([MarshalAs(UnmanagedType.LPStr)] string message);

        internal delegate int WireTransmitDelegate(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] [In] byte[] data, int length);

        internal delegate int WireReceiveDelegate(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] [Out] byte[] data, int length);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // The following declaration have the functions names exposed in the C++ DLL
        // Keep their names in sync with the C++ code @ netcore\nanoFramework.nanoCLR\nanoCLR_native.cpp
        // and netcore\nanoFramework.nanoCLR\nanoCLR_native.h
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_Run(nanoCLRSettings nanoClrSettings);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl,
            PreserveSig = false)]
        internal static extern void nanoCLR_LoadAssembly(string assemblyName, byte[] data, int length);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl,
            PreserveSig = false)]
        internal static extern void nanoCLR_LoadAssembliesSet(byte[] data, int length);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl,
            PreserveSig = false)]
        internal static extern void nanoCLR_Resolve();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_SetConfigureCallback(
            [MarshalAs(UnmanagedType.FunctionPtr)] ConfigureDelegate configureCallback);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_SetDebugPrintCallback(
            [MarshalAs(UnmanagedType.FunctionPtr)] DebugPrintDelegate debugPrintCallback);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_WireProtocolOpen();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_WireProtocolClose();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_SetWireProtocolReceiveCallback(
            [MarshalAs(UnmanagedType.FunctionPtr)] WireReceiveDelegate receiveCallback);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_SetWireProtocolTransmitCallback(
            [MarshalAs(UnmanagedType.FunctionPtr)] WireTransmitDelegate transmitCallback);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_WireProtocolProcess();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string nanoCLR_GetVersion();
    }
}
