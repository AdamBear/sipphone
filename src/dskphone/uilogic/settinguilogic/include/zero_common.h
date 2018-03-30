#ifndef _ZERO_COMMON_H_
#define _ZERO_COMMON_H_
//
#include "ylstring.h"

#define  ZERO_TOUCH_DEBUG       0

// 页面的宏定义
#define ZERO_MAIN_PAGE          "zero_main"
#define ZERO_NETWORK_PAGE       "OpenPage(zero_network_list.xml)"
#define ZERO_NETWORK_CHECK_PAGE "zero_network_check"
#define ZERO_NETWORK_FAIL_PAGE  "zero_networkfail"
#define ZERO_AUTOP_PAGE         "OpenSubPage(zero_auto_provision)"
#define ZERO_UNAUTHORIZED_PAGE  "zero_unauthorized"
#define ZERO_UPDATING_PAGE      "zero_updating"
#define ZERO_UPDATEFAIL_PAGE    "zero_updatefail"
#define ZERO_UPDATEFINISH_PAGE  "zero_updatefinish"
#define ZERO_REBOOTING_PAGE     "zero_rebooting"
#define ZERO_STATUS_PAGE        "OpenSubPage(zero_status_list.xml)"
#define ZERO_AUTOP_PAGE_DIRECT  "OpenSubPage(zero_auto_provision_direct)"

// 重定向
#define ZERO_REDIRECTOR_NOTE    "zero_redirector_note"
#define ZERO_REDIRECTOR_CONFIG  "zero_redirector_config"
#define ZERO_REDIRECTOR_FAILED  "zero_redirector_failed"
#define ZERO_REDIRECTOR_FINISH  "zero_redirector_finish"

// softkey对应操作的宏定义
#define ZERO_NEXT               "NextPage"//"next"
#define ZERO_STATUS             "status"
#define ZERO_BACK               "back"

// 进入某个Zero界面
typedef bool (*Fun_EnterZeroPage)(const yl::string & strPageAction);

#endif // _ZERO_COMMON_H_
