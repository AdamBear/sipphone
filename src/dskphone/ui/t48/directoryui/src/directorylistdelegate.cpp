#include "directorylistdelegate.h"
#include "dirctrlfactory.h"
#include "diruihelperfactory.h"
#include "delegatefactory.h"
#include "baseui/framelist.h"
#include "baseui/contactbaseitem.h"


CDirectoryListDelegate::CDirectoryListDelegate(int nState)
    : m_nState(nState)
{
    m_bItemFocus = false;
    m_bDefaultFocus = false;
}

CDirectoryListDelegate::~CDirectoryListDelegate(void)
{
    DIRUI_INFO("~CDirectoryListDelegate %p", this);
    UnInit();
}

CDirectoryControlerPtr CDirectoryListDelegate::GetControler()
{
    return m_pControler;
}

CDirUIHelperPtr CDirectoryListDelegate::GetUIHelper()
{
    return m_pUIHelper;
}

// 初始化
bool CDirectoryListDelegate::Init(int nDirType, int nGroupId/* = knInvalidRId*/,
                                  int nPageDataIndex/* = 0*/)
{
    DIRUI_INFO("CDirectoryListDelegate::Init(DirType:%d GroupId:%d PageDataIndex:%d)",
               nDirType, nGroupId, nPageDataIndex);

    m_nPageDataIndex = nPageDataIndex;

    CDirectoryListDelegatePtr pThis = g_DirDelegateFactory.GetContactDelegate(m_nState);

    if (pThis.Get() != this)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = GetControler();

    if (nDirType == GetDirType() && !pControler.IsEmpty())
    {
        pControler->SetGroupId(nGroupId);

        CDirUIHelperPtr pUIHelper = pControler->GetUIHelper();

        if (pUIHelper.IsEmpty() || m_pUIHelper.IsEmpty())
        {
            m_pUIHelper = g_DirUIHelperFactory.GetDirUIHelper(nDirType);
            pUIHelper = GetUIHelper();

            if (pUIHelper.IsEmpty())
            {
                return false;
            }

            pControler->Init(pThis, pUIHelper);
            pUIHelper->Init();
            pUIHelper->SetFrameList(GetFrameList());
        }

        return true;
    }

    UnInit();

    m_pControler = g_DirCtrlFactory.GetControler(nDirType);

    pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    pControler->SetGroupId(nGroupId);

    m_pUIHelper = g_DirUIHelperFactory.GetDirUIHelper(nDirType);

    CDirUIHelperPtr pUIHelper = GetUIHelper();

    if (pUIHelper.IsEmpty())
    {
        return false;
    }

    pControler->Init(pThis, pUIHelper);
    pUIHelper->Init();
    pUIHelper->SetFrameList(GetFrameList());

    return true;
}

// 恢复初始化
void CDirectoryListDelegate::UnInit()
{
    DIRUI_INFO("begin CDirectoryListDelegate::UnInit");
    if (!m_pControler.IsEmpty())
    {
        m_pControler->UnInit();
        m_pControler.Reset();
    }

    if (!m_pUIHelper.IsEmpty())
    {
        m_pUIHelper->UnInit();
        m_pUIHelper.Reset();
    }
    DIRUI_INFO("end CDirectoryListDelegate::UnInit");
}

void CDirectoryListDelegate::InitFrameList()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirUIHelperPtr pUIHelper = GetUIHelper();

    if (pUIHelper.IsEmpty())
    {
#ifdef FL_SUPPORT_TITLE
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NONE_TITLE_ITEM_COUNT);
#else
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NORMAL_ITEM_COUNT);
#endif
        return;
    }

    pUIHelper->SetFrameList(m_pFrameList);

    SetPageDataIndex(GetPageDataIndex());
}

void CDirectoryListDelegate::UnInitFrameList()
{
    CDirUIHelperPtr pUIHelper = GetUIHelper();

    if (pUIHelper.IsEmpty())
    {
        return;
    }

    pUIHelper->SetFrameList(NULL);
}

int CDirectoryListDelegate::GetState() const
{
    return m_nState;
}

int CDirectoryListDelegate::GetDirType()
{
    if (m_pControler.IsEmpty())
    {
        return DIRECTORY_TYPE_NONE;
    }

    return m_pControler->GetType();
}

int CDirectoryListDelegate::GetGroupId()
{
    if (m_pControler.IsEmpty())
    {
        return knInvalidRId;
    }

    return m_pControler->GetGroupId();
}

void CDirectoryListDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CDirectoryListDelegate::OnLoadData(%p %d %d)", pFrameList, nDataIndex, nDataCount);

    CDirectoryControlerPtr pControler = GetControler();

    if (NULL == pFrameList || pFrameList != m_pFrameList || pControler.IsEmpty())
    {
        return;
    }

    if (!pControler->LoadListData(nDataIndex, nDataCount))
    {
        m_pFrameList->ClearContent(true);
    }
}

void CDirectoryListDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem,
        int nAction)
{
    DIRUI_INFO("CDirectoryListDelegate::OnItemAction(FrameList:%p Item:%p Action:%d)",
               pFrameList, pItem, nAction);

    if (NULL == pFrameList || pFrameList != m_pFrameList || NULL == pItem)
    {
        return;
    }

    CDirectoryControlerPtr pControler = GetControler();
    CDirUIHelperPtr pUIHelper = GetUIHelper();

    if (pControler.IsEmpty() || pUIHelper.IsEmpty())
    {
        return;
    }

    int nItemId = pUIHelper->GetLisItemId(pItem);

    if (knInvalidRId == nItemId)
    {
        return;
    }

    int nCtrlAction = pUIHelper->GetActionByItem(nAction);

    if (DIR_ACTION_NONE != nCtrlAction)
    {
        pControler->OnAction(nCtrlAction, pItem->GetDataIndex(), nItemId);
    }
}

#ifdef FL_SUPPORT_TITLE
void CDirectoryListDelegate::OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle,
        int nAction)
{
    if (NULL == pFrameList || pFrameList != m_pFrameList || NULL == pTitle)
    {
        return;
    }

    CDirectoryControlerPtr pControler = GetControler();
    CDirUIHelperPtr pUIHelper = GetUIHelper();

    if (pControler.IsEmpty() || pUIHelper.IsEmpty())
    {
        return;
    }

    int nCtrlAction = pUIHelper->GetActionByTitle(m_nState, nAction);

    if (DIR_ACTION_NONE != nCtrlAction)
    {
        int nDataId = knInvalidRId;

        if (DIR_ACTION_BSFT_UPDATE == nCtrlAction || DIR_ACTION_REMOTE_UPDATE == nCtrlAction
#if IF_FEATURE_GBBOOK
                || DIR_ACTION_GENBAND_UPDATE == nCtrlAction
#endif
           )
        {
            nDataId = pControler->GetGroupId();
        }

        pControler->OnAction(nCtrlAction, -1, nDataId);
    }
}
#endif
