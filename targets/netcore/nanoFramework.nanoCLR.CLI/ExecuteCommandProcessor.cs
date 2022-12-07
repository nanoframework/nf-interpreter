//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using nanoFramework.nanoCLR.Host;
using nanoFramework.nanoCLR.Host.Port.TcpIp;
using System;
using System.Diagnostics;
using System.Linq;
using System.Runtime.Versioning;

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

            // flag to signal that the intenal serial port has already been configured
            bool internalSerialPortConfig = false;

            if (options.ExposedSerialPort != null)
            {
                // a serial port was requested 

                // validate serial port
                if (!Utilities.ValidateSerialPortName(options.ExposedSerialPort))
                {
                    throw new CLIException(ExitCode.E9001);
                }

                // check if Virtual Serial Port Tools are available 
                if (VirtualSerialDeviceCommandProcessor.CheckIfFunctional(virtualBridgeManager))
                {
                    VirtualSerialBridge bridge;

                    // check if the requested port it's a valid Virtual Device
                    if (VirtualSerialDeviceCommandProcessor.CheckIfPortIsValid(virtualBridgeManager, options.ExposedSerialPort))
                    {
                        // get the virtual bridge that contains the request port
                        bridge = virtualBridgeManager.GetVirtualBridgeContainingPort(options.ExposedSerialPort);
                    }
                    else
                    {
                        // no Virtual Device for that index, create a new virtual bridge
                        bridge = VirtualSerialDeviceCommandProcessor.CreateVirtualBridge(virtualBridgeManager, options.ExposedSerialPort);
                    }

                    if (bridge == null)
                    {
                        throw new CLIException(ExitCode.E1003);
                    }

                    // need to set debugger serial port to the _other_ port so it shows at the expected end
                    var internalSerialPort = $"COM{bridge.GetOtherPort(options.ExposedSerialPort)}";

                    hostBuilder.WaitForDebugger = true;
                    hostBuilder.EnterDebuggerLoopAfterExit = true;
                    hostBuilder.UseSerialPortWireProtocol(internalSerialPort);

                    // set flag
                    internalSerialPortConfig = true;
                }
                else
                {
                    return -1;
                }
            }

            if (options.AssembliesToLoad.Any())
            {
                hostBuilder.LoadAssemblies(options.AssembliesToLoad);
            }

            if (options.TryResolve)
            {
                hostBuilder.TryResolve();
            }

            if (options.ExposedSerialPort != null || options.ExposedTcpIpPort != null || options.ExposedNamedPipe != null)
            {
                hostBuilder.WaitForDebugger = true;
                hostBuilder.EnterDebuggerLoopAfterExit = true;
            }

            if (!internalSerialPortConfig
                && options.ExposedSerialPort != null)
            {
                hostBuilder.UseSerialPortWireProtocol(options.ExposedSerialPort);
            }

            if (options.ExposedTcpIpPort != null)
            {
                hostBuilder.UseTcpIpPortWireProtocol(options.ExposedTcpIpHost ?? TcpIpListeningPort.Localhost, options.ExposedTcpIpPort.Value, options.ExposedBroadcastPort ?? NetworkWireBroadcastService.DebugBroadcastPort);
            }

            if (options.ExposedNamedPipe != null)
            {
                hostBuilder.UseNamedPipeWireProtocol(options.ExposedNamedPipe);
            }

            if (options.TraceWireProtocol)
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
                    Console.WriteLine($"Failed to find parent process with ID: {options.MonitorParentPid} message: {ex.Message}");
                }
            }

            var host = hostBuilder.Build();

            Console.CancelKeyPress += (_, _) => host.Shutdown();

            try
            {
                host.Run();
            }
            catch (UnauthorizedAccessException ex)
            {
                // can't open port, most likely because there's already another instance running
                throw new CLIException(ExitCode.E9007, ex.Message);
            }
            catch (Exception ex)
            {
                throw new CLIException(ExitCode.E9008, ex.Message);
            }

            return 0;
        }

        private static void ParentProcess_Exited(object sender, EventArgs e)
        {
            Console.WriteLine("Exiting due to parent process ending");

            // force exit of this process since the parent has exited/died
            Environment.Exit(0);
        }
    }
}
