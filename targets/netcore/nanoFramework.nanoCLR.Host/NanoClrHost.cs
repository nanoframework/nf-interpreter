using System;
using System.Collections.Generic;

namespace nanoFramework.nanoCLR.Host
{
    public class NanoClrHost
    {
        private bool _isRunning = false;
        internal List<Action> PreInitConfigureSteps { get; } = new();
        internal List<Action> ConfigureSteps { get; } = new();
        internal NanoClrSettings NanoClrSettings { get; set; } = NanoClrSettings.Default;

        internal NanoClrHost()
        {
        }

        public void Run()
        {
            if (_isRunning)
                return;
            _isRunning = true;
            Interop.NanoClr_SetConfigureCallback(ConfigureRuntime);
            PreInitConfigureRuntime();
            Interop.NanoClr_Run(NanoClrSettings);
        }

        private void PreInitConfigureRuntime() =>
            PreInitConfigureSteps.ForEach(s => s());

        private uint ConfigureRuntime()
        {
            ConfigureSteps.ForEach(s => s());
            return Interop.ClrOk;
        }

        public static NanoClrHostBuilder CreateBuilder() => new NanoClrHostBuilder { };
    }
}
