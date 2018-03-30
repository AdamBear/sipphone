#if IF_FEATURE_GENBAND_E911
#include "e911locationdelegate.h"
#include "settingui/src/settingitemfactory.h"
#include "qtcommon/qmisc.h"
#include "dsklog/log.h"
#include "uikernel/languagehelper.h"
#include "include/translateiddefine.h"
#include "commonapi/stringhelperapi.h"
#include "baseui/t4xpicpath.h"

#include "setting/include/modsetting.h"
#include "e911address/include/e911address_def.h"
#include "e911address/include/mode911address.h"

namespace
{
#ifdef _T48
#define MAX_ITEM_CNT    6
#define ITEM_HIGHT      56
#else
#define MAX_ITEM_CNT    5
#define ITEM_HIGHT      38
#endif

enum
{
    SCROLL_VIEW_TYPE_NONE = 0,
    SCROLL_VIEW_TYPE_PAGE = 1,      // 数据按页滚动
    SCROLL_VIEW_TYPE_ITEM = 2,      // 数据按项滚动
};

}

CE911LocationDelegate::CE911LocationDelegate()
    : SubpageListDeleagate()
    , m_pE911LocationFramelist(NULL)
    , m_pItemPat(NULL)
{
}

#ifndef _T48
CE911LocationDelegate::CE911LocationDelegate(CFrameList* pList)
    : SubpageListDeleagate(pList)
    , m_pE911LocationFramelist(NULL)
    , m_pItemPat(NULL)
{
    BindFramelist(pList);
}
#endif

CE911LocationDelegate::~CE911LocationDelegate()
{
    UnBindFramelist();
    m_pItemPat = NULL;
}


void CE911LocationDelegate::BindFramelist(CFrameList* pList)
{
#ifndef _T48
    SaveListStatus();
#endif
    if (NULL != pList)
    {
        m_pE911LocationFramelist = pList;
        m_pE911LocationFramelist->SetDelegate(this);
        m_pE911LocationFramelist->SetScrollViewType(SCROLL_VIEW_TYPE_ITEM | SCROLL_VIEW_TYPE_PAGE);
    }
#ifdef _T48
    SubpageListDeleagate::BindFramelist(pList);
#endif
}

void CE911LocationDelegate::UnBindFramelist()
{
    m_pE911LocationFramelist = NULL;
}

void CE911LocationDelegate::PreCreateItem()
{
    int iPageItemCount = (NULL == m_pE911LocationFramelist) ? GetMaxPagePageItemCount() : m_pE911LocationFramelist->GetMaxPageItemCount();

    QWidgetPair qPair;

    qPair.first = "";
    qPair.second = NULL;
    SettingItemList& m_vecNorItem = GetSetItemList();
    m_vecNorItem.clear();

    for (int i = 0; i < iPageItemCount; ++i)
    {
        qPair.bAddIndex = false;
        qPair.bAddMark = false;
        qPair.bChangeStyleOnPress = true;
        qPair.bChangeStyleOnSelect = true;
        qPair.iIndex = i;
        //创建Item

        CSettingItem* pItem = g_SettingItemManger->CreateItemByType(SETTING_ITEM_GENBAND_E911, m_pItemPat, qPair);

        if (NULL != pItem)
        {
            pItem->setObjectName(QString("CSettingItem%1").arg(i));
            pItem->hide();
            pItem->setFixedHeight(ITEM_HIGHT);
            m_vecNorItem.push_back(pItem);
            ((CListItemPtr)pItem)->SetAction(this);
        }

    }
}

void CE911LocationDelegate::HideItem()
{
    SettingItemList& m_vecNorItem = GetSetItemList();
    for (int i = 0; i < m_vecNorItem.size(); i++)
    {
        if (NULL != m_vecNorItem[i])
        {
            if (NULL != m_vecNorItem[i]->ContentWgt())
            {
                m_vecNorItem[i]->ContentWgt()->setParent(m_pItemPat);
                m_vecNorItem[i]->ContentWgt()->hide();
            }
            m_vecNorItem[i]->hide();
        }
    }
}

