//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// #include "lwIP_Sockets.h"

#include <sockets_nrf9160.h>

#include <modem/nrf_modem_lib.h>
#include <net/tls_credentials.h>

#include <nrf9160_lte.h>

extern char connect_state;


static int last_sock_error = 0;

bool HAL_SOCK_Initialize()
{
    NATIVE_PROFILE_PAL_NETWORK();
    // Start up the nrf modem
#ifdef CONFIG_SOC_SERIES_NRF91X
    nrf9160_lte_init(180);
#endif

    printk("HAL_SOCK_Initialize-- Connect state %d\n", connect_state);
    return true;
}

bool HAL_SOCK_Uninitialize()
{
    NATIVE_PROFILE_PAL_NETWORK();
    return true;
}

int net_family_2z(int netfam)
{
    if (netfam == SOCK_AF_INET)
        netfam = AF_INET;
    else if (netfam == SOCK_AF_INET6)
        netfam = AF_INET6;
    else
        printk("**** Unknown .net af_family type %d\n", netfam);
    return netfam;
}
int z_family_2net(int zfam)
{
    if (zfam == AF_INET)
        zfam = SOCK_AF_INET;
    else if (zfam == AF_INET6)
        zfam = SOCK_AF_INET6;
    else
        printk("**** Unknown z af_family type %d\n", zfam);
    return zfam;
}

struct sockaddr *net_saddr_2z(const struct SOCK_sockaddr *address, struct sockaddr *zaddr)
{
    zaddr->sa_family = net_family_2z(address->sa_family);
    memcpy(zaddr->data, address->sa_data, sizeof(address->sa_data));
    return zaddr;
}

SOCK_SOCKET HAL_SOCK_socket(int family, int type, int protocol)
{
    NATIVE_PROFILE_PAL_NETWORK();
    printk("socket   fam %d type %d protocol %d\n", family, type, protocol);

    // Translate network family
    family = net_family_2z(family);

    printk("socket x fam %d type %d protocol %d\n", family, type, protocol);
    int sock = socket(family, type, protocol);
    if (sock < 0)
        printk("***** socket fail\n");
    return sock;
}
int HAL_SOCK_bind(SOCK_SOCKET socket, const struct SOCK_sockaddr *address, int addressLen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    printk("bind: sock %d  fam: %d\n", socket, address->sa_family);

    // Translate address into something zephyr knows about
    struct sockaddr addr;
    int rc = bind(socket, net_saddr_2z(address, &addr), addressLen);
    if (rc < 0)
        printk("***** bind fail. %d\n", errno);
    last_sock_error = errno;
    return rc;
}
int HAL_SOCK_connect(SOCK_SOCKET socket, const struct SOCK_sockaddr *address, int addressLen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    printk("connect: sock %d  fam: %d  len %d\n", socket, address->sa_family, addressLen);

    // Translate address into something zephyr knows about
    struct sockaddr addr;
    //    int rc = connect(socket, net_saddr_2z (address, &addr), addressLen);
    net_saddr_2z(address, &addr);

    printk("connect: xlate: sock %d  fam: %d  len %d\n", socket, addr.sa_family, addressLen);
    //printk("connect: old errno %d\n", errno);
    int rc = connect(socket, &addr, addressLen);
    if (rc != 0)
    {
        if (errno == EINPROGRESS)
        {
            printk("### connect returned EINPROGRESS\n");
            rc = 0;
        }
        else
            printk("***** connect fail. %d\n", errno);
    }
    last_sock_error = errno;
    return rc;
}
int HAL_SOCK_send(SOCK_SOCKET socket, const char *buf, int len, int flags)
{
    NATIVE_PROFILE_PAL_NETWORK();
    printk("HAL_SOCK_send soc %d len %d flags %x\n", socket, len, flags);
    int rc = send(socket, buf, len, flags);
    if (rc < 0)
    {
        printk("send failed %d %d\n", rc, errno);
    }
    last_sock_error = errno;
    return rc;
}
int HAL_SOCK_recv(SOCK_SOCKET socket, char *buf, int len, int flags)
{
    NATIVE_PROFILE_PAL_NETWORK();
    printk("HAL_SOCK_recv soc %d len %d flags %x\n", socket, len, flags);
    int rc = recv(socket, buf, len, flags);
    if (rc < 0)
    {
        printk("recv failed %d %d\n", rc, errno);
    }
    last_sock_error = errno;
    return rc;
}
int HAL_SOCK_close(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_NETWORK();
    printk("HAL_SOCK_close soc %d\n", socket);
    int rc = close(socket);
    if (rc)
    {
        printk("close failed %d %d\n", rc, errno);
    }
    last_sock_error = errno;
    return rc;
}
int HAL_SOCK_listen(SOCK_SOCKET socket, int backlog)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int rc = listen(socket, backlog);
    if (rc)
    {
        printk("listen failed %d %d\n", rc, errno);
    }
    last_sock_error = errno;
    return rc;
}
SOCK_SOCKET HAL_SOCK_accept(SOCK_SOCKET socket, struct SOCK_sockaddr *address, int *addressLen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int rc = accept(socket, (struct sockaddr *)address, (unsigned int *)addressLen);
    if (rc)
    {
        printk("accept failed %d %d\n", rc, errno);
    }
    last_sock_error = errno;
    return rc;
}
int HAL_SOCK_shutdown(SOCK_SOCKET socket, int how)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return shutdown(socket, how);
}
struct nf_addrinfo
{
    struct SOCK_addrinfo res;
    struct SOCK_sockaddr sa;
    struct zsock_addrinfo *z_res;
};

