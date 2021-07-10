//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using nanoFramework.nanoCLR.Host.Port;

namespace nanoFramework.nanoCLR.Host
{
    public class NanoClrHostBuilder
    {
        private static NanoClrHost s_nanoClrHost = null;
        private readonly List<Action> _configureSteps = new();
        private readonly List<Action> _preInitConfigureSteps = new();
        private IPort _wireProtocolPort;

        public int MaxContextSwitches { get; set; } = 50;
        public bool WaitForDebugger { get; set; } = false;
        public bool EnterDebuggerLoopAfterExit { get; set; } = false;

        public NanoClrHostBuilder LoadAssembly(string name, byte[] data)
        {
            _configureSteps.Add(() => Native.NanoClr_LoadAssembly(name, data, data.Length));
            return this;
        }

        public NanoClrHostBuilder LoadAssembly(string fileName)
        {
            LoadAssembly(Path.GetFileName(fileName), File.ReadAllBytes(fileName));
            return this;
        }

        public NanoClrHostBuilder LoadAssemblies(IEnumerable<string> fileNames)
        {
            fileNames.ToList().ForEach(f => LoadAssembly(f));
            return this;
        }

        public NanoClrHostBuilder LoadAssembliesSet(byte[] data)
        {
            _configureSteps.Add(() => Native.NanoClr_LoadAssembliesSet(data, data.Length));
            return this;
        }

        public NanoClrHostBuilder LoadAssembliesSet(string fileName)
        {
            LoadAssembliesSet(File.ReadAllBytes(fileName));
            return this;
        }

        public NanoClrHostBuilder UseDebugPrintCallback(Action<string> debugPrint)
        {
            _preInitConfigureSteps.Add(() => Native.NanoClr_SetDebugPrintCallback((msg) => debugPrint(msg)));
            return this;
        }

        public NanoClrHostBuilder UseConsoleDebugPrint() => UseDebugPrintCallback(Console.Write);

        public NanoClrHostBuilder TryResolve()
        {
            _preInitConfigureSteps.Add(() => Native.NanoClr_Resolve());
            return this;
        }

        public NanoClrHostBuilder UseWireProtocolPort(IPort port)
        {
            _wireProtocolPort = port;
            return this;
        }

        public NanoClrHostBuilder UseSerialPortWireProtocol(string comPort, bool traceWire = false) =>
            UseWireProtocolPort(new SerialPort(comPort));

        public NanoClrHost Build()
        {
            if (s_nanoClrHost != null)
                throw new InvalidOperationException("Cannot build two NanoClr runtime hosts");

            s_nanoClrHost = new NanoClrHost();
            s_nanoClrHost.WireProtocolPort = _wireProtocolPort;
            s_nanoClrHost.ConfigureSteps.AddRange(_configureSteps);
            s_nanoClrHost.PreInitConfigureSteps.AddRange(_preInitConfigureSteps);
            s_nanoClrHost.NanoClrSettings = new NanoClrSettings
            {
                MaxContextSwitches = (ushort) MaxContextSwitches,
                WaitForDebugger = WaitForDebugger,
                EnterDebuggerLoopAfterExit = EnterDebuggerLoopAfterExit
            };
            return s_nanoClrHost;
        }
    }
}
