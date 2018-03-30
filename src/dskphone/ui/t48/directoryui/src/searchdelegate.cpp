#include "searchdelegate.h"
#include "baseui/framelist.h"
#include "baseui/framesearchtitle.h"


CSearchDelegate::CSearchDelegate()
    : CDirectoryListDelegate(DIR_UI_STATE_SEARCH)
    , m_strSearchKey("")
{
}

CSearchDelegate::~CSearchDelegate(void)
{
}

CSearchDelegate * CSearchDelegate::GetDirDelegate(CDirectoryListDelegatePtr & pDelegate)
{
    if (pDelegate.IsEmpty() || DIR_UI_STATE_SEARCH != pDelegate->GetState())
    {
        return NULL;
    }

    return (CSearchDelegate *)pDelegate.Get();
}

bool CSearchDelegate::Init(int nType, int nGroupId, const yl::string & strSearchKey)
{
    if (!CDirectoryListDelegate::Init(nType, nGroupId))
    {
        return false;
    }

    m_strSearchKey = strSearchKey;

    CDirectoryControlerPtr pControler = GetControler();

    if (!pControler.IsEmpty())
    {
        pControler->OnEnterSearch();
    }

    return true;
}

void CSearchDelegate::OnSearchChange(const yl::string & strSearchKey)
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    //重复内容也可以搜索
//  if (strSearchKey == m_strSearchKey && "" != strSearchKey)
//  {
//      return;
//  }

    m_strSearchKey = strSearchKey;
    m_nPageDataIndex = 0;

    DIRUI_INFO("OnSearchChange:%s", m_strSearchKey.c_str());

    if (OnBeginSearch())
    {
        RefreshFrameList();
    }
    else
    {
        m_pFrameList->ClearContent(true);
    }
}

const yl::string & CSearchDelegate::GetSearchKey() const
{
    return m_strSearchKey;
}

bool CSearchDelegate::OnBeginSearch()
{
    CDirectoryControlerPtr pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    if (pControler->PreSearchContact())
    {
#ifdef FL_SUPPORT_TITLE
        m_pFrameList->SetFrameListTitle(NULL);
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NONE_TITLE_ITEM_COUNT);
#endif
        return true;
    }

    return false;
}
