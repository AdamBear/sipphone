#include "callrecordingmanager.h"
#include "account/include/accountmessagedefine.h"
#include "account/include/account_datadefine.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/modxsicommon.h"

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
static LRESULT OnProcessMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
    case ACCOUNT_STATUS_CHANGED:
        {
            g_objCallRecording.ProcesserMSG(objMessage);
        }
        break;
    default:
        break;
    }
    return 1;
}

CallRecordingProcesser & CallRecordingProcesser::GetInstance()
{
    static CallRecordingProcesser s_Inst;

    return s_Inst;
}

CallRecordingProcesser::CallRecordingProcesser()
{
    m_bConfRecordSuc = false;

    XsiAction::InsertMap(XSI_CALL_RECORD_START, "calls/");
    XsiAction::InsertMap(XSI_CALL_RECORD_STOP, "calls/");
    XsiAction::InsertMap(XSI_CALL_RECORD_PAUSE, "calls/");
    XsiAction::InsertMap(XSI_CALL_RECORD_RESUME, "calls/");

    XsiAction::InsertMap(XSI_CALL_RECORD_MODE_GET, "services/callrecording");
    XsiAction::InsertMap(XSI_CALL_RECORD_MODE_PUT, "services/callrecording");

    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, OnProcessMsg);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnProcessMsg);
}

CallRecordingProcesser::~CallRecordingProcesser()
{
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, OnProcessMsg);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnProcessMsg);
}

bool CallRecordingProcesser::CallRecord(int iCallId, const yl::string & strXsiCallId,
                                        int iAccountId)
{
    if (!SetLineID(iAccountId))
    {
        return false;
    }

    m_xsiExtenInfo.m_nEntenParm = iCallId;

    yl::string strExtenUrl = strXsiCallId;

    strExtenUrl.append("/Record");


    XsiAction * pAction = ExeXsiAction(iAccountId, XSI_CALL_RECORD_START, strExtenUrl);

    if (pAction != NULL)
    {
        pAction->SetExtraParam(iCallId);
        return true;
    }

    return false;
}

bool CallRecordingProcesser::StopRecord(int iCallId, const yl::string & strXsiCallId,
                                        int iAccountId)
{
    if (!SetLineID(iAccountId))
    {
        return false;
    }

    m_xsiExtenInfo.m_nEntenParm = iCallId;

    yl::string strExtenUrl = strXsiCallId;

    strExtenUrl.append("/StopRecording");


    XsiAction * pAction = ExeXsiAction(iAccountId, XSI_CALL_RECORD_STOP, strExtenUrl);

    if (pAction != NULL)
    {
        pAction->SetExtraParam(iCallId);
        return true;
    }

    return false;
}

bool CallRecordingProcesser::PauseRecord(int iCallId, const yl::string & strXsiCallId,
        int iAccountId)
{
    if (!SetLineID(iAccountId))
    {
        return false;
    }

    m_xsiExtenInfo.m_nEntenParm = iCallId;

    yl::string strExtenUrl = strXsiCallId;

    strExtenUrl.append("/pauseRecording");


    XsiAction * pAction = ExeXsiAction(iAccountId, XSI_CALL_RECORD_PAUSE, strExtenUrl);

    if (pAction != NULL)
    {
        pAction->SetExtraParam(iCallId);
        return true;
    }

    return false;
}


bool CallRecordingProcesser::ResumeRecord(int iCallId, const yl::string & strXsiCallId,
        int iAccountId)
{
    if (!SetLineID(iAccountId))
    {
        return false;
    }

    m_xsiExtenInfo.m_nEntenParm = iCallId;

    yl::string strExtenUrl = strXsiCallId;

    strExtenUrl.append("/ResumeRecording");


    XsiAction * pAction = ExeXsiAction(iAccountId, XSI_CALL_RECORD_RESUME, strExtenUrl);

    if (pAction != NULL)
    {
        pAction->SetExtraParam(iCallId);
        return true;
    }

    return false;
}

