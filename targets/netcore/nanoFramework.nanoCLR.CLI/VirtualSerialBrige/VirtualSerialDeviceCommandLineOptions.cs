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
        private const string _createSetName = "addset";
        private const string _removeSetName = "removeset";
        private const string _listSetName = "listset";
        private const string _installSetName = "installset";

        [Option(
            "list",
            Required = false,
            SetName = _listSetName,
            HelpText = "List existing Virtual Bridges")]
        public bool List { get; set; }

        [Option(
            VirtualSerialDeviceManager.CreateOption,
            Required = false,
            SetName = _createSetName,
            HelpText = "Create Virtual Serial Device")]
        public bool CreateVirtualSerialDevice { get; set; }

        [Option(
            VirtualSerialDeviceManager.RemoveOption,
            Required = false,
            SetName = _removeSetName,
            HelpText = "Remove Virtual Serial Device")]
        public bool RemoveVirtualSerialDevice { get; set; }

        [Value(
            0,
            Hidden = true,
            Default = null)]
        public string PortName { get; set; }

        [Option(
            "install",
            Required = false,
            Default = false,
            SetName = _installSetName,
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
