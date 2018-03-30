#include "dlgringui.h"

#include "talkuixmlitemname.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"

////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicdelegate.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "conflictmanager.h"
#include "talkuifactory.h"

namespace talkui
{

void ringShowRingingTarget(bool bShow)
{
    CDlgRingUI* pRingingUI = (CDlgRingUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_RING);
    if (NULL != pRingingUI)
    {
        pRingingUI->showRingingHint(bShow);
    }
}

CDlgRingUI::CDlgRingUI()
    : CDlgBaseTalkUI(DLG_RingUI, eWT_Talk)
    , m_pRingingInfoFrame(NULL)
    , m_pRingingArrow(NULL)
    , m_pRingingTarget(NULL)
#if IF_TALKUI_RINGUI_TARGET_SPLIT
    , m_pRingingTargetNumber(NULL)
#endif
    , m_pRingingArrowRight(NULL)
    , m_pRingingContactAnimation(NULL)
    , m_pRingingAnimation(NULL)
    , m_pMuteIcon(NULL)
{
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (PT_T19 == ePhoneType
            || PT_T41 == ePhoneType
            || PT_T42 == ePhoneType
            || PT_T43S == ePhoneType
            || PT_T43P == ePhoneType
       )
    {
        m_eTitleType = TALKUI_TITLE_TYPE_NO_TITLE;
    }
}

CDlgRingUI::~CDlgRingUI()
{

}


void CDlgRingUI::LoadLayout()
{
    loadContent(TALKUI_DLGRINGINGUI_XML);
}

void CDlgRingUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_talkWidget.SetNormalHintWidget(static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_1))
                                     , static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_2)));

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetContactWidget(static_cast<xFrameLayout *>(getViewById(TALKUI_TALK_CONTACT_IMAGE_FRAME))
                                  , static_cast<xImageView *>(getViewById(TALKUI_TALK_CONTACT_IMAGE)));
#endif

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    m_talkWidget.SetContactMask(static_cast<xImageView *>(getViewById(TALKUI_TALK_CONTACT_MASK)));
#endif

    m_pRingingInfoFrame = static_cast<xView *>(getViewById(TALKUI_RINGING_INFO_FRAME));
    m_pRingingArrow = static_cast<xImageView *>(getViewById(TALKUI_RINGING_ARROW));
    m_pRingingTarget = static_cast<xTextView *>(getViewById(TALKUI_RINGING_TARGET));
#if IF_TALKUI_RINGUI_TARGET_SPLIT
    m_pRingingTargetNumber = static_cast<xTextView *>(getViewById(TALKUI_RINGING_TARGET_NUMBER));
#endif
    m_pRingingArrowRight = static_cast<xImageView *>(getViewById(TALKUI_RINGING_ARROW_RIGHT));

    m_pMuteIcon = static_cast<xImageView *>(getViewById(TALKUI_RINGING_MUTE_ICON));

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    m_pRingingContactAnimation = static_cast<xAnimationBox*>(getViewById(TALKUI_RINGING_CONTACT_ANIMATION));

    if (NULL != m_pRingingContactAnimation)
    {
        LIST_STRING listIconPath;
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING0);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING1);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING2);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING3);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING4);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING5);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING6);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING7);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING8);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING9);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING10);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING11);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_RINGING12);

        m_pRingingContactAnimation->SetAnimation(listIconPath);
    }
#endif

    m_pRingingAnimation = static_cast<xAnimationBox*>(getViewById(TALKUI_RINGING_ANIMATION));

    if (NULL != m_pRingingAnimation)
    {
        LIST_STRING listIconPath;
        listIconPath.push_back(PIC_TALK_ANIMATION_RINGING1);
        listIconPath.push_back(PIC_TALK_ANIMATION_RINGING2);
        listIconPath.push_back(PIC_TALK_ANIMATION_RINGING3);
#if !IF_TALKUI_CONTACT_PHOTO
        listIconPath.push_back(PIC_TALK_ANIMATION_RINGING4);
#endif

        m_pRingingAnimation->SetAnimation(listIconPath);
    }
}

