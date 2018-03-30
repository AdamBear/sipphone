#include "dlgmenu.h"
#include "baseui/t4xpicpath.h"
#include "baseui/qiconview.h"
#include "setting/include/modmenu.h"
#include "uimanager/uimanager_inc.h"
#include "uimanager/uimanager_header.h"
#include "mainwnd/mainwnd.h"
#include "setting/include/common.h"
#include "setting/include/authoritymessagedefine.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "keymap.h"
#include "qtcommon/qmisc.h"
#include "uikernel/quipushbutton.h"
#include "uikernel/qstatusbar.h"
#include "uikernel/languagehelper.h"
#include "uimanager/rectdefine.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "feature/include/modfeature.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/qwidgetutility.h"
#include "feature/include/modfeature.h"
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/recordmessagedefine.h"
#include "record/include/modrecord.h"
#endif
//图片
#include "imagemanager/modimagemanager.h"

namespace
{
//定义菜单排列2行
#define MENU_ROW_COUNT 2
//定义菜单排列6列
#define MENU_COLUMN_COUNT 4
//定义图标宽 60
#define ICON_WIDTH 77
//定义图标高60
#define ICON_HEIGHT 71
//定义icon view 网格宽128

#define ICON_VIEW_GRID_WIDTH 200
//定义icon view 网格高166
#define ICON_VIEW_GRID_HEIGHT 150
// #define ICON_VIEW_WIDTH (((ICON_VIEW_GRID_WIDTH)*(MENU_COLUMN_COUNT)) + 2)
#define ICON_VIEW_HEIGHT ((ICON_VIEW_GRID_HEIGHT) * 2 + 10)

//定义icon view 坐标x 20
#define ICON_VIEW_X 0
//定义icon view 坐标y 66
#define ICON_VIEW_Y 35
//把-1当成是记忆上级菜单页面数组初始化值
#define BACK_PAGE_NONE -1
// #define MAINMENU_TITLE "Main Menu"

//T3x和T4X图形菜单只有一个界面，事实不需要多次加载数据
//设置一个临时开关
#define FIXED_MENU  1
#define FIXED_MENU_ACTION   "OpenMenu(menu_list.xml)"

#ifdef _T48
#define IF_MENU_NEED_SELECT_EFFECT
#endif

const QColor MENU_SELECT_COLOR(Qt::white);
const QColor MENU_UNSELECT_COLOR(Qt::black);

#define     PAGE_CTRL_BUTTON_WIDTH          54
#define     PAGE_CTRL_BUTTON_HEIGHT         114
#define     PAGE_CTRL_BUTTON_TO_TOP         105
#define     PAGE_CTRL_BUTTON_TO_BOUNDARY    6

#define     PAGE_FLAG_FONT_SIZES            18
#define     PAGE_FLAG_RECT                  QRect(0, 305, PHONE_WIDTH, 20)
}

Q_DECLARE_METATYPE(yl::string);

///////////////////////////////////////////////////////////////////////////////
//消息回调函数
static LRESULT OnFromMenuMsg(msgObject & objMessage)
{
    BASEUI_INFO("OnFromMenuMsg MsgID(%d)", objMessage.message);
    //!!!!!
//  CDlgMenu *pMenuDlg = BaseUI_GetExistMenu();
    CDlgMenu * pMenuDlg = qobject_cast<CDlgMenu *>(UIManager_GetPrivateDlg(DLG_CDlgMenu));
    if (pMenuDlg == NULL)
    {
        chWARNING2(FALSE, "OnFromMenuMsg Menu is inexistence");
        return TRUE;
    }

    switch (objMessage.message)
    {
    /*case LDAP_STATUS_CHANGED:
    case REMOTE_PHONEBOOK_RELOAD:
    case BROADSOFT_PHONEBOOK_RELOAD:
    case BROADSOFT_CALLLOG_RELOAD:*/
    case AUTHORITY_MESSAGE_AUTHORITY_CHANGED:
        {
            // 刷新Menu
            pMenuDlg->UpdateMenuFromMSG();
        }
        break;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#if 0
    case RECORD_STORAGE_STATE_CHANGE:
        {
            if (objMessage.wParam == 0
                    || objMessage.wParam == 1)
            {
                pMenuDlg->UpdateMenuFromMSG();
            }
        }
        break;
#endif
#endif
    default:
        break;
    }
    return 0;
}

