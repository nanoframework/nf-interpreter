//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;
using System.Threading.Tasks;

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

            if (options.InstallVirtualSerialPortTools)
            {
                return (int)InstallVirtualSerialPortTools().Result;
            }

            if (!CheckIfFunctional(virtualComManager))
            {
                return (int)ExitCode.E1000;
            }

            if (options.List)
            {
                ListVirtualBridges(virtualComManager);

                return (int)ExitCode.OK;
            }

            if (options.CreateVirtualSerialDevice)
            {
                _ = CreateVirtualBridge(
                    virtualComManager,
                    options.PortName);

                return (int)ExitCode.OK;
            }

            if (options.RemoveVirtualSerialDevice)
            {
                RemoveVirtualSerialDevice(
                    virtualComManager,
                    options.PortName);
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

                Console.WriteLine("To run nanoCLR with a virtual COM port need to install Virtual Serial Port Tools from HHD Software.");
                Console.WriteLine("Please run 'nanoclr virtualserial --install'.");
                Console.WriteLine("As an alternative download and install from https://www.hhdsoftware.com/virtual-serial-port-tools/extra-info.");

                Console.ForegroundColor = ConsoleColor.White;

                return false;
            }

            return true;
        }

        [SupportedOSPlatform("windows")]
        public static void RemoveVirtualSerialDevice(
            VirtualSerialDeviceManager virtualComManager,
            string port)
        {
            if (Program.VerbosityLevel >= VerbosityLevel.Normal)
            {
                Console.WriteLine($"Removing Virtual Serial Port {port}");
            }
            else
            {
                Console.Write($"Removing Virtual Serial Port {port}...");
            }

            if (!Utilities.ValidateSerialPortName(port))
            {
                throw new CLIException(ExitCode.E9001);
            }

            var bridge = virtualComManager.GetVirtualBridgeContainingPort(port);

            if (bridge == null)
            {
                throw new CLIException(ExitCode.E1003);
            }

            Utilities.ExecuteElevated(() => VirtualSerialDeviceManager.RemoveVirtualBridge(bridge), $"{VirtualSerialDeviceManager.VirtualSerialDeviceVerb} --{VirtualSerialDeviceManager.RemoveOption} {bridge}");

            bridge = virtualComManager.GetVirtualBridgeContainingPort(port);

            if (bridge != null)
            {
                throw new CLIException(ExitCode.E1002);
            }

            Console.ForegroundColor = ConsoleColor.Green;

            if (Program.VerbosityLevel >= VerbosityLevel.Normal)
            {
                Console.WriteLine("Virtual Bridge successfully deleted");
            }
            else
            {
                Console.WriteLine("OK");
            }

            Console.ForegroundColor = ConsoleColor.White;
        }

        /// <summary>
        /// Creates a <see cref="VirtualSerialBridge"/> for the specified COM port.
        /// </summary>
        /// <param name="virtualComManager"></param>
        /// <param name="portA">Port A for <see cref="VirtualSerialBridge"/>.</param>
        /// <param name="portB">Port A for <see cref="VirtualSerialBridge"/>. If null a second COM port will be chosen randomly and used to create the <see cref="VirtualSerialBridge"/>.</param>
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
                if (portA is null)
                {
                    // get free COM port
                    portA = GetNextFreeSerialPort(portA);
                }
                else
                {
                    if (!Utilities.ValidateSerialPortName(portA))
                    {
                        throw new CLIException(ExitCode.E9001);
                    }

                    if (!CheckFreeSerialPort(portA))
                    {
                        throw new CLIException(ExitCode.E9004);
                    }
                }

                // get next free COM port
                portB = GetNextFreeSerialPort(portA);
            }

            bridgeName = $"{portA}:{portB}";

            if (Program.VerbosityLevel >= VerbosityLevel.Normal)
            {
                Console.WriteLine($"Creating a new Virtual Bridge {bridgeName}");
            }
            else
            {
                Console.Write($"Creating a new Virtual Bridge {bridgeName}...");
            }

            // need to run this with elevated permission
            Utilities.ExecuteElevated(() => virtualComManager.CreateVirtualBridge(bridgeName), $"{VirtualSerialDeviceManager.VirtualSerialDeviceVerb} --{VirtualSerialDeviceManager.CreateOption} {portA} {portB}");

            // find virtual bridge
            VirtualSerialBridge bridge = virtualComManager.GetVirtualBridgeByName(bridgeName);

            if (bridge == null)
            {
                throw new CLIException(ExitCode.E1001);
            }

            Console.ForegroundColor = ConsoleColor.Green;

            if (Program.VerbosityLevel >= VerbosityLevel.Normal)
            {
                Console.WriteLine($"Virtual Bridge created successfully");
            }
            else
            {
                Console.WriteLine("OK");
            }

            Console.ForegroundColor = ConsoleColor.White;

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
            // for invalid index, start with a 'high' COM port index
            int newPortIndex = portIndex == -1 ? 30 : portIndex + 1;

            // get all port names currently in use
            var usedPorts = System.IO.Ports.SerialPort.GetPortNames();

            if (usedPorts.Any())
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

        private static async Task<ExitCode> InstallVirtualSerialPortTools()
        {
            string hhdsoftwareUrl = "https://www.hhdsoftware.com";
            string installerName = "virtual-serial-port-tools-redist.exe";
            HttpClient httpClient = new HttpClient();
            var installerLocation = Path.Combine(Path.GetTempPath(), installerName);

            // check if this is running on Windows Platform
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                Console.WriteLine($"Can't install Virtual Serial Port Tools in platform other than Windows.");

                return ExitCode.E9006;
            }

            try
            {
                httpClient.BaseAddress = new Uri(hhdsoftwareUrl);
                httpClient.DefaultRequestHeaders.Add("Accept", "*/*");

                Console.Write($"Downloading Virtual Serial Port Tools...");

                // get latest version available for download
                HttpResponseMessage response = await httpClient.GetAsync($"/download/{installerName}");
                response.EnsureSuccessStatusCode();

                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("OK");
                Console.ForegroundColor = ConsoleColor.White;

                var installerContent = await response.Content.ReadAsStreamAsync();
                var installerFile = File.OpenWrite(installerLocation);

                installerContent.Seek(0, SeekOrigin.Begin);
                installerContent.CopyTo(installerFile);

                installerContent.Dispose();
                installerFile.Dispose();

                Utilities.ExecuteElevated(null, "-silent", installerLocation);

                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("Virtual Serial Port Tools installed.");
                Console.ForegroundColor = ConsoleColor.White;

                return ExitCode.OK;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error installing Virtual Serial Port Tools: {ex}");

                return ExitCode.E9006;
            }
        }
    }
}
