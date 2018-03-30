#include "exedata_inc.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
void BWExeAssis_SetUserType(EEA_TYPE eType, int iLineID, const CExeAssisInspector * pInfo)
{
    g_refExeAssisProcessor.SetUserType(eType, iLineID, pInfo);
}

bool BWExeAssis_GetData(CExeAssisInspector * pInfo, int iLineID)
{
    return g_refExeAssisProcessor.GetData(pInfo, iLineID);
}

bool BWExeAssis_SetData(const CExeAssisInspector * pInfo, int iLineID,
                        ExeAssis_Action eAction/* = EAA_AutoSelect*/)
{
    return g_refExeAssisProcessor.SetData(pInfo, iLineID, eAction);
}

int BWExeAssis_GetAccountCountByType(EEA_TYPE eType)
{
    YLVector<int> listData;
    g_refExeAssisProcessor.GetAccountListByType(listData, eType);

    return listData.size();
}

int BWExeAssis_GetFirstUsableAccountByType(EEA_TYPE eType /*= EET_ALL*/)
{
    YLVector<int> listData;
    if (g_refExeAssisProcessor.GetAccountListByType(listData, eType) > 0)
    {
        return listData[0];
    }

    return -1;
}

void BWExeAssis_SetLineId(int iLineID)
{
    g_refExeAssisProcessor.SetLineId(iLineID);
}

int BWExeAssis_GetLineId()
{
    return g_refExeAssisProcessor.GetLineId();
}

bool BWExeAssis_SetCallInitiationNumber(const yl::string & strNumber, int iLineID)
{
    return g_refExeAssisProcessor.SetCallInitiationNumber(strNumber, iLineID);
}

int BWExeAssis_GetExecutiveListCount(int iLineID)
{
    return g_refExeAssisProcessor.GetExecutiveListSize(iLineID);
}

bool BWExeAssis_IsExecutive(int iLineID)
{
    return (g_refExeAssisProcessor.GetUserType(iLineID) & EET_Executive);
}

bool BWExeAssis_IsAssistant(int iLineID)
{
    return (g_refExeAssisProcessor.GetUserType(iLineID) & EET_Assistant);
}

bool BWExeAssis_IsNeedShowAssistantDetailsSetting(int iLineID/* = -1*/)
{
    // 暂时不开放，Assistant对Executive的设置权限，先预留接口
    return false;

    /*if (iLineID == -1)
    {
        iLineID = g_refExeAssisProcessor.GetLineId();
    }

    return !(g_refExeAssisProcessor.GetUserType(iLineID) & EET_ALL);*/
}

bool BWExeAssis_IsCallInitiationEnable(int iLineID)
{
    // 本地权限 + Executive列表不为空 + XSI实现时XSI鉴权成功
    return g_refExeAssisProcessor.CheckAction(EAA_CallInitiation, iLineID)
           && g_refExeAssisProcessor.GetExecutiveListSize(iLineID) > 0
           && (!g_refExeAssisProcessor.IsXsiImp() || XSI_SUCCESS == CBaseHelper::GetXSIStatus(iLineID));
}

bool BWExeAssis_IsSilentMonitorEnable(int iLineID)
{
    return g_refExeAssisProcessor.CheckAction(EAA_SilentMonitoe, iLineID);
}

bool BWExeAssis_CallPush(int iCallId, int iLineID)
{
    return g_refExeAssisProcessor.CallPush(iCallId, iLineID);
}

bool BWExeAssis_CallIntiation(const yl::string & strTarget, int iLineID)
{
    return g_refExeAssisProcessor.CallIntiation(strTarget, iLineID);
}

bool BWExeAssis_QueryAction(ExeAssis_Action eAction, int iLineID)
{
    return g_refExeAssisProcessor.QueryByType(eAction, iLineID);
}

void BWExeAssis_CancelAllRequest()
{
    g_refExeAssisProcessor.CancelAllRequest();
}

bool BWExeAssis_IsExecutiveNumber(const yl::string & strNumber, int iLineID)
{
    return g_refExeAssisProcessor.IsExecutiveNumber(strNumber, iLineID);
}


#endif


