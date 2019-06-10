//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>
#include <nanoCLR_Application.h>

// POSIX Header files
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// TI-RTOS Header files
#include <ti/drivers/GPIO.h>
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/slnetifwifi.h>

#include <targetSimpleLinkCC32xx_Sntp.h>

// Board Header files
#include "Board.h"

#include <targetHAL.h>
#include <nanoCLR_Application.h>
#include <WireProtocol_ReceiverThread.h>
#include <CLR_Startup_Thread.h>
#include <targetSimpleLinkCC32xx_Threads.h>
#include <targetSimpleLinkCC32xx_LinkLocalTask.h>
#include <targetSimpleLinkCC32xx_ProvisioningTask.h>

// externals from Simple Link sockets
extern void Status_callback();
extern void Link_callback(bool linkUp);

extern void sntp_init(void);

// other externals
extern void ConfigUART();

//////////////////////////////
#define SL_STOP_TIMEOUT         (200)

#define SPAWN_TASK_PRIORITY                 (9)
#define TASK_STACK_SIZE                     (2048)

#define SLNET_IF_WIFI_PRIO                  (5)

//////////////////////////////

// Stack size in bytes
#define THREADSTACKSIZE   4096

pthread_t provisioningThread = (pthread_t)NULL;
pthread_t slThread = (pthread_t)NULL;
pthread_t receiverThread = (pthread_t)NULL;
pthread_t nanoCLRThread = (pthread_t)NULL;

nanoFramework_CB nF_ControlBlock;

/*****************************************************************************
                  Callback Functions
*****************************************************************************/

