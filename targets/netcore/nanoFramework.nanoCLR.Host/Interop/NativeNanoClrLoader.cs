// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;

namespace nanoFramework.nanoCLR.Host.Interop
{
    internal static class NativeNanoClrLoader
    {
        private const string LogicalLibraryName = "nanoFramework.nanoCLR";
        private const string TraceEnvVar = "NANOCLR_TRACE_LOAD";
        private static readonly object s_syncRoot = new();
        private static string s_configuredDirectory = string.Empty;
        private static string s_loadedPath = string.Empty;
        private static IntPtr s_loadedHandle = IntPtr.Zero;
        private static bool s_resolverRegistered;

        internal static void ConfigureSearchDirectory(string dllPath)
        {
            lock (s_syncRoot)
            {
                s_configuredDirectory = NormalizeDirectory(dllPath);
                EnsureResolverRegistered();
            }
        }

        internal static void EnsureInitialized()
        {
            lock (s_syncRoot)
            {
                EnsureResolverRegistered();
            }
        }

        internal static string GetLoadedPath()
        {
            lock (s_syncRoot)
            {
                return s_loadedPath;
            }
        }

        internal static bool TryUnload()
        {
            lock (s_syncRoot)
            {
                if (s_loadedHandle == IntPtr.Zero)
                {
                    return false;
                }

                try
                {
                    NativeLibrary.Free(s_loadedHandle);
                    s_loadedHandle = IntPtr.Zero;
                    s_loadedPath = string.Empty;
                    return true;
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Exception occurred while unloading nanoCLR library: {ex.Message}");
                    return false;
                }
            }
        }

        private static void EnsureResolverRegistered()
        {
            if (s_resolverRegistered)
            {
                return;
            }

            NativeLibrary.SetDllImportResolver(typeof(nanoCLR).Assembly, ResolveLibrary);
            s_resolverRegistered = true;
        }

