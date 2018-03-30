#include "settingpage.h"
#include "cdlgbasesubpage.h"
#include "uicommon.h"
#include "uimanager/basedialog.h"
#include "uimanager/uimanager_inc.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/uimanager_header.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/qfrmpaint.h"
#include "uikernel/languagehelper.h"
#include "keymap.h"
#include "setting/include/modmenu.h"
#include "imagemanager/modimagemanager.h"
#include "devicelib/phonedevice.h"
#include "inputmethod/inputmethod.h"
#include "setting/include/modzero.h"
#include "mainwnd/mainwnd.h"
#include "uimanager/rectdefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "hotplug/commondefine.h"
#include "setting/include/authoritymessagedefine.h"
#include "messagebox/modmessagebox.h"
#include "baseui/t4xpicpath.h"
#include "uicommon/uikernel/qcomboboxutility.h"
#include "setting/src/menu/menucontroller.h"
#include "setting/include/modmenu.h"
#include "settingui/src/networksettingmanager.h"
#include <algorithm>
#include "settingpagehelper.h"
#include "commonapi/stringhelperapi.h"
#include "modsettingui.h"
#include "menuviewlistdelegate.h"
#include "uikernel/qwidgetutility.h"
#include "feature/include/modfeature.h"
#include "adapterbox/include/modadapterbox.h"
#include "edkui/edkuimanager.h"
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/recordmessagedefine.h"
#endif
#include "contacts/directory/include/directorymessagedefine.h"

namespace
{
#define ACTION_SWITCH_PAGE  "SwitchPage"
#define ACTION_ROLLACK_PAGE "RollackPage"
static const int MSGBOX_ID_REBOOT(100);
static const int MSGBOX_ID_SWITCH_PAGE(101);
static const int MSGBOX_ID_SAVE_COMPLETED(102);

static const QRect RECT_SETTING_SUBPAGE(256, 0, 536, 336);// setting界面的subpage的位置
}

SettingWndVec CSettingWindow::SettingWndList;

