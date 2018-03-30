#ifndef __AUTOANSWER_LISTENER_H__
#define __AUTOANSWER_LISTENER_H__

#include "basetalklistener.h"

class CAutoAnswerListener : public CBaseTalkListener
{
public:
    CAutoAnswerListener();

    bool OnMessage(msgObject& msg);
protected:
    DECL_SIGNAL_HANDLER(OnSessionDelete);

    //处理自动接听 与拒接
    DECL_SIGNAL_HANDLER(OnDealAutoAnswer);

    //处理相应TONE_ACTIVE相应事件
    DECL_SIGNAL_HANDLER(OnDealToneActive);
};

#endif
