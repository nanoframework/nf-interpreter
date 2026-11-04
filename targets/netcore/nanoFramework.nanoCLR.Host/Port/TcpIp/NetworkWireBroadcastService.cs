//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace nanoFramework.nanoCLR.Host.Port.TcpIp
{
    /// <summary>
    /// Network Device UDP broadcast service class.
    /// Announces the device every 5 seconds, so the debuggers on the network can find it.
    /// </summary>
    /// <remarks>
    /// Messages are ASCII and use the same format as the devices:
    /// "+:&lt;port&gt;:&lt;targetName&gt;" when the device is up and "-:&lt;port&gt;" when it's going away.
    /// The IP address is not part of the message, the debugger takes it from the sender of the datagram.
    /// </remarks>
    public sealed class NetworkWireBroadcastService : IDisposable
    {
        /// <summary>
        /// Default broadcast port.
        /// </summary>
        public const int DebugBroadcastPort = 23657;

        /// <summary>
        /// Interval of the announcements.
        /// </summary>
        public static readonly TimeSpan AnnounceInterval = TimeSpan.FromSeconds(5);

        private const string CommandDeviceStart = "+";
        private const string CommandDeviceStop = "-";

        private readonly int _broadcastPort;
        private readonly int _devicePort;
        private readonly string _targetName;
        private readonly object _syncRoot = new object();

        private Timer _timer;
        private bool _disposed;

        /// <summary>
        /// Instantiate broadcast service for specific port.
        /// </summary>
        /// <param name="broadcastPort">Broadcast port.</param>
        /// <param name="devicePort">Port where the device listens for the debugger.</param>
        /// <param name="targetName">Name of the target to announce.</param>
        public NetworkWireBroadcastService(int broadcastPort, int devicePort, string targetName)
        {
            _broadcastPort = broadcastPort;
            _devicePort = devicePort;
            _targetName = targetName;
        }

        /// <summary>
        /// Starts announcing the device, now and every 5 seconds. Example message +:26000:CLR_WIN32
        /// </summary>
        public void NotifyListenerStart()
        {
            lock (_syncRoot)
            {
                if (_disposed || _timer != null)
                {
                    return;
                }

                _timer = new Timer(_ => SendMessage(GetStartMessage()), null, TimeSpan.Zero, AnnounceInterval);
            }
        }

        /// <summary>
        /// Notify local network debugger that device is shutting down. Example message -:26000
        /// </summary>
        public void NotifyListenerStop()
        {
            lock (_syncRoot)
            {
                _timer?.Dispose();
                _timer = null;
            }

            SendMessage($"{CommandDeviceStop}:{_devicePort}");
        }

        public void Dispose()
        {
            lock (_syncRoot)
            {
                if (_disposed)
                {
                    return;
                }

                _disposed = true;
            }

            NotifyListenerStop();
        }

        private string GetStartMessage() => $"{CommandDeviceStart}:{_devicePort}:{_targetName}";

        private void SendMessage(string message)
        {
            try
            {
                var group = new IPEndPoint(IPAddress.Broadcast, _broadcastPort);
                var data = Encoding.ASCII.GetBytes(message);

                using (var udpClient = new UdpClient())
                {
                    udpClient.EnableBroadcast = true;
                    udpClient.Send(data, data.Length, group);
                }
            }
            catch (SocketException)
            {
                // the network may not be available (yet), the next announcement will go out
            }
        }
    }
}
