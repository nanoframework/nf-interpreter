//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Linq;
using System.Runtime.Versioning;
using System.Text.RegularExpressions;
using nanoFramework.nanoCLR.Host;

namespace nanoFramework.nanoCLR.CLI
{
    public class RunCommandProcessor
    {
        [SupportedOSPlatform("windows")]
        public static int ProcessVerb(
            RunCommandLineOptions options,
            NanoClrHostBuilder hostBuilder,
            VirtualDeviceManager virtualBridgeManager)
        {
            Program.ProcessVerbosityOptions(options.Verbosity);

            if (options.DebugSerialPort != null)
            {
                // a serial port was requested 

                // validate serial port
                Utilities.ValidateComPortName(options.DebugSerialPort);

                // check if Virtual Serial Port Tools are available 
                if (VirtualDeviceCommandProcessor.CheckIfFunctional(virtualBridgeManager))
                {
                    VirtualBridge bridge;

                    // check if the requested port it's a valid Virtual Device
                    if (VirtualDeviceCommandProcessor.CheckIfPortIsValid(virtualBridgeManager, options.DebugSerialPort))
                    {
                        // get the virtual bridge that contains the request port
                        bridge = virtualBridgeManager.GetVirtualBridgeContainingPort(options.DebugSerialPort);
                    }
                    else
                    {
                        // no Virtual Device for that index, create a new virtual bridge
                        bridge = VirtualDeviceCommandProcessor.CreateVirtualBridge(virtualBridgeManager, options.DebugSerialPort);
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

            //if (options.AssembliesSet != null)
            //{
            //    hostBuilder.LoadAssembliesSet(options.AssembliesSet);
            //}

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
