#include "dlgtalkingui.h"
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

void talkingShowIncomingHint(bool bShow)
{
    CDlgTalkingUI* pTalkingUI = (CDlgTalkingUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_TALKING);
    if (NULL != pTalkingUI)
    {
        pTalkingUI->showIncomingHint(bShow);
    }
}

void talkingShowTransFailHint(bool bShow)
{
    CDlgTalkingUI* pTalkingUI = (CDlgTalkingUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_TALKING);
    if (NULL != pTalkingUI)
    {
        pTalkingUI->showTransFailHint(bShow);
    }
}

void talkingShowStatusHint(bool bShow)
{
    CDlgTalkingUI* pTalkingUI = (CDlgTalkingUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_TALKING);
    if (NULL != pTalkingUI)
    {
        pTalkingUI->showStatusHint(bShow);
    }
}

void talkingShowTimer(bool bShow)
{
    CDlgTalkingUI* pTalkingUI = (CDlgTalkingUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_TALKING);
    if (NULL != pTalkingUI)
    {
        pTalkingUI->showTimer(bShow);
    }
}

CDlgTalkingUI::CDlgTalkingUI()
    : CDlgBaseTalkUI(DLG_TalkingUI, eWT_Talk)
#if IF_TRANSFER_FAIL_HINT_STYLE
    , m_pTransFailHint(NULL)
#endif
{
}

CDlgTalkingUI::~CDlgTalkingUI()
{

}

void CDlgTalkingUI::LoadLayout()
{
    loadContent(TALKUI_DLGTALKINGUI_XML);
}

void CDlgTalkingUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_talkWidget.SetNormalHintWidget(static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_1))
                                     , static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_2)));

    m_talkWidget.SetTimeWidget(static_cast<xTimerPieceView*>(getViewById(TALKUI_TALK_TALK_TIME)));

#if IF_TRANSFER_FAIL_HINT_STYLE
    m_pTransFailHint = static_cast<xTextView*>(getViewById(TALKUI_TALKING_TRANSFAIL_HINT));
#endif

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetContactWidget(static_cast<xFrameLayout *>(getViewById(TALKUI_TALK_CONTACT_IMAGE_FRAME))
                                  , static_cast<xImageView *>(getViewById(TALKUI_TALK_CONTACT_IMAGE)));

    m_talkWidget.SetContactStatusWidget(static_cast<xImageView *>(getViewById(TALKUI_TALK_CONTACT_COVER))
                                        , static_cast<xImageView *>(getViewById(TALKUI_TALK_STATUS_FRAME))
                                        , static_cast<xImageView *>(getViewById(TALKUI_TALK_STATUS_ICON))
                                        , static_cast<xTextView *>(getViewById(TALKUI_TALK_STATUS_HINT)));
#endif

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    m_talkWidget.SetContactMask(static_cast<xImageView *>(getViewById(TALKUI_TALK_CONTACT_MASK)));
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

void CDlgTalkingUI::ResetWidget()
{
    m_talkWidget.ResetWidget();

#if IF_TRANSFER_FAIL_HINT_STYLE
    if (NULL != m_pTransFailHint)
    {
        m_pTransFailHint->SetText("");
        m_pTransFailHint->hide(true);
    }
#endif
}

// 初始化
void CDlgTalkingUI::Init()
{
    CDlgBaseTalkUI::Init();

    CONFLICT_UNIT stUnit;

#if IF_TALKUI_CONFLICT_STYLE
    stUnit.m_ShowFunction = talkingShowIncomingHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_INCOMING_INFO, stUnit);

    stUnit.m_ShowFunction = talkingShowStatusHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_STATUS_INFO, stUnit);

    stUnit.m_bShow = true;
    stUnit.m_ShowFunction = talkingShowTimer;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_TIME, stUnit);
#endif

#if IF_TRANSFER_FAIL_HINT_STYLE
    stUnit.m_bShow = false;
    stUnit.m_ShowFunction = talkingShowTransFailHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_TRANSFAIL_INFO, stUnit);
#endif
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgTalkingUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

#if IF_TALKUI_CONFLICT_STYLE
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_INCOMING_INFO);
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_STATUS_INFO);
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_TIME);
#endif

#if IF_TRANSFER_FAIL_HINT_STYLE
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_TRANSFAIL_INFO);
#endif
}

// 刷新窗口
void CDlgTalkingUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

//获取 processor
CBaseTalkUIProcessor * CDlgTalkingUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgTalkingUI::IsShowDtmfLine() const
{
    return true;
}

// 是否显示切换话路的滚动条
bool CDlgTalkingUI::IsShowSessionScrollBar()
{
    return true;
}

// 能否切换Session
bool CDlgTalkingUI::IsChangeSession(int iKey)
{
    return true;
}


