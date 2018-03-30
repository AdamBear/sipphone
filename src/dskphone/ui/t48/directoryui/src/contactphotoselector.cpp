#include "baseui/framelist.h"
#include "contactphotoselector.h"

#include "contactphotolistitem.h"
#include "moddirectoryui.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include <resource/modresource.h>


typedef YLSmartPtr<BaseResItem> CPhotoDataPtr;
typedef YLList<CPhotoDataPtr> LIST_PHOTO;

CContactPhotoSelector::CContactPhotoSelector(void)
    : m_strCurPhoto("")
    , m_nSelIndex(-1)
{
    m_bItemFocus = false;
    m_bDefaultFocus = false;
}

CContactPhotoSelector::~CContactPhotoSelector(void)
{
    m_vecPhotoName.clear();
    m_vecPhotoPath.clear();
}

void CContactPhotoSelector::Init(const yl::string & strPhoto)
{
    m_nSelIndex = -1;
    m_vecPhotoName.clear();
    m_vecPhotoPath.clear();
    m_strCurPhoto = strPhoto;

    yl::string strResType;
    LIST_PHOTO lsPhoto;

    if (!Res_GetResType(DIR_RES_TYPE_PHOTO, strResType)
            || 0 != res_getResItemsByResType(lsPhoto, strResType.c_str(), true))
    {
        return;
    }

    LIST_PHOTO::iterator iter = lsPhoto.begin();

    for (int i = 0; iter != lsPhoto.end(); iter++, ++i)
    {
        CPhotoDataPtr & pItem = *iter;

        if (pItem.IsEmpty())
        {
            continue;
        }

        m_vecPhotoName.push_back(pItem->resItemDispLabel);
        m_vecPhotoPath.push_back(pItem->resPath);

        if (pItem->resPath == m_strCurPhoto)
        {
            m_nSelIndex = i;
        }
    }

    if (-1 == m_nSelIndex && m_vecPhotoPath.size() > 0)
    {
        m_strCurPhoto = m_vecPhotoPath[0];
        m_nSelIndex = 0;
    }
}

int CContactPhotoSelector::GetRowCount() const
{
    int nRowCount = m_vecPhotoName.size();
    int nPhotoCountPerRow = CContactPhotoListItem::GetPhotoCountPerRow();

    if (nPhotoCountPerRow > 0)
    {
        nRowCount = (nRowCount + nPhotoCountPerRow - 1) / nPhotoCountPerRow;
    }

    return nRowCount;
}

yl::string CContactPhotoSelector::GetPhoto()
{
    return m_strCurPhoto;
}

void CContactPhotoSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    if (!m_pFrameList->IsCanReuseItem(CContactPhotoListItem::GetContactPhotoItemType()))
    {
        return;
    }

    int nNameSize = m_vecPhotoName.size();
    int nPathSize = m_vecPhotoPath.size();
    int nRealSize = ((nNameSize < nPathSize) ? nNameSize : nPathSize);
    int nPhotoCountPerRow = CContactPhotoListItem::GetPhotoCountPerRow();
    int nRowCount = nRealSize;

    if (nPhotoCountPerRow <= 0)
    {
        nPhotoCountPerRow = 1;
    }
    else
    {
        nRowCount = (nRowCount + nPhotoCountPerRow - 1) / nPhotoCountPerRow;
    }

    int nItemIndex = 0;
    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();

    m_pFrameList->SetTotalDataCount(nRowCount);

    int nBeginIndex = nDataIndex * nPhotoCountPerRow;
    int nEndIndex = (nDataIndex + nDataCount) * nPhotoCountPerRow;

    if (nEndIndex > nRealSize)
    {
        nEndIndex = nRealSize;
    }

    int i = 0;
    CContactPhotoListItemPtr pPhotoItem = NULL;

    for (; nBeginIndex < nEndIndex && nItemIndex < nMaxPageTotalCount; ++nBeginIndex, ++i)
    {
        if (i >= nPhotoCountPerRow)
        {
            i = 0;
            ++nItemIndex;
        }

        if (0 == i)
        {
            bool bAdd = false;

            pPhotoItem = (CContactPhotoListItemPtr)m_pFrameList->GetItemByIndex(nItemIndex);

            if (NULL == pPhotoItem)
            {
                bAdd = true;
                pPhotoItem = new CContactPhotoListItem;

                if (NULL == pPhotoItem)
                {
                    continue;
                }
            }

            pPhotoItem->ClearContent();
            pPhotoItem->ClearPhoto();
            pPhotoItem->SetAction(this);
            pPhotoItem->SetDataIndex(nDataIndex + nItemIndex);

            if (bAdd && !m_pFrameList->AddItem(pPhotoItem))
            {
                pPhotoItem->Release();
                pPhotoItem = NULL;
            }
        }

        if (NULL == pPhotoItem)
        {
            continue;
        }

        yl::string & strPhotoPath = m_vecPhotoPath[nBeginIndex];
        QString strPhotoName = LANG_TRANSLATE(DM_GetContactImageName(m_vecPhotoName[nBeginIndex]).c_str());

        pPhotoItem->SetPhoto(i, strPhotoPath, strPhotoName);

        if (m_nSelIndex == nBeginIndex)
        {
            pPhotoItem->SetSelPhotoIndex(i);
        }
    }

    ++nItemIndex;

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
}

void CContactPhotoSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem,
        int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem
            || (CContactPhotoListItem::GetContactPhotoItemType() != pItem->GetType()))
    {
        return;
    }

    if (nAction >= LIST_ITEM_ACTION_CHOOSE_IMAGE_BEGIN && nAction <= LIST_ITEM_ACTION_CHOOSE_IMAGE_END)
    {
        OnChooseImage(pList, (CContactPhotoListItemPtr)pItem,
                      (nAction - LIST_ITEM_ACTION_CHOOSE_IMAGE_BEGIN));
    }
    else if (nAction >= LIST_ITEM_ACTION_SELECT_IMAGE_BEGIN
             && nAction <= LIST_ITEM_ACTION_SELECT_IMAGE_END)
    {
        OnSelectImage(pList, (CContactPhotoListItemPtr)pItem,
                      (nAction - LIST_ITEM_ACTION_SELECT_IMAGE_BEGIN));
    }
    else if (LIST_ITEM_ACTION_UNSELECT_IMAGE == nAction)
    {
        OnUnSelectImage(pList, (CContactPhotoListItemPtr)pItem);
    }
}

void CContactPhotoSelector::OnChooseImage(CFrameList * pFrameList, CContactPhotoListItem * pItem,
        int nIndex)
{
    if (NULL == pFrameList || NULL == pItem)
    {
        return;
    }

    int nPhotoIndex = pItem->GetDataIndex() * CContactPhotoListItem::GetPhotoCountPerRow();

    nPhotoIndex += nIndex;

    if (nPhotoIndex >= 0 && nPhotoIndex < m_vecPhotoPath.size())
    {
        m_strCurPhoto = m_vecPhotoPath[nPhotoIndex];
    }

    CMessageBoxBase * pTopMsgBox = MessageBox_GetTopMessageBox();

    if (NULL != pTopMsgBox && pTopMsgBox == pFrameList->parent())
    {
        MessageBox_RemoveTopMessageBox(MessageBox_OK);
    }
}

void CContactPhotoSelector::OnSelectImage(CFrameList * pFrameList, CContactPhotoListItem * pItem,
        int nIndex)
{
    if (NULL == pFrameList || NULL == pItem)
    {
        return;
    }

    int nItemCount = pFrameList->GetTotalItemCount();

    for (int i = 0; i < nItemCount; ++i)
    {
        CContactPhotoListItemPtr pPhotoItem = (CContactPhotoListItemPtr)pFrameList->GetItemByIndex(i);

        if (NULL != pPhotoItem)
        {
            pPhotoItem->SetSelPhotoIndex(-1);
        }
    }

    pItem->SetSelPhotoIndex(nIndex);
    pFrameList->update();
}

void CContactPhotoSelector::OnUnSelectImage(CFrameList * pFrameList, CContactPhotoListItem * pItem)
{
    if (NULL == pFrameList || NULL == pItem)
    {
        return;
    }

    int nDataIndex = 0;
    int nPhotoIndex = -1;
    int nPhotoCount = CContactPhotoListItem::GetPhotoCountPerRow();

    if (m_nSelIndex >= 0 && nPhotoCount > 0)
    {
        nDataIndex = m_nSelIndex / nPhotoCount;
        nPhotoIndex = m_nSelIndex % nPhotoCount;
    }

    int nItemCount = pFrameList->GetTotalItemCount();

    for (int i = 0; i < nItemCount; ++i)
    {
        CContactPhotoListItemPtr pPhotoItem = (CContactPhotoListItemPtr)pFrameList->GetItemByIndex(i);

        if (NULL == pPhotoItem)
        {
            continue;
        }

        if (pPhotoItem->GetDataIndex() == nDataIndex)
        {
            pPhotoItem->SetSelPhotoIndex(nPhotoIndex);
        }
        else
        {
            pPhotoItem->SetSelPhotoIndex(-1);
        }
    }

    pFrameList->update();
}
