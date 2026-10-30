//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Host_Network.h>
#include <Host_Tls.h>

#if !defined(_WIN32)
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <cerrno>
#include <climits>
#include <condition_variable>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

namespace
{
constexpr size_t c_maxSockets = 128;
constexpr size_t c_maxResolverJobs = 16;

#if defined(_WIN32)
using NativeSocket = SOCKET;
using NativePoll = WSAPOLLFD;
using NativeLength = int;
constexpr NativeSocket c_invalidSocket = INVALID_SOCKET;
static_assert(FD_SETSIZE > c_maxSockets, "The readiness worker also needs a wake socket.");
#else
using NativeSocket = int;
using NativePoll = pollfd;
using NativeLength = socklen_t;
constexpr NativeSocket c_invalidSocket = -1;
#endif

struct SocketEntry
{
    NativeSocket descriptor;
    bool datagram;
    short interest = 0;
    int lastError = 0;
    int pendingError = 0;
    bool connecting = false;
};

struct ResolveJob
{
    std::string name;
    std::string canonicalName;
    std::vector<SOCK_sockaddr_in> addresses;
    int error = 0;
    bool done = false;
    bool cancelled = false;
};

std::mutex s_mutex;
std::condition_variable s_resolverCondition;
std::map<int, SocketEntry> s_sockets;
std::map<int, std::shared_ptr<ResolveJob>> s_jobs;
std::deque<std::shared_ptr<ResolveJob>> s_resolverQueue;
std::thread s_socketThread;
std::thread s_resolverThread;
NativeSocket s_wakePipe[2] = {c_invalidSocket, c_invalidSocket};
int64_t s_nextSocket = 1;
int64_t s_nextJob = 1;
bool s_running = false;
bool s_stopping = false;
bool s_workerFailed = false;
thread_local int s_lastError = 0;

int MapError(int error)
{
#if defined(_WIN32)
    // nanoFramework socket error codes use the Winsock numbering.
    return error == WSAEINPROGRESS ? SOCK_EWOULDBLOCK : error;
#else
    switch (error)
    {
        case 0:
            return 0;
        case EINTR:
            return SOCK_EINTR;
        case EBADF:
            return SOCK_EBADF;
        case EACCES:
        case EPERM:
            return SOCK_EACCES;
        case EFAULT:
            return SOCK_EFAULT;
        case EINVAL:
            return SOCK_EINVAL;
        case EMFILE:
        case ENFILE:
            return SOCK_EMFILE;
        case EAGAIN:
            return SOCK_EWOULDBLOCK;
        case EINPROGRESS:
            return SOCK_EWOULDBLOCK;
        case EALREADY:
            return SOCK_EALREADY;
        case ENOTSOCK:
            return SOCK_ENOTSOCK;
        case EDESTADDRREQ:
            return SOCK_EDESTADDRREQ;
        case EMSGSIZE:
            return SOCK_EMSGSIZE;
        case EPROTOTYPE:
            return SOCK_EPROTOTYPE;
        case ENOPROTOOPT:
            return SOCK_ENOPROTOOPT;
        case EPROTONOSUPPORT:
            return SOCK_EPROTONOSUPPORT;
        case EOPNOTSUPP:
            return SOCK_EOPNOTSUPP;
        case EAFNOSUPPORT:
            return SOCK_EAFNOSUPPORT;
        case EADDRINUSE:
            return SOCK_EADDRINUSE;
        case EADDRNOTAVAIL:
            return SOCK_EADDRNOTAVAIL;
        case ENETDOWN:
            return SOCK_ENETDOWN;
        case ENETUNREACH:
            return SOCK_ENETUNREACH;
        case ENETRESET:
            return SOCK_ENETRESET;
        case ECONNABORTED:
            return SOCK_ECONNABORTED;
        case ECONNRESET:
        case EPIPE:
            return SOCK_ECONNRESET;
        case ENOBUFS:
        case ENOMEM:
            return SOCK_ENOBUFS;
        case EISCONN:
            return SOCK_EISCONN;
        case ENOTCONN:
            return SOCK_ENOTCONN;
        case ETIMEDOUT:
            return SOCK_ETIMEDOUT;
        case ECONNREFUSED:
            return SOCK_ECONNREFUSED;
        case EHOSTUNREACH:
            return SOCK_EHOSTUNREACH;
        case EHOSTDOWN:
            return SOCK_EHOSTDOWN;
        default:
            return SOCK_SYSCALLFAILURE;
    }
#endif
}

int NativeError()
{
#if defined(_WIN32)
    return WSAGetLastError();
#else
    return errno;
#endif
}

int CloseSocket(NativeSocket socket)
{
#if defined(_WIN32)
    return closesocket(socket);
#else
    return close(socket);
#endif
}

int NativePollSockets(NativePoll *descriptors, size_t count, int timeout)
{
#if defined(_WIN32)
    if (count == 0)
    {
        return 0;
    }
    fd_set readers, writers, exceptions;
    FD_ZERO(&readers);
    FD_ZERO(&writers);
    FD_ZERO(&exceptions);
    for (size_t i = 0; i < count; ++i)
    {
        descriptors[i].revents = 0;
        if ((descriptors[i].events & POLLIN) != 0)
        {
            FD_SET(descriptors[i].fd, &readers);
        }
        if ((descriptors[i].events & POLLOUT) != 0)
        {
            FD_SET(descriptors[i].fd, &writers);
        }
        FD_SET(descriptors[i].fd, &exceptions);
    }
    timeval wait = {timeout / 1000, (timeout % 1000) * 1000};
    int result = select(0, &readers, &writers, &exceptions, timeout < 0 ? nullptr : &wait);
    if (result > 0)
    {
        for (size_t i = 0; i < count; ++i)
        {
            if (FD_ISSET(descriptors[i].fd, &readers))
            {
                descriptors[i].revents |= POLLIN;
            }
            if (FD_ISSET(descriptors[i].fd, &writers))
            {
                descriptors[i].revents |= POLLOUT;
            }
            if (FD_ISSET(descriptors[i].fd, &exceptions))
            {
                descriptors[i].revents |= POLLPRI | POLLERR;
            }
        }
    }
    return result;
#else
    return poll(descriptors, count, timeout);
#endif
}

int NativeGetOption(NativeSocket socket, int level, int option, void *value, NativeLength *length)
{
    return getsockopt(socket, level, option, static_cast<char *>(value), length);
}

int NativeSetOption(NativeSocket socket, int level, int option, const void *value, NativeLength length)
{
    return setsockopt(socket, level, option, static_cast<const char *>(value), length);
}

NativeSocket OpenSocket(int type, int protocol)
{
#if defined(_WIN32)
    SOCKET socket = WSASocketW(AF_INET, type, protocol, nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT);
    if (socket != INVALID_SOCKET)
    {
        u_long nonblocking = 1;
        if (ioctlsocket(socket, FIONBIO, &nonblocking) != 0)
        {
            int error = WSAGetLastError();
            closesocket(socket);
            WSASetLastError(error);
            return INVALID_SOCKET;
        }
    }
    return socket;
#else
    return socket(AF_INET, type | SOCK_NONBLOCK | SOCK_CLOEXEC, protocol);
#endif
}

bool OpenWakeSockets()
{
#if defined(_WIN32)
    s_wakePipe[0] = OpenSocket(SOCK_DGRAM, IPPROTO_UDP);
    s_wakePipe[1] = OpenSocket(SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int length = sizeof(address);
    return s_wakePipe[0] != INVALID_SOCKET && s_wakePipe[1] != INVALID_SOCKET &&
           bind(s_wakePipe[0], reinterpret_cast<sockaddr *>(&address), sizeof(address)) == 0 &&
           getsockname(s_wakePipe[0], reinterpret_cast<sockaddr *>(&address), &length) == 0 &&
           connect(s_wakePipe[1], reinterpret_cast<sockaddr *>(&address), length) == 0;
#else
    return pipe2(s_wakePipe, O_NONBLOCK | O_CLOEXEC) == 0;
#endif
}

int Fail(int error, SocketEntry *entry = nullptr)
{
    s_lastError = error;
    if (entry != nullptr)
    {
        entry->lastError = error;
    }
    return SOCK_SOCKET_ERROR;
}

SocketEntry *FindSocket(int handle)
{
    auto entry = s_sockets.find(handle);
    if (entry == s_sockets.end())
    {
        Fail(SOCK_ENOTSOCK);
        return nullptr;
    }
    return &entry->second;
}

int RecordResult(SocketEntry &entry, int result)
{
    entry.lastError = result < 0 ? MapError(NativeError()) : 0;
    s_lastError = entry.lastError;
    return result;
}

void WakeWorker()
{
    const char value = 1;
    int result;
    do
    {
#if defined(_WIN32)
        result = send(s_wakePipe[1], &value, 1, 0);
#else
        result = write(s_wakePipe[1], &value, 1);
#endif
    } while (result < 0 && MapError(NativeError()) == SOCK_EINTR);
    if (result < 0 && MapError(NativeError()) != SOCK_EWOULDBLOCK)
    {
        fprintf(stderr, "nanoCLR network wake failed (%d)\n", NativeError());
    }
}

int RecordIoResult(SocketEntry &entry, int result, short interest)
{
    RecordResult(entry, result);
    if (result < 0 && entry.lastError == SOCK_EWOULDBLOCK)
    {
        entry.interest |= interest;
        WakeWorker();
    }
    return result;
}

int AddSocket(NativeSocket descriptor, bool datagram = false)
{
    if (s_sockets.size() >= c_maxSockets || s_nextSocket > INT_MAX)
    {
        CloseSocket(descriptor);
        return Fail(SOCK_EMFILE);
    }
    int handle = static_cast<int>(s_nextSocket++);
    s_sockets.emplace(handle, SocketEntry{descriptor, datagram});
    s_lastError = 0;
    return handle;
}

bool ToNativeAddress(const SOCK_sockaddr *address, int length, sockaddr_in &native)
{
    if (address == nullptr || length != sizeof(SOCK_sockaddr_in))
    {
        Fail(SOCK_EINVAL);
        return false;
    }
    SOCK_sockaddr_in source;
    memcpy(&source, address, sizeof(source));
    if (source.sin_family != SOCK_AF_INET)
    {
        Fail(SOCK_EAFNOSUPPORT);
        return false;
    }
    native = {};
    native.sin_family = AF_INET;
    native.sin_port = source.sin_port;
    native.sin_addr.s_addr = source.sin_addr.S_un.S_addr;
    return true;
}

bool FromNativeAddress(const sockaddr_in &native, SOCK_sockaddr *address, int *length)
{
    if (address == nullptr || length == nullptr || *length < static_cast<int>(sizeof(SOCK_sockaddr_in)))
    {
        Fail(SOCK_EFAULT);
        return false;
    }
    SOCK_sockaddr_in result = {};
    result.sin_family = SOCK_AF_INET;
    result.sin_port = native.sin_port;
    result.sin_addr.S_un.S_addr = native.sin_addr.s_addr;
    memcpy(address, &result, sizeof(result));
    *length = sizeof(result);
    return true;
}

int TranslateFlags(int flags)
{
    if ((flags & ~7) != 0)
    {
        return Fail(SOCK_EOPNOTSUPP);
    }
    return ((flags & 1) ? MSG_OOB : 0) | ((flags & 2) ? MSG_PEEK : 0) | ((flags & 4) ? MSG_DONTROUTE : 0);
}

void SocketWorker()
{
    for (;;)
    {
        std::vector<NativePoll> descriptors;
        std::vector<int> handles;
        {
            std::lock_guard<std::mutex> lock(s_mutex);
            if (s_stopping)
            {
                return;
            }
            descriptors.push_back({s_wakePipe[0], POLLIN, 0});
            for (auto &[handle, entry] : s_sockets)
            {
                if (entry.interest != 0)
                {
                    handles.push_back(handle);
                    descriptors.push_back({entry.descriptor, entry.interest, 0});
                }
            }
        }

        int result = NativePollSockets(descriptors.data(), descriptors.size(), -1);
        if (result < 0)
        {
            if (MapError(NativeError()) == SOCK_EINTR)
            {
                continue;
            }
#if defined(_WIN32)
            // A socket may have closed after the worker took its descriptor snapshot.
            if (NativeError() == WSAENOTSOCK)
            {
                continue;
            }
#endif
            fprintf(stderr, "nanoCLR network poll failed (%d)\n", NativeError());
            {
                std::lock_guard<std::mutex> lock(s_mutex);
                s_workerFailed = true;
            }
            Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
            return;
        }
        if (descriptors[0].revents != 0)
        {
            char buffer[128];
#if defined(_WIN32)
            while (recv(s_wakePipe[0], buffer, sizeof(buffer), 0) > 0)
#else
            while (read(s_wakePipe[0], buffer, sizeof(buffer)) > 0)
#endif
            {
            }
        }

        bool signal = false;
        {
            std::lock_guard<std::mutex> lock(s_mutex);
            for (size_t i = 1; i < descriptors.size(); ++i)
            {
                auto entry = s_sockets.find(handles[i - 1]);
                if (descriptors[i].revents != 0 && entry != s_sockets.end() &&
                    entry->second.descriptor == descriptors[i].fd && entry->second.interest != 0)
                {
                    entry->second.interest = 0;
                    signal = true;
                }
            }
        }
        if (signal)
        {
            Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
        }
    }
}

void ResolverWorker()
{
    for (;;)
    {
        std::shared_ptr<ResolveJob> job;
        {
            std::unique_lock<std::mutex> lock(s_mutex);
            s_resolverCondition.wait(lock, [] { return s_stopping || !s_resolverQueue.empty(); });
            if (s_stopping)
            {
                return;
            }
            job = s_resolverQueue.front();
            s_resolverQueue.pop_front();
            if (job->cancelled)
            {
                continue;
            }
        }

        addrinfo hints = {};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_CANONNAME;
        addrinfo *addresses = nullptr;
        int result = getaddrinfo(job->name.c_str(), nullptr, &hints, &addresses);
#if !defined(_WIN32)
        int systemError = errno;
#endif
        std::vector<SOCK_sockaddr_in> converted;
        std::string canonicalName = job->name;
        for (auto address = addresses; address != nullptr; address = address->ai_next)
        {
            if (address->ai_family == AF_INET && address->ai_addrlen == sizeof(sockaddr_in))
            {
                SOCK_sockaddr_in value = {};
                sockaddr_in native;
                memcpy(&native, address->ai_addr, sizeof(native));
                value.sin_family = SOCK_AF_INET;
                value.sin_port = native.sin_port;
                value.sin_addr.S_un.S_addr = native.sin_addr.s_addr;
                converted.push_back(value);
                if (address->ai_canonname != nullptr)
                {
                    canonicalName = address->ai_canonname;
                }
            }
        }
        if (addresses != nullptr)
        {
            freeaddrinfo(addresses);
        }
        {
            std::lock_guard<std::mutex> lock(s_mutex);
            job->addresses = std::move(converted);
            job->canonicalName = std::move(canonicalName);
            job->error = result == 0            ? 0
                         : result == EAI_AGAIN  ? SOCK_TRY_AGAIN
                         : result == EAI_MEMORY ? SOCK_ENOBUFS
#if !defined(_WIN32)
                         : result == EAI_SYSTEM ? MapError(systemError)
#endif
                                                : SOCK_HOST_NOT_FOUND;
            if (job->error == 0 && job->addresses.empty())
            {
                job->error = SOCK_NO_DATA;
            }
            job->done = true;
        }
        Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    }
}

int NativeOption(int level, int option, int &nativeLevel)
{
    if (level == SOCK_IPPROTO_TCP && option == SOCK_TCP_NODELAY)
    {
        nativeLevel = IPPROTO_TCP;
        return TCP_NODELAY;
    }
    if (level == SOCK_IPPROTO_IP)
    {
        nativeLevel = IPPROTO_IP;
        switch (option)
        {
            case SOCK_IPO_TTL:
                return IP_TTL;
            case SOCK_IPO_TOS:
                return IP_TOS;
            default:
                return Fail(SOCK_ENOPROTOOPT);
        }
    }
    if (level != SOCK_SOL_SOCKET)
    {
        return Fail(SOCK_ENOPROTOOPT);
    }
    nativeLevel = SOL_SOCKET;
    switch (option)
    {
        case SOCK_SOCKO_REUSEADDRESS:
            return SO_REUSEADDR;
        case SOCK_SOCKO_KEEPALIVE:
            return SO_KEEPALIVE;
        case SOCK_SOCKO_BROADCAST:
            return SO_BROADCAST;
        case SOCK_SOCKO_DONTROUTE:
            return SO_DONTROUTE;
        case SOCK_SOCKO_OUTOFBANDINLINE:
            return SO_OOBINLINE;
        case SOCK_SOCKO_SENDBUFFER:
            return SO_SNDBUF;
        case SOCK_SOCKO_RECEIVEBUFFER:
            return SO_RCVBUF;
        case SOCK_SOCKO_ACCEPTCONNECTION:
            return SO_ACCEPTCONN;
        case SOCK_SOCKO_ERROR:
            return SO_ERROR;
        case SOCK_SOCKO_LINGER:
        case SOCK_SOCKO_DONTLINGER:
            return SO_LINGER;
        default:
            return Fail(SOCK_ENOPROTOOPT);
    }
}
} // namespace

bool Network_Initialize()
{
    std::unique_lock<std::mutex> lock(s_mutex);
    if (s_running)
    {
        return !s_stopping && !s_workerFailed;
    }
#if defined(_WIN32)
    WSADATA data;
    int startupError = WSAStartup(MAKEWORD(2, 2), &data);
    if (startupError != 0)
    {
        fprintf(stderr, "nanoCLR WSAStartup failed (%d)\n", startupError);
        return false;
    }
#endif
    if (!OpenWakeSockets())
    {
        fprintf(stderr, "nanoCLR network wake initialization failed (%d)\n", NativeError());
        for (auto &socket : s_wakePipe)
        {
            if (socket != c_invalidSocket)
            {
                CloseSocket(socket);
                socket = c_invalidSocket;
            }
        }
#if defined(_WIN32)
        WSACleanup();
#endif
        return false;
    }
    s_stopping = false;
    s_workerFailed = false;
    s_running = true;
    try
    {
        s_socketThread = std::thread(SocketWorker);
        s_resolverThread = std::thread(ResolverWorker);
    }
    catch (const std::system_error &error)
    {
        fprintf(stderr, "nanoCLR network worker: %s\n", error.what());
        lock.unlock();
        Network_Uninitialize();
        return false;
    }
    return true;
}

void SOCKETS_CloseConnections()
{
    HostTls_Reset();
    std::lock_guard<std::mutex> lock(s_mutex);
    for (auto &[handle, entry] : s_sockets)
    {
        CloseSocket(entry.descriptor);
    }
    s_sockets.clear();
    for (auto &[id, job] : s_jobs)
    {
        job->cancelled = true;
    }
    s_jobs.clear();
    s_resolverQueue.clear();
    if (s_running)
    {
        WakeWorker();
    }
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}

bool Network_Uninitialize()
{
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (!s_running)
        {
            return true;
        }
        s_stopping = true;
        WakeWorker();
        s_resolverCondition.notify_all();
    }
    if (s_socketThread.joinable())
    {
        s_socketThread.join();
    }
    if (s_resolverThread.joinable())
    {
        s_resolverThread.join();
    }
    SOCKETS_CloseConnections();
    std::lock_guard<std::mutex> lock(s_mutex);
    CloseSocket(s_wakePipe[0]);
    CloseSocket(s_wakePipe[1]);
    s_wakePipe[0] = s_wakePipe[1] = c_invalidSocket;
#if defined(_WIN32)
    WSACleanup();
#endif
    s_running = false;
    return true;
}

