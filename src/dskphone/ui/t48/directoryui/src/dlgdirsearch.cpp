#include "baseui/framelist.h"

#include "dlgdirsearch.h"

#include "contactsearchitem.h"
#include "delegatefactory.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "precisesearchdelegate.h"
#include "searchdelegate.h"
#include "bsftcontactcontroler.h"
#include "ldapcontactcontroler.h"
#include "remoteserversearchcontroler.h"
#include "diruihelperfactory.h"
#include "bsftcontactuihelper.h"
#include "broadsoftsearchlayout.h"
#include "configiddefine.h"
#include "baseui/comboboxlistitem.h"
#include "baseui/editlistitem.h"
#include "baseui/t4xpicpath.h"
#include "configparser/modconfigparser.h"
#include "uicommon.h"
#include "keymap.h"
#include "inputmethod/inputmethod.h"
#include "mainwnd/mainwnd.h"
#include "uimanager/rectdefine.h"
#include "qtcommon/qmisc.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qwidgetutility.h"
#include "uimanager/moduimanager.h"
#include "remotecontactcontroler.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "commonunits/commonconfig.h"
//图片
#include "imagemanager/modimagemanager.h"
#include "baseui/framelistaction.h"


#if IF_FEATURE_GBBOOK
#include "genbandcontactcontroler.h"
#include "genbandcontactuihelper.h"
#include "genbandsearchlayout.h"
#endif

namespace
{
#define SEARCH_DELAY_TIME 300
#define DLG_DIR_FRAME_LIST_FONT_SIZE_NOTE 16
#define DLG_DIR_SEARCH_ITEM_OFFSET 4
}

CDlgDirSearch::CDlgDirSearch(QWidget * parent/* = NULL*/)
    : CBaseDialog(NULL)
    , m_pSearchList(NULL)
    , m_pContactList(NULL)
    , m_pBsftSearchLayout(NULL)
    , m_nContactType(DIRECTORY_TYPE_LOCAL)
    , m_nSearchType(DIRECTORY_SEARCH_TYPE_NONE)
    , m_nSearchCount(0)
    , m_bReturn(false)
#if IF_FEATURE_GBBOOK
    , m_pSearchComboBox(NULL)
    , m_pGBSearchLayout(NULL)
#endif
    , m_bShowKeyBoard(false)
{
    SetWindowType(eWT_Search_Directory);
    SetTopWnd();
    // 设置窗体位置
    SetDlgGeometry(RECT_DIRECTORY);

    SetSoftkeyType(STYLE_TYPE_DIVIDE);
    SetWindowTitle(LANG_TRANSLATE(TRID_SEARCH_FOR_CONTACTS));

    m_pSearchList = new CFrameList(this);

    if (NULL != m_pSearchList)
    {
        m_pSearchList->SetDelegate(this);
        m_pSearchList->SetRect(RECT_DIR_SEARCH_TITLE);
        m_pSearchList->SetGridLine(true);
        m_pSearchList->SetLinePosY(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pSearchList->SetLineHeight(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pSearchList->SetLineImage(PIC_GENERAL_LIST_ITEM_LEFT_LINE);
        m_pSearchList->SetItemClickBG(PIC_DIRECTORY_SEARCH_ITEM_SEL_TOP, PIC_DIRECTORY_SEARCH_ITEM_SEL_MID,
                                      NULL);
        m_pSearchList->SetItemSelectBG(PIC_DIRECTORY_SEARCH_ITEM_SEL_TOP, PIC_DIRECTORY_SEARCH_ITEM_SEL_MID,
                                       NULL);
        m_pSearchList->show();
        m_pSearchList->raise();
    }

    m_pContactList = new CFrameList(this);

    if (NULL != m_pContactList)
    {
        m_pContactList->SetRect(RECT_DIR_CONTACT);
        m_pContactList->SetContentPos(CONTACT_LIST_ITEM_CONTENT_LEFT_MARGIN,
                                      CONTACT_LIST_ITEM_CONTENT_RIGHT_MARGIN);
        m_pContactList->SetGridLine(true);
        m_pContactList->SetLinePosY(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pContactList->SetLineHeight(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pContactList->SetLineImage(PIC_GENERAL_LIST_ITEM_RIGHT_LINE);
        m_pContactList->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP, PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                       NULL);
        m_pContactList->SetNote(LANG_TRANSLATE(TRID_NONE));
        m_pContactList->SetNoteFontSize(DLG_DIR_FRAME_LIST_FONT_SIZE_NOTE);
        m_pContactList->show();
        m_pContactList->raise();
    }

    for (int i = 0; i < DIRECTORY_SEARCH_TYPE_MAX; ++i)
    {
        m_arrSearchType[i] = DIRECTORY_SEARCH_TYPE_NONE;
    }

    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimerOut()));
}

CDlgDirSearch::~CDlgDirSearch(void)
{
    StopTimer();
    ReleaseLayout();
}

void CDlgDirSearch::ReleaseLayout()
{
    if (NULL != m_pContactList)
    {
        m_pContactList->ClearContent();
    }

    ResetUIHelperLayout();

    if (NULL != m_pBsftSearchLayout)
    {
        m_pBsftSearchLayout->ReleaseAllItem();

        delete m_pBsftSearchLayout;
        m_pBsftSearchLayout = NULL;
    }

#if IF_FEATURE_GBBOOK
    if (NULL != m_pGBSearchLayout)
    {
        m_pGBSearchLayout->ReleaseAllItem();

        delete m_pGBSearchLayout;
        m_pGBSearchLayout = NULL;
    }
#endif
}

void CDlgDirSearch::ResetUIHelperLayout()
{
    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(DIRECTORY_TYPE_BROADSOFT);

    if (pUIHelper.IsEmpty())
    {
        return;
    }
    // 高级搜索换组Bug http://bugfree.yealink.com/Bug.php?BugID=136650

    CBSFTContactUIHelper * pBSFTUIHelper = CBSFTContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pBSFTUIHelper)
    {
        if (pBSFTUIHelper->GetSearchLayout() != NULL)
        {
            disconnect(pBSFTUIHelper->GetSearchLayout()->GetGroupItem()->GetComboBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxSelect(int)));
        }
        pBSFTUIHelper->SetSearchLayout(NULL);
    }
}

void CDlgDirSearch::InitBsftSearchLayout()
{
    if (NULL == m_pBsftSearchLayout)
    {
        m_pBsftSearchLayout = new CBroadSoftSearchLayout(this);

        if (NULL == m_pBsftSearchLayout)
        {
            return;
        }

        m_pBsftSearchLayout->Init();
    }
    else
    {
        m_pBsftSearchLayout->ClearEdit();
    }

    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(DIRECTORY_TYPE_BROADSOFT);
    CBSFTContactUIHelper * pBSFTUIHelper = CBSFTContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pBSFTUIHelper)
    {
        pBSFTUIHelper->SetSearchLayout(m_pBsftSearchLayout);
    }
    // 高级搜索换组Bug http://bugfree.yealink.com/Bug.php?BugID=136650

    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0 && NULL != pBSFTUIHelper && pBSFTUIHelper->GetSearchLayout() != NULL)
    {
        connect(pBSFTUIHelper->GetSearchLayout()->GetGroupItem()->GetComboBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxSelect(int)));
    }
}

