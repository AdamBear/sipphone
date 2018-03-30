#include "cdlgbasesubpage.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "keymap.h"
#include "baseui/t4xpicpath.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "mainwnd/mainwnd.h"
#include "uimanager/rectdefine.h"
#include "uicommon/uimanager/moduimanager.h"
#include "feature/include/modfeature.h"
#include "settingui/src/settingpage.h"
#include "messagebox/messageboxbase.h"
#include "baselibs/include/configparser/modconfigparser.h"
#include <configiddefine.h>
#include "baseui/framelist.h"

#include "keyguard/include/modkeyguard.h"

namespace
{
static const QSize SETTING_SUBPAGE_SIZE(536, 336);
}

////////////////////////////////////////////////////

CDlgBaseSubPage::CDlgBaseSubPage(QWidget * parent)
    : CBaseDialog(parent)
    , m_bCloseVerticalScrollBar(false)
    , m_ePageMode(PAGE_MODE_SETTING)
    , m_bCache(false)
    , m_pGeneralFrame(NULL)
    , m_bPageEnable(true)
{
    //给自己安装过滤器
    this->installEventFilter(this);

    resize(SETTING_SUBPAGE_SIZE.width(), SETTING_SUBPAGE_SIZE.height());
    this->setFocusPolicy(Qt::TabFocus);
    this->setFocus();

    m_pListDelegate = new SubpageListDeleagate();
    m_pGeneralFrame = new CFrameList(this);

    if (m_pGeneralFrame != NULL && NULL != m_pListDelegate)
    {
        m_pListDelegate->setParent(this);
        //绑定列表
        m_pListDelegate->BindFramelist(m_pGeneralFrame);
        //settingui的常规色、点击色、点击背景、选中色、选中背景都是自己实现，framelist的机制无法满足。
        m_pListDelegate->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG);
        m_pListDelegate->InitSettingListStyle();
        m_pListDelegate->SetItemParent(this);
    }
    //SetItemClickBackGround(PIC_GENERAL_LIST_ITEM_CLICK_BG);
    QObject::connect(m_pListDelegate, SIGNAL(listItemAction(int, QString)), this,
                     SLOT(OnListItemAction(int, QString)));
}

CDlgBaseSubPage::~CDlgBaseSubPage()
{
    // 卸载所有输入法
    UninstallAllIME();

    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->SetDelegate(NULL);
    }

    if (NULL != m_pListDelegate)
    {
        m_pListDelegate->UnBindFramelist();
    }
}

void CDlgBaseSubPage::SetDefaultIndex(int nIndex /* = 0 */, bool bEnter /*= false*/)
{
    if (NULL == m_pListDelegate)
    {
        return;
    }
    m_pListDelegate->SelectIndex(nIndex, bEnter);
}

// 设置子页面的模式
void CDlgBaseSubPage::SetPageMode(PAGE_MODE ePageMode)
{
    m_ePageMode = ePageMode;

    // 根据不同的mode初始化页面的属性
    switch (m_ePageMode)
    {
    case PAGE_MODE_SETTING:
        {
            SetBeCache(false);
        }
        break;
    case PAGE_MODE_ZERO:
        {
            SetBeCache(true);
        }
        break;
    default:
        break;
    }
}

// 输入预编辑内容
void CDlgBaseSubPage::ReturnAllEdit()
{
    QWPVector::iterator it = m_vecWidgetPair.begin();
    for (; it != m_vecWidgetPair.end(); ++it)
    {
        QWidget * pChildWidget = it->second;

        if (pChildWidget != NULL
                && (pChildWidget->inherits("QLineEdit")
                    || pChildWidget->inherits("QTextEdit")))
        {
            if (InputPreeditContent(pChildWidget))
            {
                // 一个界面只有一个输入框处于预编辑状态
                break;
            }
        }
    }
}

void CDlgBaseSubPage::UninstallAllIME()
{
    //处理m_vecWidgetPair
    for (QWPVector::iterator iter = m_vecWidgetPair.begin();
            iter != m_vecWidgetPair.end(); ++iter)
    {
        QWidget * pWidget = iter->second;

        if (pWidget != NULL
                && (pWidget->inherits("QLineEdit")
                    || pWidget->inherits("QTextEdit")))
        {
            qUninstallIME(pWidget);
        }
    }
}

