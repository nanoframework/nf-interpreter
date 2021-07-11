//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using nanoFramework.nanoCLR.Host.Interop;
using nanoFramework.nanoCLR.Host.Port;

namespace nanoFramework.nanoCLR.Host
{
    public class NanoClrHost
    {
        private bool _isRunning = false;
        private IChannel _wireProtocolChannel;

        internal List<Action> PreInitConfigureSteps { get; } = new();
        internal List<Action> ConfigureSteps { get; } = new();
        internal NanoClrSettings NanoClrSettings { get; set; } = NanoClrSettings.Default;
        internal IPort WireProtocolPort { get; set; }

        internal NanoClrHost()
        {
        }

        public void Run()
        {
            if (_isRunning)
                return;
            _isRunning = true;

            Native.NanoClr_SetConfigureCallback(ConfigureRuntime);

            if (WireProtocolPort != null)
                InitWireProtocol(WireProtocolPort);

            PreInitConfigureRuntime();
            Native.NanoClr_Run(NanoClrSettings);

            Cleanup();
        }

        private void Cleanup()
        {
            _wireProtocolChannel?.Disconnect();
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
            return Native.ClrOk;
        }

        public static NanoClrHostBuilder CreateBuilder() => new NanoClrHostBuilder { };
    }
}
