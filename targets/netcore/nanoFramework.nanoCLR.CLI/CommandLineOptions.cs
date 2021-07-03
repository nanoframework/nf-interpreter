using System.Collections.Generic;
using CommandLine;

namespace nanoFramework.nanoCLR.CLI
{
    public class CommandLineOptions
    {
        public const string DefaultComPort = "COM3";

        [Option('l', "load", Required = false, HelpText = "Loads list of assemblies formatted for nanoCLR")]
        public IEnumerable<string> Assemblies { get; set; }

        [Option('b', "loadDatabase", Required = false, HelpText = "Loads a set of assemblies")]
        public string AssembliesSet { get; set; }

        [Option('d', "debug", Required = false, HelpText = "Set COM port for debug")]
        public string DebugPort { get; set; }

        [Option('t', "traceWire", Required = false, HelpText = "Trace wire data packets")]
        public bool TraceWire { get; set; }

        [Option('r', "resolve", Required = false, HelpText = "Tries to resolve cross-assembly references")]
        public bool TryResolve { get; set; }
    }
}
