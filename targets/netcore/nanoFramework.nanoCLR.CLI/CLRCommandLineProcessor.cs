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
    public class CLRCommandLineProcessor
    {
        public static int ProcessVerb(
            CLRCommandLineOptions options,
            nanoCLRHostBuilder hostBuilder)
        {
            Program.ProcessVerbosityOptions(options.Verbosity);

            if (options.UpdateCLR)
            {
                Console.WriteLine($"nanoCLR version: {hostBuilder.GetCLRVersion()}");

                return (int)UpdateNanoCLR(options.TargetVersion, options.CheckPreviewVersions);
            }

            if (options.GetCLRVersion)
            {
                Console.WriteLine($"nanoCLR version: {hostBuilder.GetCLRVersion()}");

                return (int)ExitCode.OK;
            }

            return (int)ExitCode.OK;
        }

        private static ExitCode UpdateNanoCLR(string targetVersion, bool checkPreviewVersions)
        {
            throw new NotImplementedException();
        }
    }
}
