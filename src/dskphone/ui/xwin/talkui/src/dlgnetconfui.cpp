#include "dlgnetconfui.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"
#include "talkui_define.h"
#include "talkuicommonfunction.h"
#include "modtalkui.h"
#include "talkuiframemanager.h"
#include "talkuixmlitemname.h"
#include "conflictmanager.h"
#include "talkuifactory.h"
#include "talkuixmlpath.h"

////////////////// talkuilogic /////////////////////
#include "talkuilogic/talkuilogicdelegate.h"

#include "settingui/include/modsettingui.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"

namespace talkui
{
const char * TALK_UI_ATTR_LIST_ITEM_ALIGN = "android:itemalign";

void netconfShowIncomingHint(bool bShow)
{
    CDlgNetConfUI* pNetConfUI = (CDlgNetConfUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_NETCONF);
    if (NULL != pNetConfUI)
    {
        pNetConfUI->showIncomingHint(bShow);
    }
}

#if IF_NETCONF_UI_STYLE
void netconfShowStatusHint(bool bShow)
{
    CDlgNetConfUI* pNetConfUI = (CDlgNetConfUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_NETCONF);
    if (NULL != pNetConfUI)
    {
        pNetConfUI->showStatusHint(bShow);
    }
}
#else
void netconfShowMemberHint(bool bShow)
{
    CDlgNetConfUI* pNetConfUI = (CDlgNetConfUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_NETCONF);
    if (NULL != pNetConfUI)
    {
        pNetConfUI->showMemberHint(bShow);
    }
}
#endif

void netconfShowTimer(bool bShow)
{
    CDlgNetConfUI* pNetConfUI = (CDlgNetConfUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_NETCONF);
    if (NULL != pNetConfUI)
    {
        pNetConfUI->showTimer(bShow);
    }
}

CDlgNetConfUI::CDlgNetConfUI()
    : CDlgBaseTalkUI(DLG_NetConfUI, eWT_Talk)
#if !IF_NETCONF_UI_STYLE
    , m_pMemberHint(NULL)
    , m_nItemAlign(0)
    , m_nListPageCount(0)
    , m_pNetConfList(NULL)
    , m_pMemberHintFrame(NULL)
#endif
{
}

CDlgNetConfUI::~CDlgNetConfUI()
{

}

void CDlgNetConfUI::LoadLayout()
{
#if IF_NETCONF_UI_STYLE
    loadContent(TALKUI_DLGCONFUI_XML);
#else
    loadContent(TALKUI_DLGNETCONF_XML);
#endif
}

void CDlgNetConfUI::loadAttributes(xml_node& node)
{
    CDlgBaseTalkUI::loadAttributes(node);

#if !IF_NETCONF_UI_STYLE
    PopupAndroidInt(node, TALK_UI_ATTR_LIST_ITEM_ALIGN, m_nItemAlign);

    if (m_nItemAlign & align_left)
    {
        m_nItemAlign = align_left | align_v_center;
    }
    else// if (m_nItemAlign & align_h_center)
    {
        m_nItemAlign = align_center;
    }
#endif
}

void CDlgNetConfUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

#if IF_NETCONF_UI_STYLE
    m_talkWidget.SetNormalHintWidget(static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_1))
                                     , static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_2)));
#endif

    m_talkWidget.SetTimeWidget(static_cast<xTimerPieceView*>(getViewById(TALKUI_TALK_TALK_TIME)));

    m_talkWidget.SetStatusWidget(static_cast<xImageView *>(getViewById(TALKUI_TALK_NO_CONTACT_STATUS_FRAME))
                                 , static_cast<xImageView *>(getViewById(TALKUI_TALK_NO_CONTACT_STATUS_ICON))
                                 , static_cast<xTextView *>(getViewById(TALKUI_TALK_NO_CONTACT_STATUS_HINT)));

#if IF_TALKUI_CONTACT_PHOTO
    xAnimationBox* pIncomingAnimation = static_cast<xAnimationBox*>(getViewById(TALKUI_TALKING_INCOMING_ANIMATION));
#else
    xAnimationBox* pIncomingAnimation = NULL;