void CDlgRingUI::ResetWidget()
{
    m_talkWidget.ResetWidget();

    if (NULL != m_pRingingInfoFrame)
    {
        m_pRingingInfoFrame->hide(true);
    }
    if (NULL != m_pRingingArrow)
    {
        m_pRingingArrow->hide(true);
    }
    if (NULL != m_pRingingTarget)
    {
        m_pRingingTarget->hide(true);
        m_pRingingTarget->SetText("");
        m_pRingingTarget->EnableScroll(true);
    }
#if IF_TALKUI_RINGUI_TARGET_SPLIT
    if (NULL != m_pRingingTargetNumber)
    {
        m_pRingingTargetNumber->hide(true);
        m_pRingingTargetNumber->SetText("");
        m_pRingingTargetNumber->EnableScroll(true);
    }
#endif
    if (NULL != m_pRingingArrowRight)
    {
        m_pRingingArrowRight->hide(true);
    }

    if (NULL != m_pRingingContactAnimation)
    {
        m_pRingingContactAnimation->hide(true);
    }
    if (NULL != m_pRingingAnimation)
    {
        m_pRingingAnimation->hide(true);
    }
    if (NULL != m_pMuteIcon)
    {
        m_pMuteIcon->hide(true);
    }
}
////////////////////////////////继承自  dlgbasetalkui /////////////////////


// 初始化
void CDlgRingUI::Init()
{
    CDlgBaseTalkUI::Init();

    CONFLICT_UNIT stUnit;

#if IF_TALKUI_CONFLICT_STYLE
    stUnit.m_ShowFunction = ringShowRingingTarget;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_RINGING_TOLINE_INFO, stUnit);
#endif
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgRingUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

#if IF_TALKUI_CONFLICT_STYLE
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_RINGING_TOLINE_INFO);
#endif
}

// 刷新窗口
void CDlgRingUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

bool CDlgRingUI::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strBGPath)
{
    talklogic_IsAllSessionRinging() ? iMin = 0 : iMin = 1;
    iMax = 15;
    return true;
}

void CDlgRingUI::OnTalkVolumnValueChange(int iValue)
{
    SetMuteInfo();

    CDlgBaseTalkUI::OnTalkVolumnValueChange(iValue);
}

//获取 processor
CBaseTalkUIProcessor * CDlgRingUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}


//是否显示DTMF条
bool CDlgRingUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgRingUI::IsShowSessionScrollBar()
{
    return true;
}

// 能否切换Session
bool CDlgRingUI::IsChangeSession(int iKey)
{
    return true;
}
////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

////////////////////////////////继承自  CUILogicRingUIBase /////////////////////
//设置UILogic 数据
void CDlgRingUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{

}

void CDlgRingUI::StopUIAnimation()
{
    if (NULL != m_pRingingContactAnimation)
    {
        m_pRingingContactAnimation->StopAnimation();
    }

    if (NULL != m_pRingingAnimation)
    {
        m_pRingingAnimation->StopAnimation();
    }

    m_talkWidget.EnableHintScroll(false);

    if (NULL != m_pRingingTarget)
    {
        m_pRingingTarget->EnableScroll(false);
    }

#if IF_TALKUI_RINGUI_TARGET_SPLIT
    if (NULL != m_pRingingTargetNumber)
    {
        m_pRingingTargetNumber->EnableScroll(false);
    }
#endif
}

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgRingUI::RefreshTalkUIData()
{
    CRingUIData * pRingUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pRingUIData = m_pProcessor->GetRingUIData();
    }

    if (NULL == pRingUIData)
    {
        return ;
    }

    //顺序不能随意调整
    //原因是m_bShowContactImage的值要先获取
    ResetWidget();

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetPhotoInfo(pRingUIData->m_objContactInfo.strContactImage);
#endif
    SetContactInfo(pRingUIData);
    SetRingAnimation();

    SetMuteInfo();

    UpdateWnd();
    _TalkConflictManager.checkShowUnit();
}