#if IF_FEATURE_GBBOOK
void CDlgDirSearch::InitGBSearchLayout()
{
    if (NULL == m_pGBSearchLayout)
    {
        m_pGBSearchLayout = new CGenbandSearchLayout(this);

        if (NULL == m_pGBSearchLayout)
        {
            return;
        }

        m_pGBSearchLayout->Init();
    }
    else
    {
        m_pGBSearchLayout->ClearEdit();
    }

    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(DIRECTORY_TYPE_GENBAND);
    CGenbandContactUIHelper * pGBUIHelper = CGenbandContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pGBUIHelper)
    {
        pGBUIHelper->SetSearchLayout(m_pGBSearchLayout);
    }

#if IF_FEATURE_GBBOOK
    CComboBoxListItemPtr pSearchTypeItem = m_pGBSearchLayout->GetSearchTypeItem();

    if (NULL != pSearchTypeItem)
    {
        m_pSearchComboBox = pSearchTypeItem->GetComboBox();
        if (NULL != m_pSearchComboBox)
        {
            connect(m_pSearchComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSearchTypeChanged(int)));
        }
    }

#endif
    // 高级搜索换组Bug http://bugfree.yealink.com/Bug.php?BugID=136650
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(DIRECTORY_TYPE_BROADSOFT);
        CBSFTContactUIHelper * pBSFTUIHelper = CBSFTContactUIHelper::GetUIHelper(pUIHelper);

        if (NULL != pBSFTUIHelper)
        {
            if (pBSFTUIHelper->GetSearchLayout() != NULL)
            {
                connect(pBSFTUIHelper->GetSearchLayout()->GetGroupItem()->GetComboBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxSelect(int)));
            }
        }
    }

}
#endif

void CDlgDirSearch::Init()
{
    m_bReturn = false;
    m_bShowKeyBoard = true;

    InitSearchType();

    if (NULL != m_pContactList)
    {
        m_pContactList->SetOnlyTitleLine(false);
        m_pContactList->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP, PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                       NULL);
    }
    //if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    //{
    //    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    //    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
    //    if (NULL != pBSFTContact)
    //    {
    //        BsftDir_ClearContactInfo(pBSFTContact->GetGroupId());
    //        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    //        pDelegate->RefreshFrameList();
    //    }
    //}
}

void CDlgDirSearch::InitSearchType()
{
    int i = 0;

    m_arrSearchType[i] = DIRECTORY_SEARCH_TYPE_NORMAL;
    ++i;

    if (LDAP_IsEnable())
    {
        m_arrSearchType[i] = DIRECTORY_SEARCH_TYPE_LDAP;
        ++i;
    }

    if (CCommonDirCtrl::IsDirEnable(DIRECTORY_TYPE_BROADSOFT)
            && CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_BROADSOFT)
            && XSI_SUCCESS == CBaseHelper::GetXSIStatus())
    {
        m_arrSearchType[i] = DIRECTORY_SEARCH_TYPE_BROADSOFT;
        ++i;
    }

#if IF_FEATURE_GBBOOK
    if (configParser_GetConfigInt(kszGbGabBookEnable) == 1
            && CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_GENBAND, kszGabBookGroupId))
    {
        m_arrSearchType[i] = DIRECTORY_SEARCH_TYPE_GENBAND;
        ++i;
    }
#endif

    m_nSearchCount = i;

    for (; i < DIRECTORY_SEARCH_TYPE_MAX; ++i)
    {
        m_arrSearchType[i] = DIRECTORY_SEARCH_TYPE_NONE;
    }
}

void CDlgDirSearch::Uninit()
{
    StopTimer();

    if (NULL != m_pSearchList)
    {
        int nItemCount = m_pSearchList->GetTotalItemCount();

        for (int i = 0; i < nItemCount; ++i)
        {
            QLineEdit * pEdit = NULL;
            CContactSearchItemPtr pSearchItem = (CContactSearchItemPtr)m_pSearchList->GetItemByIndex(i);

            if (NULL != pSearchItem)
            {
                pEdit = pSearchItem->GetEdit();
            }

            if (NULL != pEdit)
            {
                pEdit->removeEventFilter(this);

                if (qGetIsShowWnd())
                {
                    qSetIsShowWnd(pEdit, false);
                }
            }
        }

        m_pSearchList->ClearContent();
        m_pSearchList->SetDelegate(NULL);
    }

    if (NULL != m_pContactList)
    {
        m_pContactList->ClearContent();
        m_pContactList->SetFrameListTitle(NULL);
        m_pContactList->SetDelegate(NULL);
    }

    for (int i = 0; i < DIRECTORY_SEARCH_TYPE_MAX; ++i)
    {
        m_arrSearchType[i] = DIRECTORY_SEARCH_TYPE_NONE;
    }

    m_bShowKeyBoard = false;

    m_bReturn = false;

    ReleaseLayout();
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        BsftDir_ClearContactInfo(kszServerSearchGroupId);
    }
}

bool CDlgDirSearch::CheckSearchType()
{
    for (int i = 0; i < m_nSearchCount && i < DIRECTORY_SEARCH_TYPE_MAX; ++i)
    {
        if (m_nSearchType == m_arrSearchType[i])
        {
            return true;
        }
    }

    return false;
}

