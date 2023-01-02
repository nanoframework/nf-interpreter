//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Net;
using System.Net.Sockets;
using nanoFramework.nanoCLR.Host.Port.Common;

namespace nanoFramework.nanoCLR.Host.Port.TcpIp
{
    public class TcpIpListeningPort : ListeningPortBase
    {
        public const string Localhost = "127.0.0.1";

        private readonly string _hostAddress;
        private readonly int _port;
        private readonly TcpListener _server;
        private readonly NetworkWireBroadcastService _broadcastService;

        public TcpIpListeningPort(string hostAddress, int port, int? broadcastPort)
        {
            _hostAddress = hostAddress;
            _port = port;
            _server = new TcpListener(IPAddress.Parse(hostAddress), port);
            if (broadcastPort != null)
                _broadcastService = new NetworkWireBroadcastService(broadcastPort.Value);
        }

        protected override void OpenListener()
        {
            _server.Start();
            _broadcastService?.NotifyListenerStart(_hostAddress, _port);
        }

        protected override void CloseListener()
        {
            _broadcastService?.NotifyListenerStop(_hostAddress, _port);
            _server.Stop();
        }

        protected override IPort WaitForConnection()
        {
            var client = _server.AcceptTcpClient();
            return new StreamedConnection(client.GetStream(),
                bytesAvailable: () => client.Available,
                releaseResources: () =>
                {
                    client.Close();
                    client.Dispose();
                }
            );
        }
    }
}