//消息回调函数
static LRESULT OnSettingWndMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnSettingWndMsg MsgID(%d)", objMessage.message);

    switch (objMessage.message)
    {
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    case AUTHORITY_MESSAGE_AUTHORITY_CHANGED:
        {
            SettingWndVec listWnd = CSettingWindow::GetSettingWndList();
            SettingWndVec::iterator iter = listWnd.begin();
            for (; iter != listWnd.end(); ++iter)
            {
                CSettingWindow * pWnd = *iter;
                if (pWnd == NULL)
                {
                    chWARNING2(FALSE, "OnSettingWndMsg Menu is inexistence");
                    continue;
                }
                // 刷新列表
                pWnd->UpdateWndFromMSG();
            }
        }
        break;
#endif
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    case RECORD_STORAGE_STATE_CHANGE:
        {
            SettingWndVec listWnd = CSettingWindow::GetSettingWndList();
            SettingWndVec::iterator iter = listWnd.begin();
            for (; iter != listWnd.end(); ++iter)
            {
                CSettingWindow * pWnd = *iter;
                if (pWnd == NULL)
                {
                    continue;
                }
                // 刷新列表
                pWnd->UpdateWndFromUSBMSG(objMessage);
            }
        }
        break;
#endif

    case BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT:
        {
            if (objMessage.lParam == 1)
            {
                SettingWndVec listWnd = CSettingWindow::GetSettingWndList();
                SettingWndVec::iterator iter = listWnd.begin();
                for (; iter != listWnd.end(); ++iter)
                {
                    CSettingWindow * pWnd = *iter;
                    if (pWnd == NULL)
                    {
                        chWARNING2(FALSE, "OnSettingWndMsg Menu is inexistence");
                        continue;
                    }

                    CMessageBoxBase * pMessageBox = MessageBox_GetTopMessageBox();
                    if (NULL != pMessageBox
                            && (MESSAGEBOX_BSFT_PWD == pMessageBox->GetType()
                                ||  MESSAGEBOX_PWD == pMessageBox->GetType()))
                    {
                        MessageBox_RemoveAllMessageBox(MessageBox_Destory);
                        // 重新获取bsft信息
                        CDlgBaseSubPage * pSubDialog = pWnd->GetCurrentSubPage();
                        if (NULL != pSubDialog)
                        {
                            pSubDialog->ExtraDialogInitData();
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

CBaseDialog * CSettingWindow::CreateInstance()
{
    return new CSettingWindow();
}

void CSettingWindow::AddSettingWnd(CSettingWindow * pWnd)
{
    if (NULL == pWnd)
    {
        return;
    }

    SettingWndVec::iterator iter = SettingWndList.begin();
    for (; iter != SettingWndList.end(); ++iter)
    {
        if (*iter == pWnd)
        {
            return;
        }
    }

    SettingWndList.push_back(pWnd);
}


SettingWndVec CSettingWindow::GetSettingWndList()
{
    return CSettingWindow::SettingWndList;
}

CSettingWindow::CSettingWindow(QWidget * parent)
    : CBaseDialog(parent)
    , m_ePageMode(PAGE_MODE_SETTING)
    , m_pSubPage(NULL)
    , m_strMenuAction("")
    , m_strCurrentSubPageAction("")
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    , m_pMenuList(NULL)
    , m_pMenuListDelegate(NULL)
#endif
    , m_ePageStyle(SETTING_PAGE_STYLE_NO_MENU)
{
    SETTINGUI_INFO("CSettingWindow::CSettingWindow");
    m_pPageHelper = new CSettingPageHelper(this);
    //设置顶层窗口
    SetTopWnd();

    SetDlgGeometry(RECT_SETTING);

    this->installEventFilter(this);
    CSettingWindow::AddSettingWnd(this);
}

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
bool CSettingWindow::CreateMenuView()
{
    m_pMenuList = new CFrameList(this);
    m_pMenuListDelegate = new CMenuviewDelegate(this);
    if (NULL != m_pMenuList && NULL != m_pMenuListDelegate)
    {
        m_pMenuList->setObjectName("MenuList");
        m_pMenuList->SetDelegate(m_pMenuListDelegate);
        m_pMenuListDelegate->SetFrameList(m_pMenuList);
        m_pMenuListDelegate->InitFrameList();

        connect(m_pMenuListDelegate, SIGNAL(ProcessCurrentItem(const QString &)), this,
                SLOT(OnMenuViewCurrentItemChange(const QString &)));

        return true;
    }

    return false;
}
#endif

CSettingWindow::~CSettingWindow()
{
    CSettingWindow::EraseSetttingWnd(this);

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    if (NULL != m_pMenuList)
    {
        m_pMenuList->SetDelegate(NULL);
    }
#endif
}

void CSettingWindow::EraseSetttingWnd(CSettingWindow * pWnd)
{
    SettingWndVec::iterator iter = SettingWndList.begin();
    while (iter != SettingWndList.end())
    {
        if (pWnd == *iter)
        {
            iter = SettingWndList.erase(iter);
            break;
        }
        else
        {
            ++iter;
        }
    }
}

void CSettingWindow::Init()
{
    // 注册消息
    RegisterMsg();

}

void CSettingWindow::RegisterMsg()
{
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    //注册消息
    etl_RegisterMsgHandle(AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                          AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                          OnSettingWndMsg);
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE,
                          RECORD_STORAGE_STATE_CHANGE,
                          OnSettingWndMsg);
#endif

    etl_RegisterMsgHandle(BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT,
                          BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT,
                          OnSettingWndMsg);
}

void CSettingWindow::UnRegisterMsg()
{
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    //注销消息
    etl_UnregisterMsgHandle(AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                            AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                            OnSettingWndMsg);
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE,
                            RECORD_STORAGE_STATE_CHANGE,
                            OnSettingWndMsg);
#endif

    etl_UnregisterMsgHandle(BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT,
                            BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT,
                            OnSettingWndMsg);
}

void CSettingWindow::Uninit()
{
    UnRegisterMsg();

    SETTING_INFO("CSettingWindow::Uninit");
#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
    while (m_listSubPage.size() > 0)
    {
        m_pSubPage = m_listSubPage.back();
        TakeSubPage();
    }
#else
    TakeSubPage();
#endif

    m_pSubPage = NULL;

    // 清空本地缓存的子页面
    this->ReleaseCacheDlg();

    ClearPageData();

    m_nDlgFlag = 0;
    SetTopWnd();
}

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
void CSettingWindow::SetPageStyle(SETTING_PAGE_STYLE eStyle)
{
    if (m_ePageStyle != eStyle)
    {
        m_ePageStyle = eStyle;

    }

    if (m_ePageStyle == SETTING_PAGE_STYLE_WITH_MENU)
    {
        if (m_pMenuList == NULL)
        {
            CreateMenuView();
        }

        m_pMenuList->setEnabled(true);
        m_pMenuList->show();
    }

    if (m_ePageStyle == SETTING_PAGE_STYLE_NO_MENU && m_pMenuList != NULL)
    {
        m_pMenuList->setEnabled(false);
        m_pMenuList->hide();
    }
}
#endif

void CSettingWindow::OnKeyboardShow()
{
    if (m_pSubPage != NULL)
    {
        m_pSubPage->OnKeyboardShow();
    }
}

void CSettingWindow::OnKeyboardHide()
{
    if (m_pSubPage != NULL)
    {
        m_pSubPage->OnKeyboardHide();
    }
}

void CSettingWindow::SetData(void * pData/* = NULL*/)
{
    if (m_ePageMode == PAGE_MODE_ZERO)
    {
        // 设置小窗口的风格为ZERO_SP_TOUCH
        m_eWndType = eWT_ZeroSPTouch;
    }
    else
    {
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
        if (m_ePageStyle == SETTING_PAGE_STYLE_WITH_MENU)
        {
            m_eWndType = eWT_SubMenu;

            if (NULL != m_pMenuListDelegate)
            {
                yl::string strListAction;
                if (NULL != pData)
                {
                    strListAction = *(yl::string *)pData;
                }

                if (!strListAction.empty())
                {
                    ///清空之前数据
                    m_pMenuListDelegate->ClearAllContent();
                    MenuItemListData pagemenuItemListData = Menu_GetMenuListByClickAction(strListAction);
                    m_pMenuListDelegate->SetContent(&pagemenuItemListData);
                }
            }
        }
        else
#endif
        {
            m_eWndType = eWT_SubPage;
        }
    }
}

void CSettingWindow::SetWndData(const SubMenuData & objSubMenuData)
{
    // 设置页面的Action
    m_strMenuAction = objSubMenuData.strMenuAction;

    // 设置页面的模式
    m_ePageMode = objSubMenuData.eMode;
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    // 设置选中的Item
    this->SetCurrentItem(objSubMenuData.iFirstSelectItem);
#endif
}

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
void CSettingWindow::SetCurrentItem(int iIndex)
{
    if (m_pMenuList != NULL && NULL != m_pMenuListDelegate)
    {
        m_pMenuListDelegate->SelectByIndex(iIndex, true);
    }
}

void CSettingWindow::SetCurrentItem(const QString & strItemClickAction)
{
    if (m_pMenuList && NULL != m_pMenuListDelegate)
    {
        m_pMenuListDelegate->SetCurrentItem(strItemClickAction, true);
    }
}
#endif

//事件过滤器
bool CSettingWindow::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    if (isChildOf(pObject, this)
            || pEvent->type() == QEvent::KeyPress
            || pEvent->type() == QEvent::KeyRelease)
    {
        //Zero模式下按ok键要特殊处理
        if (m_ePageMode == PAGE_MODE_ZERO)
        {
            if (pEvent->type() == QEvent::KeyPress)
            {
                QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
                if (pKeyEvent->key() == PHONE_KEY_OK)
                {
                    //当前没有弹出框时才进入下一界面
                    if (MessageBox_GetMessageBoxCount() == 0)
                    {
                        //按下ok键等同于按下next
                        OnAction(ACTION_NEXT_PAGE);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }

        switch (pEvent->type())
        {
        case QEvent::KeyPress:
            {
                if (MessageBox_GetMessageBoxCount() > 0)
                {
                    return false;
                }

                QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
                if (NULL != m_pSubPage && m_pSubPage->eventFilter(pObject, pEvent))
                {
                    return true;
                }

                switch (pKeyEvent->key())
                {
                case PHONE_KEY_CANCEL:
                    {
                        // 处理有下拉框时X键
                        QWidget * pwgt = focusWidget();
                        if (NULL != pwgt && pwgt->inherits("QComboBox") && pwgt != pObject)
                        {
                            QComboBox * pCmbBox = (QComboBox *)pwgt;
                            pCmbBox->hidePopup();
                            return true;
                        }

                        //按cancel退出一层界面
                        OnAction(ACTION_QUIT);
                        return true;
                    }
                    break;
                case PHONE_KEY_OK:
                    {
                        QWidget * pwgt = focusWidget();
                        if (pwgt != NULL && pwgt->inherits("QComboBox") && pwgt != pObject)
                        {
                            return false;
                        }

                        if (MessageBox_GetMessageBoxCount() == 0)
                        {
                            CArraySoftKeyBarData::iterator iter = m_SoftkeyData.begin();
                            for (; iter != m_SoftkeyData.end(); ++iter)
                            {
                                if (iter->m_strSoftkeyAction == ACTION_SAVE_DATA)
                                {
                                    OnUIAction(ACTION_SAVE_DATA);
                                    return true;
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            {
                if (m_pSubPage != NULL)
                {
                    // 向子页面传递事件
                    if (m_pSubPage->eventFilter(pObject, pEvent))
                    {
                        return true;
                    }
                }
            }
            break;
        }//end switch(pEvent->type())
    }
    return false;
}

void CSettingWindow::OnBtnIMEClick()
{
    //点击softbutton时，会进行输入法的切换，softbutton显示名也会随之变化
    _MainWND.SwitchNextIMEName(this->focusWidget());
}

void CSettingWindow::OnBtnDeleteClick()
{
    QWidget * pFocusWgt = focusWidget();
    if (NULL == pFocusWgt)
    {
        return;
    }

    //IME_DEL_ACTION
    if (pFocusWgt->inherits("QComboBox"))
    {
        //swith
        qComboBoxUtility::SwitchItem((QComboBox *)pFocusWgt, false);
    }
    else if (pFocusWgt->inherits("QLineEdit") || pFocusWgt->inherits("QTextEdit"))
    {
        //delete
        qOnBackspaceClicked(pFocusWgt);
    }
}

void CSettingWindow::OnProcessPageAction(const QString & strAction)
{
    if (sender() != m_pSubPage)
    {
        return;
    }

    SETTING_INFO("CSettingWindow::OnProcessPageAction[%s]", strAction.toUtf8().data());
    //Setting界面按OK键，CDlgBaseSubPage发出ProcessPageAction信号，
    if (strAction == ACTION_SAVE_DATA)
    {
        // 先交给SubPage处理
        if (m_pSubPage != NULL && m_pSubPage->OnAction(strAction))
        {
            return;
        }

        //点击了Enter
        OnBtnSaveClick();
    }
    else
    {
        bool bHandled =  false;
        if (m_pSubPage != NULL)
        {
            bHandled = m_pSubPage->OnAction(strAction);
        }

        if (!bHandled)
        {
            SETTINGUI_WARN("settingui: Page action [%s] not processed", strAction.toUtf8().data());
        }
    }

}


void CSettingWindow::OnRefreshSoftkey()
{
    // 重新设置Softkey
    SetSoftKeyBarData();

    // 刷新softkey
    _MainWND.DrawSoftkey(this);

    // 刷新IME信息
    _MainWND.DisplayIMESoftkey(focusWidget());
}

// 设置窗口的SoftKey数据
void CSettingWindow::SetSoftKeyBarData()
{
    // 初始化softkey数据
    InitSoftKeyData();

    if (NULL == m_pSubPage)
    {
        return;
    }

    m_SoftkeyData[1].m_strSoftkeyAction = IME_ACTION;
    m_SoftkeyData[2].m_strSoftkeyAction = IME_DEL_ACTION;


    // 根据不同的页面模式进行softkey特化
    if (m_ePageMode == PAGE_MODE_SETTING || m_ePageMode == PAGE_MODE_LOCK)
    {
#if IF_NEED_SOFTKEY_BACK
        // 在setting模式,第1个softkey必须设置为回退
        m_SoftkeyData[0].m_strSoftkeyAction = ACTION_QUIT;
        m_SoftkeyData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_BACK);
#endif

        if (NULL != m_pSubPage && m_pSubPage->GetSubPageEnabled()
                && m_pSubPage->IsHasValidItem())
        {
            // 设置二级菜单界面对应SoftKeybar的数据
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_SAVE_DATA;
            m_SoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
        }
    }
    else if (m_ePageMode == PAGE_MODE_ZERO)
    {
        // TestTool模式均不使用子界面的softkey
        // 设置返回上一个页面softkey
#if IF_NEED_SOFTKEY_BACK
        m_SoftkeyData[0].m_strSoftkeyAction = ACTION_ROLLACK_PAGE;
        m_SoftkeyData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_BACK);
#endif

        if (NULL != m_pSubPage)
        {
            //设置输入法切换softkey
            m_SoftkeyData[1].m_strSoftkeyAction = IME_ACTION;

            //设置删除输入softkey
            m_SoftkeyData[2].m_strSoftkeyAction = IME_DEL_ACTION;

            //设置进入下一级界面softkey
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_NEXT_PAGE;
            m_SoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_NEXT);
        }
    }

    // 优先使用子界面softkey数据
    if (NULL != m_pSubPage)
    {
        // 判断子页面是否需要自己设置SoftKey
        m_pSubPage->GetSubPageEnabled() ? m_pSubPage->GetSoftkeyData(m_SoftkeyData)
        : m_pSubPage->GetPageDisableSoftkeyData(m_SoftkeyData);
#if IF_AUTO_RELAYOUT_SOFTKEY
        m_pSubPage->ResetSoftKey(m_SoftkeyData);
#endif
    }
#if IF_SOFTKEY_HAS_ICON
    //添加softkey的图片
    AddSoftkeyIcon(m_SoftkeyData);
#endif
}

#if IF_SOFTKEY_HAS_ICON
//添加常用的softkey的图标
void CSettingWindow::AddSoftkeyIcon(CArraySoftKeyBarData & objWndData)
{
    CArraySoftKeyBarData::iterator iter = objWndData.begin();
    for (; iter != objWndData.end(); ++iter)
    {
        if (iter->m_strIconPath.isEmpty())
        {
            iter->m_strIconPath = GetIconByAction(iter->m_strSoftkeyAction);
        }
    }
}

//根据softkey类型获得图标
QString CSettingWindow::GetIconByAction(yl::string strAction)
{
    if (strAction == ACTION_SAVE_DATA)
    {
        return PIC_SOFTKEY_SAVE;
    }
    else if (strAction == IME_DEL_ACTION)
    {
        return PIC_SOFTKEY_DELETE;
    }
    else if (strAction == ACTION_INITIATION)
    {
        return PIC_SOFTKEY_INITIATION;
    }
    return "";
}
#endif

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
void CSettingWindow::UpdateWndFromMSG()
{
    MenuItemListData pagemenuItemListData = g_pMenuController->GetMenuListByClickAction(
            m_strMenuAction);
    if (m_pMenuListDelegate != NULL)
    {
        //清空之前数据
        m_pMenuListDelegate->ClearAllContent();

        m_pMenuListDelegate->SetContent(&pagemenuItemListData);

        //默认选中第一项
        if (NULL != m_pMenuList && m_pMenuList->isVisible())
        {
            m_pMenuListDelegate->SelectByIndex(0, false);
        }
    }
}
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
void CSettingWindow::UpdateWndFromUSBMSG(msgObject & objMessage)
{
    if (objMessage.message != RECORD_STORAGE_STATE_CHANGE)
    {
        return;
    }

    if (m_pSubPage == NULL)
    {
        return;
    }

    yl::string strPageAction = m_pSubPage->GetSubPageAction();
    if (objMessage.wParam == 0)
    {
        if (strPageAction.find("browse_usb_record") != yl::string::npos
                || strPageAction.find("browse_usb_storage") != yl::string::npos
                || strPageAction.find("browse_usb_photo") != yl::string::npos
                || strPageAction.find("usb_idle_record") != yl::string::npos)
        {
            OnHomeButtonClick();
        }

        // 同Idle处理，容错
        // http://10.2.1.199/Bug.php?BugID=109443
        if (!AdapterBox_IsTestMode() && STATE_CONNECT == objMessage.lParam)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_USB_DEVICE_REMOVED), MESSAGEBOX_NOTE, 2000);
        }
    }
    else if (objMessage.wParam == 1)
    {
        if (strPageAction.find("browse_usb_record") != yl::string::npos
                || strPageAction.find("browse_usb_storage") != yl::string::npos
                || strPageAction.find("browse_usb_photo") != yl::string::npos
                || strPageAction.find("usb_idle_record") != yl::string::npos)
        {
            ProcessBack();
            SettingUI_OpenPage(SubMenuData("OpenMenu(usb_record.xml)"), true);
        }
    }
}
#endif

//选择默认索引，bEnter表示是否进入
void CSettingWindow::SelectItemIndex(int nIndex, bool bEnter /*= false*/)
{
    if (m_pSubPage != NULL)
    {
        m_pSubPage->SetDefaultIndex(nIndex, bEnter);
    }
}

//提供接口设置,取得子页面
//| nIndex | 索引值对应账号id值
CDlgBaseSubPage * CSettingWindow::GetCurrentSubPage() const
{
    return m_pSubPage;
}

void CSettingWindow::OnBtnSaveClick()
{
    if (m_pSubPage != NULL && m_pSubPage->GetSubPageEnabled())
    {
        //设置处于预输入状态的字符有效
        m_pSubPage->ReturnAllEdit();

        // 进行数据有效性验证
        InputDataErrorInfo objErrorInfo;

        if (m_pSubPage->IsInputDataValid(objErrorInfo))
        {
            //保存成功时才弹出相应提示并进行后续操作
            //子页面return false表示要自己处理保存后的操作
            if (m_pSubPage->SavePageData())
            {
                if (m_pSubPage != NULL
                        && m_pSubPage->SaveCompleted())
                {
                    //弹出保存成功的提示框
                    MessageBox_ShowCommonMsgBox(this,
                                                LANG_TRANSLATE(TRID_SAVING_CONFIG_FILE),
                                                MESSAGEBOX_NOTE, 1000, "", "",
                                                MSGBOX_ID_SAVE_COMPLETED);
                }
                else
                {
                    //回到上一层界面
                    OnBtnBackClick();
                }
            }
        }
        else
        {
            //先设置焦点，再弹出提示框
            if (NULL != objErrorInfo.pLineEdit)
            {
                m_pSubPage->SetErrorFocus(objErrorInfo.pLineEdit);
            }

            //弹出错误提示
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(objErrorInfo.strErrorContent), MESSAGEBOX_Cancel,
                                        2000);
            return; // 数据错误需要重新填写
        }
    }
}

// 处理回退事件
void CSettingWindow::ProcessBack()
{
    // 销毁当前子页面
    TakeSubPage();

#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
    if (m_listSubPage.size() < 1)
    {
        SetSubPage(NULL, "");
        UIManager_PutPrivateDlg(this);
    }
    else
    {
        //如果栈中还有页面则显示栈顶的页面
        CDlgBaseSubPage * pSubPage = m_listSubPage.back();
        if (NULL != pSubPage)
        {
            SetSubPage(pSubPage, pSubPage->GetSubPageAction());
        }
    }
#else
    // 销毁当前子页面
    SetSubPage(NULL, "");

    if (NULL == GetCurrentSubPage())
    {
        UIManager_PutPrivateDlg(this);
    }
#endif
}

void CSettingWindow::OnBtnBackClick()
{
    //判断网络改变标记
    if (_NetworkChangeFlagManager.GetNetworkChangeFlag())
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_NETWORK_CHANGED_IF_TAKE_EFFECT_AFTER_RESTART),
                                    MESSAGEBOX_OkCancel, -1,  "", "",
                                    MSGBOX_ID_REBOOT);
    }
    else
    {
        ProcessBack();
    }
}

void CSettingWindow::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nResult = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_REBOOT:
        {
            switch (nResult)
            {
            case MessageBox_OK:
                {
                    //弹出重启提示后，无论点击是否，都将标记重置
                    _NetworkChangeFlagManager.ClearChangeFlag();
                    AdapterBox_Reboot(0);//点击确认，重启机器，使得网络配置生效
                }
                break;
            case MessageBox_Cancel:
                {
                    //弹出重启提示后，无论点击是否，都将标记重置
                    _NetworkChangeFlagManager.ClearChangeFlag();
                    ProcessBack(); // 点击Cancel则先回退
                }
                break;
            default:
                break;
            }
        }
        break;
    case MSGBOX_ID_SWITCH_PAGE:
        {
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
            if (nResult != MessageBox_OK)
            {
                if (NULL != m_pMenuListDelegate)
                {
                    // 为了回退的时候不响应响应的槽函数,先断开连接
                    disconnect(m_pMenuListDelegate, SIGNAL(ProcessCurrentItem(const QString &)), this,
                               SLOT(OnMenuViewCurrentItemChange(const QString &)));

                    //重新选中上一个所在的Item
                    m_pMenuListDelegate->RollackPreItem();

                    connect(m_pMenuListDelegate, SIGNAL(ProcessCurrentItem(const QString &)), this,
                            SLOT(OnMenuViewCurrentItemChange(const QString &)));
                }
            }
            else
            {
#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
                ClearSubPageStack();//清除setting的子页面栈
#endif // IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
                QVariant extraData = pMessageBox->GetExtraData();
                if (extraData.canConvert<QString>())
                {
                    QString strAction = extraData.value<QString>();
                    // 加载新的子界面
                    IterSubPage it = m_mapSubPage.find(strAction.toUtf8().constData());
                    if (it != m_mapSubPage.end())
                    {
                        // 优先使用已经缓存的子页面
                        SetSubPage(it->second, it->first);
                    }
                    else
                    {
                        // 重新请求页面数据
                        SetSubPage(SubMenuData(strAction.toUtf8().data()));
                    }
                }
            }
#endif // IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
        }
        break;
    case MSGBOX_ID_SAVE_COMPLETED:
        {
            //回到上一层界面
            OnBtnBackClick();
        }
        break;
    }
}