void rep_zsockaddr_to_sockaddr(struct zsock_addrinfo *z, struct SOCK_addrinfo *a)
{
    a->ai_flags = z->ai_flags;
    a->ai_family = z->ai_family;
    a->ai_socktype = z->ai_socktype;
    a->ai_protocol = z->ai_protocol;
    a->ai_addrlen = z->ai_addrlen;
    if (z->ai_canonname)
        hal_strcpy_s(a->ai_canonname, sizeof(a->ai_canonname), z->ai_canonname);
    else
        a->ai_canonname = 0;

    if (z->ai_addr != 0)
    {
        if (a->ai_addr != 0)
        {
            memcpy(a->ai_addr, z->ai_addr, sizeof(SOCK_sockaddr));

            // Translate this into flags .net nf knows about
            if (a->ai_addr->sa_family == AF_INET)
                a->ai_addr->sa_family = SOCK_AF_INET;
            else if (a->ai_addr->sa_family == AF_INET6)
                a->ai_addr->sa_family = SOCK_AF_INET6;
        }
        else
            printk("**********  rep_zsockaddr_to_sockaddr a->ai_addr zero while z->ai_addr nonzero\n");
    }
    else
        a->ai_addr = 0;
    a->ai_next = 0;

    /*
        int ai_flags;
        int ai_family;
        int ai_socktype;
        int ai_protocol;
        size_t ai_addrlen;
        char *ai_canonname;
        struct SOCK_sockaddr *ai_addr;
        struct SOCK_addrinfo *ai_next;


        zsock
        int ai_flags;
        int ai_family;
        int ai_socktype;
        int ai_protocol;
        socklen_t ai_addrlen;
        struct sockaddr *ai_addr;
        char *ai_canonname;

        struct sockaddr _ai_addr;
        char _ai_canonname[DNS_MAX_NAME_SIZE + 1];
    */
}

