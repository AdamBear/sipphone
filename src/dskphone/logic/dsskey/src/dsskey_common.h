#ifndef __DSSKEY_COMMON_H__
#define __DSSKEY_COMMON_H__
#include "ETLLib.hpp"
#include "interfacedefine.h"
#include <path_define.h>

#ifdef _WIN32
//这个地方定义__KERNEL__是为了在WIN32平台上不编译exp_drv.h中定义的扩展台函数
#define __KERNEL__ 0
#ifndef EFAULT
#   define EFAULT -1
#endif
#endif
#include "exp/src/expdrive/deviceplugin/modexpscan.h"

// for nzc test
//#define _LANG(str)  str

#define DSSKEY_XML_FILE_PATH TEMP_PATH "dsskey/"
#define LINEKEY_TMP_FILE DSSKEY_XML_FILE_PATH "linekey.xml"
#define MEMKEY_TMP_FILE  DSSKEY_XML_FILE_PATH "memkey.xml"
#define PROGRAME_TMP_FILE    DSSKEY_XML_FILE_PATH "programkey.xml"
#define NOTIFY_TMP_FILE DSSKEY_XML_FILE_PATH "notify.xml"
#define EXPKEY_TMP_FILE  DSSKEY_XML_FILE_PATH "expkey"


#define FILE_EXPKEY             ""
#define NODE_DSSKEY             "dsskey"            // The DssKey node(root node) name.
#define NODE_PROGRAMEKEY        "programkey"            // The DssKey node(root node) name.
#define NODE_CONFIG             "configuration"             // The config node name.
#define ATTR_DEV                "dev"               // The dev attribute name.
#define NODE_ITEM               "item"              // The item node name.
#define NODE_ITEM_DATA          "itemdata"          // The item data node name.
#define ATTR_TYPE               "type"              // The type attribute name.
#define ATTR_LINE               "line"              // The line attribute name.
#define ATTR_LABEL              "label"             // The label attribute name.
#define ATTR_VALUE              "value"             // The value attribute name.
#define ATTR_LOCK               "lock"              // The lock attribute name.
#define ATTR_EXTENSION          "extension"         // The Extension attribute name.
//#define ATTR_SELECTED_INDEX     "selectedindex"     // The SelectedIndex attribute name.
//#define ATTR_PICKUP_VALUE       "pickupvalue"       // the name of pickupvalue attribute
#define ATTR_SHORT_LABEL        "shortlabel"        // The Short Label.

#define NETWORK_AVAILABLE       "Net available"     // label for url
#define NETWORK_UNAVAILABLE     "Net unavailable"   // label for url
#define NETWORK_UNKNOW          "Net unknow"        // label for url

//获取BLF后5为的值，Idle,Ring,Talk时增加2种情况的处理，
#define GetBLFStatusNoMode(i)   ((i) & 0x1f)
#define GetBLFMode(i)           ((i) & 0x60)

//获取Exp38对应的index  (bit0~bit5表示第几个按键，bit6~bit8表示第几个扩展台)
#define GetEXPIndex(i)      (((i) >> 6) & 0xf)
//获取Exp38Key对应的key index
#define GetEXPKeyIndex(i)   ((i) & 0x003f)
// 获取Exp按键对应的EXP类型
#define GetEXPModuleType(i) (((i) >> 24))

#define OffsetIndex(index)  ((index) + FIRST_EXPKEY_INDEX)
#define ReMoveOffset(index) ((index) - FIRST_EXPKEY_INDEX)

#ifdef _T3X
#define MEMORY_KEY_NUM 10
#endif

#define EXPKeyCol(i)   ((i) & 0x000000ff)
#define EXPKeyRow(i)   ((i >> 8) & 0x0000ff)

#define  LINENUM_PERPAGE 5

#endif // __DSSKEY_COMMON_H__