#endif

    m_talkWidget.SetIncomingWidget(static_cast<xView *>(getViewById(TALKUI_TALKING_INCOMING_FRAME))
                                   , pIncomingAnimation
                                   , static_cast<xTextView *>(getViewById(TALKUI_TALKING_INCOMING_CALL_HINT))
                                   , static_cast<xImageView *>(getViewById(TALKUI_TALKING_INCOMING_CALL_ARROW))
                                   , static_cast<xTextView *>(getViewById(TALKUI_TALKING_INCOMING_CALL_TARGET)));

#if IF_TALKUI_CONTACT_PHOTO
    if (NULL != pIncomingAnimation)
    {
        LIST_STRING listIconPath;
        listIconPath.push_back(PIC_TALK_ANIMATION_TALK_INCOMINGCALL1);
        listIconPath.push_back(PIC_TALK_ANIMATION_TALK_INCOMINGCALL2);
        listIconPath.push_back(PIC_TALK_ANIMATION_TALK_INCOMINGCALL3);
        pIncomingAnimation->SetAnimation(listIconPath);
    }
#endif

#if IF_HD_ICON_DEPART
    m_talkWidget.SetHDIcon(static_cast<xImageView *>(getViewById(TALKUI_TALK_HD_ICON)));
#endif

#if !IF_NETCONF_UI_STYLE
    m_pNetConfList = static_cast<xListView *>(getViewById(TALKUI_NETCONF_LIST));
    m_pMemberHint = static_cast<xTextView *>(getViewById(TALKUI_MEMBER_HINT));
    m_pMemberHintFrame = static_cast<xListView *>(getViewById(TALKUI_MEMBER_HINT_FRMAE));
#endif
}

void CDlgNetConfUI::ResetWidget()
{
    m_talkWidget.ResetWidget();

#if !IF_NETCONF_UI_STYLE
    if (NULL != m_pNetConfList)
    {
        m_pNetConfList->ClearContent();
    }

    if (NULL != m_pMemberHint)
    {
        m_pMemberHint->SetText("");
        m_pMemberHint->hide(true);
    }

    if (NULL != m_pMemberHintFrame)
    {
        m_pMemberHintFrame->hide(true);
    }
#endif
}

// 初始化
void CDlgNetConfUI::Init()
{
    CDlgBaseTalkUI::Init();

#if !IF_NETCONF_UI_STYLE
    m_objNetConfListAdapter.Init();

    m_objNetConfListAdapter.SetTextAlign(m_nItemAlign);

    if (NULL != m_pNetConfList)
    {
        m_pNetConfList->SetAdapter(&m_objNetConfListAdapter);
        m_nListPageCount = m_pNetConfList->GetPageItemCount();
    }
#endif

    CONFLICT_UNIT stUnit;

#if IF_TALKUI_CONFLICT_STYLE
    stUnit.m_ShowFunction = netconfShowIncomingHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_INCOMING_INFO, stUnit);

#if IF_NETCONF_UI_STYLE
    stUnit.m_ShowFunction = netconfShowStatusHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_STATUS_INFO, stUnit);
#endif

    stUnit.m_bShow = true;
    stUnit.m_ShowFunction = netconfShowTimer;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_TIME, stUnit);
#endif

#if !IF_NETCONF_UI_STYLE
    stUnit.m_ShowFunction = netconfShowIncomingHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_INCOMING_INFO, stUnit);

    stUnit.m_ShowFunction = netconfShowMemberHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_NET_CONF_MEMBER_HINT, stUnit);
#endif
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgNetConfUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

#if IF_TALKUI_CONFLICT_STYLE
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_INCOMING_INFO);
#if IF_NETCONF_UI_STYLE
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_STATUS_INFO);
#endif
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_TIME);
#endif

#if !IF_NETCONF_UI_STYLE
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_INCOMING_INFO);
    _TalkConflictManager.removeConflictUnit(CONFLICT_NET_CONF_MEMBER_HINT);
#endif

#if !IF_NETCONF_UI_STYLE
    if (NULL != m_pNetConfList)
    {
        m_pNetConfList->SetAdapter(NULL);
    }

    m_objNetConfListAdapter.Uninit();