int SOCK_socket(int family, int type, int protocol)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_running || s_stopping || s_workerFailed)
    {
        return Fail(SOCK_NOTINITIALISED);
    }
    if (family != SOCK_AF_INET)
    {
        return Fail(SOCK_EAFNOSUPPORT);
    }
    if ((type != SOCK_SOCK_STREAM && type != SOCK_SOCK_DGRAM) ||
        (protocol != 0 && protocol != SOCK_IPPROTO_TCP && protocol != SOCK_IPPROTO_UDP))
    {
        return Fail(SOCK_EPROTONOSUPPORT);
    }
    NativeSocket descriptor = OpenSocket(type == SOCK_SOCK_STREAM ? SOCK_STREAM : SOCK_DGRAM, protocol);
    return descriptor == c_invalidSocket ? Fail(MapError(NativeError())) : AddSocket(descriptor, type == SOCK_SOCK_DGRAM);
}

int SOCK_bind(int handle, const SOCK_sockaddr *address, int length)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    sockaddr_in native;
    if (entry == nullptr || !ToNativeAddress(address, length, native))
    {
        return Fail(s_lastError, entry);
    }
    return RecordResult(*entry, bind(entry->descriptor, reinterpret_cast<sockaddr *>(&native), sizeof(native)));
}

int SOCK_connect(int handle, const SOCK_sockaddr *address, int length)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    sockaddr_in native;
    if (entry == nullptr || !ToNativeAddress(address, length, native))
    {
        return Fail(s_lastError, entry);
    }
    int result =
        RecordResult(*entry, connect(entry->descriptor, reinterpret_cast<sockaddr *>(&native), sizeof(native)));
    entry->connecting = result < 0 && entry->lastError == SOCK_EWOULDBLOCK;
    entry->pendingError = 0;
    return result;
}

