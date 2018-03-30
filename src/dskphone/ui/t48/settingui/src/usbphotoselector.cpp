#include "usbphotoselector.h"
#include "baseui/framelist.h"
#include "settingui/src/settingitembase.h"
#include "setting/include/common.h"
#include "uicommon/messagebox/selectmessagebox.h"
#include "uikernel/languagehelper.h"
#include "presence/include/presence_inc.h"

#include "uicommon/qtcommon/qmisc.h"


CUSBPhotoSelector::CUSBPhotoSelector()
    : m_eSelIndex(USB_PIC_SET_TYPE_WALLPAPER)
{
    m_vecDisplayName.push_back(TRID_SET_AS_WALLPAPER);
    m_vecDisplayName.push_back(TRID_SET_AS_SCREENCAPTURE);
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    m_vecDisplayName.push_back(TRID_SET_AS_DSKKEYWALLPAPER);
#endif
}

CUSBPhotoSelector::~CUSBPhotoSelector()
{
    m_vecDisplayName.clear();
    m_eSelIndex = USB_PIC_SET_TYPE_WALLPAPER;
}

void CUSBPhotoSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    m_eSelIndex = USB_PIC_SET_TYPE_WALLPAPER;
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    if (!m_pFrameList->IsCanReuseItem(CSettingRadioItem::GetSettingRadioItemType()))
    {
        m_pFrameList->ClearContent();
    }

    int nItemIndex = 0;
    int nSize = m_vecDisplayName.size();
    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();

    m_pFrameList->SetTotalDataCount(nSize);

    for (int i = 0; nItemIndex < nMaxPageItemCount && i < nSize; ++i)
    {
        if (i < nDataIndex)
        {
            continue;
        }

        //  yl::string& strFavoriteId = m_vecFavorites[i];

        bool bAdd = false;
        CSettingRadioItem * pRadioItem = (CSettingRadioItem *)m_pFrameList->GetItemByIndex(nItemIndex);

        if (NULL == pRadioItem)
        {
            bAdd = true;
            pRadioItem = new CSettingRadioItem;

            if (NULL == pRadioItem)
            {
                continue;
            }

            pRadioItem->setGeometry(0, 0, 0, SELECT_POP_BOX_POS_ITEM_HEIGHT);
            pRadioItem->SetChangeStyleOnPress(true);
            pRadioItem->SetChangeStyleOnSelect(false);
        }

        int nItemDataIndex = i;

        pRadioItem->ClearContent();
        pRadioItem->CListItem::SetAction(this);

        //这边的名称在init 的时候已经做过处理了  用这个
        pRadioItem->SetLabText(LANG_TRANSLATE(toQString(m_vecDisplayName[i])));

        //pRadioItem->SetLabText(LANG_TRANSLATE(GetFavoriteShowName(strFavoriteId).c_str()));
        pRadioItem->SetDataIndex(nItemDataIndex);

        if (nItemDataIndex == m_eSelIndex)
        {
            pRadioItem->SetFocus(true);
        }
        else
        {
            pRadioItem->SetFocus(false);
        }

        if (bAdd && !m_pFrameList->AddItem(pRadioItem))
        {
            pRadioItem->Release();
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

void CUSBPhotoSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    switch (pItem->GetDataIndex())
    {
    case 0:
        {
            m_eSelIndex = USB_PIC_SET_TYPE_WALLPAPER;
        }
        break;
    case 1:
        {
            m_eSelIndex = USB_PIC_SET_TYPE_SCRRENSAVER;
        }
        break;
    case 2:
        {
            m_eSelIndex = USB_PIC_SET_TYPE_DSSKEYWALLPAPER;
        }
        break;
    default:
        break;
    }
}