void CSettingWindow::SetPageMode(PAGE_MODE eMode)
{
    m_ePageMode = eMode;
}

// 是否跳过Return To Idle，包括通话、Idle
bool CSettingWindow::IsRejectReturnToIdle(RETURN_IDLE_TYPE
        eReturnIdle/* = RETURN_IDLE_TYPE_NORMAL*/)
{
#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
    List_SubPage::const_iterator iter =  m_listSubPage.begin();
    for (; iter != m_listSubPage.end(); ++iter)
    {
        if ((*iter) != NULL && (*iter)->IsRejectReturnToIdle(eReturnIdle))
        {
            return true;
        }
    }
#else
    if (m_pSubPage != NULL && m_pSubPage->IsRejectReturnToIdle(eReturnIdle))
    {
        return true;
    }
#endif

    return false;
}

//子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
QPixmap CSettingWindow::GetCustomBackGround()
{
    if (m_ePageStyle == SETTING_PAGE_STYLE_WITH_MENU)
    {
        return THEME_GET_BMP(PIC_SUBMENU_BACKGROUND);
    }

    return THEME_GET_BMP(PIC_GENERAL_DLG_NORMAL_BACKGROUND);
}

bool CSettingWindow::OnAction(const QString & strAction)
{
    return OnUIAction(strAction);
}

