#include "usbrecordlistdelegate.h"
#include "dlgusbbrowserecord.h"
#include "settingui/src/settingitemfactory.h"
#include "keymap.h"
#include "qtcommon/qmisc.h"
#include "dsklog/log.h"

namespace
{
#define ITEM_HIGHT      56
#define MAX_ITEM_CNT    6
}

USBRecordListDelegate::USBRecordListDelegate()
    : SubpageListDeleagate()
    , m_pUSBRecordFramelist(NULL)
    , m_pItemPat(NULL)
{
}

USBRecordListDelegate::~USBRecordListDelegate()
{
    m_vecWgtPair.clear();
    UnBindFramelist();
    m_pItemPat = NULL;
}


void USBRecordListDelegate::BindFramelist(CFrameList * pList)
{
    if (NULL != pList)
    {
        m_pUSBRecordFramelist = pList;
        m_pUSBRecordFramelist ->SetDelegate(this);
    }
    SubpageListDeleagate::BindFramelist(pList);
}

void USBRecordListDelegate::UnBindFramelist()
{
    m_pUSBRecordFramelist = NULL;
    SubpageListDeleagate::UnBindFramelist();
}

void USBRecordListDelegate::PreCreateItem()
{
    m_pUSBRecordFramelist->SetMaxPageItemCount(MAX_ITEM_CNT);
    m_iTotal = 0;

    QWidgetPair qPair;

    qPair.first = "";
    qPair.second = NULL;
    qPair.eType = SETTING_ITEM_USB_AUDIO;
    SettingItemList & m_vecNorItem = GetSetItemList();

    for (int i = 0; i < MAX_ITEM_CNT; ++i)
    {
        qPair.bAddIndex = false;
        qPair.bAddMark = false;
        qPair.iIndex = i;
        //创建Item
        CSettingItem * pItem = g_SettingItemManger->CreateItemByType(SETTING_ITEM_USB_AUDIO, m_pItemPat,
                               qPair);
        if (NULL != pItem)
        {
            pItem->setObjectName(QString("CSettingItem%1").arg(i));
            //当一个AudioItem进行某种操作时，需要通知其他所有Item去改变状态
            connect(pItem, SIGNAL(PlayPress(CUSBAudioItem *)), this,
                    SIGNAL(SetOtherItemStateWait(CUSBAudioItem *)));
            connect(pItem, SIGNAL(StopPress(CUSBAudioItem *)), this,
                    SIGNAL(SetOtherItemStateStop(CUSBAudioItem *)));
            connect(this, SIGNAL(SetOtherItemStateWait(CUSBAudioItem *)), pItem,
                    SLOT(OnOtherItemPlay(CUSBAudioItem *)));
            connect(this, SIGNAL(SetOtherItemStateStop(CUSBAudioItem *)), pItem,
                    SLOT(OnOtherItemStop(CUSBAudioItem *)));

            //记录AudioItem状态
            connect(pItem, SIGNAL(PlayStatusChange(CUSBAudioItem *)), this,
                    SLOT(OnItemPlayStatusChange(CUSBAudioItem *)));
            //播放失败通知窗口
            connect(pItem, SIGNAL(PlayFail(CUSBAudioItem *)), this, SLOT(OnItemPlayFail(CUSBAudioItem *)));


            pItem->hide();
            pItem->setFixedHeight(ITEM_HIGHT);
            m_vecNorItem.push_back(pItem);
            ((CListItemPtr)pItem)->SetAction(this);
        }
    }
}

void USBRecordListDelegate::OnItemPlayStatusChange(CUSBAudioItem * pItem)
{
    bool bPlay = false;
    m_sItemPlayInfo.Reset();
    USB_PLAY_STATE PlayState = pItem->GetPlayStatus();

    if (PlayState == USB_PLAY_STATE_PLAY
            || PlayState == USB_PLAY_STATE_PAUSE)
    {
        bPlay =  true;
        RecordFileInfo RecordInfo = pItem->GetPlayFileInfo();
        m_sItemPlayInfo.strFile = RecordInfo.strPath + RecordInfo.strName;
        m_sItemPlayInfo.nIndex = modRecord_GetResourceByFile(RecordInfo, m_sItemPlayInfo.strFile.c_str(),
                                 RECORD_AUDIO);
        m_sItemPlayInfo.PlayState = PlayState;
        SETTINGUI_INFO("USBRecordListDelegate::OnItemPlayStatusChange Index[%d] PlayState[%d] strFile[%s]",
                       m_sItemPlayInfo.nIndex, PlayState, m_sItemPlayInfo.strFile.c_str());
    }

    CDlgUSBBrowseRecord * pUSBBrowseRecord = qobject_cast<CDlgUSBBrowseRecord *>
            (UIManager_GetPrivateDlg(DLG_CDlgUSBBrowseRecord));
    if (NULL == pUSBBrowseRecord || !pUSBBrowseRecord->IsTopPage())
    {
        return ;
    }
    pUSBBrowseRecord->PlayStatusChange(bPlay);
}

