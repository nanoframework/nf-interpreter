#include <stdbool.h>

#include "nx_api.h"
#include "mx_wifi.h"
#ifndef NX_DRIVER_DEFERRED_PROCESSING
#error The symbol NX_DRIVER_DEFERRED_PROCESSING should be defined
#endif

#ifndef WIFI_SSID
#error The symbol WIFI_SSID should be defined
#endif /* WIFI_SSID  */

#ifndef WIFI_PASSWORD
#error The symbol WIFI_PASSWORD should be defined
#endif /* WIFI_PASSWORD  */

#define NX_DRIVER_ENABLE_DEFERRED

/* Indicate that driver source is being compiled.  */
#define NX_DRIVER_SOURCE

#include "nx_driver_emw3080.h"
#include "nx_driver_framework.c"


extern MX_WIFIObject_t * wifi_obj_get(void);
extern int32_t mxwifi_probe();

static void _nx_netlink_input_callback(mx_buf_t *pbuf,void *user_args);
static void _nx_mx_wifi_status_changed(uint8_t cate, uint8_t status, void * arg);

static UINT _nx_driver_emw3080_initialize(NX_IP_DRIVER *driver_req_ptr);
static UINT _nx_driver_emw3080_enable(NX_IP_DRIVER *driver_req_ptr);
static UINT _nx_driver_emw3080_disable(NX_IP_DRIVER *driver_req_ptr);
static UINT _nx_driver_emw3080_packet_send(NX_PACKET *packet_ptr);
static VOID _nx_driver_emw3080_packet_received(VOID);

static volatile bool nx_driver_interface_up = false;
static volatile bool nx_driver_ip_acquired = false;


void nx_driver_emw3080_entry(NX_IP_DRIVER *driver_req_ptr)
{
  static bool start = false;
  if (!start)
  {
    nx_driver_hardware_initialize         = _nx_driver_emw3080_initialize;
    nx_driver_hardware_enable             = _nx_driver_emw3080_enable;
    nx_driver_hardware_disable            = _nx_driver_emw3080_disable;
    nx_driver_hardware_packet_send        = _nx_driver_emw3080_packet_send;
    nx_driver_hardware_packet_received    = _nx_driver_emw3080_packet_received;

    start = true;
  }

  nx_driver_framework_entry_default(driver_req_ptr);
}

void nx_driver_emw3080_interrupt()
{
ULONG deffered_events;

  if (!nx_driver_interface_up || !nx_driver_ip_acquired)
    return; /* not yet running */

  deffered_events = nx_driver_information.nx_driver_information_deferred_events;

  nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_RECEIVED;

  if (!deffered_events)
  {
    /* Call NetX deferred driver processing.  */
    _nx_ip_driver_deferred_processing(nx_driver_information.nx_driver_information_ip_ptr);
  }
}

UINT _nx_driver_emw3080_initialize(NX_IP_DRIVER *driver_req_ptr)
{
  MX_WIFIObject_t * pMxWifiObj = wifi_obj_get();

  if (mx_wifi_alloc_init())
  {
    return NX_DRIVER_ERROR;
  }

  if (mxwifi_probe((void **)&pMxWifiObj))
  {
    return NX_DRIVER_ERROR;
  }

  if (MX_WIFI_HardResetModule(pMxWifiObj))
  {
    return NX_DRIVER_ERROR;
  }

  /* wait for mxchip wifi reboot */
  tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * 800 / 1000);

  if (MX_WIFI_Init(pMxWifiObj))
  {
    return NX_DRIVER_ERROR;
  }

  nx_driver_update_hardware_address(pMxWifiObj->SysInfo.MAC);

  if (MX_WIFI_RegisterStatusCallback_if(
    pMxWifiObj,
    _nx_mx_wifi_status_changed,
    NULL /* void * arg */,
    (mwifi_if_t)MC_STATION))
  {
    return NX_DRIVER_ERROR;
  }

  return NX_SUCCESS;
}

