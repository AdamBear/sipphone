#ifndef _DSSKEY_URL_ACTION_HEADER_
#define _DSSKEY_URL_ACTION_HEADER_
#include "../include/idsskeyimplaction.h"
#include <taskaction/taskactioncallbackbase.h>
#include <taskaction/taskaction.h>
#include <ylhashmap.h>
using NS_TA::CTaskAction;
using NS_TA::CTaskActionCallbackBase;

class CDSSkeyURLAction : public IDsskeyImplAction, CTaskActionCallbackBase
{
public:
    CDSSkeyURLAction(void);
    ~CDSSkeyURLAction(void);

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);

    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);
    bool OnTimer(int dsskeyDataID, UINT timerID, const DssKeyLogicData & logicData, void * pExtraData);
};
#endif
