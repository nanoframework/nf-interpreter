//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Linq;
using nanoFramework.nanoCLR.Host;
using nanoFramework.nanoCLR.VirtualBridge;

namespace nanoFramework.nanoCLR.CLI
{
    public class RunCommandProcessor
    {
        public static int ProcessVerb(RunCommandLineOptions options, NanoClrHostBuilder hostBuilder,
            VirtualBridgeManager virtualBridgeManager)
        {
            if (options.DebugVirtualBridge != null)
            {
                if (VirtualBridgeCommandProcessor.CheckIfSupported(virtualBridgeManager))
                {
                    var (comA, _) =
                        VirtualBridgeCommandProcessor.CreateVirtualBridge(virtualBridgeManager,
                            options.DebugVirtualBridge);

                    hostBuilder.WaitForDebugger = true;
                    hostBuilder.EnterDebuggerLoopAfterExit = true;
                    hostBuilder.UseSerialPortWireProtocol($"COM{comA}");
                }
                else
                {
                    return -1;
                }
            }

            if (options.Assemblies.Any())
            {
                hostBuilder.LoadAssemblies(options.Assemblies);
            }

            if (options.AssembliesSet != null)
            {
                hostBuilder.LoadAssembliesSet(options.AssembliesSet);
            }

            if (options.TryResolve)
            {
                hostBuilder.TryResolve();
            }

            if (options.DebugSerialPort != null || options.DebugTcpIpPort != null || options.DebugNamedPipe != null)
            {
                hostBuilder.WaitForDebugger = true;
                hostBuilder.EnterDebuggerLoopAfterExit = true;
            }

            if (options.DebugSerialPort != null)
            {
                hostBuilder.UseSerialPortWireProtocol(options.DebugSerialPort);
            }

            if (options.DebugTcpIpPort != null)
            {
                hostBuilder.UseTcpIpPortWireProtocol(options.DebugTcpIpPort.Value);
            }

            if (options.DebugNamedPipe != null)
            {
                hostBuilder.UseNamedPipeWireProtocol(options.DebugNamedPipe);
            }

            if (options.TraceWire)
            {
                hostBuilder.UsePortTrace();
            }

            hostBuilder.Build().Run();

            return 0;
        }
    }
}
