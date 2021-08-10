//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Security.Principal;

namespace nanoFramework.nanoCLR.VirtualCom
{
    public class VirtualComCommandProcessor
    {
        public static int ProcessVerb(VirtualComCommandLineOptions opts, VirtualComManager virtualComManager)
        {
            if (CheckIfSupported(virtualComManager))
            {
                return -1;
            }

            if (opts.List)
            {
                ListVirtualPortPairs(virtualComManager);
            }

            if (opts.DeleteComPair != null)
            {
                DeleteVirtualPortPair(virtualComManager, opts.DeleteComPair);
            }

            if (opts.CreateComPair != null)
            {
                CreateVirtualPortPairIfNotExists(virtualComManager, opts.CreateComPair);
            }

            return 0;
        }

        public static bool CheckIfSupported(VirtualComManager virtualComManager)
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

        public static void DeleteVirtualPortPair(VirtualComManager virtualComManager, string name)
        {
            RequireElevated(() => virtualComManager.DeletePair(name), $"com -d {name}");
        }

        public static (int comA, int comB) CreateVirtualPortPairIfNotExists(VirtualComManager virtualComManager,
            string name)
        {
            PortPair pair = virtualComManager.GetPairByName(name);
            if (pair == null)
            {
                Console.WriteLine($"Creating virtual COM Pair - {name} ...");
                RequireElevated(() => virtualComManager.CreatePair(name), $"com -c {name}");
            }
            else
            {
                Console.WriteLine($"Using existing virtual COM Pair - {name}");
            }

            return virtualComManager.ParsePair(name);
        }

        public static void ListVirtualPortPairs(VirtualComManager virtualComManager)
        {
            IEnumerable<PortPair> bridges = virtualComManager.GetPortPairs();

            Console.WriteLine("Existing virtual port pairs:");

            foreach (var comBridge in bridges)
            {
                Console.WriteLine(comBridge);
            }
        }

        private static void RequireElevated(Action action, string arguments)
        {
            bool isAdministrator = new WindowsPrincipal(WindowsIdentity.GetCurrent())
                .IsInRole(WindowsBuiltInRole.Administrator);

            if (isAdministrator)
            {
                action();
            }
            else
            {
                RunAsAdministrator(arguments);
            }
        }

        private static void RunAsAdministrator(string arguments)
        {
            var fileName = Process.GetCurrentProcess().MainModule.FileName;

            var info = new ProcessStartInfo(fileName, arguments)
            {
                UseShellExecute = true,
                WindowStyle = ProcessWindowStyle.Hidden,
                Verb = "runas",
            };

            var process = new Process
            {
                EnableRaisingEvents = true,
                StartInfo = info
            };

            process.Start();
            process.WaitForExit();
        }
    }
}
