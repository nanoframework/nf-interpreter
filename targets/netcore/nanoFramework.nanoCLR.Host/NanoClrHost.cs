//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;

namespace nanoFramework.nanoCLR.Host
{
    public class NanoClrHost
    {
        private bool _isRunning = false;

        private Interop.WireTransmitDelegate _wireReceiveCallback;
        internal List<Action> PreInitConfigureSteps { get; } = new();
        internal List<Action> ConfigureSteps { get; } = new();
        internal NanoClrSettings NanoClrSettings { get; set; } = NanoClrSettings.Default;
        internal IDeviceSink WireProtocolSink { get; set; }

        internal NanoClrHost()
        {
        }

        public void Run()
        {
            if (_isRunning)
                return;
            _isRunning = true;

            Interop.NanoClr_SetConfigureCallback(ConfigureRuntime);

            if (WireProtocolSink != null)
                InitWireProtocolSink();

            PreInitConfigureRuntime();
            Interop.NanoClr_Run(NanoClrSettings);

            Cleanup();
        }

        private void Cleanup()
        {
            if (WireProtocolSink != null)
                WireProtocolSink.Close();
        }

        private void InitWireProtocolSink()
        {
            _wireReceiveCallback = new Interop.WireTransmitDelegate(WireReceiveCallback);
            Interop.NanoClr_SetWireTransmitCallback(_wireReceiveCallback);
            WireProtocolSink.Initialize(WireTransmitToDevice);
            WireProtocolSink.Open();
        }

        private void WireReceiveCallback(byte[] bytes, int length)
        {
            WireProtocolSink.ReceivedFromDevice(bytes);
        }

        private void WireTransmitToDevice(byte[] bytes)
        {
            Interop.NanoClr_WireReceive(bytes, bytes.Length);
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