CDlgMenu::CDlgMenu(QWidget * parent /*= NULL*/)
    : CBaseDialog(parent)
    , m_pMenuIconView(NULL)
    , m_bPageCtrlInitSucceed(false)
{
    // 设置小窗口的风格为MENU
    SetWindowType(eWT_Menu);

    //设置顶层窗口属性
    SetTopWnd();

    SetDlgGeometry(RECT_MENU);

    SetSoftkeyType(STYLE_TYPE_DIVIDE);

    InitData();

#if FIXED_MENU
    yl::string pstrFixedMenuAction(FIXED_MENU_ACTION);
    SetData(&pstrFixedMenuAction);
#endif

}

//析构
CDlgMenu::~CDlgMenu()
{
}

void CDlgMenu::SetTitle(const QString & strTitle)
{
    SetWindowTitle(strTitle);
    _MainWND.SetTitle(strTitle);
}

void CDlgMenu::InitData()
{
    //成员变量初始化
    m_strCurrentMenuAction = "";
    m_nBackPage = 0;
    //存放上级菜单列表数组初始化为空字符窜
    for (int i = 0; i < MAX_FLOOR_MENU; i++)
    {
        m_aStrMenuAction[i] = "";
        m_aBackPage[i] = BACK_PAGE_NONE;
    }

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    m_bUSBConnect = modRecord_IsStorageConnect();
#endif

    //数据初始化
    m_pMenuIconView = new CIconView(this);
    if (NULL != m_pMenuIconView)
    {
        //设置页面图标排列数
        m_pMenuIconView->setMatrixGrid(MENU_COLUMN_COUNT, MENU_ROW_COUNT);

        //设置图标大小
        m_pMenuIconView->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));

        // 设置网格中有效区域的大小
        m_pMenuIconView->SetItemSize(QSize(ICON_VIEW_GRID_WIDTH, ICON_VIEW_GRID_HEIGHT));

        //点击菜单信号连接　//### V70~t3X 移植修改： 选中了哪个Item可以直接读取，不需要参数传递
        QObject::connect(m_pMenuIconView, SIGNAL(ItemEntered()), this, SLOT(EnterMenuAction()));

        // 设置控件位置
        m_pMenuIconView->setGeometry(ICON_VIEW_X, ICON_VIEW_Y, RECT_MENU.width(), ICON_VIEW_HEIGHT);
        m_pMenuIconView->SetTextColor(MENU_SELECT_COLOR, MENU_UNSELECT_COLOR);
    }

    InitPageCtrlWidget();

    //安装过滤器
    this->installEventFilter(this);
}

//消息注册
void CDlgMenu::RegisterMsg()
{
    //监视LDAP网页更新
    etl_RegisterMsgHandle(LDAP_MESSAGE_BEGIN,
                          LDAP_MESSAGE_END, OnFromMenuMsg);
    //remote phonebook
    etl_RegisterMsgHandle(REMOTE_PHONEBOOK_RELOAD,
                          REMOTE_PHONEBOOK_RELOAD, OnFromMenuMsg);

    //broadsoft contact
    etl_RegisterMsgHandle(BROADSOFT_PHONEBOOK_RELOAD,
                          BROADSOFT_PHONEBOOK_RELOAD, OnFromMenuMsg);

    //broadsoft CallLog
    // 注册网页消息.
    etl_RegisterMsgHandle(BROADSOFT_CALLLOG_RELOAD,
                          BROADSOFT_CALLLOG_RELOAD,
                          OnFromMenuMsg);
    etl_RegisterMsgHandle(AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                          AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                          OnFromMenuMsg);

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#if 0
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE,
                          RECORD_STORAGE_STATE_CHANGE,
                          OnFromMenuMsg);
