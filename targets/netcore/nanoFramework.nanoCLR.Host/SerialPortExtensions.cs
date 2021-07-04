//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;
using System.IO.Ports;

namespace nanoFramework.nanoCLR.Host
{
    public static class SerialPortExtensions
    {
        public static byte[] ReadAllBytes(this SerialPort port)
        {
            var bytes = new List<byte>();
            do
            {
                var buff = new byte[port.BytesToRead];
                port.Read(buff, 0, buff.Length);
                bytes.AddRange(buff);
            } while (port.BytesToRead != 0);

            return bytes.ToArray();
        }
    }
}
