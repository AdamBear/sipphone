#pragma once

#include "basetalklistener.h"

class CSipMsgFliter
    : public CBaseTalkListener
{
public:
    CSipMsgFliter();

private:
    /* ETLmsg消息回调函数 */
    virtual bool OnMessage(msgObject& msg);
};

