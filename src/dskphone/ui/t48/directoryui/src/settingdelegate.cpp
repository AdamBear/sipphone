#include "settingdelegate.h"

#ifdef DIR_SUPPORT_SETTING
#include <algorithm>
#ifdef FL_SUPPORT_TITLE
#include "contactlisttitle.h"
#endif
#include "commondirctrl.h"
#include "directorycontroler.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "baseui/contactbaseitem.h"
#include "baseui/framelist.h"
#include "baseui/framelistaction.h"
#if IF_FEATURE_GBBOOK
#include "genbandcontactcontroler.h"
#endif


int CSettingDelegate::s_nTotalCount = 0;
ST_DIR_EDIT_STATUS CSettingDelegate::s_stEditStatus;

CSettingDelegate::CSettingDelegate()
{
    m_nState = DIR_UI_STATE_SETTING;
}

CSettingDelegate::~CSettingDelegate(void)
{
}

CSettingDelegate * CSettingDelegate::GetDirDelegate(CDirectoryListDelegatePtr & pDelegate)
{
    if (pDelegate.IsEmpty() || DIR_UI_STATE_SETTING != pDelegate->GetState())
    {
        return NULL;
    }

    return (CSettingDelegate *)pDelegate.Get();
}

bool CSettingDelegate::Init(int nType, int nGroupId, int nPageDataIndex/* = 0*/)
{
    SetTotalCount(0);

    return CDirectoryListDelegate::Init(nType, nGroupId, nPageDataIndex);
}

bool CSettingDelegate::OnChangeGroup(int nGroupId, bool bRefresh/* = true*/)
{
    CDirectoryControlerPtr pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    ResetSelectCount();

    return CBrowserDelegate::OnChangeGroup(nGroupId, bRefresh);
}

void CSettingDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    CDirectoryListDelegate::OnLoadData(pFrameList, nDataIndex, nDataCount);

    if (NULL == m_pFrameList || pFrameList != m_pFrameList)
    {
        return;
    }

    if (0 == m_pFrameList->GetTotalItemCount())
    {
        SetTotalCount(0);

        CDirUIHelperPtr pUIHelper = g_DirectoryMgr.GetActiveUIHelper();

        if (!pUIHelper.IsEmpty())
        {
            pUIHelper->RefreshSettingTitle(0, 0);
        }
    }
}

void CSettingDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem)
    {
        return;
    }

    int nContactItemType = CContactBaseItem::GetContactBaseItemType();
    int nItemType = pItem->GetType();

    if ((CONTACT_ITEM_ACTION_CHECK != nAction && LIST_ITEM_ACTION_SELECT != nAction)
            || (nContactItemType != (nContactItemType & nItemType)))
    {
        CDirectoryListDelegate::OnItemAction(pFrameList, pItem, nAction);
        return;
    }

    CContactBaseItemPtr pContactItem = (CContactBaseItemPtr)pItem;
    int nContactId = pContactItem->GetId();
    bool bAdd = false;

    if (pContactItem->IsChecked())
    {
        if (!s_stEditStatus.bSelectAll)
        {
            bAdd = true;
        }
    }
    else if (s_stEditStatus.bSelectAll)
    {
        bAdd = true;
    }

    int nIndex = GetIndexById(nContactId);

    if (bAdd)
    {
#if IF_FEATURE_GBBOOK
        // GB联系人只有删除一个接口
        CDirectoryControlerPtr pControler = GetControler();
        CGenbandContactControler * pDirCtrl = CGenbandContactControler::GetControler(pControler);
        if (pDirCtrl != NULL)
        {
            s_stEditStatus.vecSelectId.clear();
            int nSixe = m_pFrameList->GetTotalItemCount();
            for (int i = 0; i < nSixe; i++)
            {
                CContactBaseItemPtr pTempItem = (CContactBaseItemPtr)m_pFrameList->GetItemByIndex(i);
                if (pTempItem == NULL)
                {
                    continue;
                }

                if (pTempItem->GetId() != nContactId)
                {
                    pTempItem->SetChecked(false, true);
                }
            }
        }
#endif

        if (-1 == nIndex)
        {
            s_stEditStatus.vecSelectId.push_back(nContactId);
        }
    }
    else if (-1 != nIndex)
    {
        s_stEditStatus.vecSelectId.removeAt(nIndex);
    }

    CFrameListTitlePtr pTitle = pList->GetFrameListTitle();

    if (NULL == pTitle || !CContactListTitle::IsContactListTitle(pTitle))
    {
        return;
    }

    CContactListTitlePtr pContactListTitle = (CContactListTitlePtr)pTitle;

    if (NULL == pContactListTitle)
    {
        return;
    }

    pContactListTitle->SetSelectAmount(GetSelectCount(), true);
}

