//
// Copyright (c) 2021 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.IO;
using System.Linq;
using System.Reflection;
using CommandLine;
using CommandLine.Text;
using nanoFramework.nanoCLR.Host;
using nanoFramework.nanoCLR.VirtualBridge;
using nanoFramework.nanoCLR.VirtualBridge;

namespace nanoFramework.nanoCLR.CLI
{
    internal class Program
    {
        private static ExitCode _exitCode;
        private static string _extraMessage;
        private static VerbosityLevel _verbosityLevel = VerbosityLevel.Normal;
        private static AssemblyInformationalVersionAttribute _informationalVersionAttribute;
        private static string _headerInfo;
        private static CopyrightInfo _copyrightInfo;
        internal static string ExecutingPath;

        static int Main(string[] args)
        {
            // take care of static fields
            _informationalVersionAttribute = Attribute.GetCustomAttribute(
                Assembly.GetEntryAssembly()!,
                typeof(AssemblyInformationalVersionAttribute))
            as AssemblyInformationalVersionAttribute;

            _headerInfo = $".NET nanoFramework CLR CLI v{_informationalVersionAttribute.InformationalVersion}";

            _copyrightInfo = new CopyrightInfo(true, $".NET Foundation and nanoFramework project contributors", 2019);

            // need this to be able to use ProcessStart at the location where the .NET Core CLI tool is running from
            string codeBase = Assembly.GetExecutingAssembly().Location;
            var uri = new UriBuilder(codeBase);
            var fullPath = Uri.UnescapeDataString(uri.Path);
            ExecutingPath = Path.GetDirectoryName(fullPath);
            
            // check for empty argument collection
            if (!args.Any())
            {
                // no argument provided, show help text and usage examples

                // because of short-comings in CommandLine parsing 
                // need to customize the output to provide a consistent output
                var parser = new Parser(config => config.HelpWriter = null);
                var result = parser.ParseArguments<RunCommandLineOptions>(new[] { "", "" });

                var helpText = new HelpText(
                    new HeadingInfo(_headerInfo),
                    _copyrightInfo)
                        .AddPreOptionsLine("No command was provided.")
                        .AddPreOptionsLine("")
                        .AddPreOptionsLine(HelpText.RenderUsageText(result))
                        .AddPreOptionsLine("")
                        .AddOptions(result);

                Console.WriteLine(helpText.ToString());

                return (int)ExitCode.OK;
            }

            LogErrors(() =>
            {
                VirtualBridgeManager virtualBridgeManager = new();
                virtualBridgeManager.Initialize();

                NanoClrHostBuilder hostBuilder = NanoClrHost.CreateBuilder();
                hostBuilder.UseConsoleDebugPrint();

                var parsedArguments = Parser.Default.ParseArguments<RunCommandLineOptions, VirtualBridgeCommandLineOptions>(args);

                parsedArguments.MapResult(
                        (RunCommandLineOptions opts) =>
                            RunCommandProcessor.ProcessVerb(opts, hostBuilder, virtualBridgeManager),
                        (VirtualBridgeCommandLineOptions opts) =>
                            VirtualBridgeCommandProcessor.ProcessVerb(opts, virtualBridgeManager),
                        (IEnumerable<Error> errors) => HandleErrors(errors));
            });

            if (_verbosityLevel > VerbosityLevel.Quiet)
            {
                OutputError(_exitCode, _verbosityLevel > VerbosityLevel.Normal, _extraMessage);
            }

            return (int)_exitCode;
        }

        private static int HandleErrors(IEnumerable<Error> errors)
        {
            _exitCode = ExitCode.E9000;

            return (int)_exitCode;
        }

        private static void OutputError(ExitCode errorCode, bool outputMessage, string extraMessage = null)
        {
            if (errorCode == ExitCode.OK)
            {
                return;
            }

            Console.ForegroundColor = ConsoleColor.Red;

            if (outputMessage)
            {
                Console.Write($"Error {errorCode}");

                var exitCodeDisplayName = errorCode.GetAttribute<DisplayAttribute>();

                if (!string.IsNullOrEmpty(exitCodeDisplayName.Name))
                {
                    Console.Write($": { exitCodeDisplayName.Name }");
                }

                if (string.IsNullOrEmpty(extraMessage))
                {
                    Console.WriteLine();
                }
                else
                {
                    Console.WriteLine($" ({ extraMessage })");
                }
            }
            else
            {
                Console.Write($"{errorCode}");
                Console.WriteLine();
            }

            Console.ForegroundColor = ConsoleColor.White;
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