void CDlgDirSearch::UpdateWnd()
{
    if (NULL == m_pSearchList || NULL == m_pContactList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    int nPreciseSearchState = CBSFTContactControler::PRECISE_SEARCH_STATE_NONE;
    int nPageDataIndex = 0;

    if (m_bReturn)
    {
        InitSearchType();

        if (!CheckSearchType())
        {
            UIManager_PutPrivateDlg(this);
        }

        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        int nDirType = m_nContactType;
        int nUIState = DIR_UI_STATE_SEARCH;
        int nGroupId = knInvalidRId;

        if (DIRECTORY_SEARCH_TYPE_LDAP == m_nSearchType)
        {
            nDirType = DIRECTORY_TYPE_LDAP;
        }
        else if (DIRECTORY_SEARCH_TYPE_BROADSOFT == m_nSearchType)
        {
            nDirType = DIRECTORY_TYPE_BROADSOFT;
            nUIState = DIR_UI_STATE_PRECISE_SEARCH;
        }
#if IF_FEATURE_GBBOOK
        else if (DIRECTORY_SEARCH_TYPE_GENBAND == m_nSearchType)
        {
            nDirType = DIRECTORY_TYPE_GENBAND;
            nUIState = DIR_UI_STATE_PRECISE_SEARCH;
        }
#endif

        if (!pControler.IsEmpty())
        {
            nGroupId = pControler->GetGroupId();
        }

        CDirectoryListDelegatePtr pReturnDelegate = g_DirDelegateFactory.FindContactDelegate(nUIState);

        if (!pReturnDelegate.IsEmpty())
        {
            nPageDataIndex = pReturnDelegate->GetPageDataIndex();

            if (DIR_UI_STATE_PRECISE_SEARCH == nUIState)
            {
                CDirectoryControlerPtr pReturnControler = g_DirCtrlFactory.FindControler(DIRECTORY_TYPE_BROADSOFT);
                CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pReturnControler);

                if (NULL != pBSFTContact)
                {
                    nPreciseSearchState = pBSFTContact->GetPreciseSearchState();
                }
            }
        }

        g_DirectoryMgr.SetCurDirType(nDirType);
        g_DirectoryMgr.CheckContactDelegate(nUIState, nGroupId, false);

        pDelegate = g_DirectoryMgr.GetActiveDelegate();
    }

    if (pDelegate.IsEmpty())
    {
        return;
    }

    if (m_bReturn && DIR_UI_STATE_PRECISE_SEARCH != nPreciseSearchState)
    {
        CDirectoryControlerPtr pControler = pDelegate->GetControler();
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

        if (NULL != pBSFTContact)
        {
            pBSFTContact->AdjustPreciseState(nPreciseSearchState);
        }
    }

    if (NULL == m_pContactList->GetFrameDelegate())
    {
        m_pContactList->SetDelegate(pDelegate.Get());
    }

    if (NULL == m_pSearchList->GetFrameDelegate())
    {
        m_pSearchList->SetDelegate(this);
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();

    if (!pControler.IsEmpty())
    {
        int nModType = g_DirectoryMgr.GetModType(pControler->GetType());
        bool bReadOnly = !CDirectoryControler::IsDirCanEidt(nModType);

        pControler->SetReadOnly(bReadOnly);
    }

    m_pSearchList->RefreshData(0);

    m_pContactList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
    pDelegate->SetPageDataIndex(nPageDataIndex);

    if (DIRECTORY_SEARCH_TYPE_NORMAL == m_nSearchType || DIRECTORY_SEARCH_TYPE_LDAP == m_nSearchType)
    {
        CSearchDelegate * pSearchDelegate = CSearchDelegate::GetDirDelegate(pDelegate);
        QLineEdit * pEdit = NULL;
        QWidget * pFocusWidget = focusWidget();

        if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
        {
            pEdit = (QLineEdit *)pFocusWidget;
        }

        QString strSearchKey = toQString(pSearchDelegate->GetSearchKey());
        QString strEditText = pEdit->text();

        if (strSearchKey != strEditText)
        {
            pEdit->setText(strSearchKey);
        }
        else if (DIRECTORY_SEARCH_TYPE_LDAP == m_nSearchType && strSearchKey.isEmpty() && !m_bReturn)
        {
            // 判断是 LDAP 搜索界面，才进行一次搜索
            SearchContact(strSearchKey);
            return;
        }
    }
    pDelegate->RefreshFrameList();
    m_bReturn = false;
}

void CDlgDirSearch::OnFocusChanged(bool bFocused)
{
    if (!bFocused)
    {
        m_bReturn = true;

        m_bShowKeyBoard = qGetIsShowWnd();

        QWidget* pFocusWidget = focusWidget();

        if (m_bShowKeyBoard && NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
        {
            qSetIsShowWnd(pFocusWidget, false);
        }

        if (NULL != m_pContactList)
        {
            m_pContactList->SetDelegate(NULL);
        }
    }
}

void CDlgDirSearch::InitPreciseSearch(int nDirType)
{
    //初始化Softkey
    InitSoftKeyData();
    if (nDirType == DIRECTORY_TYPE_GENBAND)
    {
        InitGBSearchLayout();
    }
    else if (nDirType == DIRECTORY_TYPE_BROADSOFT)
    {
        InitBsftSearchLayout();
    }
}

bool CDlgDirSearch::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    //判断事件类型
    int nType = pEvent->type();

    switch (nType)
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
    case QEvent::FocusIn://焦点集中事件
        {
            OnCtrlFocusChanged((QFocusEvent *)pEvent);
        }
        break;
    case QEvent::FocusOut:
        {
            OnCtrlFocusChanged((QFocusEvent *)pEvent);
        }
        break;
    case QEvent::KeyPress://键盘输入事件
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            int nKey = pKeyEvent->key();

            switch (nKey)
            {
            case PHONE_KEY_CANCEL:
                {
                    if (DIRECTORY_SEARCH_TYPE_NORMAL == m_nSearchType)
                    {
                        CDirectoryControlerPtr pActiveControler = g_DirectoryMgr.GetActiveControler();
                        CRemoteServerSearchControler * pRemoteSearchControler =
                            CRemoteServerSearchControler::GetServerSearchControler(pActiveControler);

                        if (NULL != pRemoteSearchControler)
                        {
                            if (pRemoteSearchControler->OnBack())
                            {
                                return true;
                            }
                        }
                    }

                    UIManager_PutPrivateDlg(this);
                    return true;
                }
                break;
            case PHONE_KEY_OK:
                {
                    if (OnEnterPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_LEFT:
                {
                    if (OnLeftPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_RIGHT:
                {
                    if (OnRightPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_UP:
                {
                    if (OnUpPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_DOWN:
                {
                    if (OnDownPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            default:
                break;
            }

            if (NULL != m_pContactList && m_pContactList->eventFilter(pObj, pEvent))
            {
                return true;
            }
        }
        break;
    case QEvent::MouseButtonPress:
        {
            if (OnMousePressEventFilter(pObj, pEvent))
            {
                return true;
            }
        }
        break;

    default:
        break;
    }

    return CBaseDialog::eventFilter(pObj, pEvent);
}

bool CDlgDirSearch::OnEnterPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    if (!((DIRECTORY_SEARCH_TYPE_BROADSOFT == m_nSearchType)
#if IF_FEATURE_GBBOOK
            || (DIRECTORY_SEARCH_TYPE_GENBAND == m_nSearchType)
#endif
            || (DIRECTORY_SEARCH_TYPE_NORMAL == m_nSearchType
                && DIRECTORY_TYPE_REMOTE_SEARCH == m_nContactType)
            || (DIRECTORY_TYPE_BROADSOFT == pControler->GetType() && configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)))
    {
        return false;
    }

    bool bSearch = false;
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget)
    {
        if (pFocusWidget->inherits("QComboBox"))
        {
            if (pFocusWidget == pObj)
            {
                bSearch = true;
            }
        }
        else if (pFocusWidget->inherits("QLineEdit"))
        {
            bSearch = true;
        }
    }

    if (bSearch)
    {
        OnBtnSearch();
    }

    return true;
}

bool CDlgDirSearch::OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    if (DIRECTORY_SEARCH_TYPE_BROADSOFT != m_nSearchType)
    {
        return false;
    }

    QWidget * pFocusWidget = focusWidget();

    return qComboBoxUtility::OnLeftPressEventFilter(pObj, pEvent, pFocusWidget);
}

bool CDlgDirSearch::OnRightPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    if (DIRECTORY_SEARCH_TYPE_BROADSOFT != m_nSearchType)
    {
        return false;
    }

    QWidget * pFocusWidget = focusWidget();

    return qComboBoxUtility::OnRightPressEventFilter(pObj, pEvent, pFocusWidget);
}

bool CDlgDirSearch::OnUpPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    if (DIRECTORY_SEARCH_TYPE_BROADSOFT != m_nSearchType)
    {
        return false;
    }

    bool bComboBoxFilter = false;
    CListItemPtr pListItem = GetUpDownFilterFocusItem(pObj, bComboBoxFilter);

    if (bComboBoxFilter)
    {
        return false;
    }

    if (NULL != pListItem)
    {
        return SwitchItemFocus(pListItem, true);
    }

    return false;
}

bool CDlgDirSearch::OnDownPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    if (DIRECTORY_SEARCH_TYPE_BROADSOFT != m_nSearchType)
    {
        return false;
    }

    bool bComboBoxFilter = false;
    CListItemPtr pListItem = GetUpDownFilterFocusItem(pObj, bComboBoxFilter);

    if (bComboBoxFilter)
    {
        return false;
    }

    if (NULL != pListItem)
    {
        return SwitchItemFocus(pListItem, false);
    }

    return false;
}

CListItem * CDlgDirSearch::GetUpDownFilterFocusItem(QObject * pEventObj, bool & bComboBoxFilter)
{
    if (NULL == m_pContactList)
    {
        return NULL;
    }

    CListItemPtr pListItem = NULL;
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget)
    {
        if (pFocusWidget->inherits("QLineEdit"))
        {
            pListItem = (CListItemPtr)pFocusWidget->parent();
        }
        else if (pFocusWidget->inherits("QComboBox"))
        {
            if (pFocusWidget != pEventObj)
            {
                bComboBoxFilter = true;
                return NULL;
            }

            pListItem = (CListItemPtr)pFocusWidget->parent();
        }
    }

    return pListItem;
}

bool CDlgDirSearch::OnMousePressEventFilter(QObject * pObj, QEvent * pEvent)
{
    if (DIRECTORY_SEARCH_TYPE_BROADSOFT != m_nSearchType)
    {
        return false;
    }

    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget && NULL != pObj && pObj != pFocusWidget
            && (!pObj->inherits("qButtonItem")))
    {
        bool bComboBoxFocus = pFocusWidget->inherits("QComboBox");
        bool bEditFocus = pFocusWidget->inherits("QLineEdit");

        if (bComboBoxFocus)
        {
            QComboBox * pComboBox = (QComboBox *)pFocusWidget;
            QAbstractItemView * pView = pComboBox->view();

            if (NULL != pView)
            {
                QScrollBar * pScrollBar = pView->verticalScrollBar();

                if (pView == pObj || pScrollBar == pObj || pObj->parent() == pView)
                {
                    // 下拉框弹出时点中下拉框内的范围
                    bComboBoxFocus = false;
                }
                else if (pView->isVisible())
                {
                    // 点击其他地方
                    return true;
                }
            }
        }

        if ((bComboBoxFocus || bEditFocus)
                && (!pObj->inherits("QComboBox") && !pObj->inherits("QLineEdit"))
                && (!FullKeyboardEnabled()
                    || !(pObj->inherits("CBaseInputDlg")
                         || pObj->inherits("CInputBoxButton")
                        )
                   )
           )
        {
            pFocusWidget->clearFocus();

            if (bComboBoxFocus)
            {
                OnCtrlFocusChanged(NULL);
            }
        }
        if (FullKeyboardEnabled())
        {
            CListItemPtr pListItem = NULL;
            pFocusWidget = focusWidget();
            if (pFocusWidget != NULL)
            {
                if (pFocusWidget->inherits("QLineEdit"))
                {
                    pListItem = (CListItemPtr)pFocusWidget->parent();
                }
                if (IsLastEditItem(pListItem))
                {
                    qSetEnterType(ENTER_TYPE_DONE);
                }
                else
                {
                    qSetEnterType(ENTER_TYPE_NEXT);
                }
            }

            qRaiseFullKeyboard();
        }
    }

    return false;
}

bool CDlgDirSearch::SwitchItemFocus(CListItem * pListItem, bool bPreItem)
{
    if (NULL == pListItem || NULL == m_pContactList)
    {
        return false;
    }

    int nIndex = m_pContactList->GetIndexByItem(pListItem);

    if (INVALID_ITEM_INDEX == nIndex)
    {
        return false;
    }

    if (bPreItem)
    {
        --nIndex;

        if (nIndex < 0)
        {
            m_pContactList->OnNotifyScrollPrevious();

            int nItemCount = m_pContactList->GetTotalItemCount();

            if (nItemCount <= 0)
            {
                return false;
            }

            nIndex = nItemCount - 1;
        }
    }
    else
    {
        ++nIndex;

        int nItemCount = m_pContactList->GetTotalItemCount();

        if (nIndex >= nItemCount)
        {
            m_pContactList->OnNotifyScrollNext();

            nIndex = 0;
        }
    }

    CListItemPtr pFocusListItem = m_pContactList->GetItemByIndex(nIndex);



    if (NULL == pFocusListItem)
    {
        return false;
    }
    if (IsLastEditItem(pFocusListItem))
    {
        qSetEnterType(ENTER_TYPE_DONE);
    }
    else
    {
        qSetEnterType(ENTER_TYPE_NEXT);
    }

    pFocusListItem->FocusItem(true);
    AdjustFrameList(false);
    return true;
}

void CDlgDirSearch::OnEditKeyOkClick()
{
    // 搜索
    StopTimer();

    if (DIRECTORY_SEARCH_TYPE_BROADSOFT == m_nSearchType)
    {
        return;
    }

    BeginSearch();
}

void CDlgDirSearch::OnKeyWordChanged(const QString & strText)
{
    StopTimer();

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CRemoteServerSearchControler * pSearchControler =
        CRemoteServerSearchControler::GetServerSearchControler(pControler);
    // 远程联系人远程搜索只能用搜索按钮进行搜索
    if (NULL != pSearchControler)
    {
        return;
    }

    if (DIRECTORY_TYPE_BROADSOFT == pControler->GetType() && configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        return;
    }

    if (strText.isEmpty())
    {
        SearchContact(strText);
    }
    else
    {
        m_timer.start(SEARCH_DELAY_TIME);
    }
}

void CDlgDirSearch::OnSearchEditFocus(bool bFocus)
{
    OnCtrlFocusChanged(NULL);
}


void CDlgDirSearch::OnComboBoxSelect(int iIndex)
{
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
        if (pDelegate.IsEmpty())
        {
            return;
        }
        CDirectoryControlerPtr pControler = pDelegate->GetControler();
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
        if (NULL != pBSFTContact)
        {
            pDelegate->ResetIndex();
        }
    }
}

