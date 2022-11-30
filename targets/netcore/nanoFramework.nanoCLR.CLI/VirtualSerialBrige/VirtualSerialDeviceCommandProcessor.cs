//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Versioning;

namespace nanoFramework.nanoCLR.CLI
{
    public static class VirtualSerialDeviceCommandProcessor
    {
        [SupportedOSPlatform("windows")]
        public static int ProcessVerb(
            VirtualSerialDeviceCommandLineOptions options,
            VirtualSerialDeviceManager virtualComManager)
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
            VirtualSerialDeviceManager virtualBridgeManager,
            string port)
        {
            int portIndex = Utilities.GetPortIndex(port);

            return virtualBridgeManager.GetVirtualPortDevice(portIndex) != null;
        }

        public static bool CheckIfFunctional(VirtualSerialDeviceManager virtualComManager)
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
            VirtualSerialDeviceManager virtualComManager,
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

            Utilities.ExecuteElevated(() => VirtualSerialDeviceManager.DeleteVirtualBridge(bridge), $"{VirtualSerialDeviceManager.VirtualSerialDeviceVerb} --{VirtualSerialDeviceManager.DeleteOption} {bridge}");

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
        /// Creates a <see cref="VirtualSerialBridge"/> for the specified COM port.
        /// </summary>
        /// <param name="virtualComManager"></param>
        /// <param name="portA">Port A for <see cref="VirtualSerialBridge"/>.</param>
        /// <param name="portB">Port A for <see cref="VirtualSerialBridge"/>. If null a second COM port will be choosen randomly and used to create the <see cref="VirtualSerialBridge"/>.</param>
        /// <returns></returns>
        [SupportedOSPlatform("windows")]
        public static VirtualSerialBridge CreateVirtualBridge(
            VirtualSerialDeviceManager virtualComManager,
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
            ExecuteElevated(() => virtualComManager.CreateVirtualBridge(bridgeName), $"{VirtualSerialDeviceManager.VirtualSerialDeviceVerb} --{VirtualSerialDeviceManager.CreateOption} {portA} {portB}");

            // find virtual brige
            VirtualSerialBridge bridge = virtualComManager.GetVirtualBridgeByName(bridgeName);

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

        public static void ListVirtualBridges(VirtualSerialDeviceManager virtualComManager)
        {
            IEnumerable<VirtualSerialBridge> bridges = virtualComManager.GetAvailableVirtualBridges();

            if (bridges.Any())
            {
                Console.WriteLine("Existing virtual serial port pairs:");

                foreach (var comBridge in bridges)
                {
                    Console.WriteLine(comBridge);
                }
            }
            else
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("No virtual serial port pairs found.");
                Console.ForegroundColor = ConsoleColor.White;
            }
        }
    }
}