#endif
#endif
}

void CDlgMenu::UnRegisterMsg()
{
    etl_UnregisterMsgHandle(LDAP_MESSAGE_BEGIN,
                            LDAP_MESSAGE_END, OnFromMenuMsg);
    //remote phonebook
    etl_UnregisterMsgHandle(REMOTE_PHONEBOOK_RELOAD,
                            REMOTE_PHONEBOOK_RELOAD, OnFromMenuMsg);

    //broadsoft contact
    etl_UnregisterMsgHandle(BROADSOFT_PHONEBOOK_RELOAD,
                            BROADSOFT_PHONEBOOK_RELOAD, OnFromMenuMsg);

    //broadsoft CallLog
    // 注册网页消息.
    etl_UnregisterMsgHandle(BROADSOFT_CALLLOG_RELOAD,
                            BROADSOFT_CALLLOG_RELOAD,
                            OnFromMenuMsg);
    etl_UnregisterMsgHandle(AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                            AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                            OnFromMenuMsg);

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#if 0
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE,
                            RECORD_STORAGE_STATE_CHANGE,
                            OnFromMenuMsg);
#endif
#endif
}

//初始化数据
void CDlgMenu::Init()
{
    //注册消息
    RegisterMsg();

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    m_pMenuIconView->ShowPage(1);
#if 0
    bool bUSBConnect = modRecord_IsStorageConnect();

    // USB状态改变重新加载Menu列表
    if (bUSBConnect != m_bUSBConnect)
    {
        m_bUSBConnect = bUSBConnect;
        m_pMenuIconView->clear();
        yl::string pstrFixedMenuAction(FIXED_MENU_ACTION);
        SetData(&pstrFixedMenuAction);
    }
#endif
    RefreshPageCtrl();
#endif
}

//释放数据
void CDlgMenu::Uninit()
{
    // 注销注册的消息
    UnRegisterMsg();

    ClearPageData();
}


void CDlgMenu::ClearPageData()
{
#if !FIXED_MENU
    m_pMenuIconView->clear();
#endif

    //存放上级菜单列表数组初始化为空字符窜
    for (int i = 0; i < MAX_FLOOR_MENU; i++)
    {
        m_aStrMenuAction[i] = "";
        m_aBackPage[i] = BACK_PAGE_NONE;
    }

    m_strCurrentMenuAction = "";
    //记录具体菜单第几页面，用于同级回退
    m_nBackPage = 0;
    //记录当前选中的Row
    m_nSelectedRow = 0;
}