void CE911LocationDelegate::OnLoadData(CFrameListBase* pFrameList, int nDataIndex, int nDataCount)
{
    HideItem();

    SETTINGUI_INFO("CE911LocationDelegate::OnLoadData, nDataIndex = %d, nDataCount = %d", nDataIndex, nDataCount);
    if (nDataIndex < 0 || nDataCount < 1)
    {
        return;
    }

    E911AddressListData e911ListData;
    e911ListData.m_nIndexFrom = nDataIndex;
    e911ListData.m_nCountGet = nDataCount;
    if (!e911_GetAddressListData(&e911ListData, m_nPageID))
    {
        SETTINGUI_INFO("CE911LocationDelegate Get List Fail");
        return;
    }

    int nTotalItemCount = e911ListData.m_nTotalCount;

    SETTINGUI_INFO("CE911LocationDelegate::OnLoadData, nTotalItemCount = %d", nTotalItemCount);
    if (NULL == m_pE911LocationFramelist
            || (nTotalItemCount < nDataIndex
                && nTotalItemCount <= 0))
    {
        return;
    }

    int iCurrentIndex = GetCurrentIndex();
    SettingItemList& vecItemList = GetSetItemList();

    //先清空当前数据
    m_pE911LocationFramelist->DetachAllItem(true);
    //重新设置父类
    for (SettingItemList::iterator iter = vecItemList.begin(); iter != vecItemList.end(); ++iter)
    {
        (*iter)->setParent(m_pE911LocationFramelist);
    }
    m_pE911LocationFramelist->SetTotalDataCount(nTotalItemCount);
    m_pE911LocationFramelist->SetItemAttach();

    YLVector<int>::iterator it = e911ListData.m_vlistID.begin();
    YLVector<int>::iterator it_end = e911ListData.m_vlistID.end();

    int iPageItemCount = m_pE911LocationFramelist->GetMaxPageItemCount();

    for (int i = nDataIndex, j = 0
                                 ; i < (nDataIndex + nDataCount) && i < nTotalItemCount  && j < iPageItemCount && it != it_end
            ; ++i, ++j, ++it)
    {
        if (NULL == vecItemList[j])
        {
            continue;
        }

        E911AddressData *pData = e911_GetAddressDetailByID(*it);
        if (pData == NULL)
        {
            continue;
        }
        ((CListItemPtr)vecItemList[j])->SetDataIndex(i);

        //翻页时纠正选中项
        if (vecItemList[j]->IsFocus() && vecItemList[j]->GetDataIndex() != iCurrentIndex)
        {
            vecItemList[j]->FocusItem(false);
        }

        ((CListItemPtr)vecItemList[j])->SetAction(this);
        (vecItemList[j])->SetLabText(pData->m_strDisplayName.c_str());
        m_pE911LocationFramelist->AddItem(vecItemList[j]);
        vecItemList[j]->show();
        vecItemList[j]->Relayout();
    }

    if (iCurrentIndex >= nDataIndex && iCurrentIndex < nDataIndex + m_pE911LocationFramelist->GetTotalItemCount())
    {
        m_pE911LocationFramelist->SetItemFocusByIndex(m_pE911LocationFramelist->GetIndexByDataIndex(iCurrentIndex));
        m_pE911LocationFramelist->FocusItemByDataIndex(iCurrentIndex);
    }
    else
    {
        m_pE911LocationFramelist->SetItemFocusByIndex(0);
    }
    m_pE911LocationFramelist->setFocus();
}

int CE911LocationDelegate::GetMaxPagePageItemCount()
{
    return MAX_ITEM_CNT;
}

int CE911LocationDelegate::GetFocusItemID()
{
    int iCurrentIndex = GetFocusItemDataIndex();
    if (iCurrentIndex < 0)
    {
        return -1;
    }
    return e911_GetIDByIndex(m_nPageID, iCurrentIndex);
}

bool CE911LocationDelegate::FocusItemHasParentItem()
{
    return m_nPageID > 0;
}

bool CE911LocationDelegate::FouceItemHasChildItem()
{
    int nID = GetFocusItemID();
    E911AddressData* pData = e911_GetAddressDetailByID(nID);
    if (pData != NULL)
    {
        return pData->m_vChildrenID.size() > 0;
    }

    return false;
}

void CE911LocationDelegate::SetPageID(int nPageID)
{
    m_nPageID = nPageID;
}

int CE911LocationDelegate::GetPageID()
{
    return m_nPageID;
}

yl::string CE911LocationDelegate::GetCurrentPageTitle()
{
    E911AddressData* pData = e911_GetAddressDetailByID(m_nPageID);
    if (pData != NULL)
    {
        return pData->m_strDisplayName;
    }
    return "";
}

void CE911LocationDelegate::SetScrollViewType(int nType)
{
    if (NULL != m_pE911LocationFramelist)
    {
        m_pE911LocationFramelist->SetScrollViewType(nType);
    }
}

#endif
