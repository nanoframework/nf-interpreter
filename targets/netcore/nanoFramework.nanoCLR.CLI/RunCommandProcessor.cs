//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Linq;
using nanoFramework.nanoCLR.Host;
using nanoFramework.nanoCLR.VirtualCom;

namespace nanoFramework.nanoCLR.CLI
{
    public class RunCommandProcessor
    {
        public static int ProcessVerb(RunCommandLineOptions opts, NanoClrHostBuilder hostBuilder,
            VirtualComManager virtualComManager)
        {
            if (opts.DebugVirtualPortPair != null)
            {
                if (VirtualComCommandProcessor.CheckIfSupported(virtualComManager))
                {
                    return -1;
                }

                var (comA, _) =
                    VirtualComCommandProcessor.CreateVirtualPortPairIfNotExists(virtualComManager,
                        opts.DebugVirtualPortPair);
                hostBuilder.WaitForDebugger = true;
                hostBuilder.EnterDebuggerLoopAfterExit = true;
                hostBuilder.UseSerialPortWireProtocol($"COM{comA}");
            }

            if (opts.Assemblies.Any())
            {
                hostBuilder.LoadAssemblies(opts.Assemblies);
            }

            if (opts.AssembliesSet != null)
            {
                hostBuilder.LoadAssembliesSet(opts.AssembliesSet);
            }

            if (opts.TryResolve)
            {
                hostBuilder.TryResolve();
            }

            if (opts.DebugSerialPort != null || opts.DebugTcpIpPort != null || opts.DebugNamedPipe != null)
            {
                hostBuilder.WaitForDebugger = true;
                hostBuilder.EnterDebuggerLoopAfterExit = true;
            }

            if (opts.DebugSerialPort != null)
            {
                hostBuilder.UseSerialPortWireProtocol(opts.DebugSerialPort);
            }

            if (opts.DebugTcpIpPort != null)
            {
                hostBuilder.UseTcpIpPortWireProtocol(opts.DebugTcpIpPort.Value);
            }

            if (opts.DebugNamedPipe != null)
            {
                hostBuilder.UseNamedPipeWireProtocol(opts.DebugNamedPipe);
            }

            if (opts.TraceWire)
            {
                hostBuilder.UsePortTrace();
            }

            hostBuilder.Build().Run();

            return 0;
        }
    }
}
