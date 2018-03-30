#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef _WIFITESTDATADEFINE_H_
#define _WIFITESTDATADEFINE_H_

#include <service_system.h>
#include "extended/wifimessagedefine.h"
#include "ylstl/ylvector.h"
#include "ylstl/yllist.h"

enum AdapterStatus
{
    ENABLE = 0,
    DISABLE,
};

typedef sys_net_link_status_t net_link_status;
// nw_id作为两个链表的纽带
typedef YLList<sys_wifi_nw_stat_t> list_nw_stat;
typedef YLList<sys_wifi_ap_t> list_ap;

enum GET_RAND
{
    LINK_SPEED = 1,
    SIGNAL,
    MAC,
    SUCCESS_RATE,
    RAND_AP_ID,
};

#endif
#endif //IF_WIFI_SIMULATION_DRIVER