int SOCK_listen(int handle, int backlog)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    return entry == nullptr ? SOCK_SOCKET_ERROR : RecordResult(*entry, listen(entry->descriptor, backlog));
}

int SOCK_accept(int handle, SOCK_sockaddr *address, int *length)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    sockaddr_in native;
    NativeLength size = sizeof(native);
    NativeSocket descriptor =
#if defined(_WIN32)
        accept(entry->descriptor, reinterpret_cast<sockaddr *>(&native), &size);
    int result = descriptor == INVALID_SOCKET ? -1 : 0;
    if (result == 0)
    {
        u_long nonblocking = 1;
        if (ioctlsocket(descriptor, FIONBIO, &nonblocking) != 0)
        {
            int error = WSAGetLastError();
            CloseSocket(descriptor);
            return Fail(MapError(error), entry);
        }
        if (!SetHandleInformation(reinterpret_cast<HANDLE>(descriptor), HANDLE_FLAG_INHERIT, 0))
        {
            fprintf(stderr, "nanoCLR accepted socket inheritance failed (%lu)\n", GetLastError());
            CloseSocket(descriptor);
            return Fail(SOCK_SYSCALLFAILURE, entry);
        }
    }
    if (RecordIoResult(*entry, result, POLLIN) < 0)
#else
        accept4(entry->descriptor, reinterpret_cast<sockaddr *>(&native), &size, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (RecordIoResult(*entry, descriptor, POLLIN) < 0)
#endif
    {
        return SOCK_SOCKET_ERROR;
    }
    if (address != nullptr && !FromNativeAddress(native, address, length))
    {
        CloseSocket(descriptor);
        return Fail(s_lastError, entry);
    }
    return AddSocket(descriptor);
}

