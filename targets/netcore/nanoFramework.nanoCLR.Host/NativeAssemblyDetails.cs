// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace nanoFramework.nanoCLR.Host
{
    /// <summary>
    /// Information about a native assembly.
    /// </summary>
    public sealed class NativeAssemblyDetails
    {
        #region Properties
        /// <summary>
        /// Gets the name of the assembly.
        /// </summary>
        public string Name
        {
            get;
        }

        /// <summary>
        /// Gets the version of the assembly.
        /// </summary>
        public Version Version
        {
            get;
        }

        /// <summary>
        /// Gets the checksum of the assembly.
        /// </summary>
        public uint CheckSum
        {
            get;
        }
        #endregion

        #region Construction
        /// <summary>
        /// Get the assembly information from the nanoCLR instance.
        /// </summary>
        /// <returns>Returns a list with the metadata of native assemblies. If the runtime does
        /// not support this, <c>null</c> is returned.</returns>
        public static List<NativeAssemblyDetails> Get()
        {
            var result = new List<NativeAssemblyDetails>();

            ushort numAssemblies;
            try
            {
                numAssemblies = Interop.nanoCLR.nanoCLR_GetNativeAssemblyCount();
            }
            catch (EntryPointNotFoundException)
            {
                return null;
            }

            if (numAssemblies == 0)
            {
                return result;
            }

            // assembly data size is coming from debugger library: NativeAssemblyDetails.Size
            const int assemblyDataSize = 4 + 4 * 2 + 128 * 1;

            byte[] data = new byte[numAssemblies * assemblyDataSize];

            if (!Interop.nanoCLR.nanoCLR_GetNativeAssemblyInformation(data, data.Length))
            {
                return null;
            }

            using var buffer = new MemoryStream(data);
            using var reader = new BinaryReader(buffer);
            for (int i = 0; i < numAssemblies; i++)
            {
                result.Add(new NativeAssemblyDetails(reader));
            }

            return result;
        }

        /// <summary>
        /// Deserialize the description of the native assembly.
        /// In nf-debugger this is done via a converter. The code in this
        /// constructor is essentially the same.
        /// </summary>
        /// <param name="reader"></param>
        private NativeAssemblyDetails(BinaryReader reader)
        {
            CheckSum = reader.ReadUInt32();
            Version = new Version(
                reader.ReadUInt16(),
                reader.ReadUInt16(),
                reader.ReadUInt16(),
                reader.ReadUInt16()
            );
            Name = GetZeroTerminatedString(reader.ReadBytes(128), true);
        }
        #endregion

        #region Helpers
        /// <summary>
        /// Original is in nf-debugger\nanoFramework.Tools.DebugLibrary.Shared\WireProtocol\Commands.cs
        /// </summary>
        /// <param name="buf"></param>
        /// <param name="fUTF8"></param>
        /// <returns></returns>
        private static string GetZeroTerminatedString(byte[] buf, bool fUTF8)
        {
            if (buf is null)
            {
                return null;
            }

            int len = 0;
            int num = buf.Length;

            while (len < num && buf[len] != 0)
            {
                len++;
            }

            return fUTF8 ? Encoding.UTF8.GetString(buf, 0, len) : Encoding.ASCII.GetString(buf, 0, len);
        }
        #endregion
    }
}
