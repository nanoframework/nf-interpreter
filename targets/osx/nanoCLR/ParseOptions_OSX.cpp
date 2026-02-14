//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string>
#include <vector>

struct NanoClrHostOptions
{
    bool showHelp = false;
    bool showVersion = false;
    std::vector<std::string> assemblyPaths;
};

// TODO: replace with CLR_RT_ParseOptions integration once shared runtime code is enabled.
bool nanoCLR_OSX_ParseOptions(
    const std::vector<std::string> &arguments,
    NanoClrHostOptions &options,
    std::string &error)
{
    for (size_t i = 0; i < arguments.size(); i++)
    {
        const std::string &arg = arguments[i];

        if (arg == "--help" || arg == "-h")
        {
            options.showHelp = true;
            continue;
        }

        if (arg == "--version" || arg == "-v")
        {
            options.showVersion = true;
            continue;
        }

        if (arg == "-load")
        {
            if (i + 1 >= arguments.size())
            {
                error = "Missing value for -load";
                return false;
            }

            options.assemblyPaths.push_back(arguments[++i]);
            continue;
        }

        error = "Unknown option: " + arg;
        return false;
    }

    return true;
}