int SOCK_close(int handle)
{
    HostTls_Close(handle);
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    int result = RecordResult(*entry, CloseSocket(entry->descriptor));
    s_sockets.erase(handle);
    WakeWorker();
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    return result;
}

int SOCK_shutdown(int handle, int how)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    if (how < 0 || how > 2)
    {
        return Fail(SOCK_EINVAL, entry);
    }
    int result = RecordResult(*entry, shutdown(entry->descriptor, how));
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    return result;
}

int SOCK_sendto(int handle, const char *buffer, int length, int flags, const SOCK_sockaddr *address, int addressLength)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    int nativeFlags = TranslateFlags(flags);
    sockaddr_in native;
    if (nativeFlags < 0 || length < 0 || (address != nullptr && !ToNativeAddress(address, addressLength, native)))
    {
        return Fail(length < 0 ? SOCK_EINVAL : s_lastError, entry);
    }
    const char empty = 0;
    if (length == 0 && buffer == nullptr)
    {
        buffer = &empty;
    }
    return RecordIoResult(
        *entry,
        sendto(
            entry->descriptor,
            buffer,
            length,
            nativeFlags
#if !defined(_WIN32)
                | MSG_NOSIGNAL
#endif
            ,
            address == nullptr ? nullptr : reinterpret_cast<sockaddr *>(&native),
            address == nullptr ? 0 : sizeof(native)),
        POLLOUT);
}