        private static IntPtr ResolveLibrary(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
        {
            if (!string.Equals(libraryName, LogicalLibraryName, StringComparison.Ordinal))
            {
                return IntPtr.Zero;
            }

            lock (s_syncRoot)
            {
                if (s_loadedHandle != IntPtr.Zero)
                {
                    TraceLoad($"already loaded from '{s_loadedPath}'");
                    return s_loadedHandle;
                }

                string runtimeIdentifier = GetRuntimeIdentifier();
                string nativeFileName = GetNativeLibraryFileName();
                List<string> attemptedProbePaths = new();

                TraceLoad(
                    $"OS='{RuntimeInformation.OSDescription}', ProcessArchitecture='{RuntimeInformation.ProcessArchitecture}', RuntimeIdentifier='{runtimeIdentifier}'");

                foreach (string candidatePath in BuildProbePaths(nativeFileName, runtimeIdentifier))
                {
                    attemptedProbePaths.Add(candidatePath);
                    TraceLoad($"probe '{candidatePath}'");

                    if (NativeLibrary.TryLoad(candidatePath, out IntPtr handle))
                    {
                        s_loadedHandle = handle;
                        s_loadedPath = candidatePath;

                        TraceLoad($"selected '{s_loadedPath}'");
                        TraceProbeSummary(attemptedProbePaths);

                        return handle;
                    }
                }

                if (NativeLibrary.TryLoad(nativeFileName, out IntPtr defaultHandle))
                {
                    s_loadedHandle = defaultHandle;
                    s_loadedPath = $"<default-loader>/{nativeFileName}";

                    TraceLoad("selected default OS loader path");
                    TraceProbeSummary(attemptedProbePaths);

                    return defaultHandle;
                }

                TraceProbeSummary(attemptedProbePaths);

                if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
                {
                    throw new DllNotFoundException(
                        $"Unable to load '{nativeFileName}'. " +
                        "The macOS native nanoFramework.nanoCLR library could not be found or loaded. " +
                        "Ensure that the correct macOS native library is built and available on the native library search path, " +
                        "or set the search directory explicitly via NativeNanoClrLoader.");
                }

                return IntPtr.Zero;
            }
        }

        private static IEnumerable<string> BuildProbePaths(string nativeFileName, string runtimeIdentifier)
        {
            List<string> probeDirectories = BuildBaseProbeDirectories();

            foreach (string directory in probeDirectories)
            {
                yield return Path.Combine(directory, nativeFileName);
            }

            if (!string.IsNullOrWhiteSpace(runtimeIdentifier))
            {
                // Probe the exact RID first (e.g. "ubuntu.24.04-x64").
                foreach (string directory in probeDirectories)
                {
                    yield return Path.Combine(directory, "runtimes", runtimeIdentifier, "native", nativeFileName);
                }

                // Also probe the portable RID (e.g. "linux-x64", "osx-arm64") so that packages
                // that ship under the portable RID are found on distro-specific runtimes such as
                // "ubuntu.24.04-x64" where RuntimeInformation.RuntimeIdentifier does not match.
                string portableRid = GetPortableRuntimeIdentifier();

                if (!string.IsNullOrWhiteSpace(portableRid) &&
                    !string.Equals(portableRid, runtimeIdentifier, StringComparison.OrdinalIgnoreCase))
                {
                    foreach (string directory in probeDirectories)
                    {
                        yield return Path.Combine(directory, "runtimes", portableRid, "native", nativeFileName);
                    }
                }
            }
        }

        private static string GetPortableRuntimeIdentifier()
        {
            string arch = RuntimeInformation.ProcessArchitecture switch
            {
                Architecture.X64   => "x64",
                Architecture.Arm64 => "arm64",
                Architecture.X86   => "x86",
                Architecture.Arm   => "arm",
                _                  => string.Empty,
            };

            if (string.IsNullOrEmpty(arch))
            {
                return string.Empty;
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return $"win-{arch}";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                return $"osx-{arch}";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                return $"linux-{arch}";
            }

            return string.Empty;
        }

        private static List<string> BuildBaseProbeDirectories()
        {
            List<string> probeDirectories = new();
            string assemblyDirectory = Path.GetDirectoryName(typeof(nanoCLR).Assembly.Location) ?? string.Empty;

            AddProbeDirectory(probeDirectories, s_configuredDirectory);
            AddProbeDirectory(probeDirectories, AppContext.BaseDirectory);
            AddProbeDirectory(probeDirectories, Path.Combine(AppContext.BaseDirectory, "NanoCLR"));
            AddProbeDirectory(probeDirectories, assemblyDirectory);
            AddProbeDirectory(probeDirectories, Path.Combine(assemblyDirectory, "NanoCLR"));

            return probeDirectories;
        }

        private static void AddProbeDirectory(List<string> probeDirectories, string path)
        {
            string normalizedPath = NormalizeDirectory(path);

            if (string.IsNullOrWhiteSpace(normalizedPath))
            {
                return;
            }

            foreach (string probeDirectory in probeDirectories)
            {
                if (string.Equals(probeDirectory, normalizedPath, StringComparison.OrdinalIgnoreCase))
                {
                    return;
                }
            }

            probeDirectories.Add(normalizedPath);
        }

        private static string NormalizeDirectory(string path)
        {
            if (string.IsNullOrWhiteSpace(path))
            {
                return string.Empty;
            }

            try
            {
                string fullPath = Path.GetFullPath(path);
                string rootPath = Path.GetPathRoot(fullPath) ?? string.Empty;

                if (string.Equals(fullPath, rootPath, StringComparison.OrdinalIgnoreCase))
                {
                    return fullPath;
                }

                return fullPath.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
            }
            catch
            {
                return string.Empty;
            }
        }

        private static string GetNativeLibraryFileName()
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return "nanoFramework.nanoCLR.dll";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                return "nanoFramework.nanoCLR.dylib";
            }

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                return "nanoFramework.nanoCLR.so";
            }

            return LogicalLibraryName;
        }

        private static string GetRuntimeIdentifier()
        {
            try
            {
                return RuntimeInformation.RuntimeIdentifier;
            }
            catch
            {
                return string.Empty;
            }
        }

        private static void TraceProbeSummary(List<string> attemptedProbePaths)
        {
            if (!IsTraceEnabled())
            {
                return;
            }

            Console.WriteLine("[nanoCLR-loader] attempted probe paths:");

            foreach (string probePath in attemptedProbePaths)
            {
                Console.WriteLine($"[nanoCLR-loader] {probePath}");
            }

            Console.WriteLine($"[nanoCLR-loader] final path: '{s_loadedPath}'");
        }

        private static void TraceLoad(string message)
        {
            if (IsTraceEnabled())
            {
                Console.WriteLine($"[nanoCLR-loader] {message}");
            }
        }

        private static bool IsTraceEnabled() =>
            string.Equals(Environment.GetEnvironmentVariable(TraceEnvVar), "1", StringComparison.Ordinal);
    }
}
