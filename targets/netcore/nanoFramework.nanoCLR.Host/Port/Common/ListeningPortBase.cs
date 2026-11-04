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
        private DateTime _holdOffUntil = DateTime.MinValue;

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

        /// <summary>
        /// Called when a connection becomes the active one.
        /// </summary>
        protected virtual void OnConnectionOpened()
        {
        }

        /// <summary>
        /// Called when the active connection is closed.
        /// </summary>
        protected virtual void OnConnectionClosed()
        {
        }

        /// <summary>
        /// Closes the active connection, if there is one, and rejects the new connections for a while.
        /// </summary>
        /// <param name="holdOff">Time to reject new connections.</param>
        protected void DropActiveConnection(TimeSpan holdOff)
        {
            _syncRoot.Sync(() =>
            {
                if (holdOff > TimeSpan.Zero)
                {
                    _holdOffUntil = DateTime.UtcNow + holdOff;
                }

                CloseActiveConnection();
            });
        }

        // has to be called with the lock
        private void CloseActiveConnection()
        {
            if (_activeConnection != null)
            {
                var connection = _activeConnection;
                _activeConnection = null;

                connection.Close();

                OnConnectionClosed();
            }
        }

        public void Open()
        {
            OpenListener();
            Status = PortStatus.Opened;
            _listeningThread = new Thread(ListenForConnections);
            _listeningThread.Start();
        }

        public void Close()
        {
            // first the status, so the listening thread knows the failure of the accept is because of the close
            Status = PortStatus.Closed;
            CloseListener();
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
                        if (DateTime.UtcNow < _holdOffUntil)
                        {
                            // holding off, reject this one
                            connection.Close();

                            return;
                        }

                        CloseActiveConnection();

                        _activeConnection = connection;
                        _activeConnection.Open();

                        OnConnectionOpened();
                    });

                    Thread.Sleep(WaitTimeout);
                }
                catch (Exception e)
                {
                    if (Status != PortStatus.Opened)
                    {
                        // the port was closed while waiting for a connection
                        break;
                    }

                    Console.WriteLine($"Listening thread exception: {e}");
                }
            }
        }
    }
}