void CDlgDirSearch::OnTimerOut()
{
    if (DIRECTORY_SEARCH_TYPE_LDAP != m_nSearchType && DIRECTORY_SEARCH_TYPE_NORMAL != m_nSearchType)
    {
        return;
    }

    BeginSearch();
}

void CDlgDirSearch::BeginSearch()
{
    QWidget * pWidget = focusWidget();

    if (NULL == pWidget || !pWidget->inherits("QLineEdit"))
    {
        return;
    }

    QLineEdit * pEdit = (QLineEdit *)pWidget;
    SearchContact(pEdit->text());
}

void CDlgDirSearch::SearchContact(const QString & strKey)
{
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
        if (NULL != pBSFTContact)
        {
            pBSFTContact->SetNeedReLoad(true);
        }

        BsftDir_ClearContactInfo(kszServerSearchGroupId);
    }
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSearchDelegate * pSearchDelegate = CSearchDelegate::GetDirDelegate(pDelegate);

    if (NULL != pSearchDelegate)
    {
        yl::string strSearchKey = strKey.trimmed().toUtf8().data();

        if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0 && strSearchKey.empty())
        {
            return;
        }
        pSearchDelegate->OnSearchChange(strSearchKey);
    }
}

void CDlgDirSearch::StopTimer()
{
    if (m_timer.isActive())
    {
        m_timer.stop();
    }
}

