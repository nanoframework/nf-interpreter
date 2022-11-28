//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using nanoFramework.nanoCLR.Host;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Reflection;
using System.Threading.Tasks;

namespace nanoFramework.nanoCLR.CLI
{
    public class CLRCommandLineProcessor
    {
        private const string _cloudSmithApiUrl = "https://api.cloudsmith.io/v1/packages/net-nanoframework/";

        public static int ProcessVerb(
            CLRCommandLineOptions options,
            nanoCLRHostBuilder hostBuilder)
        {
            Program.ProcessVerbosityOptions(options.Verbosity);

            if (options.UpdateCLR)
            {
                return (int)UpdateNanoCLRAsync(options.TargetVersion, hostBuilder.GetCLRVersion()).Result;
            }
            else if (options.GetCLRVersion)
            {
                Console.WriteLine($"nanoCLR version: {hostBuilder.GetCLRVersion()}");

                return (int)ExitCode.OK;
            }

            return (int)ExitCode.OK;
        }

        private static async Task<ExitCode> UpdateNanoCLRAsync(
            string targetVersion,
            string currentVersion)
        {
            try
            {
                // compose current version
                Version version = Version.Parse(currentVersion);

                string nanoClrDllLocation = Path.Combine(Assembly.GetExecutingAssembly().Location, "nanoFramework.nanoClr.dll");

                using var cloudsmithClient = new HttpClient();
                cloudsmithClient.BaseAddress = new Uri(_cloudSmithApiUrl);
                cloudsmithClient.DefaultRequestHeaders.Add("Accept", "*/*");

                // get latest version available for download
                HttpResponseMessage response = await cloudsmithClient.GetAsync($"nanoframework-images/?query=name:^WIN32_DLL_nanoCLR version:^latest$");
                string responseBody = await response.Content.ReadAsStringAsync();

                if (responseBody == "[]")
                {
                    Console.WriteLine($"Error getting latest nanoCLR version.");
                    return ExitCode.E9005;
                }

                var packageInfo = JsonConvert.DeserializeObject<List<CloudsmithPackageInfo>>(responseBody);

                if (packageInfo.Count() != 1)
                {
                    Console.WriteLine($"Error parsing latest nanoCLR version.");
                    return ExitCode.E9005;
                }
                else
                {
                    Version latestFwVersion = Version.Parse(packageInfo.ElementAt(0).Version);

                    if (latestFwVersion > version
                        || Version.Parse(targetVersion) > Version.Parse(currentVersion))
                    {
                        response = await cloudsmithClient.GetAsync(packageInfo.ElementAt(0).DownloadUrl);
                        response.EnsureSuccessStatusCode();

                        await using var ms = await response.Content.ReadAsStreamAsync();
                        await using var fs = File.OpenWrite(nanoClrDllLocation);

                        ms.Seek(0, SeekOrigin.Begin);
                        ms.CopyTo(fs);

                        fs.Flush();
                    }

                    return ExitCode.OK;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error downloading nanoCLR: {ex}");

                return ExitCode.E9005;
            }
        }
    }
}
