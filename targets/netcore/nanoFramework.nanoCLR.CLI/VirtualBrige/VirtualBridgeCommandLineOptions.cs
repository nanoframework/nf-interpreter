//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using CommandLine;

namespace nanoFramework.nanoCLR.VirtualBridge
{
    [Verb(VirtualBridgeManager.VirtualBridgeVerb, HelpText = "Manage Virtual Bridges.")]
    public class VirtualBridgeCommandLineOptions
    {
        [Option('l', "list", Required = false, HelpText = "List existing Virtual Bridges")]
        public bool List { get; set; }

        [Option('c', "create", Required = false, HelpText = "Create new Virtual Bridge")]
        public string CreateVirtualBridge { get; set; }

        [Option('d', "delete", Required = false, HelpText = "Delete Virtual Bridge")]
        public string DeleteVirtualBridge { get; set; }

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
