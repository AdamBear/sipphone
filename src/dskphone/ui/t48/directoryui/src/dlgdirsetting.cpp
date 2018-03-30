#include "baseui/framelist.h"
#include "baseui/treelist.h"

#include "dlgdirsetting.h"

#include "directorycommon.h"
#include "settingdelegate.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "groupedittitle.h"
#include "groupedittreeitem.h"
#include "baseui/t4xpicpath.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "uicommon.h"
#include "keymap.h"
#include "remotecontactcontroler.h"
#include "bsftcontactcontroler.h"
#include "inputmethod/inputmethod.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/rectdefine.h"
#include "mainwnd/mainwnd.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"
#include "uimanager/moduimanager.h"
//图片
#include "imagemanager/modimagemanager.h"

namespace
{
#define DLG_DIR_FRAME_LIST_FONT_SIZE_NOTE 16
#define DLG_DIR_SETTING_ITEM_OFFSET 4
}

CDlgDirSetting::CDlgDirSetting(QWidget * parent/* = NULL*/)
    : CBaseDialog(parent)
    , m_pFrameList(NULL)
    , m_pTreeList(NULL)
{
    SetWindowType(eWT_Directory);
    SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_DIRECTORY);

    SetSoftkeyType(STYLE_TYPE_DIVIDE);
    SetWindowTitle(LANG_TRANSLATE(TRID_CONTACT_SETTING));

    m_pTreeList = new CTreeList(this);

    if (NULL != m_pTreeList)
    {
        CGroupEditTitlePtr pTitle = new CGroupEditTitle;

        m_pTreeList->SetFrameListTitle(pTitle);

        m_pTreeList->SetRect(RECT_DIR_GROUP);
        m_pTreeList->SetGridLine(true);
        m_pTreeList->SetLinePosY(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pTreeList->SetLineHeight(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pTreeList->SetItemSpace(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pTreeList->SetLineImage(PIC_GENERAL_LIST_ITEM_LEFT_LINE);
        m_pTreeList->SetItemSelectBG(NULL, PIC_GENERAL_TREE_ITEM_SELECT,
                                     PIC_GENERAL_TREE_ITEM_BOTTOM_SELECT);
        m_pTreeList->SetItemClickBG(NULL, PIC_GENERAL_TREE_ITEM_CLICK, PIC_GENERAL_TREE_ITEM_BOTTOM_CLICK);
        m_pTreeList->show();
        m_pTreeList->raise();
    }

    m_pFrameList = new CFrameList(this);

    if (NULL != m_pFrameList)
    {
        m_pFrameList->SetRect(RECT_DIR_CONTACT);
        m_pFrameList->SetContentPos(CONTACT_LIST_ITEM_CONTENT_LEFT_MARGIN,
                                    CONTACT_LIST_ITEM_CONTENT_RIGHT_MARGIN);
        m_pFrameList->SetGridLine(true);
        m_pFrameList->SetLinePosY(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFrameList->SetLineHeight(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFrameList->SetItemSpace(DEFAULT_RIGHT_LIST_ITEM_SPACE);
        m_pFrameList->SetLineImage(PIC_GENERAL_LIST_ITEM_RIGHT_LINE);
        m_pFrameList->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP, PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                     NULL);
        m_pFrameList->SetNote(LANG_TRANSLATE(TRID_NONE));
        m_pFrameList->SetNoteFontSize(DLG_DIR_FRAME_LIST_FONT_SIZE_NOTE);
        m_pFrameList->show();
        m_pFrameList->raise();
    }
}

CDlgDirSetting::~CDlgDirSetting(void)
{
}

void CDlgDirSetting::Init()
{
}

void CDlgDirSetting::Uninit()
{
    if (NULL != m_pTreeList)
    {
        m_pTreeList->SetDelegate(NULL);
    }

    if (NULL != m_pFrameList)
    {
        m_pFrameList->SetDelegate(NULL);
    }
}

void CDlgDirSetting::UpdateWnd()
{
    g_DirectoryMgr.OnUIShow(DIR_UI_STATE_SETTING, m_pTreeList, m_pFrameList);
}

void CDlgDirSetting::OnFocusChanged(bool bFocused)
{
    if (!bFocused)
    {
        g_DirectoryMgr.OnUIHide(m_pTreeList, m_pFrameList);
    }
}

CTreeList * CDlgDirSetting::GetTreeList() const
{
    return m_pTreeList;
}

CFrameList * CDlgDirSetting::GetFrameList() const
{
    return m_pFrameList;
}

void CDlgDirSetting::SetDirType(int nDirType)
{
    if (CALL_LOG_TYPE_LOCAL == nDirType || CALL_LOG_TYPE_BROADSOFT == nDirType
#if IF_FEATURE_METASWITCH_CALLLOG
            || CALL_LOG_TYPE_MTSW == nDirType
#endif
       )
    {
        SetWindowTitle(LANG_TRANSLATE(TRID_HISTORY_SETTING));
    }
    else
    {
        SetWindowTitle(LANG_TRANSLATE(TRID_CONTACT_SETTING));
    }
}

bool CDlgDirSetting::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    if (MessageBox_GetMessageBoxCount() > 0)
    {
        return false;
    }

    if (NULL != pEvent)
    {
        switch (pEvent->type())
        {
        case QEvent::Show:
            {
                QWidget * pFocusWidget = focusWidget();

                if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
                {
                    _MainWND.DisplayIMESoftkey(pFocusWidget);
                }
            }
            break;
        case QEvent::MouseButtonPress:
            {
                QWidget* pFocusWidget = QApplication::focusWidget();

                if (NULL != pFocusWidget && pFocusWidget->parent() != pObject
                        && pFocusWidget != pObject && pFocusWidget->inherits("QLineEdit")
                        && (!pObject->inherits("qButtonItem"))
                        && (!FullKeyboardEnabled()
                            || !(pObject->inherits("CBaseInputDlg")
                                 || pObject->inherits("CInputBoxButton")
                                )
                           )
                   )
                {
                    pFocusWidget->clearFocus();
                    return true;
                }
            }
            break;
        case QEvent::KeyPress://键盘输入事件
            {
                if (NULL != m_pFrameList && m_pFrameList->eventFilter(pObject, pEvent))
                {
                    return true;
                }

                QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
                int nKey = pKeyEvent->key();

                switch (nKey)
                {
                case PHONE_KEY_OK:
                    {
                        if (FullKeyboardEnabled())
                        {
                            if (NULL == m_pTreeList)
                            {
                                break;
                            }

                            int nFocusIndex = GetFocusItemIndex();
                            CListItemPtr pItem = m_pTreeList->GetItemByDataIndex(nFocusIndex);
                            CGroupEditTreeItem* pTreeItem = CGroupEditTreeItem::GetGroupEditTreeItem(pItem);

                            if (NULL != pTreeItem && pTreeItem->SaveGroup())
                            {
                                return true;
                            }
                        }
                    }
                    break;
                case PHONE_KEY_CANCEL:
                    {
                        QWidget * pFocusWidget = QApplication::focusWidget();

                        if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
                        {
                            pFocusWidget->clearFocus();
                            return true;
                        }

                        UIManager_PutPrivateDlg(this);
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
    }

    return CBaseDialog::eventFilter(pObject, pEvent);
}

//子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
QPixmap CDlgDirSetting::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_BACKGROUND);
}

bool CDlgDirSetting::OnAction(const QString & strAction)
{
    bool bProcess = true;

    if (strAction == IME_ACTION)
    {
        OnBtnIMEClick();
    }
    else if (strAction == IME_DEL_ACTION)
    {
        OnBtnDeleteClick();
    }
    else if (strAction == ACTION_BACK)
    {
        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
        CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

        if (NULL != pSettingDelegate)
        {
            return pSettingDelegate->OnBack();
        }

        return false;
    }
    else
    {
        bProcess = false;
    }

    return bProcess;
}

// 焦点变化刷新SoftKey
void CDlgDirSetting::OnCtrlFocusChanged()
{
    QWidget * pFocusWidget = focusWidget();

    InitSoftKeyData();

    if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
    {
        m_SoftkeyData[0].m_strSoftkeyAction = IME_ACTION;
        m_SoftkeyData[1].m_strSoftkeyAction = IME_DEL_ACTION;
        m_SoftkeyData[1].m_strIconPath = PIC_SOFTKEY_DELETE;

        _MainWND.DrawSoftkey(this);
        _MainWND.DisplayIMESoftkey(pFocusWidget);
    }
    else
    {
        _MainWND.DrawSoftkey(this);
    }
}

// 点击IME
void CDlgDirSetting::OnBtnIMEClick()
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL == pFocusWidget)
    {
        return;
    }

    _MainWND.SwitchNextIMEName(pFocusWidget);

    yl::string strIMEName = fromQString(qGetCurrentIMEName(pFocusWidget));

    if (!strIMEName.empty())
    {
        configParser_SetConfigString(kszContactDefaultIME, strIMEName.c_str(), CONFIG_LEVEL_PHONE);
    }
}

// 点击删除
void CDlgDirSetting::OnBtnDeleteClick()
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL == pFocusWidget)
    {
        return;
    }

    if (pFocusWidget->inherits("QLineEdit") || pFocusWidget->inherits("QTextEdit"))
    {
        //点击删除时，调用此函数，对当前焦点的文本输入框的内容进行退格删除
        qOnBackspaceClicked(this);
    }
}