//*****************************************************************************
//
//! The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Id)
    {
        case SL_WLAN_EVENT_CONNECT:
        {
            SET_STATUS_BIT(nF_ControlBlock.Status, AppStatusBits_Connection);
            CLR_STATUS_BIT(nF_ControlBlock.Status, AppStatusBits_IpAcquired);
            CLR_STATUS_BIT(nF_ControlBlock.Status,
                        AppStatusBits_Ipv6lAcquired);
            CLR_STATUS_BIT(nF_ControlBlock.Status,
                        AppStatusBits_Ipv6gAcquired);

            //    Information about the connected AP (like name, MAC etc) will be
            //    available in 'slWlanConnectAsyncResponse_t'-Applications
            //    can use it if required:

            //    slWlanConnectAsyncResponse_t *pEventData = NULL;
            //    pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            
            // Copy new connection SSID and BSSID to global parameters
            // memcpy(nF_ControlBlock.connectionSSID,
            //        pWlanEvent->Data.Connect.SsidName,
            //        pWlanEvent->Data.Connect.SsidLen);

            // nF_ControlBlock.ssidLen = pWlanEvent->Data.Connect.SsidLen;

            // memcpy(nF_ControlBlock.connectionBSSID,
            //        pWlanEvent->Data.Connect.Bssid,
            //        SL_WLAN_BSSID_LENGTH);

            UART_PRINT(
                "[WLAN EVENT] STA Connected to the AP: %s ,"
                "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                pWlanEvent->Data.Connect.SsidName,
                pWlanEvent->Data.Connect.Bssid[0],
                pWlanEvent->Data.Connect.Bssid[1],
                pWlanEvent->Data.Connect.Bssid[2],
                pWlanEvent->Data.Connect.Bssid[3],
                pWlanEvent->Data.Connect.Bssid[4],
                pWlanEvent->Data.Connect.Bssid[5]);

            sem_post(&Provisioning_ControlBlock.connectionAsyncEvent);

            Link_callback(true);
        }
        break;

        case SL_WLAN_EVENT_DISCONNECT:
        {
            SlWlanEventDisconnect_t*    pEventData = NULL;

            CLR_STATUS_BIT(nF_ControlBlock.Status, AppStatusBits_Connection);
            CLR_STATUS_BIT(nF_ControlBlock.Status, AppStatusBits_IpAcquired);
            CLR_STATUS_BIT(nF_ControlBlock.Status,
                        AppStatusBits_Ipv6lAcquired);
            CLR_STATUS_BIT(nF_ControlBlock.Status,
                        AppStatusBits_Ipv6gAcquired);

            pEventData = &pWlanEvent->Data.Disconnect;

            //  If the user has initiated 'Disconnect' request,
            //    'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED.
            
            if(SL_WLAN_DISCONNECT_USER_INITIATED == pEventData->ReasonCode)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the AP");
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP");
            }
            // memset(nF_ControlBlock.connectionSSID, 0,
            //        sizeof(nF_ControlBlock.connectionSSID));
            // memset(nF_ControlBlock.connectionBSSID, 0,
            //        sizeof(nF_ControlBlock.connectionBSSID));

            Link_callback(false);
        }
        break;

        case SL_WLAN_EVENT_STA_ADDED:
        {
            UART_PRINT(
                "[WLAN EVENT] External Station connected to SimpleLink AP\r\n");

            UART_PRINT("[WLAN EVENT] STA BSSID: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                    pWlanEvent->Data.STAAdded.Mac[0],
                    pWlanEvent->Data.STAAdded.Mac[1],
                    pWlanEvent->Data.STAAdded.Mac[2],
                    pWlanEvent->Data.STAAdded.Mac[3],
                    pWlanEvent->Data.STAAdded.Mac[4],
                    pWlanEvent->Data.STAAdded.Mac[5]);
        }
        break;

        case SL_WLAN_EVENT_STA_REMOVED:
        {
            UART_PRINT(
                "[WLAN EVENT] External Station disconnected from SimpleLink AP\r\n");
        }
        break;

        case SL_WLAN_EVENT_PROVISIONING_PROFILE_ADDED:
        {
            UART_PRINT("[WLAN EVENT] Profile Added\r\n");
        }
        break;

        case SL_WLAN_EVENT_PROVISIONING_STATUS:
        {
            uint16_t status =
                pWlanEvent->Data.ProvisioningStatus.ProvisioningStatus;
            switch(status)
            {
                case SL_WLAN_PROVISIONING_GENERAL_ERROR:
                case SL_WLAN_PROVISIONING_ERROR_ABORT:
                {
                    // SignalProvisioningEvent(PrvnEvent_Error);
                }
                break;

                case SL_WLAN_PROVISIONING_ERROR_ABORT_INVALID_PARAM:
                case SL_WLAN_PROVISIONING_ERROR_ABORT_HTTP_SERVER_DISABLED:
                case SL_WLAN_PROVISIONING_ERROR_ABORT_PROFILE_LIST_FULL:
                {
                    UART_PRINT("[WLAN EVENT] Provisioning Error status=%d\r\n", status);
                    // SignalProvisioningEvent(PrvnEvent_StartFailed);
                }
                break;

                case SL_WLAN_PROVISIONING_ERROR_ABORT_PROVISIONING_ALREADY_STARTED:
                {
                    UART_PRINT("[WLAN EVENT] Provisioning already started");
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_FAIL_NETWORK_NOT_FOUND:
                {
                    UART_PRINT("[WLAN EVENT] Confirmation fail: network not found\r\n");
                    // SignalProvisioningEvent(PrvnEvent_ConfirmationFailed);
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_FAIL_CONNECTION_FAILED:
                {
                    UART_PRINT("[WLAN EVENT] Confirmation fail: Connection failed\r\n");
                    // SignalProvisioningEvent(PrvnEvent_ConfirmationFailed);
                }
                break;

                case
                    SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_CONNECTION_SUCCESS_IP_NOT_ACQUIRED
                    :
                    {
                        UART_PRINT(
                            "[WLAN EVENT] Confirmation fail: IP address not acquired\r\n");
                        // SignalProvisioningEvent(PrvnEvent_ConfirmationFailed);
                    }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_SUCCESS_FEEDBACK_FAILED:
                {
                    UART_PRINT(
                        "[WLAN EVENT] Connection Success "
                        "(feedback to Smartphone app failed)\r\n");                    
                    // SignalProvisioningEvent(PrvnEvent_ConfirmationFailed);
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_SUCCESS:
                {
                    UART_PRINT("[WLAN EVENT] Confirmation Success!\r\n");
                    // SignalProvisioningEvent(PrvnEvent_ConfirmationSuccess);
                }
                break;

                case SL_WLAN_PROVISIONING_AUTO_STARTED:
                {
                    UART_PRINT("[WLAN EVENT] Auto-Provisioning Started\r\n");

                    // stop auto provisioning - 
                    // may trigger in case of returning to default
                    // SignalProvisioningEvent(PrvnEvent_Stopped);
                }
                break;

                case SL_WLAN_PROVISIONING_STOPPED:
                {
                    if(ROLE_STA == pWlanEvent->Data.ProvisioningStatus.Role)
                    {
                        UART_PRINT(" [WLAN EVENT] - WLAN Connection Status:%d\r\n",
                           pWlanEvent->Data.ProvisioningStatus.WlanStatus);

                        if(SL_WLAN_STATUS_CONNECTED ==
                        pWlanEvent->Data.ProvisioningStatus.WlanStatus)
                        {
                            UART_PRINT(" [WLAN EVENT] - Connected to SSID:%s\r\n",
                                    pWlanEvent->Data.ProvisioningStatus.Ssid);
                            // memcpy (nF_ControlBlock.connectionSSID,
                            //         pWlanEvent->Data.ProvisioningStatus.Ssid,
                            //         pWlanEvent->Data.ProvisioningStatus.Ssidlen);
                            // nF_ControlBlock.ssidLen =
                            //     pWlanEvent->Data.ProvisioningStatus.Ssidlen;

                            // Provisioning is stopped by the device and
                            // provisioning is done successfully
                            // SignalProvisioningEvent(PrvnEvent_Stopped);

                            break;
                        }
                        else
                        {
                            CLR_STATUS_BIT(nF_ControlBlock.Status,
                                        AppStatusBits_Connection);
                            CLR_STATUS_BIT(nF_ControlBlock.Status,
                                        AppStatusBits_IpAcquired);
                            CLR_STATUS_BIT(nF_ControlBlock.Status,
                                        AppStatusBits_Ipv6lAcquired);
                            CLR_STATUS_BIT(nF_ControlBlock.Status,
                                        AppStatusBits_Ipv6gAcquired);

                            // Provisioning is stopped by the device and provisioning 
                            // is not done yet, still need to connect to AP
                            // SignalProvisioningEvent(PrvnEvent_WaitForConn);

                            break;
                        }
                    }
                }
                // SignalProvisioningEvent(PrvnEvent_Stopped);
                break;

                case SL_WLAN_PROVISIONING_SMART_CONFIG_SYNCED:
                {
                    UART_PRINT("[WLAN EVENT] Smart Config Synced!\r\n");
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_WLAN_CONNECT:
                {
                    SET_STATUS_BIT(nF_ControlBlock.Status,
                                AppStatusBits_Connection);
                    CLR_STATUS_BIT(nF_ControlBlock.Status,
                                AppStatusBits_IpAcquired);
                    CLR_STATUS_BIT(nF_ControlBlock.Status,
                                AppStatusBits_Ipv6lAcquired);
                    CLR_STATUS_BIT(nF_ControlBlock.Status,
                                AppStatusBits_Ipv6gAcquired);

                    UART_PRINT("[WLAN EVENT] Connection to AP succeeded\r\n");
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_IP_ACQUIRED:
                {
                    SET_STATUS_BIT(nF_ControlBlock.Status,
                                AppStatusBits_IpAcquired);

                    UART_PRINT("[WLAN EVENT] IP address acquired\r\n");
                }
                break;

                case SL_WLAN_PROVISIONING_SMART_CONFIG_SYNC_TIMEOUT:
                {
                    UART_PRINT("[WLAN EVENT] Smart Config Sync timeout\r\n");
                }
                break;

                default:
                {
                    UART_PRINT("[WLAN EVENT] Unknown Provisioning Status: %d\r\n",
                            pWlanEvent->Data.ProvisioningStatus.ProvisioningStatus);
                }
                break;
            }
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                    pWlanEvent->Id);

            // SignalProvisioningEvent(PrvnEvent_Error);
        }
        break;
    }
}

//*****************************************************************************
//
//! The Function Handles the Fatal errors
//!
//! \param[in]  slFatalErrorEvent - Pointer to Fatal Error Event info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent)
{
    uint8_t msg = 4;
    int32_t msgqRetVal;

    switch(slFatalErrorEvent->Id)
    {
        case SL_DEVICE_EVENT_FATAL_DEVICE_ABORT:
        {
        }
        break;

        case SL_DEVICE_EVENT_FATAL_DRIVER_ABORT:
        {
        }
        break;

        case SL_DEVICE_EVENT_FATAL_NO_CMD_ACK:
        {
        }
        break;

        case SL_DEVICE_EVENT_FATAL_SYNC_LOSS:
        {
        }
        break;

        case SL_DEVICE_EVENT_FATAL_CMD_TIMEOUT:
        {
        }
        break;

        default:
            break;
    }

    // msgqRetVal = mq_send(controlMQueue, (char *)&msg, 1, 0);
    // if(msgqRetVal < 0)
    // {
    //     //UART_PRINT("[Control task] could not send element to msg queue\n\r");
    //     while(1)
    //     {
    //         ;
    //     }
    // }
}

//*****************************************************************************
//
//! This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    SlNetAppEventData_u *pNetAppEventData = NULL;

    if(NULL == pNetAppEvent)
    {
        return;
    }

    pNetAppEventData = &pNetAppEvent->Data;

    switch(pNetAppEvent->Id)
    {
        case SL_NETAPP_EVENT_IPV4_ACQUIRED:
        {
            SlIpV4AcquiredAsync_t   *pEventData = NULL;

            SET_STATUS_BIT(nF_ControlBlock.Status, AppStatusBits_IpAcquired);

            // Ip Acquired Event Data
            pEventData = &pNetAppEvent->Data.IpAcquiredV4;

            // Gateway IP address
            // nF_ControlBlock.gatewayIP = pEventData->Gateway;

            sem_post(&Provisioning_ControlBlock.connectionAsyncEvent);

            Status_callback();
        }
        break;

        case SL_NETAPP_EVENT_IPV6_ACQUIRED:
        {
            if(!GET_STATUS_BIT(nF_ControlBlock.Status,
                            AppStatusBits_Ipv6lAcquired))
            {
                SET_STATUS_BIT(nF_ControlBlock.Status,
                            AppStatusBits_Ipv6lAcquired);
            }
            else
            {
                SET_STATUS_BIT(nF_ControlBlock.Status,
                            AppStatusBits_Ipv6gAcquired);
            }

            sem_post(&Provisioning_ControlBlock.connectionAsyncEvent);

            Status_callback();        
        }
        break;

        case SL_NETAPP_EVENT_DHCPV4_LEASED:
        {
            SET_STATUS_BIT(nF_ControlBlock.Status, AppStatusBits_IpLeased);
        }
        break;

        case SL_NETAPP_EVENT_DHCPV4_RELEASED:
        {
            CLR_STATUS_BIT(nF_ControlBlock.Status, AppStatusBits_IpLeased);

            switch(pNetAppEventData->IpReleased.Reason)
            {
                case SL_IP_LEASE_PEER_RELEASE:
                    break;

                case SL_IP_LEASE_PEER_DECLINE:
                    break;

                case SL_IP_LEASE_EXPIRED:
                    break;
            }
        }
        break;

        case SL_NETAPP_EVENT_DHCP_IPV4_ACQUIRE_TIMEOUT:
        {
        }
        break;

        default:
        {
        }
        break;
    }
}

//*****************************************************************************
//
//! This function handles HTTP server events
//!
//! \param[in]  pServerEvent    - Contains the relevant event information
//! \param[in]  pServerResponse - Should be filled by the user with the
//!                               relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerEventHandler(SlNetAppHttpServerEvent_t *pHttpEvent,
    SlNetAppHttpServerResponse_t *
    pHttpResponse)
{
    // Unused in this application
}