bool CDlgDirSearch::OnAction(const QString & strAction)
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
    else if (strAction == TRID_SEARCH)
    {
        OnBtnSearch();
    }
    else if (strAction == ACTION_BACK && DIRECTORY_SEARCH_TYPE_BROADSOFT == m_nSearchType)
    {
        CDirectoryControlerPtr pActiveControler = g_DirectoryMgr.GetActiveControler();
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pActiveControler);

        if (NULL != pBSFTContact)
        {
            if (pBSFTContact->OnBack())
            {
                if (NULL != m_pBsftSearchLayout)
                {
                    m_pBsftSearchLayout->ClearEdit();
                }

                CBSFTContactUIHelper * pBSFTUIHelper = pBSFTContact->GetBSFTUIHelper();

                if (NULL != pBSFTUIHelper)
                {
                    pBSFTUIHelper->OnAttachFrameList();
                }

                if (NULL != m_pContactList)
                {
                    m_pContactList->RefreshData(0);
                }

                return true;
            }
        }

        return false;
    }
    else if ((strAction == ACTION_BACK)
             && DIRECTORY_SEARCH_TYPE_NORMAL == m_nSearchType)
    {
        CDirectoryControlerPtr pActiveControler = g_DirectoryMgr.GetActiveControler();
        CRemoteServerSearchControler * pRemoteSearchControler =
            CRemoteServerSearchControler::GetServerSearchControler(pActiveControler);

        if (NULL != pRemoteSearchControler)
        {
            if (!pRemoteSearchControler->OnBack())
            {
                UIManager_PutPrivateDlg(this);
            }

            return true;
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
void CDlgDirSearch::OnCtrlFocusChanged(QFocusEvent * pEvent)
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit")
            && NULL != pEvent && Qt::MouseFocusReason != pEvent->reason())
    {
        pFocusWidget->clearFocus();
        if (FullKeyboardEnabled())
        {
            if (pFocusWidget->parent() != NULL && pFocusWidget->parent()->inherits("CContactSearchItem"))
            {
                InitSoftKeyData();
                _MainWND.DrawSoftkey(this);
            }
        }
        return;
    }
    if (FullKeyboardEnabled())
    {
        CListItemPtr pListItem = NULL;
        pFocusWidget = focusWidget();
        if (pFocusWidget != NULL)
        {
            if (pFocusWidget->inherits("QLineEdit"))
            {
                pListItem = (CListItemPtr)pFocusWidget->parent();
            }
            if (IsLastEditItem(pListItem))
            {
                qSetEnterType(ENTER_TYPE_DONE);
            }
            else
            {
                qSetEnterType(ENTER_TYPE_NEXT);
            }
        }

        qRaiseFullKeyboard();
    }
    pFocusWidget = focusWidget();

    int nSearchType = m_nSearchType;

    if (NULL != m_pSearchList)
    {
        CContactSearchItemPtr pSearchItem = (CContactSearchItemPtr)m_pSearchList->GetFocusItem();

        if (NULL != pSearchItem)
        {
            nSearchType = pSearchItem->GetSearchType();
        }
    }

    InitSoftKeyData();

    if (NULL == pFocusWidget)
    {
        _MainWND.DrawSoftkey(this);
    }
    else if (pFocusWidget->inherits("QLineEdit"))
    {
        // 切换搜索框时，要重新获取nSearchType
        CheckSearchType(pFocusWidget, nSearchType);

        m_SoftkeyData[0].m_strSoftkeyAction = IME_ACTION;

        m_SoftkeyData[1].m_strSoftkeyAction = IME_DEL_ACTION;
        m_SoftkeyData[1].m_strIconPath = PIC_SOFTKEY_DELETE;

        if (DIRECTORY_SEARCH_TYPE_BROADSOFT == nSearchType
#if IF_FEATURE_GBBOOK
                || DIRECTORY_SEARCH_TYPE_GENBAND == nSearchType
#endif
           )
        {
            m_SoftkeyData[2].m_strSoftkeyAction = TRID_SEARCH;
            m_SoftkeyData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SEARCH);
            m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_CONTACT_SEARCH;

#if IF_FEATURE_GBBOOK
            if (NULL != m_pGBSearchLayout && DIRECTORY_SEARCH_TYPE_GENBAND == nSearchType)
            {
                CComboBoxListItemPtr pSearchTypeItem = m_pGBSearchLayout->GetSearchTypeItem();
                if (pSearchTypeItem != NULL)
                {
                    GBBOOK_OPERATION_TYPE eCurType = (GBBOOK_OPERATION_TYPE)pSearchTypeItem->GetItemData();
                    if (eCurType == GAB_SEARCH_NUMBER)
                    {
                        qSetCurrentIME(pFocusWidget, IME_123);
                    }
                    else
                    {
                        qSetCurrentIME(pFocusWidget, IME_Abc);
                    }

                    qUseKeyBoardFromIME(pFocusWidget, INPUT_DIALOG_SEARCH);
                    if (qGetIsShowWnd())
                    {
                        qSetIsShowWnd(pFocusWidget, true);
                    }
                }
            }
#endif
        }
        else
        {
            if (DIRECTORY_TYPE_REMOTE_SEARCH == m_nContactType && DIRECTORY_SEARCH_TYPE_NORMAL == nSearchType)
            {
                m_SoftkeyData[2].m_strSoftkeyAction = TRID_SEARCH;
                m_SoftkeyData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SEARCH);
                m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_CONTACT_SEARCH;
            }

            yl::string strDefaultIME(configParser_GetConfigString(kszContactSearchDefaultIME));

            if (!strDefaultIME.empty())
            {
                qSetCurrentIME(pFocusWidget, strDefaultIME.c_str());

                qUseKeyBoardFromIME(pFocusWidget, INPUT_DIALOG_SEARCH);
                if (qGetIsShowWnd())
                {
                    qSetIsShowWnd(pFocusWidget, true);
                }
            }
        }

        _MainWND.DrawSoftkey(this);
        _MainWND.DisplayIMESoftkey(pFocusWidget);
    }
    else if (pFocusWidget->inherits("QComboBox"))
    {
        if (DIRECTORY_SEARCH_TYPE_BROADSOFT == nSearchType
#if IF_FEATURE_GBBOOK
                || DIRECTORY_SEARCH_TYPE_GENBAND == nSearchType
#endif
           )
        {
            m_SoftkeyData[0].m_strSoftkeyAction = TRID_SEARCH;
            m_SoftkeyData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SEARCH);
            m_SoftkeyData[0].m_strIconPath = PIC_SOFTKEY_CONTACT_SEARCH;
        }

        _MainWND.DrawSoftkey(this);
    }
    else
    {
        _MainWND.DrawSoftkey(this);
    }
}

