#ifndef _WICED_H
#define _WICED_H

#include <stdint.h>

// #include "nx_api.h"
// #include "tx_api.h"

// wwd_constants.h
#define RESULT_ENUM(prefix, name, value) prefix##name = (value)

#define WWD_RESULT_LIST(prefix)                                                                                        \
    RESULT_ENUM(prefix, SUCCESS, 0), RESULT_ENUM(prefix, PENDING, 1), RESULT_ENUM(prefix, TIMEOUT, 2),                 \
        RESULT_ENUM(prefix, BADARG, 5), RESULT_ENUM(prefix, UNFINISHED, 10),                                           \
        RESULT_ENUM(prefix, PARTIAL_RESULTS, 1003), RESULT_ENUM(prefix, INVALID_KEY, 1004),                            \
        RESULT_ENUM(prefix, DOES_NOT_EXIST, 1005), RESULT_ENUM(prefix, NOT_AUTHENTICATED, 1006),                       \
        RESULT_ENUM(prefix, NOT_KEYED, 1007), RESULT_ENUM(prefix, IOCTL_FAIL, 1008),                                   \
        RESULT_ENUM(prefix, BUFFER_UNAVAILABLE_TEMPORARY, 1009),                                                       \
        RESULT_ENUM(prefix, BUFFER_UNAVAILABLE_PERMANENT, 1010), RESULT_ENUM(prefix, CONNECTION_LOST, 1012),           \
        RESULT_ENUM(prefix, OUT_OF_EVENT_HANDLER_SPACE, 1013), RESULT_ENUM(prefix, SEMAPHORE_ERROR, 1014),             \
        RESULT_ENUM(prefix, FLOW_CONTROLLED, 1015), RESULT_ENUM(prefix, NO_CREDITS, 1016),                             \
        RESULT_ENUM(prefix, NO_PACKET_TO_SEND, 1017), RESULT_ENUM(prefix, CORE_CLOCK_NOT_ENABLED, 1018),               \
        RESULT_ENUM(prefix, CORE_IN_RESET, 1019), RESULT_ENUM(prefix, UNSUPPORTED, 1020),                              \
        RESULT_ENUM(prefix, BUS_WRITE_REGISTER_ERROR, 1021), RESULT_ENUM(prefix, SDIO_BUS_UP_FAIL, 1022),              \
        RESULT_ENUM(prefix, JOIN_IN_PROGRESS, 1023), RESULT_ENUM(prefix, NETWORK_NOT_FOUND, 1024),                     \
        RESULT_ENUM(prefix, INVALID_JOIN_STATUS, 1025), RESULT_ENUM(prefix, UNKNOWN_INTERFACE, 1026),                  \
        RESULT_ENUM(prefix, SDIO_RX_FAIL, 1027), RESULT_ENUM(prefix, HWTAG_MISMATCH, 1028),                            \
        RESULT_ENUM(prefix, RX_BUFFER_ALLOC_FAIL, 1029), RESULT_ENUM(prefix, BUS_READ_REGISTER_ERROR, 1030),           \
        RESULT_ENUM(prefix, THREAD_CREATE_FAILED, 1031), RESULT_ENUM(prefix, QUEUE_ERROR, 1032),                       \
        RESULT_ENUM(prefix, BUFFER_POINTER_MOVE_ERROR, 1033), RESULT_ENUM(prefix, BUFFER_SIZE_SET_ERROR, 1034),        \
        RESULT_ENUM(prefix, THREAD_STACK_NULL, 1035), RESULT_ENUM(prefix, THREAD_DELETE_FAIL, 1036),                   \
        RESULT_ENUM(prefix, SLEEP_ERROR, 1037), RESULT_ENUM(prefix, BUFFER_ALLOC_FAIL, 1038),                          \
        RESULT_ENUM(prefix, NO_PACKET_TO_RECEIVE, 1039), RESULT_ENUM(prefix, INTERFACE_NOT_UP, 1040),                  \
        RESULT_ENUM(prefix, DELAY_TOO_LONG, 1041), RESULT_ENUM(prefix, INVALID_DUTY_CYCLE, 1042),                      \
        RESULT_ENUM(prefix, PMK_WRONG_LENGTH, 1043), RESULT_ENUM(prefix, UNKNOWN_SECURITY_TYPE, 1044),                 \
        RESULT_ENUM(prefix, WEP_NOT_ALLOWED, 1045), RESULT_ENUM(prefix, WPA_KEYLEN_BAD, 1046),                         \
        RESULT_ENUM(prefix, FILTER_NOT_FOUND, 1047), RESULT_ENUM(prefix, SPI_ID_READ_FAIL, 1048),                      \
        RESULT_ENUM(prefix, SPI_SIZE_MISMATCH, 1049), RESULT_ENUM(prefix, ADDRESS_ALREADY_REGISTERED, 1050),           \
        RESULT_ENUM(prefix, SDIO_RETRIES_EXCEEDED, 1051), RESULT_ENUM(prefix, NULL_PTR_ARG, 1052),                     \
        RESULT_ENUM(prefix, THREAD_FINISH_FAIL, 1053), RESULT_ENUM(prefix, WAIT_ABORTED, 1054),                        \
        RESULT_ENUM(prefix, SET_BLOCK_ACK_WINDOW_FAIL, 1055), RESULT_ENUM(prefix, DELAY_TOO_SHORT, 1056),              \
        RESULT_ENUM(prefix, INVALID_INTERFACE, 1057), RESULT_ENUM(prefix, WEP_KEYLEN_BAD, 1058),                       \
        RESULT_ENUM(prefix, HANDLER_ALREADY_REGISTERED, 1059), RESULT_ENUM(prefix, AP_ALREADY_UP, 1060),               \
        RESULT_ENUM(prefix, EAPOL_KEY_PACKET_M1_TIMEOUT, 1061),                                                        \
        RESULT_ENUM(prefix, EAPOL_KEY_PACKET_M3_TIMEOUT, 1062),                                                        \
        RESULT_ENUM(prefix, EAPOL_KEY_PACKET_G1_TIMEOUT, 1063), RESULT_ENUM(prefix, EAPOL_KEY_FAILURE, 1064),          \
        RESULT_ENUM(prefix, MALLOC_FAILURE, 1065), RESULT_ENUM(prefix, ACCESS_POINT_NOT_FOUND, 1066),                  \
        RESULT_ENUM(prefix, RTOS_ERROR, 1067), RESULT_ENUM(prefix, CLM_BLOB_DLOAD_ERROR, 1068),