bool CDlgTalkingUI::OnKeyPreProcess(int iKey)
{
#if IF_HD_ICON_DEPART
    if (NULL != m_pProcessor)
    {
        CTalkingUIData * pTalkingUIData = m_pProcessor->GetTalkingUIData();
        if (NULL != pTalkingUIData)
        {
            SetHDIconInfo(pTalkingUIData);
        }
    }
#endif

    return CDlgBaseTalkUI::OnKeyPreProcess(iKey);
}


void CDlgTalkingUI::OnTalkVolumnVisibleChange(bool bVisible)
{
#if IF_HD_ICON_DEPART
    if (NULL != m_pProcessor)
    {
        CTalkingUIData * pTalkingUIData = m_pProcessor->GetTalkingUIData();
        if (NULL != pTalkingUIData)
        {
            SetHDIconInfo(pTalkingUIData);
        }
    }
#endif

    CDlgBaseTalkUI::OnTalkVolumnVisibleChange(bVisible);
}

//设置UILogic 数据
void CDlgTalkingUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{
    //不做处理  统一放到RefreshTalkUIData处理
}

void CDlgTalkingUI::ShowSecurityBox(int nAccount)
{
    ShowSecurityMessageBox(nAccount);
}

void CDlgTalkingUI::ProcessSecurityCallBack(const yl::string & strResult)
{
    CBaseTalkUIProcessor * pProcessor = GetTalkUIProcessor();

    if (pProcessor != NULL)
    {
        pProcessor->DealSecurityResult(strResult);
    }
}

void CDlgTalkingUI::ShowACDDispCodeMenu(int nCallID)
{
    SettingUI_EnterPage(yl::string("disposition_code"), &nCallID);
}

void CDlgTalkingUI::ShowEmergencyMenu(int nCallID)
{
    SettingUI_EnterPage(yl::string("emergency_escalation"), &nCallID);
}

void CDlgTalkingUI::DealPool()
{
    ProcessNormalOpenPool();
}

void CDlgTalkingUI::ShowDealLastNote()
{
    _TalkUIFrameManager.UpdateMsgboxNoteNotify(_UILOGIC_LANG_TRAN(TRID_DEAL_LAST_ACTION));
}

bool CDlgTalkingUI::ShowErrorNote(CCallInfo * pCallInfo)
{
    return _TalkUIFrameManager.ShowErrorNote(pCallInfo);
}

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgTalkingUI::RefreshTalkUIData()
{
    CTalkingUIData * pTalkingUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pTalkingUIData = m_pProcessor->GetTalkingUIData();
    }

    if (NULL == pTalkingUIData)
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
    m_talkWidget.SetPhotoInfo(pTalkingUIData->m_objContactInfo.strContactImage);
#endif
    SetContactInfo(pTalkingUIData);
    SetStatusInfo(pTalkingUIData);
    SetIncomingInfo(pTalkingUIData);

    UpdateWnd();

    _TalkConflictManager.checkShowUnit();

#if IF_HD_ICON_DEPART
    SetHDIconInfo(pTalkingUIData);
#endif
}

void CDlgTalkingUI::SetContactInfo(CTalkingUIData * pTalkingUIData)
{
    yl::string strName = pTalkingUIData->m_objContactInfo.strName;
    yl::string strNumber = pTalkingUIData->m_objContactInfo.strNumber;

    if (pTalkingUIData->m_bSecurity)
    {
        ChangeNameAndNumberBySecurityLevel(strName
                                           , strNumber
                                           , pTalkingUIData->m_objContactInfo.strSecurityLevel
                                           , !m_talkWidget.bHaveTwoHint());
    }

    m_talkWidget.SetBaseContactInfo(strName, strNumber);

#if IF_TALKUI_FONT_SIZE_CHANGE
#if IF_TALKUI_FONT_SIZE_CHANGE_WHEN_TWO
    if (strNumber.empty())
    {
        m_talkWidget.SetNameFontSize(RINGINGUI_FONT_SIZE_BIG, RINGINGUI_MARGIN_BOTTOM_SMALL);
    }
    else
    {
        m_talkWidget.SetNameFontSize(RINGINGUI_FONT_SIZE_SMALL, RINGINGUI_MARGIN_BOTTOM_SMALL);
    }
#endif
#endif
}

