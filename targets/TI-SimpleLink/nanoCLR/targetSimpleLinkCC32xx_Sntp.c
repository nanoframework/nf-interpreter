//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <targetSimpleLinkCC32xx_Sntp.h>
#include <targetSimpleLinkCC32xx_Threads.h>
#include <targetSimpleLinkCC32xx_ProvisioningTask.h>

// POSIX Header files
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// RTOS header files
// #include "FreeRTOS.h"
// #include "task.h"
//#include <ti/sysbios/BIOS.h>

#include <ti/drivers/net/wifi/slnetifwifi.h>
#include <ti/net/slnetsock.h>

// nanoFramework header files
#include <targetHAL.h>

static struct sntp_server SNTP_ServersList[NTP_SERVERS];

pthread_t sntpWorkingThread = (pthread_t)NULL;
void * SntpWorkingThread(void *arg0);

void sntp_init(void)
{
    struct sched_param priorityParams;
    pthread_attr_t threadAttributes;
    int retc;

    if(sntpWorkingThread == NULL)
    {
        pthread_attr_init(&threadAttributes);
        priorityParams.sched_priority = NF_TASK_PRIORITY;
        retc = pthread_attr_setschedparam(&threadAttributes, &priorityParams);
        retc |= pthread_attr_setstacksize(&threadAttributes, 1024);
        if (retc != 0)
        {
            // failed to set attributes
            while (1) {}
        }

        retc = pthread_create(&sntpWorkingThread, &threadAttributes, SntpWorkingThread, NULL);
        if(retc != 0)
        {
            // pthread_create() failed
            HAL_AssertEx();
            UART_PRINT("Unable to create SNTP thread \n");

            while(1)
            {
                ;
            }
        }
    }
}

void sntp_stop(void)
{
    pthread_cancel(sntpWorkingThread);
    sntpWorkingThread = NULL;
}

bool sntp_enabled(void)
{ 
    return (sntpWorkingThread != NULL)? true : false;
}

char* sntp_getservername(int index)
{ 
    if(index < NTP_SERVERS)
    {
        return SNTP_ServersList[index].name;
    }

    return NULL;
}

void sntp_setservername(int index, char* server)
{
    if(index < NTP_SERVERS)
    {
        SNTP_ServersList[index].name = server;
    }
}

static int32_t getAddrByName(const char *name, uint32_t *addr, uint16_t *family)
{
    int32_t       ifID;
    uint16_t      addrLen = 1;

    /* Query DNS for IPv4 address. */
    ifID = SlNetUtil_getHostByName(0, (char *)name, strlen(name), addr, &addrLen, SLNETSOCK_AF_INET);

    if(ifID < 0)
    {
        /* If call fails, try again for IPv6. */
        ifID = SlNetUtil_getHostByName(0, (char *)name, strlen(name), addr, &addrLen, SLNETSOCK_AF_INET6);
        if(ifID < 0)
        {
            /* return an error */
            return -1;
        }
        else
        {
            *family = SLNETSOCK_AF_INET6;
        }
    }
    else
    {
        *family = SLNETSOCK_AF_INET;
    }

    /* Return the interface ID */
    return (ifID);
}

static int32_t hasKissCode(char *str)
{
    if (strncmp((char *)SNTP_KOD_RATE_STR, str, SNTP_KOD_ERROR_CODE_SIZE) == 0)
    {
        return (SNTP_KOD_RATE_CODE);
    }
    else if (strncmp((char *)SNTP_KOD_DENY_STR, str, SNTP_KOD_ERROR_CODE_SIZE) == 0)
    {
        return (SNTP_KOD_DENY_CODE);
    }
    else if (strncmp((char *)SNTP_KOD_RSTR_STR, str, SNTP_KOD_ERROR_CODE_SIZE) == 0)
    {
        return (SNTP_KOD_RSTR_CODE);
    }
    else
    {
        return (0);
    }
}

