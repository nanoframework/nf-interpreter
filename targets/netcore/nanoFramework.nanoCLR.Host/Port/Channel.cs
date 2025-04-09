//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Threading;

namespace nanoFramework.nanoCLR.Host.Port
{
    internal class Channel : IChannel
    {
        private bool _connected = false;
        private readonly Thread _listeningThread;
        public IPort PortA { get; private set; }
        public IPort PortB { get; private set; }

        public Channel(IPort portA, IPort portB)
        {
            PortA = portA;
            PortB = portB;
            _listeningThread = new Thread(Listen);
        }

        public void Connect()
        {
            PortA.Open();
            PortB.Open();
            _connected = true;
            _listeningThread.Start();
        }

        public void Disconnect()
        {
            _connected = false;
            PortA.Close();
            PortB.Close();
        }

        private void Listen()
        {
            while (_connected)
            {
                if (PortA.BytesAvailable > 0)
                {
                    PortB.TransmitData(PortA.ReceiveData());
                }

                PortB.Process();
                if (PortB.BytesAvailable > 0)
                {
                    PortA.TransmitData(PortB.ReceiveData());
                }

                PortA.Process();
                if (PortA.BytesAvailable == 0 && PortB.BytesAvailable == 0)
                {
                    Thread.Sleep(100);
                }
            }
        }
    }
}