#define WLAN_RESULT_LIST(prefix)                                                                                       \
    RESULT_ENUM(prefix, ERROR, 2001), RESULT_ENUM(prefix, BADARG, 2002), RESULT_ENUM(prefix, BADOPTION, 2003),         \
        RESULT_ENUM(prefix, NOTUP, 2004), RESULT_ENUM(prefix, NOTDOWN, 2005), RESULT_ENUM(prefix, NOTAP, 2006),        \
        RESULT_ENUM(prefix, NOTSTA, 2007), RESULT_ENUM(prefix, BADKEYIDX, 2008), RESULT_ENUM(prefix, RADIOOFF, 2009),  \
        RESULT_ENUM(prefix, NOTBANDLOCKED, 2010), RESULT_ENUM(prefix, NOCLK, 2011),                                    \
        RESULT_ENUM(prefix, BADRATESET, 2012), RESULT_ENUM(prefix, BADBAND, 2013),                                     \
        RESULT_ENUM(prefix, BUFTOOSHORT, 2014), RESULT_ENUM(prefix, BUFTOOLONG, 2015),                                 \
        RESULT_ENUM(prefix, BUSY, 2016), RESULT_ENUM(prefix, NOTASSOCIATED, 2017),                                     \
        RESULT_ENUM(prefix, BADSSIDLEN, 2018), RESULT_ENUM(prefix, OUTOFRANGECHAN, 2019),                              \
        RESULT_ENUM(prefix, BADCHAN, 2020), RESULT_ENUM(prefix, BADADDR, 2021), RESULT_ENUM(prefix, NORESOURCE, 2022), \
        RESULT_ENUM(prefix, UNSUPPORTED, 2023), RESULT_ENUM(prefix, BADLEN, 2024),                                     \
        RESULT_ENUM(prefix, NOTREADY, 2025), RESULT_ENUM(prefix, EPERM, 2026), RESULT_ENUM(prefix, NOMEM, 2027),       \
        RESULT_ENUM(prefix, ASSOCIATED, 2028), RESULT_ENUM(prefix, RANGE, 2029), RESULT_ENUM(prefix, NOTFOUND, 2030),  \
        RESULT_ENUM(prefix, WME_NOT_ENABLED, 2031), RESULT_ENUM(prefix, TSPEC_NOTFOUND, 2032),                         \
        RESULT_ENUM(prefix, ACM_NOTSUPPORTED, 2033), RESULT_ENUM(prefix, NOT_WME_ASSOCIATION, 2034),                   \
        RESULT_ENUM(prefix, SDIO_ERROR, 2035), RESULT_ENUM(prefix, WLAN_DOWN, 2036),                                   \
        RESULT_ENUM(prefix, BAD_VERSION, 2037), RESULT_ENUM(prefix, TXFAIL, 2038), RESULT_ENUM(prefix, RXFAIL, 2039),  \
        RESULT_ENUM(prefix, NODEVICE, 2040), RESULT_ENUM(prefix, UNFINISHED, 2041),                                    \
        RESULT_ENUM(prefix, NONRESIDENT, 2042), RESULT_ENUM(prefix, DISABLED, 2043),