#endif
}

// 刷新窗口
void CDlgNetConfUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

//获取 processor
CBaseTalkUIProcessor * CDlgNetConfUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgNetConfUI::IsShowDtmfLine() const
{
    return true;
}

// 是否显示切换话路的滚动条
bool CDlgNetConfUI::IsShowSessionScrollBar()
{
    return true;
}

// 能否切换Session
bool CDlgNetConfUI::IsChangeSession(int iKey)
{
    return true;
}


bool CDlgNetConfUI::OnKeyPreProcess(int iKey)
{
#if IF_HD_ICON_DEPART
    if (NULL != m_pProcessor)
    {
        CNetConfUIData * pNetConfUIData = m_pProcessor->GetNetConfUIData();
        if (NULL != pNetConfUIData)
        {
            SetHDIconInfo(pNetConfUIData);
        }
    }
#endif

    return CDlgBaseTalkUI::OnKeyPreProcess(iKey);
}


void CDlgNetConfUI::OnTalkVolumnVisibleChange(bool bVisible)
{
#if IF_HD_ICON_DEPART
    if (NULL != m_pProcessor)
    {
        CNetConfUIData * pNetConfUIData = m_pProcessor->GetNetConfUIData();
        if (NULL != pNetConfUIData)
        {
            SetHDIconInfo(pNetConfUIData);
        }
    }
#endif

    CDlgBaseTalkUI::OnTalkVolumnVisibleChange(bVisible);
}

//设置UILogic 数据
void CDlgNetConfUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{
    //不做处理  统一放到RefreshTalkUIData处理
}

void CDlgNetConfUI::ShowSecurityBox(int nAccount)
{
    ShowSecurityMessageBox(nAccount);
}

void CDlgNetConfUI::ProcessSecurityCallBack(const yl::string & strResult)
{
    CBaseTalkUIProcessor * pProcessor = GetTalkUIProcessor();

    if (pProcessor != NULL)
    {
        pProcessor->DealSecurityResult(strResult);
    }
}

void CDlgNetConfUI::ShowACDDispCodeMenu(int nCallID)
{
    SettingUI_EnterPage(yl::string("disposition_code"), &nCallID);
}

void CDlgNetConfUI::ShowEmergencyMenu(int nCallID)
{
    SettingUI_EnterPage(yl::string("emergency_escalation"), &nCallID);
}

void CDlgNetConfUI::DealPool()
{
    ProcessNormalOpenPool();
}

void CDlgNetConfUI::ShowDealLastNote()
{
    _TalkUIFrameManager.UpdateMsgboxNoteNotify(_UILOGIC_LANG_TRAN(TRID_DEAL_LAST_ACTION));
}

bool CDlgNetConfUI::ShowErrorNote(CCallInfo * pCallInfo)
{
    return _TalkUIFrameManager.ShowErrorNote(pCallInfo);
}

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgNetConfUI::RefreshTalkUIData()
{
    CNetConfUIData * pNetConfUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pNetConfUIData = m_pProcessor->GetNetConfUIData();
    }

    if (NULL == pNetConfUIData)
    {
        return;
    }

    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_CCSTATUS, true);
    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_DTMF, true);
    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_NETWORK, true);

    //顺序不能随意调整
    //原因是m_bShowContactImage的值要先获取
    ResetWidget();

#if IF_NETCONF_UI_STYLE
    SetConfInfo(pNetConfUIData);
#else
    SetConfList(pNetConfUIData);
    SetMemberHint(pNetConfUIData);
#endif

    SetStatusInfo(pNetConfUIData);

    SetIncomingInfo(pNetConfUIData);

    UpdateWnd();

    _TalkConflictManager.checkShowUnit();

#if IF_HD_ICON_DEPART
    SetHDIconInfo(pNetConfUIData);
#endif
}

#if IF_NETCONF_UI_STYLE
void CDlgNetConfUI::SetConfInfo(CNetConfUIData * pNetConfUIData)
{
    yl::string strName = GetMemberHint(pNetConfUIData);
    yl::string strNumber = "";

    m_talkWidget.SetBaseContactInfo(strName, strNumber);
}
#endif

