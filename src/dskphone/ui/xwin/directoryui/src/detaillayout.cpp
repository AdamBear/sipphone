#include "detaillayout.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "imeedit/ximeedit.h"


CDetailLayout::CDetailLayout(int nType)
    : m_nType(nType)
{
}

CDetailLayout::~CDetailLayout()
{
    ReleaseAllItem();
}

void CDetailLayout::UnInit()
{
    ReleaseAllItem();
}

void CDetailLayout::ReleaseAllItem()
{
    VEC_DETAIL_LIST_ITEM::iterator iter = m_vecListItem.begin();

    for (; iter != m_vecListItem.end(); ++iter)
    {
        CDetailListItemBasePtr pListItem = *iter;

        if (NULL != pListItem)
        {
            pListItem->Release();
        }
    }

    m_vecListItem.clear();
}

bool CDetailLayout::IsLayoutItem(xListViewItemPtr pListItem)
{
    if (NULL == pListItem)
    {
        return false;
    }

    VEC_DETAIL_LIST_ITEM::iterator iter = m_vecListItem.begin();

    for (; iter != m_vecListItem.end(); ++iter)
    {
        CDetailListItemBasePtr pDetailItem = *iter;

        if (pDetailItem == pListItem)
        {
            return true;
        }
    }

    return false;
}

int CDetailLayout::GetType() const
{
    return m_nType;
}

int CDetailLayout::GetItemCount() const
{
    return m_vecListItem.size();
}

CDetailListItemBasePtr CDetailLayout::GetDetailListItem(int nIndex)
{
    int nSize = m_vecListItem.size();

    if (nIndex < 0 || nIndex >= nSize)
    {
        return NULL;
    }

    return m_vecListItem[nIndex];
}

void CDetailLayout::OnDetailIMEChanged(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                       int nDataBytes, LPVOID pData)
{
    xEditTextView * pEdit = (xEditTextView *)wParam;
    yl::string strIMEName;

    if (NULL != pEdit)
    {
        strIMEName = pEdit->GetImeName();
    }

    if (!strIMEName.empty())
    {
        configParser_SetConfigString(kszContactDefaultIME, strIMEName.c_str(), CONFIG_LEVEL_PHONE);
    }
}