void CDlgDirSearch::CheckSearchType(QWidget * pLineEdit, int & nSearchType)
{
    if (NULL == pLineEdit)
    {
        return;
    }

    yl::string strParClass;
    QObject * pParent = pLineEdit->parent();

    if (NULL != pParent)
    {
        const QMetaObject * pParMetaObj = pParent->metaObject();

        if (NULL != pParMetaObj)
        {
            strParClass = pParMetaObj->className();
        }
    }

    if ("CContactSearchItem" == strParClass)
    {
        CContactSearchItemPtr pSearchItem = (CContactSearchItemPtr)pParent;

        if (NULL != pSearchItem)
        {
            nSearchType = pSearchItem->GetSearchType();
        }
    }
}

// 点击IME
void CDlgDirSearch::OnBtnIMEClick()
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL == pFocusWidget)
    {
        return;
    }

    _MainWND.SwitchNextIMEName(pFocusWidget);

    if (DIRECTORY_SEARCH_TYPE_BROADSOFT != m_nSearchType)
    {
        yl::string strIMEName = fromQString(qGetCurrentIMEName(pFocusWidget));

        if (!strIMEName.empty())
        {
            configParser_SetConfigString(kszContactSearchDefaultIME, strIMEName.c_str(), CONFIG_LEVEL_PHONE);
        }
    }
}

// 点击删除
void CDlgDirSearch::OnBtnDeleteClick()
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

void CDlgDirSearch::OnBtnSearch()
{
    DIRUI_INFO(" ====== CDlgDirSearch::OnBtnSearch %d ======", m_nSearchType);
    if (DIRECTORY_SEARCH_TYPE_NORMAL == m_nSearchType && (DIRECTORY_TYPE_REMOTE_SEARCH == m_nContactType || DIRECTORY_TYPE_BROADSOFT == m_nContactType))
    {
        qSetHideKeyboard();
        BeginSearch();
        return;
    }

    CDirectoryListDelegatePtr pActiveDelegate = g_DirectoryMgr.GetActiveDelegate();
    CPreciseSearchDelegate * pSearchDelegate = CPreciseSearchDelegate::GetDirDelegate(pActiveDelegate);
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
        if (NULL != pBSFTContact)
        {
            pBSFTContact->SetNeedReLoad(true);
        }
        BsftDir_ClearContactInfo(kszServerSearchGroupId);
    }

#if IF_FEATURE_GBBOOK
    if (NULL == pSearchDelegate)
    {
        return;
    }

    if (DIRECTORY_SEARCH_TYPE_BROADSOFT == m_nSearchType)
    {
        CDirectoryControlerPtr pControler = pSearchDelegate->GetControler();
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

        if (NULL == pBSFTContact)
        {
            return;
        }

        if (pBSFTContact->PreciseSearch() && pBSFTContact->IsPreciseSearchResult())
        {
            pSearchDelegate->SetPageDataIndex(0);
            pSearchDelegate->RefreshFrameList();
        }
    }
    else if (DIRECTORY_SEARCH_TYPE_GENBAND == m_nSearchType)
    {
        CDirectoryControlerPtr pControler = pSearchDelegate->GetControler();
        CGenbandContactControler * pGBContact = CGenbandContactControler::GetControler(pControler);

        if (NULL == pGBContact)
        {
            return;
        }

        if (pGBContact->PreciseSearch() && pGBContact->IsPreciseSearchResult())
        {
            pSearchDelegate->SetPageDataIndex(0);
            pSearchDelegate->RefreshFrameList();
        }
    }
#else
    if (NULL == pSearchDelegate || DIRECTORY_SEARCH_TYPE_BROADSOFT != m_nSearchType)
    {
        return;
    }

    CDirectoryControlerPtr pControler = pSearchDelegate->GetControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return;
    }

    if (pBSFTContact->PreciseSearch() && pBSFTContact->IsPreciseSearchResult())
    {
        pSearchDelegate->SetPageDataIndex(0);
        pSearchDelegate->RefreshFrameList();
    }
    // 分段下载开启时，防止搜索失败，不刷新title显示数目
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 0)
    {
        pBSFTContact->RefreshTotalCount();
    }
#endif
}

CFrameList * CDlgDirSearch::GetContactFrameList() const
{
    return m_pContactList;
}

void CDlgDirSearch::SetSearchType(int nSearchType)
{
    m_nSearchType = nSearchType;
}

int CDlgDirSearch::GetSearchType() const
{
    return m_nSearchType;
}

void CDlgDirSearch::SetContactType(int nContactType)
{
    m_nContactType = nContactType;
}

int CDlgDirSearch::GetContactType() const
{
    return m_nContactType;
}

