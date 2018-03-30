#include "../include/xsibaseprocess.h"
#include "../include/xsiaction.h"
#include "../include/cbasehelper.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "taskaction/modtaskaction.h"
#include "devicelib/networkserver.h"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "dsklog/log.h"

void XsiBaseProcesser::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    if (pTaskAction->GetReqType() != TA_REQUEST_NETWORK_FILE)
    {
        return;
    }

    // 返回结果的类型为请求网络文件.
    XsiAction * pXsiAction =
        static_cast<XsiAction *>(pTaskAction);
    if (pXsiAction == NULL)
    {
        return;
    }

    XsiTaskMap::iterator iter = m_TaskMap.find(pXsiAction);

    if (iter == m_TaskMap.end())
    {
        remove(pXsiAction->GetFilePath().c_str());
        return;
    }
    else
    {
        m_TaskMap.erase(iter);
    }

    CBaseHelper::ProcessCallback(pTaskAction, pXsiAction->GetLineId());

    bool suc = false;

    if (pXsiAction->IsOperationSucceed() && !ProcessByType(pXsiAction))
    {
        pXsiAction->SetTaskActionErrorType(TA_UNKOWN_CONTENT_ERROR);
    }

    if (!pXsiAction->IsOperationSucceed())
    {
        XSI_INFO("XSI Req Fail (ActionType: %d, ErrCode: %d)", pXsiAction->GetXsiType(),
                 (int)pTaskAction->GetTaskActionErrorType());
        FailProcess(pXsiAction);
    }
    //有些请求存在多个任务 ，当多个任务同时返回再发完成消息
    else if (m_TaskMap.size() == 0)
    {
        // 发送消息.
        etl_NotifyApp(
            false,
            XSI_MESSAGE_REQ_FINISH,
            pXsiAction->GetXsiType(),
            pXsiAction->GetLineId());
    }

    //删除内存中的临时文件
    remove(pXsiAction->GetFilePath().c_str());
}

XsiAction * XsiBaseProcesser::ExeXsiAction(int nLineId, eXsiActionType eXsiType,
        const yl::string & strExtUrl/* = ""*/, const yl::string & strFile /* = */)
{
    if (configParser_GetConfigInt(kszBroadsoftActive) != 1
            || configParser_GetConfigInt(kszXSISwitch) == 0)
    {
        return NULL;
    }

    if (!netIsNetworkReady())
    {
        SendFailNotif(eXsiType, TA_HAS_NO_NETWORK_REPLAY, nLineId);

        return NULL;
    }
    else if (CBaseHelper::GetXSIStatus(nLineId) != XSI_SUCCESS)
    {
        SendFailNotif(eXsiType, TA_HTTP_DOWNLOAD_UNAUTHORIZED, nLineId);

        return NULL;
    }

    XsiAction * pAction = new XsiAction(nLineId, eXsiType, strExtUrl, strFile);

    if (pAction != NULL)
    {
        pAction->SetSync(false);
        pAction->SetCallback(this);

        bool suc = TaskAction_ExecAction(pAction);

        if (suc)
        {
            m_TaskMap[pAction] = 1;
        }

        return pAction;
    }

    return NULL;
}

XsiBaseProcesser::XsiBaseProcesser()
{
    m_nLineID = -1;
}

XsiBaseProcesser::~XsiBaseProcesser()
{
}

//取消所有的命令请求
void XsiBaseProcesser::CancelAllRequest()
{
    m_TaskMap.clear();
}

bool XsiBaseProcesser::SetLineID(int nLineID)
{
    if (nLineID < 0)
    {
        return false;
    }

    m_nLineID = nLineID;
    return true;
}

void XsiBaseProcesser::GetFailCodeString(yl::string & strFail, TaskActionErrorType eErrCode)
{
    switch (eErrCode)
    {
    case TA_HTTP_DOWNLOAD_UNAUTHORIZED:

        strFail = "auth error";
        break;
    case TA_HTTP_ERR400:

        strFail = "Bad Request";
        break;
    case TA_HTTP_ERR403:

        strFail = "Forbidden";
        break;
    case TA_HTTP_ERR408:

        strFail = "Time out";
        break;
    case TA_HTTP_ERR500:

        strFail = "server error";
        break;
    case TA_UNKNOW_NETWORK_ERROR:
        strFail = "network unavailable";
        break;
    default:
        strFail = "Unknown";
        break;
    }
}

void XsiBaseProcesser::FailProcess(XsiAction * pAction)
{
    m_xsiExtenInfo.Reset();

    if (pAction != NULL)
    {
        m_xsiExtenInfo.m_nEntenParm = pAction->GetExtraParam();

        SendFailNotif(pAction->GetXsiType(), pAction->GetTaskActionErrorType(), pAction->GetLineId());
    }
}

void XsiBaseProcesser::SendFailNotif(eXsiActionType eType, TaskActionErrorType nError, int nLineId)
{
    XSI_INFO("XsiBaseProcesser::SendFailNotif eType[%d],nError[%d]", eType, nError);
    m_xsiExtenInfo.m_nError = nError;

    GetFailCodeString(m_xsiExtenInfo.m_strErrInfo, nError);
    // 发送消息.
    etl_NotifyAppEx(
        false,
        XSI_MESSAGE_REQ_FAIL,
        eType,
        nLineId,
        sizeof(SXsiExtenInfo),
        &m_xsiExtenInfo);
}


