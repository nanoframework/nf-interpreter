//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using hhdvspkit;

namespace nanoFramework.nanoCLR.VirtualBridge
{
    public class VirtualBridge
    {
        public IBridgePortDevice PortA { get; set; }
        public IBridgePortDevice PortB { get; set; }

        public override string ToString() => $"COM{PortA.port}<->COM{PortB.port}";
    }
}
