//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

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
    }
}