bool CSettingWindow::OnUIAction(const QString & strAction, void * pActionData /*= NULL*/)
{
    SETTINGUI_INFO("CSettingWindow::OnAction(%s)", strAction.toUtf8().data());

    /*Bug ID : 44154 44167
    原因： 1、之前设计问题，没处理超过一页后的情况，指针乱了
    2、操作过程中，改变传入的const QString& Softkey Action的值或清空，导致死机
    临时解决方法：不直接传入Softkey Action引用，传临时变量*/

    QString strTemp = strAction;
    // 先交给子界面处理，子界面没处理，则交给CSettingWindow处理
    if (NULL != m_pSubPage
            && m_pSubPage->OnAction(strTemp))
    {
        SETTINGUI_INFO("subpage handed the action[%s]", strTemp.toUtf8().data());
        return true;
    }

    if (strTemp == ACTION_SWITCH_PAGE)
    {
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
        QString strAction = (NULL != pActionData) ? *((QString *)pActionData) : "";

        bool bSamePage = strcmp(strAction.toUtf8().data(), m_strCurrentSubPageAction.c_str()) == 0;

        //页面切换前询问是否保存数据
        if (m_pSubPage != NULL)
        {
            // 当页面不需要缓存,并且页面的属性修改
            if (!bSamePage
                    && !m_pSubPage->ShouldBeCache()
                    && m_pSubPage->IsStatusChanged())
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_DO_YOU_WANT_TO_DISCARD_YOUR_CHANGES),
                                            MESSAGEBOX_OkCancel, -1,  "", "",
                                            MSGBOX_ID_SWITCH_PAGE,
                                            QVariant::fromValue(strAction));
                return true;
            }
        }

