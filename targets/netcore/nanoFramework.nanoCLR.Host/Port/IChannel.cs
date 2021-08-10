//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

namespace nanoFramework.nanoCLR.Host.Port
{
    public interface IChannel
    {
        IPort PortA { get; }
        IPort PortB { get; }

        public void Connect();
        public void Disconnect();
    }
}