static int32_t getTime(SlNetSock_Addr_t *server, uint16_t ifID,
        SlNetSock_Timeval_t *timeout, uint64_t *ntpTimeStamp)
{
    SNTP_Header_t sntpPkt;
    int32_t ret = 0;
    int16_t sd = -1;
    SlNetSocklen_t saLen;
    uint32_t integrityCheck;
    struct timespec tspec;

    if (server->sa_family == SLNETSOCK_AF_INET)
    {
        saLen = sizeof(SlNetSock_AddrIn_t);
    }
    else if (server->sa_family == SLNETSOCK_AF_INET6)
    {
        saLen = sizeof(SlNetSock_AddrIn6_t);
    }
    else
    {
        return (SNTP_EINVALIDFAMILY);
    }

    /* Create a UDP socket to communicate with NTP server */
    sd = SlNetSock_create(server->sa_family, SLNETSOCK_SOCK_DGRAM, SLNETSOCK_PROTO_UDP, ifID, 0);
    if (sd < 0)
    {
        return (SNTP_ESOCKCREATEFAIL);
    }

    ret = SlNetSock_connect(sd, server, saLen);
    if (ret < 0)
    {
        SlNetSock_close(sd);
        return (SNTP_ECONNECTFAIL);
    }

    if (timeout != NULL)
    {
        /* Set the timeout for the server response to the user's value */
        ret = SlNetSock_setOpt(sd, SLNETSOCK_LVL_SOCKET, SLNETSOCK_OPSOCK_RCV_TIMEO, timeout, sizeof(SlNetSock_Timeval_t));
        if (ret < 0)
        {
            SlNetSock_close(sd);
            return (SNTP_ESOCKOPTFAIL);
        }
    }

    /* Initialize the SNTP packet, setting version and mode = client */
    memset(&sntpPkt, 0, sizeof(SNTP_Header_t));
    sntpPkt.flags = SNTP_VERSION << 3;
    sntpPkt.flags |= SNTP_MODE_CLIENT;

    /* Set packet's transmit time as integrity check value */
    clock_gettime(CLOCK_REALTIME, &tspec);
    integrityCheck = tspec.tv_sec;
    sntpPkt.transmitTS[0] = integrityCheck;

    /* Send out our SNTP request to the current server */
    ret = SlNetSock_send(sd, (void *)&sntpPkt, sizeof(SNTP_Header_t), 0);
    if (ret < 0)
    {
        SlNetSock_close(sd);
        return (SNTP_ESENDFAIL);
    }

    memset(&sntpPkt, 0, sizeof(SNTP_Header_t));

    /* Retrieve the NTP packet from the socket and update our time. */
    ret = SlNetSock_recv(sd, &sntpPkt, sizeof(SNTP_Header_t), 0);
    if ((ret < 0) || (ret != sizeof(SNTP_Header_t)) || (sntpPkt.originateTS[0] != integrityCheck))
    {
        SlNetSock_close(sd);
        return (SNTP_ERECVFAIL);
    }

    /* Check for errors in server response */
    if (sntpPkt.stratum == 0)
    {
        /* Per RFC5905, we MUST handle Kiss O' Death packet */
        if ((sntpPkt.flags >> 6) == SNTP_NOSYNC)
        {
            /* KOD recv'd. Inspect kiss code & handle accordingly */
            ret = hasKissCode((char *)&sntpPkt.referenceID);

            if (ret == SNTP_KOD_RATE_CODE)
            {
                SlNetSock_close(sd);
                return (SNTP_ERATEBACKOFF);
            }
            /* Check for fatal kiss codes */
            else if ((ret == SNTP_KOD_DENY_CODE) || (ret == SNTP_KOD_RSTR_CODE))
            {
                SlNetSock_close(sd);
                return (SNTP_EFATALNORETRY);
            }
            /* Per RFC5905, other kiss codes are ignored */
        }
        else
        {
            /*
             *  A server response with stratum == 0, with no kiss
             *  code, is a fatal error. Mark server as invalid
             */
            SlNetSock_close(sd);
            return (SNTP_EINVALIDRESP);
        }
    }

    /* return the time in seconds */
    sntpPkt.transmitTS[0] = SlNetUtil_ntohl(sntpPkt.transmitTS[0]);
    sntpPkt.transmitTS[1] = SlNetUtil_ntohl(sntpPkt.transmitTS[1]);

    *ntpTimeStamp = ((uint64_t)sntpPkt.transmitTS[0] << 32) | sntpPkt.transmitTS[1];

    SlNetSock_close(sd);
    return (0);
}

