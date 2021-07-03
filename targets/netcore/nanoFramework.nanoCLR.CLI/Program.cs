using System;
using System.Linq;
using CommandLine;
using nanoFramework.nanoCLR.Host;

namespace nanoFramework.nanoCLR.CLI
{
    class Program
    {
        static void Main(string[] args)
        {
            LogErrors(() =>
            {
                var hostBuilder = NanoClrHost.CreateBuilder();

                Parser.Default.ParseArguments<CommandLineOptions>(args).WithParsed(o =>
                {
                    hostBuilder.WaitForDebugger = o.WaitForDebugger;
                    hostBuilder.EnterDebuggerLoopAfterExit = o.EnterDebuggerLoopAfterExit;

                    if (o.Assemblies.Any())
                        hostBuilder.LoadAssemblies(o.Assemblies);

                    if (o.AssembliesSet != null)
                        hostBuilder.LoadAssembliesSet(o.AssembliesSet);

                    if (o.TryResolve)
                        hostBuilder.TryResolve();

                    if (o.Debug)
                        hostBuilder.UseConsoleDebugPrint();

                    hostBuilder.Build().Run();
                });
            });
        }

        private static void LogErrors(Action scope)
        {
            try
            {
                scope();
            }
            catch (Exception e)
            {
                Console.WriteLine($"Error: {e.Message}");
            }
        }
    }
}
