#include "dlgconfui.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"
#include "talkui_define.h"
#include "talkuicommonfunction.h"
#include "talkuiframemanager.h"
#include "talkuixmlitemname.h"
#include "conflictmanager.h"
#include "talkuifactory.h"
#include "talkuixmlpath.h"

////////////////// talkuilogic /////////////////////
#include "talkuilogic/talkuilogicdelegate.h"

#include "settingui/include/modsettingui.h"

namespace talkui
{

void confShowIncomingHint(bool bShow)
{
    CDlgConfUI* pConfUI = (CDlgConfUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_CONF);
    if (NULL != pConfUI)
    {
        pConfUI->showIncomingHint(bShow);
    }
}

void confShowStatusHint(bool bShow)
{
    CDlgConfUI* pConfUI = (CDlgConfUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_CONF);
    if (NULL != pConfUI)
    {
        pConfUI->showStatusHint(bShow);
    }
}

void confShowTimer(bool bShow)
{
    CDlgConfUI* pConfUI = (CDlgConfUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_CONF);
    if (NULL != pConfUI)
    {
        pConfUI->showTimer(bShow);
    }
}

CDlgConfUI::CDlgConfUI()
    : CDlgBaseTalkUI(DLG_ConfUI, eWT_Talk)
{

}

CDlgConfUI::~CDlgConfUI()
{

}


void CDlgConfUI::LoadLayout()
{
    loadContent(TALKUI_DLGCONFUI_XML);
}

void CDlgConfUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_talkWidget.SetNormalHintWidget(static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_1))
                                     , static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_2)));

    m_talkWidget.SetTimeWidget(static_cast<xTimerPieceView*>(getViewById(TALKUI_TALK_TALK_TIME)));

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetContactWidget(static_cast<xFrameLayout *>(getViewById(TALKUI_TALK_CONTACT_IMAGE_FRAME))
                                  , static_cast<xImageView *>(getViewById(TALKUI_TALK_CONTACT_IMAGE)));

    m_talkWidget.SetContactStatusWidget(static_cast<xImageView *>(getViewById(TALKUI_TALK_CONTACT_COVER))
                                        , static_cast<xImageView *>(getViewById(TALKUI_TALK_STATUS_FRAME))
                                        , static_cast<xImageView *>(getViewById(TALKUI_TALK_STATUS_ICON))
                                        , static_cast<xTextView *>(getViewById(TALKUI_TALK_STATUS_HINT)));
#endif

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
}

void CDlgConfUI::ResetWidget()
{
    m_talkWidget.ResetWidget();
}

// 初始化
void CDlgConfUI::Init()
{
    CDlgBaseTalkUI::Init();

    CONFLICT_UNIT stUnit;

#if IF_TALKUI_CONFLICT_STYLE
    stUnit.m_ShowFunction = confShowIncomingHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_INCOMING_INFO, stUnit);

    stUnit.m_ShowFunction = confShowStatusHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_STATUS_INFO, stUnit);

    stUnit.m_bShow = true;
    stUnit.m_ShowFunction = confShowTimer;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_TIME, stUnit);
#endif
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgConfUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

#if IF_TALKUI_CONFLICT_STYLE
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_INCOMING_INFO);
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_STATUS_INFO);
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_TIME);
#endif
}

// 刷新窗口
void CDlgConfUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

//获取 processor
CBaseTalkUIProcessor * CDlgConfUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgConfUI::IsShowDtmfLine() const
{
    return true;
}

// 是否显示切换话路的滚动条
bool CDlgConfUI::IsShowSessionScrollBar()
{
    return true;
}

// 能否切换Session
bool CDlgConfUI::IsChangeSession(int iKey)
{
    return true;
}

bool CDlgConfUI::OnKeyPreProcess(int iKey)
{
#if IF_HD_ICON_DEPART
    if (NULL != m_pProcessor)
    {

        CConfUIData * pConfUIData = m_pProcessor->GetConfData();
        if (NULL != pConfUIData)
        {
            SetHDIconInfo(pConfUIData);
        }
    }
#endif

    return CDlgBaseTalkUI::OnKeyPreProcess(iKey);
}