UINT _nx_driver_emw3080_enable(NX_IP_DRIVER *driver_req_ptr)
{
  MX_WIFIObject_t  *pMxWifiObj = wifi_obj_get();

  /* *** a WPS connection could be used instead
  if (MX_WIFI_WPS_Connect(pMxWifiObj))
  {
    return NX_DRIVER_ERROR;
  }
  */

  if (MX_WIFI_Connect(
    pMxWifiObj,
    WIFI_SSID,
    WIFI_PASSWORD,
    MX_WIFI_SEC_AUTO))
  {
    return NX_DRIVER_ERROR;
  }

  while (!nx_driver_interface_up)
  {
    MX_WIFI_IO_YIELD(pMxWifiObj, 10 /* timeout */);
  }

  if (MX_WIFI_STATUS_OK != MX_WIFI_Network_bypass_mode_set(
    pMxWifiObj,
    1 /* enable */,
    _nx_netlink_input_callback,
    NULL /* user arg */))
  {
    return NX_DRIVER_ERROR;
  }

  return NX_SUCCESS;
}

UINT _nx_driver_emw3080_disable(NX_IP_DRIVER *driver_req_ptr)
{
  MX_WIFIObject_t  *pMxWifiObj = wifi_obj_get();

  MX_WIFI_Network_bypass_mode_set(
    pMxWifiObj,
    0 /* disable */,
    NULL,
    NULL);

  return NX_SUCCESS;
}

UINT _nx_driver_emw3080_packet_send(NX_PACKET *packet_ptr)
{
  MX_WIFIObject_t  *pMxWifiObj = wifi_obj_get();

  static int errors = 0;

  // verify that the length matches the size between the pointers
  if (packet_ptr->nx_packet_length != (packet_ptr->nx_packet_append_ptr - packet_ptr->nx_packet_prepend_ptr))
  {
    return NX_DRIVER_ERROR;
  }

  // TODO: chained packets not currently supported
  if (packet_ptr->nx_packet_next)
  {
    NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
    nx_packet_transmit_release(packet_ptr);
    return NX_DRIVER_ERROR;
  }

  if ((packet_ptr->nx_packet_prepend_ptr - packet_ptr->nx_packet_data_start) < 28)
  {
    printf("Incorrect NX packet ,need at least 28 byte in front of payload , got %d\n",packet_ptr->nx_packet_prepend_ptr - packet_ptr->nx_packet_data_start);
  }

  if (MX_WIFI_Network_bypass_netlink_output(
    pMxWifiObj,
    packet_ptr->nx_packet_prepend_ptr, packet_ptr->nx_packet_length,
    (int32_t)STATION_IDX))
  {
    errors++;
  }

  NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
  nx_packet_transmit_release(packet_ptr);

  return NX_SUCCESS;
}

static void _nx_netlink_input_callback(mx_buf_t *buffer, void *user_args) // (void *pbuf, int32_t size, void *data, int32_t len, void *user_args)
{
  NX_PACKET * packet_ptr = buffer;

  /* Avoid starving.  */
  if (packet_ptr -> nx_packet_pool_owner -> nx_packet_pool_available == 0)
  {
    nx_packet_release(packet_ptr);
    return;
  }

  /* Everything is OK, transfer the packet to NetX.  */
  nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, packet_ptr);
}

static VOID _nx_driver_emw3080_packet_received(VOID)
{
  MX_WIFIObject_t  *pMxWifiObj = wifi_obj_get();

  MX_WIFI_IO_YIELD(pMxWifiObj, 100 /* timeout */);
}

static void _nx_mx_wifi_status_changed(uint8_t cate, uint8_t status, void * arg)
{
  if ((uint8_t)MC_STATION == cate)
  {
    switch (status)
    {
      case MWIFI_EVENT_STA_DOWN:
        nx_driver_interface_up = false;
        break;

      case MWIFI_EVENT_STA_UP:
        nx_driver_interface_up = true;
        break;

      case MWIFI_EVENT_STA_GOT_IP:
        nx_driver_ip_acquired = true;
        break;

      default:
        break;
    }
  }
  else if ((uint8_t)MC_SOFTAP == cate)
  {
    switch (status)
    {
      case MWIFI_EVENT_AP_DOWN:
        break;

      case MWIFI_EVENT_AP_UP:
        break;

      default:
        break;
    }
  }
  else
  {
    /* nothing */
  }
}
