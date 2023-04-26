//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using Newtonsoft.Json;
using System;

namespace nanoFramework.nanoCLR.CLI
{
    [Serializable]
    internal class CloudsmithPackageInfo
    {
        [JsonProperty("version")]
        public string Version { get; set; }

        [JsonProperty("cdn_url")]
        public string DownloadUrl { get; set; }

        [JsonProperty("name")]
        public string TargetName { get; set; }

        [JsonProperty("uploaded_at")]
        public DateTime PackageDate { get; set; }
    }
}