void CDlgConfUI::OnTalkVolumnVisibleChange(bool bVisible)
{
#if IF_HD_ICON_DEPART
    if (NULL != m_pProcessor)
    {

        CConfUIData * pConfUIData = m_pProcessor->GetConfData();
        if (NULL != pConfUIData)
        {
            SetHDIconInfo(pConfUIData);
        }
    }
#endif

    CDlgBaseTalkUI::OnTalkVolumnVisibleChange(bVisible);
}

////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

////////////////////////////////继承自  CUILogicConfUIBase /////////////////////
//设置UILogic 数据
void CDlgConfUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{
    //不做处理  统一放到RefreshTalkUIData处理
}

int CDlgConfUI::GetFocusCallID()
{
    return -1;
}

void CDlgConfUI::ShowSecurityBox(int nAccount)
{
    ShowSecurityMessageBox(nAccount);
}

void CDlgConfUI::ProcessSecurityCallBack(const yl::string & strResult)
{
    CBaseTalkUIProcessor * pProcessor = GetTalkUIProcessor();

    if (pProcessor != NULL)
    {
        pProcessor->DealSecurityResult(strResult);
    }
}

void CDlgConfUI::ShowACDDispCodeMenu(int nCallID)
{
    SettingUI_EnterPage(yl::string("disposition_code"), &nCallID);
}

void CDlgConfUI::ShowEmergencyMenu(int nCallID)
{
    SettingUI_EnterPage(yl::string("emergency_escalation"), &nCallID);
}

void CDlgConfUI::ShowDealLastNote()
{
    _TalkUIFrameManager.UpdateMsgboxNoteNotify(_UILOGIC_LANG_TRAN(TRID_DEAL_LAST_ACTION));
}

bool CDlgConfUI::ShowErrorNote(CALL_RECORD_ACTION_RESULT eFailResult)
{
    return _TalkUIFrameManager.ShowErrorNote(eFailResult);
}

////////////////////////////////继承自  CUILogicConfUIBase 结束 /////////////////////

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgConfUI::RefreshTalkUIData()
{
    CConfUIData * pConfUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pConfUIData = m_pProcessor->GetConfData();
    }

    if (NULL == pConfUIData)
    {
        return;
    }

    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_CCSTATUS, true);
    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_DTMF, true);
    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_NETWORK, true);

    //顺序不能随意调整
    //原因是m_bShowContactImage的值要先获取
    ResetWidget();

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetConfPhoto();
#endif
    SetContactInfo(pConfUIData);
    SetStatusInfo(pConfUIData);
    SetIncomingInfo(pConfUIData);

    UpdateWnd();

    _TalkConflictManager.checkShowUnit();

#if IF_HD_ICON_DEPART
    SetHDIconInfo(pConfUIData);
#endif
}

