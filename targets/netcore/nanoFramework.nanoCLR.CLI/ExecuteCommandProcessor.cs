//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Diagnostics;
using System.Linq;
using System.Runtime.Versioning;
using System.Text.RegularExpressions;
using nanoFramework.nanoCLR.Host;
using nanoFramework.nanoCLR.Host.Port.TcpIp;

namespace nanoFramework.nanoCLR.CLI
{
    internal static class ExecuteCommandProcessor
    {
        [SupportedOSPlatform("windows")]
        public static int ProcessVerb(
            ExecuteCommandLineOptions options,
            nanoCLRHostBuilder hostBuilder,
            VirtualSerialDeviceManager virtualBridgeManager)
        {
            Program.ProcessVerbosityOptions(options.Verbosity);

            if (options.DebugSerialPort != null)
            {
                // a serial port was requested 

                // validate serial port
                Utilities.ValidateComPortName(options.DebugSerialPort);

                // check if Virtual Serial Port Tools are available 
                if (VirtualSerialDeviceCommandProcessor.CheckIfFunctional(virtualBridgeManager))
                {
                    VirtualSerialBridge bridge;

                    // check if the requested port it's a valid Virtual Device
                    if (VirtualSerialDeviceCommandProcessor.CheckIfPortIsValid(virtualBridgeManager, options.DebugSerialPort))
                    {
                        // get the virtual bridge that contains the request port
                        bridge = virtualBridgeManager.GetVirtualBridgeContainingPort(options.DebugSerialPort);
                    }
                    else
                    {
                        // no Virtual Device for that index, create a new virtual bridge
                        bridge = VirtualSerialDeviceCommandProcessor.CreateVirtualBridge(virtualBridgeManager, options.DebugSerialPort);
                    }

                    if (bridge == null)
                    {
                        throw new CLIException(ExitCode.E1003);
                    }

                    // need to set debugger serial port to the _other_ port so it shows at the expected end
                    options.DebugSerialPort = $"COM{bridge.GetOtherPort(options.DebugSerialPort)}";

                    hostBuilder.WaitForDebugger = true;
                    hostBuilder.EnterDebuggerLoopAfterExit = true;
                    hostBuilder.UseSerialPortWireProtocol(options.DebugSerialPort);
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
                hostBuilder.UseTcpIpPortWireProtocol(options.DebugTcpIpHost ?? TcpIpListeningPort.Localhost, options.DebugTcpIpPort.Value, options.DebugBroadcastPort ?? NetworkWireBroadcastService.DebugBroadcastPort);
            }

            if (options.DebugNamedPipe != null)
            {
                hostBuilder.UseNamedPipeWireProtocol(options.DebugNamedPipe);
            }

            if (options.TraceWire)
            {
                hostBuilder.UsePortTrace();
            }

            if (options.MonitorParentPid != null)
            {
                try
                {
                    Process parentProcess = Process.GetProcessById((int)options.MonitorParentPid);
                    parentProcess.EnableRaisingEvents = true;
                    parentProcess.Exited += ParentProcess_Exited;
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Failed to find parent process with pid: {options.MonitorParentPid} message: {ex.Message}");
                }
            }

            var host = hostBuilder.Build();

            Console.CancelKeyPress += (_, _) => host.Shutdown();

            host.Run();

            return 0;
        }

        private static void ParentProcess_Exited(object sender, EventArgs e)
        {
            Console.WriteLine("Exiting due to parent process ending");
            Environment.Exit(0);        // force exit of this process since the parent has exited/died
        }
    }
}
