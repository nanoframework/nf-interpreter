//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using nanoFramework.nanoCLR.Host.Port.Common;
using System.IO.Pipes;

namespace nanoFramework.nanoCLR.Host.Port.NamedPipe
{
    internal class NamedPipeServerPort : ListeningPortBase
    {
        private const int FixedBufferSize = 1024;
        private const int MaxThreads = 32;

        private readonly string _pipeName;

        public NamedPipeServerPort(string pipeName)
        {
            _pipeName = pipeName;
        }

        protected override void OpenListener()
        {
        }

        protected override void CloseListener()
        {
        }

        protected override IPort WaitForConnection()
        {
            var pipeServer = new NamedPipeServerStream(_pipeName, PipeDirection.InOut,
                maxNumberOfServerInstances: MaxThreads, PipeTransmissionMode.Byte, PipeOptions.Asynchronous);
            pipeServer.WaitForConnection();
            return new StreamedConnection(pipeServer,
                bytesAvailable: () => FixedBufferSize,
                releaseResources: () =>
                {
                    pipeServer.Close();
                    pipeServer.Dispose();
                }
            );
        }
    }
}
