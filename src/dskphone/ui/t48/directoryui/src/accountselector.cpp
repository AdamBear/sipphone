#include "baseui/framelist.h"
#include "accountselector.h"

#include "diruihelper.h"
#include "baseui/detaillistitem.h"
#include "commonunits/modcommonunits.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/selectmessagebox.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "account/include/modaccount.h"


namespace
{
#define NUMBER_SELECT_POP_BOX_ITEM_HEIGHT 47
}


CAccountSelector::CAccountSelector()
    : m_nSelIndex(-1)
{
    m_bItemFocus = false;
    m_bDefaultFocus = false;
}

CAccountSelector::~CAccountSelector(void)
{
}

int CAccountSelector::GetDialAccountId() const
{
    if (m_nSelIndex >= 0 && m_nSelIndex < m_vecAccountNode.size())
    {
        return m_vecAccountNode[m_nSelIndex].nAccountId;
    }
    else
    {
        return -1;
    }
}

void CAccountSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    if (!m_pFrameList->IsCanReuseItem(CDetailListItem::GetDetailListItemType()))
    {
        return;
    }

    m_vecAccountNode.clear();
    int iAccountNum = acc_AccountNum();
    for (int i = 0; i < iAccountNum; i++)
    {
        if (!acc_IsAccountAvailable(i))
        {
            continue;
        }

        ST_ACCOUNT_NODE stNote;
        // 获取账号的状态
        stNote.nAccountId = i;
        stNote.strName = commonAPI_FormatString("%d", i + 1) + "." + acc_GetUsername(i);
        stNote.strValue = TRID_REGISTERED;

        if (stNote.nAccountId == acc_GetDefaultAccount())
        {
            stNote.strValue = TRID_DEFAULT_ACCOUNT;
        }
        m_vecAccountNode.push_back(stNote);
    }

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    int nSize = m_vecAccountNode.size();
    yl::string strValue;
    yl::string strName;
    int nItemIndex = 0;

    m_pFrameList->SetTotalDataCount(nSize);

    for (; nItemIndex < nMaxPageTotalCount && nDataIndex < nSize; ++nItemIndex, ++nDataIndex)
    {
        strName = m_vecAccountNode[nDataIndex].strName;
        strValue = m_vecAccountNode[nDataIndex].strValue;

        bool bAdd = false;
        CDetailListItemPtr pDetailItem = (CDetailListItemPtr)m_pFrameList->GetItemByIndex(nItemIndex);

        if (NULL == pDetailItem)
        {
            bAdd = true;
            pDetailItem = new CDetailListItem;

            if (NULL == pDetailItem)
            {
                continue;
            }

            pDetailItem->SetItemHeight(NUMBER_SELECT_POP_BOX_ITEM_HEIGHT);
        }

        pDetailItem->ClearContent();
        pDetailItem->SetAction(this);
        pDetailItem->SetDataIndex(nDataIndex);
        pDetailItem->SetTitle(LANG_TRANSLATE(strName.c_str()));
        pDetailItem->SetValue(toQString(strValue, true));

        if (bAdd && !m_pFrameList->AddItem(pDetailItem))
        {
            pDetailItem->Release();
        }
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
}

void CAccountSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    m_nSelIndex = pItem->GetDataIndex();


    CMessageBoxBase * pTopMsgBox = MessageBox_GetTopMessageBox();

    if (NULL != pTopMsgBox && pTopMsgBox == pFrameList->parent())
    {
        MessageBox_RemoveTopMessageBox(MessageBox_OK);
    }
}