void rep_sockaddr_to_zsockaddr(const struct SOCK_addrinfo *a, struct zsock_addrinfo *z)
{
    z->ai_flags = a->ai_flags;
    z->ai_family = a->ai_family;
    z->ai_socktype = a->ai_socktype;
    z->ai_protocol = a->ai_protocol;
    z->ai_addrlen = a->ai_addrlen;
    if (a->ai_canonname)
        hal_strcpy_s(z->ai_canonname, sizeof(z->ai_canonname), a->ai_canonname);
    else
        z->ai_canonname = 0;

    if (a->ai_addr != 0)
    {
        if (z->ai_addr != 0)
        {
            memcpy(z->ai_addr, a->ai_addr, sizeof(SOCK_sockaddr));
        }
        else
            printk("**********  rep_zsockaddr_sockaddr ai_addr zero but a->ai_addr non-zero\n");
    }
    else
        z->ai_addr = 0;
}
char *ipaddr2str (uint32_t addr)
{
    static char sz[48];
    sprintf (sz, "%d.%d.%d.%d", (addr && 0xff000000) >> 24,   (addr && 0xff0000) >> 16,
    (addr && 0xff00) >> 8, (addr && 0xff));
    return sz;
}
int HAL_SOCK_getaddrinfo(
    const char *nodename,
    char *servname,
    const struct SOCK_addrinfo *hints,
    struct SOCK_addrinfo **res)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int rc;
    // used to convert hints to zsock hints
    struct zsock_addrinfo ai;

    // Covert hints to zsock hints
    rep_sockaddr_to_zsockaddr(hints, &ai);

    // Alloc a compatible structure
    struct nf_addrinfo *internal_res = (struct nf_addrinfo *)platform_malloc(sizeof(struct nf_addrinfo));
    internal_res->res.ai_addr = &internal_res->sa;

    if (servname != 0)
    {
        if ((*servname > ' ') && (*servname < 0x7e))
        {
            for (int q = 0; q < 16; q++)
            {
                printk ("%x ", *(servname + q));
            }
            printk ("\n");
        }
        else
            printk ("servname unreadable %p\n", servname);
    }
    else
        printk ("servname zero\n");

    //printk ("server >%s<\n", servname);
    //printk("addr internal_res %x\n", (uint32_t)internal_res);

    // Fetch the zsock version
    rc = getaddrinfo(nodename, servname, &ai, &internal_res->z_res);
    if (rc == 0)
    {
        // Copy the zsock data to the SOCK_addrinfo data
        rep_zsockaddr_to_sockaddr(internal_res->z_res, &internal_res->res);
        *res = (SOCK_addrinfo *)internal_res;
        printk("addr res %s  \n", ipaddr2str((uint32_t)(*res)->ai_addr->sa_data));
    }
    last_sock_error = errno;
    return rc;
}
void HAL_SOCK_freeaddrinfo(struct SOCK_addrinfo *ai)
{
    NATIVE_PROFILE_PAL_NETWORK();

    // First, have the stack release it's block
    freeaddrinfo(((struct nf_addrinfo *)ai)->z_res);

    // Now, release ours.
    platform_free(ai);
}
int HAL_SOCK_ioctl(SOCK_SOCKET socket, int cmd, int *data)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int rc = -1;
    int flags;
    printk("ioctl sock %d cmd 0x%x \n", socket, cmd);
    switch ((uint32_t)cmd)
    {
        case SOCK_FIONBIO:
            printk("Set blocking %d\n", *data);
            flags = fcntl(socket, F_GETFL, 0);

            if (*data == 1)
            {
                fcntl(socket, F_SETFL, flags | (int)O_NONBLOCK);
            }
            else
            {
                fcntl(socket, F_SETFL, flags & ~(int)O_NONBLOCK);
            }
            rc = 0;
            break;
        case SOCK_FIONREAD:
            printk("SOCK_FIONREAD ");
            // Fake this ioctl by peeking at socket and seeing how
            // much data returned. This is non-optimal but should work.
            rc = GetAvailBytes (socket);
            *data = rc;
            break;
        default:
            printk("**********************************\n");
            printk("******* ERROR unsupported HAL_SOCK_ioct cmd %d\n", cmd);
            printk("**********************************\n");
            last_sock_error = errno;
            break;
    }
    return rc;
}
//static char tmpbuf[256];
static char tmpbuf[1520];
int GetAvailBytes (SOCK_SOCKET socket)
{
    int rc;
    printk ("GetAvailBytes soc %d available:", socket);
    rc = recv(socket, tmpbuf, sizeof(tmpbuf), MSG_DONTWAIT | MSG_PEEK);
    if (rc >= 0)
    {
        printk("%d bytes\n", rc);
        last_sock_error = 0;
    }
    else
    {
        if (errno == EAGAIN)
        {
            printk("0 bytes\n");
            rc = 0;
            last_sock_error = 0;
        }
        else
        {
            printk("\n error %d %d\n", rc, errno);
            last_sock_error = errno;
        }
    }
    return rc;
}

