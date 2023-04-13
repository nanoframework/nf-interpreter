﻿//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using CommandLine;
using System.Collections.Generic;

namespace nanoFramework.nanoCLR.CLI
{
    [Verb(
        "run",
        HelpText = "Run nanoCLR assembly.")]
    public class ExecuteCommandLineOptions
    {
        [Option(
            'a',
            "assemblies",
            Required = false,
            HelpText = "List of PE assemblies to load in nanoCLR.")]
        public IEnumerable<string> AssembliesToLoad { get; set; }

        [Option(
            's',
            "serialport",
            Required = false,
            HelpText = "Virtual serial port exposing the nanoCLR instance.")]
        public string ExposedSerialPort { get; set; }

        [Option(
            'p',
            "networkport",
            Required = false,
            Hidden = true,
            HelpText = "Set TCP/IP port exposing the nanoCLR instance.")]
        public int? ExposedTcpIpPort { get; set; }

        [Option(
            'h',
            "host",
            Required = false,
            Hidden = true,
            HelpText = "Set TCP/IP host address exposing the nanoCLR instance.")]
        public string ExposedTcpIpHost { get; set; }

        [Option(
            'b',
            "broadcast",
            Required = false,
            Hidden = true,
            HelpText = "Set UDP broadcast port for device discovery.")]
        public int? ExposedBroadcastPort { get; set; }

        [Option(
            'n',
            "namedpipe",
            Required = false,
            HelpText = "Set Pipe Name exposing the nanoCLR instance.")]
        public string ExposedNamedPipe { get; set; }

        [Option(
            "tracewireprotocol",
            Required = false,
            Hidden = true,
            HelpText = "Trace wire protocol packets.")]
        public bool TraceWireProtocol { get; set; }

        [Option(
            'r',
            "resolve",
            Required = false,
            HelpText = "Tries to resolve cross-assembly references.")]
        public bool TryResolve { get; set; }

        [Option(
            'm',
            "monitorparentpid",
            Required = false,
            HelpText = "Parent process id to monitor - exit if the parent exits.")]
        public int? MonitorParentPid { get; set; }

        [Option(
            "waitfordebugger",
            Required = false,
            Default = false,
            HelpText = "Option to wait for a debugger connection after nanoCLR is started.")]
        public bool WaitForDebugger { get; set; }

        [Option(
            "loopafterexit",
            Required = false,
            Default = false,
            HelpText = "Option to remain in loop waiting for a debugger connection after the program exits.")]
        public bool EnterDebuggerLoopAfterExit { get; set; }

        [Option(
            "localinstance",
            Required = false,
            Default = null,
            Hidden = true,
            HelpText = "Path to a local instance of the nanoCLR.")]
        public string LocalInstance { get; set; }

        /// <summary>
        /// Allowed values:
        /// q[uiet]
        /// m[inimal]
        /// n[ormal]
        /// d[etailed]
        /// diag[nostic]
        /// </summary>
        [Option(
            'v',
            "verbosity",
            Required = false,
            Default = "n",
            HelpText = "Sets the verbosity level of the command. Allowed values are q[uiet], m[inimal], n[ormal], d[etailed], and diag[nostic]. Not supported in every command; see specific command page to determine if this option is available.")]
        public string Verbosity { get; set; }
    }
}
