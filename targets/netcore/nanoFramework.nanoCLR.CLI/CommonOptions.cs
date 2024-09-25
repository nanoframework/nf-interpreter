// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using CommandLine;

namespace nanoFramework.nanoCLR.CLI
{
    public class CommonOptions
    {
        [Option(
            "clrpath",
            Required = false,
            Default = null,
            HelpText = "Path to the directory from which to load a specific version of nanoFramework.nanoCLR.dll.")]
        public string PathToCLRInstance { get; set; }

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