int32_t SNTP_getTime(SlNetSock_Timeval_t *timeout, uint64_t *ntpTimeStamp)
{
    int32_t ret = 0;
    int32_t ifID;
    SlNetSock_AddrIn_t sa4;
    SlNetSock_AddrIn6_t sa6;
    SlNetSock_Addr_t *sa;
    uint32_t addr[4];
    uint16_t family;
    uint32_t i;
    size_t count = NTP_SERVERS;

    for (i = 0; i < count; i++)
    {
        memset(&addr, 0, sizeof(addr));
        ifID = getAddrByName(SNTP_ServersList[i].name, addr, &family);
        if(ifID >= 0)
        {
            if (family == SLNETSOCK_AF_INET)
            {
                sa4.sin_family = SLNETSOCK_AF_INET;
                sa4.sin_port = SlNetUtil_htons(NTP_SERVER_PORT);
                sa4.sin_addr.s_addr = SlNetUtil_htonl(addr[0]);
                sa = (SlNetSock_Addr_t *)&sa4;
            }
            else
            {
                sa6.sin6_family = SLNETSOCK_AF_INET6;
                sa6.sin6_port = SlNetUtil_htons(NTP_SERVER_PORT);
                sa6.sin6_addr._S6_un._S6_u32[0] = SlNetUtil_htonl(addr[0]);
                sa6.sin6_addr._S6_un._S6_u32[1] = SlNetUtil_htonl(addr[1]);
                sa6.sin6_addr._S6_un._S6_u32[2] = SlNetUtil_htonl(addr[2]);
                sa6.sin6_addr._S6_un._S6_u32[3] = SlNetUtil_htonl(addr[3]);
                sa = (SlNetSock_Addr_t *)&sa6;
            }
        }
        else
        {
            ret = SNTP_EGETHOSTBYNAMEFAIL;
            continue;
        }

        ret = getTime(sa, ifID, timeout, ntpTimeStamp);
        if (ret == 0)
        {
            break;
        }
    }

    return (ret);
}

////////////////////////////////////////////////////////////////////////////////
// SNTP working thread
////////////////////////////////////////////////////////////////////////////////
void* SntpWorkingThread(void* argument)
{
    (void)argument;

    uint64_t ntpTimeStamp;
    uint32_t currentTime;
    int32_t retval;
    time_t ts;
    SlNetSock_Timeval_t timeval;
    struct timespec tspec;

    // Set timeout value for NTP server reply
    timeval.tv_sec = NTP_REPLY_WAIT_TIME;
    timeval.tv_usec = 0;

    UART_PRINT("[SNTP task] started\n\r");

    // delay 1st request, if configured
    if(SNTP_STARTUP_DELAY > 0)
    {
        UART_PRINT("[SNTP task] start delay: %d\n\r", SNTP_STARTUP_DELAY);

        ClockP_sleep(SNTP_STARTUP_DELAY);
    }

    while(1)
    {
        // need to be connected and have an IP
        if(!IS_IP_ACQUIRED(nF_ControlBlock.Status))
        {
            UART_PRINT("[SNTP task] not connected. Retrying in %d seconds.\n\r", SNTP_RETRY_TIMEOUT);

            clock_gettime(CLOCK_REALTIME, &tspec);
            tspec.tv_sec += SNTP_RETRY_TIMEOUT;

            // wait for connection event with retry timeout
            // don't bother checking if it exited on timeout or event
            // because the loop is restarted and the check for connection & address is performed
            sem_timedwait(&Provisioning_ControlBlock.connectionAsyncEvent, &tspec);

            // retrying: start over
            continue;
        }

        UART_PRINT("[SNTP task] getting time...\n\r");

        // Get the time use the SNTP_ServersList
        retval = SNTP_getTime(&timeval, &ntpTimeStamp);

        if (retval != 0)
        {
            UART_PRINT("[SNTP task] failed to get time. Error: %d\n\r", retval);

            clock_gettime(CLOCK_REALTIME, &tspec);
            tspec.tv_sec += SNTP_RETRY_TIMEOUT;

            // wait for connection event with retry timeout
            // don't bother checking if it exited on timeout or event
            // because the loop is restarted and the check for connection & address is performed
            sem_timedwait(&Provisioning_ControlBlock.connectionAsyncEvent, &tspec);

            // retrying: start over
            continue;
        }

        currentTime = ntpTimeStamp >> 32;
        currentTime = TIME_NTP_TO_LOCAL(currentTime);

        tspec.tv_nsec = 0;
        tspec.tv_sec = currentTime;

        UART_PRINT("[SNTP task] updated time: %d\n\r", currentTime);

        if (clock_settime(CLOCK_REALTIME, &tspec) != 0)
        {
            // failed to set current time
            // don't do anything, just wait for the next attempt
        }

        // wait for connection event with retry timeout
        // don't bother checking if it exited on timeout or event
        // because the loop is restarted and the check for connection & address is performed
        clock_gettime(CLOCK_REALTIME, &tspec);
        tspec.tv_sec += SNTP_UPDATE_DELAY;

        sem_timedwait(&Provisioning_ControlBlock.connectionAsyncEvent, &tspec);
    }

    pthread_exit(0);
}