static int GetNativeError(int error)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

    switch (error)
    {
        case EINTR:
            ret = SOCK_EINTR;
            break;

        case EACCES:
            ret = SOCK_EACCES;
            break;

        case EFAULT:
            ret = SOCK_EFAULT;
            break;

        case EINVAL:
            ret = SOCK_EINVAL;
            break;

        case EMFILE:
            ret = SOCK_EMFILE;
            break;

        case EAGAIN:
        case EBUSY:
        /* case EWOULDBLOCK: same as EINPROGRESS */
        case EINPROGRESS:
            ret = SOCK_EWOULDBLOCK;
            break;

        case EALREADY:
            ret = SOCK_EALREADY;
            break;

        case ENOTSOCK:
            ret = SOCK_ENOTSOCK;
            break;

        case EDESTADDRREQ:
            ret = SOCK_EDESTADDRREQ;
            break;

        case EMSGSIZE:
            ret = SOCK_EMSGSIZE;
            break;

        case EPROTOTYPE:
            ret = SOCK_EPROTOTYPE;
            break;

        case ENOPROTOOPT:
            ret = SOCK_ENOPROTOOPT;
            break;

        case EPROTONOSUPPORT:
            ret = SOCK_EPROTONOSUPPORT;
            break;
            // TODO nanoframework check why missing
            // case ESOCKTNOSUPPORT:
            //     ret = SOCK_ESOCKTNOSUPPORT;
            //     break;

        case EPFNOSUPPORT:
            ret = SOCK_EPFNOSUPPORT;
            break;

        case EAFNOSUPPORT:
            ret = SOCK_EAFNOSUPPORT;
            break;

        case EADDRINUSE:
            ret = SOCK_EADDRINUSE;
            break;

        case EADDRNOTAVAIL:
            ret = SOCK_EADDRNOTAVAIL;
            break;

        case ENETDOWN:
            ret = SOCK_ENETDOWN;
            break;

        case ENETUNREACH:
            ret = SOCK_ENETUNREACH;
            break;

        case ENETRESET:
            ret = SOCK_ENETRESET;
            break;

        case ECONNABORTED:
            ret = SOCK_ECONNABORTED;
            break;

        case ECONNRESET:
            ret = SOCK_ECONNRESET;
            break;

        case ENOBUFS:
        case ENOMEM:
            ret = SOCK_ENOBUFS;
            break;

        case EISCONN:
            ret = SOCK_EISCONN;
            break;

        case ENOTCONN:
            ret = SOCK_EISCONN;
            break;

#if !defined(__GNUC__) // same as ENOTSOCK for GCC
        case ESHUTDOWN:
            ret = SOCK_ESHUTDOWN;
            break;
#endif

        case ETIMEDOUT:
            ret = SOCK_ETIMEDOUT;
            break;

        case ECONNREFUSED:
            ret = SOCK_ECONNREFUSED;
            break;

        case EHOSTDOWN:
            ret = SOCK_EHOSTDOWN;
            break;

        case EHOSTUNREACH:
            ret = SOCK_EHOSTUNREACH;
            break;

        case ENODATA:
            ret = SOCK_NO_DATA;
            break;

        default:
            ret = error;
            break;
    }

    return (ret);
}

int HAL_SOCK_getlasterror()
{
    NATIVE_PROFILE_PAL_NETWORK();
    int rc = GetNativeError(last_sock_error);
    printk("last error %d xlate: %d\n", last_sock_error, rc);
    return rc;
}
int HAL_SOCK_getsocklasterror(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return HAL_SOCK_getlasterror();
}

static int MARSHAL_SOCK_FDSET_TO_FDSET(SOCK_fd_set *sf, fd_set *f)
{
    if (f != NULL && sf != NULL)
    {
        FD_ZERO(f);

        for (unsigned int i = 0; i < sf->fd_count; i++)
        {
            FD_SET(sf->fd_array[i], f);
        }
        return sf->fd_count;
    }

    return 0;
}

