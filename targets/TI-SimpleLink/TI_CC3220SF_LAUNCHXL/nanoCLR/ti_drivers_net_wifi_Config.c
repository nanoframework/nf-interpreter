
/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>

/*
 *  =============================== SimpleLink Wifi ===============================
 */

/*
 *  ======== SimpleLinkWifi_config ========
 */
const SlWifiCC32XXConfig_t SimpleLinkWifiCC32XX_config = {

        .Mode = ROLE_STA,
		.Ipv4Mode = SL_NETCFG_IPV4_STA_ADDR_MODE,
		.ConnectionPolicy = SL_WLAN_CONNECTION_POLICY(1,0,0,1),
        .PMPolicy = SL_WLAN_NORMAL_POLICY,
		.MaxSleepTimeMS = 0,
		.ScanPolicy = SL_WLAN_SCAN_POLICY(0,0),
		.ScanIntervalInSeconds = 0,
		.Ipv4Config = SL_NETCFG_ADDR_DHCP,		
		.Ipv4 = 0,
		.IpMask = 0,
		.IpGateway = 0,
		.IpDnsServer = 0,
		.ProvisioningStop = 0,
		.DeleteAllProfile = 0
};
