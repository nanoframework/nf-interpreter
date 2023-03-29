﻿//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using CommandLine;

namespace nanoFramework.nanoCLR.CLI
{
    [Verb(
        "instance",
        HelpText = "Operations with the current nanoCLR instance.")]
    public class ClrInstanceOperationsOptions
    {
        [Option(
            "getversion",
            Required = false,
            HelpText = "Gets the version of the current nanoCLR instance.")]
        public bool GetCLRVersion { get; set; }

        [Option(
            "update",
            Required = false,
            Default = false,
            HelpText = "Checks for stable versions of nanoCLR and updates it, if there is one. A specific version can be specified. To check preview versions add the --preview option.")]
        public bool UpdateCLR { get; set; }

        [Option(
            "preview",
            Required = false,
            HelpText = "Include preview versions when checking for nanoCLR updates.")]
        public bool CheckPreviewVersions { get; set; }

        [Option(
            "clrversion",
            Required = false,
            Default = null,
            HelpText = "Specify a version of nanoCRL to install.")]
        public string TargetVersion { get; set; }

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