#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
        ClearSubPageStack();//清除setting的子页面栈
#endif

        // 加载新的子界面
        IterSubPage it = m_mapSubPage.find(strAction.toUtf8().constData());
        if (it != m_mapSubPage.end())
        {
            // 优先使用已经缓存的子页面
            SetSubPage(it->second, it->first);
        }
        else
        {
            // 重新请求页面数据
            SetSubPage(SubMenuData(strAction.toUtf8().data()));
        }
#endif // IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    }
    else if (strTemp == ACTION_QUIT)
    {
        if (m_pSubPage != NULL)
        {
            m_pSubPage->OnBack();
        }

        if (m_ePageMode == PAGE_MODE_SETTING)
        {
            OnBtnBackClick();
        }
        else
        {
            OnUIAction(ACTION_ROLLACK_PAGE);
        }
    }
    else if (strTemp == ACTION_SAVE_DATA)
    {
        OnBtnSaveClick();
    }
    else if (strTemp == ACTION_NEXT_PAGE)
    {
        //输入当前界面的预编辑字符
        CDlgBaseSubPage * pSubPage = GetCurrentSubPage();
        if (NULL != pSubPage)
        {
            pSubPage->ReturnAllEdit();
        }

        // 保存缓存的界面数据
        if (SaveCacheDlgData())
        {
            // 清空当前的页面
            TakeSubPage();

            // 清空缓存的子页面资源
            ReleaseCacheDlg();

            // 进入下一级界面
            zero_EnterNextStage(m_strMenuAction, "next");
        }
    }
    else if (strTemp == ACTION_ROLLACK_PAGE)
    {
        //输入当前界面的预编辑字符
        CDlgBaseSubPage * pSubPage = GetCurrentSubPage();
        if (NULL != pSubPage)
        {
            pSubPage->ReturnAllEdit();
        }

        // 清空当前的页面
        TakeSubPage();

        // 清空缓存的子页面资源
        ReleaseCacheDlg();

        m_pSubPage = NULL;

        //返回上次的界面
        zero_EnterNextStage(m_strMenuAction, "back");
    }
    else if (strTemp == IME_ACTION)
    {
        OnBtnIMEClick();
    }
    else if (strTemp == IME_DEL_ACTION)
    {
        OnBtnDeleteClick();
    }
    else
    {
        SETTINGUI_WARN("handleSoftKeyAction action error(%s)!", strTemp.toUtf8().data());
        return false;
    }

    return true;
}

