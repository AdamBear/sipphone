#ifndef __VQRTCP_LISTENER_H__
#define __VQRTCP_LISTENER_H__

#include "basetalklistener.h"

#ifdef IF_FEATURE_RTCPXR

/* rtcp 业务 */

class CRTCPListener : public CBaseTalkListener
{
public:
    CRTCPListener();

    DECL_SIGNAL_HANDLER(OnFocusChange);

    DECL_SIGNAL_HANDLER(OnStartTalk);

    DECL_SIGNAL_HANDLER(OnStopTalk);

    DECL_SIGNAL_HANDLER(OnCallIDChange);

    DECL_SIGNAL_HANDLER(OnHeldChange);
};

#endif
#endif
