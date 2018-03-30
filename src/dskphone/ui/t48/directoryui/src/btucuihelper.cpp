#include <QtGui>
#include "uicommon.h"
#include "baseui/framelist.h"
#include "btucuihelper.h"
#include "btuccontactcontroler.h"
#include "btuclistitem.h"
#include "contactdetailtitle.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "directorymgr.h"
#include "baseui/detaillistitem.h"
#include "baseui/framelist.h"
#include "baseui/menulistitem.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"


CBTUCUIHelper::CBTUCUIHelper()
    : CDirUIHelper(DIRECTORY_TYPE_BTUC)
{
}

CBTUCUIHelper::~CBTUCUIHelper()
{
}

CBTUCUIHelper * CBTUCUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_BTUC != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CBTUCUIHelper *)pUIHelper.Get();
}

void CBTUCUIHelper::OnAttachFrameList()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    int nState = pDelegate->GetState();

    if (DIR_UI_STATE_DETAIL == nState)
    {
        InitDetailTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_DETAIL_ITEM_COUNT);
        return;
    }

    if (DIR_UI_STATE_SEARCH == nState)
    {
        m_pFrameList->SetFrameListTitle(NULL);
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }

    CDirUIHelper::OnAttachFrameList();
}

void CBTUCUIHelper::InitDetailTitle()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    bool bAdd = true;
    CContactDetailTitlePtr pDetailTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactDetailTitle::IsContactDetailTitle(pTitle))
    {
        bAdd = false;
        pDetailTitle = (CContactDetailTitlePtr)pTitle;
    }
    else
    {
        pDetailTitle = new CContactDetailTitle;
        if (NULL == pDetailTitle)
        {
            return;
        }
    }

    pDetailTitle->SetBtnCount(1);
    pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
    pDetailTitle->SetAction(pDelegate.Get());

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pDetailTitle);
    }
    else
    {
        pDetailTitle->RefreshTitle();
    }
}

#if FEATURE_BROADTOUCH_UC
bool CBTUCUIHelper::FillGroupList(GroupList & listGroup, int nDataIndex, int nDataCount)
{
    return false;
}

