//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;
using CommandLine;
using CommandLine.Text;

[assembly: DefaultDllImportSearchPaths(DllImportSearchPath.UserDirectories | DllImportSearchPath.UseDllDirectoryForDependencies)]

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

        public static VerbosityLevel VerbosityLevel => _verbosityLevel;

        static int Main(string[] args)
        {
            // take care of static fields
            _informationalVersionAttribute = Attribute.GetCustomAttribute(
                Assembly.GetEntryAssembly()!,
                typeof(AssemblyInformationalVersionAttribute))
            as AssemblyInformationalVersionAttribute;

            _headerInfo = $".NET nanoFramework nanoCLR CLI v{_informationalVersionAttribute.InformationalVersion}";

            _copyrightInfo = new CopyrightInfo(true, ".NET Foundation and nanoFramework project contributors", 2021);

            // need this to be able to use ProcessStart at the location where the .NET Core CLI tool is running from
            string assemblyLocation = Assembly.GetExecutingAssembly().Location;
            ExecutingPath = Path.GetDirectoryName(Path.GetFullPath(assemblyLocation));

            // check for empty argument collection
            if (!args.Any())
            {
                // no argument provided, show help text and usage examples

                // because of short-comings in CommandLine parsing 
                // need to customize the output to provide a consistent output
                var parser = new Parser(config => config.HelpWriter = null);

                var result = parser.ParseArguments<ExecuteCommandLineOptions, VirtualSerialDeviceCommandLineOptions, VirtualSerialDeviceCommandLineOptions>(new[] { "", "" });

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
            else
            {
                if (_verbosityLevel > VerbosityLevel.Quiet)
                {
                    // show CLI header
                    Console.ForegroundColor = ConsoleColor.White;

                    Console.WriteLine(_headerInfo);
                    Console.WriteLine(_copyrightInfo);
                    Console.WriteLine();

                    Console.WriteLine();
                }
            }

            LogErrors(() =>
            {
                // Virtual serial bridge support is Windows-only (uses hhdvspkit COM library).
                VirtualSerialDeviceManager virtualSerialBridgeManager = null;
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    virtualSerialBridgeManager = new VirtualSerialDeviceManager();
                    virtualSerialBridgeManager.Initialize();
                }

                var parsedArguments = Parser.Default.ParseArguments<ExecuteCommandLineOptions, ClrInstanceOperationsOptions, VirtualSerialDeviceCommandLineOptions>(args);

                Console.ForegroundColor = ConsoleColor.White;

                parsedArguments.MapResult(
                        (ExecuteCommandLineOptions opts) =>
                            ExecuteCommandProcessor.ProcessVerb(
                                opts,
                                virtualSerialBridgeManager),
                        (ClrInstanceOperationsOptions opts) =>
                            ClrInstanceOperationsProcessor.ProcessVerb(
                                opts),
                        (VirtualSerialDeviceCommandLineOptions opts) =>
                            RuntimeInformation.IsOSPlatform(OSPlatform.Windows)
                                ? VirtualSerialDeviceCommandProcessor.ProcessVerb(opts, virtualSerialBridgeManager)
                                : throw new CLIException(ExitCode.E9000, "Virtual serial bridge is only supported on Windows."),
                        (IEnumerable<Error> errors) => HandleErrors(errors));

                // do we need to show version?
                if (parsedArguments.Errors.Any(error => error is VersionRequestedError))
                {
                    // output version
                    var versionAtt = Attribute.GetCustomAttribute(
                        Assembly.GetEntryAssembly()!,
                        typeof(AssemblyFileVersionAttribute)) as AssemblyFileVersionAttribute;

                    var version = new Version(versionAtt.Version);

                    Console.WriteLine(version.ToString(3));
                }
                else if (parsedArguments.Errors.Any(error => error is HelpVerbRequestedError)
                         || parsedArguments.Errors.Any(error => error is HelpRequestedError))
                {
                    // output help
                    return;
                }
            });

            if (_verbosityLevel > VerbosityLevel.Quiet)
            {
                OutputError(_exitCode, _verbosityLevel > VerbosityLevel.Quiet, _extraMessage);
            }

            return (int)_exitCode;
        }

        internal static void ProcessVerbosityOptions(string verbosityLevel)
        {
            _verbosityLevel = verbosityLevel switch
            {
                // quiet
                "q" or "quiet" => VerbosityLevel.Quiet,

                // minimal
                "m" or "minimal" => VerbosityLevel.Minimal,

                // normal
                "n" or "normal" => VerbosityLevel.Normal,

                // detailed
                "d" or "detailed" => VerbosityLevel.Detailed,

                // diagnostic
                "diag" or "diagnostic" => VerbosityLevel.Diagnostic,

                _ => throw new ArgumentException("Invalid option for Verbosity"),
            };
        }

        private static int HandleErrors(IEnumerable<Error> errors)
        {
            if (errors.Any(error => error is VersionRequestedError)
                || errors.Any(error => error is HelpRequestedError)
                || errors.Any(error => error is HelpVerbRequestedError))
            {
                // we're good here
            }
            else
            {
                _exitCode = ExitCode.E9000;
            }

            return (int)_exitCode;
        }

        private static void OutputError(
            ExitCode errorCode,
            bool outputMessage,
            string extraMessage = null)
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
                    Console.Write($": {exitCodeDisplayName.Name}");
                }

                if (string.IsNullOrEmpty(extraMessage))
                {
                    Console.WriteLine();
                }
                else
                {
                    Console.WriteLine($" ({extraMessage})");
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
            catch (CLIException e)
            {
                _exitCode = e.ExecutionError;
                _extraMessage = e.ExecutionErrorMessage;
            }
            catch (Exception e)
            {
                Console.WriteLine($"Error: {e.Message}");
            }
        }

    }
}
