// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
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
                // Ensure the path includes a trailing path separator
                if (!value.EndsWith(Path.DirectorySeparatorChar.ToString(), StringComparison.Ordinal))
                {
                    value += Path.DirectorySeparatorChar;
                }

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

        internal delegate void ProfilerMessageDelegate(
            [MarshalAs(UnmanagedType.LPStr)] string message);

        internal delegate void ProfilerDataDelegate(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In] byte[] data, int length);

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // The following declaration have the functions names exposed in the C++ DLL                     //
        // Keep their names in sync with the C++ code @ netcore\nanoFramework.nanoCLR\nanoCLR_native.cpp //
        // and netcore\nanoFramework.nanoCLR\nanoCLR_native.h                                            //
        ///////////////////////////////////////////////////////////////////////////////////////////////////

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
        internal static extern void nanoCLR_SetProfilerMessageCallback(
            [MarshalAs(UnmanagedType.FunctionPtr)] ProfilerMessageDelegate profilerMessageCallback);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_SetProfilerDataCallback(
            [MarshalAs(UnmanagedType.FunctionPtr)] ProfilerDataDelegate profilerDataCallback);

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void nanoCLR_WireProtocolProcess();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string nanoCLR_GetVersion();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern ushort nanoCLR_GetNativeAssemblyCount();

        [DllImport(NativeLibraryName, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern bool nanoCLR_GetNativeAssemblyInformation(byte[] buffer, int size);

        [DllImport("kernel32", SetLastError = true)]
        private static extern bool FreeLibrary(IntPtr hModule);

        public static void UnloadNanoClrImageDll()
        {
            string nanoClrDllLocation = Path.Combine(DllPath, _nanoClrDllName);

            var modules = Process.GetCurrentProcess().Modules.Cast<ProcessModule>()
                .Where(mod => mod.FileName.Equals(nanoClrDllLocation, StringComparison.OrdinalIgnoreCase));

            foreach (var mod in modules)
            {
                try
                {
                    if (FreeLibrary(mod.BaseAddress))
                    {
                        // Successfully unloaded the DLL
                        break;
                    }
                    else
                    {
                        // Handle the error if FreeLibrary fails
                        int errorCode = Marshal.GetLastWin32Error();
                        Console.WriteLine($"Failed to unload nanoCLR DLL. Error code: {errorCode}");
                    }
                }
                catch (Exception ex)
                {
                    // Handle any exceptions that occur during the unload process
                    Console.WriteLine($"Exception occurred while unloading nanoCLR DLL: {ex.Message}");
                }
            }
        }

        public static string FindNanoClrDll()
        {
            try
            {
                // Perform dummy call to load DLL, in case it's not loaded
                _ = nanoCLR_GetVersion();

                // Sweep processes and look for a DLL with the nanoCLR name
                var module = Process.GetCurrentProcess().Modules.Cast<ProcessModule>()
                    .FirstOrDefault(mod => mod.FileName.EndsWith(_nanoClrDllName, StringComparison.OrdinalIgnoreCase));

                return module?.FileName ?? string.Empty;
            }
            catch (Exception ex)
            {
                // Handle any exceptions that occur during the process
                Console.WriteLine($"Exception occurred while finding nanoCLR DLL: {ex.Message}");
                return string.Empty;
            }
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool SetDllDirectory(string lpPathName);
    }
}