void CDlgRingUI::SetContactInfo(CRingUIData * pRingUIData)
{
    bool bWithTargetInfo = false;

    //获取联系人信息显示策略
    SHOW_CONTACTDETAIL_TYPE nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)
            _TalkUILogicDelegate.GetDisplayMethod();

    yl::string strContactName = "";     //联系人名称
    yl::string strContactNumber = "";   //联系人号码
    yl::string strFullContactInfo = ""; //联系人全信息
    yl::string strViaInfo = "";     //转接信息

    //不是recall 有显示中转信息
    if ((!pRingUIData->m_bIsRecalling) && _TalkUILogicDelegate.IsShowViaInfo()
            && (!pRingUIData->m_strViaInfo.empty()))
    {
        strContactName = pRingUIData->m_objContactInfo.strName;     //联系人名称
        strContactNumber = pRingUIData->m_objContactInfo.strNumber; //联系人号码
        strFullContactInfo = pRingUIData->m_objContactInfo.strFullContactInfo;  //联系人全信息
        strViaInfo = pRingUIData->m_strViaInfo;     //转接信息

        if (strContactName.empty())
        {
            strContactName = strContactNumber;
        }

#if !IF_BUG_26914
        //如果 名称不为空 那么要加上from
        if (!strContactName.empty())
        {
            strContactName = _UILOGIC_LANG_TRAN(TRID_TALK_FROM) + yl::string(":") + strContactName;
        }

        if (_TalkUILogicDelegate.IsShowTargetInfo())
        {
            bWithTargetInfo = true;

            //如果号码不为空 那么前面要加上number
            if (!strContactNumber.empty())
            {
                strContactName += " " + strContactNumber;
            }
        }
        else
        {
            //如果号码不为空 那么前面要加上number
            if (!strContactNumber.empty())
            {
                // http://10.3.5.199/Bug.php?BugID=63013
                // 翻译项改变了 要加冒号
                strContactNumber = _UILOGIC_LANG_TRAN(TRID_FROM_NUMBER) + yl::string(":") + strContactNumber;
            }
        }

        //这边就不用判空 了  因为之前已经判断过了
        strViaInfo = _UILOGIC_LANG_TRAN(TRID_VIA) + yl::string(":") + strViaInfo;

#endif

        //根据这些信息获取 需要显示的信息
        ChangeNameAndNumberByInfoMethod(strContactName, strContactNumber, strFullContactInfo,
                                        nDisplayMethod);
    }
    else        //不显示中转信息
    {
        //这边信息 内部已经转好
        strContactName = pRingUIData->m_objContactInfo.strName;     //联系人名称
        strContactNumber = pRingUIData->m_objContactInfo.strNumber; //联系人号码
        strViaInfo = "";        //转接信息
    }

    if (bWithTargetInfo)
    {
        strContactNumber = strViaInfo;
    }

    yl::string strTargetName = pRingUIData->m_strTargetName;
    yl::string strTargetNumber = pRingUIData->m_strTargetNumber;
    yl::string strTargetFullContactInfo = pRingUIData->m_strTargetFullContactInfo;

    if (_TalkUILogicDelegate.IsShowTargetInfo()
            && (!strTargetName.empty()) || (!strTargetNumber.empty()))
    {
#if IF_TALKUI_RINGUI_TARGET_SPLIT
        if (strTargetName.empty())
        {
            strTargetName = strTargetNumber;
        }
#else
        //根据这些信息获取 需要显示的信息
        ChangeNameAndNumberByInfoMethod(strTargetName, strTargetNumber, strTargetFullContactInfo,
                                        (SHOW_CONTACTDETAIL_TYPE)nDisplayMethod);
#endif

        //有中转信息
        strViaInfo = strTargetName;
    }

#if IF_TALKUI_FONT_SIZE_CHANGE_WHEN_TWO
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (PT_T21 == ePhoneType
            || PT_T23 == ePhoneType)
    {
        if (strViaInfo.empty())
        {
            strViaInfo = strContactNumber;
            strContactNumber = "";
        }
    }
#endif

    m_talkWidget.SetBaseContactInfo(strContactName, strContactNumber);

    if (!strViaInfo.empty())
    {
        if (NULL != m_pRingingInfoFrame)
        {
            m_pRingingInfoFrame->show();
        }

        //是target info的情况下才显示箭头
        if (!pRingUIData->m_strTargetName.empty()
                || !pRingUIData->m_strTargetNumber.empty())
        {
#if IF_BUG_27800
            if (NULL != m_pRingingArrow)
            {
                m_pRingingArrow->hide(true);
            }
            if (NULL != m_pRingingArrowRight)
            {
                m_pRingingArrowRight->hide(true);
            }
#else
            if (NULL != m_pRingingArrow)
            {
                m_pRingingArrow->show();
            }
            if (NULL != m_pRingingArrowRight)
            {
                m_pRingingArrowRight->show();
            }
#endif
        }
        else
        {
            if (NULL != m_pRingingArrow)
            {
                m_pRingingArrow->hide(true);
            }
            if (NULL != m_pRingingArrowRight)
            {
                m_pRingingArrowRight->hide(true);
            }
        }

#if IF_BUG_27800
        if (!strTargetName.empty())
        {
            //显示本地被叫方信息时，使用"To:"替换">"
            strViaInfo = _UILOGIC_LANG_TRAN(TRID_RINGUI_TO) + strViaInfo;
        }
#endif

        if (NULL != m_pRingingTarget)
        {
            m_pRingingTarget->show();
            m_pRingingTarget->SetText(strViaInfo);
        }

#if IF_TALKUI_RINGUI_TARGET_SPLIT
        if (!strTargetNumber.empty()
                && strTargetName != strTargetNumber
                && NULL != m_pRingingTargetNumber)
        {
            m_pRingingTargetNumber->show();
            m_pRingingTargetNumber->SetText(strTargetNumber);
        }
#endif
    }

    bool bNameBigFont = false;