void CDlgTalkingUI::SetStatusInfo(CTalkingUIData * pTalkingUIData)
{
    bool bHoldMuteInfo = false;
    bool bTransferFail = false;

    yl::string strHoldMuteIcon = "";
    yl::string strHoldMuteTip = "";

    //failedinfo
    if (pTalkingUIData->m_bTransferFailed)
    {
        bHoldMuteInfo = true;
        bTransferFail = true;

        strHoldMuteTip = _UILOGIC_LANG_TRAN(TRID_TRANSFER_FAILED);
    }
    else if (pTalkingUIData->m_bCallParkFailed)
    {
        bHoldMuteInfo = true;
        bTransferFail = true;

#if !IF_TALKUI_TRANS_FAIL_INFO_IN_ONE_LINE
        if (m_talkWidget.GetIsShowContact())
        {
            strHoldMuteTip = pTalkingUIData->m_strParkFailedInfoFirst;

            if (!pTalkingUIData->m_strParkFailedInfoSecond.empty())
            {
                strHoldMuteTip = strHoldMuteTip + ":\n";
                strHoldMuteTip = strHoldMuteTip + pTalkingUIData->m_strParkFailedInfoSecond;
            }
        }
        else
#endif
        {
            strHoldMuteTip = pTalkingUIData->m_strParkFailedInfo;
        }
    }
#if IF_TALKUI_PSTN
    //pstn flash优先级低
    else if (pTalkingUIData->m_bPstnFlashing)
    {
        bHoldMuteInfo = true;

        strHoldMuteTip = _UILOGIC_LANG_TRAN(TRID_FLASHING);
    }
#endif
    else if (pTalkingUIData->m_bLocalHold)
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
    else if (pTalkingUIData->m_bRemoteHold)
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
    else if (pTalkingUIData->m_bAudioMute)
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

    if (bTransferFail)
    {
#if IF_TRANSFER_FAIL_HINT_STYLE
        if (bTransferFail)
        {
            if (m_talkWidget.GetIsShowContact())
            {
                //仅有显示联系人头像的情况下需要
                strHoldMuteIcon = PIC_TALK_HOLD;
                m_talkWidget.SetStatusInfo(strHoldMuteIcon, "");
            }

            if (NULL != m_pTransFailHint)
            {
                m_pTransFailHint->SetText(strHoldMuteTip);
            }

            _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_TRANSFAIL_INFO, true);
        }
#else
        m_talkWidget.SetStatusInfo("", strHoldMuteTip);
#endif
    }
    else
    {
#if IF_TRANSFER_FAIL_HINT_STYLE
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_TRANSFAIL_INFO, false);
#endif

        if (bHoldMuteInfo)
        {
            m_talkWidget.SetStatusInfo(strHoldMuteIcon, strHoldMuteTip);
        }
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

void CDlgTalkingUI::SetIncomingInfo(CTalkingUIData * pTalkingUIData)
{
    // http://10.2.1.199/Bug.php?BugID=75691
    // 添加新来电提醒
    //有新来电  且  来电号码不为空
    if (pTalkingUIData->m_bHasCallComming && (!pTalkingUIData->m_strIncomingCallNum.empty()))
    {
        yl::string strTargetInfo = "";
        if (NULL != m_pProcessor)
        {
            strTargetInfo = m_pProcessor->GetTargetInfo();
        }

        m_talkWidget.SetIncomingInfo(pTalkingUIData->m_strIncomingCallNum, strTargetInfo);

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
void CDlgTalkingUI::SetHDIconInfo(CTalkingUIData * pTalkingUIData)
{
    if (_TalkConflictManager.getVolumebarVisible())
    {
        m_talkWidget.SetHDIconInfo(ACT_NONE);
        return;
    }

    AUDIO_CODEC_TYPE eAudioCodec = pTalkingUIData->m_eAudioCodec;
    TALKUI_INFO("CDlgTalkingUI::SetHDIconInfo AudioCodec Is [%d]", eAudioCodec);
    m_talkWidget.SetHDIconInfo(eAudioCodec);
}
#endif

void CDlgTalkingUI::UpdateTalkUITime(uint iHour, uint iMin, uint iSecond)
{
    CBaseTalkUIProcessor* pProcessor = GetTalkUIProcessor();
    if (NULL != pProcessor)
    {
        m_talkWidget.UpdateTalkUITime(pProcessor->GetTalkTime());
    }
}

void CDlgTalkingUI::showStatusHint(bool bShow)
{
    m_talkWidget.showStatusHint(bShow);
}

void CDlgTalkingUI::showTransFailHint(bool bShow)
{
#if IF_TRANSFER_FAIL_HINT_STYLE
    if (NULL != m_pTransFailHint)
    {
        if (bShow)
        {
            m_pTransFailHint->show();
        }
        else
        {
            m_pTransFailHint->hide(true);
        }
    }
#endif
}

void CDlgTalkingUI::showTimer(bool bShow)
{
    m_talkWidget.showTimer(bShow);
}

void CDlgTalkingUI::showIncomingHint(bool bShow)
{
    m_talkWidget.showIncomingHint(bShow);
}

}       //namespace talkui