static void MARSHAL_FDSET_TO_SOCK_FDSET(SOCK_fd_set *sf, fd_set *f)
{
    if (sf != NULL && f != NULL)
    {
        int cnt = sf->fd_count;
        sf->fd_count = 0;
        for (int i = 0; i < cnt; i++)
        {
            if (FD_ISSET(sf->fd_array[i], f))
            {
                sf->fd_array[sf->fd_count] = sf->fd_array[i];
                sf->fd_count++;
            }
        }
    }
}

int HAL_SOCK_select(
    int nfds,
    SOCK_fd_set *readfds,
    SOCK_fd_set *writefds,
    SOCK_fd_set *except,
    const struct SOCK_timeval *timeout)
{
    (void)nfds;

    NATIVE_PROFILE_PAL_NETWORK();
    int ret = 0;
    fd_set read;
    fd_set write;
    fd_set excpt;

    fd_set *pR = (readfds != NULL) ? &read : NULL;
    fd_set *pW = (writefds != NULL) ? &write : NULL;
    fd_set *pE = (except != NULL) ? &excpt : NULL;

    int r = 0, w = 0, e = 0;
    if (readfds != 0)
        r += readfds->fd_count;
    if (writefds != 0)
        w += writefds->fd_count;
    if (except != 0)
        e += except->fd_count;
    if (r || w || e)
    {
        printk("select: r %d w %d e %d ", r, w, e);
        if (timeout != 0)
            printk("no wait  ");
        else
            printk("wait  ");
    }

    // // If the network goes down then we should alert any pending socket actions
    // if (except != NULL && except->fd_count > 0)
    // {
    //     struct netif *networkInterface =
    //     netif_find_interface(g_LWIP_SOCKETS_Driver.m_interfaces[0].m_interfaceNumber);

    //     if (networkInterface != NULL)
    //     {
    //         if (!netif_is_up(networkInterface))
    //         {
    //             if (readfds != NULL)
    //                 readfds->fd_count = 0;
    //             if (writefds != NULL)
    //                 writefds->fd_count = 0;

    //             last_sock_error = ENETDOWN;

    //             return exceptfds->fd_count;
    //         }
    //     }
    // }

    MARSHAL_SOCK_FDSET_TO_FDSET(readfds, pR);
    MARSHAL_SOCK_FDSET_TO_FDSET(writefds, pW);
    MARSHAL_SOCK_FDSET_TO_FDSET(except, pE);

    // int max_sd = MEMP_NUM_NETCONN;
    int max_sd = CONFIG_NET_MAX_CONN;

    // developer note:
    // our declaration of SOCK_timeval is dependent of "long" type which is platform dependent
    // so it's not safe to cast it to "timeval"
    timeval timeoutCopy;
    timeoutCopy.tv_sec = timeout->tv_sec;
    timeoutCopy.tv_usec = timeout->tv_usec;

    bool noblock = false;
    if ((timeout->tv_sec == 0) && (timeout->tv_usec == 0))
        noblock = true;

    if (noblock)
    {
        // if (to_first)
        // {
        //     // schedule work func.
        // 	k_work_init_delayable(&select_delayed_work, select_delayed_work_fn);
        //     to_first = false;
        // }
    }

    ret = select(max_sd, pR, pW, pE, &timeoutCopy);
    printk("select returned %d  %d\n", ret, errno);

    // ENOBUFS=119
    // EAGAIN=11
    if (noblock && (ret == 0) && ((errno == 119) || (errno == 11)))
    {
        // printk ("Scheduling delayed work.\n");
        // k_work_reschedule(&select_delayed_work, K_MSEC(1000));
    }

    MARSHAL_FDSET_TO_SOCK_FDSET(readfds, pR);
    MARSHAL_FDSET_TO_SOCK_FDSET(writefds, pW);
    MARSHAL_FDSET_TO_SOCK_FDSET(except, pE);
    printk ("HAL_SOCK_select-- ret %d\n", ret);
    return ret;
}
int translate_opt_net_to_z(int *level, int *netopt)
{
    int rc = 0;
    if (*level == 0xffff) //.net socket level
    {
        *level = SOL_SOCKET;
    }
    else if (*level == 6)
    {
        // See if 'debug' option
        if (*netopt == 1)
        {
            printk("********** setsockopt: TCP / Debug option unsupported but fake it.\n");
            return -2; // Ignore
        }

        printk("********** setsockopt: TCP level unsupported\n");
        return -1;
    }
    else
    {
        printk("********** setsockopt: Unknown level %d\n", *level);
        return -1;
    }
    if (*level == SOL_SOCKET)
    {
        switch (*netopt)
        {
            case 4: // reuse addr
                *netopt = SO_REUSEADDR;
                break;

            // Not supported in zephyr
            case 8: // keepAlive
                printk("Opt 8 (keepalive) not supported\n");
                rc = -2; // Ignore
                break;

            case 1: // NoDelay
                printk("Opt 1 (NoDelay) not supported\n");
                rc = -2; // Ignore
                break;

            default:
                printk("********** setsockopt: Unknown option %d\n", *netopt);
                rc = -1;
                break;
        }
    }
    return rc;
}
int HAL_SOCK_setsockopt(SOCK_SOCKET socket, int level, int optname, const char *optval, int optlen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    printk("setsockopt++ sock %d level %x optname %d len %d\n", socket, level, optname, optlen);
    int org_level, org_optname;
    org_level = level;
    org_optname = optname;
    // Translate to something zephyr understands.
    int rc = translate_opt_net_to_z(&level, &optname);
    if (rc == 0)
    {
        rc = setsockopt(socket, level, optname, optval, optlen);
        if (rc != 0)
        {
            printk("***** setsockopt  %d %d %d fail err %d\n", socket, level, optname, errno);
            last_sock_error = errno;
        }
    }
    else if (rc == -2)
    {
        return 0;
    }
    else
    {
        printk("***** Error. Unknown translation lev 0x%x  opt:%d\n", org_level, org_optname);
    }
    return rc;
}
int HAL_SOCK_getsockopt(SOCK_SOCKET socket, int level, int optname, char *optval, int *optlen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    printk("HAL_SOCK_getsockopt++ sock %d  level %d  optname %d\n", socket, level, optname);
    int rc = getsockopt(socket, level, optname, optval, (unsigned int *)optlen);
    if (rc != 0)
    {
        printk(" ********* getsockopt failure %d  level %d opt %d\n", rc, level, optname);
        last_sock_error = errno;
    }
    return rc;
}
int HAL_SOCK_getpeername(SOCK_SOCKET socket, struct SOCK_sockaddr *name, int *namelen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int rc;
    printk("HAL_SOCK_getpeername  %d \n", socket);

    // Translate address into something zephyr knows about
    struct sockaddr addr;
    memset(&addr, 0, sizeof(addr));
    uint32_t len = sizeof(addr);
    addr.sa_family = AF_UNSPEC;
    rc = getpeername(socket, &addr, &len);
    if (rc == 0)
    {
        name->sa_family = z_family_2net(addr.sa_family);
        memcpy(&name->sa_data, &addr.data, sizeof(name->sa_data));
    }
    else
    {
        last_sock_error = errno;
        printk("Error callling getpeername rc %d errno %d\n", rc, last_sock_error);
    }
    return rc;
}
int HAL_SOCK_getsockname(SOCK_SOCKET socket, struct SOCK_sockaddr *name, int *namelen)
{
    NATIVE_PROFILE_PAL_NETWORK();

    // Translate address into something zephyr knows about
    struct sockaddr addr;
    return getsockname(socket, net_saddr_2z(name, &addr), (unsigned int *)namelen);
}
int HAL_SOCK_recvfrom(SOCK_SOCKET s, char *buf, int len, int flags, struct SOCK_sockaddr *from, int *fromlen)
{
    NATIVE_PROFILE_PAL_NETWORK();

    // Translate address into something zephyr knows about
    struct sockaddr addr;
    return recvfrom(s, buf, len, flags, net_saddr_2z(from, &addr), (unsigned int *)fromlen);
}
int HAL_SOCK_sendto(SOCK_SOCKET s, const char *buf, int len, int flags, const struct SOCK_sockaddr *to, int tolen)
{
    NATIVE_PROFILE_PAL_NETWORK();

    // Translate address into something zephyr knows about
    struct sockaddr addr;
    return sendto(s, buf, len, flags, net_saddr_2z(to, &addr), (unsigned int)tolen);
}

