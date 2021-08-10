//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;
using CommandLine;
using nanoFramework.nanoCLR.Host;
using nanoFramework.nanoCLR.VirtualCom;

namespace nanoFramework.nanoCLR.CLI
{
    class Program
    {
        static void Main(string[] args)
        {
            LogErrors(() =>
            {
                VirtualComManager virtualComManager = new VirtualComManager();
                virtualComManager.Initialize();

                NanoClrHostBuilder hostBuilder = NanoClrHost.CreateBuilder();
                hostBuilder.UseConsoleDebugPrint();

                Parser.Default.ParseArguments<RunCommandLineOptions, VirtualComCommandLineOptions>(args)
                    .MapResult(
                        (RunCommandLineOptions opts) =>
                            RunCommandProcessor.ProcessVerb(opts, hostBuilder, virtualComManager),
                        (VirtualComCommandLineOptions opts) =>
                            VirtualComCommandProcessor.ProcessVerb(opts, virtualComManager),
                        errs => 1);
            });
        }

        private static void LogErrors(Action scope)
        {
            try
            {
                scope();
            }
            catch (Exception e)
            {
                Console.WriteLine($"Error: {e.Message}");
            }
        }
    }
}
