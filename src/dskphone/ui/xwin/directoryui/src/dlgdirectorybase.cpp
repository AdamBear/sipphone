#include "dlgdirectorybase.h"

#include "translateiddefine.h"
#include "common/common_data_define.h"
#include "messagebox/modmessagebox.h"
#include "languagehelper/modlangtranslate.h"
#include "edkui/include/modedkui.h"

#include "directorymgr.h"

#include "xwindow/xWindowManager.h"
#if IS_COLOR
#include "baseui/include/modbaseui.h"
#endif

#include "dsskey/include/moddsskey.h"
#include "dsskeyui/moddsskeyui.h"
#include "moddirectoryui.h"

CDlgDirectoryBase::CDlgDirectoryBase()
    : CBaseDialog(this, eWT_Directory)
    , m_pListView(NULL)
{
}

CDlgDirectoryBase::~CDlgDirectoryBase()
{
}

void CDlgDirectoryBase::Init()
{

}

void CDlgDirectoryBase::Uninit()
{
    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
        m_pListView->SetAdapter(NULL);
    }

#if DIR_SUPPORT_SELECT_CONTACT
    g_DirectoryMgr.CancelSelectDirectory();
#endif
}

void CDlgDirectoryBase::UpdateWnd(bool bUIReturn/* = false*/)
{
    if (!bUIReturn)
    {
        return;
    }

    // 返回时重新刷列表
    if (NULL == m_pListView)
    {
        DIRUI_WARN("CDlgDirectory::UpdateWnd() fail. ListView is NULL");
        return;
    }

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

    if (pAdapter.IsEmpty())
    {
        DIRUI_WARN("CDlgDirectory::UpdateWnd() fail. Adapter is NULL");
        return;
    }

    pAdapter->RefreshListView();

    RefreshSoftkey();

    RefreshTitle();
}

void CDlgDirectoryBase::OnReturnToIdle()
{
    g_DirectoryMgr.OnReturnIdle(this);
}

xListView * CDlgDirectoryBase::GetListView() const
{
    return m_pListView;
}

bool CDlgDirectoryBase::SetTitle(const yl::string & strTitle)
{
    m_titleNormal.SetTitle(strTitle);

    return true;
}

bool CDlgDirectoryBase::SetTitlePageInfo(const yl::string & strPageInfo)
{
    m_titleNormal.SetPageInfo(strPageInfo);

    return true;
}

void CDlgDirectoryBase::loadChildrenElements(xml_node & node)
{
    xLinearLayout::loadChildrenElements(node);

    m_pListView = static_cast<xListView *>(getViewById(BASE_UI_ID_LIST_VIEW));

    m_titleNormal.SetTitleView(getViewById(DIR_UI_ID_NORMAL_TITLE));
}

void CDlgDirectoryBase::RefreshTitle()
{
    if (!IsShowTitle())
    {
        return;
    }

    m_titleNormal.Show();

    UpdateTitleInfo();
}

void CDlgDirectoryBase::GetSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

    if (pAdapter.IsEmpty() || NULL == m_pListView)
    {
        return;
    }

    int nIndex = 0;

    for (; nIndex != SOFTKEY_NUM; ++nIndex)
    {
        CSoftKeyBarData objSoftkeyData;

        objSoftkeyData.m_iType = ST_EMPTY;

        vecSoftKey.push_back(objSoftkeyData);
    }

    if (0 == nIndex)
    {
        return;
    }

    vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);
    vecSoftKey[0].m_iType = ST_BACK;

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();

    if (!pAdapter->GetStateSoftKey(pFocusItem, vecSoftKey))
    {
        IDirUIHelperPtr pDirUIHelper = pAdapter->GetUIHelper();
        CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

        if (NULL != pUIHelper)
        {
            pUIHelper->GetSoftKey(pFocusItem, pAdapter->GetState(), vecSoftKey);
        }
    }

#if DIR_SUPPORT_SELECT_CONTACT
    if (g_DirectoryMgr.IsInSelecting() && ST_SEND == vecSoftKey[3].m_iType)
    {
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
    }
#endif
}

bool CDlgDirectoryBase::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (!bPress || MessageBox_GetMessageBoxCount() > 0)
    {
        return false;
    }

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

    // 这几个按键走过滤流程，因为会有默认的全局处理
    if (!pAdapter.IsEmpty() && (PHONE_KEY_HANDSET_OFF_HOOK == nKeyCode
                                || PHONE_KEY_HANDFREE == nKeyCode || PHONE_KEY_EHS_OPEN == nKeyCode))
    {
        return pAdapter->ProcessKeyPress(nKeyCode);
    }

    return false;
}

bool CDlgDirectoryBase::onKeyPressedEvent(int nKeyCode)
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

    if (!pAdapter.IsEmpty())
    {
        if ((nKeyCode >= PHONE_KEY_0 && nKeyCode <= PHONE_KEY_9)
                || PHONE_KEY_STAR == nKeyCode || PHONE_KEY_POUND == nKeyCode)
        {
            if (pAdapter->ProcessDigitKeyPress(nKeyCode))
            {
                return true;
            }
        }
        else if (nKeyCode >= PHONE_KEY_LINE1 && nKeyCode <= PHONE_KEY_LINE10)
        {
            int nDsskeyId = dsskeyUI_GetDsskeyIDByKeycode(nKeyCode);
            DssKey_Type dsskeyType = dsskey_GetDsskeyType(nDsskeyId);

            if (DT_LINE != dsskeyType)
            {
                if (DM_IsDirDssKey(dsskeyType))
                {
                    return true;
                }
                return false;
            }

            if (pAdapter->ProcessLineKeyPress(nKeyCode))
            {
                return true;
            }
        }
        else if (pAdapter->ProcessKeyPress(nKeyCode))
        {
            return true;
        }
    }

    if (PHONE_KEY_CANCEL == nKeyCode && g_DirectoryMgr.LeaveCurrentState())
    {
        return true;
    }

    return false;
}

#if IS_COLOR
xBrush CDlgDirectoryBase::GetBackGroundBrush()
{
    xBrush brushBG;

    BaseUI_GetNormalDialogBG(brushBG);

    return brushBG;
}
#endif

bool CDlgDirectoryBase::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

    if (pAdapter.IsEmpty())
    {
        return false;
    }

    if (pAdapter->ProcessAction(iType))
    {
        return true;
    }

    if (ST_BACK == iType && g_DirectoryMgr.LeaveCurrentState())
    {
        return true;
    }

    return false;
}

bool CDlgDirectoryBase::IsShowTitle()
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

    if (pAdapter.IsEmpty())
    {
        return false;
    }

    IDirUIHelperPtr pDirUIHelper = pAdapter->GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL != pUIHelper && !pUIHelper->IsShowTitle())
    {
        m_titleNormal.Hide();
        return false;
    }

    return true;
}

void CDlgDirectoryBase::UpdateTitleInfo()
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();

    if (pAdapter.IsEmpty())
    {
        return ;
    }

    yl::string strTitle = EdkUI_GetMenuIDByDirType(pAdapter->GetDirType());

    if (strTitle.empty())
    {
        pAdapter->GetTitle(strTitle);
    }

    SetTitle(strTitle);

    yl::string strPageInfo;

    if (pAdapter->IsShowPageInfo())
    {
        strPageInfo = pAdapter->GetPageInfo();
    }

    SetTitlePageInfo(strPageInfo);
}
