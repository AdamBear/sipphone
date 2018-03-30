#ifndef __DIALPLAN_LISTENER_H__
#define __DIALPLAN_LISTENER_H__

#include "basetalklistener.h"

/* dialplan业务：dialnow、autodial、autoquit、area、replace、digitmap等 */

class CDialPlanListener : public CBaseTalkListener
{
public:
    CDialPlanListener();

protected:
    //账号改变事件处理
    DECL_SIGNAL_HANDLER(OnAccountChange);

    //文本改变处理
    DECL_SIGNAL_HANDLER(OnTextChange);

    //创建话路
    DECL_SIGNAL_HANDLER(OnCreateEvent);

    //按键事件处理
    DECL_SIGNAL_HANDLER(OnUIAction);

    DECL_SIGNAL_HANDLER(OnDialCallout);

    DECL_SIGNAL_HANDLER(OnSessionDestory);

};

#endif
