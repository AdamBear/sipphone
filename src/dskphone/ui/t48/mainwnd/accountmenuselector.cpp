#include "baseui/framelist.h"

#include "accountmenuselector.h"
#include "popupmenulistitem.h"
#include "popupmenulisttitle.h"
#include "baseui/t4xpicpath.h"
// #include "IdleScreen/IdleScreenController.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "qtcommon/qmisc.h"
#include "talk/talklogic/include/modtalklogic.h"


CAccountMenuSelector::CAccountMenuSelector(void)
    : m_nSelIndex(-1)
{
}

CAccountMenuSelector::~CAccountMenuSelector(void)
{
}

void CAccountMenuSelector::Init()
{
    m_listAccount.clear();

    m_listAccount = acc_GetAvailableAccountList();

    //http://10.2.1.199/Bug.php?BugID=93528 MCA添加限制条件 如果不能再起一路呼出的账号就不显示
    ListAccounts::iterator iter = m_listAccount.begin();

    for (; iter != m_listAccount.end();)
    {
        if (!talklogic_IsAllowNewCallByAccount(*iter))
        {
            iter = m_listAccount.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

void CAccountMenuSelector::SetSelectIndexByAccountID(int iAccountID)
{
    ListAccounts::iterator iter = m_listAccount.begin();

    for (int i = 0; iter != m_listAccount.end(); ++iter, ++i)
    {
        if (*iter == iAccountID)
        {
            m_nSelIndex = i;
            break;
        }
    }
}

int CAccountMenuSelector::GetAccountId()
{
    ListAccounts::iterator iter = m_listAccount.begin();

    for (int i = 0; iter != m_listAccount.end(); ++iter, ++i)
    {
        if (i == m_nSelIndex)
        {
            return *iter;
            break;
        }
    }

    return -1;
}

int CAccountMenuSelector::GetAccountSize()
{
    return m_listAccount.size();
}

IFrameListDelegatePtr CAccountMenuSelector::GetDelegate() const
{
    return (IFrameListDelegatePtr)this;
}

bool CAccountMenuSelector::OnLastItemNext()
{
    return false;
}

bool CAccountMenuSelector::OnFirstItemPrevious()
{
    return false;
}

void CAccountMenuSelector::OnCleanContent(CFrameListBase * pFrameList)
{

}

void CAccountMenuSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    if (!m_pFrameList->IsCanReuseItem(CPopupMenuListItem::GetPopupMenuListItemType()))
    {
        return;
    }

    int nItemIndex = 0;
    int nSize = m_listAccount.size();
    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    ListAccounts::iterator iter = m_listAccount.begin();

    m_pFrameList->SetTotalDataCount(nSize);

    for (; nItemIndex < nMaxPageTotalCount && iter != m_listAccount.end(); ++iter, ++nDataIndex)
    {
        bool bAdd = false;
        CPopupMenuListItemPtr pMenuItem = (CPopupMenuListItemPtr)m_pFrameList->GetItemByIndex(nItemIndex);

        if (NULL == pMenuItem)
        {
            bAdd = true;
            pMenuItem = new CPopupMenuListItem;

            if (NULL == pMenuItem)
            {
                continue;
            }
        }

        pMenuItem->ClearContent();
        pMenuItem->SetAction(this);
        pMenuItem->SetDataIndex(nDataIndex);
        pMenuItem->SetMenuIcon(PIC_DSS_LINE_REGISTERED, PIC_DSS_LINE_REGISTERED);
        //pMenuItem->SetMenuText(toQString(acc_GetLabel(iter->dataEntry).c_str()));
        pMenuItem->setObjectName(QString("MenuItem%1").arg(nItemIndex));

#if IF_BUG_25367
        yl::string strAccountText = acc_GetAccountShowText(*iter, true).c_str();
        commonUnits_FormatNumber(strAccountText);
        QString strAccount = toQString(strAccountText.c_str());
#elif IF_FEATURE_32591
        QString strAccount = toQString(acc_GetStatusBarLabel(*iter));
        if (strAccount.isEmpty())
        {
            strAccount = toQString(acc_GetLabel(*iter).c_str());
            if (strAccount.isEmpty())
            {
                strAccount = toQString(acc_GetAccountShowText(*iter).c_str());
            }
        }
#else
        QString strAccount = toQString(acc_GetLabel(*iter).c_str());

        if (strAccount.isEmpty())
        {
            strAccount = toQString(acc_GetAccountShowText(*iter).c_str());
        }
#endif
        pMenuItem->SetMenuText(strAccount);

        if (m_nSelIndex == nDataIndex)
        {
            pMenuItem->SelectItem(true);
        }
        else
        {
            pMenuItem->SelectItem(false);
        }

        if (bAdd && !m_pFrameList->AddItem(pMenuItem))
        {
            pMenuItem->Release();
        }

        ++nItemIndex;
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
}

bool CAccountMenuSelector::IsFrameListAttach(CFrameListBase * pFrameList)
{
    return IsAttach(pFrameList);
}

void CAccountMenuSelector::OnFrameListDetach(CFrameListBase * pFrameList)
{
    Detach(pFrameList);
}

void CAccountMenuSelector::OnFrameListAttach(CFrameListBase * pFrameList)
{
    if (NULL != m_pFrameList)
    {
        SetFrameList(NULL);
    }

    Attach(pFrameList);

    if (NULL == m_pFrameList)
    {
        return;
    }

    int nCurAccountId = acc_GetDefaultAccount();
#if IF_BUG_25367
    yl::string strAccountText = acc_GetAccountShowText(nCurAccountId, true).c_str();
    commonUnits_FormatNumber(strAccountText);
    QString strAccount = toQString(strAccountText.c_str());
#elif IF_FEATURE_32591
    QString strAccount = toQString(acc_GetStatusBarLabel(nCurAccountId));
    if (strAccount.isEmpty())
    {
        strAccount = toQString(acc_GetLabel(nCurAccountId));
        if (strAccount.isEmpty())
        {
            strAccount = toQString(acc_GetAccountShowText(nCurAccountId));
        }
    }
#else
    QString strAccount = toQString(acc_GetLabel(nCurAccountId).c_str());

    if (strAccount.isEmpty())
    {
        strAccount = toQString(acc_GetAccountShowText(nCurAccountId).c_str());
    }
#endif
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();
    CPopupMenuListTitlePtr pPopupTitle = NULL;

    if (NULL != pTitle && CPopupMenuListTitle::IsPopupMenuTitle(pTitle))
    {
        pPopupTitle = (CPopupMenuListTitlePtr)pTitle;

        pPopupTitle->SetTitleText(strAccount);
    }
    else
    {
        pPopupTitle = new CPopupMenuListTitle;

        if (NULL != pPopupTitle)
        {
            pPopupTitle->SetTitleText(strAccount);
        }

        m_pFrameList->SetFrameListTitle(pPopupTitle);
    }
}

int CAccountMenuSelector::GetTotalMenuCount()
{
    return m_listAccount.size();
}

void CAccountMenuSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
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