void USBRecordListDelegate::OnItemPlayFail(CUSBAudioItem * pItem)
{
    CDlgUSBBrowseRecord * pUSBBrowseRecord = qobject_cast<CDlgUSBBrowseRecord *>
            (UIManager_GetPrivateDlg(DLG_CDlgUSBBrowseRecord));
    if (NULL == pUSBBrowseRecord || !pUSBBrowseRecord->IsTopPage())
    {
        return ;
    }
    pUSBBrowseRecord->PlayFail();
}

void USBRecordListDelegate::SetItemsData(QWPVector vecWgtpair, int iTotal)
{
    m_vecWgtPair.clear();
    m_vecWgtPair = vecWgtpair;
    m_iTotal = iTotal;
}

void USBRecordListDelegate::HideItem()
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

void USBRecordListDelegate::PageChange(bool bHide)
{
    SETTINGUI_INFO("USBRecordListDelegate::PageChange(%d) nIndex[%d] strFile[%s]", bHide,
                   m_sItemPlayInfo.nIndex, m_sItemPlayInfo.strFile.c_str());

    //不处于播放状态则不做任何处理
    if (m_sItemPlayInfo.nIndex < 0)
    {
        return;
    }

    //隐藏页面暂停正在播放的录音
    if (bHide)
    {
        CListItemPtr pFocusWidget = m_pUSBRecordFramelist->GetItemByIndex(m_sItemPlayInfo.nIndex %
                                    MAX_ITEM_CNT);
        CSettingItem * pFocusItem = (CSettingItem *)pFocusWidget;

        if (NULL != pFocusItem && pFocusItem->inherits("CUSBAudioItem"))
        {
            CUSBAudioItem * pAudioItem = (CUSBAudioItem *)pFocusItem;
            if (NULL == pAudioItem)
            {
                return;
            }
            pAudioItem->AudioItemActionPause();
            //清空播放信息
            RecordFileInfo RecordInfo;
            pAudioItem->SetPlayFileInfo(RecordInfo);
        }
    }
}

void USBRecordListDelegate::ReloadList()
{
    //不处于播放状态则不做任何处理
    if (m_sItemPlayInfo.nIndex < 0)
    {
        return;
    }

    RecordFileInfo RecordInfo;
    //矫正Index,可能在播放录音的情况下,接到来电后又录制了新的录音文件
    m_sItemPlayInfo.nIndex = modRecord_GetResourceByFile(RecordInfo, m_sItemPlayInfo.strFile.c_str(),
                             RECORD_AUDIO);
    SETTINGUI_INFO("USBRecordListDelegate::ReloadList() nIndex[%d] strFile[%s] [%s] [%d] [%d]",
                   m_sItemPlayInfo.nIndex, m_sItemPlayInfo.strFile.c_str(), RecordInfo.strName.c_str(),
                   RecordInfo.lBreak, RecordInfo.lTime);
    if (m_sItemPlayInfo.nIndex < 0)
    {
        m_sItemPlayInfo.Reset();
        return;
    }

    CListItemPtr pFocusWidget = m_pUSBRecordFramelist->GetItemByIndex(m_sItemPlayInfo.nIndex %
                                MAX_ITEM_CNT);
    CSettingItem * pFocusItem = (CSettingItem *)pFocusWidget;

    if (NULL != pFocusItem && pFocusItem->inherits("CUSBAudioItem"))
    {
        CUSBAudioItem * pAudioItem = (CUSBAudioItem *)pFocusItem;
        if (NULL == pAudioItem)
        {
            return;
        }
        //重新加载播放文件数据
        pAudioItem->SetPlayFileInfo(RecordInfo);
        pAudioItem->LoadPlayData(RecordInfo.lTime, RecordInfo.lBreak);
        yl::string strFile = RecordInfo.strPath + RecordInfo.strName;
        modRecord_PlayStart(RECORD_AUDIO, strFile.c_str());
        modRecord_PlayPause(RECORD_AUDIO, strFile.c_str(), RecordInfo.lBreak);
    }
}

void USBRecordListDelegate::ReloadData(int nDataIndex, int nDataCount)
{
    CDlgUSBBrowseRecord * pUSBBrowseRecord = qobject_cast<CDlgUSBBrowseRecord *>
            (UIManager_GetPrivateDlg(DLG_CDlgUSBBrowseRecord));
    if (NULL == pUSBBrowseRecord || !pUSBBrowseRecord->IsTopPage())
    {
        return ;
    }
    pUSBBrowseRecord->LoadRecordList(nDataIndex, nDataCount);
}