void CSettingWindow::DisconnectSubpage(CDlgBaseSubPage * pSubPage)
{
    if (NULL != pSubPage)
    {
        disconnect(pSubPage, SIGNAL(ProcessPageAction(const QString &)), this,
                   SLOT(OnProcessPageAction(const QString &)));
        disconnect(pSubPage, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
        disconnect(pSubPage, SIGNAL(BtnBackClick()), this, SLOT(OnBtnBackClick()));
        disconnect(pSubPage, SIGNAL(openpage(const SubMenuData &)), this,
                   SLOT(OnOpenpage(const SubMenuData &)));
        disconnect(pSubPage, SIGNAL(openpage(const yl::string &)), this,
                   SLOT(OnOpenpage(const yl::string &)));
        disconnect(pSubPage, SIGNAL(backandopenpage(const SubMenuData &)), this,
                   SLOT(OnBackAndEnterNewPage(const SubMenuData &)));
        disconnect(pSubPage, SIGNAL(RefreshTitle()), this, SLOT(OnRefreshTitle()));
        disconnect(pSubPage, SIGNAL(replaceCurrentPage(const SubMenuData &)), this,
                   SLOT(OnReplaceCurPage(const SubMenuData &)));
        disconnect(pSubPage, SIGNAL(RefreshTimer()), this, SLOT(OnRefreshTimer()));
    }
}

void CSettingWindow::ConnectSubpage(CDlgBaseSubPage * pSubPage)
{
    if (NULL != pSubPage)
    {
        connect(pSubPage, SIGNAL(ProcessPageAction(const QString &)), this,
                SLOT(OnProcessPageAction(const QString &)));
        connect(pSubPage, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
        connect(pSubPage, SIGNAL(BtnBackClick()), this, SLOT(OnBtnBackClick()));
        connect(pSubPage, SIGNAL(openpage(const SubMenuData &)), this,
                SLOT(OnOpenpage(const SubMenuData &)));
        connect(pSubPage, SIGNAL(openpage(const yl::string &)), this, SLOT(OnOpenpage(const yl::string &)));
        connect(pSubPage, SIGNAL(backandopenpage(const SubMenuData &)), this,
                SLOT(OnBackAndEnterNewPage(const SubMenuData &)));
        connect(pSubPage, SIGNAL(RefreshTitle()), this, SLOT(OnRefreshTitle()));
        connect(pSubPage, SIGNAL(replaceCurrentPage(const SubMenuData &)), this,
                SLOT(OnReplaceCurPage(const SubMenuData &)));
        connect(pSubPage, SIGNAL(RefreshTimer()), this, SLOT(OnRefreshTimer()));
    }
}

bool CSettingWindow::SetSubPage(const SubMenuData & pageInfo)
{
    SETTINGUI_INFO("CSettingWindow::SetSubPage [%s]", pageInfo.strMenuAction.c_str());
    if (NULL != m_pPageHelper)
    {
        return OnOpenpage(pageInfo);
    }
    return false;
}

void CSettingWindow::SetSubPage(CDlgBaseSubPage * pSubPage, const yl::string & strSubPageAction,
                                const int nDefaultIndex /* = 0 */)
{
    SETTINGUI_INFO("CSettingWindow::SetSubPage to [%x] [%s]", pSubPage, strSubPageAction.c_str());
    //如果前后是同一个界面，只更新界面数据，不作界面替换操作
    if (pSubPage != m_pSubPage)
    {
        if (NULL == pSubPage)
        {
            return;
        }

        if (NULL != m_pSubPage)
        {
            m_pSubPage->hide();

            //断开旧子页面的信号槽连接，防止多次连接造成槽函数多次响应
            DisconnectSubpage(m_pSubPage);
        }

        m_pSubPage = pSubPage;

        // 连接信号和槽
        ConnectSubpage(m_pSubPage);
    }

    if (NULL != m_pSubPage)
    {
#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
        pageListIiter iter = std::find(m_listSubPage.begin(), m_listSubPage.end(), pSubPage);
        if (iter != m_listSubPage.end())
        {
            if (!strSubPageAction.empty())
            {
                m_strCurrentSubPageAction = strSubPageAction;
            }
            RefreshTitle();

            m_pSubPage->raise();
            m_pSubPage->show();

            //回来之后刷新title和softkey既可
            UIManager_UpdateWnd(this);

            m_pSubPage->ExtraDialogInitData();
            return;
        }
#endif

        // 优先设置父窗口
        m_pSubPage->setParent(this);

        //根据页面的style设置位置
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
        if (m_ePageStyle == SETTING_PAGE_STYLE_WITH_MENU)
        {
            m_pSubPage->setGeometry(RECT_SETTING_SUBPAGE);
        }
        else
#endif
        {
            m_pSubPage->setGeometry((this->width() - RECT_SETTING_SUBPAGE.width()) / 2, 0,
                                    RECT_SETTING_SUBPAGE.width(), RECT_SETTING_SUBPAGE.height());
        }

        // 设置页面的模式
        m_pSubPage->SetPageMode(m_ePageMode);

        if (!strSubPageAction.empty())
        {
            m_pSubPage->SetOpenSubPageAction(strSubPageAction);
            m_strCurrentSubPageAction = strSubPageAction;
        }

        // 设置界面读写模式
        yl::string strAction = g_pMenuController->GetWebItemKeyByClickAction(strSubPageAction);
        m_pSubPage->SetSubPageEnabled(!feature_UserModeConfigItemReadOnly(strAction));

#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
        //将子页面加入子页栈（setting）
        PushSubPage(m_pSubPage);
#endif
        m_pSubPage->LoadPageData();
        RefreshTitle();
        CheckReturnToIdleFlag();

        // 刷新窗口
        UIManager_UpdateWnd(this);

        //显示subpage
        m_pSubPage->raise();
        m_pSubPage->show();
        m_pSubPage->UpdateWnd();
        if (nDefaultIndex > 0)
        {
            //framelist的操作要页面显示出来再进行，否则会出现很多错误
            m_pSubPage->SetDefaultIndex(nDefaultIndex);
        }
        // 进行一些需要额外的操作，比如一些阻塞或同步的动作，默认空实现
        m_pSubPage->ExtraDialogInitData();
    }
}

// 刷新Title
void CSettingWindow::OnRefreshTitle()
{
    SETTINGUI_INFO("OnRefreshTitle()");
    RefreshTitle();
    UIManager_UpdateWnd(this);
}


// 刷新定时器状态
void CSettingWindow::OnRefreshTimer()
{
    CheckCloseTimerFlag();
    UIManager_UpdateWnd(this);
}

bool CSettingWindow::RefreshTitle()
{
    //更新标题
    bool bDone = false;
    QString strSubPageTitle = "";
    if (NULL != m_pSubPage)
    {
        strSubPageTitle = m_pSubPage->GetWindowTitle();
        if (!strSubPageTitle.isEmpty())
        {
            SETTINGUI_INFO("CSettingWindow::RefreshTitle use subpage title");
            SetWindowTitle(strSubPageTitle);
            bDone = true;
        }
    }

    if (!bDone)
    {
        strSubPageTitle = LANG_TRANSLATE(Menu_GetTitleByClickAction(m_strCurrentSubPageAction).c_str());
        if (!strSubPageTitle.isEmpty())
        {
            SETTINGUI_INFO("CSettingWindow::RefreshTitle use menu title");
            SetWindowTitle(strSubPageTitle);
            bDone = true;
        }
    }

    return bDone;
}


#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
void CSettingWindow::PushSubPage(CDlgBaseSubPage * pSubPage)
{
    m_listSubPage.push_back(pSubPage);
}

void CSettingWindow::ClearSubPageStack()
{
    pageListIiter iter =  m_listSubPage.begin();
    for (; iter != m_listSubPage.end(); ++iter)
    {
        DisconnectSubpage(m_pSubPage);
        UIManager_PutPrivateDlg((*iter));
    }
    m_listSubPage.clear();
    m_pSubPage = NULL;
}
#endif

yl::string CSettingWindow::GetPageAction()
{
    return m_strMenuAction;
}

// 弹出当前的子页面
void CSettingWindow::TakeSubPage()
{
    if (NULL == m_pSubPage)
    {
        BASEUI_DEBUG("CSettingWindow::TakeSubPage (NULL == m_pSubPage)");
        return;
    }


    // 判断前子页面是否存在
    if (m_pSubPage != NULL)
    {
        SETTING_INFO("CSettingWindow::TakeSubPage [%s]", m_pSubPage->GetSubPageAction().c_str());

        // 界面退出时确保子界面framelist正确位置
        OnKeyboardHide();

        // 断开子界面的信号槽
        DisconnectSubpage(m_pSubPage);

        m_pSubPage->setParent(NULL);

#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
        m_listSubPage.pop_back();
#endif

        // 查看页面是否缓存
        if (!m_pSubPage->ShouldBeCache())
        {
            // 归还子页面的资源
            UIManager_PutPrivateDlg(m_pSubPage);
        }
        else
        {
            m_pSubPage->hide();
        }

        CheckReturnToIdleFlag();

        m_pSubPage = NULL;
    }
}

// 释放缓存的子页面
void CSettingWindow::ReleaseCacheDlg()
{
    std::map<yl::string, CDlgBaseSubPage *>::iterator it = m_mapSubPage.begin();
    for (/* */; it != m_mapSubPage.end(); ++it)
    {
        CDlgBaseSubPage * pSubPage = it->second;
        if (NULL != pSubPage)
        {
            //如果当前的被释放要置为NULL
            if (pSubPage == m_pSubPage)
            {
                m_pSubPage = NULL;
            }
            // 归还子页面的资源
            pSubPage->setParent(NULL);
            UIManager_PutPrivateDlg(pSubPage);
        }
    }

    m_mapSubPage.clear();
}

// 保存缓存的页面数据
bool CSettingWindow::SaveCacheDlgData()
{
    SETTINGUI_INFO("CSettingWindow::SaveCacheDlgData");
    // 先遍历进行数据校验
    std::map<yl::string, CDlgBaseSubPage *>::iterator it = m_mapSubPage.begin();
    for (/* */; it != m_mapSubPage.end(); ++it)
    {
        CDlgBaseSubPage * pSubPage = it->second;
        if (NULL != pSubPage)
        {
            // 进行数据有效性验证
            InputDataErrorInfo objErrorInfo;
            if (!pSubPage->IsInputDataValid(objErrorInfo))
            {
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
                // 设置数据有问题的界面为当前页面
                if (NULL != m_pMenuListDelegate)
                {
                    m_pMenuListDelegate->SetCurrentItem(it->first.c_str());
                }
#endif
                if (NULL != objErrorInfo.pLineEdit)
                {
                    // 设置错误控件的焦点
                    pSubPage->SetErrorFocus(objErrorInfo.pLineEdit);
                }
                // 给出提示
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(objErrorInfo.strErrorContent), MESSAGEBOX_Cancel);
                return false; // 数据错误需要重新填写
            }
        }
    }

    // 遍历进行数据保存
    it = m_mapSubPage.begin();
    for (/* */; it != m_mapSubPage.end(); ++it)
    {
        CDlgBaseSubPage * pSubPage = it->second;
        if (NULL != pSubPage)
        {
            //保存数据
            if (pSubPage->SavePageData())
            {
                // 保存成功后才进行后续操作
                pSubPage->SaveCompleted();
            }
        }
    }

    return true;
}

void CSettingWindow::OnMenuViewCurrentItemChange(const QString & strClickAction)
{
    OnUIAction(ACTION_SWITCH_PAGE, (void *)&strClickAction);
}

void CSettingWindow::ClearPageData()
{
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    if (m_pMenuListDelegate != NULL)
    {
        m_pMenuListDelegate->ClearAllContent();
    }
#endif
}

bool CSettingWindow::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                     QRect & rtVolume, yl::string & strBGPath) const
{
    if (NULL != m_pSubPage)
    {
        return m_pSubPage->IsShowVolumeBar(iVolumeType, iMin, iMax, rtVolume, strBGPath);
    }
    return false;
}

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
//断开submenu对左侧菜单切换的响应
void CSettingWindow::DisconnectMenuView()
{
    if (NULL != m_pMenuListDelegate)
    {
        disconnect(m_pMenuListDelegate, SIGNAL(ProcessCurrentItem(const QString &)), this,
                   SLOT(OnMenuViewCurrentItemChange(QString)));
    }
}