void CDlgNetConfUI::SetStatusInfo(CNetConfUIData * pNetConfUIData)
{
    bool bHoldMuteInfo = false;

    yl::string strHoldMuteIcon = "";
    yl::string strHoldMuteTip = "";

    //failedinfo
    if (pNetConfUIData->m_TalkingUIData.m_bTransferFailed)
    {
        bHoldMuteInfo = true;

        strHoldMuteTip = _UILOGIC_LANG_TRAN(TRID_TRANSFER_FAILED);
    }
    else if (pNetConfUIData->m_TalkingUIData.m_bCallParkFailed)
    {
        bHoldMuteInfo = true;

#if !IF_TALKUI_TRANS_FAIL_INFO_IN_ONE_LINE
        if (m_talkWidget.GetIsShowContact())
        {
            strHoldMuteTip = pNetConfUIData->m_TalkingUIData.m_strParkFailedInfoFirst;

            if (!pNetConfUIData->m_TalkingUIData.m_strParkFailedInfoSecond.empty())
            {
                strHoldMuteTip = strHoldMuteTip + ":\n";
                strHoldMuteTip = strHoldMuteTip + pNetConfUIData->m_TalkingUIData.m_strParkFailedInfoSecond;
            }
        }
        else
#endif
        {
            strHoldMuteTip = pNetConfUIData->m_TalkingUIData.m_strParkFailedInfo;
        }
    }
#if IF_TALKUI_PSTN
    //pstn flash优先级低
    else if (pNetConfUIData->m_TalkingUIData.m_bPstnFlashing)
    {
        bHoldMuteInfo = true;

        strHoldMuteTip = _UILOGIC_LANG_TRAN(TRID_FLASHING);
    }
#endif
    else if (pNetConfUIData->m_TalkingUIData.m_bLocalHold)
    {
        bHoldMuteInfo = true;
        if (m_talkWidget.GetIsShowContact())
        {
            //显示联系人头像
            strHoldMuteIcon = PIC_TALK_HOLD;
        }
        else    //不显示联系人头像
        {
            strHoldMuteIcon = PIC_TALK_HOLD_SMALL;
        }
        strHoldMuteTip = _UILOGIC_LANG_TRAN(TRID_HOLD);
    }
    else if (pNetConfUIData->m_TalkingUIData.m_bRemoteHold)
    {
        bHoldMuteInfo = true;
        if (m_talkWidget.GetIsShowContact())
        {
            //显示联系人头像
            strHoldMuteIcon = PIC_TALK_HELD;
        }
        else    //不显示联系人头像
        {
            strHoldMuteIcon = PIC_TALK_HELD_SMALL;
        }
        strHoldMuteTip = _UILOGIC_LANG_TRAN(TRID_HELD);
    }
    else if (pNetConfUIData->m_TalkingUIData.m_bAudioMute)
    {
        bHoldMuteInfo = true;
        if (m_talkWidget.GetIsShowContact())
        {
            //显示联系人头像
            strHoldMuteIcon = PIC_TALK_MUTE;
        }
        else    //不显示联系人头像
        {
            strHoldMuteIcon = PIC_TALK_MUTE_SMALL;
        }
        strHoldMuteTip = _UILOGIC_LANG_TRAN(TRID_MUTE);
    }

    if (bHoldMuteInfo)
    {
        m_talkWidget.SetStatusInfo(strHoldMuteIcon, strHoldMuteTip);
    }

#if IF_TALKUI_CONFLICT_STYLE
    if (bHoldMuteInfo)
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_STATUS_INFO, true);
    }
    else
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_STATUS_INFO, false);
    }
#endif
}

