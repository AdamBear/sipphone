#include "dlgparknotifyui.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"
#include "talkuixmlitemname.h"
#include "conflictmanager.h"
#include "talkuifactory.h"
#include "talkuixmlpath.h"

////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicdelegate.h"
#include "talkuipath.h"

namespace talkui
{

void parknotifyShowStatusHint(bool bShow)
{
    CDlgParkNotifyUI* pParkNotifyUI = (CDlgParkNotifyUI*)_TalkUIFactory.GetTalkUI(PROCESSORNAME_PARKNOTIFY);
    if (NULL != pParkNotifyUI)
    {
        pParkNotifyUI->showStatusHint(bShow);
    }
}

CDlgParkNotifyUI::CDlgParkNotifyUI()
    : CDlgBaseTalkUI(DLG_ParkNotifyUI, eWT_Talk)
{

}

CDlgParkNotifyUI::~CDlgParkNotifyUI()
{

}

void CDlgParkNotifyUI::LoadLayout()
{
    loadContent(TALKUI_DLGPARKNOFITY_XML);
}

void CDlgParkNotifyUI::loadChildrenElements(xml_node & node)
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

    m_talkWidget.SetStatusWidget(static_cast<xImageView *>(getViewById(TALKUI_TALK_NO_CONTACT_STATUS_FRAME))
                                 , static_cast<xImageView *>(getViewById(TALKUI_TALK_NO_CONTACT_STATUS_ICON))
                                 , static_cast<xTextView *>(getViewById(TALKUI_TALK_NO_CONTACT_STATUS_HINT)));
}

void CDlgParkNotifyUI::ResetWidget()
{
    m_talkWidget.ResetWidget();
}

////////////////////////////////继承自  dlgbasetalkui /////////////////////
// 初始化
void CDlgParkNotifyUI::Init()
{
    CDlgBaseTalkUI::Init();

    CONFLICT_UNIT stUnit;

#if IF_TALKUI_CONFLICT_STYLE
    stUnit.m_ShowFunction = parknotifyShowStatusHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_STATUS_INFO, stUnit);
#endif
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgParkNotifyUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

#if IF_TALKUI_CONFLICT_STYLE
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_STATUS_INFO);
#endif
}

// 刷新窗口
void CDlgParkNotifyUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

//获取 processor
CBaseTalkUIProcessor * CDlgParkNotifyUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgParkNotifyUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgParkNotifyUI::IsShowSessionScrollBar()
{
    return true;
}

// 能否切换Session
bool CDlgParkNotifyUI::IsChangeSession(int iKey)
{
    return true;
}
////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

////////////////////////////////继承自  CUILogicParkNotifyUIBase /////////////////////
//设置UILogic 数据
void CDlgParkNotifyUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{

}
////////////////////////////////继承自  CUILogicParkNotifyUIBase 结束 /////////////////////

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgParkNotifyUI::RefreshTalkUIData()
{
    CParkNotifyUIData * pParkNotifyUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pParkNotifyUIData = m_pProcessor->GetParkNotifyUIData();
    }

    if (NULL == pParkNotifyUIData)
    {
        return;
    }

    //顺序不能随意调整
    //原因是m_bShowContactImage的值要先获取
    ResetWidget();

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetPhotoInfo(pParkNotifyUIData->objContactInfo.strContactImage);
#endif
    SetContactInfo(pParkNotifyUIData);
    SetStatusInfo(pParkNotifyUIData);

    UpdateWnd();
    _TalkConflictManager.checkShowUnit();
}

void CDlgParkNotifyUI::SetContactInfo(CParkNotifyUIData * pParkNotifyUIData)
{
    m_talkWidget.SetBaseContactInfo(pParkNotifyUIData->objContactInfo.strName
                                    , pParkNotifyUIData->objContactInfo.strNumber);

#if IF_TALKUI_FONT_SIZE_CHANGE
#if IF_TALKUI_FONT_SIZE_CHANGE_WHEN_TWO
    if (pParkNotifyUIData->objContactInfo.strNumber.empty())
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

void CDlgParkNotifyUI::SetStatusInfo(CParkNotifyUIData * pParkNotifyUIData)
{
#if !IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetStatusInfo(PIC_TALK_HOLD_SMALL, _UILOGIC_LANG_TRAN(TRID_CALL_PARKED));

#if IF_TALKUI_CONFLICT_STYLE
    _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_STATUS_INFO, true);
#endif
#endif
}

void CDlgParkNotifyUI::showStatusHint(bool bShow)
{
#if !IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.showStatusHint(bShow);
#endif
}

}       //namespace talkui