void CallRecordingProcesser::SetRecordMode(int iAccountId, CALL_RECORDING_MODE eMode)
{
    m_cs.Lock();
    MapAccIDRecordMode::iterator itr = m_mapIdToMode.find(iAccountId);
    if (itr == m_mapIdToMode.end())
    {
        m_mapIdToMode.put(iAccountId, eMode);
    }
    else if (itr->second != eMode)
    {
        itr->second = eMode;
        // 发送消息.
        etl_NotifyApp(
            false,
            CONFIG_MSG_BCAST_CHANGED,
            ST_CALLRECORD_MODE,
            iAccountId);
    }
    m_cs.Unlock();

}


CALL_RECORDING_MODE CallRecordingProcesser::GetRecordMode(int iAccountId)
{
    // 速度：屏蔽log
    // XSI_INFO("RecordMode Account[%d] RecordOn[%d]", iAccountId, IsCallRecordOn(iAccountId));
    if (IsCallRecordOn(iAccountId))
    {
        MapAccIDRecordMode::iterator itr = m_mapIdToMode.find(iAccountId);
        if (itr != m_mapIdToMode.end())
        {
            return itr->second;
        }
    }
    return CRM_NEVER;
}

bool CallRecordingProcesser::IsCallRecordOn(int iAccountId)
{
    return configParser_GetConfigInt(kszBroadsoftActive) == 1
           && configParser_GetCfgItemIntValue(kszAccountCallRecordSwitch, iAccountId) == 1;
}

bool CallRecordingProcesser::ProcesserMSG(msgObject & objMessage)
{
    if (objMessage.message != ACCOUNT_STATUS_CHANGED
            && objMessage.message != CONFIG_MSG_BCAST_CHANGED)
    {
        //若非对应消息，则不予处理
        return false;
    }

    if (objMessage.message == ACCOUNT_STATUS_CHANGED)
    {
        int iAccountId = (int)objMessage.wParam;
        LINE_STATE eState = acc_GetState(iAccountId);

        if (NULL != (bool *)objMessage.GetExtraData())
        {
            if (*((bool *)objMessage.GetExtraData()))
            {
                return false;
            }
        }

        switch (eState)
        {
        case LS_DISABLE:
        case LS_REGISTER_FAIL:
        case LS_BE_UNREGISTERED:
            {
                m_cs.Lock();
                MapAccIDRecordMode::iterator itr = m_mapIdToMode.find(iAccountId);
                if (itr != m_mapIdToMode.end())
                {
                    m_mapIdToMode.erase(itr);
                }
                m_cs.Unlock();
            }
            break;
        default:
            break;
        }
    }
    else if (objMessage.message == CONFIG_MSG_BCAST_CHANGED)
    {
        if (ST_FEATURE_SYNC == objMessage.wParam)
        {
            XSI_INFO("CallRecordingProcesser::ProcesserMSG BroadSoftFeatureSync : %d",
                     configParser_GetConfigInt(kszBroadSoftFeatureSync));
            if (configParser_GetConfigInt(kszBroadSoftFeatureSync) == 0)
            {
                m_cs.Lock();
                m_mapIdToMode.clear();
                m_cs.Unlock();
            }
        }
    }
    return true;
}

