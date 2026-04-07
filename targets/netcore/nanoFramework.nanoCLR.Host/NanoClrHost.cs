//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using nanoFramework.nanoCLR.Host.Port;
using System;
using System.Collections.Generic;

namespace nanoFramework.nanoCLR.Host
{
    public class nanoCLRHost
    {
        private bool _isRunning = false;
        private IChannel _wireProtocolChannel;

        internal List<Action> PreInitConfigureSteps { get; } = new();
        internal List<Action> ConfigureSteps { get; } = new();
        internal List<Action> CleanupSteps { get; } = new();
        internal nanoCLRSettings nanoCLRSettings { get; set; } = nanoCLRSettings.Default;
        internal IPort WireProtocolPort { get; set; }

        internal nanoCLRHost()
        {

        }

        public void Run()
        {
            if (_isRunning)
            {
                return;
            }

            _isRunning = true;

            Interop.nanoCLR.nanoCLR_SetConfigureCallback(this.ConfigureRuntime);

            if (WireProtocolPort != null)
            {
                InitWireProtocol(WireProtocolPort);
            }

            PreInitConfigureRuntime();
            Interop.nanoCLR.nanoCLR_Run(nanoCLRSettings);

            Cleanup();
        }

        private void Cleanup()
        {
            try
            {
                _wireProtocolChannel?.Disconnect();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error during wire protocol cleanup: {ex.Message}");
            }

            try
            {
                CleanupRuntime();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error during runtime cleanup: {ex.Message}");
            }
        }

        private void InitWireProtocol(IPort wireProtocolPort)
        {
            _wireProtocolChannel = new Channel(wireProtocolPort, new NativeWireProtocolPort());
            _wireProtocolChannel.Connect();
        }

        private void PreInitConfigureRuntime() =>
            PreInitConfigureSteps.ForEach(s => s());

        private uint ConfigureRuntime()
        {
            ConfigureSteps.ForEach(s => s());
            return Interop.nanoCLR.ClrOk;
        }

        private void CleanupRuntime() => CleanupSteps.ForEach(s => s());

        public static nanoCLRHostBuilder CreateBuilder() => new nanoCLRHostBuilder() { };

        public static nanoCLRHostBuilder CreateBuilder(string dllPath) => new nanoCLRHostBuilder(dllPath) { };

        public void Shutdown()
        {
            Cleanup();
        }
    }
}
