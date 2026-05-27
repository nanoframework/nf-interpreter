// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using nanoFramework.nanoCLR.Host.Port;
using nanoFramework.nanoCLR.Host.Port.NamedPipe;
using nanoFramework.nanoCLR.Host.Port.Serial;
using nanoFramework.nanoCLR.Host.Port.TcpIp;
using nanoFramework.nanoCLR.Host.Profiler;

namespace nanoFramework.nanoCLR.Host
{
    public class nanoCLRHostBuilder
    {
        private static nanoCLRHost s_nanoClrHost = null;
        private readonly List<Action> _configureSteps = new();
        private readonly List<Action> _preInitConfigureSteps = new();
        private readonly List<Action> _cleanupSteps = new();
        private IPort _wireProtocolPort;
        private ProfilerMessageProcessor _profilerMessageProcessor;
        private ProfilerLogFileProcessor _profilerLogFileProcessor;

        public int MaxContextSwitches { get; set; } = 50;
        public bool WaitForDebugger { get; set; } = false;
        public bool EnterDebuggerLoopAfterExit { get; set; } = false;

        public bool PerformGarbageCollection { get; set; } = false;

        public bool PerformHeapCompaction { get; set; } = false;

        public nanoCLRHostBuilder()
        {
            Interop.nanoCLR.DllPath = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "NanoCLR");
        }

        public nanoCLRHostBuilder(string dllPath)
        {
            Interop.nanoCLR.DllPath = dllPath;
        }

        public nanoCLRHostBuilder LoadAssembly(string name, byte[] data)
        {
            _configureSteps.Add(() => Interop.nanoCLR.nanoCLR_LoadAssembly(name, data, data.Length));
            return this;
        }

        public nanoCLRHostBuilder LoadAssembly(string fileName)
        {
            LoadAssembly(Path.GetFileName(fileName), File.ReadAllBytes(fileName));
            return this;
        }

        public nanoCLRHostBuilder LoadAssemblies(IEnumerable<string> fileNames)
        {
            fileNames.ToList().ForEach(f => LoadAssembly(f));
            return this;
        }

        public nanoCLRHostBuilder LoadAssembliesSet(byte[] data)
        {
            _configureSteps.Add(() => Interop.nanoCLR.nanoCLR_LoadAssembliesSet(data, data.Length));
            return this;
        }

        public nanoCLRHostBuilder LoadAssembliesSet(string fileName)
        {
            LoadAssembliesSet(File.ReadAllBytes(fileName));
            return this;
        }

        public nanoCLRHostBuilder UseDebugPrintCallback(Action<string> debugPrint)
        {
            _preInitConfigureSteps.Add(() => Interop.nanoCLR.nanoCLR_SetDebugPrintCallback((msg) => debugPrint(msg)));
            return this;
        }

        public nanoCLRHostBuilder UseConsoleDebugPrint() => UseDebugPrintCallback(Console.Write);

        public nanoCLRHostBuilder TryResolve()
        {
            _preInitConfigureSteps.Add(() => Interop.nanoCLR.nanoCLR_Resolve());
            return this;
        }

        public nanoCLRHostBuilder UseWireProtocolPort(IPort port)
        {
            _wireProtocolPort = port;
            return this;
        }

        public string GetCLRVersion() =>
            Interop.nanoCLR.nanoCLR_GetVersion();

        public List<NativeAssemblyDetails> GetNativeAssemblies() =>
            NativeAssemblyDetails.Get();

        public nanoCLRHostBuilder UseSerialPortWireProtocol(string comPort) =>
            UseWireProtocolPort(new SerialPort(comPort));

        public nanoCLRHostBuilder UseTcpIpPortWireProtocol(string bindAddress, int port, int? discoveryPort) =>
            UseWireProtocolPort(new TcpIpListeningPort(bindAddress, port, discoveryPort));

        public nanoCLRHostBuilder UseNamedPipeWireProtocol(string name) =>
            UseWireProtocolPort(new NamedPipeServerPort(name));

        public nanoCLRHostBuilder UsePortTrace() =>
            UseWireProtocolPort(new TraceDataPort(_wireProtocolPort));

        public nanoCLRHostBuilder EnableProfiler()
        {
            _profilerMessageProcessor = new ProfilerMessageProcessor();
            SetProfilerMessageCallback(_profilerMessageProcessor.MessageHandler);

            _cleanupSteps.Add(() => _profilerMessageProcessor.Dispose());

            return this;
        }

        public nanoCLRHostBuilder DumpProfilerLogData()
        {
            _profilerLogFileProcessor = new ProfilerLogFileProcessor();

            SetProfilerLogDataCallback(_profilerLogFileProcessor.MessageHandler);

            _cleanupSteps.Add(() => _profilerLogFileProcessor.Dispose());

            return this;
        }

        public nanoCLRHost Build()
        {
            if (s_nanoClrHost != null)
            {
                throw new InvalidOperationException("Cannot build two nanoCLR runtime hosts");
            }

            s_nanoClrHost = new nanoCLRHost();

            s_nanoClrHost.WireProtocolPort = _wireProtocolPort;
            s_nanoClrHost.ConfigureSteps.AddRange(_configureSteps);
            s_nanoClrHost.PreInitConfigureSteps.AddRange(_preInitConfigureSteps);
            s_nanoClrHost.CleanupSteps.AddRange(_cleanupSteps);

            s_nanoClrHost.nanoCLRSettings = new nanoCLRSettings
            {
                MaxContextSwitches = (ushort)MaxContextSwitches,
                WaitForDebugger = WaitForDebugger,
                EnterDebuggerLoopAfterExit = EnterDebuggerLoopAfterExit,
                PerformGarbageCollection = PerformGarbageCollection,
                PerformHeapCompaction = PerformHeapCompaction
            };

            return s_nanoClrHost;
        }

        public void UnloadNanoClrDll() => Interop.nanoCLR.UnloadNanoClrImageDll();

        public void OutputNanoClrDllInfo() => Console.WriteLine($"nanoCLR loaded from '{Interop.nanoCLR.FindNanoClrDll()}'");

        private nanoCLRHostBuilder SetProfilerMessageCallback(Action<string> profilerMessage)
        {
            _configureSteps.Add(() => Interop.nanoCLR.nanoCLR_SetProfilerMessageCallback((msg) => profilerMessage(msg)));

            return this;
        }

        private nanoCLRHostBuilder SetProfilerLogDataCallback(Action<byte[], int> messageHandler)
        {
            _configureSteps.Add(() => Interop.nanoCLR.nanoCLR_SetProfilerDataCallback((data, length) => messageHandler(data, length)));

            return this;
        }
    }
}
