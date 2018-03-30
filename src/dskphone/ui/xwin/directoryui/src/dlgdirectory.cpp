#include "directoryuicommon.h"
#include "dlgdirectory.h"
#include "xwindow/xListView.h"
#include "dsklog/log.h"
#include "directorymgr.h"
#include "browseadapter.h"

CDlgDirectory::CDlgDirectory()
#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    : m_pTagTitle(NULL)
#endif
{
}

CDlgDirectory::~CDlgDirectory()
{
}

CBaseDialog * CDlgDirectory::CreateInstance()
{
    CDlgDirectory * pDlgDirectory = new CDlgDirectory();

    if (NULL != pDlgDirectory)
    {
        pDlgDirectory->loadContent(DIR_UI_LAYOUT_FILE_DIRECTORY);
    }

    return pDlgDirectory;
}

#if !IS_COLOR && IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
bool CDlgDirectory::SetTitlePageInfo(const yl::string & strPageInfo)
{
    if (NULL != m_pTagTitle && m_pTagTitle->checkVisible())
    {
        m_pTagTitle->SetPageIndexTip(strPageInfo);
        return true;
    }

    return CDlgDirectoryBase::SetTitlePageInfo(strPageInfo);
}
#endif

#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
void CDlgDirectory::loadChildrenElements(xml_node & node)
{
    CDlgDirectoryBase::loadChildrenElements(node);

    m_pTagTitle = static_cast<CTagTitle*>(getViewById(DIR_UI_ID_TAG_TITLE));
}

void CDlgDirectory::RefreshTitle()
{
    if (NULL == m_pTagTitle)
    {
        CDlgDirectoryBase::RefreshTitle();
        return;
    }

    if (!IsShowTitle())
    {
        m_pTagTitle->hide(true);
        return;
    }

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CBrowseAdapter* pBrowserAdapter = CBrowseAdapter::GetBrowserAdapter(pAdapter);
    if (NULL == pBrowserAdapter || !pBrowserAdapter->IsGroupSwitchPage())
    {
        ChangeTitleStatus(false);
        UpdateTitleInfo();
        return;
    }

    ChangeTitleStatus(true);

    IDirUIHelperPtr pDirUIHelper = pBrowserAdapter->GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);
    if (NULL == pUIHelper)
    {
        return;
    }

    VEC_STRING listTitle;
    int nGroupIndex = 0;

    pUIHelper->GetGroupTitleList(nGroupIndex, listTitle);

    m_pTagTitle->SetTagList(listTitle);

    m_pTagTitle->SetCurrenTagIndex(nGroupIndex);

#if !IS_COLOR
    //黑白屏显示为列表焦点Index
    yl::string strPageIndex = pBrowserAdapter->GetPageInfo();
    m_pTagTitle->SetPageIndexTip(strPageIndex);
#endif
}

void CDlgDirectory::ChangeTitleStatus(bool bShowTag)
{
    if (NULL == m_pTagTitle)
    {
        m_titleNormal.Show();
        return;
    }

    if (bShowTag)
    {
        m_pTagTitle->show();
        m_titleNormal.Hide();
    }
    else
    {
        m_titleNormal.Show();
        m_pTagTitle->hide();
    }
}

#endif
