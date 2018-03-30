#include "usbphotolistdelegate.h"
#include "dlgusbbrowsephoto.h"
#include "settingui/src/settingitemfactory.h"
#include "keymap.h"
#include "qtcommon/qmisc.h"
#include "dsklog/log.h"
#include "actiondefinecommon.h"
#include "usbphotolistcontrl.h"

namespace
{
#define ITEM_HIGHT      56
#define MAX_ITEM_CNT    6
}

USBPhotoListDelegate::USBPhotoListDelegate()
    : SubpageListDeleagate()
    , m_pUSBPhotoFramelist(NULL)
    , m_pItemPat(NULL)
{
}

USBPhotoListDelegate::~USBPhotoListDelegate()
{
    m_vecWgtPair.clear();
    UnBindFramelist();
    m_pItemPat = NULL;
}


void USBPhotoListDelegate::BindFramelist(CFrameList * pList)
{
    if (NULL != pList)
    {
        m_pUSBPhotoFramelist = pList;
        m_pUSBPhotoFramelist ->SetDelegate(this);
    }
    SubpageListDeleagate::BindFramelist(pList);
}

void USBPhotoListDelegate::UnBindFramelist()
{
    m_pUSBPhotoFramelist = NULL;
    SubpageListDeleagate::UnBindFramelist();
}

