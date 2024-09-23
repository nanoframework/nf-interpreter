// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

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
            "getnativeassemblies",
            Required = false,
            HelpText = "Gets the names and versions of the native assemblies.")]
        public bool GetNativeAssemblies { get; set; }

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

        [Option(
            "clrinstancepath",
            Required = false,
            Default = null,
            Hidden = true,
            HelpText = "Path to the directory where a local version of nanoFramework.nanoCLR.dll is located.")]
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
