#ifndef __XSI_BASEPROCESS_H__
#define __XSI_BASEPROCESS_H__

#include <ETLLib.hpp>
#include <ylhashmap.h>
#include "xsicommon.h"
#include "xsiaction.h"
#include "taskaction/taskactioncallbackbase.h"

using namespace NS_TA;

typedef YLHashMap<void *, int> XsiTaskMap;

class XsiBaseProcesser : public CTaskActionCallbackBase
{
public:
    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    //取消所有的命令请求
    void CancelAllRequest();

    //static LRESULT OnNetworkException(msgObject& msg);

    //设置哪个账号
    bool SetLineID(int nLineID);

    int GetLineID()
    {
        return m_nLineID;
    }

    static void GetFailCodeString(yl::string & strFail, TaskActionErrorType eErrCode);

public:

    XsiBaseProcesser();
    virtual ~XsiBaseProcesser();

protected:

    //操作成功处理
    virtual bool ProcessByType(XsiAction * pAction)
    {
        return true;
    }

    //操作失败处理
    virtual void FailProcess(XsiAction * pAction);

    XsiAction * ExeXsiAction(int nLineId, eXsiActionType eXsiType, const yl::string & strExtUrl = "",
                             const yl::string & strFile = "");

    void SendFailNotif(eXsiActionType eType, TaskActionErrorType nError, int nLineId);

protected:
    //账号ID
    int m_nLineID;

    SXsiExtenInfo m_xsiExtenInfo;//错误信息

    XsiTaskMap m_TaskMap;
};

#endif

