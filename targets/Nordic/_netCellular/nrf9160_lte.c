/*
 * Copyright (c) Ericsson AB 2020, all rights reserved

+CGCONTRDP: 0,,"internet.cxn","","","193.181.246.58","193.181.246.57",,,,,1500
 */

#include <zephyr.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <modem/modem_info.h>
#include <nrf_modem_at.h>

#include <modem/nrf_modem_lib.h>

#include <date_time.h>
#include <modem/lte_lc.h>
#include <net/socket.h>

#include <nrf9160_lte.h>

// #include <nanoHAL_v2.h>
// #include <nanoPAL_Events.h>

int signal_nf_stack(void);

BUILD_ASSERT(
    !IS_ENABLED(CONFIG_LTE_AUTO_INIT_AND_CONNECT),
    "The nrf9160_lte library does not support LTE auto-init and connect");

static struct k_work_delayable connect_work;

void lte_evt_handler(const struct lte_lc_evt *const evt);

static int query_modem(const char *cmd, char *buf, size_t buf_len);
int nrf9160_lte_connect(uint16_t timeout);
int get_ip_info(uint32_t *ip_addr, uint32_t *dns1, uint32_t *dns2);

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
/* Certificate for TLS */
static const char cert[] = {
#include "cert/GlobalSign-Root-CA-R2"
};

BUILD_ASSERT(sizeof(cert) < KB(4), "Certificate too large");

int cert_provision(void);

#endif // CONFIG_NET_SOCKETS_SOCKOPT_TLS

int handle_new_connection(int);

// static struct modem_param_info modem_param;

bool fconnected = false;
char szIMEI[IMxI_LEN];
char szModemVer[FW_VER_LEN];

uint32_t ip4_addr;
uint32_t ip4_dns1_addr;
uint32_t ip4_dns2_addr;

char connect_state = CONN_STATE_DISCONNECTED;

// struct lte_defs defs;
/*
 * nrf9160_lte_init - Initialize LTE and connect with the network.
 */
int nrf9160_lte_init(uint16_t timeout)
{
    int rc;

    // Init the Nordic LTE library
    rc = lte_lc_init();

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
    /* Provision certificates before connecting to the LTE network */
    rc = cert_provision();
    if (rc)
    {
        printk("***********************************************\n");
        printk("Failed to privision TLS cert. %d\nLTE init failed.\n", rc);
        printk("***********************************************\n");
        return -1;
    }

#endif // CONFIG_NET_SOCKETS_SOCKOPT_TLS

    rc = nrf9160_lte_connect(timeout);
    if (rc != 0)
    {
        printk("nrf9160_lte_connect fail. rc %d\n", rc);
    }
    return rc;
}

/*
 * handles events from the LTE lib
 */
void lte_evt_handler(const struct lte_lc_evt *const evt)
{
    char sz[128];
    bool flag_error = false;
    switch (evt->type)
    {
        case LTE_LC_EVT_NW_REG_STATUS:
            strcpy(sz, "*******   ");
            switch (evt->nw_reg_status)
            {
                case LTE_LC_NW_REG_NOT_REGISTERED:
                    strcat(sz, "Not Registered");
                    if (fconnected)
                    {
                        fconnected = false;
                        connect_state = CONN_STATE_DISCONNECTED;
                        // TODO:: Notify of disconnect.
                    }
                    break;
                case LTE_LC_NW_REG_REGISTERED_HOME:
                    strcat(sz, "Registered Home");
                    if (!fconnected)
                    {
                        fconnected = true;
                        connect_state = CONN_STATE_CONN_HOME;
                        // TODO:: Notify of connect.
                        k_work_schedule(&connect_work, K_NO_WAIT);
                        // handle_new_connection (0);
                    }
                    break;
                case LTE_LC_NW_REG_SEARCHING:
                    strcat(sz, "Searching..");
                    break;
                case LTE_LC_NW_REG_REGISTRATION_DENIED:
                    strcat(sz, "Registration denied");
                    flag_error = true;
                    fconnected = false;
                    break;
                case LTE_LC_NW_REG_UNKNOWN:
                    strcat(sz, "Registration unknown");
                    break;
                case LTE_LC_NW_REG_REGISTERED_ROAMING:
                    strcat(sz, "Registered Roaming");
                    if (!fconnected)
                    {
                        fconnected = true;
                        connect_state = CONN_STATE_CONN_HOME;
                        // TODO:: Notify of connect.
                        k_work_schedule(&connect_work, K_NO_WAIT);
                    }
                    break;
                case LTE_LC_NW_REG_REGISTERED_EMERGENCY:
                    strcat(sz, "Registered Emergency");
                    flag_error = true;
                    break;
                case LTE_LC_NW_REG_UICC_FAIL:
                    strcat(sz, "UICC Fail");
                    flag_error = true;
                    fconnected = false;
                    break;
                default:
                    strcat(sz, "Unknown network status");
                    break;
            }
            if (flag_error)
            {
                printk("!!!!!! %s.  !!!!!!\n", sz);
            }
            else
            {
                printk("%s.\n", sz);
            }
            break;

            break;
        default:
            break;
    }
}
/*
 * date_time sync stuff.
 */
