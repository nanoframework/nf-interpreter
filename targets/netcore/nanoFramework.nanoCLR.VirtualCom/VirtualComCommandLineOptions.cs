//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using CommandLine;

namespace nanoFramework.nanoCLR.VirtualCom
{
    [Verb("comports", HelpText = "Manage Virtual COM Ports.")]
    public class VirtualComCommandLineOptions
    {
        [Option('l', "list", Required = false, HelpText = "List existing Virtual COM Pairs")]
        public bool List { get; set; }

        [Option('c', "create", Required = false, HelpText = "Create New Virtual COM Pair")]
        public string CreateComPair { get; set; }

        [Option('d', "delete", Required = false, HelpText = "Delete Virtual COM Pair")]
        public string DeleteComPair { get; set; }
    }
}
