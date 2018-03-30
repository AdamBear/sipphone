#include "netconfprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "talkuilogicprocessorname.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
// #include "settingui/include/modsettingui.h"
#include "talkuilogicframemanager.h"
#if IF_TALKUI_USB_ENABLE
#include "record/include/modrecord.h"
#endif
#include "uilogicnetconfuibase.h"

#include "modtalkuilogic.h"

#include "broadsoft/xsiactionhelper/include/modxsicommon.h"

CNetConfProcessor::CNetConfProcessor()
{
    m_pTalkUI = NULL;

    m_bShowTalkTime = true;     //是否显示通话时间

    m_strProcessorName = PROCESSORNAME_NETCONF;

#warning modRecord_IsIdle
//#if IF_TALKUI_USB_ENABLE
//    if (modRecord_IsIdle(RECORD_AUDIO)
//            && modRecord_IsRecordEnable(RECORD_AUDIO)
//            && 0 != configParser_GetConfigInt(kszUSBAutoRecordEnable))
//    {
//        TALKUI_INFO("Auto record when talk established.");
//        DealBeginRecord();
//    }
//#endif
}

CNetConfProcessor::~CNetConfProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CNetConfProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objNetConfUIData.Reset();
}

//释放前解除UI绑定前调用
void CNetConfProcessor::Uninit()
{
    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CNetConfProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicNetConfUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CNetConfProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取保存的ui数据
CNetConfUIData * CNetConfProcessor::GetNetConfUIData()
{
    return &m_objNetConfUIData;
}

//设置数据
void CNetConfProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    // 入口参数检查
    if (NULL == pData)
    {
        return;
    }

    CTalkingProcessor::SetTalkData(pData, eReason);

    m_objNetConfUIData.m_TalkingUIData = m_objTalkingUIData;

    m_objNetConfUIData.m_listNetUserInfo.clear();
    DataTalking2UI * pTalkingData = (DataTalking2UI *)(pData);
    if (NULL != pTalkingData)
    {
        for (YLList<DataNetConfUserInfo>::iterator iter = pTalkingData->listNetUserInfo.begin()
                ; pTalkingData->listNetUserInfo.end() != iter
                ; ++iter)
        {
            m_objNetConfUIData.m_listNetUserInfo.push_back((*iter).strUserName);
        }
    }
}


void CNetConfProcessor::GetTalkStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (m_objNetConfUIData.m_listNetUserInfo.size() > 0)
    {
        VecSoftkey.push_back(ST_CONF_MANAGE);
    }

    CTalkingProcessor::GetTalkStatusSoftkey(VecSoftkey);
}

//softKey 按键处理
bool CNetConfProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;
    switch (eSoftkey)
    {
    case ST_CONF_MANAGE:
        {
            SendUIRequestAction(TALK_ACTION_ENTER_CONF_MANAGER);
            bHandled = true;
        }
        break;

    default:
        bHandled = false;
        break;
    }

    if (bHandled)
    {
        return true;
    }

    return CTalkingProcessor::OnSoftkeyPress(eSoftkey);
}

//设置title
void CNetConfProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    DataTalking2UI * pTalkingData = (DataTalking2UI *)(pData);

    if (NULL != pTalkingData)
    {
        bool bResult = (pTalkingData->listNetUserInfo.size() > 0);

        if (bResult)
        {
            m_strTitle = pTalkingData->strConfName;
        }
        else
        {
            m_strTitle = yl::string(TRID_CONFERENCE);
        }
    }
}

//给子界面设置数据
void CNetConfProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objNetConfUIData, eReason);
    }
}

