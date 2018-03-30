#ifndef _CALL_PARK_COMMON_H_
#define _CALL_PARK_COMMON_H_

#include <ylstring.h>
#include "innermsgdefine.h"
#include "talk/talklogic/include/uiandlogic_base.h"

// Notify界面需要的数据
struct DataParkNotify : public DataLogic2UIBase
{
    DataParkNotify()
        : strNumber("")
        , strDiaplayName("")
    {

    }

    void Reset()
    {
        strNumber.clear();
        strDiaplayName.clear();
    }

    // 被Park通话的号码
    yl::string strNumber;

    // 被Park通话的联系DisplayName
    yl::string strDiaplayName;
};

struct DataParkNotifyEx
{
#define MAX_CALL_PARK_NAME_LEN 128
    DataParkNotifyEx()
    {
        Reset();
    }

    void Reset()
    {
        memset(szNumber, 0, MAX_CALL_PARK_NAME_LEN);
        memset(szDisplayName, 0, MAX_CALL_PARK_NAME_LEN);
    }

    char szNumber[MAX_CALL_PARK_NAME_LEN];
    char szDisplayName[MAX_CALL_PARK_NAME_LEN];
};

enum CALL_PARK_MSG_DEFINE
{
    CALL_PARK_MSG_BEGIN = INNER_MSG_CALL_PARK_MESSAGE_BEGIN,

    // 有电话park到本机上时的发送的消息
    CALL_PARK_NOTIFY, // wParam:line id lParam:0/Enable 1/DisEnable extra: DataParkNotify

    CALL_PARK_MSG_END = INNER_MSG_CALL_PARK_MESSAGE_END,
};

#endif // _CALL_PARK_COMMON_H_
