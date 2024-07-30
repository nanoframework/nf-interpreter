//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using nanoFramework.nanoCLR.Host;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Http;
using System.Reflection;
using System.Threading.Tasks;

namespace nanoFramework.nanoCLR.CLI
{
    public static class ClrInstanceOperationsProcessor
    {
        private const string _cloudSmithApiUrl = "https://api.cloudsmith.io/v1/packages/net-nanoframework/";
        private static HttpClient _httpClient = new HttpClient();

        public static int ProcessVerb(ClrInstanceOperationsOptions options)
        {
            Program.ProcessVerbosityOptions(options.Verbosity);

            nanoCLRHostBuilder hostBuilder = nanoCLRHost.CreateBuilder();
            hostBuilder.UseConsoleDebugPrint();

            if (options.UpdateCLR)
            {
                return (int)UpdateNanoCLRAsync(
                    options.TargetVersion,
                    hostBuilder.GetCLRVersion(),
                    hostBuilder).Result;
            }
            else if (options.GetCLRVersion)
            {
                if (Program.VerbosityLevel > VerbosityLevel.Normal)
                {
                    hostBuilder.OutputNanoClrDllInfo();
                }

                Console.WriteLine($"nanoCLR version: {hostBuilder.GetCLRVersion()}");

                return (int)ExitCode.OK;
            }

            return (int)ExitCode.OK;
        }

        private static async Task<ExitCode> UpdateNanoCLRAsync(
            string targetVersion,
            string currentVersion,
            nanoCLRHostBuilder hostBuilder)
        {
            try
            {
                // compose current version
                // need to get rid of git hub has, in case it has one
                if (string.IsNullOrEmpty(currentVersion))
                {
                    currentVersion = "0.0.0.0";
                }
                else
                {
                    currentVersion = currentVersion.Substring(
                        0,
                        currentVersion.IndexOf("+") < 0 ? currentVersion.Length : currentVersion.IndexOf("+"));
                }

                Version version = Version.Parse(currentVersion);

                string nanoClrDllLocation = Path.Combine(
                    Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location),
                    "NanoCLR",
                    "nanoFramework.nanoCLR.dll");

                _httpClient.BaseAddress = new Uri(_cloudSmithApiUrl);
                _httpClient.DefaultRequestHeaders.Add("Accept", "*/*");

                // get latest version available for download
                HttpResponseMessage response = await _httpClient.GetAsync($"nanoframework-images/?query=name:^WIN_DLL_nanoCLR version:^latest$");
                string responseBody = await response.Content.ReadAsStringAsync();

                if (responseBody == "[]")
                {
                    Console.WriteLine($"Error getting latest nanoCLR version.");
                    return ExitCode.E9005;
                }

                var packageInfo = JsonConvert.DeserializeObject<List<CloudsmithPackageInfo>>(responseBody);

                if (packageInfo.Count != 1)
                {
                    Console.WriteLine($"Error parsing latest nanoCLR version.");
                    return ExitCode.E9005;
                }
                else
                {
                    Version latestFwVersion = Version.Parse(packageInfo[0].Version);

                    if (latestFwVersion < version)
                    {
                        Console.WriteLine($"Current version {version} lower than available version {packageInfo[0].Version}");
                    }
                    else if (latestFwVersion > version
                            || (!string.IsNullOrEmpty(targetVersion)
                            && (Version.Parse(targetVersion) > Version.Parse(currentVersion))))
                    {
                        response = await _httpClient.GetAsync(packageInfo[0].DownloadUrl);
                        response.EnsureSuccessStatusCode();

                        // need to unload the DLL before updating it
                        hostBuilder.UnloadNanoClrDll();

                        await using var ms = await response.Content.ReadAsStreamAsync();
                        await using var fs = File.OpenWrite(nanoClrDllLocation);

                        ms.Seek(0, SeekOrigin.Begin);
                        await ms.CopyToAsync(fs);

                        await fs.FlushAsync();

                        Console.WriteLine($"Updated to v{packageInfo[0].Version}");
                    }
                    else
                    {
                        Console.WriteLine($"Already at v{packageInfo[0].Version}");
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
