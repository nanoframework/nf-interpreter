//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.IO;
using System.Runtime.InteropServices;

namespace nanoFramework.nanoCLR.Host.Interop
{
    internal class nanoCLR
    {
        internal const uint ClrOk = 0;
        internal const uint ClrErrorFail = 0xFF000000;
        private const string NativeLibraryName = "nanoFramework.nanoCLR";
        private const string _nanoClrDllName = "nanoFramework.nanoCLR.dll";
        private static string _dllPath;

        internal static string DllPath
        {
            get => _dllPath;

            set
            {
                _dllPath = value;

                // set path to search nanoCLR DLL
                _ = SetDllDirectory(_dllPath);
            }
        }

        internal delegate uint ConfigureDelegate();

        internal delegate void DebugPrintDelegate([MarshalAs(UnmanagedType.LPStr)] string message);

        internal delegate int WireTransmitDelegate(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In] byte[] data, int length);

        internal delegate int WireReceiveDelegate(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][Out] byte[] data, int length);

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

        [DllImport("kernel32", SetLastError = true)]
        private static extern bool FreeLibrary(IntPtr hModule);

        public static void UnloadNanoClrImageDll()
        {
            string nanoClrDllLocation = Path.Combine(DllPath, _nanoClrDllName);

            foreach (System.Diagnostics.ProcessModule mod in System.Diagnostics.Process.GetCurrentProcess().Modules)
            {
                if (mod.FileName.Equals(nanoClrDllLocation, StringComparison.OrdinalIgnoreCase))
                {
                    FreeLibrary(mod.BaseAddress);

                    // done here!
                    break;
                }
            }
        }

        public static string FindNanoClrDll()
        {
            // perform dummy call to load DLL, in case it's not loaded
            _ = nanoCLR_GetVersion();

            // sweep processes and look for a DLL with the nanoCLR namme
            foreach (System.Diagnostics.ProcessModule mod in System.Diagnostics.Process.GetCurrentProcess().Modules)
            {
                if (mod.FileName.EndsWith(_nanoClrDllName, StringComparison.OrdinalIgnoreCase))
                {
                    return mod.FileName;
                }
            }

            return "";
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool SetDllDirectory(string lpPathName);
    }
}