//设置menu小窗口数据
void CDlgMenu::SetData(void * pData/* = NULL*/)
{
    //指针如果为空直接返回
    if (pData == NULL)
    {
        return;
    }

    MenuItemListData menuItemListData;
    yl::string * pstrAction = (yl::string *)pData;

    //存储顶层菜单,首次进入顶层菜单情形
    if (m_aStrMenuAction[0] == "")
    {
        //label显示为main menu.
        SetTitle(LANG_TRANSLATE(TRID_MAIN_MENU));
        m_vecMenuTitle.clear();
        m_vecMenuTitle.push_back((yl::string)(LANG_TRANSLATE(TRID_MAIN_MENU).toUtf8().data()));
        m_aStrMenuAction[0] = *pstrAction;
        m_strCurrentMenuAction = *pstrAction;
    }
    //再次进入顶层菜单情形
    else if (m_aStrMenuAction[0] == *pstrAction)
    {
        ClearPageData();
        SetTitle(LANG_TRANSLATE(TRID_MAIN_MENU));
        m_vecMenuTitle.clear();
        m_vecMenuTitle.push_back((yl::string)(LANG_TRANSLATE(TRID_MAIN_MENU).toUtf8().data()));
        m_strCurrentMenuAction = *pstrAction;
    }

    //两字符串不相等，表示进入子菜单情形
    //而相等时表示返回上一级菜单或者处在顶层菜单状态
    if (m_strCurrentMenuAction != *pstrAction)
    {
        //记录上级菜单项
        for (int i = 0; i < MAX_FLOOR_MENU; i++)
        {
            if (m_aStrMenuAction[i] == "")
            {
                m_aStrMenuAction[i] = m_strCurrentMenuAction;
                break;
            }
        }
        //记录要进入的子菜单的Action
        m_strCurrentMenuAction = *pstrAction;
    }

    // 设置小窗口对应SoftKeybar的数据

#if FIXED_MENU
    //菜单项非空，说明已经加载过数据
    if (m_pMenuIconView->count() != 0)
    {
#ifdef  IF_MENU_NEED_SELECT_EFFECT
        //m_pMenuIconView->setCurrentRow(m_nSelectedRow);
#endif
        return;
    }
#endif
    //向menu control 请求菜单列表数据
    menuItemListData = Menu_GetMenuListByClickAction(m_strCurrentMenuAction);

    //清空之前数据
    m_pMenuIconView->clear();
    //加载数据
    m_pMenuIconView->LoadMenuItemData(&menuItemListData);

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    RefreshPageCtrl();
#endif
}

bool CDlgMenu::OnAction(const QString & strAction)
{
    if (strAction == "Exit")
    {
        BackLastMenu();
    }
    else if (strAction == "Enter")
    {
        EnterMenuAction();
    }
    else
    {
        return false;
    }

    return true;
}

void CDlgMenu::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    if (pMessageBox->GetType() == MESSAGEBOX_PWD
            && pMessageBox->GetResult() == MessageBox_OK
            && pMessageBox->GetExtraData().canConvert<yl::string>())
    {
        EnterPageByAction(pMessageBox->GetExtraData().value<yl::string>());
    }

#ifdef  IF_MENU_NEED_SELECT_EFFECT
    if (pMessageBox->GetResult() != MessageBox_OK)
    {
        m_pMenuIconView->CancelItemSelect();
    }
#endif
}

//插槽：当点击菜单发生的操作
void CDlgMenu::EnterMenuAction()
{
    //用于记录上一级菜单的第几页面数
    if (m_pMenuIconView == NULL || m_pMenuIconView->selectedItems().size() <= 0)
    {
        return;
    }
    QListWidgetItem * pItem = m_pMenuIconView->selectedItems()[0];
    CIconViewItem * pIconViewItem = (CIconViewItem *)pItem;
    QString strEnterAction = pIconViewItem->GetClickAction();

    int i = 0;
    for (/* */; i < MAX_FLOOR_MENU; i++)
    {
        if (m_aStrMenuAction[i] == "")
        {
            m_aBackPage[i] = m_pMenuIconView->GetCurrentPageIndex();
            break;
        }
    }
    //当i=1时，说明是顶级菜单触发了该函数，记录当前选中的Item的位置
    if (i == 1 && m_pMenuIconView->selectedItems().size() > 0)
    {
        m_nSelectedRow = m_pMenuIconView->row(m_pMenuIconView->selectedItems()[0]);
    }

    yl::string strCurrentMenuAction = yl::string(strEnterAction.toUtf8().constData());

    //应当先根据点击的菜单，获取其所需权限，如果不为空，则需要弹出密码输入框
    //弹出密码框后，如果输入密码正确，则正常进入下一级
    //获取当前用户
    if (strCurrentMenuAction == "OpenMenu(advanced_setting_list.xml)")
    {
        USER_TYPE eUserMode = feature_UserModeGetCurrType();
        yl::string strUserName = feature_UserModeGetUserName(eUserMode);

        MessageBox_ShowPasswordBox(this, strUserName.c_str(), QVariant::fromValue(strCurrentMenuAction));
    }
    else
    {
        EnterPageByAction(strCurrentMenuAction);
    }
}