void CallRecordingProcesser::OnTaskActionCallback(CTaskAction * pTaskAction)
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

    if (pXsiAction->GetXsiType() == XSI_CALL_RECORD_START
            || pXsiAction->GetXsiType() == XSI_CALL_RECORD_STOP
            || pXsiAction->GetXsiType() == XSI_CALL_RECORD_PAUSE
            || pXsiAction->GetXsiType() == XSI_CALL_RECORD_RESUME)
    {
        int iCallId = pXsiAction->GetExtraParam();
        // 网络会议，需要全部失败，才发生失败消息，有一路成功就算成功。
        if (Xsi_IsNetworkConfCallId(iCallId) && pXsiAction->IsOperationSucceed())
        {
            m_bConfRecordSuc = true;
        }
        if (Xsi_IsNetworkConfCallId(iCallId) && m_TaskMap.size() == 0)
        {
            if (m_bConfRecordSuc)
            {
                // 会议每路通话执行Type相同
                // 发送消息.
                etl_NotifyAppEx(
                    false,
                    XSI_MESSAGE_REQ_FINISH,
                    pXsiAction->GetXsiType(),
                    pXsiAction->GetLineId(),
                    sizeof(int),
                    &iCallId);
            }
            else
            {
                // 全部失败，最后一路也是失败
                XSI_INFO("XSI Conf Req Fail (ActionType: %d, ErrCode: %d)", pXsiAction->GetXsiType(),
                         (int)pTaskAction->GetTaskActionErrorType());
                FailProcess(pXsiAction);
            }
            m_bConfRecordSuc = false;
        }
        else if (!Xsi_IsNetworkConfCallId(iCallId))
        {
            if (!pXsiAction->IsOperationSucceed())
            {
                XSI_INFO("XSI Req Fail (ActionType: %d, ErrCode: %d)", pXsiAction->GetXsiType(),
                         (int)pTaskAction->GetTaskActionErrorType());
                FailProcess(pXsiAction);
            }
            else
            {
                // 发送消息.
                etl_NotifyAppEx(
                    false,
                    XSI_MESSAGE_REQ_FINISH,
                    pXsiAction->GetXsiType(),
                    pXsiAction->GetLineId(),
                    sizeof(int),
                    &iCallId);
            }
        }
    }
    else
    {
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
    }

    //删除内存中的临时文件
    remove(pXsiAction->GetFilePath().c_str());
}

//操作成功处理
bool CallRecordingProcesser::ProcessByType(XsiAction * pAction)
{
    bool bSucc = true;
    if (pAction != NULL)
    {
        switch (pAction->GetXsiType())
        {
        case XSI_CALL_RECORD_MODE_GET:
            {
                bSucc = ParseReponseCmdFromFile(pAction->GetFilePath());
                m_mapCallRecordInfo[pAction->GetLineId()] = m_CallRecodInfo;
            }
            break;
        case XSI_CALL_RECORD_MODE_PUT:
            {
                m_mapCallRecordInfo[pAction->GetLineId()] = m_CallRecodInfo;
                //m_mapCallRecordInfo[pAction->GetLineId()] = m_bActive;//保存结果
            }
            break;
        default:
            break;
        }
    }
    return bSucc;
}

//解析XML数据
bool CallRecordingProcesser::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    if (strFilePath.empty())
    {
        return false;
    }
    xml_document doc;
    doc.load_file(strFilePath.c_str());

    xml_node nodeRoot = doc.child("CallRecording");
    if (!nodeRoot.empty())
    {
        yl::string strTemp;
        GetXmlChildText(nodeRoot, "recordingMode", strTemp);
        m_CallRecodInfo.nRecordMode = GetValueByText(strTemp);

        strTemp.clear();
        GetXmlChildText(nodeRoot, "pauseResumeNotifyMethod", strTemp);
        m_CallRecodInfo.nPauseResumeMethod = GetValueByText(strTemp);

        GetXmlChildText(nodeRoot, "recordVoiceMessaging", m_CallRecodInfo.bRecordVM);
        GetXmlChildText(nodeRoot, "playCallRecordingStartStopAnnouncement",
                        m_CallRecodInfo.bPlayStartStopAnnouncement);
        GetXmlChildText(nodeRoot, "playCallRecordingWarningTone", m_CallRecodInfo.bPlayTone);
        GetXmlChildText(nodeRoot, "playCallRecordingWarningToneSeconds", m_CallRecodInfo.strPlayToneTime);
        return true;
    }
    return false;
}

void CallRecordingProcesser::GetServerRecordMode(SCallRecord & RecordInfo, int nLineID)
{
    MapRecordInfo::iterator iter = m_mapCallRecordInfo.find(nLineID);
    if (m_mapCallRecordInfo.end() != iter)
    {
        RecordInfo = iter->second;
    }
}


bool CallRecordingProcesser::QueryServerRecordMode(int nLineID)
{
    if (SetLineID(nLineID))
    {
        return ExeXsiAction(nLineID, XSI_CALL_RECORD_MODE_GET, "") != NULL;
    }
    return false;
}

