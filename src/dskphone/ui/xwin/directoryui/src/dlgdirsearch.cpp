#include "directoryuicommon.h"
#include "dlgdirsearch.h"
#include "directorymgr.h"
#include "searchadapter.h"
#include "keymap.h"
#include "commonunits/modcommonunits.h"
#include "imeedit/ximeedit.h"
#include "xwindow/xWindowManager.h"
#include "baseui/include/inputmethod.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"


namespace
{
const char * DIR_UI_ID_EDIT_KEY = "editKey";

#define SEARCH_DELAY_TIME 300
}

CDlgDirSearch::CDlgDirSearch()
    : m_pEdit(NULL)
    , m_bEditInit(false)
{
}

CDlgDirSearch::~CDlgDirSearch()
{
}

CBaseDialog * CDlgDirSearch::CreateInstance()
{
    CDlgDirSearch * pDlgDirSearch = new CDlgDirSearch;

    if (NULL != pDlgDirSearch)
    {
        pDlgDirSearch->loadContent(DIR_UI_LAYOUT_FILE_SEARCH);
    }

    return pDlgDirSearch;
}

void CDlgDirSearch::Init()
{
    if (NULL != m_pEdit)
    {
        yl::string strSupportIME;
        yl::string strIME;

        DIRUI_GetSearchIME(strSupportIME, strIME);

        IME_InstallIME(m_pEdit, strSupportIME.c_str());

        m_pEdit->SetIme(strIME);
        m_pEdit->SetMaxLength(MAX_LEN_PHONE_NUMBER_LONG);

        SLOT_CONNECT(m_pEdit, signalTextConfirmChange, this, CDlgDirSearch::OnKeyWordChanged);
        SLOT_CONNECT(m_pEdit, signalImeChanged, this, CDlgDirSearch::slotOnIMEChanged);
    }
}

void CDlgDirSearch::Uninit()
{
    m_tmTimer.KillTimer();

    if (NULL != m_pEdit)
    {
        SLOT_DISCONNECT(m_pEdit, signalTextConfirmChange, this, CDlgDirSearch::OnKeyWordChanged);
        SLOT_DISCONNECT(m_pEdit, signalImeChanged, this, CDlgDirSearch::slotOnIMEChanged);
    }
}

void CDlgDirSearch::OnFocusChanged(bool bFocused)
{
    if (!bFocused)
    {
        m_tmTimer.KillTimer();
    }
}

void CDlgDirSearch::loadChildrenElements(xml_node & node)
{
    CDlgDirectoryBase::loadChildrenElements(node);

    m_pEdit = static_cast<xEditTextView *>(getViewById(DIR_UI_ID_EDIT_KEY));
}

void CDlgDirSearch::SetInitSearchKey(int nKeyCode)
{
    if (NULL == m_pEdit)
    {
        return;
    }

    m_pEdit->setFocus();
    m_bEditInit = true;
    m_pEdit->SetInitText("");
    m_bEditInit = false;

    if (PHONE_KEY_NONE != nKeyCode)
    {
        g_WorkStation.xWorkStation::dispatchKeypadEvent(nKeyCode, true);

        return;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (!pControler.IsEmpty() && pControler->IsEmptyKeySearch())
    {
        SearchContact("");
    }
}

void CDlgDirSearch::slotOnIMEChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                     LPVOID pData)
{
    if (NULL == m_pEdit)
    {
        return;
    }

    yl::string strIME = m_pEdit->GetImeName();

    if (strIME.empty())
    {
        strIME = "Abc";
    }

    configParser_SetConfigString(kszContactSearchDefaultIME, strIME.c_str(), CONFIG_LEVEL_PHONE);
}

bool CDlgDirSearch::IsFocusEdit() const
{
    if (NULL != m_pEdit && m_pEdit->hasFocus())
    {
        return true;
    }

    return false;
}