#ifdef CONFIG_DATE_TIME
#define DATE_TIME_TIMEOUT_S 10
K_SEM_DEFINE(date_time_obtained, 0, 1);
/*
 * date_time_event_handler - callback function recording time query
 */
static void date_time_event_handler(const struct date_time_evt *evt)
{
    switch (evt->type)
    {
        case DATE_TIME_OBTAINED_MODEM:
            printk("date_time obtained (modem)\n");
            break;
        case DATE_TIME_OBTAINED_NTP:
            printk("date_time obtained (ntp)\n");
            break;
        case DATE_TIME_OBTAINED_EXT:
            printk("date_time obtained (ext)\n");
            break;
        case DATE_TIME_NOT_OBTAINED:
            printk("date_time read failed\n");
            break;
        default:
            break;
    }

    /** Do not depend on obtained time, continue upon any event from the
     *  date time library.
     */
    k_sem_give(&date_time_obtained);
}
#endif
/*
 * Triggered by the LTE status callback when a connection is detected.
 */
static void connect_work_fn(struct k_work *work)
{
    int rc = -1;
    printk("LTE Network connected.\n");

    // Get the IP addresses
    rc = get_ip_info(&ip4_addr, &ip4_dns1_addr, &ip4_dns2_addr);
    // printk ("get_ip_info returned %x %x %x\n", ip4_addr, ip4_dns1_addr, ip4_dns2_addr);

#ifdef CONFIG_DATE_TIME
    date_time_update_async(date_time_event_handler);
    // printk("Waiting for date_time...");
    // rc = k_sem_take(&date_time_obtained, K_SECONDS(DATE_TIME_TIMEOUT_S));
    // if (rc)
    // {
    //     printk("Date time, no callback event within %d seconds\n", DATE_TIME_TIMEOUT_S);
    // }
#endif
    signal_nf_stack();
    return;
}

// /*
//  * collect_modem_info - Collects connection info and writes to the database
//  */
// static int collect_modem_info(int attempts, int wait_interval)
// {
//     int rc = -1;

//     printk("Get modem parameters\n");
//     rc = modem_info_params_get(&modem_param);
//     while ((rc < 0) && (attempts-- > 0))
//     {
//         k_sleep(K_MSEC(wait_interval));
//         printk("Get modem parameters re-attempt\n");
//         rc = modem_info_params_get(&modem_param);
//     }
//     if (rc < 0)
//     {
//         printk("Unable to obtain all modem parameters: %d\n", rc);
//     }
//     return rc;
// }

int comma_parse(char *buf, int inlen, int find_comma, char *outbuf, int outlen)
{
    int i, j, k = 0, rc = 0;
    int comma_cnt = 0;

    //    printk ("comma_parse++ %d >%s<\n", find_comma, buf);

    for (i = 0; (i < inlen) && (rc == 0); i++)
    {
        if (buf[i] == '\0')
        {
            rc = -1;
            break;
        }
        if (buf[i] == ',')
        {
            comma_cnt++;
            // printk ("Comma at %d\n", i);
        }
        if (comma_cnt == find_comma)
        {
            // We expect the string to have a starting quote.
            if (buf[i + 1] != '\"')
            {
                // printk("i %d char %c\n", i, buf[i + 1]);
                rc = -2;
            }
            // Find closing quote
            for (j = i + 2; j < inlen; j++)
            {
                // Unexpected end of input buf
                if (buf[j] == '\0')
                {
                    rc = -3;
                    break;
                }
                // Found closing quote.
                if (buf[j] == '\"')
                {
                    outbuf[k] = '\0'; // zero terminate
                    rc = k;           // Copy length
                    break;
                }
                else
                {
                    // Copy char if room.
                    if (k >= outlen - 1)
                    {
                        outbuf[k] = '\0';
                        rc = -4;
                        break;
                    }
                    outbuf[k++] = buf[j];
                }
            }
        }
    }
    // printk ("comma_parse-- %d", rc);
    // if (rc > 0)
    //     printk (">%s<\n", outbuf);
    // else
    //     printk ("\n");
    return rc;
}
/*
 *  +CGPADDR: 1,"10.0.0.130","1050:0000:0000:0000:0005:0600:300c:326b"
 *  +CGCONTRDP: 0,,"ibasis.iot","","","216.168.184.184","62.93.146.209",,,,,1430
 */