bool CDlgBaseSubPage::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    // 默认为有效
    return true;
}

// 设置子页面的SoftKey
bool CDlgBaseSubPage::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    return false;
}

// 设置子页面不可编辑时的SoftKey
bool CDlgBaseSubPage::GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    return false;
}

// 下面三个虚函数用于屏蔽CBaseDial的函数
void CDlgBaseSubPage::Init()
{
}

void CDlgBaseSubPage::Uninit()
{

}

void CDlgBaseSubPage::AdjustWgtPair(QWPVector & vec, bool bAddIndex, bool bAddMark
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
                                    , bool bChangeStyleOnPress
#endif
                                    , bool bChangeStyleOnSelect
#endif
                                   )
{
    if (!bAddIndex || !bAddMark
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
            || !bChangeStyleOnPress
#endif
            || !bChangeStyleOnSelect
#endif
       )
    {
        QWPVector::iterator iter = vec.begin();
        for (; iter !=  vec.end(); ++iter)
        {
            if (!bAddIndex)
            {
                iter->bAddIndex = false;
            }
            if (!bAddMark)
            {
                iter->bAddMark = false;
            }
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
            if (!bChangeStyleOnPress)
            {
                iter->bChangeStyleOnPress = false;
            }
#endif
            if (!bChangeStyleOnSelect)
            {
                iter->bChangeStyleOnSelect = false;
            }
#endif
            // 安装全键盘
            if (iter->second != NULL
                    && (iter->second->inherits("QLineEdit") || iter->second->inherits("QTextEdit")))
            {
                qUseKeyBoardFromIME(iter->second, INPUT_DIALOG_NEWLINE, false);
            }
        }
    }
    else
    {
        QWPVector::iterator iter = vec.begin();
        for (; iter != vec.end(); ++iter)
        {
            // 安装全键盘
            if (iter->second != NULL
                    && (iter->second->inherits("QLineEdit") || iter->second->inherits("QTextEdit")))
            {
                qUseKeyBoardFromIME(iter->second, INPUT_DIALOG_NEWLINE, false);
            }
        }
    }
}

void CDlgBaseSubPage::OnKeyboardShow()
{
    if (m_pListDelegate != NULL)
    {
        if (!(m_pListDelegate->IsListReady()))
        {
            return;
        }
        m_pListDelegate->AdjustFrameList(false);
    }
}

void CDlgBaseSubPage::OnKeyboardHide()
{
    if (m_pListDelegate != NULL)
    {
        m_pListDelegate->AdjustFrameList(true);
    }
}

bool CDlgBaseSubPage::isListReady()
{
    if (NULL != m_pListDelegate)
    {
        return m_pListDelegate->IsListReady();
    }

    return false;
}

void CDlgBaseSubPage::FilterItemByUserLevel(QWPVector & vec)
{
    QWPVector::iterator iter = vec.begin();
    while (iter != vec.end())
    {
        if (feature_UserModeConfigItemShow(iter->strAuthorId))
        {
            // 权限是否为只读
            if (iter->second != NULL
                    && feature_UserModeConfigItemReadOnly(iter->strAuthorId))
            {
                iter->second->setEnabled(false);
            }
        }
        else
        {
            // 不显示则隐藏控件
            if (iter->second != NULL)
            {
                iter->second->hide();
            }

            iter = vec.erase(iter);
            continue;
        }

        ++iter;
    }

    OnFilterWPVec();
}

//三级权限过滤之后的特殊处理
void CDlgBaseSubPage::OnFilterWPVec()
{
    //
}

void CDlgBaseSubPage::AddWidget(QWPVector & vec, bool bAddIndex /* = true */,
                                bool bAddMark /* = true */
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
                                , bool bChangeStyleOnPress/* = false*/
#endif
                                , bool bChangeStyleOnSelect/* = false*/
#endif
                               )
{
    if (NULL == m_pListDelegate)
    {
        return;
    }

    AdjustWgtPair(vec, bAddIndex, bAddMark
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
                  , bChangeStyleOnPress
#endif
                  , bChangeStyleOnSelect
#endif
                 );

    FilterItemByUserLevel(vec);

    m_pListDelegate->SetContent(&vec);
}