//连接submenu对左侧菜单切换的响应
void CSettingWindow::ConnectMenuView()
{
    if (NULL != m_pMenuListDelegate)
    {
        connect(m_pMenuListDelegate, SIGNAL(ProcessCurrentItem(const QString &)), this,
                SLOT(OnMenuViewCurrentItemChange(QString)));
    }
}
#endif

void CSettingWindow::UpdateWnd()
{
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    if (SETTING_PAGE_STYLE_WITH_MENU == m_ePageStyle
            && NULL != m_pMenuListDelegate
            && NULL != m_pMenuList
            && m_pMenuList->GetTotalItemCount() == 0)
    {
        m_pMenuListDelegate->RefreshFrameList();
    }
    else
#endif
        if (m_pSubPage != NULL)
        {
            m_pSubPage->OnReShow();
            OnRefreshSoftkey();
        }
        else
        {
            // 三级权限时，SettingUI为空时，按X键没返回上级菜单，缺少focus
            setFocus();
        }
}

bool CSettingWindow::OnBackAndEnterNewPage(const SubMenuData & pageInfo)
{
    OnBtnBackClick();

    return OnOpenpage(pageInfo);
}

bool CSettingWindow::OnOpenpage(const yl::string & strAction)
{
    SubMenuData objData(strAction);
    return OnOpenpage(objData);
}

