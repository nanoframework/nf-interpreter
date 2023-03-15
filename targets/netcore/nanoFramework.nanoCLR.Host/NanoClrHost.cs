//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using nanoFramework.nanoCLR.Host.Port;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

namespace nanoFramework.nanoCLR.Host
{
    public class nanoCLRHost
    {
        private bool _isRunning = false;
        private IChannel _wireProtocolChannel;

        internal List<Action> PreInitConfigureSteps { get; } = new();
        internal List<Action> ConfigureSteps { get; } = new();
        internal nanoCLRSettings nanoCLRSettings { get; set; } = nanoCLRSettings.Default;
        internal IPort WireProtocolPort { get; set; }

        internal nanoCLRHost(string pathForNanoClrDll)
        {
            // set app directory, if different from executing assembly
            if (!string.IsNullOrEmpty(pathForNanoClrDll)
               && pathForNanoClrDll != Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location))
            {
                Interop.nanoCLR.DllPath = pathForNanoClrDll;
            }
            else
            {
                // use default path
                Interop.nanoCLR.DllPath = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "NanoCLR");
            }

            // set path to search nanoCLR DLL
            _ = Interop.nanoCLR.SetDllDirectory(Interop.nanoCLR.DllPath);
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
            return Interop.nanoCLR.ClrOk;
        }

        public static nanoCLRHostBuilder CreateBuilder() => new nanoCLRHostBuilder { };

        public void Shutdown()
        {
            Cleanup();
        }
    }
}
