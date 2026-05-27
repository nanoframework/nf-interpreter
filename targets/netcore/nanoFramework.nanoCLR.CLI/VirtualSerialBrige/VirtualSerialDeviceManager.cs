//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using hhdvspkit;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;

// TODO: Add IsInstalled method that does the following:
// A) Get the guid from SerialPortLibraryClass
// B) Checks registry for the existence of the library ex: HKEY_CLASSES_ROOT\CLSID\{e13da62c-3a88-45a4-a5d0-224dea7bf4ff}
// TODO: Check license AFTER confirming installation so a more specific error can be displayed
namespace nanoFramework.nanoCLR.CLI
{
    public class VirtualSerialDeviceManager
    {
        private const string _licenseResourceName = "nanoFramework.nanoCLR.CLI.License.vspt.vsptlic";
        private static Regex s_PairRegex = new(@"COM(\d+):COM(\d+)", RegexOptions.IgnoreCase);

        // verbs and options for virtual serial device
        internal const string VirtualSerialDeviceVerb = "virtualserial";
        internal const string CreateOption = "create";
        internal const string RemoveOption = "remove";

        /// <summary>
        /// If Virtual Serial Port Tools are installed and functional this property is <see langword="true"/>, otherwise <see langword="false"/>.
        /// </summary>
        public bool IsFunctional { get; private set; }

        internal SerialPortLibrary _serialPortLibrary = null;

        public VirtualSerialDeviceManager()
        {
        }

        public bool Initialize()
        {
            try
            {
                _serialPortLibrary = new SerialPortLibrary();
                InstallLicense();
                IsFunctional = true;
            }
#if DEBUG
            catch (Exception ex)
            {
                Debug.WriteLine($"Exception when performing initialization: {ex}.");
#else
            catch
            {
#endif
                IsFunctional = false;
                return false;
            }

            return true;
        }

        private IBridgePortDevice[] GetVirtualPortDevices() =>
            _serialPortLibrary.getPorts(SerialPortType.Bridge).Cast<IBridgePortDevice>().ToArray();

        internal IBridgePortDevice GetVirtualPortDevice(int port) => GetVirtualPortDevices().FirstOrDefault(p => p.port == port);

        public IEnumerable<VirtualSerialBridge> GetAvailableVirtualBridges()
        {
            IBridgePortDevice[] bridgePorts = GetVirtualPortDevices();

            var bridges = new List<VirtualSerialBridge>();

            foreach (var port in bridgePorts)
            {
                if (port.bridgePort > 0 && bridges.All(b => b.PortA.port != port.bridgePort))
                {
                    bridges.Add(new VirtualSerialBridge { PortA = port, PortB = bridgePorts.First(p => p.port == port.bridgePort) });
                }
            }

            return bridges;
        }

        internal VirtualSerialBridge GetVirtualBridgeContainingPort(string port)
        {
            int portIndex = Utilities.GetPortIndex(port);

            // get all virtual bridges
            var bridges = GetAvailableVirtualBridges();

            // try to find this port index on any of the available virtual bridges
            foreach (var b in bridges)
            {
                if (b.PortAIndex == portIndex || b.PortBIndex == portIndex)
                {
                    // done here
                    return b;
                }
            }

            // couldn't find a bridge containing this port
            return null;
        }

        public VirtualSerialBridge CreateVirtualBridge(int comA, int comB)
        {
            try
            {
                IBridgePortDevice portA = GetVirtualPortDevice(comA) ?? _serialPortLibrary.createBridgePort(comA);
                IBridgePortDevice portB = GetVirtualPortDevice(comB) ?? _serialPortLibrary.createBridgePort(comB);

                portA.bridgePort = portB.port;
                portB.bridgePort = portA.port;

                return new VirtualSerialBridge { PortA = portA, PortB = portB };
            }
            catch (Exception e)
            {
                throw new InvalidOperationException($@"Cannot create Virtual Bridge. {e.Message}", e);
            }
        }

        public VirtualSerialBridge CreateVirtualBridge(string bridgeName)
        {
            var (comA, comB) = ParseVirtualBridge(bridgeName);

            return CreateVirtualBridge(comA, comB);
        }

        public static void RemoveVirtualBridge(VirtualSerialBridge bridge)
        {
            //Not working due to interop issues between 0x86/0x64 code. Error: 0xE0000235. Vendor issue?
            try
            {
                bridge.PortA.bridgePort = 0;
                bridge.PortB.bridgePort = 0;

                bridge.PortA.deleteDevice();
                bridge.PortB.deleteDevice();
            }
            catch (Exception e)
            {
                throw new InvalidOperationException($@"Cannot Delete Port Pair. {e.Message}", e);
            }
        }

        public void DeleteVirtualBridge(string bridgeName)
        {
            VirtualSerialBridge pair = GetVirtualBridgeByName(bridgeName);

            if (pair == null)
            {
                throw new ArgumentException($"Virtual COM Pair {bridgeName.ToUpper()} not found.");
            }

            RemoveVirtualBridge(pair);
        }

        public static (int comA, int comB) ParseVirtualBridge(string bridgeName)
        {
            Match match = s_PairRegex.Match(bridgeName);

            if (!match.Success)
            {
                throw new ArgumentException(
                    $"Incorrect Virtual COM Pair name {bridgeName.ToUpper()}. Correct example COM5:COM6");
            }

            var comA = int.Parse(match.Groups[1].Value);
            var comB = int.Parse(match.Groups[2].Value);

            return (comA, comB);
        }

        public VirtualSerialBridge GetVirtualBridgeByName(string bridgeName)
        {
            var (comA, comB) = ParseVirtualBridge(bridgeName);

            IBridgePortDevice portA = GetVirtualPortDevice(comA);
            IBridgePortDevice portB = GetVirtualPortDevice(comB);

            if (portA == null || portB == null)
            {
                return null;
            }

            return new VirtualSerialBridge { PortA = portA, PortB = portB };
        }

        private void InstallLicense()
        {
            var assembly = typeof(VirtualSerialDeviceManager).GetTypeInfo().Assembly;

            using var resource = assembly.GetManifestResourceStream(_licenseResourceName);
            if (resource is null)
            {
                // TODO: Should probably return an error
                return;
            }

            using var memoryStream = new MemoryStream();

            resource.CopyTo(memoryStream);
            var bytes = memoryStream.ToArray();

            if (bytes.Length <= 0)
            {
                // TODO: Should probably return an error
                return;
            }
            
            // This appears to be a misleading name as the license appears to be persisted
            _serialPortLibrary.installLicenseInMemory(bytes);
        }
    }
}