int SOCK_send(int handle, const char *buffer, int length, int flags)
{
    return SOCK_sendto(handle, buffer, length, flags, nullptr, 0);
}

int SOCK_recvfrom(int handle, char *buffer, int length, int flags, SOCK_sockaddr *address, int *addressLength)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    int nativeFlags = TranslateFlags(flags);
    if (nativeFlags < 0 || length < 0)
    {
        return Fail(length < 0 ? SOCK_EINVAL : s_lastError, entry);
    }
    if ((nativeFlags & MSG_DONTROUTE) != 0)
    {
        return Fail(SOCK_EOPNOTSUPP, entry);
    }
    if (address != nullptr && (addressLength == nullptr || *addressLength < static_cast<int>(sizeof(SOCK_sockaddr_in))))
    {
        return Fail(SOCK_EFAULT, entry);
    }
    sockaddr_in native;
    NativeLength size = sizeof(native);
    char empty;
    if (length == 0 && buffer == nullptr)
    {
        buffer = &empty;
    }
    int result = recvfrom(entry->descriptor, buffer, length, nativeFlags, reinterpret_cast<sockaddr *>(&native), &size);
    if ((result < 0 || (result == 0 && !entry->datagram && length > 0)) && entry->pendingError != 0)
    {
        return Fail(entry->pendingError, entry);
    }
    RecordIoResult(*entry, result, POLLIN);
    if (result >= 0 && address != nullptr && !FromNativeAddress(native, address, addressLength))
    {
        return Fail(s_lastError, entry);
    }
    return result;
}