bool CDlgBaseSubPage::ShouldBeCache() const
{
    return m_bCache;
}

void CDlgBaseSubPage::SetBeCache(bool bCache)
{
    m_bCache  = bCache;
}

void CDlgBaseSubPage::SetErrorFocus(QLineEdit * pLineEdit)
{
    if (NULL == m_pListDelegate)
    {
        return;
    }
    m_pListDelegate->SetErrorFocus(pLineEdit);
}

void CDlgBaseSubPage::SetData(void * pData/* = NULL*/)
{
    //
}

// 设置SubPageAction
void CDlgBaseSubPage::SetOpenSubPageAction(const yl::string & strSubPageAction)
{
    m_strSubPageAction = strSubPageAction;

    SetData(&m_strSubPageAction);
}

bool CDlgBaseSubPage::IsCloseVerticalScrollBar() const
{
    return m_bCloseVerticalScrollBar;
}

bool CDlgBaseSubPage::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (!CheckEventValid(pObj, pEvent))
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::Show:
        {
            //当前选中控件获取焦点
            if (NULL != m_pListDelegate)
            {
                m_pListDelegate->RefreshFocus();
            }

            //更新softkey IM/Switch
            QWidget * pWgt = focusWidget();
            if (pWgt != NULL
                    && isChildOf(pWgt, m_pGeneralFrame))
            {
                _MainWND.DisplayIMESoftkey(pWgt);
            }
        }
        break;

    case QEvent::FocusOut:
        {
            // 输入密码有预输入，点击其他控件失焦先提交预输入
            if (FullKeyboardEnabled() && pObj->inherits("QLineEdit"))
            {
                QLineEdit *pEdit = static_cast<QLineEdit *>(pObj);
                qReturnEdit(pEdit);
            }
        }
        break;
    //键盘输入事件
    case  QEvent::KeyPress:
        {
            //按键处理顺序：subpage->listDeletate->list
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_DOWN:
            case PHONE_KEY_UP:
                {
                    if (NULL == m_pListDelegate || NULL == m_pGeneralFrame || m_pGeneralFrame->GetTotalItemCount() < 1)
                    {
                        break;
                    }

                    QWidget * pwgt = focusWidget();
                    if (pwgt != NULL
                            && pwgt->inherits("QComboBox"))
                    {
                        QComboBox * pComBox = (QComboBox *) pwgt;
                        QAbstractItemView * pView = pComBox->view();
                        if (pView == NULL || !pView->isVisible())
                        {
                            break;
                        }
                        return false;
                    }
                }
                break;
            case  PHONE_KEY_OK:
                {
                    return false;
                }
                break;
            default:
                break;
            }

            if (NULL != m_pListDelegate && m_pListDelegate->ProcessKeyPress(pKeyEvent->key()))
            {
                return true;
            }

            if (m_pGeneralFrame != NULL && m_pGeneralFrame->isVisible())
            {
                return m_pGeneralFrame->eventFilter(pObj, pEvent);
            }
        }
        break;
    case  QEvent::KeyRelease:
        {
            QKeyEvent* pKeyEvent = (QKeyEvent*)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_DOWN:
            case PHONE_KEY_UP:
                {
                    if (qGetIsShowWnd())
                    {
                        if (NULL != m_pListDelegate)
                        {
                            m_pListDelegate->AdjustFrameList();
                        }
                    }

                }
                break;
            default:
                break;
            }
        }
        break;
    //case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        {
            if (FullKeyboardEnabled())
            {
                if (pObj == this || pObj == m_pGeneralFrame)
                {
                    SETTINGUI_INFO("Subpage MouseButton filt");
                    pEvent->accept();
                    qSetHideKeyboard();
                    return true;
                }

                if (NULL != m_pListDelegate && m_pListDelegate->ProcessMouse(pObj))
                {
                    return true;
                }
            }
        }
        break;
    default:
        break;
    }
    return CBaseDialog::eventFilter(pObj, pEvent);
}

