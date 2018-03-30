#include "acdemergencyescalationdelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "settingui/include/itemnamedefine.h"

CACDEmergencyEscalationDelegate::CACDEmergencyEscalationDelegate()
    : m_nCallId(-1)
{

}

CACDEmergencyEscalationDelegate::~CACDEmergencyEscalationDelegate()
{

}

CSettingUIDelegateBase * CACDEmergencyEscalationDelegate::CreateACDEmergencyEscalationDelegate()
{
    CACDEmergencyEscalationDelegate * pDelegate = new CACDEmergencyEscalationDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CACDEmergencyEscalationDelegate::InitExtraData(void * pData)
{
    if (NULL == pData)
    {
        return;
    }

    m_nCallId = *(int *)pData;
}

CSettingUIPageDataBase * CACDEmergencyEscalationDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    //获取Supervisor列表
    SupervisorList listSupervisor;

#ifdef _WIN32
    Supervisor supervisor;
    supervisor.m_strNumber = "1001";
    supervisor.m_strName = "Number1";
    listSupervisor.push_back(supervisor);

    supervisor.m_strNumber = "1002";
    supervisor.m_strName = "Number2";
    listSupervisor.push_back(supervisor);

    supervisor.m_strNumber = "1003";
    supervisor.m_strName = "Number3";
    listSupervisor.push_back(supervisor);

    supervisor.m_strNumber = "1004";
    supervisor.m_strName = "Number4";
    listSupervisor.push_back(supervisor);

    supervisor.m_strNumber = "1005";
    supervisor.m_strName = "Number5";
    listSupervisor.push_back(supervisor);
#else
    ACD_GetSupervisorList(listSupervisor);
#endif

    m_codeVec.clear();

    //添加数据项
    for (SupervisorList::iterator iter = listSupervisor.begin(); iter != listSupervisor.end(); ++iter)
    {
        PushbackItem((*iter).m_strNumber, (*iter).m_strName);
    }

    return m_pPageData;
}

bool CACDEmergencyEscalationDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    int iRsnIdx = pPageData->GetFocusIndex();

    yl::string strNumber = "";
    if (iRsnIdx == 0)
    {
        CSettingUIItemDataBasePtr pItemData = pPageData->GetItemByItemId(
                ACD_EMERGENCY_ESCALATION_ENTER_NUMBER);
        if (NULL != pItemData)
        {
            strNumber = pItemData->GetValue();
        }
    }
    else if (iRsnIdx > 0)
    {
        // 去掉第一项的编辑框
        iRsnIdx--;
        if (iRsnIdx < m_codeVec.size())
        {
            strNumber = m_codeVec[iRsnIdx];
        }
    }

    CallCenter_EmergencyEscalation(strNumber, m_nCallId);

    g_SetUICallBackFun.FallBackToPrevousPage();

    return true;
}

bool CACDEmergencyEscalationDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        eActionType = SET_ACTION_TYPE_SAVE;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

void CACDEmergencyEscalationDelegate::PushbackItem(const yl::string & strCode,
        const yl::string & strName)
{
    if (strCode.empty())
    {
        // 如果Code为空，则不添加
        return;
    }

    CSettingUIItemDataBasePtr pItemData = new CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE);
    if (NULL == pItemData)
    {
        return;
    }

    pItemData->SetDisplayName(strCode);
    pItemData->SetShowIndex(true);
    pItemData->SetValue(strName);
    if (!m_pPageData->AddItem(pItemData))
    {
        delete pItemData;
    }

    //记录Reason项，用于操作
    m_codeVec.push_back(strCode);
}

void CACDEmergencyEscalationDelegate::SetCallID(int iCallID)
{
    m_nCallId = iCallID;
}
