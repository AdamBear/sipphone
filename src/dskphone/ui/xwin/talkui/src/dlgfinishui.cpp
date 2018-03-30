#include "dlgfinishui.h"

#include "talkuixmlitemname.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"

////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicdelegate.h"

#include "conflictmanager.h"
#include "talkuifactory.h"

namespace talkui
{
#if IF_FINISH_HINT_WRAP
#define FINISH_HINT_WRAP_WIDTH      65
#endif

void finishShowFinishInfo(bool bShow)
{
    CDlgFinishUI* pFinishUI = (CDlgFinishUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_FINISH);
    if (NULL != pFinishUI)
    {
        pFinishUI->showFinishInfo(bShow);
    }
}

void finishShowFinishTime(bool bShow)
{
    CDlgFinishUI* pFinishUI = (CDlgFinishUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_FINISH);
    if (NULL != pFinishUI)
    {
        pFinishUI->showFinishTime(bShow);
    }
}

CDlgFinishUI::CDlgFinishUI()
    : CDlgBaseTalkUI(DLG_FinishUI, eWT_Talk)
    , m_pTalkTime(NULL)
    , m_pFinishHintFrame(NULL)
    , m_pFinishIcon(NULL)
    , m_pFinishHint(NULL)
{

}

CDlgFinishUI::~CDlgFinishUI()
{

}


void CDlgFinishUI::LoadLayout()
{
    loadContent(TALKUI_DLGFINISHUI_XML);
}

void CDlgFinishUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_talkWidget.SetNormalHintWidget(static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_1))
                                     , static_cast<xTextView *>(getViewById(TALKUI_TALK_HINT_2)));

    m_pTalkTime = static_cast<xTextView*>(getViewById(TALKUI_TALK_TALK_TIME));

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetContactWidget(static_cast<xFrameLayout *>(getViewById(TALKUI_TALK_CONTACT_IMAGE_FRAME))
                                  , static_cast<xImageView *>(getViewById(TALKUI_TALK_CONTACT_IMAGE)));
#endif

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    m_talkWidget.SetContactMask(static_cast<xImageView *>(getViewById(TALKUI_TALK_CONTACT_MASK)));
#endif

    m_pFinishHintFrame = static_cast<xView*>(getViewById(TALKUI_FINISH_HINT_FRAME));
    m_pFinishIcon = static_cast<xImageView*>(getViewById(TALKUI_FINISH_ICON));
    m_pFinishHint = static_cast<xTextView*>(getViewById(TALKUI_FINISH_HINT));
}

void CDlgFinishUI::ResetWidget()
{
    m_talkWidget.ResetWidget();

    if (NULL != m_pTalkTime)
    {
        m_pTalkTime->hide(true);
        m_pTalkTime->SetText("");
    }

    if (NULL != m_pFinishHintFrame)
    {
        m_pFinishHintFrame->hide(true);
    }
    if (NULL != m_pFinishIcon)
    {
        m_pFinishIcon->hide(true);
    }
    if (NULL != m_pFinishHint)
    {
        m_pFinishHint->hide(true);
        m_pFinishHint->SetText("");
    }
}
////////////////////////////////继承自  dlgbasetalkui /////////////////////

// 初始化
void CDlgFinishUI::Init()
{
    CDlgBaseTalkUI::Init();

    CONFLICT_UNIT stUnit;

#if IF_TALKUI_CONFLICT_STYLE
    stUnit.m_ShowFunction = finishShowFinishInfo;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_FINISHI_INFO, stUnit);

    stUnit.m_ShowFunction = finishShowFinishTime;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_TIME, stUnit);
#endif
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgFinishUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

#if IF_TALKUI_CONFLICT_STYLE
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_FINISHI_INFO);
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_TIME);
#endif
}

// 刷新窗口
void CDlgFinishUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

//获取 processor
CBaseTalkUIProcessor * CDlgFinishUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgFinishUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgFinishUI::IsShowSessionScrollBar()
{
    return true;
}

