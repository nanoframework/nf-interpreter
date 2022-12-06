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
        public static int GetPortIndex(string portName)
        {
            if (int.TryParse(portName?.Replace("COM", ""), out int comIndex))
            {
                return comIndex;
            }
            else
            {
                // null or invalid port name
                // 'impossible' COM port 
                return -1;
            }
        }

        public static bool ValidateSerialPortName(string portName)
        {
            return Regex.Match(portName, "(?:^COM[1-9]{1}[0-9]{0,2}$)").Success;
        }

        [SupportedOSPlatform("windows")]
        public static void ExecuteElevated(
            Action action,
            string arguments,
            string executable = null)
        {
            // check if the process is running with elevated permissions
            bool isAdministrator = new WindowsPrincipal(WindowsIdentity.GetCurrent())
                .IsInRole(WindowsBuiltInRole.Administrator);

            if (isAdministrator && action is not null)
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

                RunAsAdministrator(
                    arguments,
                    executable);
            }
        }

        public static void RunAsAdministrator(
            string arguments,
            string executable = null)
        {
            var fileName = executable ?? Environment.ProcessPath;

            // this ProcessStartInfo is required to run the process with elevated permissions
            // can't use other options, can't route output, can't show window
            var info = new ProcessStartInfo(fileName, arguments)
            {
                UseShellExecute = true,
                WindowStyle = ProcessWindowStyle.Minimized,
                Verb = "runas"
            };

            var process = new Process
            {
                EnableRaisingEvents = true,
                StartInfo = info
            };

            process.Start();

            if (process.WaitForExit(60_000))
            {
                // check exit code
                var exitCode = process.ExitCode;

                if (exitCode != 0)
                {
                    throw new CLIException(ExitCode.E9002, $"Error running elevated process. Exit code was:{exitCode}.");
                }
            }
            else
            {
                // timeout occurred, report 
                throw new CLIException(ExitCode.E9003, "Running elevated process failed to execute in alloted time.");
            }
        }
    }
}
