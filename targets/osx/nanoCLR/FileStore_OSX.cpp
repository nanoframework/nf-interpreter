//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

// TODO: replace with CLR_RT_FileStore integration once shared runtime code is enabled.

bool nanoCLR_OSX_LoadFile(const std::string &path, std::vector<unsigned char> &content, std::string &error)
{
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open())
    {
        error = "Cannot open file: " + path;
        return false;
    }

    content.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());

    if (in.bad() || (in.fail() && !in.eof()))
    {
        error = "Failed reading file: " + path;
        content.clear();
        return false;
    }

    return true;
}

bool nanoCLR_OSX_SaveFile(const std::string &path, const std::vector<unsigned char> &content, std::string &error)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out.is_open())
    {
        error = "Cannot open file for writing: " + path;
        return false;
    }

    out.write(reinterpret_cast<const char *>(content.data()), static_cast<std::streamsize>(content.size()));

    if (!out.good())
    {
        error = "Failed writing file: " + path;
        return false;
    }

    return true;
}
