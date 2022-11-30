//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using CommandLine;

namespace nanoFramework.nanoCLR.CLI
{
    [Verb(
        VirtualSerialDeviceManager.VirtualSerialDeviceVerb,
        HelpText = "Manage Virtual Devices.")]
    public class VirtualSerialDeviceCommandLineOptions
    {
        [Option(
            'l',
            "list",
            Required = false,
            HelpText = "List existing Virtual Bridges")]
        public bool List { get; set; }

        [Option(
            'c',
            VirtualSerialDeviceManager.CreateOption,
            Required = false,
            HelpText = "Create Virtual Serial Device")]
        public string CreateVirtualSerialDevice { get; set; }

        [Option(
            'r',
            VirtualSerialDeviceManager.RemoveOption,
            Required = false,
            HelpText = "Remove Virtual Serial Device")]
        public string RemoveVirtualSerialDevice { get; set; }

        [Option(
            "install",
            Required = false,
            Default = false,
            HelpText = "Install Virtual Serial Port Tools")]
        public bool InstallVirtualSerialPortTools { get; set; }

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
            Default = "d",
            HelpText = "Sets the verbosity level of the command. Allowed values are q[uiet], m[inimal], n[ormal], d[etailed], and diag[nostic]. Not supported in every command; see specific command page to determine if this option is available.")]
        public string Verbosity { get; set; }
    }
}