HRESULT HAL_SOCK_CONFIGURATION_LoadAdapterConfiguration(
    HAL_Configuration_NetworkInterface *config,
    uint32_t interfaceIndex)
{
    NATIVE_PROFILE_PAL_NETWORK();
    // return LWIP_SOCKETS_Driver::LoadAdapterConfiguration(config, interfaceIndex);
    //  printk ("**********************************\n");
    //  printk ("HAL_SOCK_CONFIGURATION_LoadAdapterConfiguration NOT IMPLIMENTED\n");
    //  printk ("**********************************\n");

    if (ConfigurationManager_GetConfigurationBlock(config, DeviceConfigurationOption_Network, interfaceIndex))
    {
        return 0;
    }
    return -1;
}

HRESULT HAL_SOCK_CONFIGURATION_UpdateAdapterConfiguration(
    HAL_Configuration_NetworkInterface *config,
    uint32_t interfaceIndex,
    uint32_t updateFlags)
{
    NATIVE_PROFILE_PAL_NETWORK();
    // printk ("**********************************\n");
    // printk ("HAL_SOCK_CONFIGURATION_UpdateAdapterConfiguration NOT IMPLIMENTED\n");
    // printk ("**********************************\n");
    // return LWIP_SOCKETS_Driver::UpdateAdapterConfiguration(interfaceIndex, updateFlags, config);

    if (ConfigurationManager_UpdateConfigurationBlock(config, DeviceConfigurationOption_Network, interfaceIndex))
    {
        return 0;
    }
    return -1;
}

