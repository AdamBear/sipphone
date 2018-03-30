#include "exedata_inc.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
CExecutiveAssistantProcessor & CExecutiveAssistantProcessor::GetInstance()
{
    static CExecutiveAssistantProcessor s_Inst;
    return s_Inst;
}

CExecutiveAssistantProcessor::CExecutiveAssistantProcessor() :
    m_bXSIImp(true)
{
    XsiAction::InsertMap(XSI_EXECUTIVE_ASSISTANT_CALLPUSH, "calls/");
    XsiAction::InsertMap(XSI_EXECUTIVE_ASSISTANT_CALLINITIATE, "calls/");

    XsiAction::InsertMap(XSI_EXECUTIVE_ASSISTANTLIST_GET, "services/executive/assistants");
    XsiAction::InsertMap(XSI_EXECUTIVE_ASSISTANTLIST_PUT, "services/executive/assistants");

    XsiAction::InsertMap(XSI_EXECUTIVE_FILTER_GET, "services/executive/filtering");
    XsiAction::InsertMap(XSI_EXECUTIVE_FILTER_PUT, "services/executive/filtering");

    XsiAction::InsertMap(XSI_EXECUTIVE_SCREENING_GET, "services/executive/screening");
    XsiAction::InsertMap(XSI_EXECUTIVE_SCREENING_PUT, "services/executive/screening");

    XsiAction::InsertMap(XSI_EXECUTIVE_ALTER_GET, "services/executive/alerting");
    XsiAction::InsertMap(XSI_EXECUTIVE_ALTER_PUT, "services/executive/alerting");

    XsiAction::InsertMap(XSI_ASSISTANT_EXECUTIVELIST_GET, "services/executiveassistant");
    XsiAction::InsertMap(XSI_ASSISTANT_EXECUTIVELIST_PUT, "services/executiveassistant");
}

bool CExecutiveAssistantProcessor::QueryByType(ExeAssis_Action eAction, int iAccount)
{
    return CreateExeXsiAction(iAccount, eAction, true) != NULL;
}

//override, 处理执行cmd之后从服务器返回的结果
bool CExecutiveAssistantProcessor::ProcessByType(XsiAction * pAction)
{
    bool bSucc = true;
    if (NULL != pAction)
    {
        int iAccount = pAction->GetLineId();
        eXsiActionType eType = pAction->GetXsiType();
        int iExtraParam = pAction->GetExtraParam();

        XSI_INFO("ParseReponseCmdFromFile action=[%d] xsi_action=[%d] ext=[%d]", iAccount, eType,
                 iExtraParam);

        switch (eType)
        {
        case XSI_EXECUTIVE_ASSISTANTLIST_GET:
        case XSI_EXECUTIVE_FILTER_GET:
        case XSI_EXECUTIVE_SCREENING_GET:
        case XSI_EXECUTIVE_ALTER_GET:
        case XSI_ASSISTANT_EXECUTIVELIST_GET:
            {
                bSucc = ParseReponseCmdFromFile(pAction->GetFilePath(), iAccount);
            }
            break;
        case XSI_EXECUTIVE_ASSISTANTLIST_PUT:
        case XSI_EXECUTIVE_FILTER_PUT:
        case XSI_EXECUTIVE_SCREENING_PUT:
        case XSI_EXECUTIVE_ALTER_PUT:
        case XSI_ASSISTANT_EXECUTIVELIST_PUT:
            {
                ExeAssis_Action eAction = MapXSIAction2LocalAction(eType);
                int iIndex = eType == XSI_ASSISTANT_EXECUTIVELIST_PUT ? iExtraParam : -1;

                if (iIndex >= 0)
                {
                    SplitRefOperator(false, eAction);
                }

                return Local2Map(iAccount, eAction, iIndex);
            }
            break;
        case XSI_EXECUTIVE_ASSISTANT_CALLPUSH:
            {
                // Call Push 成功直接退出通话
                return talklogic_ExitSession(iExtraParam);
            }
            break;
        default:
            break;
        }
    }

    return bSucc;
}

void CExecutiveAssistantProcessor::FailProcess(XsiAction * pAction)
{
    if (pAction != NULL)
    {
        XsiBaseProcesser::FailProcess(pAction);

        ExeAssis_Action eAction = MapXSIAction2LocalAction(pAction->GetXsiType());
        if (eAction == EAA_AssisCommon && pAction->GetExtraParam() >= 0)
        {
            SplitRefOperator(false, eAction);
        }
    }
}

