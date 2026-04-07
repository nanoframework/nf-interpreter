//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Text.Json.Serialization;

namespace nanoFramework.nanoCLR.CLI
{
    [Serializable]
    internal class CloudsmithPackageInfo
    {
        [JsonPropertyName("version")]
        public string Version { get; set; }

        [JsonPropertyName("cdn_url")]
        public string DownloadUrl { get; set; }

        [JsonPropertyName("name")]
        public string TargetName { get; set; }

        [JsonPropertyName("uploaded_at")]
        public DateTime PackageDate { get; set; }
    }
}