// 进入界面
void CDlgMenu::EnterPageByAction(const yl::string & strAction)
{
    if (NULL == m_pMenuIconView)
    {
        return;
    }

    BASEUI_INFO("EnterPageByAction action[%s]", strAction.c_str());
    //调用menu control，跳转页面
    QString strTitle = m_pMenuIconView->item(m_pMenuIconView->row(
                           m_pMenuIconView->selectedItems()[0]))->text();
    yl::string strMenuTitle = strTitle.toUtf8().data();
    //在这里，会刷新两遍。
    m_vecMenuTitle.push_back(strMenuTitle);

#ifdef  IF_MENU_NEED_SELECT_EFFECT
    if (NULL != m_pMenuIconView)
    {
        // 取消Item的选中效果
        m_pMenuIconView->CancelItemSelect();
    }
#endif
    Menu_DoAction(strAction);
    //_MainWND.SetTitle(strTitle);
}

void CDlgMenu::OnSoftKeyAction(qSoftButton * pbtn)
{
    if (pbtn)
    {
        OnAction(pbtn->m_strButtonName);
    }
}

//插槽：回退到上一级菜单
void CDlgMenu::BackLastMenu()
{
    BASEUI_INFO("CDlgMenu::BackLastMenu m_aStrMenuAction[1] (%s), titlelist size [%d]",
                m_aStrMenuAction[1].c_str(), m_vecMenuTitle.size());
    //如果没有MecuAction的记录，或者也没有菜单名（displayName）的记录，说明已经是最顶层菜单，那么直接返回到Idle界面
    if (m_aStrMenuAction[1] == ""
            || m_vecMenuTitle.begin() == m_vecMenuTitle.end())
    {
        UIManager_PutPrivateDlg(this);
        m_vecMenuTitle.clear();
        return;
    }

    //返回到上一级菜单时，需要把从上级菜单列表去除掉
    for (int i = 1; i < MAX_FLOOR_MENU; i++)
    {
        //查找第一个是空字符串的下标i
        if (m_aStrMenuAction[i] == "")
        {
            //把回退的上级菜单保存在m_strCurrentMenuAction中，目的：使得区分回退菜单还是进入子菜单
            m_strCurrentMenuAction = m_aStrMenuAction[i - 1];
            m_aStrMenuAction[i - 1] = "";
            m_nBackPage = m_aBackPage[i - 1];
            m_aBackPage[i] = BACK_PAGE_NONE;
            m_aBackPage[i - 1] = BACK_PAGE_NONE;
            break;
        }
    }

    BASEUI_INFO("BackLastMenu: menu dlg has NOT quited, more than one storey menu exist! Now load [%s]",
                m_strCurrentMenuAction.c_str());
    //向menu control 请求数据
    MenuItemListData menuItemListData = Menu_GetMenuListByClickAction(m_strCurrentMenuAction);

    //清空之前数据
    m_pMenuIconView->clear();

    //存储Title的堆栈回退到上级菜单
    BackLastTitle();
    //加载数据
    m_pMenuIconView->LoadMenuItemData(&menuItemListData);
    //因为是回到上一级菜单，因此须设置为之前进入的页面菜单名称,解释: *(m_vecMenuTitle.end() - 1为当前菜单名。
    //回退菜单时，记录菜单的数据进行出栈处理

    if (m_nBackPage == BACK_PAGE_NONE)
    {
        //异常情况设置默认第一页面,第一个Item被选中
        m_pMenuIconView->ShowPage(1);
#ifdef IF_MENU_NEED_SELECT_EFFECT
        //m_pMenuIconView->setCurrentRow(0);
#endif
    }
    else
    {
        m_pMenuIconView->ShowPage(m_nBackPage);
#ifdef IF_MENU_NEED_SELECT_EFFECT
        //m_pMenuIconView->setCurrentRow(m_nSelectedRow);
#endif
    }

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    RefreshPageCtrl();
#endif
}

