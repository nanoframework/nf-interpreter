
#include "lwip/netif.h"

static struct netif fsl_netif0;


struct netif * nf_getNetif() {
    return &fsl_netif0;
}