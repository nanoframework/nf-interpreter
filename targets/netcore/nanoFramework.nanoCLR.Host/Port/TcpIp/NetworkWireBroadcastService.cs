//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Net;
using System.Net.Sockets;
using System.Text;

namespace nanoFramework.nanoCLR.Host.Port.TcpIp
{
    /// <summary>
    /// Network Device UDP broadcast service class.
    /// </summary>
    public class NetworkWireBroadcastService
    {
        /// <summary>
        /// Default broadcast port.
        /// </summary>        
        public const int DebugBroadcastPort = 23657;
        
        private const string CommandDeviceStart = "+";
        private const string CommandDeviceStop = "-";

        private readonly int _broadcastPort;

        /// <summary>
        /// Instantiate broadcast service for specific port.
        /// </summary>
        /// <param name="broadcastPort">Broadcast port</param>
        public NetworkWireBroadcastService(int broadcastPort)
        {
            _broadcastPort = broadcastPort;
        }

        /// <summary>
        /// Notify local network debugger that device is up and ready for connection. Example message +:192.168.0.10:8500
        /// </summary>
        public void NotifyListenerStart(string host, int port) => SendMessage(GetCommand(CommandDeviceStart, host, port)); 

        /// <summary>
        /// Notify local network debugger that device is shutting down. Example message -:192.168.0.10:8500
        /// </summary>        
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