int SOCK_recv(int handle, char *buffer, int length, int flags)
{
    return SOCK_recvfrom(handle, buffer, length, flags, nullptr, nullptr);
}

int SOCK_getsockname(int handle, SOCK_sockaddr *address, int *length)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    sockaddr_in native;
    NativeLength size = sizeof(native);
    int result = RecordResult(*entry, getsockname(entry->descriptor, reinterpret_cast<sockaddr *>(&native), &size));
    return result < 0 || !FromNativeAddress(native, address, length) ? Fail(s_lastError, entry) : 0;
}

int SOCK_getpeername(int handle, SOCK_sockaddr *address, int *length)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    sockaddr_in native;
    NativeLength size = sizeof(native);
    int result = RecordResult(*entry, getpeername(entry->descriptor, reinterpret_cast<sockaddr *>(&native), &size));
    return result < 0 || !FromNativeAddress(native, address, length) ? Fail(s_lastError, entry) : 0;
}

int SOCK_ioctl(int handle, int command, int *data)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    if (data == nullptr)
    {
        return Fail(SOCK_EFAULT, entry);
    }
    if (command == static_cast<int>(SOCK_FIONBIO))
    {
        // Managed blocking semantics are implemented by the CLR scheduler, never by the descriptor.
        return *data == 1 ? RecordResult(*entry, 0) : Fail(SOCK_EOPNOTSUPP, entry);
    }
    if (command != static_cast<int>(SOCK_FIONREAD))
    {
        return Fail(SOCK_EOPNOTSUPP, entry);
    }
#if defined(_WIN32)
    u_long available = 0;
    int result = RecordResult(*entry, ioctlsocket(entry->descriptor, FIONREAD, &available));
    if (result == 0)
    {
        *data = static_cast<int>((std::min)(available, static_cast<u_long>(INT_MAX)));
    }
    return result;
#else
    return RecordResult(*entry, ioctl(entry->descriptor, FIONREAD, data));
#endif
}

int SOCK_getlasterror()
{
    return s_lastError;
}

bool HostNetwork_ArmSocket(int handle, bool write)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr || !s_running || s_stopping || s_workerFailed)
    {
        return false;
    }
    entry->interest |= write ? POLLOUT : POLLIN;
    WakeWorker();
    return true;
}

int SOCK_getsocklasterror(int handle)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry != nullptr)
    {
        s_lastError = entry->pendingError != 0 ? entry->pendingError : entry->lastError;
    }
    return s_lastError;
}

static int SelectSockets(
    SOCK_fd_set *readfds,
    SOCK_fd_set *writefds,
    SOCK_fd_set *exceptfds,
    const SOCK_timeval *timeout)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_workerFailed)
    {
        return Fail(SOCK_ENETDOWN);
    }
    if (timeout == nullptr || timeout->tv_sec != 0 || timeout->tv_usec != 0)
    {
        return Fail(SOCK_EOPNOTSUPP);
    }
    std::map<int, short> interests;
    auto collect = [&](SOCK_fd_set *set, short interest) {
        if (set != nullptr)
        {
            if (set->fd_count > SOCK_FD_SETSIZE)
            {
                return false;
            }
            for (unsigned i = 0; i < set->fd_count; ++i)
            {
                interests[set->fd_array[i]] |= interest;
            }
        }
        return true;
    };
    if (!collect(readfds, POLLIN) || !collect(writefds, POLLOUT) || !collect(exceptfds, POLLPRI))
    {
        return Fail(SOCK_EINVAL);
    }
    std::vector<NativePoll> descriptors;
    for (auto [handle, interest] : interests)
    {
        auto entry = FindSocket(handle);
        if (entry == nullptr)
        {
            return Fail(SOCK_EBADF);
        }
        descriptors.push_back({entry->descriptor, interest, 0});
    }
    int result;
    do
    {
        result = NativePollSockets(descriptors.data(), descriptors.size(), 0);
    } while (result < 0 && MapError(NativeError()) == SOCK_EINTR);
    if (result < 0)
    {
        return Fail(MapError(NativeError()));
    }
    std::map<int, short> ready;
    size_t index = 0;
    bool wake = false;
    for (auto [handle, interest] : interests)
    {
        auto &entry = s_sockets.at(handle);
        short events = descriptors[index++].revents;
        if ((events & (POLLERR | POLLOUT | POLLHUP)) != 0 && (entry.connecting || (events & POLLERR) != 0))
        {
            int error = 0;
            NativeLength size = sizeof(error);
            if (NativeGetOption(entry.descriptor, SOL_SOCKET, SO_ERROR, &error, &size) < 0)
            {
                error = NativeError();
            }
            if (error != 0)
            {
                entry.pendingError = MapError(error);
            }
            entry.connecting = false;
        }
        if (entry.pendingError != 0)
        {
            events |= POLLPRI;
            events &= ~POLLOUT;
        }
        if ((events & POLLHUP) != 0 && entry.pendingError == 0)
        {
            events |= POLLIN;
        }
        ready[handle] = events;
        if ((events & (interest | POLLERR | POLLHUP)) == 0)
        {
            entry.interest |= interest;
            wake = true;
        }
    }
    int count = 0;
    auto filter = [&](SOCK_fd_set *set, short interest) {
        if (set != nullptr)
        {
            unsigned output = 0;
            for (unsigned i = 0; i < set->fd_count; ++i)
            {
                if ((ready[set->fd_array[i]] & interest) != 0)
                {
                    set->fd_array[output++] = set->fd_array[i];
                }
            }
            set->fd_count = output;
            count += output;
        }
    };
    filter(readfds, POLLIN);
    filter(writefds, POLLOUT);
    filter(exceptfds, POLLPRI);
    if (wake)
    {
        WakeWorker();
    }
    s_lastError = 0;
    return count;
}