int get_ip_info(uint32_t *ip_addr, uint32_t *dns1, uint32_t *dns2)
{
    int rc;
    char buf[128];
    char outbuf[96];
    struct sockaddr_in sa;

    *ip_addr = 0;
    *dns1 = 0;
    *dns2 = 0;

    // First, convert IP address.
    memset(buf, 0, sizeof(buf));
    // Use specific cmd to get the IP addresses
    rc = query_modem("AT+CGPADDR=0", buf, sizeof(buf));
    // printk ("query_modem ret %d >%s<\n", rc, buf);
    if (rc == 0)
    {
        // printk ("IP addr: %s\n", buf);
        rc = comma_parse(buf, strlen(buf), 1, outbuf, sizeof(outbuf));
        if (rc > 0)
        {
            rc = inet_pton(AF_INET, outbuf, &sa.sin_addr);
            if (rc)
            {
                *ip_addr = sa.sin_addr.s_addr;
                printk("device ip: %x\n", *ip_addr);
            }
        }
    }
    else
    {
        printk("Failed to get ip address from modem. %d\n", rc);
    }
    // Now use cmd to get the DNS addresses
    rc = query_modem("AT+CGCONTRDP=0", buf, sizeof(buf));
    if (rc == 0)
    {
        // printk ("modem string: %s\n", buf);
        //  DNS 1
        rc = comma_parse(buf, strlen(buf), 5, outbuf, sizeof(outbuf));
        if (rc > 0)
        {
            rc = inet_pton(AF_INET, outbuf, &sa.sin_addr);
            if (rc)
                *dns1 = sa.sin_addr.s_addr;
        }
        // DNS 2
        rc = comma_parse(buf, strlen(buf), 6, outbuf, sizeof(outbuf));
        if (rc > 0)
        {
            rc = inet_pton(AF_INET, outbuf, &sa.sin_addr);
            if (rc)
                *dns2 = sa.sin_addr.s_addr;
        }
    }
    else
    {
        printk("Failed to read DNS %d\n", rc);
    }
    return rc;
}

/*
 * nrf9160_lte_connect - Connect to the network.
 */
int nrf9160_lte_connect(uint16_t timeout)
{
    int rc = 0;
    k_work_init_delayable(&connect_work, connect_work_fn);

    printk("Calling lte_lc_connect_async\n");
    rc = lte_lc_connect_async(lte_evt_handler);
    return rc;
}

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
/* Provision certificate to modem */
int cert_provision(void)
{
    int rc;
    bool exists;

    rc = modem_key_mgmt_exists(TLS_SEC_TAG, MODEM_KEY_MGMT_CRED_TYPE_CA_CHAIN, &exists);
    if (rc)
    {
        printk("Failed to check for certificates rc %d\n", rc);
        return rc;
    }

    if (exists)
    {
        /* For the sake of simplicity we delete what is provisioned
         * with our security tag and reprovision our certificate.
         */
        rc = modem_key_mgmt_delete(TLS_SEC_TAG, MODEM_KEY_MGMT_CRED_TYPE_CA_CHAIN);
        if (rc)
        {
            printk("Failed to delete existing certificate, rc %d\n", rc);
        }
    }

    printk("Provisioning certificate\n");

    /*  Provision certificate to the modem */
    rc = modem_key_mgmt_write(TLS_SEC_TAG, MODEM_KEY_MGMT_CRED_TYPE_CA_CHAIN, cert, sizeof(cert) - 1);
    if (rc)
    {
        printk("Failed to provision certificate, rc %d\n", rc);
        return rc;
    }
    return 0;
}

#endif // CONFIG_NET_SOCKETS_SOCKOPT_TLS
/*
 * remove_whitespace - helper for query_modem below.
 */
static int remove_whitespace(char *buf)
{
    size_t i, j = 0, len;

    len = strlen(buf);
    for (i = 0; i < len; i++)
    {
        if (buf[i] >= 32 && buf[i] <= 126)
        {
            if (j != i)
            {
                buf[j] = buf[i];
            }
            j++;
        }
    }
    if (j < len)
    {
        buf[j] = '\0';
    }

    return 0;
}
/*
 * query_modem - simpler way to send a cmd to the modem.
 */
static int query_modem(const char *cmd, char *buf, size_t buf_len)
{
    int rc;
    rc = nrf_modem_at_cmd(buf, buf_len, cmd);
    if (rc)
    {
        printk("nrf_modem_at_cmd [%s] error:%d\n", cmd, rc);
        return rc;
    }

    remove_whitespace(buf);
    return 0;
}

/*
 * nrf9160_lte_offline - Disconnect from the network.
 */
int nrf9160_lte_offline(void)
{
    return lte_lc_offline();
}
/*
 * nrf9160_lte_power_off - Turn off the modem.
 */
int nrf9160_lte_power_off(void)
{
    // TODO: check this is signaled by LTE callback.
    return lte_lc_power_off();
}
