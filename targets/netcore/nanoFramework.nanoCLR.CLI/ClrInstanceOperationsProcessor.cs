// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Reflection;
using System.Text.Json;
using System.Threading.Tasks;
using nanoFramework.nanoCLR.Host;

namespace nanoFramework.nanoCLR.CLI
{
    public static class ClrInstanceOperationsProcessor
    {
        private const string _cloudSmithApiUrl = "https://api.cloudsmith.io/v1/packages/net-nanoframework/";
        private const string _refTargetsDevRepo = "nanoframework-images-dev";
        private const string _refTargetsStableRepo = "nanoframework-images";

        private static HttpClient _httpClient = new HttpClient();

        public static int ProcessVerb(ClrInstanceOperationsOptions options)
        {
            Program.ProcessVerbosityOptions(options.Verbosity);

            nanoCLRHostBuilder hostBuilder;

            // are we to use a local DLL?
            if (options.PathToCLRInstance != null)
            {
                if (options.UpdateCLR)
                {
                    // These options cannot be combined
                    throw new CLIException(ExitCode.E9010);
                }

                // check if path exists
                if (!Directory.Exists(options.PathToCLRInstance))
                {
                    throw new CLIException(ExitCode.E9009);
                }

                hostBuilder = nanoCLRHost.CreateBuilder(options.PathToCLRInstance);
            }
            else
            {
                hostBuilder = nanoCLRHost.CreateBuilder();
            }
            hostBuilder.UseConsoleDebugPrint();

            if (options.UpdateCLR)
            {
                return (int)UpdateNanoCLRAsync(
                    options.TargetVersion,
                    hostBuilder.GetCLRVersion(),
                    options.CheckPreviewVersions,
                    hostBuilder).Result;
            }
            else if (options.GetCLRVersion || options.GetNativeAssemblies)
            {
                if (Program.VerbosityLevel > VerbosityLevel.Normal)
                {
                    hostBuilder.OutputNanoClrDllInfo();
                }

                if (options.GetCLRVersion)
                {
                    Console.WriteLine($"nanoCLR version: {hostBuilder.GetCLRVersion()}");
                }

                if (options.GetNativeAssemblies)
                {
                    List<NativeAssemblyDetails> nativeAssemblies = hostBuilder.GetNativeAssemblies();

                    if (nativeAssemblies is not null)
                    {
                        if (options.GetCLRVersion)
                        {
                            Console.WriteLine();
                        }

                        OutputNativeAssembliesList(nativeAssemblies);
                    }
                    else if (Program.VerbosityLevel > VerbosityLevel.Normal)
                    {
                        return (int)ExitCode.E9011;
                    }
                }

                return (int)ExitCode.OK;
            }

            return (int)ExitCode.OK;
        }

        private static void OutputNativeAssembliesList(List<NativeAssemblyDetails> nativeAssemblies)
        {
            Console.WriteLine("Native assemblies:");

            // do some math to get a tidy output
            int maxAssemblyNameLength = nativeAssemblies.Max(assembly => assembly.Name.Length);
            int maxAssemblyVersionLength = nativeAssemblies.Max(assembly => assembly.Version.ToString().Length);

            foreach (NativeAssemblyDetails assembly in from na in nativeAssemblies
                                                       orderby na.Name
                                                       select na)
            {
                Console.WriteLine($"  {assembly.Name.PadRight(maxAssemblyNameLength)} v{assembly.Version.ToString().PadRight(maxAssemblyVersionLength)} 0x{assembly.CheckSum:X8}");
            }
        }

        private static async Task<ExitCode> UpdateNanoCLRAsync(
            string targetVersion,
            string currentVersion,
            bool usePreview,
            nanoCLRHostBuilder hostBuilder)
        {
            try
            {
                // compose current version
                // need to get rid of GitHub hash, in case it has one
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

                Version installedVersion = Version.Parse(currentVersion);

                string nanoClrDllLocation = Path.Combine(
                    Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location),
                    "NanoCLR",
                    "nanoFramework.nanoCLR.dll");

                _httpClient.BaseAddress = new Uri(_cloudSmithApiUrl);
                _httpClient.DefaultRequestHeaders.Add("Accept", "*/*");

                string repoName = usePreview ? _refTargetsDevRepo : _refTargetsStableRepo;
                List<CloudsmithPackageInfo> packageInfo;
                string responseBody;

                if (string.IsNullOrEmpty(targetVersion))
                {
                    // no specific version requested, get latest version available for download
                    HttpResponseMessage response = await _httpClient.GetAsync($"{repoName}/?query=name:^WIN_DLL_nanoCLR version:^latest$");

                    responseBody = await response.Content.ReadAsStringAsync();

                    if (responseBody == "[]")
                    {
                        Console.WriteLine($"Error getting latest available nanoCLR package.");
                        return ExitCode.E9005;
                    }
                }
                else
                {
                    // specific version requested, get details for that version
                    HttpResponseMessage response = await _httpClient.GetAsync($"{repoName}/?query=name:^WIN_DLL_nanoCLR version:{targetVersion}");

                    responseBody = await response.Content.ReadAsStringAsync();

                    if (responseBody == "[]")
                    {
                        Console.WriteLine($"Error getting package details for v{targetVersion}.");
                        return ExitCode.E9005;
                    }
                }

                // parse the response
                packageInfo = JsonSerializer.Deserialize<List<CloudsmithPackageInfo>>(responseBody);

                if (packageInfo.Count != 1)
                {
                    Console.WriteLine($"Error parsing nanoCLR version from package details.");
                    return ExitCode.E9005;
                }
                else
                {
                    Version availableFwVersion = Version.Parse(packageInfo[0].Version);

                    // update if the version is different from the installed one (either the requested target version or the latest available in the repo)
                    if ((!string.IsNullOrEmpty(targetVersion)
                         && (Version.Parse(targetVersion) != installedVersion))
                        || (availableFwVersion > installedVersion))
                    {
                        HttpResponseMessage response = await _httpClient.GetAsync(packageInfo[0].DownloadUrl);
                        response.EnsureSuccessStatusCode();

                        // need to unload the DLL before updating it
                        hostBuilder.UnloadNanoClrDll();

                        await using Stream ms = await response.Content.ReadAsStreamAsync();
                        await using FileStream fs = File.OpenWrite(nanoClrDllLocation);

                        ms.Seek(0, SeekOrigin.Begin);
                        await ms.CopyToAsync(fs);

                        await fs.FlushAsync();

                        Console.WriteLine($"Updated to v{packageInfo[0].Version}");
                    }
                    else
                    {
                        Console.WriteLine($"At v{packageInfo[0].Version}, skipping update");
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