void CDlgConfUI::SetContactInfo(CConfUIData * pConfUIData)
{
    yl::string strNameFirst = "";
    yl::string strNameSecond = "";

    bool bShowMembersName = false;

    if (pConfUIData->listContactInfo.size() <= 2
#if IF_CONF_SECURITY_IN_TITLE
            && !pConfUIData->bAllowSecurity
#endif
       )
    {
        YLList<CContactInfo>::iterator listIterator = pConfUIData->listContactInfo.begin();
        if (pConfUIData->listContactInfo.end() != listIterator)
        {
            CContactInfo & objContactFirst = *listIterator;
            strNameFirst = objContactFirst.strName;

            //获取第二个 联系人名称
            listIterator++;
            if (pConfUIData->listContactInfo.end() != listIterator)
            {
                CContactInfo & objContactInfoSecond = *listIterator;
                strNameSecond = objContactInfoSecond.strName;
            }
        }

        bShowMembersName = true;
    }
    else
    {
        strNameFirst = _UILOGIC_LANG_TRAN(TRID_CONFERENCE);

        char szConfMemberCount[8] = { 0 };
        sprintf(szConfMemberCount, "%d", pConfUIData->listContactInfo.size());
        strNameSecond = szConfMemberCount;
        strNameSecond += " ";
        strNameSecond += _UILOGIC_LANG_TRAN(TRID_MEMBERS);

        bShowMembersName = false;
    }

#if IF_CONF_SECURITY_IN_TITLE
    //security显示在title上
#else
    if (pConfUIData->bAllowSecurity)
    {
        ChangeNameAndNumberBySecurityLevel(strNameFirst
                                           , strNameSecond
                                           , pConfUIData->strSecurityLevel
                                           , !m_talkWidget.bHaveTwoHint());
    }
#endif

    m_talkWidget.SetBaseContactInfo(strNameFirst, strNameSecond);

#if IF_TALKUI_FONT_SIZE_CHANGE
    if (!bShowMembersName)
    {
        m_talkWidget.SetNameFontSize(RINGINGUI_FONT_SIZE_BIG, RINGINGUI_MARGIN_BOTTOM_SMALL);
    }
    else
    {
        m_talkWidget.SetNameFontSize(RINGINGUI_FONT_SIZE_SMALL, RINGINGUI_MARGIN_BOTTOM_SMALL);
    }
#endif
}

void CDlgConfUI::SetStatusInfo(CConfUIData * pConfUIData)
{
    bool bHoldMuteInfo = false;

    yl::string strHoldMuteIcon = "";
    yl::string strHoldMuteTip = "";

    if (pConfUIData->bHoldConf)
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
    else if (pConfUIData->bRemoteHold)
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
    else if (_TalkUILogicDelegate.GetAudioMuteStatus())     //音频mute
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

void CDlgConfUI::SetIncomingInfo(CConfUIData * pConfUIData)
{
    // http://10.2.1.199/Bug.php?BugID=75691
    // 添加新来电提醒
    //有新来电  且  来电号码不为空
    if (pConfUIData->bHasCallComming && (!pConfUIData->strIncomingCallNum.empty()))
    {
        yl::string strTargetInfo = "";
        if (NULL != m_pProcessor)
        {
            strTargetInfo = m_pProcessor->GetTargetInfo();
        }

        m_talkWidget.SetIncomingInfo(pConfUIData->strIncomingCallNum, strTargetInfo);

#if IF_TALKUI_CONFLICT_STYLE
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_INCOMING_INFO, true);
#endif
    }
#if IF_TALKUI_CONFLICT_STYLE
    else
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_INCOMING_INFO, false);
    }
#endif
}

#if IF_HD_ICON_DEPART
void CDlgConfUI::SetHDIconInfo(CConfUIData * pConfUIData)
{
    if (_TalkConflictManager.getVolumebarVisible())
    {
        m_talkWidget.SetHDIconInfo(ACT_NONE);
        return;
    }

    AUDIO_CODEC_TYPE eAudioCodec = pConfUIData->eAudioCodec;
    TALKUI_INFO("CDlgConfUI::SetHDIconInfo AudioCodec Is [%d]", eAudioCodec);
    m_talkWidget.SetHDIconInfo(eAudioCodec);
}
#endif

void CDlgConfUI::UpdateTalkUITime(uint iHour, uint iMin, uint iSecond)
{
    CBaseTalkUIProcessor* pProcessor = GetTalkUIProcessor();
    if (NULL != pProcessor)
    {
        m_talkWidget.UpdateTalkUITime(pProcessor->GetTalkTime());
    }
}

void CDlgConfUI::showIncomingHint(bool bShow)
{
    m_talkWidget.showIncomingHint(bShow);
}

void CDlgConfUI::showStatusHint(bool bShow)
{
    m_talkWidget.showStatusHint(bShow);
}

void CDlgConfUI::showTimer(bool bShow)
{
    m_talkWidget.showTimer(bShow);
}

}       //namespace talkui


