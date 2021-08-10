//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using hhdvspkit;

namespace nanoFramework.nanoCLR.VirtualCom
{
    public class PortPair
    {
        public IBridgePortDevice PortA { get; set; }
        public IBridgePortDevice PortB { get; set; }

        public override string ToString() => $"COM{PortA.port}:COM{PortB.port}";
    }
}
