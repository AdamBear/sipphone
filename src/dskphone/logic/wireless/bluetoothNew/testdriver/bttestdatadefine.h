#ifdef IF_BT_TEST_LOGIC
#ifndef __BTTESTDATADEFINE_H__
#define __BTTESTDATADEFINE_H__

#ifdef WIN32
#include "../../../../../3rdlib/include/bluetooth/btkit_api.h"
#else
#include "bluetooth/btkit_api.h"
#endif // WIN32

#include "ylstl/yllist.h"
#include "ylstl/ylvector.h"

#define  BTTEST_MAX_DEVICE 100
#define  BTTEST_SCAN_TIMER 500

typedef struct
{
    btkit_dev_instance_t dev_hnd;
    btkit_dev_type_t dev_type;
    btkit_dev_t dev_property;
    WIRELESS_DEVICE_STATE state;
    bool bPower;
} bttest_dev_t;

typedef struct
{
    btkit_dev_instance_t dev_hnd;
    btkit_headset_property_t headset_property;
} bttest_headset_t;

typedef struct
{
    btkit_dev_instance_t dev_hnd;
    btkit_mb_property_t  mb_property;
} bttest_mb_t;

typedef struct
{
    btkit_dev_instance_t   dev_hnd;
    btkit_mb_call_status_t callstatus;
} bttest_callstatus_t;

typedef YLVector<bttest_dev_t> list_dev;
typedef YLList<bttest_headset_t> list_headset;
typedef YLList<bttest_mb_t> list_mb;
typedef YLList<bttest_callstatus_t>list_callstatus;
#endif //__BTTESTDATADEFINE_H__





#endif // IF_BT_TEST_LOGIC
