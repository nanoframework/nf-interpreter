//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Net;
using System.Net.Sockets;
using System.Text;

namespace nanoFramework.nanoCLR.Host.Port.TcpIp
{
    public class NetworkWireDiscoveryService
    {
        public const int DebugBroadcastPort = 23657;
        private const string CommandDeviceStart = "+";
        private const string CommandDeviceStop = "-";

        private readonly int _broadcastPort;
        
        public NetworkWireDiscoveryService(int broadcastPort)
        {
            _broadcastPort = broadcastPort;
        }

        public void NotifyListenerStart(string host, int port) => SendMessage(GetCommand(CommandDeviceStart, host, port)); 

        public void NotifyListenerStop(string host, int port) => SendMessage(GetCommand(CommandDeviceStop, host, port));

        private string GetCommand(string command, params object[] args) => $"{command}:{string.Join(":", args)}";

        private void SendMessage(string message)
        {
            var group = new IPEndPoint(IPAddress.Broadcast, _broadcastPort);
            var udpClient = new UdpClient();
            var data =  Encoding.ASCII.GetBytes(message);

            udpClient.Send(data, data.Length, group);
        }
    }
}