void CDlgBaseSubPage::UpdateWnd()
{
    if (NULL == m_pListDelegate)
    {
        return;
    }

    m_pListDelegate->UpdateList();

    if (m_vecWidgetPair.size() > 0)
    {
        m_pListDelegate->UpdateItemStatus(m_bPageEnable);
    }

    emit RefreshSoftkey();
}

void CDlgBaseSubPage::MakeReadyForRelayout()
{
    if (NULL == m_pListDelegate)
    {
        return;
    }
    //清除列表数据，但不删除成员控件，以便重新布局
    m_pListDelegate->ClearContent(false);
    m_vecWidgetPair.clear();
}

void CDlgBaseSubPage::ClearAllContent()
{
    //清除所有内容，包括delete成员控件，执行之后，页面成员控件指针必须赋值为NULL，否则出现野指针
    //适用于成员指针是加载数据时动态创建的情况
    m_vecWidgetPair.clear();
    if (NULL == m_pListDelegate)
    {
        return;
    }
    m_pListDelegate->ClearContent(true);
}

void CDlgBaseSubPage::ResetSoftKey(CArraySoftKeyBarData & objWndData)
{
    QWidget * pWgt = focusWidget();
    bool bShowIme = (NULL != pWgt && (pWgt->inherits("QLineEdit") || pWgt->inherits("QTextEdit"))
                     && pWgt->isEnabled());

    CArraySoftKeyBarData::iterator iter = objWndData.begin();
    while (iter != objWndData.end())
    {
        if (iter->m_strSoftkeyAction.empty())
        {
            iter = objWndData.erase(iter);
            continue;
        }

        if (!bShowIme && (iter->m_strSoftkeyAction == IME_ACTION
                          || iter->m_strSoftkeyAction == IME_DEL_ACTION))
        {
            iter = objWndData.erase(iter);
            continue;
        }
        ++iter;
    }
}



// 判断是否已经存在于列表当中
bool CDlgBaseSubPage::IsWidgetInList(QWidget * pWidget)
{
    if (NULL == pWidget)
    {
        return false;
    }
    QWPVector::iterator iter = m_vecWidgetPair.begin();
    // 遍历容器 查找控件指针
    for (/* */; iter !=  m_vecWidgetPair.end(); ++iter)
    {
        if (isChildOf(pWidget, iter->second))
        {
            // 找到，则表示已存在
            return true;
        }
    }
    return false;
}

bool CDlgBaseSubPage::IsTopPage()
{
    if (this == UIManager_GetTopWindow())
    {
        //这个判断逻辑只适合t46
        return true;
    }

    CBaseDialog * pTop = UIManager_GetTopWindow();
    if (pTop != NULL && pTop->inherits("CSettingWindow"))
    {
        CSettingWindow * pPage = (CSettingWindow *)pTop;
        if (pPage->GetCurrentSubPage() == this)
        {
            return true;
        }
    }

    return false;
}

void CDlgBaseSubPage::SetCurrentIndex(int iIndex)
{
    if (NULL == m_pListDelegate)
    {
        return;
    }
    m_pListDelegate->SetCurrentIndex(iIndex);
}

int CDlgBaseSubPage::GetCurrentIndex()
{
    if (NULL == m_pListDelegate)
    {
        return -1;
    }

    return m_pListDelegate->GetCurrentIndex();
}

bool CDlgBaseSubPage::CheckEventValid(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject
            || NULL == pEvent)
    {
        return false;
    }

    // 按键事件以及自身控件的事件，为有效事件
    return (isChildOf(pObject, this)
            || pEvent->type() == QEvent::KeyPress
            || pEvent->type() == QEvent::KeyRelease);
}

void CDlgBaseSubPage::OnCurrentIndexChanged(int iDataIndex)
{
    emit RefreshSoftkey();
}

void CDlgBaseSubPage::OnListItemClicked(QString strItemAction)
{
    emit ProcessPageAction(strItemAction);
}

