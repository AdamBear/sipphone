#pragma once

#include "talk/talklistener/include/basetalklistener.h"

class CTalkAdapterListener
    : public CBaseTalkListener
{
public:
    CTalkAdapterListener();

private:
    DECL_SIGNAL_HANDLER(TryCreateSession);

    DECL_SIGNAL_HANDLER(OnRoutineChange);

    DECL_SIGNAL_HANDLER(OnSessionHide);

    DECL_SIGNAL_HANDLER(OnSessionPreDelete);

    DECL_SIGNAL_HANDLER(OnSessionAftDelete);

    DECL_SIGNAL_HANDLER(BeforeAnswerCall);

    DECL_SIGNAL_HANDLER(OnAnswerCall);

    DECL_SIGNAL_HANDLER(OnSubSessionActive);

    DECL_SIGNAL_HANDLER(OnAddSessionNetConf);

    DECL_SIGNAL_HANDLER(OnPageReceived);

    DECL_SIGNAL_HANDLER(OnSessionCreate);

    DECL_SIGNAL_HANDLER(BeforeCallout);

    DECL_SIGNAL_HANDLER(OnHoldChange);

    DECL_SIGNAL_HANDLER(OnHostChange);

    DECL_SIGNAL_HANDLER(ResetFocusSession);

    DECL_SIGNAL_HANDLER(OnStartTalk);

private:
    typedef YLVector<int>   VEC_HOST_SESSION;

    void Push(int sub, int host);

private:

    VEC_HOST_SESSION        m_vec;
};

