//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System;

namespace nanoFramework.nanoCLR.Host
{
    public interface IDeviceSink
    {
        void Initialize(Action<byte[]> transmitToDevice);
        void ReceivedFromDevice(byte[] bytes);
        void Open();
        void Close();
    }
}