void USBPhotoListDelegate::PreCreateItem()
{
    m_pUSBPhotoFramelist->SetMaxPageItemCount(MAX_ITEM_CNT);
    m_iTotal = 0;

    QWidgetPair qPair;

    qPair.first = "";
    qPair.second = NULL;
    qPair.eType = SETTING_ITEM_USB_PHOTO;
    SettingItemList & m_vecNorItem = GetSetItemList();

    for (int i = 0; i < MAX_ITEM_CNT; ++i)
    {
        qPair.bAddIndex = false;
        qPair.bAddMark = false;
        qPair.iIndex = i;
        //创建Item
        CSettingItem * pItem = g_SettingItemManger->CreateItemByType(SETTING_ITEM_USB_PHOTO, m_pItemPat,
                               qPair);
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

void USBPhotoListDelegate::SetItemsData(QWPVector vecWgtpair, int iTotal)
{
    m_vecWgtPair.clear();
    m_vecWgtPair = vecWgtpair;
    m_iTotal = iTotal;
}

void USBPhotoListDelegate::HideItem()
{
    SettingItemList & m_vecNorItem = GetSetItemList();
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


void USBPhotoListDelegate::ReloadData(int nDataIndex, int nDataCount)
{
    CDlgUSBBrowsePhoto * pUSBBrowsePhoto = qobject_cast<CDlgUSBBrowsePhoto *>
                                           (UIManager_GetPrivateDlg(DLG_CDlgUSBBrowsePhoto));
    if (NULL == pUSBBrowsePhoto || !pUSBBrowsePhoto->IsTopPage())
    {
        return ;
    }
    pUSBBrowsePhoto->LoadPhotoList(nDataIndex, nDataCount);
}

void USBPhotoListDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem,
                                        int nAction)
{
    if (NULL == pItem || !(pItem->inherits("CUSBPhotoItem")) || pFrameList != m_pUSBPhotoFramelist)
    {
        return;
    }

    if (nAction == LIST_ITEM_ACTION_SELECT)
    {
        g_pUSBPhotoContrl->SetCurrentIndex(pItem->GetDataIndex());

        CDlgUSBBrowsePhoto * pUSBBrowsePhoto = qobject_cast<CDlgUSBBrowsePhoto *>
                                               (UIManager_GetPrivateDlg(DLG_CDlgUSBBrowsePhoto));
        if (NULL == pUSBBrowsePhoto || !pUSBBrowsePhoto->IsTopPage())
        {
            return;
        }
        pUSBBrowsePhoto->OnAction(ACTION_ENTER);

    }
    else if (nAction == USB_CLICK_ACTION_DELETE)
    {
        g_pUSBPhotoContrl->SetCurrentIndex(pItem->GetDataIndex());

        emit ItemDeleteAction(pItem);
        return;
    }

    return;
}

void USBPhotoListDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    HideItem();
    ReloadData(nDataIndex, nDataCount);
    SETTINGUI_INFO("USBPhotoListDelegate OnLoadData nDataIndex[%d], nDataCount[%d] size[%d] ",
                   nDataIndex, nDataCount, m_iTotal);
    if (NULL == m_pUSBPhotoFramelist
            || (m_iTotal < nDataIndex
                && m_iTotal != 0))
    {
        return;
    }

    int iCurrentIndex = GetCurrentIndex();
    SettingItemList & m_vecNorItem = GetSetItemList();

    //先清空当前数据
    m_pUSBPhotoFramelist->DetachAllItem(true);
    //重新设置父类
    for (SettingItemList::iterator iter = m_vecNorItem.begin(); iter != m_vecNorItem.end(); ++iter)
    {
        (*iter)->setParent(m_pUSBPhotoFramelist);
    }
    m_pUSBPhotoFramelist->SetTotalDataCount(m_iTotal);
    m_pUSBPhotoFramelist->SetItemAttach();

    int iPageItemCount = m_pUSBPhotoFramelist->GetMaxPageItemCount();
    for (int i = nDataIndex, j = 0; i < (nDataIndex + nDataCount) && i < m_iTotal
            && j < iPageItemCount; i++, j++)
    {
        if (NULL == m_vecNorItem[j])
        {
            continue;
        }

        ((CListItemPtr)m_vecNorItem[j])->SetDataIndex(i);

        if (m_vecNorItem[j]->inherits("CUSBPhotoItem"))
        {
            CUSBPhotoItem * pUSBAudioItem = (CUSBPhotoItem *)m_vecNorItem[j];
            pUSBAudioItem->ResetItemInfo(m_vecWgtPair[j]);
        }

        //翻页时纠正选中项
        if (m_vecNorItem[j]->IsFocus() && m_vecNorItem[j]->GetDataIndex() != iCurrentIndex)
        {
            m_vecNorItem[j]->FocusItem(false);
        }

        ((CListItemPtr)m_vecNorItem[j])->SetAction(this);
        m_pUSBPhotoFramelist->AddItem(m_vecNorItem[j]);
        m_vecNorItem[j]->Relayout();
    }

    if (iCurrentIndex >= nDataIndex
            && iCurrentIndex < nDataIndex + m_pUSBPhotoFramelist->GetTotalItemCount())
    {
        m_pUSBPhotoFramelist->SetItemFocusByIndex(m_pUSBPhotoFramelist->GetIndexByDataIndex(
                    iCurrentIndex));
        m_pUSBPhotoFramelist->FocusItemByDataIndex(iCurrentIndex);
    }
    else
    {
        m_pUSBPhotoFramelist->SetItemFocusByIndex(0);
    }
    m_pUSBPhotoFramelist->setFocus();
}

void USBPhotoListDelegate::UpdateList()
{

    if (m_pUSBPhotoFramelist != NULL)
    {
        m_pUSBPhotoFramelist->setFocus(Qt::ActiveWindowFocusReason);//页面捕获去焦点，焦点始终给list
        int iPageItemCount = m_pUSBPhotoFramelist->GetMaxPageItemCount();
        if (m_pUSBPhotoFramelist->GetTotalItemCount() == 0 && m_iCurrentIndex < iPageItemCount)
        {
            //第一次进入页面时加载第一页
            m_pUSBPhotoFramelist->RefreshData(0);
            return;
        }

        if (iPageItemCount <= 0)
        {
            return;
        }

        //重新回到这个页面时刷新到之前浏览的页
        m_pUSBPhotoFramelist->RefreshData((g_pUSBPhotoContrl->GetCurrentIndex() / iPageItemCount) * iPageItemCount);
    }

}


