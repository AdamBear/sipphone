#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "mtswvoicemaillistdelegate.h"
#include "settingui/src/settingitemfactory.h"
#include "dlgmtswvoicemail.h"
#include "dsklog/log.h"
#include "metaswitchui/mtswvoicemaillistitem.h"
#include "include/uicommon.h"
#include "baseui/t4xpicpath.h"
#include "uicommon/qtcommon/qmisc.h"

#define ITEM_HIGHT      56
#define MAX_ITEM_CNT    6

MTSWVoicemailListDelegate::MTSWVoicemailListDelegate()
    : SubpageListDeleagate()
    , m_pVoicemailFramelist(NULL)
    , m_pItemPat(NULL)
{

    m_iTotal = 0;
}

MTSWVoicemailListDelegate::~MTSWVoicemailListDelegate()
{
    m_vecWgtPair.clear();
    UnBindFramelist();
    m_pItemPat = NULL;
}

void MTSWVoicemailListDelegate::PreCreateItem()
{
    m_pFramelist->SetMaxPageItemCount(MAX_ITEM_CNT);
    int iPageItemCount = (NULL == m_pVoicemailFramelist) ? MAX_ITEM_CNT :
                         m_pVoicemailFramelist->GetMaxPageItemCount();

    QWidgetPair qPair;

    qPair.first = "";
    qPair.second = NULL;
    SettingItemList & m_vecNorItem = GetSetItemList();

    for (int i = 0; i < iPageItemCount; ++i)
    {
        qPair.bAddIndex = false;
        qPair.iIndex = i;
        //创建Item
        CSettingItem * pItem = g_SettingItemManger->CreateItemByType(VOICEMAIL_LIST_ITEM, m_pItemPat,
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

void MTSWVoicemailListDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem,
        int nAction)
{
    if (NULL == pItem || !(pItem->inherits("CVoiceMailListItem")) || !IsFrameListAttach(pFrameList))
    {
        return;
    }

    if (nAction == MTSW_VOICE_MAIL_MORE)
    {
        emit ItemMoreAction(pItem);
        return;
    }
    else if (nAction == MTSW_VOICE_MAIL_STOP)
    {
        emit ItemStopAction(pItem);
        return;
    }
    else if (nAction == MTSW_VOICE_MAIL_FINISH)
    {
        emit ItemFinishAction(pItem);
        return;
    }

    return SubpageListDeleagate::OnItemAction(pFrameList, pItem, nAction);
}


void MTSWVoicemailListDelegate::BindFramelist(CFrameList * pList)
{
    if (NULL != pList)
    {
        m_pVoicemailFramelist = pList;
        m_pVoicemailFramelist ->SetDelegate(this);
    }
    SubpageListDeleagate::BindFramelist(pList);
}

void MTSWVoicemailListDelegate::UnBindFramelist()
{
    m_pVoicemailFramelist = NULL;
    SubpageListDeleagate::UnBindFramelist();
}

// set widget pair
void MTSWVoicemailListDelegate::SetItemsData(QWPVector vecWgtpair, int iTotal)
{
    m_vecWgtPair.clear();
    m_vecWgtPair = vecWgtpair;
    m_iTotal = iTotal;
}

void MTSWVoicemailListDelegate::HideItem()
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

void MTSWVoicemailListDelegate::ReloadData(int nDataIndex, int nDataCount)
{
    CDlgMTSWVoicemail * pMTSWVoicemail = qobject_cast<CDlgMTSWVoicemail *>(UIManager_GetPrivateDlg(
            DLG_CDlgMTSWVoicemail));
    if (NULL == pMTSWVoicemail || !pMTSWVoicemail->IsTopPage())
    {
        return ;
    }
    pMTSWVoicemail->LoadVoicemailList(nDataIndex, nDataCount);
}

void MTSWVoicemailListDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex,
        int nDataCount)
{
    HideItem();
    int iCurrentIndex = GetCurrentIndex();
    ReloadData(nDataIndex, nDataCount);
    SettingItemList & m_vecNorItem = GetSetItemList();
    if (NULL != m_pVoicemailFramelist && m_iTotal >= nDataIndex)
    {
        //先清空当前数据
        m_pVoicemailFramelist->DetachAllItem(true);
        m_pVoicemailFramelist->SetTotalDataCount(m_iTotal);
        m_pVoicemailFramelist->SetItemAttach();

        int iPageItemCount = m_pVoicemailFramelist->GetMaxPageItemCount();
        //重新添加数据
        for (int i = nDataIndex, j = 0; i < nDataIndex + nDataCount && i < m_iTotal
                && j < iPageItemCount; i++, j++)
        {
            if (NULL == m_vecNorItem[j])
            {
                continue;
            }

            ((CListItemPtr)m_vecNorItem[j])->SetDataIndex(i);

            if (NULL != m_vecWgtPair[j].first)
            {
                m_vecNorItem[j]->SetLabText(m_vecWgtPair[j].first);
            }
            if (m_vecWgtPair[j].second != NULL)
            {
                m_vecNorItem[j]->SetContentWgt(m_vecWgtPair[j].second);
            }
            if (NULL != m_vecWgtPair[j].third)
            {
                m_vecNorItem[j]->SetClickAction(m_vecWgtPair[j].third);
            }

            if (NULL != m_vecNorItem[j] && m_vecNorItem[j]->inherits("CVoiceMailListItem"))
            {
                VoiceMsgItemDetailData VMIDD;
                MTSWVM_GetMessageDetail(&VMIDD, i);

                //初始或stop状态
                if (strcmp(m_vecWgtPair[j].third.toUtf8().data(), DM_SOFTKEY_MTSW_PLAY) == 0)
                {
                    sys_Time_t timer;
                    sys_get_systime(&timer);
                    yl::string strDate = GetData(VMIDD.m_tTime + timer.sys_tm.tm_gmtoff, true);
                    yl::string strTime = GetHourMinute(VMIDD.m_tTime, true);

                    ((CVoiceMailListItem *)m_vecNorItem[j])->SetDate(toQString(strDate));
                    ((CVoiceMailListItem *)m_vecNorItem[j])->SetTime(toQString(strTime));
                    ((CVoiceMailListItem *)m_vecNorItem[j])->SetPlayStatus(false);
                    //Stop状态关闭定时器
                    if (strcmp(m_vecWgtPair[j].strIcon.c_str(), PIC_USB_BROWSE_ITEM_STOP_PRESS) == 0)
                    {
                        m_vecWgtPair[j].strIcon = PIC_USB_BROWSE_ITEM_PLAY;
                        ((CVoiceMailListItem *)m_vecNorItem[j])->StopPlayTimer();
                        ((CVoiceMailListItem *)m_vecNorItem[j])->ResetTime();
                    }
                }
                //Play或Resume状态
                else if (strcmp(m_vecWgtPair[j].third.toUtf8().data(), DM_SOFTKEY_MTSW_PAUSE) == 0)
                {
                    //Play状态关闭定时器
                    if (strcmp(m_vecWgtPair[j].strIcon.c_str(), PIC_USB_BROWSE_ITEM_PLAY_PRESS) == 0)
                    {
                        m_vecWgtPair[j].strIcon = PIC_USB_BROWSE_ITEM_PAUSE;
                        CloseAllTimer(m_vecNorItem);
                    }
                    ((CVoiceMailListItem *)m_vecNorItem[j])->SetPlayStatus(true);
                    ((CVoiceMailListItem *)m_vecNorItem[j])->StartPlayTimer();
                }
                //Pause状态
                else if (strcmp(m_vecWgtPair[j].third.toUtf8().data(), DM_SOFTKEY_MTSW_RESUME) == 0)
                {
                    ((CVoiceMailListItem *)m_vecNorItem[j])->SetPlayStatus(true);
                    ((CVoiceMailListItem *)m_vecNorItem[j])->StopPlayTimer();
                }

                ((CVoiceMailListItem *)m_vecNorItem[j])->SetReadStatus(VMIDD.m_eStatus == STATUS_READ);
                ((CVoiceMailListItem *)m_vecNorItem[j])->SetShowUrgent(VMIDD.m_bUrgent);
                ((CVoiceMailListItem *)m_vecNorItem[j])->AddIcon(m_vecWgtPair[j].strIcon.c_str());
            }

            //翻页时纠正选中项
            if (m_vecNorItem[j]->IsFocus() && m_vecNorItem[j]->GetDataIndex() != GetCurrentIndex())
            {
                m_vecNorItem[j]->FocusItem(false);
            }

            ((CListItemPtr)m_vecNorItem[j])->SetAction(this);
            m_pVoicemailFramelist->AddItem(m_vecNorItem[j]);
            m_vecNorItem[j]->Relayout();
        }

        if (iCurrentIndex >= nDataIndex
                && iCurrentIndex < nDataIndex + m_pVoicemailFramelist->GetTotalItemCount())
        {
            m_pVoicemailFramelist->SetItemFocusByIndex(m_pVoicemailFramelist->GetIndexByDataIndex(
                        iCurrentIndex));
        }
        else
        {
            m_pVoicemailFramelist->SetItemFocusByIndex(0);
        }
        m_pVoicemailFramelist->setFocus();
    }
}

void MTSWVoicemailListDelegate::CloseAllTimer(SettingItemList & m_vecNorItem)
{
    for (int j = 0; j < m_vecNorItem.size(); j++)
    {
        if (NULL == m_vecNorItem[j])
        {
            continue;
        }
        ((CVoiceMailListItem *)m_vecNorItem[j])->StopPlayTimer();
        ((CVoiceMailListItem *)m_vecNorItem[j])->ResetTime();
    }
}
#endif