void CSettingDelegate::OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction)
{
    if (NULL == pFrameList || pFrameList != m_pFrameList || NULL == pTitle
            || (!CContactListTitle::IsContactListTitle(pTitle)))
    {
        return;
    }

    if (LIST_TITLE_ACTION_CHECK_STATE != nAction)
    {
        DIRUI_INFO("CSettingDelegate::OnTitleAction(FrameList:%p Title:%p Action:%d)",
                   pFrameList, pTitle, nAction);

        CDirUIHelperPtr pUIHelper = GetUIHelper();
        CDirectoryControlerPtr pControler = GetControler();
        CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

        if (NULL == pCommonDirCtrl || pUIHelper.IsEmpty())
        {
            return;
        }

        int nCtrlAction = pUIHelper->GetActionByTitle(m_nState, nAction);

        if (DIR_ACTION_NONE == nCtrlAction)
        {
            return;
        }

        VEC_CONTACT_ID vecAllContactId;

        pCommonDirCtrl->GetGroupContactId(vecAllContactId);

        int nSize = vecAllContactId.size();

        if (0 == nSize)
        {
            CDirUIHelper::ShowMsgBox(TRID_NO_CONTACT);
            return;
        }

        if (!IsHadSelected())
        {
            CDirUIHelper::ShowMsgBox(TRID_PLEASE_SELECT_CONTACT);
            return;
        }

        pControler->OnAction(nCtrlAction, -1, knInvalidRId);
        return;
    }

    CContactListTitlePtr pContactListTitle = (CContactListTitlePtr)pTitle;

    s_stEditStatus.vecSelectId.clear();
    s_stEditStatus.bSelectAll = pContactListTitle->IsCheck();

    pContactListTitle->SetSelectAmount(GetSelectCount(), true);

    int nItemCount = pFrameList->GetTotalItemCount();

    for (int i = 0; i < nItemCount; ++i)
    {
        CContactBaseItemPtr pItem = (CContactBaseItemPtr)pFrameList->GetItemByIndex(i);

        if (NULL == pItem)
        {
            continue;
        }

        pItem->SetChecked(s_stEditStatus.bSelectAll, true);
    }
}

bool CSettingDelegate::IsSelectAll() const
{
    if (s_stEditStatus.bSelectAll && 0 == s_stEditStatus.vecSelectId.size())
    {
        return true;
    }

    return (s_stEditStatus.vecSelectId.size() == s_nTotalCount);
}

bool CSettingDelegate::IsHadSelected() const
{
    if (s_nTotalCount <= 0)
    {
        return false;
    }

    if (s_stEditStatus.bSelectAll)
    {
        if (s_stEditStatus.vecSelectId.size() == s_nTotalCount)
        {
            return false;
        }
    }
    else if (0 == s_stEditStatus.vecSelectId.size())
    {
        return false;
    }

    return true;
}

bool CSettingDelegate::IsSelect(int nId)
{
    int nIndex = GetIndexById(nId);

    if (s_stEditStatus.bSelectAll)
    {
        return (-1 == nIndex);
    }

    return (-1 != nIndex);
}

int CSettingDelegate::GetIndexById(int nId)
{
    int nSize = s_stEditStatus.vecSelectId.size();

    for (int i = 0; i < nSize; ++i)
    {
        if (nId == s_stEditStatus.vecSelectId[i])
        {
            return i;
        }
    }

    return -1;
}

void CSettingDelegate::SetTotalCount(int nTotalCount)
{
    s_nTotalCount = nTotalCount;
}

int CSettingDelegate::GetTotalCount() const
{
    return s_nTotalCount;
}

void CSettingDelegate::ResetSelectCount()
{
    s_stEditStatus.bSelectAll = false;
    s_stEditStatus.vecSelectId.clear();
}

int CSettingDelegate::GetSelectCount() const
{
    if (s_stEditStatus.bSelectAll)
    {
        return (s_nTotalCount - s_stEditStatus.vecSelectId.size());
    }

    return s_stEditStatus.vecSelectId.size();
}

bool CSettingDelegate::GetSelectContactId(VEC_CONTACT_ID & vecId)
{
    CDirectoryControlerPtr pControler = GetControler();
    CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL == pCommonDirCtrl)
    {
        return false;
    }

    VEC_CONTACT_ID vecAllContactId;

    pCommonDirCtrl->GetGroupContactId(vecAllContactId);

    int nSize = vecAllContactId.size();

    if (0 == nSize)
    {
        return true;
    }

    if (IsSelectAll())
    {
        vecId = vecAllContactId;
    }
    else
    {
        for (int i = 0; i < nSize; ++i)
        {
            int nId = vecAllContactId[i];

            if (IsSelect(nId))
            {
                vecId.push_back(nId);
            }
        }
    }

    return true;
}

#endif
