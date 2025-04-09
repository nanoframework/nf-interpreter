//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Threading;

namespace nanoFramework.nanoCLR.Host.Port.Common
{
    public abstract class ListeningPortBase : IPort
    {
        private const int WaitTimeout = 100;

        private readonly object _syncRoot = new object();
        private Thread _listeningThread;
        private IPort _activeConnection;

        public PortStatus Status { get; private set; }
        public int BytesAvailable => _syncRoot.Sync(() => _activeConnection?.BytesAvailable ?? 0);
        public byte[] ReceiveData() => _syncRoot.Sync(() => _activeConnection?.ReceiveData());
        public void TransmitData(byte[] data) => _syncRoot.Sync(() => _activeConnection?.TransmitData(data));

        public void Process()
        {
            // empty on purpose, nothing to process here
        }

        protected abstract void OpenListener();
        protected abstract void CloseListener();
        protected abstract IPort WaitForConnection();

        public void Open()
        {
            OpenListener();
            Status = PortStatus.Opened;
            _listeningThread = new Thread(ListenForConnections);
            _listeningThread.Start();
        }

        public void Close()
        {
            CloseListener();
            Status = PortStatus.Closed;
        }

        private void ListenForConnections()
        {
            while (Status == PortStatus.Opened)
            {
                try
                {
                    IPort connection = WaitForConnection();
                    _syncRoot.Sync(() =>
                    {
                        _activeConnection?.Close();
                        _activeConnection = connection;
                        _activeConnection.Open();
                    });

                    Thread.Sleep(WaitTimeout);
                }
                catch (Exception e)
                {
                    Console.WriteLine($"Listening thread exception: {e}");
                }
            }
        }
    }
}
