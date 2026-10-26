//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_Network.h>
#include <Target_Tls.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

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

struct SocketEntry
{
    int descriptor;
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
int s_wakePipe[2] = {-1, -1};
int64_t s_nextSocket = 1;
int64_t s_nextJob = 1;
bool s_running = false;
bool s_stopping = false;
bool s_workerFailed = false;
thread_local int s_lastError = 0;

int MapError(int error)
{
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
    entry.lastError = result < 0 ? MapError(errno) : 0;
    s_lastError = entry.lastError;
    return result;
}

void WakeWorker()
{
    const char value = 1;
    int result;
    do
    {
        result = write(s_wakePipe[1], &value, 1);
    } while (result < 0 && errno == EINTR);
    if (result < 0 && errno != EAGAIN)
    {
        perror("nanoCLR network wake");
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

int AddSocket(int descriptor, bool datagram = false)
{
    if (s_sockets.size() >= c_maxSockets || s_nextSocket > INT_MAX)
    {
        close(descriptor);
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
        std::vector<pollfd> descriptors;
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

        int result = poll(descriptors.data(), descriptors.size(), -1);
        if (result < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("nanoCLR network poll");
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
            while (read(s_wakePipe[0], buffer, sizeof(buffer)) > 0)
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
        int systemError = errno;
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
                         : result == EAI_SYSTEM ? MapError(systemError)
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
    if (pipe2(s_wakePipe, O_NONBLOCK | O_CLOEXEC) < 0)
    {
        perror("nanoCLR network pipe");
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
    LinuxTls_Reset();
    std::lock_guard<std::mutex> lock(s_mutex);
    for (auto &[handle, entry] : s_sockets)
    {
        close(entry.descriptor);
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
    close(s_wakePipe[0]);
    close(s_wakePipe[1]);
    s_wakePipe[0] = s_wakePipe[1] = -1;
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
    int descriptor =
        socket(AF_INET, (type == SOCK_SOCK_STREAM ? SOCK_STREAM : SOCK_DGRAM) | SOCK_NONBLOCK | SOCK_CLOEXEC, protocol);
    return descriptor < 0 ? Fail(MapError(errno)) : AddSocket(descriptor, type == SOCK_SOCK_DGRAM);
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
    socklen_t size = sizeof(native);
    int descriptor =
        accept4(entry->descriptor, reinterpret_cast<sockaddr *>(&native), &size, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (RecordIoResult(*entry, descriptor, POLLIN) < 0)
    {
        return SOCK_SOCKET_ERROR;
    }
    if (address != nullptr && !FromNativeAddress(native, address, length))
    {
        close(descriptor);
        return Fail(s_lastError, entry);
    }
    return AddSocket(descriptor);
}

int SOCK_close(int handle)
{
    LinuxTls_Close(handle);
    std::lock_guard<std::mutex> lock(s_mutex);
    auto entry = FindSocket(handle);
    if (entry == nullptr)
    {
        return Fail(SOCK_EBADF);
    }
    int result = RecordResult(*entry, close(entry->descriptor));
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
    return RecordIoResult(
        *entry,
        sendto(
            entry->descriptor,
            buffer,
            length,
            nativeFlags | MSG_NOSIGNAL,
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
    if (address != nullptr && (addressLength == nullptr || *addressLength < static_cast<int>(sizeof(SOCK_sockaddr_in))))
    {
        return Fail(SOCK_EFAULT, entry);
    }
    sockaddr_in native;
    socklen_t size = sizeof(native);
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
    socklen_t size = sizeof(native);
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
    socklen_t size = sizeof(native);
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
    return RecordResult(*entry, ioctl(entry->descriptor, FIONREAD, data));
}

int SOCK_getlasterror()
{
    return s_lastError;
}

bool LinuxNetwork_ArmSocket(int handle, bool write)
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
    std::vector<pollfd> descriptors;
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
        result = poll(descriptors.data(), descriptors.size(), 0);
    } while (result < 0 && errno == EINTR);
    if (result < 0)
    {
        return Fail(MapError(errno));
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
            socklen_t size = sizeof(error);
            if (getsockopt(entry.descriptor, SOL_SOCKET, SO_ERROR, &error, &size) < 0)
            {
                error = errno;
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
        if (value == nullptr)
        {
            return Fail(SOCK_EINVAL, entry);
        }
        if (option == SOCK_SOCKO_LINGER && length == sizeof(nativeValue))
        {
            memcpy(&nativeValue, value, sizeof(nativeValue));
            nativeValue.l_onoff = nativeValue.l_onoff != 0;
        }
        else if (length == sizeof(int))
        {
            int scalarValue;
            memcpy(&scalarValue, value, sizeof(scalarValue));
            if (option == SOCK_SOCKO_LINGER)
            {
                // The managed scalar is a timeout in seconds, not an enable flag.
                nativeValue.l_onoff = 1;
                nativeValue.l_linger = scalarValue;
            }
            else
            {
                socklen_t size = sizeof(nativeValue);
                if (RecordResult(*entry, getsockopt(entry->descriptor, SOL_SOCKET, SO_LINGER, &nativeValue, &size)) < 0)
                {
                    return SOCK_SOCKET_ERROR;
                }
                nativeValue.l_onoff = scalarValue == 0;
            }
        }
        else
        {
            return Fail(SOCK_EINVAL, entry);
        }
        if (nativeValue.l_linger < 0)
        {
            return Fail(SOCK_EINVAL, entry);
        }
        return RecordResult(
            *entry,
            setsockopt(entry->descriptor, SOL_SOCKET, SO_LINGER, &nativeValue, sizeof(nativeValue)));
    }
    if (value == nullptr || length != sizeof(int))
    {
        return Fail(SOCK_EINVAL, entry);
    }
    int nativeValue;
    memcpy(&nativeValue, value, sizeof(nativeValue));
    return RecordResult(
        *entry,
        setsockopt(entry->descriptor, nativeLevel, nativeOption, &nativeValue, sizeof(nativeValue)));
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
        socklen_t size = sizeof(nativeValue);
        int result = RecordResult(*entry, getsockopt(entry->descriptor, SOL_SOCKET, SO_LINGER, &nativeValue, &size));
        if (result == 0)
        {
            if (option == SOCK_SOCKO_LINGER && *length >= static_cast<int>(sizeof(nativeValue)))
            {
                memcpy(value, &nativeValue, sizeof(nativeValue));
                *length = sizeof(nativeValue);
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
    socklen_t size = sizeof(nativeValue);
    int result = RecordResult(*entry, getsockopt(entry->descriptor, nativeLevel, nativeOption, &nativeValue, &size));
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
    in_addr native{address};
    return inet_ntop(AF_INET, &native, buffer, sizeof(buffer));
}

int LinuxNetwork_IsAvailable()
{
    std::vector<LinuxNetworkInterface> interfaces;
    if (LinuxNetwork_ListInterfaces(interfaces) < 0)
    {
        return -1;
    }
    return std::any_of(interfaces.begin(), interfaces.end(), [](const auto &entry) { return entry.available; }) ? 1 : 0;
}

int LinuxNetwork_StartResolve(const char *name)
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

void LinuxNetwork_CancelResolve(int request)
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

int LinuxNetwork_CompleteResolve(int request, SOCK_addrinfo **result)
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
            node->ai_canonname = strdup(job->canonicalName.c_str());
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