//存储Title的堆栈m_vecMenuTitle回退到上级菜单
void CDlgMenu::BackLastTitle()
{
    if (m_vecMenuTitle.size() >= 2)
    {
        SetTitle((m_vecMenuTitle.end() - 2)->c_str());
        m_vecMenuTitle.erase(m_vecMenuTitle.end() - 1);
    }
}


void CDlgMenu::UpdateMenuFromMSG()
{
    BASEUI_INFO("CDlgMenu::UpdateMenuFromMSG MenuAction(%s)", m_strCurrentMenuAction.c_str());

    // 只有联系人和CallLog的时候才刷新数据
    if (m_strCurrentMenuAction == "OpenMenu(directory_list.xml)"
            || m_strCurrentMenuAction == "OpenMenu(calllog_list.xml)"
            || m_strCurrentMenuAction == "OpenMenu(menu_list.xml)")
    {
        MenuItemListData menuItemListData;
        //重新请求数据链表菜单
        //向menu control 请求菜单列表数据
        menuItemListData = Menu_GetMenuListByClickAction(m_strCurrentMenuAction);
        if (NULL == menuItemListData.m_pMenuItemList)
        {
            BASEUI_WARN("CDlgMenu::UpdateMenuFromMSG: GetMenuListByClickAction Error! point is NULL");
            return;
        }

        if (menuItemListData.m_pMenuItemList->size() < 2)
        {
            if (feature_UserModeIsOpen())
            {
                //清空之前数据，http://10.2.1.199/Bug.php?BugID=44381
                m_pMenuIconView->clear();
            }
            //回退到上一级菜单
            BackLastMenu();
        }
        else
        {
            if (m_pMenuIconView != NULL)
            {
                //清空之前数据
                m_pMenuIconView->clear();
                //加载数据
                m_pMenuIconView->LoadMenuItemData(&menuItemListData);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
                m_bUSBConnect = modRecord_IsStorageConnect();
                RefreshPageCtrl();
#endif
            }
            if (!this->isHidden())
            {
                this->update();
            }
        }
    }
}

//子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
QPixmap CDlgMenu::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_MENU_BACKGROUND);
}

//事件过滤器
bool CDlgMenu::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    if (isChildOf(pObject, this)
            || isChildOf(this, pObject))
    {
        if (pEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent * pKeyEvent  = (QKeyEvent *)pEvent;
            if (pKeyEvent->key() == PHONE_KEY_CANCEL)
            {
                BackLastMenu();
                return true;
            }
            else if (PHONE_KEY_LEFT == pKeyEvent->key())
            {
                OnClickLeftButton();
                return true;
            }
            else if (PHONE_KEY_RIGHT == pKeyEvent->key())
            {
                OnClickRightButton();
                return true;
            }
        }
        //showEvent
        if (pEvent->type() == QEvent::Show)
        {
            // lvl当前页面层数
            // lvl=1：从IdleScreen中进入到主菜单
            // lvl>1:从子页面回退到上级菜单
            int lvl = m_vecMenuTitle.size();
            if (lvl > 1)
            {
                BackLastTitle();
            }

            setFocus();
        }

        //HideEvent
        if (pEvent->type() == QEvent::Hide)
        {
            if (NULL != m_pMenuIconView)
            {
                // 取消Item的选中效果
                m_pMenuIconView->CancelItemSelect();
            }
        }


    }
    return CBaseDialog::eventFilter(pObject, pEvent);
}