bool CDlgDirSearch::ProcessEditKeyPress(int nKey)
{
    if (NULL == m_pListView || NULL == m_pEdit || m_pEdit->hasFocus())
    {
        return false;
    }

    m_pListView->FocusItemByIndex(INVALID_ITEM_INDEX);
    m_pEdit->setFocus();

    g_WorkStation.xWorkStation::dispatchKeypadEvent(nKey, true);

    return true;
}

bool CDlgDirSearch::onKeyPressedEvent(int nKeyCode)
{
    //焦点在输入框上，则进行搜索
    if (PHONE_KEY_OK == nKeyCode)
    {
        if (IsFocusEdit())
        {
            OnEditKeyOkClick();

            return true;
        }
    }
    else if ((nKeyCode >= PHONE_KEY_0 && nKeyCode <= PHONE_KEY_9)
             || ((PHONE_KEY_STAR == nKeyCode || PHONE_KEY_POUND == nKeyCode)
                 && !commonUnits_IsSendDigitKey(nKeyCode)))
    {
        if (ProcessEditKeyPress(nKeyCode))
        {
            return true;
        }
    }
    else if (PHONE_KEY_LEFT == nKeyCode || PHONE_KEY_RIGHT == nKeyCode)
    {
        return true;
    }
    else if (PHONE_KEY_UP == nKeyCode || PHONE_KEY_DOWN == nKeyCode)
    {
        if (NULL != m_pEdit && m_pEdit->hasFocus())
        {
#if IS_COLOR
            if (NULL == m_pListView || m_pListView->GetTotalDataCount() <= 0)
            {
                return true;
            }
#endif
            return false;
        }
    }

    return CDlgDirectoryBase::onKeyPressedEvent(nKeyCode);
}

bool CDlgDirSearch::onFocusMoveEvent(xView * pNewFocus, xView * pOldFocus)
{
    if (pNewFocus == m_pEdit || (pOldFocus == m_pEdit && NULL != m_pListView
                                 && m_pListView->hasFocus()))
    {
        if (NULL != m_pListView && !m_pListView->hasFocus())
        {
            m_pListView->SetItemFocusByIndex(INVALID_ITEM_INDEX);
        }

        CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

        if (!pAdapter.IsEmpty())
        {
            pAdapter->OnFocusChanged();
        }

        return true;
    }

    return false;
}

#if IF_HIDE_TITLE_BUT_EDIT
void CDlgDirSearch::RefreshTitle()
{
    UpdateTitleInfo();
}
#endif

void CDlgDirSearch::OnEditKeyOkClick()
{
    if (NULL == m_pEdit)
    {
        return;
    }

    m_pEdit->EndIme();

    SearchContact(m_pEdit->GetText());
}

void CDlgDirSearch::OnKeyWordChanged(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                     int nDataBytes, LPVOID pData)
{
    m_tmTimer.KillTimer();

    if (NULL == m_pEdit || m_bEditInit)
    {
        return;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (pControler.IsEmpty() || pControler->IsManualSearch())
    {
        return;
    }

    const yl::string & strText = m_pEdit->GetText();

    if (strText.empty())
    {
        SearchContact(strText);
    }
    else
    {
        m_tmTimer.SetTimer(SEARCH_DELAY_TIME, TIMER_METHOD(this, CDlgDirSearch::OnEditDelayTimerOut));
    }
}

void CDlgDirSearch::OnEditDelayTimerOut(xThreadTimer* timer, LPVOID pExtraData)
{
    m_tmTimer.KillTimer();

    if (NULL == m_pEdit)
    {
        return;
    }

    const yl::string & strText = m_pEdit->GetText();

    SearchContact(strText);
}

void CDlgDirSearch::SearchContact(const yl::string & strKey)
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CSearchAdapter * pSearchAdapter = CSearchAdapter::GetSearchAdapter(pAdapter);

    if (NULL != pSearchAdapter)
    {
        pSearchAdapter->OnSearchChange(strKey);
    }
}

bool CDlgDirSearch::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (!bLongPress && ST_SEARCH == iType)
    {
        OnEditKeyOkClick();

        return true;
    }

    return CDlgDirectoryBase::OnSoftkeyAction(iType, bLongPress);
}
