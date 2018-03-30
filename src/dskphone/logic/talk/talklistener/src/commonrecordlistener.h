#ifndef __SIP_URL_RECORD_LISTENER_H__
#define __SIP_URL_RECORD_LISTENER_H__

#include "basetalklistener.h"
#include "taskaction/modtaskaction.h"
#include "taskaction/taskaction.h"
#include "taskaction/taskactioncallbackbase.h"
#include "ylhashmap.h"

/* sip record、url record 业务 */

class CCommonRecordListener
    : public CBaseTalkListener, public NS_TA::CTaskActionCallbackBase
{
public:
    CCommonRecordListener();

    typedef YLHashMap<NS_TA::CTaskAction*, int> ASYNC_MAP_ACTION;

private:
    virtual bool OnMessage(msgObject& msg);

    // 回调处理函数(override from CTaskActionCallbackBase)
    virtual void OnTaskActionCallback(CTaskAction* pTaskAction);

    CTaskAction* CreateURLRecord(yl::string& strFullUrl);

    /* 录音状态刷新 */
    bool SetRecord(int iDsskey);

    void UpdateRecordStatus(const CSessionStateProxy& proxy, CRecordASyncProxy& record);

private:
    ASYNC_MAP_ACTION      m_mapAction;

private:

    DECL_SIGNAL_HANDLER(OnUIAction);

    DECL_SIGNAL_HANDLER(OnGetDataToUI);

    DECL_SIGNAL_HANDLER(UpdateRecord);
};

#endif
