//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using CommandLine;

namespace nanoFramework.nanoCLR.VirtualCom
{
    [Verb("com", HelpText = "Manage Virtual Com Ports.")]
    public class VirtualComCommandLineOptions
    {
        [Option('l', "list", Required = false, HelpText = "List existing Virtual Com Pairs")]
        public bool List { get; set; }

        [Option('c', "create", Required = false, HelpText = "Create New Virtual Com Pair")]
        public string CreateComPair { get; set; }

        [Option('d', "delete", Required = false, HelpText = "Delete Virtual Com Pair")]
        public string DeleteComPair { get; set; }
    }
}
