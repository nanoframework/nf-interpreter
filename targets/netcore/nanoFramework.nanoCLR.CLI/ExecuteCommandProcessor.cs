// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Versioning;
using nanoFramework.nanoCLR.Host;
using nanoFramework.nanoCLR.Host.Port.TcpIp;

namespace nanoFramework.nanoCLR.CLI
{
    internal static class ExecuteCommandProcessor
    {
        [SupportedOSPlatform("windows")]
        public static int ProcessVerb(
            ExecuteCommandLineOptions options,
            VirtualSerialDeviceManager virtualBridgeManager)
        {
            Program.ProcessVerbosityOptions(options.Verbosity);

            nanoCLRHostBuilder hostBuilder;

            // are we to use a local DLL?
            if (options.PathToCLRInstance != null)
            {
                // check if path exists
                if (!Directory.Exists(options.PathToCLRInstance))
                {
                    throw new CLIException(ExitCode.E9009);
                }

                hostBuilder = nanoCLRHost.CreateBuilder(options.PathToCLRInstance);
            }
            else
            {
                hostBuilder = nanoCLRHost.CreateBuilder();
            }

            hostBuilder.UseConsoleDebugPrint();

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

                    hostBuilder.UseSerialPortWireProtocol(internalSerialPort);

                    // set flag
                    internalSerialPortConfig = true;
                }
                else
                {
                    return -1;
                }
            }

            if (Program.VerbosityLevel > VerbosityLevel.Normal)
            {
                hostBuilder.OutputNanoClrDllInfo();
            }

            if (options.AssembliesToLoad.Any())
            {
                hostBuilder.LoadAssemblies(options.AssembliesToLoad);
            }

            if (options.TryResolve)
            {
                hostBuilder.TryResolve();
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

            if (options.EnableProfiler)
            {
                hostBuilder.EnableProfiler();
            }

            if(options.DumpProfilerData)
            {
                hostBuilder.DumpProfilerLogData();
            }

            hostBuilder.WaitForDebugger = options.WaitForDebugger;
            hostBuilder.EnterDebuggerLoopAfterExit = options.EnterDebuggerLoopAfterExit;
            hostBuilder.PerformGarbageCollection = options.PerformGarbageCollection;
            hostBuilder.PerformHeapCompaction = options.PerformHeapCompaction;

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
