//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.Versioning;
using System.Security.Principal;

namespace nanoFramework.nanoCLR.CLI
{
    public static class VirtualDeviceCommandProcessor
    {
        [SupportedOSPlatform("windows")]
        public static int ProcessVerb(
            VirtualDeviceCommandLineOptions options,
            VirtualDeviceManager virtualComManager)
        {
            Program.ProcessVerbosityOptions(options.Verbosity);

            if (!CheckIfFunctional(virtualComManager))
            {
                return (int)ExitCode.E1000;
            }

            if (options.List)
            {
                ListVirtualBridges(virtualComManager);

                return (int)ExitCode.OK;
            }

            if (options.CreateVirtualSerialDevice != null)
            {
                CreateVirtualBridge(virtualComManager, options.CreateVirtualSerialDevice);

                return (int)ExitCode.OK;
            }

            if (options.DeleteVirtualSerialDevice != null)
            {
                DeleteVirtualSerialDevice(virtualComManager, options.DeleteVirtualSerialDevice);
            }

            return 0;
        }

        internal static bool CheckIfPortIsValid(
            VirtualDeviceManager virtualBridgeManager,
            string port)
        {
            int portIndex = Utilities.GetPortIndex(port);

            return virtualBridgeManager.GetVirtualPortDevice(portIndex) != null;
        }

        public static bool CheckIfFunctional(VirtualDeviceManager virtualComManager)
        {
            if (!virtualComManager.IsFunctional)
            {
                Console.ForegroundColor = ConsoleColor.Red;

                Console.WriteLine("*** Virtual Serial Port Tools are not installed ***");

                Console.WriteLine("To run nanoCLR with virtual COM port Virtual Serial Port Tools from HHD Software needs to be installed.");
                Console.WriteLine("Please donwload and install from https://www.hhdsoftware.com/virtual-serial-port-tools/extra-info.");

                Console.ForegroundColor = ConsoleColor.White;

                return false;
            }

            return true;
        }

        [SupportedOSPlatform("windows")]
        public static void DeleteVirtualSerialDevice(
            VirtualDeviceManager virtualComManager,
            string port)
        {
            if (Program.VerbosityLevel >= VerbosityLevel.Normal)
            {
                Console.WriteLine($"Deleting Virtual Serial Device: {port}");
            }

            var bridge = virtualComManager.GetVirtualBridgeContainingPort(port);

            if (bridge == null)
            {
                throw new CLIException(ExitCode.E1003);
            }

            ExecuteElevated(() => VirtualDeviceManager.DeleteVirtualBridge(bridge), $"{VirtualDeviceManager.VirtualDeviceVerb} --{VirtualDeviceManager.DeleteOption} {bridge}");

            bridge = virtualComManager.GetVirtualBridgeContainingPort(port);

            if(bridge != null)
            {
                throw new CLIException(ExitCode.E1002);
            }

            if (Program.VerbosityLevel >= VerbosityLevel.Normal)
            {
                Console.WriteLine("");
                Console.WriteLine($"Virtual Bridge succesfully deleted");
                Console.WriteLine("");
            }

        }

        /// <summary>
        /// Creates a <see cref="VirtualBridge"/> for the specified COM port.
        /// </summary>
        /// <param name="virtualComManager"></param>
        /// <param name="portA">Port A for <see cref="VirtualBridge"/>.</param>
        /// <param name="portB">Port A for <see cref="VirtualBridge"/>. If null a second COM port will be choosen randomly and used to create the <see cref="VirtualBridge"/>.</param>
        /// <returns></returns>
        [SupportedOSPlatform("windows")]
        public static VirtualBridge CreateVirtualBridge(
            VirtualDeviceManager virtualComManager,
            string portA, 
            string portB = null)
        {
            string bridgeName;

            if (portB == null)
            {
                // only 1st COM port was specified
                // check if this COM port is not in use
                if (!CheckFreeSerialPort(portA))
                {
                    throw new CLIException(ExitCode.E9004);
                }

                // get next free COM port
                portB = GetNextFreeSerialPort(portA);
            }

            bridgeName = $"{portA}:{portB}";

            if (Program.VerbosityLevel >= VerbosityLevel.Normal)
            {
                Console.WriteLine($"Creating a new Virtual Bridge: {bridgeName}");
            }

            // need to run this with elevated permission
            ExecuteElevated(() => virtualComManager.CreateVirtualBridge(bridgeName), $"{VirtualDeviceManager.VirtualDeviceVerb} --{VirtualDeviceManager.CreateOption} {portA} {portB}");

            // find virtual brige
            VirtualBridge bridge = virtualComManager.GetVirtualBridgeByName(bridgeName);

            if(bridge == null)
            {
                throw new CLIException(ExitCode.E1001);
            }

            if (Program.VerbosityLevel >= VerbosityLevel.Normal)
            {
                Console.WriteLine("");
                Console.WriteLine($"Virtual Bridge created succesfully");
                Console.WriteLine("");
            }

            return bridge;
        }