bool CBTUCUIHelper::FillContactList(ContactBuddyListData & listContact)
{
    DIRUI_INFO("CBTUCUIHelper::FillContactList(TotalCount:%d GetCount:%d Index:%d)",
               listContact.m_nTotalCount, listContact.m_nCountToGet, listContact.m_nIndexFrom);

    if (NULL == m_pFrameList)
    {
        DIRUI_ERR("FillContactList when m_pFrameList is null");
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBTUCContactControler * pBTUCContact = CBTUCContactControler::GetControler(pControler);

    if (NULL == pBTUCContact)
    {
        DIRUI_ERR("CBTUCUIHelper::FillContactList fail when pBTUCContact is null");
        return false;
    }

    int nUIState = pBTUCContact->GetState();

    RefreshListItemAmount(nUIState, listContact.m_nIndexFrom, listContact.m_nTotalCount);

    int nSize = listContact.m_list.size();

    if (nSize <= 0)
    {
        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

        if (!pDelegate.IsEmpty())
        {
            pDelegate->SetPageDataIndex(0);
        }

        m_pFrameList->ClearContent(true);
        return true;
    }

    IterPresenceInfo iter = listContact.m_list.begin();
    PresenceInfo info;
    int nItemIndex = 0;

    for (; iter != listContact.m_list.end(); iter++)
    {
        int & nId = *iter;

        if (!pBTUCContact->GetDetail(nId, info))
        {
            continue;
        }

        if (RefreshContactItem(nItemIndex, listContact.m_nIndexFrom + nItemIndex, listContact.m_nTotalCount,
                               info))
        {
            ++nItemIndex;
        }
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();

    return true;
}

bool CBTUCUIHelper::RefreshContactItem(int nItemIndex, int nDataIndex, int nDataCount,
                                       PresenceInfo & info)
{
    DIRUI_INFO("RefreshContactItem(ItemIndex:%d DataIndex:%d ContactId:%d Name:%s Info:%s)",
               nItemIndex, nDataIndex, info.m_nLocalId, info.strContactName.c_str(),
               info.strAvatarSignature.c_str());

    if (NULL == m_pFrameList)
    {
        return false;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CDirectoryListDelegate * pListDelegate = pDelegate.Get();

    if (NULL == pListDelegate || pListDelegate != m_pFrameList->GetFrameDelegate())
    {
        return false;
    }

    if (0 == nItemIndex)
    {
        if (!m_pFrameList->IsCanReuseItem(CBTUCListItem::GetBTUCItemType()))
        {
            m_pFrameList->ClearContent();
        }

        m_pFrameList->SetTotalDataCount(nDataCount);
        pListDelegate->SetPageDataIndex(nDataIndex);
    }

    bool bAdd = false;
    CBTUCListItemPtr pItem = (CBTUCListItemPtr)m_pFrameList->GetItemByIndex(nItemIndex);

    if (NULL == pItem)
    {
        pItem = new CBTUCListItem;

        if (NULL == pItem)
        {
            return false;
        }
        pItem->setObjectName(QString("Item%1").arg(nItemIndex));
        bAdd = true;
    }

    pItem->ClearContent();

    pItem->SetAction(pListDelegate);
    pItem->SetDataIndex(nDataIndex);
    pItem->SetTitle(toQString(info.strContactName, true));
    pItem->SetSubTitle(toQString(info.m_strTitle, true));
    pItem->setShowSubTitle(true);
    //高亮
    CDirectoryControlerPtr pControler = pListDelegate->GetControler();
    if (!pControler.IsEmpty())
    {
        yl::string strSearch = pControler->GetSearchKey();
        pItem->SetHighLightText(toQString(strSearch));
    }
    pItem->setShowInfo(false);
    pItem->SetPresence(info.ePresenceType);
    pItem->SetPhoto(info.strAvatarInfo);
    pItem->SetId(info.m_nLocalId);
    pItem->SetEdit(false);


    bool bReadOnly = true;

    if (!pControler.IsEmpty())
    {
        bReadOnly = pControler->IsReadOnly();
    }

    pItem->SetDetail(!bReadOnly);

    if (bAdd && !m_pFrameList->AddItem(pItem))
    {
        pItem->Release();
        return false;
    }

    return true;
}

bool CBTUCUIHelper::FillContact(PresenceInfo & info, int nDataIndex, int nDataCount)
{
    int nTotalCount = 2 + info.m_listAttribute.size();
    int nAttrIndex = nDataIndex;
    int nItemIndex = 0;

    if (nDataIndex <= 0)
    {
        // 第一页默认第1项为User ID，第2项为Name
        QString strTitleUserID = LANG_TRANSLATE(TRID_USER_ID);
        QString strUserID = toQString(info.strContactID);

        RefreshDetailItem(0, 0, nTotalCount, strTitleUserID, strUserID);

        QString strTitleName = LANG_TRANSLATE(TRID_NAME);
        QString strName = toQString(info.strContactName, true);

        RefreshDetailItem(1, 1, nTotalCount, strTitleName, strName);

        nItemIndex = 2;
    }
    else
    {
        nAttrIndex -= 2;
    }

    YLList<Attribute>::iterator iter = info.m_listAttribute.begin();
    int i = 0;

    for (; iter != info.m_listAttribute.end() && nItemIndex < nDataCount; ++i, iter++)
    {
        if (i < nAttrIndex)
        {
            continue;
        }

        Attribute & attr = *iter;
        QString strTitle = LANG_TRANSLATE(attr.m_strName.c_str());
        QString strValue = toQString(attr.m_strValue, true);

        RefreshDetailItem(nItemIndex, (nDataIndex + nItemIndex), nTotalCount, strTitle, strValue);

        ++nItemIndex;
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();

    return true;
}
#endif
