#include "dlgconnectui.h"

#include "talkuixmlitemname.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"

////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicdelegate.h"

#include "conflictmanager.h"
#include "talkuifactory.h"

namespace talkui
{
CDlgConnectUI::CDlgConnectUI()
    : CDlgBaseTalkUI(DLG_ConnectUI, eWT_Talk)
    , m_pConnectContactAnimation(NULL)
    , m_pConnectAnimation(NULL)
{

}

CDlgConnectUI::~CDlgConnectUI()
{

}


void CDlgConnectUI::LoadLayout()
{
    loadContent(TALKUI_DLGCONNECTUI_XML);
}

void CDlgConnectUI::loadChildrenElements(xml_node & node)
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

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    m_pConnectContactAnimation = static_cast<xAnimationBox*>(getViewById(TALKUI_CONNECT_CONATCT_ANIMATION));

    if (NULL != m_pConnectContactAnimation)
    {
        LIST_STRING listIconPath;
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT0);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT1);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT2);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT3);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT4);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT5);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT6);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT7);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT8);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT9);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT10);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT11);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT12);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT13);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT14);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT15);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT16);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT17);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT18);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONTACT_CONNECT19);

        m_pConnectContactAnimation->SetAnimation(listIconPath);
    }
#endif

    m_pConnectAnimation = static_cast<xAnimationBox*>(getViewById(TALKUI_CONNECT_ANIMATION));

    if (NULL != m_pConnectAnimation)
    {
        LIST_STRING listIconPath;
        listIconPath.push_back(PIC_TALK_ANIMATION_CONNECT1);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONNECT2);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONNECT3);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONNECT4);
        listIconPath.push_back(PIC_TALK_ANIMATION_CONNECT5);

#if IF_TALKUI_CONTACT_PHOTO
        listIconPath.push_back(PIC_TALK_ANIMATION_CONNECT6);
#endif

        m_pConnectAnimation->SetAnimation(listIconPath);
    }
}

void CDlgConnectUI::ResetWidget()
{
    m_talkWidget.ResetWidget();

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    if (NULL != m_pConnectContactAnimation)
    {
        m_pConnectContactAnimation->hide(true);
    }
#endif
    if (NULL != m_pConnectAnimation)
    {
        m_pConnectAnimation->hide(true);
    }
}
////////////////////////////////继承自  dlgbasetalkui /////////////////////

// 初始化
void CDlgConnectUI::Init()
{
    CDlgBaseTalkUI::Init();
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgConnectUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();
}

// 刷新窗口
void CDlgConnectUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}


//获取 processor
CBaseTalkUIProcessor * CDlgConnectUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}


//是否显示DTMF条
bool CDlgConnectUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgConnectUI::IsShowSessionScrollBar()
{
    return true;
}

// 能否切换Session
bool CDlgConnectUI::IsChangeSession(int iKey)
{
    return true;
}
////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

////////////////////////////////继承自  CUILogicConnectUIBase /////////////////////
//设置UILogic 数据
void CDlgConnectUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{

}
////////////////////////////////继承自  CUILogicConnectUIBase 结束 /////////////////////


//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgConnectUI::RefreshTalkUIData()
{
    CConnectUIData * pConnectUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pConnectUIData = m_pProcessor->GetConnectUIData();
    }

    if (NULL == pConnectUIData)
    {
        return;
    }

    //顺序不能随意调整
    //原因是m_bShowContactImage的值要先获取
    ResetWidget();

#if IF_TALKUI_CONTACT_PHOTO
    m_talkWidget.SetPhotoInfo(pConnectUIData->m_objContactInfo.strContactImage);
#endif
    SetContactInfo(pConnectUIData);

    SetConnectAnimation();

    UpdateWnd();
    _TalkConflictManager.checkShowUnit();
}

void CDlgConnectUI::SetContactInfo(CConnectUIData * pConnectUIData)
{
    yl::string strContactName = pConnectUIData->m_objContactInfo.strName;     //联系人名称
    yl::string strContactNumber = pConnectUIData->m_objContactInfo.strNumber;   //联系人号码

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

void CDlgConnectUI::SetConnectAnimation()
{
    bool bShowContactAnimation = false;

#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
    if (m_talkWidget.GetIsShowContact()
            && NULL != m_pConnectContactAnimation)
    {
        bShowContactAnimation = true;
    }

    if (NULL != m_pConnectContactAnimation)
    {
        if (bShowContactAnimation)
        {
            m_pConnectContactAnimation->show();
        }
        else
        {
            m_pConnectContactAnimation->hide();
        }
    }
#endif

    if (NULL != m_pConnectAnimation)
    {
        if (!bShowContactAnimation)
        {
            m_pConnectAnimation->show();
        }
        else
        {
            m_pConnectAnimation->hide();
        }
    }
}

}       //namespace talkui
