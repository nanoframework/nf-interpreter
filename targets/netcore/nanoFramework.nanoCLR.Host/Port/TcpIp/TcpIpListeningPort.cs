//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using nanoFramework.nanoCLR.Host.Port.Common;

namespace nanoFramework.nanoCLR.Host.Port.TcpIp
{
    public class TcpIpListeningPort : ListeningPortBase
    {
        public const string Localhost = "127.0.0.1";

        /// <summary>
        /// Default TCP port for debugging over the network. Same as the devices.
        /// </summary>
        public const int DefaultPort = 26000;

        /// <summary>
        /// Maximum length of the authentication token, in UTF-8 bytes.
        /// </summary>
        public const int MaxTokenLength = 64;

        // name announced by the discovery, it can't have ':'
        private const string TargetName = "CLR_VIRTUAL";

        // time to reject new connections after too many failed authentications
        private static readonly TimeSpan s_holdOffAfterAuthenticationFailures = TimeSpan.FromSeconds(1);

        private readonly string _token;
        private readonly TcpListener _server;
        private readonly NetworkWireBroadcastService _broadcastService;

        // keep the delegate alive as long as the native code can call it
        private readonly Interop.nanoCLR.WireDropClientDelegate _dropClientCallback;

        /// <summary>
        /// Listening port that accepts a debugger over TCP/IP.
        /// </summary>
        /// <param name="hostAddress">Address to bind to.</param>
        /// <param name="port">TCP port to listen to.</param>
        /// <param name="broadcastPort">UDP port to announce the device. Null to not announce.</param>
        /// <param name="token">Token that the debugger has to present. Null or empty for no authentication.</param>
        public TcpIpListeningPort(string hostAddress, int port, int? broadcastPort, string token = null)
        {
            if (token != null && Encoding.UTF8.GetByteCount(token) > MaxTokenLength)
            {
                throw new ArgumentException($"The token can't be longer than {MaxTokenLength} UTF-8 bytes.", nameof(token));
            }

            var address = IPAddress.Parse(hostAddress);

            _token = token ?? string.Empty;
            _server = new TcpListener(address, port);

            if (broadcastPort != null)
            {
                _broadcastService = new NetworkWireBroadcastService(
                    broadcastPort.Value,
                    port,
                    TargetName,
                    address);
            }

            _dropClientCallback = () =>
            {
                // the session that failed to authenticate is the one that is active now
                var session = ActiveConnection;

                if (session == null)
                {
                    return;
                }

                // let the reply that was just sent go out before closing
                Thread.Sleep(50);

                // only that session, another one may have connected in the meantime
                DropActiveConnection(s_holdOffAfterAuthenticationFailures, session);
            };
        }

        protected override void OpenListener()
        {
            // set the token, there's no session yet
            Interop.nanoCLR.nanoCLR_SetWireProtocolChannelInfo(false, _token);
            Interop.nanoCLR.nanoCLR_SetWireProtocolDropClientCallback(_dropClientCallback);

            _server.Start();
            _broadcastService?.NotifyListenerStart();
        }

        protected override void CloseListener()
        {
            _broadcastService?.Dispose();
            _server.Stop();

            DropActiveConnection(TimeSpan.Zero);
        }

        protected override void OnConnectionOpened()
        {
            // new session over the network, the debugger has to authenticate
            Interop.nanoCLR.nanoCLR_SetWireProtocolChannelInfo(true, _token);
        }

        protected override void OnConnectionClosed()
        {
            // session is gone, so it's no longer authenticated
            Interop.nanoCLR.nanoCLR_SetWireProtocolChannelInfo(false, null);
        }

        protected override IPort WaitForConnection()
        {
            var client = _server.AcceptTcpClient();

            try
            {
                client.NoDelay = true;
                client.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.KeepAlive, true);

                return new StreamedConnection(client.GetStream(),
                    bytesAvailable: () =>
                    {
                        var available = client.Available;

                        if (available == 0 && IsClientGone(client))
                        {
                            // the debugger went away
                            DropActiveConnection(TimeSpan.Zero);
                        }

                        return available;
                    },
                    releaseResources: () =>
                    {
                        client.Close();
                        client.Dispose();
                    }
                );
            }
            catch
            {
                // not handed over to a connection, nobody else will release it
                client.Dispose();

                throw;
            }
        }

        private static bool IsClientGone(TcpClient client)
        {
            try
            {
                // readable with nothing to read means the connection was closed (or reset)
                return client.Client.Poll(0, SelectMode.SelectRead) && client.Client.Available == 0;
            }
            catch (Exception ex) when (ex is SocketException || ex is ObjectDisposedException)
            {
                return true;
            }
        }
    }
}
