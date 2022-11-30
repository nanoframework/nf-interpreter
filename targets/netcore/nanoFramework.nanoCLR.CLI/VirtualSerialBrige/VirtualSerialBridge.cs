//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using hhdvspkit;
using System;

namespace nanoFramework.nanoCLR.CLI
{
    /// <summary>
    /// Object representing a Virtual Bridge.
    /// </summary>
    public class VirtualSerialBridge
    {
        public VirtualSerialBridge()
        {
        }

        internal VirtualSerialBridge(VirtualSerialDeviceManager virtualComManager, string bridgeName)
        {
            var (comA, comB) = VirtualSerialDeviceManager.ParseVirtualBridge(bridgeName);

            PortA = virtualComManager.GetVirtualPortDevice(comA);
            PortB = virtualComManager.GetVirtualPortDevice(comB);
        }

        /// <summary>
        /// Bridge Device for first port of the <see cref="VirtualSerialBridge"/>.
        /// </summary>
        public IBridgePortDevice PortA { get; set; }

        /// <summary>
        /// Bridge Device for second port of the <see cref="VirtualSerialBridge"/>.
        /// </summary>
        public IBridgePortDevice PortB { get; set; }

        /// <summary>
        /// Index of the <see cref="PortA"/>.
        /// </summary>
        public uint PortAIndex => PortA.bridgePort;

        /// <summary>
        /// Index of the <see cref="PortB"/>.
        /// </summary>
        public uint PortBIndex => PortB.bridgePort;

        /// <summary>
        /// Index for the other port of the <see cref="VirtualSerialBridge"/>.
        /// </summary>
        /// <returns></returns>
        public uint GetOtherPort(IBridgePortDevice port)
        {
            if(port.bridgePort != PortA.bridgePort && port.bridgePort != PortB.bridgePort)
            {
                throw new ArgumentException("The port parameter is not part of this VirtualBridge.");
            }

            if(port == PortA)
            {
                return PortBIndex;
            }

            if (port == PortB)
            {
                return PortAIndex;
            }

            // shouln't ever get here!!
            throw new InvalidOperationException();
        }

        /// <summary>
        /// Index for the other port of the <see cref="VirtualSerialBridge"/>.
        /// </summary>
        /// <returns></returns>
        public uint GetOtherPort(string port)
        {
            int portIndex = Utilities.GetPortIndex(port);

            if (portIndex != PortAIndex && portIndex != PortBIndex)
            {
                throw new ArgumentException("The port parameter is not part of this VirtualBridge.");
            }

            if (portIndex == PortAIndex)
            {
                return PortBIndex;
            }

            if (portIndex == PortBIndex)
            {
                return PortAIndex;
            }

            // shouln't ever get here!!
            throw new InvalidOperationException();
        }

        /// <inheritdoc/>
        public override string ToString() => $"COM{PortA.port}<->COM{PortB.port}";
    }
}
