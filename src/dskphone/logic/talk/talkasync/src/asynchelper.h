#ifndef __TALK_DL_ASYNC_BASE_HEADER_H__
#define __TALK_DL_ASYNC_BASE_HEADER_H__

#include "taskaction/modtaskaction.h"
#include "taskaction/taskaction.h"
#include "taskaction/taskactioncallbackbase.h"
#include "singletonclassdefine.h"
#include "talkasync_data.h"
#include "ylhashmap.h"


class CAsyncHelper : public NS_TA::CTaskActionCallbackBase
{
    DECLARE_SINGLETON_CLASS(CAsyncHelper)

public:
    typedef YLHashMap<NS_TA::CTaskAction*, int> ASYNC_MAP_ACTION;

public:
    // 回调处理函数(override from CTaskActionCallbackBase)
    virtual void OnTaskActionCallback(CTaskAction* pTaskAction);
    // 设置Action对应的CallId
    void SetCallID(CTaskAction* pTaskAction, int iCallID);

    bool OnCallback(int iCallID, CTaskAction* pTaskAction);

    bool OnRingDownloadCallback(int iCallID, CTaskAction* pTaskAction);

    bool OnImageDownloadCallback(int iCallID, CTaskAction* pTaskAction);

    bool OnContractMatchCallback(int iCallID, CTaskAction* pTaskAction);

private:
    // callid和对应的TaskAction
    ASYNC_MAP_ACTION      m_mapAction;
};

// 获取全局唯一的对象
#define _ASyncHelper GET_SINGLETON_INSTANCE(CAsyncHelper)

#endif