//设置mobility
bool CallRecordingProcesser::SetServerRecordMode(SCallRecord sRecordInfo, int nLineID)
{
    if (SetLineID(nLineID))
    {
        m_CallRecodInfo = sRecordInfo;
        yl::string strFile = XsiAction::GetRandomFile();

        if (SaveRequestCmdToFile(strFile.c_str()))
        {
            return ExeXsiAction(nLineID, XSI_CALL_RECORD_MODE_PUT, "", strFile) != NULL;//
        }
    }
    return false;
}

//保存要提交到服务器的数据到文件
bool CallRecordingProcesser::SaveRequestCmdToFile(const yl::string & strFilePath)
{
    //增加Xml头
    xml_document doc;
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "iso-8859-1");

    //增加根结点
    xml_node nodeRoot = doc.append_child("CallRecording");

    // 设置属性
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");

    yl::string strTemp;
    strTemp = GetXmlRecordMode((CALL_RECORDING_MODE)m_CallRecodInfo.nRecordMode);
    if (!strTemp.empty())
    {
        PutXmlChildText(nodeRoot, "recordingMode", strTemp);
    }

    strTemp = GetXmlPauseResumeMethod((CR_PAUSERESUME_METHOD)m_CallRecodInfo.nPauseResumeMethod);
    if (!strTemp.empty())
    {
        PutXmlChildText(nodeRoot, "pauseResumeNotifyMethod", strTemp);
    }

    PutXmlChildBool(nodeRoot, "recordVoiceMessaging", m_CallRecodInfo.bRecordVM);
    PutXmlChildBool(nodeRoot, "playCallRecordingStartStopAnnouncement",
                    m_CallRecodInfo.bPlayStartStopAnnouncement);
    PutXmlChildBool(nodeRoot, "playCallRecordingWarningTone", m_CallRecodInfo.bPlayTone);
    PutXmlChildText(nodeRoot, "playCallRecordingWarningToneSeconds", m_CallRecodInfo.strPlayToneTime);

    doc.save_file(strFilePath.c_str());
    return true;
}

yl::string CallRecordingProcesser::GetXmlPauseResumeMethod(CR_PAUSERESUME_METHOD eMethod)
{
    yl::string strValue = "";
    switch (eMethod)
    {
    case CPM_NONE:
        {
            strValue = "none";
        }
        break;
    case CPM_BEEP:
        {
            strValue = "beeponly";
        }
        break;
    case CPM_ANNOUNCEMENT:
        {
            strValue = "announcement";
        }
        break;
    default:
        break;
    }
    return strValue;
}

yl::string CallRecordingProcesser::GetXmlRecordMode(CALL_RECORDING_MODE eMode)
{
    yl::string strValue = "";
    switch (eMode)
    {
    case CRM_NEVER:
        {
            strValue = "never";
        }
        break;
    case CRM_ALWAYS:
        {
            strValue = "always";
        }
        break;
    case CRM_ALWAYS_PAUSE_RESUME:
        {
            strValue = "always-pause-resume";
        }
        break;
    case CRM_DEMAND:
        {
            strValue = "on-demand";
        }
        break;
    case CRM_DEMAND_USER_START:
        {
            strValue = "on-demand-user-start";
        }
        break;
    default:
        break;
    }
    return strValue;
}

int CallRecordingProcesser::GetValueByText(yl::string strValue)
{
    if (strValue == "never")
    {
        return (int)CRM_NEVER;
    }
    else if (strValue == "always")
    {
        return (int)CRM_ALWAYS;
    }
    else if (strValue == "always-pause-resume")
    {
        return (int)CRM_ALWAYS_PAUSE_RESUME;
    }
    else if (strValue == "on-demand")
    {
        return (int)CRM_DEMAND;
    }
    else if (strValue == "on-demand-user-start")
    {
        return (int)CRM_DEMAND_USER_START;
    }
    else if (strValue == "none")
    {
        return (int)CPM_NONE;
    }
    else if (strValue == "beeponly")
    {
        return (int)CPM_BEEP;
    }
    else if (strValue == "announcement")
    {
        return (int)CPM_ANNOUNCEMENT;
    }

    return 0;
}
#endif
