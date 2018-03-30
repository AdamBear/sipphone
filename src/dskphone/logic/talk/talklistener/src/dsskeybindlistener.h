#ifndef __DSSKEY_BIND_LISTENER_H__
#define __DSSKEY_BIND_LISTENER_H__

#include "basetalklistener.h"

/* dsskey业务：mca、dsskeybind、dsskeycallout等 */

class CDsskeyBindListener : public CBaseTalkListener
{
public:
    CDsskeyBindListener();

protected:

    DECL_SIGNAL_HANDLER(OnHideChange);

    DECL_SIGNAL_HANDLER(OnFocusChange);

    DECL_SIGNAL_HANDLER(OnRefresh);

    DECL_SIGNAL_HANDLER(OnSessionExit);

    DECL_SIGNAL_HANDLER(OnRoutineChange);

    DECL_SIGNAL_HANDLER(OnHostChange);

    DECL_SIGNAL_HANDLER(OnAccountChange);

    DECL_SIGNAL_HANDLER(OnDialCallout);

    DECL_SIGNAL_HANDLER(OnCallOut);

    DECL_SIGNAL_HANDLER(OnCallIncome);

    DECL_SIGNAL_HANDLER(TryCreateSession);

};

#endif