int SOCK_select(int, SOCK_fd_set *readfds, SOCK_fd_set *writefds, SOCK_fd_set *exceptfds, const SOCK_timeval *timeout)
{
    int result = SelectSockets(readfds, writefds, exceptfds, timeout);

    if (result == SOCK_SOCKET_ERROR)
    {
        // Callers inspect the sets even when select fails, so report nothing ready. Otherwise the caller's
        // untouched sets would read as an exception and the error stored by Fail() could be overwritten.
        for (SOCK_fd_set *set : {readfds, writefds, exceptfds})
        {
            if (set != nullptr)
            {
                set->fd_count = 0;
            }
        }
    }

    return result;
}

int SOCK_setsockopt(int handle, int level, int option, const char *value, int length)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    int nativeLevel;
    int nativeOption = NativeOption(level, option, nativeLevel);
    if (nativeOption < 0)
    {
        return Fail(s_lastError, entry);
    }
    if (nativeLevel == SOL_SOCKET && nativeOption == SO_LINGER)
    {
        linger nativeValue = {};
        int timeout = 0;
        if (value == nullptr)
        {
            return Fail(SOCK_EINVAL, entry);
        }
        if (option == SOCK_SOCKO_LINGER && length == 2 * sizeof(int))
        {
            int enabled;
            memcpy(&enabled, value, sizeof(enabled));
            memcpy(&timeout, value + sizeof(int), sizeof(timeout));
            nativeValue.l_onoff = enabled != 0;
        }
        else if (length == sizeof(int))
        {
            int scalarValue;
            memcpy(&scalarValue, value, sizeof(scalarValue));
            if (option == SOCK_SOCKO_LINGER)
            {
                // The managed scalar is a timeout in seconds, not an enable flag.
                nativeValue.l_onoff = 1;
                timeout = scalarValue;
            }
            else
            {
                NativeLength size = sizeof(nativeValue);
                if (RecordResult(*entry, NativeGetOption(entry->descriptor, SOL_SOCKET, SO_LINGER, &nativeValue, &size)) < 0)
                {
                    return SOCK_SOCKET_ERROR;
                }
                nativeValue.l_onoff = scalarValue == 0;
                timeout = nativeValue.l_linger;
            }
        }
        else
        {
            return Fail(SOCK_EINVAL, entry);
        }
        if (timeout < 0
#if defined(_WIN32)
            || timeout > USHRT_MAX
#endif
        )
        {
            return Fail(SOCK_EINVAL, entry);
        }
        nativeValue.l_linger = static_cast<decltype(nativeValue.l_linger)>(timeout);
        return RecordResult(
            *entry,
            NativeSetOption(entry->descriptor, SOL_SOCKET, SO_LINGER, &nativeValue, sizeof(nativeValue)));
    }
    if (value == nullptr || length != sizeof(int))
    {
        return Fail(SOCK_EINVAL, entry);
    }
    int nativeValue;
    memcpy(&nativeValue, value, sizeof(nativeValue));
    return RecordResult(
        *entry,
        NativeSetOption(entry->descriptor, nativeLevel, nativeOption, &nativeValue, sizeof(nativeValue)));
}