#if IF_TALKUI_FONT_SIZE_CHANGE
#if IF_TALKUI_FONT_SIZE_CHANGE_WHEN_TWO
    if (PT_T21 == ePhoneType
            || PT_T23 == ePhoneType)
    {
        if (strContactNumber.empty() && strViaInfo.empty())
        {
            bNameBigFont = true;
        }
    }
    else
    {
        if (strContactNumber.empty() || strViaInfo.empty())
        {
            bNameBigFont = true;
        }
    }
#else
    if (strContactNumber.empty() && strViaInfo.empty())
    {
        bNameBigFont = true;
    }
#endif//IF_TALKUI_FONT_SIZE_CHANGE_WHEN_TWO
#endif//IF_TALKUI_FONT_SIZE_CHANGE

#if IF_TALKUI_FONT_SIZE_CHANGE
    if (bNameBigFont)
    {
#if IF_TALKUI_FONT_SIZE_CHANGE_WHEN_TWO
        m_talkWidget.SetNameFontSize(RINGINGUI_FONT_SIZE_BIG, RINGINGUI_MARGIN_BOTTOM_SMALL);
#else
        m_talkWidget.SetNameFontSize(RINGINGUI_FONT_SIZE_BIG, RINGINGUI_MARGIN_BOTTOM_BIG);
#endif
    }
    else
    {
        m_talkWidget.SetNameFontSize(RINGINGUI_FONT_SIZE_SMALL, RINGINGUI_MARGIN_BOTTOM_SMALL);
    }
#endif

#if IF_TALKUI_CONFLICT_STYLE
    if (!strViaInfo.empty())
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_RINGING_TOLINE_INFO, true);
    }
    else
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_RINGING_TOLINE_INFO, false);
    }
#endif
}

void CDlgRingUI::SetRingAnimation()
{
    bool bShowContactAnimation = false;

    if (m_talkWidget.GetIsShowContact()
            && NULL != m_pRingingContactAnimation)
    {
        bShowContactAnimation = true;
    }

    if (NULL != m_pRingingContactAnimation)
    {
        if (bShowContactAnimation)
        {
            m_pRingingContactAnimation->show();
        }
        else
        {
            m_pRingingContactAnimation->hide();
        }
    }

    if (NULL != m_pRingingAnimation)
    {
        if (!bShowContactAnimation)
        {
            m_pRingingAnimation->show();
        }
        else
        {
            m_pRingingAnimation->hide();
        }
    }
}

////////////////////////////////继承自  CUILogicRingUIBase 结束 /////////////////////

void CDlgRingUI::showRingingHint(bool bShow)
{
    if (NULL != m_pRingingInfoFrame)
    {
        if (bShow)
        {
            m_pRingingInfoFrame->show();
        }
        else
        {
            m_pRingingInfoFrame->hide(true);
        }
    }
}

void CDlgRingUI::SetMuteInfo()
{
    if (TALKUI_TITLE_TYPE_NO_TITLE != m_eTitleType
            || NULL == m_pMuteIcon)
    {
        return;
    }

    CRingUIData * pRingUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pRingUIData = m_pProcessor->GetRingUIData();
    }

    if ((NULL != pRingUIData && pRingUIData->m_bIsSilence)
            || voice_GetVolume(VT_RING) == 0)
    {
        m_pMuteIcon->show();
    }
    else
    {
        m_pMuteIcon->hide();
    }
}

}       //namespace talkui