bool CSettingWindow::OnOpenpage(const SubMenuData & pageInfo)
{
    SETTINGUI_INFO("CSettingWindow::OnOpenpage [%s]", pageInfo.strMenuAction.c_str());
    if (NULL == m_pPageHelper)
    {
        return false;
    }

    yl::string strAction = pageInfo.strMenuAction;
    yl::string strActionType;
    yl::string strContent;

    bool bRet = commonAPI_ParseActionInfo(&strActionType, &strContent, strAction);
    SETTINGUI_INFO("OnOpenpage, parse action [%s] result[%d], strActionType[%s], strContent[%s]",
                   strAction.c_str(), bRet, strActionType.c_str(), strContent.c_str());

    yl::string strSimpleAction = strContent.empty() ? strAction : strContent;
    yl::string strFullSubPageAction = strAction;

    if (strActionType.empty())
    {
        strActionType =  strAction.find(".xml") != string::npos ? kszOpenPageAction : kszOpenSubPageAction;
        strFullSubPageAction = strActionType + "(";
        strFullSubPageAction += strSimpleAction;
        strFullSubPageAction += ")";
    }

    SubMenuData info(strFullSubPageAction);
    info.iFirstSelectItem = pageInfo.iFirstSelectItem;
    info.pAfterSetSubpageData = pageInfo.pAfterSetSubpageData;
    info.pBeforeSetSubpageData = pageInfo.pBeforeSetSubpageData;

    if (strActionType != kszOpenSubPageAction
            && strActionType != kszOpenPageAction
            && strActionType != kszOpenXmlAction)
    {
        //打开非setting模块页面
        Menu_DoAction(strAction);
        return true;
    }

#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
    if (strActionType == kszOpenPageAction || strActionType == kszOpenXmlAction)
    {
        //在窗口中打开新的页面
        return m_pPageHelper->CreatePageMenu(strFullSubPageAction, pageInfo.iFirstSelectItem);
    }
    else if (strActionType == kszOpenSubPageAction)
    {
        return m_pPageHelper->CreateSubPage(info, strSimpleAction);
    }
#else
    //调用外部函数打开新的窗口
    return SettingUI_OpenPage(info, SettingUI_GetDefaultPageStyle() == SETTING_PAGE_STYLE_WITH_MENU);
#endif

    return false;
}

bool CSettingWindow::IsEmpty()
{
    bool bEmpty = GetCurrentSubPage() == NULL;
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    bEmpty = bEmpty && (m_pMenuList == NULL || m_pMenuList->GetTotalItemCount() == 0);
#endif
    return  bEmpty;
}

void CSettingWindow::CheckReturnToIdleFlag()
{

    m_nDlgFlag = 0;
    SetTopWnd();
}

void CSettingWindow::OnBackButtonClick()
{
    if (m_pSubPage != NULL && m_pSubPage->PorcessBackBtnClick())
    {
        return;
    }

    CBaseDialog::OnBackButtonClick();
}

void CSettingWindow::OnHomeButtonClick()
{
    if (m_pSubPage != NULL && m_pSubPage->PorcessHomeBtnClick())
    {
        return;
    }

    CBaseDialog::OnHomeButtonClick();
}

bool CSettingWindow::OnReplaceCurPage(const SubMenuData & subData)
{
    TakeSubPage();
    return OnOpenpage(subData);
}

//设置窗口的提示模式命令
void CSettingWindow::SetHintTitle(const QString & strHint)
{
    if (m_pSubPage != NULL)
    {
        m_pSubPage->SetHintTitle(strHint);
    }
}

//获取窗口提示模式命令
const QString CSettingWindow::GetWindowHint() const
{
    if (m_pSubPage != NULL)
    {
        return m_pSubPage->GetWindowHint();
    }
    else
    {
        return "";
    }
}

void CSettingWindow::CheckCloseTimerFlag()
{
// #if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
//  List_SubPage::const_iterator iter =  m_listSubPage.begin();
//  for (; iter != m_listSubPage.end(); ++iter)
//  {
//      if ((*iter) != NULL)
//      {
//          if ((*iter)->IsCloseTimer())
//          {
//              SetReturnToIdleTimer(false);
//          }
//          else
//          {
//              SetReturnToIdleTimer(true);
//          }
//          return;
//      }
//  }
// #else
//  if (m_pSubPage != NULL)
//  {
//      if (m_pSubPage->IsCloseTimer())
//      {
//          SetReturnToIdleTimer(false);
//      }
//      else
//      {
//          SetReturnToIdleTimer(true);
//      }
//      return;
//  }
// #endif

    m_nDlgFlag = 0;
    SetTopWnd();
}