void CDlgDirSetting::OnKeyboardShow()
{
    AdjustFrameList(false);
}

void CDlgDirSetting::OnKeyboardHide()
{
    CancelInputGroupName();

    AdjustFrameList(true);
}

void CDlgDirSetting::AdjustFrameList(bool bReset)
{
    if (NULL == m_pTreeList || NULL == m_pFrameList)
    {
        return;
    }
    if (bReset)
    {
        m_pTreeList->SetRect(RECT_DIR_GROUP);
        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
        CSettingDelegate* pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

        if (NULL != pSettingDelegate && pSettingDelegate == m_pFrameList->GetFrameDelegate())
        {
            pSettingDelegate->RefreshFrameList();
        }
        return;
    }
    DirUI_AdjustFrameList(m_pTreeList
                          , bReset
                          , RECT_DIR_GROUP
                          , DLG_DIR_SETTING_ITEM_OFFSET
                          , RECT_DIRECTORY.y());
}

int CDlgDirSetting::GetFocusItemIndex()
{
    if (NULL == m_pTreeList)
    {
        return INVALID_ITEM_INDEX;
    }

    CGroupDelegate* pGroupDelegate = (CGroupDelegate*)m_pTreeList->GetFrameDelegate();

    if (NULL == pGroupDelegate)
    {
        return INVALID_ITEM_INDEX;
    }

    int nSelectId = pGroupDelegate->GetSelectId();
    CTreeItemPtr pItem = m_pTreeList->GetItemById(nSelectId);

    if (NULL == pItem)
    {
        return INVALID_ITEM_INDEX;
    }

    return pItem->GetDataIndex();
}

bool CDlgDirSetting::CancelInputGroupName()
{
    QWidget* pFocusWidget = focusWidget();

    if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
    {
        pFocusWidget->clearFocus();
        return true;
    }

    return false;
}