int SOCK_getsockopt(int handle, int level, int option, char *value, int *length)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    int nativeLevel;
    int nativeOption = NativeOption(level, option, nativeLevel);
    if (nativeOption < 0)
    {
        return Fail(s_lastError, entry);
    }
    if (value == nullptr || length == nullptr || *length < static_cast<int>(sizeof(int)))
    {
        return Fail(SOCK_EINVAL, entry);
    }
    if (nativeLevel == SOL_SOCKET && nativeOption == SO_LINGER)
    {
        linger nativeValue = {};
        NativeLength size = sizeof(nativeValue);
        int result = RecordResult(*entry, NativeGetOption(entry->descriptor, SOL_SOCKET, SO_LINGER, &nativeValue, &size));
        if (result == 0)
        {
            if (option == SOCK_SOCKO_LINGER && *length >= 2 * static_cast<int>(sizeof(int)))
            {
                int fields[] = {nativeValue.l_onoff != 0, nativeValue.l_linger};
                memcpy(value, fields, sizeof(fields));
                *length = sizeof(fields);
            }
            else
            {
                // Managed GetSocketOption negates DontLinger's enable flag.
                int scalarValue = option == SOCK_SOCKO_LINGER ? nativeValue.l_linger : nativeValue.l_onoff != 0;
                memcpy(value, &scalarValue, sizeof(scalarValue));
                *length = sizeof(scalarValue);
            }
        }
        return result;
    }
    int nativeValue = 0;
    NativeLength size = sizeof(nativeValue);
    int result = RecordResult(*entry, NativeGetOption(entry->descriptor, nativeLevel, nativeOption, &nativeValue, &size));
    if (result == 0)
    {
        if (nativeOption == SO_ERROR && nativeLevel == SOL_SOCKET)
        {
            nativeValue = entry->pendingError != 0 ? entry->pendingError : MapError(nativeValue);
            entry->pendingError = 0;
        }
        memcpy(value, &nativeValue, sizeof(nativeValue));
        *length = sizeof(nativeValue);
    }
    return result;
}

HRESULT SOCK_IPV4AddressFromString(const char *text, uint64_t *address)
{
    in_addr native;
    if (text == nullptr || address == nullptr || inet_pton(AF_INET, text, &native) != 1)
    {
        return CLR_E_INVALID_PARAMETER;
    }
    *address = native.s_addr;
    return S_OK;
}

const char *SOCK_IPV4AddressToString(uint32_t address)
{
    thread_local char buffer[INET_ADDRSTRLEN];
    in_addr native = {};
    native.s_addr = address;
    return inet_ntop(AF_INET, &native, buffer, sizeof(buffer));
}

int HostNetwork_IsAvailable()
{
    std::vector<HostNetworkInterface> interfaces;
    if (HostNetwork_ListInterfaces(interfaces) < 0)
    {
        return -1;
    }
    return std::any_of(interfaces.begin(), interfaces.end(), [](const auto &entry) { return entry.available; }) ? 1 : 0;
}

int HostNetwork_StartResolve(const char *name)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (!s_running || s_stopping)
    {
        return Fail(SOCK_NOTINITIALISED);
    }
    if (name == nullptr || name[0] == 0)
    {
        return Fail(SOCK_EINVAL);
    }
    if (s_jobs.size() >= c_maxResolverJobs || s_nextJob > INT_MAX)
    {
        return Fail(SOCK_ENOBUFS);
    }
    auto job = std::make_shared<ResolveJob>();
    job->name = name;
    int id = static_cast<int>(s_nextJob++);
    s_jobs.emplace(id, job);
    s_resolverQueue.push_back(job);
    s_resolverCondition.notify_one();
    return id;
}

void HostNetwork_CancelResolve(int request)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto job = s_jobs.find(request);
    if (job != s_jobs.end())
    {
        job->second->cancelled = true;
        auto cancelled = job->second;
        s_resolverQueue.erase(
            std::remove(s_resolverQueue.begin(), s_resolverQueue.end(), cancelled),
            s_resolverQueue.end());
        s_jobs.erase(job);
    }
}

int HostNetwork_CompleteResolve(int request, SOCK_addrinfo **result)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto found = s_jobs.find(request);
    if (result == nullptr || found == s_jobs.end())
    {
        return Fail(SOCK_EINVAL);
    }
    *result = nullptr;
    auto job = found->second;
    if (!job->done)
    {
        return Fail(SOCK_EWOULDBLOCK);
    }
    s_jobs.erase(found);
    if (job->error != 0)
    {
        return Fail(job->error);
    }
    SOCK_addrinfo **tail = result;
    for (auto &address : job->addresses)
    {
        auto node = static_cast<SOCK_addrinfo *>(calloc(1, sizeof(SOCK_addrinfo)));
        if (node != nullptr)
        {
            node->ai_addr = static_cast<SOCK_sockaddr *>(malloc(sizeof(address)));
            size_t nameLength = job->canonicalName.size() + 1;
            node->ai_canonname = static_cast<char *>(malloc(nameLength));
            if (node->ai_canonname != nullptr)
            {
                memcpy(node->ai_canonname, job->canonicalName.c_str(), nameLength);
            }
        }
        if (node == nullptr || node->ai_addr == nullptr || node->ai_canonname == nullptr)
        {
            SOCK_freeaddrinfo(node);
            SOCK_freeaddrinfo(*result);
            *result = nullptr;
            return Fail(SOCK_ENOBUFS);
        }
        node->ai_family = SOCK_AF_INET;
        node->ai_socktype = SOCK_SOCK_STREAM;
        node->ai_protocol = SOCK_IPPROTO_TCP;
        node->ai_addrlen = sizeof(address);
        memcpy(node->ai_addr, &address, sizeof(address));
        *tail = node;
        tail = &node->ai_next;
    }
    return 0;
}

void SOCK_freeaddrinfo(SOCK_addrinfo *address)
{
    while (address != nullptr)
    {
        auto next = address->ai_next;
        free(address->ai_addr);
        free(address->ai_canonname);
        free(address);
        address = next;
    }
}