//*****************************************************************************
//
//! This function handles General Events
//!
//! \param[in]  pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    uint8_t msg = 4;
    int32_t msgqRetVal;

    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application.
    
    if(NULL == pDevEvent)
    {
        return;
    }
    switch(pDevEvent->Id)
    {
        case SL_DEVICE_EVENT_RESET_REQUEST:
        {
        }
        break;

        default:
        {
            // msgqRetVal = mq_send(controlMQueue, (char *)&msg, 1, 0);
            // if(msgqRetVal < 0)
            // {
            //    //UART_PRINT("[Control task] could not send element to msg queue\n\r");
            //    while(1)
            //    {
            //         ;
            //    }
            // }
        }
        break;
    }
}

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]  pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(SL_SOCKET_ASYNC_EVENT == pSock->Event)
    {
        nFSlSocketAsyncEvent_t event;
        event.Sd = pSock->SocketAsyncEvent.SockAsyncData.Sd;
        event.Type = pSock->SocketAsyncEvent.SockAsyncData.Type;

        mq_send(nF_ControlBlock.socketAsyncEvent,
            (char *)&event, sizeof(nFSlSocketAsyncEvent_t), 0);

        switch(pSock->SocketAsyncEvent.SockAsyncData.Type)
        {
            case SL_SSL_NOTIFICATION_CONNECTED_SECURED:
                break;
            case SL_SSL_NOTIFICATION_HANDSHAKE_FAILED:
                break;
            case SL_SSL_ACCEPT:
                break;
            case SL_OTHER_SIDE_CLOSE_SSL_DATA_NOT_ENCRYPTED:
                break;
            case SL_SSL_NOTIFICATION_WRONG_ROOT_CA:
                break;
            default:
                break;
        }
    }

    // This application doesn't work w/ socket - Events are not expected
    switch(pSock->Event)
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch(pSock->SocketAsyncEvent.SockTxFailData.Status)
            {
                case SL_ERROR_BSD_ECLOSE:
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void SimpleLinkNetAppRequestMemFreeEventHandler(uint8_t *buffer)
{
    // Unused in this application
}


//////////// these are coming from link task, they probably don't belong here

void SimpleLinkSocketTriggerEventHandler(SlSockTriggerEvent_t *pSlTriggerEvent)
{
    // Unused in this application
    (void)pSlTriggerEvent;
}

void * mainThread(void *arg)
{
    struct sched_param priorityParams;

    pthread_attr_t threadAttributes;

    int retc;
    struct timespec ts = {0};

    // peripherals initialization
    // make this dependent on having the corresponding NF FEATURE enabled
    GPIO_init();
    UART_init();
    SPI_init();
    I2C_init();
    ADC_init();
    PWM_init();

    // initialize UART (need UART to be functional ASAP in order to output to terminal, if required)
    ConfigUART();

    // Initialize SlNetSock layer
    SlNetIf_init(0);
    SlNetIf_add(SLNETIF_ID_1, "nF",
                (const SlNetIf_Config_t *)&SlNetIfConfigWifi,
                SLNET_IF_WIFI_PRIO);

    SlNetSock_init(0);
    SlNetUtil_init(0);

    // initialize the realtime clock
    clock_settime(CLOCK_REALTIME, &ts);

    // Switch off all LEDs on boards
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);

    // clear SimpleLink Status
    nF_ControlBlock.Status = 0;

    // reset control block vars
    nF_ControlBlock.configurationDone = 0;

    // Create sockets Async event queue
    mq_attr attr;
    attr.mq_maxmsg = SLNETSOCK_MAX_CONCURRENT_SOCKETS;
    attr.mq_msgsize = sizeof(nFSlSocketAsyncEvent_t);
    nF_ControlBlock.socketAsyncEvent = mq_open("", O_CREAT, 0, &attr);

    // initializes signals for all tasks
    sem_init(&Provisioning_ControlBlock.connectionAsyncEvent, 0, 0);
    sem_init(&Provisioning_ControlBlock.provisioningDoneSignal, 0, 0);
    sem_init(&Provisioning_ControlBlock.provisioningConnDoneToOtaServerSignal,
             0,
             0);
    sem_init(&LinkLocal_ControlBlock.otaReportServerStartSignal, 0, 0);
    sem_init(&LinkLocal_ControlBlock.otaReportServerStopSignal, 0, 0);

    // Create the sl_Task internal spawn thread
    pthread_attr_init(&threadAttributes);
    priorityParams.sched_priority = SPAWN_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&threadAttributes, &priorityParams);
    retc |= pthread_attr_setstacksize(&threadAttributes, 2 * TASK_STACK_SIZE);

    // The SimpleLink host driver architecture mandate spawn 
    // thread to be created prior to calling Sl_start (turning the NWP on).
    // The purpose of this thread is to handle
    // asynchronous events sent from the NWP.
    // Every event is classified and later handled 
    // by the Host driver event handlers.
    retc = pthread_create(&slThread, &threadAttributes, sl_Task, NULL);
    if(retc)
    {
        // Handle Error
        UART_PRINT("Unable to create sl_Task thread \n");
        HAL_AssertEx();
        while(1)
        {
            ;
        }
    }

    // Before turning on the NWP on, reset any previously configured parameters
    // TODO: check if we should have a better reset implementation instead of using the standard one
    retc = sl_WifiConfig();
    if(retc < 0)
    {
        // Handle Error
        UART_PRINT("Network Terminal - Couldn't configure Network Processor - %d\n", retc);
        HAL_AssertEx();
        return(NULL);
    }

    // need to setup the network interface to enable IPv6 otherwise there are errors in socket 
    // when IPv6 addresses are brought in (for example on a response to a DNS query)
    // this can probably go away when support for IPv6 is official
    uint32_t ifBitmap = 0;
    ifBitmap = SL_NETCFG_IF_IPV6_LOCAL_STATEFUL | SL_NETCFG_IF_IPV6_GLOBAL_STATEFUL;
    sl_NetCfgSet(SL_NETCFG_IF, SL_NETCFG_IF_STATE, sizeof(ifBitmap), &ifBitmap);

    // start network processor
    retc = sl_Start(NULL, NULL, NULL);
    if(retc >= 0)
    {
        // we are good!
        // sl_Start returns on success the role that device started on
        nF_ControlBlock.Role = retc;
        ////////////////////////////////////// **** uncomment this if using provisioning
        // retc = sl_Stop(SL_STOP_TIMEOUT);
        // if(retc < 0)
        // {
        //     /* Handle Error */
        //     // UART_PRINT("\n sl_Stop failed\n");
        //     while(1)
        //     {
        //         ;
        //     }
        // }
        ////////////////////////////////////// ****
    }
    else if((retc < 0) && (retc != SL_ERROR_RESTORE_IMAGE_COMPLETE))
    {
        // Handle Error
        HAL_AssertEx();
        while(1)
        {
            ;
        }
    }

    // // provisioning task
    // pthread_attr_init(&threadAttributes);
    // priorityParams.sched_priority = NF_TASK_PRIORITY;
    // retc = pthread_attr_setschedparam(&threadAttributes, &priorityParams);
    // retc |= pthread_attr_setstacksize(&threadAttributes, TASK_STACK_SIZE);
    // if(retc)
    // {
    //     // Handle Error
    //     // UART_PRINT("Unable to configure provisioningTask thread parameters \n");
    //     while(1)
    //     {
    //         ;
    //     }
    // }
    // retc = pthread_create(&provisioningThread, &threadAttributes, provisioningTask, NULL);
    // if (retc != 0) {
    //     // Unable to create provisioningTask thread
    //     while (1) {}
    // }

    // retc |= pthread_attr_setstacksize(&threadAttributes, 2048);
    // if(retc)
    // {
    //     // Handle Error
    //     // UART_PRINT("Unable to create provisioningTask thread \n");
    //     while(1)
    //     {
    //         ;
    //     }
    // }

    // receiver thread
    pthread_attr_init(&threadAttributes);
    priorityParams.sched_priority = NF_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&threadAttributes, &priorityParams);
    retc |= pthread_attr_setstacksize(&threadAttributes, 1024);
    if (retc != 0)
    {
        // failed to set attributes
        while (1) {}
    }

    retc = pthread_create(&receiverThread, &threadAttributes, ReceiverThread, NULL);
    if(retc != 0)
    {
        // pthread_create() failed
        UART_PRINT("Unable to create receiver thread \n");
        HAL_AssertEx();
        while(1)
        {
            ;
        }
    }

    // CLR thread
    pthread_attr_init(&threadAttributes);
    priorityParams.sched_priority = NF_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&threadAttributes, &priorityParams);
    retc |= pthread_attr_setstacksize(&threadAttributes, 7168);
    if (retc != 0)
    {
        // failed to set attributes
        while (1) {}
    }

    // forward CLR_SETTINGS to CLR startup thread
    retc = pthread_create(&nanoCLRThread, &threadAttributes, CLRStartupThread, arg);
    if(retc != 0)
    {
        // pthread_create() failed
        UART_PRINT("Unable to create CLR thread \n");
        HAL_AssertEx();
        while(1)
        {
            ;
        }
    }

  #ifdef SL_APP_SNTP
    sntp_setservername(0, SNTP_SERVER_DEFAULT_ADDRESS);
    sntp_init();
  #endif

    return (0);
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    // Handle Memory Allocation Errors
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
// void vApplicationStackOverflowHook(TaskHandle_t pxTask,
//                                    char *pcTaskName)
// {
//     //Handle FreeRTOS Stack Overflow
//     while(1)
//     {
//     }
// }