bool USBRecordListDelegate::ProcessKeyEvent(QObject * pObject, QEvent * pEvent)
{
    if (NULL == m_pUSBRecordFramelist  || !m_pUSBRecordFramelist->isVisible())
    {
        return false;
    }

    QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
    switch (pKeyEvent->key())
    {
    case PHONE_KEY_OK:
    case PHONE_KEY_LEFT:
    case PHONE_KEY_RIGHT:
    {
        if (m_sItemPlayInfo.nIndex < 0)
        {
            return true;
        }
        // 由Item自己去处理快进(退)
        CListItemPtr pFocusWidget = m_pUSBRecordFramelist->GetItemByIndex(m_sItemPlayInfo.nIndex %
                                    MAX_ITEM_CNT);
        CSettingItem * pFocusItem = (CSettingItem *)pFocusWidget;

        if (NULL != pFocusItem && pFocusItem->inherits("CUSBAudioItem"))
        {
            CUSBAudioItem * pAudioItem = (CUSBAudioItem *)pFocusItem;
            if (NULL != pAudioItem && pAudioItem->ProcessKeyEvent(pObject, pEvent))
            {
                if (pAudioItem->GetDataIndex() != m_sItemPlayInfo.nIndex)
                {
                    if (pKeyEvent->key() == PHONE_KEY_OK)
                    {
                        if (m_sItemPlayInfo.PlayState == USB_PLAY_STATE_PLAY)
                        {
                            m_sItemPlayInfo.PlayState = USB_PLAY_STATE_PAUSE;
                        }
                        else if (m_sItemPlayInfo.PlayState == USB_PLAY_STATE_PAUSE)
                        {
                            m_sItemPlayInfo.PlayState = USB_PLAY_STATE_PLAY;
                        }
                    }
                }

                return true;
            }
        }
    }
    break;
    default:
        break;
    }

    return false;
}

void USBRecordListDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem,
        int nAction)
{
    if (NULL == pItem || !(pItem->inherits("CUSBAudioItem")) || pFrameList != m_pUSBRecordFramelist)
    {
        return;
    }

    CUSBAudioItem * pUSBAudioItem = (CUSBAudioItem *)pItem;
    m_iCurrentIndex = pUSBAudioItem->GetDataIndex();
    if (nAction == LIST_ITEM_ACTION_SELECT)
    {
        pUSBAudioItem->AudioItemSelect();
    }
    else if (nAction == USB_CLICK_ACTION_DELETE)
    {
        emit ItemDeleteAction(pItem);
        return;
    }

    return;
}

void USBRecordListDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    HideItem();
    ReloadData(nDataIndex, nDataCount);
    SETTINGUI_INFO("USBRecordListDelegate OnLoadData nDataIndex[%d], nDataCount[%d] size[%d] m_sItemPlayInfo.nIndex[%d]",
                   nDataIndex, nDataCount, m_iTotal, m_sItemPlayInfo.nIndex);
    if (NULL == m_pUSBRecordFramelist
            || (m_iTotal < nDataIndex
                && m_iTotal != 0))
    {
        return;
    }

    int iCurrentIndex = GetCurrentIndex();
    SettingItemList & m_vecNorItem = GetSetItemList();

    //先清空当前数据
    m_pUSBRecordFramelist->DetachAllItem(true);
    //重新设置父类
    for (SettingItemList::iterator iter = m_vecNorItem.begin(); iter != m_vecNorItem.end(); ++iter)
    {
        (*iter)->setParent(m_pUSBRecordFramelist);
    }
    m_pUSBRecordFramelist->SetTotalDataCount(m_iTotal);
    m_pUSBRecordFramelist->SetItemAttach();

    int iPageItemCount = m_pUSBRecordFramelist->GetMaxPageItemCount();
    for (int i = nDataIndex, j = 0; i < (nDataIndex + nDataCount) && i < m_iTotal
            && j < iPageItemCount; i++, j++)
    {
        if (NULL == m_vecNorItem[j])
        {
            continue;
        }

        ((CListItemPtr)m_vecNorItem[j])->SetDataIndex(i);

        if (m_vecNorItem[j]->inherits("CUSBAudioItem"))
        {
            CUSBAudioItem * pUSBAudioItem = (CUSBAudioItem *)m_vecNorItem[j];
            pUSBAudioItem->ResetItemInfo(m_vecWgtPair[j]);

            if (m_sItemPlayInfo.nIndex != -1)
            {
                if (m_sItemPlayInfo.nIndex == i)
                {
                    pUSBAudioItem->SetPlayStatus(m_sItemPlayInfo.PlayState);
                }
                else
                {
                    pUSBAudioItem->SetPlayStatus(USB_PLAY_STATE_WAIT);
                }
            }
        }

        //翻页时纠正选中项
        if (m_vecNorItem[j]->IsFocus() && m_vecNorItem[j]->GetDataIndex() != iCurrentIndex)
        {
            m_vecNorItem[j]->FocusItem(false);
        }

        ((CListItemPtr)m_vecNorItem[j])->SetAction(this);
        m_pUSBRecordFramelist->AddItem(m_vecNorItem[j]);
        m_vecNorItem[j]->Relayout();
    }

    if (iCurrentIndex >= nDataIndex
            && iCurrentIndex < nDataIndex + m_pUSBRecordFramelist->GetTotalItemCount())
    {
        m_pUSBRecordFramelist->SetItemFocusByIndex(m_pUSBRecordFramelist->GetIndexByDataIndex(
                    iCurrentIndex));
        m_pUSBRecordFramelist->FocusItemByDataIndex(iCurrentIndex);
    }
    else
    {
        m_pUSBRecordFramelist->SetItemFocusByIndex(0);
    }
    m_pUSBRecordFramelist->setFocus();
}

