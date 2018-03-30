#ifndef _EXP_APP_H
#define _EXP_APP_H

#include <dskthread/mkthread.h>
#include "etlmsghandler/modetlmsghandler.h"
#include "singletonclassdefine.h"
#include "exp/src/expdrawer/expbase/expdefine.h"
#include "exp_struct.h"
#include "service_config.h"

#if (not defined _USE_SKIA) &&(not defined _XWIN)
#include "qdskapplication.h"
#endif

#ifdef _DEBUGMEM
#include "3rdlib/memory/memory.h"
#endif

LRESULT OnEXPMsg(msgObject & objMessage);

class CEXPApp
#if (defined _USE_SKIA) || (defined _XWIN)
    : public CMKThread
#else
    : public QDSKApplication
#endif
{
#if (not defined _USE_SKIA) &&(not defined _XWIN)
    Q_OBJECT
#endif

    BEGIN_MSG_MAP(CEXPApp)
    TRY_MSG_ON_METHOD(TM_TIMER, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_UPDATE_KEY, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_BG_CHANGE, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_BG_REFRESH, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_BACKLIGHT, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_RESET_LIGHT, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_EDITING_TEXT, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_COUNT_CHANGE, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_PROCESS_RELOAD, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_SCREENSAVER, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_FILL_COLOR, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_FILL_PICTURE, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_DRAW_TEXT, OnEXPMsg)
    TRY_MSG_ON_METHOD(EXP_MESSAGE_SHOW_LOGO, OnEXPMsg)
    TRY_MSG_ON_METHOD(CONFIG_MSG_BCAST_CHANGED, OnEXPMsg)
    CHAIN_MSG_TO_BASE(CMKThread)
    END_MSG_MAP()



public:
#if (defined _USE_SKIA) || (defined _XWIN)
    explicit CEXPApp();
#else
    explicit CEXPApp(int & argc, char ** argv);
#endif

public:
    virtual ~CEXPApp(void);
#ifdef _XWIN
public:
    static CEXPApp & GetInstance();
#endif // _XWIN

#if (defined _USE_SKIA) || (defined _XWIN)
    virtual BOOL AfterThreadLaunched();
    virtual void BeforeThreadShutdown();
#else
    virtual void Init();
    virtual void Uninit();
#endif

};
#ifdef _XWIN
#define _EXPLcdDrawThreadColor GET_SINGLETON_INSTANCE(CEXPApp)
#endif // _XWIN


#endif //_EXP_APP_H