void CDlgBaseSubPage::OnListItemAction(int eActionType, QString strItemAction)
{
    if (eActionType == LIST_ITEM_ACTION_SELECT)
    {
        if (!strItemAction.isEmpty())
        {
            //此操作相当于点击或按OK操作
            OnListItemClicked(strItemAction);
        }

        if (NULL != m_pListDelegate && this->isVisible())
        {
            int iListFocusIndex = m_pListDelegate->GetFocusItemDataIndex();
            OnCurrentIndexChanged(iListFocusIndex);
        }
        return;
    }
    else if (eActionType == LIST_ITEM_ACTION_FOCUS)
    {
        OnCurrentIndexChanged(m_pListDelegate->GetFocusItemDataIndex());
        return;
    }

    OnExtraAction(eActionType);

}

QString CDlgBaseSubPage::GetCurrentItemAction()
{
    if (NULL != m_pListDelegate)
    {
        return m_pListDelegate->GetCurrentItemAction();
    }

    return "";
}

void CDlgBaseSubPage::SelectIndex(int iDataIndex)
{
    if (NULL != m_pListDelegate)
    {
        m_pListDelegate->SelectIndex(iDataIndex);
    }
}

void CDlgBaseSubPage::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    switch (pMessageBox->GetID())
    {
    case MSG_ID_SHOW_NOTE_BEFORE_BACK:
        {
            emit BtnBackClick();
        }
        break;
    default:
        {
            ProcessMsgBoxCallBack(pMessageBox);
        }
        break;
    }
}

void CDlgBaseSubPage::OnReShow()
{
    if (NULL != m_pGeneralFrame && NULL != m_pListDelegate)
    {
        m_pListDelegate->RefreshFocus();
    }
    else
    {
        this->setFocus(Qt::ActiveWindowFocusReason);
    }
}


void CDlgBaseSubPage::SetConfigedIndex(int iIndex)
{
    if (NULL != m_pListDelegate)
    {
        m_pListDelegate->SetConfigedIndex(iIndex);
    }
}

bool CDlgBaseSubPage::IsHasValidItem()
{
    return NULL == m_pGeneralFrame ? false : m_pGeneralFrame->GetTotalDataCount() > 0;
}

void CDlgBaseSubPage::SetItemClickBackGround(const char * pszTopBG, const char * pszMidBG,
        const char * pszBottomBG)
{
    if (NULL != m_pListDelegate)
    {
        m_pListDelegate->SetItemClickBG(pszTopBG, pszMidBG, pszBottomBG);
    }
}

void CDlgBaseSubPage::SetNoClickBG()
{
    if (NULL != m_pListDelegate)
    {
        m_pListDelegate->SetNoClickBG();
    }
}

void CDlgBaseSubPage::ExtraDialogInitData()
{

}

// 是否允许键盘事件通过
bool CDlgBaseSubPage::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
{
    // 键盘锁开启的时候才进行判断
    if (!keyGuard_IsLock())
    {
        return true;
    }

    switch (keyGuard_GetKeyGuardType())
    {
    // menu key 只锁定函数入口不锁定按键
    case KG_MENU_KEY:
        {
            return true;
        }
        break;

    case KG_FUN_KEY:
    case KG_ALL_KEYS:
        {
            return false;
        }
        break;


    default:
        break;
    }

    // 键盘锁开启时默认不允许其他键盘事件通过
    return false;
}

// 是否允许鼠标事件通过
bool CDlgBaseSubPage::AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent)
{
    // 键盘锁开启的时候才进行判断
    if (!keyGuard_IsLock())
    {
        return true;
    }

    switch (keyGuard_GetKeyGuardType())
    {
    // menu key 只锁定函数入口不锁定按键
    case KG_MENU_KEY:
        {
            return true;
        }
        break;

    case KG_FUN_KEY:
    case KG_ALL_KEYS:
        {
            return false;
        }
        break;


    default:
        break;
    }

    // 键盘锁开启时默认不允许其他键盘事件通过
    return false;
}

int CDlgBaseSubPage::GetFocusItemFlag()
{
    int iIndex = m_pGeneralFrame->GetFocusItemDataIndex();
    int iSize = m_vecWidgetPair.size();

    if (iSize == 0 || iIndex >= iSize || iIndex < 0)
    {
        return setting_invalid_flag;
    }

    return m_vecWidgetPair[iIndex].iFlag;
}