        private static bool CheckFreeSerialPort(string port)
        {
            var usedPorts = System.IO.Ports.SerialPort.GetPortNames();

            return !usedPorts.Contains(port);
        }

        private static string GetNextFreeSerialPort(string port)
        {
            int portIndex = Utilities.GetPortIndex(port);
            
            // start with the one right after the first one
            int newPortIndex = portIndex + 1;

            // get all port names currently in use
            var usedPorts = System.IO.Ports.SerialPort.GetPortNames();

            if(usedPorts.Any())
            {
                int lastUsedPortIndex = Utilities.GetPortIndex(usedPorts.Last());

                if (newPortIndex > lastUsedPortIndex)
                {
                    return $"COM{newPortIndex}";
                }
                else
                {
                    // grab the next free one after the last
                    lastUsedPortIndex++;

                    newPortIndex = lastUsedPortIndex;

                    // sanity check
                    if (newPortIndex == portIndex)
                    {
                        newPortIndex++;
                    }
                }
            }

            return $"COM{newPortIndex}";
        }

        public static void ListVirtualBridges(VirtualDeviceManager virtualComManager)
        {
            IEnumerable<VirtualBridge> bridges = virtualComManager.GetAvailableVirtualBridges();

            Console.WriteLine("Existing virtual port pairs:");

            foreach (var comBridge in bridges)
            {
                Console.WriteLine(comBridge);
            }
        }

        [SupportedOSPlatform("windows")]
        private static void ExecuteElevated(
            Action action,
            string arguments)
        {
            // check if the process is running with elevated permissions
            bool isAdministrator = new WindowsPrincipal(WindowsIdentity.GetCurrent())
                .IsInRole(WindowsBuiltInRole.Administrator);

            if (isAdministrator)
            {
                action();
            }
            else
            {
                if (Program.VerbosityLevel >= VerbosityLevel.Normal)
                {
                    // show explanation message to user
                    Console.ForegroundColor = ConsoleColor.Yellow;

                    Console.WriteLine("");
                    Console.WriteLine("**************************************************");
                    Console.WriteLine("*** Requires running with elevated permissions ***");
                    Console.WriteLine("*** UAC prompt will be shown, please authorize ***");
                    Console.WriteLine("**************************************************");
                    Console.WriteLine("");

                    Console.ForegroundColor = ConsoleColor.White;
                }

                RunAsAdministrator(arguments);
            }
        }

        private static void RunAsAdministrator(string arguments)
        {
            var fileName = Process.GetCurrentProcess().MainModule.FileName;

            // this ProcessStartInfo is required to run the process with elevated permisssions
            // can't use other options, can't route output, can't show window
            var info = new ProcessStartInfo(fileName, arguments)
            {
                UseShellExecute = true,
                WindowStyle = ProcessWindowStyle.Hidden,
                Verb = "runas"
            };

            var process = new Process
            {
                EnableRaisingEvents = true,
                StartInfo = info
            };

            process.Start();
            
            if(process.WaitForExit(20000))
            {
                // check exit code
                var exitCode = process.ExitCode;

                if(exitCode != 0)
                {
                    throw new CLIException(ExitCode.E9002, $"Exit code was:{exitCode}");
                }
            }
            else
            {
                // timeout occurred, report 
                throw new CLIException(ExitCode.E9003);
            }
        }
    }
}