void vApplicationTickHook(void)
{
    //  This function will be called by each tick interrupt if
    //  configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
    //  added here, but the tick hook is called from an interrupt context, so
    //  code must not attempt to block, and only the interrupt safe FreeRTOS API
    //  functions can be used (those that end in FromISR()).
    
}

void vPreSleepProcessing(uint32_t ulExpectedIdleTime)
{
}

//*****************************************************************************
//
//! \brief Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook(void)
{
    // Handle Idle Hook for Profiling, Power Management etc
}

//*****************************************************************************
//
//! \brief  Overwrite the GCC _sbrk function which check the heap limit related
//!         to the stack pointer.
//!         In case of freertos this checking will fail.
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void * _sbrk(uint32_t delta)
{
    extern char _end;     // Defined by the linker
    extern char __HeapLimit;
    static char *heap_end;
    static char *heap_limit;
    char *prev_heap_end;

    if(heap_end == 0)
    {
        heap_end = &_end;
        heap_limit = &__HeapLimit;
    }

    prev_heap_end = heap_end;
    if(prev_heap_end + delta > heap_limit)
    {
        return((void *) -1L);
    }
    heap_end += delta;
    return((void *) prev_heap_end);
}


// /*****************************************************************************
//                  Local Functions
// *****************************************************************************/

