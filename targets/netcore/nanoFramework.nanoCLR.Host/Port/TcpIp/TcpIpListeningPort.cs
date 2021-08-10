//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System.Net;
using System.Net.Sockets;
using nanoFramework.nanoCLR.Host.Port.Common;

namespace nanoFramework.nanoCLR.Host.Port.TcpIp
{
    public class TcpIpListeningPort : ListeningPortBase
    {
        private readonly TcpListener _server;
        private const string LocalHost = "127.0.0.1";

        public TcpIpListeningPort(int port)
        {
            var local = IPAddress.Parse(LocalHost);
            _server = new TcpListener(local, port);
        }

        protected override void OpenListener() => _server.Start();

        protected override void CloseListener() => _server.Stop();

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
