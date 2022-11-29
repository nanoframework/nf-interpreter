﻿//
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

namespace nanoFramework.nanoCLR.CLI
{
    public class VirtualDeviceManager
    {
        private const string _licenseResourceName = "nanoFramework.nanoCLR.CLI.License.vspt.vsptlic";
        private static Regex s_PairRegex = new(@"COM(\d+):COM(\d+)", RegexOptions.IgnoreCase);

        // verbs and options for virtual device
        internal const string VirtualDeviceVerb = "virtualdevice";
        internal const string CreateOption = "create";
        internal const string DeleteOption = "delete";

        /// <summary>
        /// If Virtual Serial Port Tools are installed and functional this property is <see langword="true"/>, otherwise <see langword="false"/>.
        /// </summary>
        public bool IsFunctional { get; private set; }

        internal SerialPortLibrary _serialPortLibrary = null;

        public VirtualDeviceManager()
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

        public IEnumerable<VirtualBridge> GetAvailableVirtualBridges()
        {
            IBridgePortDevice[] bridgePorts = GetVirtualPortDevices();

            var bridges = new List<VirtualBridge>();

            foreach (var port in bridgePorts)
            {
                if (port.bridgePort > 0 && bridges.All(b => b.PortA.port != port.bridgePort))
                {
                    bridges.Add(new VirtualBridge { PortA = port, PortB = bridgePorts.First(p => p.port == port.bridgePort) });
                }
            }

            return bridges;
        }

        internal VirtualBridge GetVirtualBridgeContainingPort(string port)
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

        public VirtualBridge CreateVirtualBridge(int comA, int comB)
        {
            try
            {
                IBridgePortDevice portA = GetVirtualPortDevice(comA) ?? _serialPortLibrary.createBridgePort(comA);
                IBridgePortDevice portB = GetVirtualPortDevice(comB) ?? _serialPortLibrary.createBridgePort(comB);

                portA.bridgePort = portB.port;
                portB.bridgePort = portA.port;

                return new VirtualBridge { PortA = portA, PortB = portB };
            }
            catch (Exception e)
            {
                throw new InvalidOperationException($@"Cannot create Virtual Bridge. {e.Message}", e);
            }
        }

        public VirtualBridge CreateVirtualBridge(string bridgeName)
        {
            var (comA, comB) = ParseVirtualBridge(bridgeName);

            return CreateVirtualBridge(comA, comB);
        }

        public static void DeleteVirtualBridge(VirtualBridge bridge)
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
            VirtualBridge pair = GetVirtualBridgeByName(bridgeName);

            if (pair == null)
            {
                throw new ArgumentException($"Virtual COM Pair {bridgeName.ToUpper()} not found.");
            }

            DeleteVirtualBridge(pair);
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

        public VirtualBridge GetVirtualBridgeByName(string bridgeName)
        {
            var (comA, comB) = ParseVirtualBridge(bridgeName);

            IBridgePortDevice portA = GetVirtualPortDevice(comA);
            IBridgePortDevice portB = GetVirtualPortDevice(comB);

            if (portA == null || portB == null)
            {
                return null;
            }

            return new VirtualBridge { PortA = portA, PortB = portB };
        }

        private void InstallLicense()
        {
            var assembly = typeof(VirtualDeviceManager).GetTypeInfo().Assembly;

            Stream resource = assembly.GetManifestResourceStream(_licenseResourceName);
            MemoryStream memoryStream = new MemoryStream();

            resource.CopyTo(memoryStream);
            var bytes = memoryStream.ToArray();

            if (bytes.Length > 0)
            {
                _serialPortLibrary.installLicenseInMemory(bytes);
            }
        }
    }
}