#if !IF_NETCONF_UI_STYLE
void CDlgNetConfUI::SetConfList(CNetConfUIData * pNetConfUIData)
{
    talkui_vector<yl::string> listMemberForShow;
    int nCount = 0;
    talkui_vector<yl::string>::iterator iter = pNetConfUIData->m_listNetUserInfo.begin();

    //可添加的数据量为不超过一页
    for (
        ; nCount < m_nListPageCount, pNetConfUIData->m_listNetUserInfo.end() != iter
        ; ++nCount, ++iter)
    {
        listMemberForShow.push_back(*iter);
    }

    m_objNetConfListAdapter.SetNetConfListData(pNetConfUIData->m_listNetUserInfo);
}

void CDlgNetConfUI::SetMemberHint(CNetConfUIData * pNetConfUIData)
{
    if (NULL != m_pMemberHint)
    {
        m_pMemberHint->show();
        m_pMemberHint->SetText(GetMemberHint(pNetConfUIData));
        _TalkConflictManager.setConflictUnitShow(CONFLICT_NET_CONF_MEMBER_HINT, true);
    }
}
#endif

void CDlgNetConfUI::SetIncomingInfo(CNetConfUIData * pNetConfUIData)
{
    // http://10.2.1.199/Bug.php?BugID=75691
    // 添加新来电提醒
    //有新来电  且  来电号码不为空
    if (pNetConfUIData->m_TalkingUIData.m_bHasCallComming && (!pNetConfUIData->m_TalkingUIData.m_strIncomingCallNum.empty()))
    {
        yl::string strTargetInfo = "";
        if (NULL != m_pProcessor)
        {
            strTargetInfo = m_pProcessor->GetTargetInfo();
        }

        m_talkWidget.SetIncomingInfo(pNetConfUIData->m_TalkingUIData.m_strIncomingCallNum, strTargetInfo);

#if IF_TALKUI_CONFLICT_STYLE || !IF_NETCONF_UI_STYLE
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_INCOMING_INFO, true);
#endif
    }
#if IF_TALKUI_CONFLICT_STYLE || !IF_NETCONF_UI_STYLE
    else
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_INCOMING_INFO, false);
    }
#endif
}

#if IF_HD_ICON_DEPART
void CDlgNetConfUI::SetHDIconInfo(CNetConfUIData * pNetConfUIData)
{
    if (_TalkConflictManager.getVolumebarVisible())
    {
        m_talkWidget.SetHDIconInfo(ACT_NONE);
        return;
    }

    AUDIO_CODEC_TYPE eAudioCodec = pNetConfUIData->m_TalkingUIData.m_eAudioCodec;
    TALKUI_INFO("CDlgNetConfUI::SetHDIconInfo AudioCodec Is [%d]", eAudioCodec);
    m_talkWidget.SetHDIconInfo(eAudioCodec);
}
#endif

yl::string CDlgNetConfUI::GetMemberHint(CNetConfUIData * pNetConfUIData)
{
    yl::string strHint = "";

    int nTotalCount = pNetConfUIData->m_listNetUserInfo.size();

    char szTotalCount[8] = { 0 };
    sprintf(szTotalCount, "%d", nTotalCount);
    strHint = yl::string(szTotalCount) + " " + _UILOGIC_LANG_TRAN(TRID_LITTLE_MEMBERS);

    return strHint;
}

void CDlgNetConfUI::UpdateTalkUITime(uint iHour, uint iMin, uint iSecond)
{
    CBaseTalkUIProcessor* pProcessor = GetTalkUIProcessor();
    if (NULL != pProcessor)
    {
        m_talkWidget.UpdateTalkUITime(pProcessor->GetTalkTime());
    }
}

#if IF_NETCONF_UI_STYLE
void CDlgNetConfUI::showStatusHint(bool bShow)
{
    m_talkWidget.showStatusHint(bShow);
}
#else
void CDlgNetConfUI::showMemberHint(bool bShow)
{
    if (NULL != m_pMemberHint)
    {
        if (bShow)
        {
            m_pMemberHintFrame->show();
        }
        else
        {
            m_pMemberHintFrame->hide();
        }
    }
}
#endif

void CDlgNetConfUI::showTimer(bool bShow)
{
    m_talkWidget.showTimer(bShow);
}

void CDlgNetConfUI::showIncomingHint(bool bShow)
{
    m_talkWidget.showIncomingHint(bShow);
}

}       //namespace talkui