//*****************************************************************************
//
//! This  function clears and enables a GPIO pin interrupt flag
//!
//! \param    index - GPIO index
//!
//! \return None
//!
//*****************************************************************************
static void GPIO_clearAndEnable(uint8_t index)
{
    GPIO_clearInt(index);
    GPIO_enableInt(index);
}

void Platform_TimerInit(void (*timerIntHandler)(sigval val),
                        timer_t *timerId)
{
    sigevent sev;

    // Create Timer
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_notify_function = timerIntHandler;
    timer_create(CLOCK_MONOTONIC, &sev, timerId);
}

void Platform_TimerStart(uint32_t asyncEvtTimeoutMsec,
                         timer_t timerId,
                         uint8_t periodic)
{
    struct itimerspec value;

    // set the timeout
    value.it_value.tv_sec = (asyncEvtTimeoutMsec / 1000);
    value.it_value.tv_nsec = (asyncEvtTimeoutMsec % 1000) * 1000000;

    value.it_value.tv_sec += (value.it_value.tv_nsec / 1000000000);
    value.it_value.tv_nsec = value.it_value.tv_nsec % 1000000000;

    if(periodic)
    {
        // set as periodic timer
        value.it_interval.tv_sec = value.it_value.tv_sec;
        value.it_interval.tv_nsec = value.it_value.tv_nsec;
    }
    else
    {
        // set as one shot timer
        value.it_interval.tv_sec = 0;
        value.it_interval.tv_nsec = 0;
    }

    // kick the timer
    timer_settime(timerId, 0, &value, NULL);
}

void Platform_TimerStop(timer_t timerId)
{
    struct itimerspec value;

    // stop timer
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = 0;
    value.it_value.tv_sec = 0;
    value.it_value.tv_nsec = 0;
    timer_settime(timerId, 0, &value, NULL);
}

void Platform_TimerInterruptClear(void)
{
    // Do nothing...
}

//*****************************************************************************
//
//! \brief This function reboot the M4 host processor
//!
//! \param[in]  none
//!
//! \return none
//!
//****************************************************************************
void mcuReboot(void)
{
    // stop network processor activities before reseting the MCU
    sl_Stop(SL_STOP_TIMEOUT);

    // UART_PRINT("[Common] CC32xx MCU reset request\r\n");

    // Reset the MCU in order to test the bundle
    PRCMHibernateCycleTrigger();
}
