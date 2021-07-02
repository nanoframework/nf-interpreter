using System.Runtime.InteropServices;
using nanoFramework.nanoCLR.Host;

namespace nanoFramework.nanoCLR.CLI
{
    class Program
    {
        static void Main(string[] args)
        {
            Interop.nanoclr_run(args.Length, args);
        }
    }
}
