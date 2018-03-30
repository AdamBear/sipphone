#include "visualvoicemaillistdelegate.h"
#include "settingui/src/settingitemfactory.h"
#include "messageui/src/messagelistitem.h"
#include "qtcommon/qmisc.h"
#include "dsklog/log.h"
#include "uikernel/languagehelper.h"
#include "include/translateiddefine.h"
#include "broadsoft/voicemail/include/modvirtualvoicemail.h"
#include "commonapi/stringhelperapi.h"
#include "baseui/t4xpicpath.h"

#include "fileplayerdefine.h"
#include "setting/include/modsetting.h"

#include "vvmfileplayercontroller.h"
#include "vvmitem.h"

namespace
{
#ifdef _T48
#define MAX_ITEM_CNT    6
#define ITEM_HIGHT      56
#else
#define MAX_ITEM_CNT    5
#define ITEM_HIGHT      38
#endif

}

CVisualVoiceMailListDelegate::CVisualVoiceMailListDelegate()
    : SubpageListDeleagate()
    , m_pVisualVoiceMailFramelist(NULL)
    , m_pItemPat(NULL)
    , m_nTotalItemCount(0)
{
}

#ifndef _T48
CVisualVoiceMailListDelegate::CVisualVoiceMailListDelegate(CFrameList * pList)
    : SubpageListDeleagate(pList)
    , m_pVisualVoiceMailFramelist(NULL)
    , m_pItemPat(NULL)
    , m_nTotalItemCount(0)
{
    BindFramelist(pList);
}
#endif

CVisualVoiceMailListDelegate::~CVisualVoiceMailListDelegate()
{
    UnBindFramelist();
    m_pItemPat = NULL;
}


void CVisualVoiceMailListDelegate::BindFramelist(CFrameList * pList)
{
#ifndef _T48
    SaveListStatus();
#endif
    if (NULL != pList)
    {
        m_pVisualVoiceMailFramelist = pList;
        m_pVisualVoiceMailFramelist ->SetDelegate(this);
    }
#ifdef _T48
    SubpageListDeleagate::BindFramelist(pList);
#endif
}

void CVisualVoiceMailListDelegate::UnBindFramelist()
{
    m_pVisualVoiceMailFramelist = NULL;
}

void CVisualVoiceMailListDelegate::PreCreateItem()
{
    int iPageItemCount = (NULL == m_pVisualVoiceMailFramelist) ? GetMaxPagePageItemCount() :
                         m_pVisualVoiceMailFramelist->GetMaxPageItemCount();

    QWidgetPair qPair;
    m_vecpLabel.reserve(iPageItemCount);

    qPair.first = "";
    qPair.second = NULL;
#ifdef _T48
    qPair.eType = SETTING_ITEM_VVM;
#endif
    SettingItemList & m_vecNorItem = GetSetItemList();
    m_vecNorItem.clear();

    for (int i = 0; i < iPageItemCount; ++i)
    {
        qPair.bAddIndex = false;
        qPair.bAddMark = false;
        qPair.iIndex = i;
        //创建Item

        CSettingItem * pItem = g_SettingItemManger->CreateItemByType(SETTING_ITEM_VVM, m_pItemPat, qPair);

        if (NULL != pItem)
        {
#ifdef _T48
            connect(&g_VVMController, SIGNAL(DialogRefreshPlayInfo()), pItem,
                    SLOT(OnItemRefreshPlayProgress()));
#endif

            pItem->setObjectName(QString("CSettingItem%1").arg(i));
            pItem->hide();
            pItem->setFixedHeight(ITEM_HIGHT);
            m_vecNorItem.push_back(pItem);
            ((CListItemPtr)pItem)->SetAction(this);
//          pItem->SetChangeStyleOnSelect(false);
        }

#ifndef _T48
        //创建label
        QLabel * plabName = new QLabel(m_pItemPat);
        if (NULL != plabName)
        {
            m_vecpLabel.push_back(plabName);
        }
#endif

    }
}

void CVisualVoiceMailListDelegate::HideItem()
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

void CVisualVoiceMailListDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex,
        int nDataCount)
{
    HideItem();

    SETTINGUI_INFO("CVisualVoiceMailListDelegate::OnLoadData, nDataIndex = %d, nDataCount = %d",
                   nDataIndex, nDataCount);
    if (nDataIndex < 0 || nDataCount < 1)
    {
        return;
    }

    VoiceMailListData voiceMailListData;
    voiceMailListData.m_nIndexFrom = nDataIndex;
    voiceMailListData.m_nCountToGet = nDataCount;
    if (!vvMail_GetVoiceMailListData(&voiceMailListData))
    {
        return;
    }

    int nTotalItemCount = voiceMailListData.m_nTotalCount;

    SETTINGUI_INFO("CVisualVoiceMailListDelegate::OnLoadData, nTotalItemCount = %d", nTotalItemCount);
    if (NULL == m_pVisualVoiceMailFramelist
            || (nTotalItemCount < nDataIndex
                && nTotalItemCount <= 0))
    {
        return;
    }

    int iCurrentIndex = GetCurrentIndex();
    SettingItemList & vecItemList = GetSetItemList();

    //先清空当前数据
    m_pVisualVoiceMailFramelist->DetachAllItem(true);
    //重新设置父类
    for (SettingItemList::iterator iter = vecItemList.begin(); iter != vecItemList.end(); ++iter)
    {
        (*iter)->setParent(m_pVisualVoiceMailFramelist);
    }
    m_pVisualVoiceMailFramelist->SetTotalDataCount(nTotalItemCount);
    m_pVisualVoiceMailFramelist->SetItemAttach();

    YLList<sVoiceMailData>::iterator it = voiceMailListData.m_listData.begin();
    YLList<sVoiceMailData>::iterator it_end = voiceMailListData.m_listData.end();

    int iPageItemCount = m_pVisualVoiceMailFramelist->GetMaxPageItemCount();

    for (int i = nDataIndex, j = 0
                                 ; i < (nDataIndex + nDataCount) && i < nTotalItemCount  && j < iPageItemCount && it != it_end
            ; ++i, ++j, ++it)
    {
        if (NULL == vecItemList[j])
        {
            continue;
        }

        sVoiceMailData & voiceMailData = *it;
        ((CListItemPtr)vecItemList[j])->SetDataIndex(i);

#ifdef _T48
        vecItemList[j]->SetClickAction(FILEPLAYER_ACTION_ENTER_PLAY);

        if (vecItemList[j]->inherits("CVVMItem"))
        {
            CVVMItem * pVVMItem = (CVVMItem *)vecItemList[j];
            if (NULL == pVVMItem)
            {
                continue;
            }

            pVVMItem->ResetItem();
            pVVMItem->LoadItemData();
        }
#else
        //First
        if (vecItemList[j]->inherits("CVVMItem"))
        {
            CVVMItem * pVVMItem = (CVVMItem *)vecItemList[j];
            if (NULL == pVVMItem)
            {
                continue;
            }

            if (voiceMailData.m_bUrgent)
            {
                pVVMItem->SetUrgent(true);
            }
            else
            {
                pVVMItem->SetUrgent(false);
            }

            if (voiceMailData.m_bConfidential)
            {
                pVVMItem->SetConfidential(true);
            }
            else
            {
                pVVMItem->SetConfidential(false);
            }
        }

        QString strName  = "";
        if (voiceMailData.m_bPrivacy)
        {
            strName = LANG_TRANSLATE(TRID_ANONYMOUS);
        }
        else
        {
            strName = toQString(voiceMailData.m_strName);
        }
        vecItemList[j]->SetLabText(strName);

        //Second
        yl::string strTime = voiceMailData.m_strTime.substr(0, 10);
        time_t tTime = atoi(strTime.c_str());
        strTime = Timet2String(tTime);

        QLabel * pLabel = m_vecpLabel[j];
        if (NULL != pLabel)
        {
            pLabel->setText(toQString(strTime));
            pLabel->setParent(vecItemList[j]);
            vecItemList[j]->SetContentWgt(pLabel);
        }

        //Third
        vecItemList[j]->SetClickAction(FILEPLAYER_ACTION_ENTER_PLAY);

        //Icon
        if (NULL != vecItemList[j] && vecItemList[j]->inherits("CMessageListItem"))
        {
            if (MEDIA_audio == voiceMailData.m_eMediaType)
            {
                if (voiceMailData.m_bRead)
                {
                    ((CMessageListItem *)vecItemList[j])->AddIcon(PIC_GENERAL_VOICE_MESSAGE_READ);
                }
                else
                {
                    ((CMessageListItem *)vecItemList[j])->AddIcon(PIC_GENERAL_VOICE_MESSAGE_UNREAD);
                }
            }
            else if (MEDIA_video == voiceMailData.m_eMediaType)
            {
                if (voiceMailData.m_bRead)
                {
                    ((CMessageListItem *)vecItemList[j])->AddIcon(PIC_GENERAL_VIDEO_MESSAGE_READ);
                }
                else
                {
                    ((CMessageListItem *)vecItemList[j])->AddIcon(PIC_GENERAL_VIDEO_MESSAGE_UNREAD);
                }
            }
        }
#endif

        //翻页时纠正选中项
        if (vecItemList[j]->IsFocus() && vecItemList[j]->GetDataIndex() != iCurrentIndex)
        {
            vecItemList[j]->FocusItem(false);
        }

        ((CListItemPtr)vecItemList[j])->SetAction(this);
        m_pVisualVoiceMailFramelist->AddItem(vecItemList[j]);
        vecItemList[j]->show();
        vecItemList[j]->Relayout();
    }

    if (iCurrentIndex >= nDataIndex
            && iCurrentIndex < nDataIndex + m_pVisualVoiceMailFramelist->GetTotalItemCount())
    {
        m_pVisualVoiceMailFramelist->SetItemFocusByIndex(m_pVisualVoiceMailFramelist->GetIndexByDataIndex(
                    iCurrentIndex));
        m_pVisualVoiceMailFramelist->FocusItemByDataIndex(iCurrentIndex);
    }
    else
    {
        m_pVisualVoiceMailFramelist->SetItemFocusByIndex(0);
    }
    m_pVisualVoiceMailFramelist->setFocus();
}

int CVisualVoiceMailListDelegate::GetMaxPagePageItemCount()
{
    return MAX_ITEM_CNT;
}