void CDlgMenu::RefreshPageCtrl()
{
    if (NULL == m_pMenuIconView || !m_bPageCtrlInitSucceed)
    {
        return;
    }

    if (m_pMenuIconView->GetLastPageIndex() <= 1)
    {
        m_pBtnLeft->hide();
        m_pBtnRight->hide();
        m_pPageFlag->hide();
    }
    else
    {
        SetPageFlag(m_pMenuIconView->GetLastPageIndex(), m_pMenuIconView->GetCurrentPageIndex());

        m_pBtnLeft->show();
        m_pBtnRight->show();
        m_pPageFlag->show();
    }
}
void CDlgMenu::InitPageCtrlWidget()
{
    m_pBtnLeft = new qUIPushButton(this);
    m_pBtnRight = new qUIPushButton(this);
    m_pPageFlag = new QLabel(this);

    m_bPageCtrlInitSucceed = true;

    if (NULL == m_pBtnLeft
            || NULL == m_pBtnRight
            || NULL == m_pPageFlag)
    {
        m_bPageCtrlInitSucceed = false;
        return;
    }

    m_pBtnLeft->setIconSize(QSize(PAGE_CTRL_BUTTON_WIDTH, PAGE_CTRL_BUTTON_HEIGHT));
    m_pBtnLeft->setGeometry(PAGE_CTRL_BUTTON_TO_BOUNDARY, PAGE_CTRL_BUTTON_TO_TOP,
                            PAGE_CTRL_BUTTON_WIDTH, PAGE_CTRL_BUTTON_HEIGHT);
    m_pBtnLeft->set3StatePic(PIC_DLG_MENU_PAGE_CTRL_LEFT_NORMAL, PIC_DLG_MENU_PAGE_CTRL_LEFT_FOCUS,
                             PIC_DLG_MENU_PAGE_CTRL_LEFT_NORMAL);

    m_pBtnRight->setIconSize(QSize(PAGE_CTRL_BUTTON_WIDTH, PAGE_CTRL_BUTTON_HEIGHT));
    m_pBtnRight->setGeometry(PHONE_WIDTH - PAGE_CTRL_BUTTON_WIDTH - PAGE_CTRL_BUTTON_TO_BOUNDARY,
                             PAGE_CTRL_BUTTON_TO_TOP, PAGE_CTRL_BUTTON_WIDTH, PAGE_CTRL_BUTTON_HEIGHT);
    m_pBtnRight->set3StatePic(PIC_DLG_MENU_PAGE_CTRL_RIGHT_NORMAL, PIC_DLG_MENU_PAGE_CTRL_RIGHT_FOCUS,
                              PIC_DLG_MENU_PAGE_CTRL_RIGHT_NORMAL);

    connect(m_pBtnLeft, SIGNAL(clicked()), this, SLOT(OnClickLeftButton()));
    connect(m_pBtnRight, SIGNAL(clicked()), this, SLOT(OnClickRightButton()));

    QPalette objpalette = m_pPageFlag->palette();
    objpalette.setColor(QPalette::WindowText, Qt::white);
    m_pPageFlag->setPalette(objpalette);

    QFont objFont = m_pPageFlag->font();
    objFont.setPixelSize(PAGE_FLAG_FONT_SIZES);
    m_pPageFlag->setFont(objFont);

    m_pPageFlag->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    m_pPageFlag->setGeometry(PAGE_FLAG_RECT);
}

void CDlgMenu::OnClickLeftButton()
{
    if (!m_bPageCtrlInitSucceed)
    {
        return;
    }

    m_pMenuIconView->PageUp();
    RefreshPageCtrl();
}

void CDlgMenu::OnClickRightButton()
{
    if (!m_bPageCtrlInitSucceed)
    {
        return;
    }

    m_pMenuIconView->PageDown();
    RefreshPageCtrl();
}

void CDlgMenu::SetPageFlag(int nRang, int nCurPage)
{
    if (!m_bPageCtrlInitSucceed)
    {
        return;
    }

    char szBuffer[32];

    snprintf(szBuffer, sizeof(szBuffer), "%d/%d", nCurPage, nRang);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    m_pPageFlag->setText(QLatin1String(szBuffer));
}