ExeAssis_Action CExecutiveAssistantProcessor::MapXSIAction2LocalAction(eXsiActionType eXsiType)
{
    switch (eXsiType)
    {
    case XSI_EXECUTIVE_ASSISTANTLIST_PUT:
    case XSI_EXECUTIVE_ASSISTANTLIST_GET:
        return EAA_ExeCommon;
    case XSI_EXECUTIVE_FILTER_GET:
    case XSI_EXECUTIVE_FILTER_PUT:
        return EAA_Filter;
    case XSI_EXECUTIVE_SCREENING_GET:
    case XSI_EXECUTIVE_SCREENING_PUT:
        return EAA_Screen;
    case XSI_EXECUTIVE_ALTER_GET:
    case XSI_EXECUTIVE_ALTER_PUT:
        return EAA_Alter;
    case XSI_ASSISTANT_EXECUTIVELIST_GET:
    case XSI_ASSISTANT_EXECUTIVELIST_PUT:
        return EAA_AssisCommon;
    case XSI_EXECUTIVE_ASSISTANT_CALLPUSH:
        return EAA_CallPush;
    case XSI_EXECUTIVE_ASSISTANT_CALLINITIATE:
        return EAA_CallInitiation;
    default:
        return EAA_None;
    }
}

eXsiActionType CExecutiveAssistantProcessor::MapLocalAction2XSIAction(ExeAssis_Action eActionType,
        bool bGet)
{
    if (bGet)
    {
        switch (eActionType)
        {
        case EAA_ExeCommon:
            return XSI_EXECUTIVE_ASSISTANTLIST_GET;
        case EAA_Filter:
            return XSI_EXECUTIVE_FILTER_GET;
        case EAA_Screen:
            return XSI_EXECUTIVE_SCREENING_GET;
        case EAA_Alter:
            return XSI_EXECUTIVE_ALTER_GET;
        case EAA_AssisCommon:
            return XSI_ASSISTANT_EXECUTIVELIST_GET;
        case EAA_CallInitiation:
            return XSI_EXECUTIVE_ASSISTANT_CALLINITIATE;
        default:
            return XSI_GET_BEGIN;
        }
    }
    else
    {
        switch (eActionType)
        {
        case EAA_ExeCommon:
            return XSI_EXECUTIVE_ASSISTANTLIST_PUT;
        case EAA_Filter:
            return XSI_EXECUTIVE_FILTER_PUT;
        case EAA_Screen:
            return XSI_EXECUTIVE_SCREENING_PUT;
        case EAA_Alter:
            return XSI_EXECUTIVE_ALTER_PUT;
        case EAA_AssisCommon:
            return XSI_ASSISTANT_EXECUTIVELIST_PUT;
        case EAA_CallInitiation:
            return XSI_EXECUTIVE_ASSISTANT_CALLINITIATE;
        case EAA_CallPush:
            return XSI_EXECUTIVE_ASSISTANT_CALLPUSH;
        default:
            return XSI_PUT_BEGIN;
        }
    }
}

bool CExecutiveAssistantProcessor::CallPush(int iCallId, int iAccount)
{
    if (m_bXSIImp)
    {
        m_xsiExtenInfo.m_nEntenParm = iCallId;

        // callid 获取
        yl::string strExtenUrl = Xsi_GetXsiCallId(iCallId);
        strExtenUrl.append("/executiveAssistantCallPush");

        XsiAction * pAction = CreateExeXsiAction(iAccount, EAA_CallPush, false, strExtenUrl);
        if (pAction != NULL)
        {
            pAction->SetExtraParam(iCallId);
            return true;
        }
    }
    else
    {
        /* FAC 待实现
        yl::string strTarget = "*63";
        return talklogic_CallOut(strTarget, "", iAccount);*/
    }

    return false;
}

bool CExecutiveAssistantProcessor::CallIntiation(const yl::string & strTarget, int iAccount)
{
    yl::string strInitiationNumber = GetInitiationNumber(iAccount);
    if (strInitiationNumber.empty())
    {
        return false;
    }

    if (m_bXSIImp)
    {
        yl::string strExtenUrl =
            commonAPI_FormatString("ExecutiveAssistantInitiateCall?executiveAddress=%s&address=%s",
                                   strInitiationNumber.c_str(), strTarget.c_str());

        return CreateExeXsiAction(iAccount, EAA_CallInitiation, false, strExtenUrl) != NULL;
    }
    else
    {
        /* FAC 待实现
        yl::string strTarget = "*64";
        strTarget += strInitiationNumber + "*" + strTarget;
        return talklogic_CallOut(strTarget, "", iAccount);*/
    }

    return false;
}

XsiAction * CExecutiveAssistantProcessor::CreateExeXsiAction(int iAccount,
        ExeAssis_Action eActionType, bool bGet,
        const yl::string & strExtUrl /*= ""*/, const yl::string & strFile /*= ""*/, int iIndex/* = -1*/)
{
    XSI_INFO("CreateExeXsiAction account=[%d] action=[%d] get=[%d] ext=[%s] file=[%s]",
             iAccount, eActionType, bGet, strExtUrl.c_str(), strFile.c_str());

    if (CheckAction(eActionType, iAccount)
            && SetLineID(iAccount))
    {
        XsiAction * pAction = ExeXsiAction(iAccount, MapLocalAction2XSIAction(eActionType, bGet), strExtUrl,
                                           strFile);
        if (pAction != NULL)
        {
            pAction->SetExtraParam(iIndex);
            return pAction;
        }
    }

    return NULL;
}
#endif