HRESULT HAL_SOCK_CONFIGURATION_Link_status(uint32_t interfaceIndex, bool *status)
{
    NATIVE_PROFILE_PAL_NETWORK();
    // printk ("HAL_SOCK_CONFIGURATION_Link_status++ interface %d\n", interfaceIndex);
    if (connect_state == CONN_STATE_DISCONNECTED)
    {
        *status = false;
    }
    else
    {
        *status = true;
    }
    return S_OK;
}

HRESULT HAL_SOCK_IPAddressFromString(const char *ipString, uint64_t *address)
{
    NATIVE_PROFILE_PAL_NETWORK();

    // printk ("HAL_SOCK_IPAddressFromString++\n");
    int rc = inet_pton(AF_INET, ipString, address);
    if (rc == 0)
        return S_OK;
    return -1;
}
static char *str;
const char *HAL_SOCK_IPAddressToString(uint32_t address)
{
    NATIVE_PROFILE_PAL_NETWORK();
    printk("HAL_SOCK_IPAddressToString++ %x\n", address);
    static char buff[INET_ADDRSTRLEN] = {0};
#if 0    
    return inet_ntop(AF_INET, (const void *)&address, buff, sizeof (buff));
#else
    {
        str = inet_ntop(AF_INET, (const void *)&address, buff, sizeof(buff));
        printk("inet_ntop ret >%s<\n", str);
        return str;
    }
#endif
}

void HAL_SOCK_EventsSet(uint32_t events)
{
    NATIVE_PROFILE_PAL_NETWORK();
    ASSERT((events == SOCKET_EVENT_FLAG_SOCKET) || (events == SOCKET_EVENT_FLAG_SOCKETS_READY));

    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}

void *HAL_SOCK_GlobalLockContext()
{
    NATIVE_PROFILE_PAL_NETWORK();
    return NULL;
}
int signal_nf_stack(void)
{
    printk ("Signal NF Stack++\n");
    Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
    return 0;
}