#include "dsskeyaction.h"
#include <cstdlib>
#include "../include/idsskeyimplaction.h"

CDSSKeyAction::CDSSKeyAction(DssKey_Type type)
{
    m_dsskeyType = type;
    m_pRelatedImplAction = NULL;
}

CDSSKeyAction::CDSSKeyAction()
{
    m_dsskeyType = DT_NA;
    m_pRelatedImplAction = NULL;
}

CDSSKeyAction::~CDSSKeyAction(void)
{
    m_pRelatedImplAction = NULL;
}

void CDSSKeyAction::SetDsskeyType(DssKey_Type type)
{
    m_dsskeyType = type;
}

bool CDSSKeyAction::SetImplAction(IDsskeyImplAction * pRelatedAction)
{
    m_pRelatedImplAction = pRelatedAction;
    return true;
}

bool CDSSKeyAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                               void * pExtraData)
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->OnKeyPress(dsskeyDataID, logicData, pExtraData);
    }
    return true;
}

bool CDSSKeyAction::OnUpdateUI(int dsskeyDataID, const DssKeyLogicData & logicData,
                               void * pExtraData)
{
#if 0
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->OnUpdateUI(dsskeyDataID, logicData);
    }
#else

#endif
    return true;
}

bool CDSSKeyAction::OnBinded(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->OnBindedAction(dsskeyDataID, logicData, pExtraData);
    }
    return true;
}

bool CDSSKeyAction::OnDebinded(int dsskeyDataID, const DssKeyLogicData & logicData,
                               void * pExtraData)
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->OnDebinded(dsskeyDataID, logicData, pExtraData);
    }
    return true;
}

DssKey_Type CDSSKeyAction::GetDsskeyType()
{
    return m_dsskeyType;
}

// 定时器处理
bool CDSSKeyAction::OnTimer(int dsskeyID, UINT uTimerID, const DssKeyLogicData & logicData,
                            void * pExtraData)
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->OnTimer(dsskeyID, uTimerID, logicData, pExtraData);
    }
    return true;
}

bool CDSSKeyAction::OnKeyLongPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                   void * pExtraData)
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->OnKeyLongPress(dsskeyDataID, logicData, pExtraData);
    }
    return true;
}

bool CDSSKeyAction::IsBindType()
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->IsBindType();
    }
    return false;
}

void CDSSKeyAction::SetTimer(int dsskeyID,  UINT uTimeInterval, const DssKeyLogicData & logicData,
                             void * pExtraData)
{
    if (m_pRelatedImplAction)
    {
        m_pRelatedImplAction->SetTimer(dsskeyID, uTimeInterval, logicData, pExtraData);
    }
}

//获取呼出号码
bool CDSSKeyAction::GetCallOutNumber(int dsskeyID, yl::string & strCallOutNumber,
                                     const DssKeyLogicData & logicData, void * pExtraData, int nExtType)
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->GetCallOutNumber(dsskeyID, strCallOutNumber, logicData, pExtraData,
                nExtType);
    }
    return false;
}

bool CDSSKeyAction::OnTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                 void * pExtraData)
{
    if (m_pRelatedImplAction)
    {
        DsskeyTypeList::iterator iterType = m_listListenConfigChange.begin();
        for (; iterType != m_listListenConfigChange.end(); ++iterType)
        {
            CDSSKeyAction * pAction = CDSSKeyActionManger::GetInstance()->GetDSSKeyAction(*iterType);
            if (pAction)
            {
                if (pAction->m_pRelatedImplAction)
                {
                    pAction->m_pRelatedImplAction->OnDsskeyTypeModify(dsskeyID, oldType, newType, pExtraData);
                }
            }
        }
        return true;
    }
    return false;
}

bool CDSSKeyAction::OnConfigChange(int dsskeyID, void * pExtraData)
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->OnConfigChange(dsskeyID, pExtraData);
    }
    return false;
}

bool CDSSKeyAction::AddChangeListenType(DssKey_Type eType)
{
    DsskeyTypeList::iterator iterType = yl::find(m_listListenConfigChange.begin(),
                                        m_listListenConfigChange.end(),
                                        eType);

    // 如果类型不存在则添加
    if (iterType == m_listListenConfigChange.end())
    {
        m_listListenConfigChange.push_back(eType);
        return true;
    }

    return false;
}

bool CDSSKeyAction::DeleteChangeListenType(DssKey_Type eType)
{
    DsskeyTypeList::iterator iterType = yl::find(m_listListenConfigChange.begin(),
                                        m_listListenConfigChange.end(),
                                        eType);

    //如果类型存在则删除
    if (iterType != m_listListenConfigChange.end())
    {
        m_listListenConfigChange.erase(iterType);
        return true;
    }

    return false;
}

bool CDSSKeyAction::OnKeyReloadFinish()
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->OnKeyReloadFinish();
    }
    return false;
}

bool CDSSKeyAction::BeforeUpdateUI(int dsskeyDataID, DsskeyExpRefresh dsskeyRefresh/* = DSS_KEY_REFRESH_ALL*/)
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->BeforeUpdateUI(dsskeyDataID, dsskeyRefresh);
    }
    return true;
}

#if IF_FEATRUE_PTT
bool CDSSKeyAction::OnKeyRelease(int dsskeyDataID, const DssKeyLogicData & logicData,
                                 void * pExtraData)
{
    if (m_pRelatedImplAction)
    {
        return  m_pRelatedImplAction->OnKeyRelease(dsskeyDataID, logicData, pExtraData);
    }
    return true;
}
#endif
