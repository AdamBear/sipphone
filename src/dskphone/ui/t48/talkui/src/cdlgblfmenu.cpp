#if 1//IF_FEATURE_SHOW_BLF_CALLID
#include "cdlgblfmenu.h"
#include "uikernel/languagehelper.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uimanager/moduimanager.h"
#include "uimanager/rectdefine.h"
#include "uikernel/qscrolltextlabel.h"
#include "mainwnd/mainwnd.h"
#include "modtalkui.h"
#include "blf/include/modblf.h"

#define ACTION_DLG_BLFMENU_BACK     "back"

CDlgBLFMenu::CDlgBLFMenu(QWidget* parent /*= 0*/)
    : CDlgBaseSubPage(parent)
{
    SetWindowType(eWT_SubPage);

    //设置顶层窗口属性
    SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_SETTING);

    InitData();

    //设置标题
    SetWindowTitle(LANG_TRANSLATE(TRID_BLF_CALL_ID));

    if (m_pGeneralFrame != NULL)
    {
        m_pGeneralFrame->SetItemSelectBG(PIC_WIDGET_SOME_PAGE_SELECT_BG);
        m_pGeneralFrame->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG);
        m_pGeneralFrame->SetRect(132, 0, 536, 336);
    }

    connect(this, SIGNAL(ProcessPageAction(const QString&)), this, SLOT(OnProcessPageAction(const QString&)));

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

CDlgBLFMenu::~CDlgBLFMenu()
{

}

void CDlgBLFMenu::Init()
{

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgBLFMenu::Uninit()
{
}

QPixmap CDlgBLFMenu::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_GENERAL_DLG_NORMAL_BACKGROUND);
}

void CDlgBLFMenu::SetData(void * pData)
{
    if (NULL == pData)
    {
        TALKUI_INFO("CDlgBLFMenu: Unexpected null pointer !!");
        return;
    }

    int iDsskeyId = *(int *)pData;
    m_strKey = GetBLFkeyByDsskeyId(iDsskeyId);
    if (!SetBLFMenuData(iDsskeyId))
    {
        return;
    }

    //if (NULL != m_pGeneralFrame)
    //{
    //    m_pGeneralFrame->SelectItemByDataIndex(0);
    //}
}

bool CDlgBLFMenu::SetBLFMenuData(int iDsskeyId)
{
    if (m_pGeneralFrame == NULL)
    {
        return false;
    }

    YLList<yl::string> listMenu;
    YLList<int> listStatus;
    if (!blf_GetBLFMenuList(iDsskeyId, listMenu, listStatus))
    {
        return false;
    }

    // clear old data
    int iOldFocus = m_pGeneralFrame->GetFocusItemDataIndex();
    m_pGeneralFrame->ClearContent(true);
    m_vecWidgetPair.clear();

    yl::string strContent = "";
    QWidgetPair qPair;

    YLList<yl::string>::iterator itMenu = listMenu.begin();
    YLList<int>::iterator itStatus = listStatus.begin();
    for (int i = 0; itMenu != listMenu.end(); ++itMenu, ++itStatus, i++)
    {
        qScrollTextLabel *pLabel = new qScrollTextLabel(this);
        if (NULL == pLabel)
        {
            continue;
        }
        strContent = *itMenu;
        pLabel->setText(QString::fromUtf8(strContent.c_str()));

        qPair.first = QString::fromUtf8(GetNameByStatus(*itStatus).c_str());
        qPair.second = pLabel;
        qPair.bChangeStyleOnPress = true;
        m_vecWidgetPair.push_back(qPair);
    }

    AddWidget(m_vecWidgetPair, true, false, true, true);

    // set focus item
    iOldFocus = qMax(iOldFocus, 0);
    iOldFocus = qMin(iOldFocus, m_pGeneralFrame->GetTotalDataCount() - 1);
    SetDefaultIndex(iOldFocus);

    return true;
}

bool CDlgBLFMenu::LoadPageData()
{
    return true;
}

//获取Softkey数据
void CDlgBLFMenu::GetSoftkey(CArraySoftKeyBarData& objSoftkeyData)
{
    objSoftkeyData = m_SoftkeyData;
}

bool CDlgBLFMenu::OnAction(const QString& strAction)
{
    bool bHandled = true;
    if (strAction == ACTION_DLG_BLFMENU_BACK)
    {
        OnCancel();
    }
    else
    {
        bHandled = false;
    }

    return bHandled;
}

//处理cancle事件
void CDlgBLFMenu::OnCancel()
{
    talkui_ExitBLFMenu();
}

bool CDlgBLFMenu::eventFilter(QObject *pObject, QEvent *pEvent)
{
    // 不处理非自身的消息
    if (!CheckEventValid(pObject, pEvent))
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent *pKeyEvent = static_cast<QKeyEvent*>(pEvent);
            int nKey = pKeyEvent->key();
            switch (nKey)
            {
            case PHONE_KEY_CANCEL:
                {
                    OnCancel();
                    return true;
                }
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgBLFMenu::InitData()
{
    m_SoftkeyData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_BACK);
    m_SoftkeyData[0].m_strSoftkeyAction = ACTION_DLG_BLFMENU_BACK;
    m_SoftkeyData[0].m_strIconPath = PIC_SOFTKEY_CANCEL;

    m_SoftkeyData[1].m_strSoftkeyAction = "";
    m_SoftkeyData[1].m_strSoftkeyTitle  = "";

    m_SoftkeyData[2].m_strSoftkeyAction = "";
    m_SoftkeyData[2].m_strSoftkeyTitle  = "";

    m_SoftkeyData[3].m_strSoftkeyAction = "";
    m_SoftkeyData[3].m_strSoftkeyTitle  = "";
}

// 根据SCA状态获取状态显示名
yl::string CDlgBLFMenu::GetNameByStatus(int iStatus)
{
    yl::string strStatus;
    switch (iStatus)
    {
    case DS_BLF_STATUS_RINGING:
    case DS_BLF_STATUS_RINGING_ME:
    case DS_BLF_STATUS_CALLING_ME:
        {
            strStatus = LANG_TRANSLATE(TRID_RINGING).toUtf8().data();
        }
        break;
    case DS_BLF_STATUS_TALKING:
    case DS_BLF_STATUS_TALKING_ME:
        {
            strStatus = LANG_TRANSLATE(TRID_TALKING).toUtf8().data();
        }
        break;
    case DS_BLF_STATUS_PARKED:
        {
            strStatus = LANG_TRANSLATE(TRID_CALL_PARKED).toUtf8().data();
        }
        break;
    case DS_BLF_STATUS_HOLD:
        {
            strStatus = LANG_TRANSLATE(TRID_HOLD).toUtf8().data();
        }
        break;
    case DS_BLF_STATUS_HELD_ME:
        {
            strStatus = LANG_TRANSLATE(TRID_HELD).toUtf8().data();
        }
        break;
    case DS_BLF_STATUS_CALLOUT:
        {
            strStatus = LANG_TRANSLATE(TRID_CALL_OUT).toUtf8().data();
        }
        break;
    case DS_BLF_STATUS_PARK:
        {
            strStatus = LANG_TRANSLATE(TRID_PARK_CALL).toUtf8().data();
        }
        break;
    case DS_BLF_STATUS_DND:
        {
            strStatus = LANG_TRANSLATE(TRID_DND).toUtf8().data();
        }
        break;
    default:
        break;
    }
    return strStatus;
}
#endif