void CDlgDirSearch::SetInitSearchKey(int nKeyCode)
{
    if (PHONE_KEY_NONE == nKeyCode)
    {
        return;
    }

    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
    {
        QKeyEvent keyEvt(QKeyEvent::KeyPress, nKeyCode, 0);

        qPassEventToEdit(pFocusWidget, &keyEvt);
    }
}

bool CDlgDirSearch::OnLastItemNext()
{
    return false;
}

bool CDlgDirSearch::OnFirstItemPrevious()
{
    return false;
}

void CDlgDirSearch::OnKeyboardShow()
{
    AdjustFrameList(false);
}

void CDlgDirSearch::OnKeyboardHide()
{
    AdjustFrameList(true);
}

void CDlgDirSearch::AdjustFrameList(bool bReset)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }
    if (NULL == m_pContactList)
    {
        return;
    }

    if (bReset)
    {
        m_pContactList->SetRect(RECT_DIR_CONTACT);
        return;
    }

    DirUI_AdjustFrameList(m_pContactList
                          , bReset
                          , RECT_DIR_CONTACT
                          , DLG_DIR_SEARCH_ITEM_OFFSET
                          , RECT_DIRECTORY.y());
}

bool CDlgDirSearch::IsLastEditItem(CListItemPtr pListItem)
{
    if (NULL == pListItem)
    {
        return false;
    }
    if (m_pBsftSearchLayout != NULL
            && m_nSearchType == DIRECTORY_SEARCH_TYPE_BROADSOFT)
    {
        return m_pBsftSearchLayout->IsLastEditItem();
    }
#if IF_FEATURE_GBBOOK
    else if (m_pGBSearchLayout
             && m_nSearchType == DIRECTORY_SEARCH_TYPE_GENBAND)
    {
        return m_pGBSearchLayout->IsLastEditItem();
    }
#endif
    return false;
}

void CDlgDirSearch::OnCleanContent(CFrameListBase * pFrameList)
{

}

//子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
QPixmap CDlgDirSearch::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_BACKGROUND);
}

void CDlgDirSearch::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    if (NULL == pFrameList || pFrameList != m_pSearchList)
    {
        return;
    }

    int i = 0;
    int nItemSelIndex = 0;
    int nSearchCount = m_nSearchCount;
    QString arrTitle[DIRECTORY_SEARCH_TYPE_MAX] = {LANG_TRANSLATE(TRID_SEARCH_ALL_CONTACTS),
                                                   LANG_TRANSLATE(TRID_SEARCH_LDAP_CONTACTS),
                                                   LANG_TRANSLATE(TRID_SEARCH_NETWORK_CONATCTS)
#if IF_FEATURE_GBBOOK
                                                   , LANG_TRANSLATE(TRID_SEARCH_GENBAND_CONATCTS)
#endif
                                                  };

    if (nSearchCount > DIRECTORY_SEARCH_TYPE_MAX)
    {
        nSearchCount = DIRECTORY_SEARCH_TYPE_MAX;
    }

    m_pSearchList->SetTotalDataCount(nSearchCount);

    yl::string strDefaultIME(configParser_GetConfigString(kszContactSearchDefaultIME));

    if (strDefaultIME.empty())
    {
        strDefaultIME = IME_Abc;
    }

    for (; i < nSearchCount; ++i)
    {
        int nCurSearchType = m_arrSearchType[i];

        if (nCurSearchType >= DIRECTORY_SEARCH_TYPE_MAX)
        {
            continue;
        }

        bool bAdd = false;
        CContactSearchItemPtr pSearchItem = (CContactSearchItemPtr)m_pSearchList->GetItemByIndex(i);

        if (NULL == pSearchItem)
        {
            bAdd = true;
            pSearchItem = new CContactSearchItem;

            if (NULL == pSearchItem)
            {
                continue;
            }
        }
        pSearchItem->SetBtnSearch(i == 0);
        pSearchItem->ClearContent();
        pSearchItem->SetDataIndex(i);
        pSearchItem->SetAction(this);
        pSearchItem->SetSearchType(nCurSearchType);
        pSearchItem->SetTitle(arrTitle[nCurSearchType]);

        if (DIRECTORY_SEARCH_TYPE_BROADSOFT != nCurSearchType
#if IF_FEATURE_GBBOOK
                && DIRECTORY_SEARCH_TYPE_GENBAND != nCurSearchType
#endif
           )
        {
            pSearchItem->SetSearch(true);

            QLineEdit * pEdit = pSearchItem->GetEdit();

            if (NULL != pEdit && bAdd)
            {
                yl::string strDefaultIME(configParser_GetConfigString(kszContactSearchDefaultIME));

                bool bCustomIme = Ime_GetCustomImeCount() > 0;

                if (strDefaultIME.empty())
                {
                    strDefaultIME = bCustomIme ? IME_DIY : IME_Abc;
                }

                if (bCustomIme)
                {
                    qInstallIME(pEdit, IME_CUSTOM);
                }
                else
                {
                    qInstallIME(pEdit, IME_Name);
                }

                qSetCurrentIME(pEdit, strDefaultIME.c_str());
                qLineEditUtility::SetMaxLength(pEdit, EDIT_TYPE_CONTACT_NUMBER);

                if (FullKeyboardEnabled())
                {
                    pSearchItem->InstallKeyboard(INPUT_DIALOG_SEARCH);
                    // 捕获Edit的焦点变化不能用eventFilter，因为会跟全键盘冲突
                    connect(pSearchItem, SIGNAL(editFocus(bool)), this, SLOT(OnSearchEditFocus(bool)));
                }
                else
                {
                    pEdit->installEventFilter(this);
                }

                connect(pEdit, SIGNAL(textEdited(const QString &)), this, SLOT(OnKeyWordChanged(const QString &)));
                //connect(pEdit, SIGNAL(returnPressed()), this, SLOT(OnEditKeyOkClick()));
            }
        }
        else
        {
            pSearchItem->SetSearch(false);
        }

        pSearchItem->SetFocus(false);

        if (m_nSearchType == m_arrSearchType[i])
        {
            nItemSelIndex = i;

            if (m_bShowKeyBoard)
            {
                m_bShowKeyBoard = false;
                pSearchItem->DelayShowKeyboard();
            }
        }

        if (bAdd && (!m_pSearchList->AddItem(pSearchItem)))
        {
            pSearchItem->Release();
        }
    }

    int nItemCount = m_pSearchList->GetTotalItemCount();

    for (int nItemIndex = i; i < nItemCount; ++i)
    {
        QLineEdit * pEdit = NULL;
        CContactSearchItemPtr pSearchItem = (CContactSearchItemPtr)m_pSearchList->GetItemByIndex(
                                                nItemIndex);

        if (NULL != pSearchItem)
        {
            pEdit = pSearchItem->GetEdit();
        }

        if (NULL != pEdit)
        {
            pEdit->removeEventFilter(this);
        }

        m_pSearchList->DelItemByIndex(nItemIndex);
    }

    m_pSearchList->SetItemFocusByIndex(nItemSelIndex);
}

