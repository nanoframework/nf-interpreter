//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Linq;
using CommandLine;
using nanoFramework.nanoCLR.Host;

namespace nanoFramework.nanoCLR.CLI
{
    class Program
    {
        static void Main(string[] args)
        {
            LogErrors(() =>
            {
                NanoClrHostBuilder hostBuilder = NanoClrHost.CreateBuilder();
                hostBuilder.UseConsoleDebugPrint();

                Parser.Default.ParseArguments<CommandLineOptions>(args).WithParsed(o =>
                {
                    if (o.Assemblies.Any())
                        hostBuilder.LoadAssemblies(o.Assemblies);

                    if (o.AssembliesSet != null)
                        hostBuilder.LoadAssembliesSet(o.AssembliesSet);

                    if (o.TryResolve)
                        hostBuilder.TryResolve();

                    if (o.DebugSerialPort != null || o.DebugTcpIpPort != null || o.DebugNamedPipe != null)
                    {
                        hostBuilder.WaitForDebugger = true;
                        hostBuilder.EnterDebuggerLoopAfterExit = true;
                    }

                    if (o.DebugSerialPort != null)
                        hostBuilder.UseSerialPortWireProtocol(o.DebugSerialPort);

                    if (o.DebugTcpIpPort != null)
                        hostBuilder.UseTcpIpPortWireProtocol(o.DebugTcpIpPort.Value);

                    if (o.DebugNamedPipe != null)
                        hostBuilder.UseNamedPipeWireProtocol(o.DebugNamedPipe);

                    if (o.TraceWire)
                        hostBuilder.UsePortTrace();

                    hostBuilder.Build().Run();
                });
            });
        }

        private static void LogErrors(Action scope)
        {
            try
            {
                scope();
            }
            catch (Exception e)
            {
                Console.WriteLine($"Error: {e.Message}");
            }
        }
    }
}
