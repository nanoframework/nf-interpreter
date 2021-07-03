using System.Collections.Generic;
using CommandLine;

namespace nanoFramework.nanoCLR.CLI
{
    public class CommandLineOptions
    {
        [Option('l', "load", Required = false, HelpText = "Loads list of assemblies formatted for nanoCLR")]
        public IEnumerable<string> Assemblies { get; set; }

        [Option('b', "loadDatabase", Required = false, HelpText = "Loads a set of assemblies")]
        public string AssembliesSet { get; set; }

        [Option('d', "debug", Required = false, HelpText = "Force execution engine to wait for a debugger to be connected", Default = false)]
        public bool WaitForDebugger { get; set; }

        [Option('w', "debugOnExit", Required = false, HelpText = "Connection from a debugger is to be awaited after the execution engine terminates", Default = false)]
        public bool EnterDebuggerLoopAfterExit { get; set; }

        [Option('v', "verbose", Required = false, HelpText = "Output debug information to console", Default = true)]
        public bool Debug { get; set; }

        [Option('r', "resolve", Required = false, HelpText = "Tries to resolve cross-assembly references")]
        public bool TryResolve { get; set; }
    }
}