bool CDlgDirSearch::IsFrameListAttach(CFrameListBase * pFrameList)
{
    // 只处理左边列表，因此不需要绑定
    return true;
}

void CDlgDirSearch::OnFrameListDetach(CFrameListBase * pFrameList)
{
    // 只处理左边列表，因此不需要绑定
}

void CDlgDirSearch::OnFrameListAttach(CFrameListBase * pFrameList)
{
    // 只处理左边列表，因此不需要绑定
}

void CDlgDirSearch::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    if (NULL == pFrameList || pFrameList != m_pSearchList || NULL == pItem)
    {
        return;
    }

    if (NULL != pFrameList && m_pSearchList == pFrameList && CONTACT_ITEM_ACTION_SEARCH == nAction)
    {
        qSetHideKeyboard();
        BeginSearch();
    }
    if (NULL == m_pContactList || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    int nSearchType = m_nSearchType;
    CContactSearchItemPtr pSearchItem = (CContactSearchItemPtr)pItem;

    m_nSearchType = pSearchItem->GetSearchType();

    if (nSearchType != m_nSearchType)
    {
        AdjustFrameList(true);
        m_pContactList->SetDelegate(NULL);
        StopTimer();

        int nDirType = m_nContactType;
        int nGroupId = kszAllContactId;
        int nUIState = DIR_UI_STATE_SEARCH;

        if (DIRECTORY_SEARCH_TYPE_LDAP == m_nSearchType)
        {
            nDirType = DIRECTORY_TYPE_LDAP;
        }
        else if (DIRECTORY_SEARCH_TYPE_BROADSOFT == m_nSearchType)
        {
            nDirType = DIRECTORY_TYPE_BROADSOFT;
            nUIState = DIR_UI_STATE_PRECISE_SEARCH;
        }
#if IF_FEATURE_GBBOOK
        else if (DIRECTORY_SEARCH_TYPE_GENBAND ==  m_nSearchType)
        {
            nDirType = DIRECTORY_TYPE_GENBAND;
            nUIState = DIR_UI_STATE_PRECISE_SEARCH;
        }
#endif

        CDirectoryControlerPtr pControler = g_DirCtrlFactory.FindControler(nDirType);
        CRemoteServerSearchControler * pRemoteSearchControler =
            CRemoteServerSearchControler::GetServerSearchControler(pControler);

        if (NULL != pRemoteSearchControler)
        {
            nGroupId = pRemoteSearchControler->GetSearchGroupId();
        }
        else if (!pControler.IsEmpty())
        {
            nGroupId = pControler->GetGroupId();
        }

        g_DirectoryMgr.SetCurDirType(nDirType);
        g_DirectoryMgr.CheckContactDelegate(nUIState, nGroupId, false);

        if (!pControler.IsEmpty()
                && nDirType == DIRECTORY_TYPE_REMOTE_SEARCH
                && m_nSearchType == DIRECTORY_SEARCH_TYPE_NORMAL)
        {
            m_SoftkeyData[2].m_strSoftkeyAction = TRID_SEARCH;
            m_SoftkeyData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SEARCH);
            m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_CONTACT_SEARCH;

            _MainWND.DrawSoftkey(this);
        }

        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

        if (pDelegate.IsEmpty() || NULL == m_pContactList)
        {
            return;
        }

        m_pContactList->SetDelegate(pDelegate.Get());

        int nModType = g_DirectoryMgr.GetModType(nDirType);
        bool bReadOnly = !CDirectoryControler::IsDirCanEidt(nModType);

        if (!pControler.IsEmpty())
        {
            pControler->SetReadOnly(bReadOnly);
        }

        CSearchDelegate * pSearchDelegate = CSearchDelegate::GetDirDelegate(pDelegate);

        if (NULL != pSearchDelegate)
        {
            yl::string strSearchKey = "";

            pSearchDelegate->Init(nDirType, nGroupId, strSearchKey);

            m_pContactList->SetOnlyTitleLine(false);
            m_pContactList->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP, PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                           NULL);
        }
        else if (DIR_UI_STATE_PRECISE_SEARCH == nUIState)
        {
#if IF_FEATURE_GBBOOK
            if (DIRECTORY_SEARCH_TYPE_GENBAND == m_nSearchType)
            {
                InitGBSearchLayout();
            }
            else
#endif
            {
                InitBsftSearchLayout();
            }
        }

        if (DIRECTORY_SEARCH_TYPE_BROADSOFT == m_nSearchType)
        {
            CListItemPtr pItem = GetSearchItem(nSearchType);
            CContactSearchItemPtr pContactSearchItem = CContactSearchItem::GetContactSearchItem(pItem);

            if (NULL != pContactSearchItem)
            {
                qSetIsShowWnd(pContactSearchItem->GetEdit(), false);
            }
        }
        else if (DIRECTORY_SEARCH_TYPE_LDAP == m_nSearchType)
        {
            QString strSearchKey = QLatin1String("");

            SearchContact(strSearchKey);
            return;
        }

        m_pContactList->RefreshData(0);
    }
}

CListItem* CDlgDirSearch::GetSearchItem(int nSearchType)
{
    if (NULL == m_pSearchList)
    {
        return NULL;
    }

    int nIndex = INVALID_ITEM_INDEX;
    int nSearchCount = m_nSearchCount;

    if (nSearchCount > DIRECTORY_SEARCH_TYPE_MAX)
    {
        nSearchCount = DIRECTORY_SEARCH_TYPE_MAX;
    }

    for (int i = 0; i < nSearchCount; ++i)
    {
        if (nSearchType == m_arrSearchType[i])
        {
            nIndex = i;
            break;
        }
    }

    return m_pSearchList->GetItemByIndex(nIndex);
}

void CDlgDirSearch::OnSearchTypeChanged(int iIndex)
{
#if IF_FEATURE_GBBOOK
    if (m_pGBSearchLayout == NULL)
    {
        return;
    }

    CComboBoxListItemPtr pSearchTypeItem = m_pGBSearchLayout->GetSearchTypeItem();
    CEditListItemPtr pKeyItem = m_pGBSearchLayout->GetSearchKeyItem();

    if (pSearchTypeItem == NULL || pKeyItem == NULL)
    {
        return;
    }

    GBBOOK_OPERATION_TYPE eCurType = (GBBOOK_OPERATION_TYPE)pSearchTypeItem->GetItemData();
    yl::string strKey = GAB_GetCurentGabKey();

    if (eCurType != GAB_GetCurrentGabOptType())
    {
        m_pGBSearchLayout->ClearEdit();
    }
    else
    {
        pKeyItem->SetValue(strKey.c_str());
    }

#endif
}
