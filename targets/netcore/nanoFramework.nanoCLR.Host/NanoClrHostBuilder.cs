using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace nanoFramework.nanoCLR.Host
{
    public class NanoClrHostBuilder
    {
        private static NanoClrHost _nanoClrHost = null;
        private readonly List<Action> _configureSteps = new();
        private readonly List<Action> _preInitConfigureSteps = new();
        private IDeviceSink _wireProtocolSink;

        public int MaxContextSwitches { get; set; } = 50;
        public bool WaitForDebugger { get; set; } = false;
        public bool EnterDebuggerLoopAfterExit { get; set; } = false;

        public NanoClrHostBuilder LoadAssembly(string name, byte[] data)
        {
            _configureSteps.Add(() => Interop.NanoClr_LoadAssembly(name, data, data.Length));
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
            _configureSteps.Add(() => Interop.NanoClr_LoadAssembliesSet(data, data.Length));
            return this;
        }

        public NanoClrHostBuilder LoadAssembliesSet(string fileName)
        {
            LoadAssembliesSet(File.ReadAllBytes(fileName));
            return this;
        }

        public NanoClrHostBuilder UseDebugPrintCallback(Action<string> debugPrint)
        {
            _preInitConfigureSteps.Add(() => Interop.NanoClr_SetDebugPrintCallback((msg) => debugPrint(msg)));
            return this;
        }

        public NanoClrHostBuilder UseConsoleDebugPrint() => UseDebugPrintCallback(Console.Write);

        public NanoClrHostBuilder TryResolve()
        {
            _preInitConfigureSteps.Add(() => Interop.NanoClr_Resolve());
            return this;
        }

        public NanoClrHostBuilder UseWireProtocolSink(IDeviceSink sink)
        {
            _wireProtocolSink = sink;
            return this;
        }

        public NanoClrHostBuilder UseSerialPortWireProtocol(string comPort, bool traceWire = false) =>
            UseWireProtocolSink(new SerialPortSink(comPort, traceWire: traceWire));

        public NanoClrHost Build()
        {
            if (_nanoClrHost != null)
                throw new InvalidOperationException("Cannot build two NanoClr runtime hosts");

            _nanoClrHost = new NanoClrHost();
            _nanoClrHost.WireProtocolSink = _wireProtocolSink;
            _nanoClrHost.ConfigureSteps.AddRange(_configureSteps);
            _nanoClrHost.PreInitConfigureSteps.AddRange(_preInitConfigureSteps);
            _nanoClrHost.NanoClrSettings = new NanoClrSettings
            {
                MaxContextSwitches = (ushort) MaxContextSwitches,
                WaitForDebugger = WaitForDebugger,
                EnterDebuggerLoopAfterExit = EnterDebuggerLoopAfterExit
            };
            return _nanoClrHost;
        }
    }
}
