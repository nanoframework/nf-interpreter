//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Diagnostics;
using System.Runtime.Versioning;
using System.Security.Principal;
using System;
using System.Text.RegularExpressions;

namespace nanoFramework.nanoCLR.CLI
{
    internal static class Utilities
    {
        public static bool ValidateComPortName(string portName)
        {
            // validate format
            Regex comPortRegex = new(@"COM(\d+)", RegexOptions.IgnoreCase);
            Match match = comPortRegex.Match(portName);

            if (!match.Success)
            {
                throw new CLIException(ExitCode.E9001);
            }

            return true;
        }

        public static int GetPortIndex(string portName)
        {
            return int.Parse(portName.Replace("COM", ""));
        }

        [SupportedOSPlatform("windows")]
        public static void ExecuteElevated(
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

        public static void RunAsAdministrator(string arguments)
        {
            var fileName = Process.GetCurrentProcess().MainModule.FileName;

            // this ProcessStartInfo is required to run the process with elevated permissions
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

            if (process.WaitForExit(20000))
            {
                // check exit code
                var exitCode = process.ExitCode;

                if (exitCode != 0)
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