typedef enum
{
    WICED_FALSE = 0,
    WICED_TRUE  = 1
} wiced_bool_t;

typedef enum
{
    WWD_RESULT_LIST(WWD_) WLAN_RESULT_LIST(WWD_WLAN_)
} wwd_result_t;

#define MK_CNTRY(a, b, rev) (((unsigned char)(a)) + (((unsigned char)(b)) << 8) + (((unsigned short)(rev)) << 16))

typedef enum
{
    WICED_COUNTRY_UNITED_STATES = MK_CNTRY('U', 'S', 0),
    WICED_COUNTRY_WORLD_WIDE_XX = MK_CNTRY('X', 'X', 0)
} wiced_country_code_t;

#define WEP_ENABLED    0x0001
#define TKIP_ENABLED   0x0002
#define AES_ENABLED    0x0004
#define SHARED_ENABLED 0x00008000
#define WPA_SECURITY   0x00200000
#define WPA2_SECURITY  0x00400000

typedef enum
{
    WICED_SECURITY_OPEN         = 0,
    WICED_SECURITY_WEP_SHARED   = (WEP_ENABLED | SHARED_ENABLED),
    WICED_SECURITY_WPA_TKIP_PSK = (WPA_SECURITY | TKIP_ENABLED),
    WICED_SECURITY_WPA2_AES_PSK = (WPA2_SECURITY | AES_ENABLED),
    WICED_SECURITY_FORCE_32_BIT = 0x7fffffff
} wiced_security_t;

typedef enum
{
    WWD_STA_INTERFACE      = 0,
    WWD_AP_INTERFACE       = 1,
    WWD_P2P_INTERFACE      = 2,
    WWD_ETHERNET_INTERFACE = 3,

    WWD_INTERFACE_MAX,
    WWD_INTERFACE_FORCE_32_BIT = 0x7fffffff
} wwd_interface_t;

// wwd_network_constants.h
#define MAX_BUS_HEADER_LENGTH                        (12)
#define MAX_SDPCM_HEADER_LENGTH                      (18)
#define WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX (MAX_BUS_HEADER_LENGTH + MAX_SDPCM_HEADER_LENGTH)
#define WICED_LINK_TAIL_AFTER_ETHERNET_FRAME         (0)
#define WICED_ETHERNET_SIZE                          (14)
#define WICED_PAYLOAD_MTU                            (1500)
#define WICED_PHYSICAL_HEADER                        (WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX + WICED_ETHERNET_SIZE)
#define WICED_PHYSICAL_TRAILER                       (WICED_LINK_TAIL_AFTER_ETHERNET_FRAME)
#define WICED_LINK_MTU                               (WICED_PAYLOAD_MTU + WICED_PHYSICAL_HEADER + WICED_PHYSICAL_TRAILER)

// wwd_structures.h
typedef struct
{
    uint8_t length;
    uint8_t value[32];
} wiced_ssid_t;

typedef struct
{
    uint8_t octet[6]; /**< Unique 6-byte MAC address */
} wiced_mac_t;

// wwd_management.h
extern wwd_result_t wwd_management_wifi_on(wiced_country_code_t country);

// wwd_network.h
extern VOID wiced_sta_netx_duo_driver_entry(NX_IP_DRIVER* driver);

// wwd_buffer_interface.h
extern wwd_result_t wwd_buffer_init(void* native_arg);

// wwd_wifi.h
extern wwd_result_t wwd_wifi_join(const wiced_ssid_t* ssid,
    wiced_security_t auth_type,
    const uint8_t* security_key,
    uint8_t key_length,
    TX_SEMAPHORE* semaphore,
    wwd_interface_t interface);

extern wwd_result_t wwd_wifi_leave(wwd_interface_t interface);
extern wwd_result_t wwd_wifi_join_halt(wiced_bool_t halt);
extern wwd_result_t wwd_wifi_get_mac_address(wiced_mac_t* mac, wwd_interface_t interface);
extern wwd_result_t wwd_wifi_is_ready_to_transceive(wwd_interface_t interface);

#endif
