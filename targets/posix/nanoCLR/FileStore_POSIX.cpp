//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <sys/stat.h>

// Maximum .pe file size accepted (64 MiB — well above any realistic managed assembly).
static constexpr std::streamoff c_MaxFileBytes = 64LL * 1024 * 1024;

// TODO: replace with CLR_RT_FileStore integration once shared runtime code is enabled.

bool nanoCLR_POSIX_LoadFile(const std::string &path, std::vector<unsigned char> &content, std::string &error)
{
    // Reject anything that is not a regular file (FIFOs, device nodes, sockets, …).
    struct stat st{};
    if (::stat(path.c_str(), &st) != 0)
    {
        error = "Cannot stat file: " + path;
        return false;
    }
    if (!S_ISREG(st.st_mode))
    {
        error = "Not a regular file: " + path;
        return false;
    }
    if (st.st_size > c_MaxFileBytes)
    {
        error = "File too large (> 64 MiB): " + path;
        return false;
    }

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

bool nanoCLR_POSIX_SaveFile(const std::string &path, const std::vector<unsigned char> &content, std::string &error)
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
