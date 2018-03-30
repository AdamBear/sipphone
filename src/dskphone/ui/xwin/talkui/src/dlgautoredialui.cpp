#include "dlgautoredialui.h"

#include "talkuixmlitemname.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"

////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicdelegate.h"

#include "conflictmanager.h"
#include "talkuifactory.h"

namespace talkui
{
CDlgAutoRedialUI::CDlgAutoRedialUI()
    : CDlgBaseTalkUI(DLG_AutoRedialUI, eWT_Talk)
    , m_pFinishHintFrame(NULL)
    , m_pRedialIcon(NULL)
    , m_pRedialHint(NULL)
{

}

CDlgAutoRedialUI::~CDlgAutoRedialUI()
{

}


void CDlgAutoRedialUI::LoadLayout()
{
    loadContent(TALKUI_DLGAUTOREDIAL_XML);
}

void CDlgAutoRedialUI::loadChildrenElements(xml_node & node)
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

    m_pFinishHintFrame = static_cast<xView*>(getViewById(TALKUI_AUTO_REDIAL_HINT_FRAME));
    m_pRedialIcon = static_cast<xImageView*>(getViewById(TALKUI_AUTO_REDIAL_ICON));
    m_pRedialHint = static_cast<xTextView*>(getViewById(TALKUI_AUTO_REDIAL_HINT));
}

void CDlgAutoRedialUI::ResetWidget()
{
    m_talkWidget.ResetWidget();

    if (NULL != m_pFinishHintFrame)
    {
        m_pFinishHintFrame->hide(true);
    }
    if (NULL != m_pRedialIcon)
    {
        m_pRedialIcon->hide(true);
    }
    if (NULL != m_pRedialHint)
    {
        m_pRedialHint->hide(true);
        m_pRedialHint->SetText("");
    }
}
////////////////////////////////继承自  dlgbasetalkui /////////////////////

// 初始化
void CDlgAutoRedialUI::Init()
{
    CDlgBaseTalkUI::Init();
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgAutoRedialUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();
}

// 刷新窗口
void CDlgAutoRedialUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}


//获取 processor
CBaseTalkUIProcessor * CDlgAutoRedialUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}


//是否显示DTMF条
bool CDlgAutoRedialUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgAutoRedialUI::IsShowSessionScrollBar()
{
    return true;
}

// 能否切换Session
bool CDlgAutoRedialUI::IsChangeSession(int iKey)
{
    return true;
}
////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

////////////////////////////////继承自  CUILogicFinishUIBase /////////////////////
//设置UILogic 数据
void CDlgAutoRedialUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{

}
////////////////////////////////继承自  CUILogicFinishUIBase 结束 /////////////////////

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgAutoRedialUI::RefreshTalkUIData()
{
    CAutoRedialUIData * pAutoRedialUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pAutoRedialUIData = m_pProcessor->GetAutoRedialUIData();
    }

    if (NULL == pAutoRedialUIData)
    {
        return;
    }

    //顺序不能随意调整
    //原因是m_bShowContactImage的值要先获取
    ResetWidget();

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetPhotoInfo(pAutoRedialUIData->objContactInfo.strContactImage);
#endif
    SetContactInfo(pAutoRedialUIData);
    SetRedialInfo(pAutoRedialUIData);

    UpdateWnd();
    _TalkConflictManager.checkShowUnit();
}

void CDlgAutoRedialUI::SetContactInfo(CAutoRedialUIData * pAutoRedialUIData)
{
    yl::string strContactName = pAutoRedialUIData->objContactInfo.strName;
    yl::string strContactNumber = pAutoRedialUIData->objContactInfo.strNumber;

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

//更新 autoredial界面提示
//AUTOREDIAL_STATUS eAutoRedialStatus;      //自动重拨状态
////显示自动重拨询问信息
//剩余的重拨次数
// 保存自动重拨倒计时超时秒数
void CDlgAutoRedialUI::SetRedialInfo(CAutoRedialUIData * pAutoRedialUIData)
{
    UpdateAutoRedialNote(pAutoRedialUIData->eAutoRedialStatus
                         , pAutoRedialUIData->strInfo
                         , pAutoRedialUIData->nRemainRedialTimes
                         , pAutoRedialUIData->nRedialInterval);
}

void CDlgAutoRedialUI::UpdateAutoRedialNote(AUTOREDIAL_STATUS eAutoRedialStatus
        , yl::string strInfo, int nRemainRedialTimes, int nRedialInterval)
{
    if (NULL != m_pFinishHintFrame)
    {
        m_pFinishHintFrame->show();
    }
    if (NULL != m_pRedialIcon)
    {
        m_pRedialIcon->show();
    }

    yl::string strAutoRedialHint = "";

    //这个是需要倒计时的
    if (AUTOREDIAL_COUNTDOWN == eAutoRedialStatus)
    {
        // 更新提示
#if IF_AUTO_REDIAL_SHOW_FULL
        strAutoRedialHint = commonAPI_FormatString("%s: %d, %s: %ds",
                            _UILOGIC_LANG_TRAN(TRID_REDIAL_TIMES).c_str()
                            , nRemainRedialTimes
                            , _UILOGIC_LANG_TRAN(TRID_INTERVAL).c_str()
                            , nRedialInterval);
#else
        strAutoRedialHint = commonAPI_FormatString("%s: %d - %ds",
                            _UILOGIC_LANG_TRAN(TRID_REDIAL_TIMES).c_str()
                            , nRemainRedialTimes
                            , nRedialInterval);
#endif
    }
    else    //这个是询问的
    {
        //已经翻译好了
        strAutoRedialHint = strInfo;
    }

    if (NULL != m_pRedialHint
            && !strAutoRedialHint.empty())
    {
        m_pRedialHint->show();
        m_pRedialHint->SetText(strAutoRedialHint);
    }
}

}       //namespace talkui


