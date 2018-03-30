#include "netconfmgrprocessor.h"
#include "talkuilogiccommon.h"

#include "talkuilogicdelegate.h"
#include "talkuilogicprocessorname.h"

#include "talkuilogicframemanager.h"
#include "uilogicnetconfmgruibase.h"

#include "modtalkuilogic.h"

CNetConfMgrProcessor::CNetConfMgrProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_NETCONFMGR)
{
    m_pTalkUI = NULL;
}

CNetConfMgrProcessor::~CNetConfMgrProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CNetConfMgrProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objNetConfMgrUIData.Reset();

}

//释放前解除UI绑定前调用
void CNetConfMgrProcessor::Uninit()
{
    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CNetConfMgrProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicNetConfMgrUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CNetConfMgrProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取保存的ui数据
CNetConfMgrUIData * CNetConfMgrProcessor::GetNetConfMgrUIData()
{
    return &m_objNetConfMgrUIData;
}

//设置数据
void CNetConfMgrProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    // 入口参数检查
    if (NULL == pData)
    {
        return;
    }

    yl::string strIncomingCallNum = "";

    //如果有获取到新来电号码 那么有来电的标识设置为true  否则 设置为false
    if (talklogic_GetIncomingCallNum(strIncomingCallNum))
    {
        return;
    }

    DataTalking2UI * pTalkingData = (DataTalking2UI *)(pData);
    if (NULL == pTalkingData)
    {
        return;
    }

    m_objNetConfMgrUIData.listContactInfo.clear();
    m_objNetConfMgrUIData.m_listNetConfUserID.clear();

    int nCreatorIndex = 0;
    //根据列表设置话路信息,设置列表
    for (YLList<DataNetConfUserInfo>::iterator itUserInfo = pTalkingData->listNetUserInfo.begin()
            ; itUserInfo != pTalkingData->listNetUserInfo.end()
            ; ++itUserInfo)
    {
        DataNetConfUserInfo& refDataInfo = *itUserInfo;
        CContactInfo objContactInfo;

        objContactInfo.strName = refDataInfo.strUserName;
        objContactInfo.iCallID = -1;
        objContactInfo.m_eAudioCodec = ACT_NONE;

        objContactInfo.bLocalHold = false;
        objContactInfo.bFarHold = false;
        objContactInfo.bRemoteHold = false;

        if (refDataInfo.bConfCreator
                && m_objNetConfMgrUIData.m_nCreatorIndex != -1)
        {
            m_objNetConfMgrUIData.m_nCreatorIndex = nCreatorIndex;
        }
        ++nCreatorIndex;

        //数据插入队列中
        m_objNetConfMgrUIData.listContactInfo.push_back(objContactInfo);
        m_objNetConfMgrUIData.m_listNetConfUserID.push_back(refDataInfo.strUserID);
    }
}


void CNetConfMgrProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    VecSoftkey.push_back(ST_CANCEL);
    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EMPTY);

    if (talklogic_IsNetConfCreator(GetSessionID())
            && !GetIsFocusCreator())
    {
        VecSoftkey.push_back(ST_CONF_REMOVE);
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }
}

bool CNetConfMgrProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_CANCEL:
        {
            //这边是退出 confmanager 界面
            SendUIRequestAction(TALK_ACTION_EXIT_CONF_MANAGER);

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

//softKey 按键处理
bool CNetConfMgrProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;
    switch (eSoftkey)
    {
    case ST_CANCEL:
        {
            SendUIRequestAction(TALK_ACTION_EXIT_CONF_MANAGER);
            bHandled = true;
        }
        break;
    case ST_CONF_REMOVE:
        {
            if (NULL != m_pTalkUI)
            {
                yl::string strNameHint = GetFocusName();

                if (!strNameHint.empty())
                {
                    m_pTalkUI->ShowRemoveMemberBox(strNameHint);
                }
            }

            bHandled = true;
        }
        break;

    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

//设置title
void CNetConfMgrProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    DataTalking2UI * pTalkingData = (DataTalking2UI *)(pData);
    if (NULL == pTalkingData)
    {
        return;
    }

    m_strTitle = pTalkingData->strConfName;
}

//设置图标列表
void CNetConfMgrProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    return;
}

//给子界面设置数据
void CNetConfMgrProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objNetConfMgrUIData, eReason);
    }
}


yl::string CNetConfMgrProcessor::GetFocusName()
{
    yl::string strName = "";

    if (NULL != m_pTalkUI)
    {
        int nDataIndex = m_pTalkUI->GetFocusCallID();
        if (nDataIndex > 0
                && nDataIndex < m_objNetConfMgrUIData.listContactInfo.size())
        {
            strName = m_objNetConfMgrUIData.listContactInfo[nDataIndex].strName;
        }
    }

    return strName;
}

bool CNetConfMgrProcessor::GetIsFocusCreator()
{
    if (NULL != m_pTalkUI)
    {
        int nDataIndex = m_pTalkUI->GetFocusCallID();
        if (nDataIndex == m_objNetConfMgrUIData.m_nCreatorIndex)
        {
            return true;
        }
    }

    return false;
}

void CNetConfMgrProcessor::DealRemoveMember()
{
    int nDataIndex = m_pTalkUI->GetFocusCallID();

    if (nDataIndex > 0
            && nDataIndex < m_objNetConfMgrUIData.m_listNetConfUserID.size())
    {
        SendUIRequestAction(TALK_ACTION_REMOVE_NET_CONF_PARTY
                            , 0
                            , 0
                            , (char*)m_objNetConfMgrUIData.m_listNetConfUserID[nDataIndex].c_str());
    }
}

