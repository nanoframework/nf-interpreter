//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using nanoFramework.nanoCLR.CLI;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Security.Principal;

namespace nanoFramework.nanoCLR.VirtualBridge
{
    public class VirtualBridgeCommandProcessor
    {
        public static int ProcessVerb(VirtualBridgeCommandLineOptions opts, VirtualBridgeManager virtualComManager)
        {
            if (!CheckIfSupported(virtualComManager))
            {
                return (int)ExitCode.E1000;
            }

            if (opts.List)
            {
                ListVirtualBridges(virtualComManager);

                return (int)ExitCode.OK;
            }

            if (opts.DeleteVirtualBridge != null)
            {
                DeleteVirtualBridge(virtualComManager, opts.DeleteVirtualBridge);
            }

            if (opts.CreateVirtualBridge != null)
            {
                CreateVirtualBridge(virtualComManager, opts.CreateVirtualBridge);
            }

            return 0;
        }

        public static bool CheckIfSupported(VirtualBridgeManager virtualComManager)
        {
            if (!virtualComManager.IsInstalled)
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

        public static void DeleteVirtualBridge(VirtualBridgeManager virtualComManager, string name)
        {
            RequireElevated(() => virtualComManager.DeleteVirtualBridge(name), $"{VirtualBridgeManager.VirtualBridgeVerb} -d {name}");
        }

        /// <summary>
        /// Creates a Virtual Bridge between the specified virtual ports.
        /// If the virtual bridge already exists it will be returned instead.
        /// </summary>
        /// <param name="virtualComManager"></param>
        /// <param name="bridgeName"></param>
        /// <returns></returns>
        public static (int comA, int comB) CreateVirtualBridge(VirtualBridgeManager virtualComManager,
            string bridgeName)
        {
            VirtualBridge pair = virtualComManager.GetVirtualBridgeByName(bridgeName);
            if (pair == null)
            {
                RequireElevated(() => virtualComManager.CreateVirtualBridge(bridgeName), $"{VirtualBridgeManager.VirtualBridgeVerb} -c {bridgeName}");
            }
            else
            {
                Console.WriteLine($"Using existing Virtual Bridge: {pair}");
            }

            return virtualComManager.ParseVirtualBridge(bridgeName);
        }

        public static void ListVirtualBridges(VirtualBridgeManager virtualComManager)
        {
            IEnumerable<VirtualBridge> bridges = virtualComManager.GetAvailableVirtualBridges();

            Console.WriteLine("Existing virtual port pairs:");

            foreach (var comBridge in bridges)
            {
                Console.WriteLine(comBridge);
            }
        }

        private static void RequireElevated(Action action, string arguments)
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
                // show explanation message to user
                Console.ForegroundColor = ConsoleColor.Yellow;

                Console.WriteLine("**************************************************");
                Console.WriteLine("*** Requires running with elevated permissions ***");
                Console.WriteLine("*** UAC prompt will be shown, please authorize ***");
                Console.WriteLine("**************************************************");

                Console.ForegroundColor = ConsoleColor.White;

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
            }
            else
            {
                // timeout occurred, report 

            }

        }
    }
}
