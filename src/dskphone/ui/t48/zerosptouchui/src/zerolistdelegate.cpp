#include "zerolistdelegate.h"
#include "keymap.h"
#include "settingui/src/settingitemfactory.h"
#include "uicommon/qtcommon/qmisc.h"
#include "dsklog/log.h"
#include "inputmethod/inputmethod.h"

namespace
{
// Item not delete and add from Framelist
#define SAME_ITEM_ADD_TO_FRAMELIST_ONCE
#define PRE_CREATE

#define ITEM_HIGHT      48
#define MAX_ITEM_CNT    7
}

ZeroListDelegate::ZeroListDelegate()
    : SubpageListDeleagate()
    , m_pZeroFramelist(NULL)
    , m_pItemPat(NULL)
{
    m_bIsEditPage = true;
}

ZeroListDelegate::~ZeroListDelegate()
{
    m_vecWgtPair.clear();
    UnBindFramelist();
    m_pItemPat = NULL;
}

void ZeroListDelegate::PreCreateItem()
{
    m_bIsListReady = false;

#ifdef PRE_CREATE
    int iPageItemCount = (NULL == m_pZeroFramelist) ? MAX_ITEM_CNT :
                         m_pZeroFramelist->GetMaxPageItemCount();

    QWidgetPair qPair;

    qPair.first = "";
    qPair.second = NULL;
    SettingItemList & m_vecNorItem = GetSetItemList();

    for (int i = 0; i < iPageItemCount; ++i)
    {
        qPair.bAddIndex = true;
        qPair.iIndex = i;
        //创建Item
        CSettingItem * pItem = g_SettingItemManger->CreateItemByType(SETTING_ITEM_NORMAL, m_pItemParent,
                               qPair);
        if (NULL != pItem)
        {
            pItem->setObjectName(QString("CSettingItem%1").arg(i));

            pItem->SetWgetHost(m_pItemParent);

            pItem->hide();
            pItem->setFixedHeight(ITEM_HIGHT);
            m_vecNorItem.push_back(pItem);
            ((CListItemPtr)pItem)->SetAction(this);

#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
            pItem->SetChangeStyleOnPress(false);
#endif
            pItem->SetChangeStyleOnSelect(false);
#endif

#ifdef SAME_ITEM_ADD_TO_FRAMELIST_ONCE
            if (NULL != m_pZeroFramelist)
            {
                m_pZeroFramelist->AddItem(pItem);
            }
#endif
        }
    }
#ifdef SAME_ITEM_ADD_TO_FRAMELIST_ONCE
    if (NULL != m_pZeroFramelist)
    {
        m_pZeroFramelist->SetTotalDataCount(iPageItemCount);
    }
#endif
#endif

    m_bIsListReady = true;
}

void ZeroListDelegate::BindFramelist(CFrameList * pList)
{
    if (NULL != pList)
    {
        m_pZeroFramelist = pList;
        m_pZeroFramelist->SetDelegate(this);
    }
    SubpageListDeleagate::BindFramelist(pList);
}

void ZeroListDelegate::UnBindFramelist()
{
    m_pZeroFramelist = NULL;
    SubpageListDeleagate::UnBindFramelist();
}

// set widget pair
void ZeroListDelegate::SetItemsData(QWPVector vecWgtpair)
{
    m_vecWgtPair.clear();
    m_vecWgtPair = vecWgtpair;
#ifdef SAME_ITEM_ADD_TO_FRAMELIST_ONCE
    if (NULL != m_pZeroFramelist)
    {
        m_pZeroFramelist->SetTotalDataCount(m_vecWgtPair.size());
    }
#endif
}

void ZeroListDelegate::HideItem()
{
    SettingItemList & m_vecNorItem = GetSetItemList();
    for (int i = 0; i < m_vecNorItem.size(); i++)
    {
        if (NULL != m_vecNorItem[i])
        {
            if (NULL != m_vecNorItem[i]->ContentWgt())
            {
                m_vecNorItem[i]->ContentWgt()->setParent(m_pItemParent);
                m_vecNorItem[i]->ContentWgt()->hide();
            }
            m_vecNorItem[i]->hide();
        }
    }
}

void ZeroListDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    ZEROUI_INFO("OnLoadData DataIdx[%d], DataCnt[%d], FrameTot[%d]"
                , nDataIndex, nDataCount, m_pZeroFramelist->GetTotalItemCount());

    m_bIsListReady = false;

    HideItem();
    SettingItemList & m_vecNorItem = GetSetItemList();

    if (NULL != m_pZeroFramelist && m_vecWgtPair.size() >= nDataIndex)
    {
#ifndef SAME_ITEM_ADD_TO_FRAMELIST_ONCE
        //先清空当前数据
        m_pZeroFramelist->DetachAllItem(true);
        m_pZeroFramelist->SetTotalDataCount(m_vecWgtPair.size());
#endif
        m_pZeroFramelist->SetItemAttach();
        int iPageItemCount = m_pZeroFramelist->GetMaxPageItemCount();
        //重新添加数据
        int j = 0;
        for (int i = nDataIndex; i < nDataIndex + nDataCount && i < m_vecWgtPair.size()
                && j < iPageItemCount; ++i, ++j)
        {
#ifndef PRE_CREATE
            CSettingItem * pItem = g_SettingItemManger->CreateItemByType(SETTING_ITEM_NORMAL, m_pItemParent,
                                   m_vecWgtPair[i]);
            ((CListItemPtr)pItem)->SetAction(this);
            if (NULL != pItem)
            {
                pItem->setObjectName(QString("CSettingItem%1").arg(i));
                pItem->hide();
                pItem->setFixedHeight(38);
                m_vecNorItem.push_back(pItem);
                m_pZeroFramelist->AddItem(pItem);
            }

            //翻页时纠正选中项
            if (m_vecNorItem[j]->IsFocus() && m_vecNorItem[j]->GetDataIndex() != GetCurrentIndex())
            {
                m_vecNorItem[j]->FocusItem(false);
            }

            m_vecNorItem[j]->show();
#else
            if (NULL == m_vecNorItem[j])
            {
                continue;
            }

            ((CListItemPtr)m_vecNorItem[j])->SetDataIndex(i);

            if (NULL != m_vecWgtPair[i].first)
            {
                m_vecNorItem[j]->SetLabText(m_vecWgtPair[i].first);
            }
            if (m_vecWgtPair[i].second != NULL)
            {
                m_vecNorItem[j]->SetContentWgt(m_vecWgtPair[i].second);
#ifdef SAME_ITEM_ADD_TO_FRAMELIST_ONCE
                m_vecNorItem[j]->ContentWgt()->show();
#endif

                if (!m_bIsEditPage && FullKeyboardEnabled())
                {
                    if ((m_vecWgtPair[i].second->inherits("QComboBox")
                            || m_vecWgtPair[i].second->inherits("QLineEdit")
                            || m_vecWgtPair[i].second->inherits("qCheckBox"))
                            && m_vecWgtPair[i].second->isEnabled())
                    {
                        m_bIsEditPage = true;
                    }
                }
            }

            m_vecNorItem[j]->SetWgetHost(m_pItemParent);

            //翻页时纠正选中项
            if (m_vecNorItem[j]->IsFocus() && m_vecNorItem[j]->GetDataIndex() != GetCurrentIndex())
            {
                m_vecNorItem[j]->FocusItem(false);
            }

#ifndef SAME_ITEM_ADD_TO_FRAMELIST_ONCE
            ((CListItemPtr)m_vecNorItem[j])->SetAction(this);
            m_pZeroFramelist->AddItem(m_vecNorItem[j]);
            m_vecNorItem[j]->Relayout();
#else
            m_vecNorItem[j]->show();
            if (j >= m_pZeroFramelist->GetTotalItemCount())
            {
                ((CListItemPtr)m_vecNorItem[j])->SetAction(this);
                m_pZeroFramelist->AddItem(m_vecNorItem[j]);
            }
            m_vecNorItem[j]->Relayout();
#endif
#endif
        }
#ifdef SAME_ITEM_ADD_TO_FRAMELIST_ONCE
        // del over Items
        for (int iDelIdx = iPageItemCount - 1; iDelIdx >= j; --iDelIdx)
        {
            m_pZeroFramelist->DelItemByIndex(iDelIdx);
        }
#endif

        // 这里设置数据准备好 为了下边设置焦点弹出全键盘
        m_bIsListReady = true;

        ZEROUI_INFO("curDataIdx[%d]", GetCurrentIndex());
        if (GetCurrentIndex() >= nDataIndex && GetCurrentIndex() < m_vecWgtPair.size()
                && GetCurrentIndex() < nDataIndex + m_pZeroFramelist->GetTotalItemCount()
                // 上下键切页 在处理按键时处理了焦点，这里不设置焦点
                && (!FullKeyboardEnabled() || !m_bKeyChangePage)
           )
        {
            m_pZeroFramelist->SetItemFocusByIndex(m_pZeroFramelist->GetIndexByDataIndex(GetCurrentIndex()));
        }
        else
        {
            m_pZeroFramelist->SelectItemByIndex(0);
        }
#ifdef SAME_ITEM_ADD_TO_FRAMELIST_ONCE
        m_pZeroFramelist->update();
#endif
    }
    SetKeyboardEnterType();
    m_bIsListReady = true;
    m_bKeyChangePage = false;
}

bool ZeroListDelegate::ProcessMouse(QObject* pObj)
{
    if (m_pZeroFramelist == NULL)
    {
        return false;
    }

    if (pObj->inherits("CSettingItem"))
    {
        bool bOneEditItem = GetEditItemCount() == 1;
        CListItemPtr pItem = CListItemPtr(pObj);

        // 点击item此时还未focus,要判断Item的index
        int index = m_pZeroFramelist->GetIndexByItem(pItem);
        qSetEnterType(bOneEditItem
                      ? ENTER_TYPE_ICON
                      : (index == GetLastEditItemIndex()
                         ? ENTER_TYPE_DONE
                         : ENTER_TYPE_NEXT));
        if (index != m_pFramelist->GetFocusItemIndex())
        {
            pItem->SetFocus(true);
            AdjustFrameList(false);
        }
    }
    else if (pObj->inherits("QLineEdit"))
    {
        AdjustFrameList(false);
        SetKeyboardEnterType();
    }
    else
    {
        SetKeyboardEnterType();
    }

    return false;
}

void ZeroListDelegate::SetKeyboardEnterType()
{
    bool bOneEditItem = GetEditItemCount() == 1;
    int iRow = m_pZeroFramelist->GetFocusItemIndex();

    qSetEnterType(bOneEditItem
                  ? ENTER_TYPE_ICON
                  : (iRow == GetLastEditItemIndex()
                     ? ENTER_TYPE_DONE
                     : ENTER_TYPE_NEXT));
}