// 能否切换Session
bool CDlgFinishUI::IsChangeSession(int iKey)
{
    return true;
}
////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

////////////////////////////////继承自  CUILogicFinishUIBase /////////////////////
//设置UILogic 数据
void CDlgFinishUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{

}

////////////////////////////////继承自  CUILogicFinishUIBase 结束 /////////////////////

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgFinishUI::RefreshTalkUIData()
{
    CFinishUIData * pFinishUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pFinishUIData = m_pProcessor->GetFinishUIData();
    }

    if (NULL == pFinishUIData)
    {
        return;
    }

    //顺序不能随意调整
    //原因是m_bShowContactImage的值要先获取
    ResetWidget();

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetPhotoInfo(pFinishUIData->objContactInfo.strContactImage);
#endif
    SetContactInfo(pFinishUIData);
    SetFinishInfo(pFinishUIData);

    UpdateWnd();
    _TalkConflictManager.checkShowUnit();
}

void CDlgFinishUI::SetContactInfo(CFinishUIData * pFinishUIData)
{
    yl::string strContactName = pFinishUIData->objContactInfo.strName;
    yl::string strContactNumber = pFinishUIData->objContactInfo.strNumber;

    m_talkWidget.SetBaseContactInfo(strContactName, strContactNumber);

#if IF_TALKUI_FONT_SIZE_CHANGE
#if IF_TALKUI_FONT_SIZE_CHANGE_WHEN_TWO
    if (strContactNumber.empty())
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

void CDlgFinishUI::SetFinishInfo(CFinishUIData * pFinishUIData)
{
    //设置错误信息
    if (pFinishUIData->bError)
    {
        if (NULL != m_pFinishHint)
        {
            if (NULL != m_pFinishIcon)
            {
                m_pFinishIcon->show();
            }

            m_pFinishHint->show();
            m_pFinishHint->SetText(pFinishUIData->strInfo);

#if IF_FINISH_HINT_WRAP
            xFont font = m_pFinishHint->GetFont();
            chSize szCharSize = font.textBound(pFinishUIData->strInfo);
            if (szCharSize.cx > FINISH_HINT_WRAP_WIDTH)
            {
                m_pFinishHint->SetMultiLineEnable(true);
                m_pFinishHint->layout_width = FINISH_HINT_WRAP_WIDTH;

                if (NULL != m_pFinishIcon)
                {
                    m_pFinishIcon->hide(true);
                }
            }
            else
            {
                m_pFinishHint->SetMultiLineEnable(false);
                m_pFinishHint->layout_width = LayoutParams::wrap_content;

                if (NULL != m_pFinishIcon)
                {
                    m_pFinishIcon->show();
                }
            }
#endif
        }

#if IF_TALKUI_CONFLICT_STYLE
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_FINISHI_INFO, true);
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_TIME, false);
#else
        showFinishInfo(true);
        showFinishTime(false);
#endif
    }
    //设置通话时间
    else
    {
        if (NULL != m_pTalkTime)
        {
            m_pTalkTime->show();
            m_pTalkTime->SetText(pFinishUIData->strInfo);
        }

#if IF_TALKUI_CONFLICT_STYLE
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_FINISHI_INFO, false);
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_TIME, true);
#else
        showFinishInfo(false);
        showFinishTime(true);
#endif
    }
}

void CDlgFinishUI::showFinishInfo(bool bShow)
{
    if (NULL != m_pFinishHintFrame)
    {
        if (bShow)
        {
            m_pFinishHintFrame->show();
        }
        else
        {
            m_pFinishHintFrame->hide(true);
        }
    }
}

void CDlgFinishUI::showFinishTime(bool bShow)
{
    if (NULL != m_pTalkTime)
    {
        if (bShow)
        {
            m_pTalkTime->show();
        }
        else
        {
            m_pTalkTime->hide(true);
        }
    }
}

}       //namespace talkui

