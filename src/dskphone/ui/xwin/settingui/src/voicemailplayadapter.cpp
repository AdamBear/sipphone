#include "voicemailplayadapter.h"

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
#include "baseui/include/listviewitemfactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "voicemailtimeritem.h"
#include "baseui/include/timerpieceview.h"
#include "voicemailplaybasedelegate.h"
#include "settingui/include/pageiddefine.h"

CVoiceMailPlayAdapter::CVoiceMailPlayAdapter()
    : m_pTimerPiece(NULL)
{
}

CVoiceMailPlayAdapter:: ~CVoiceMailPlayAdapter()
{
}

CSettingUIAdapter* CVoiceMailPlayAdapter::CreateVoiceMailPlayAdapter()

{
    CVoiceMailPlayAdapter* pAdapter = new CVoiceMailPlayAdapter;
    return pAdapter;
}

xListViewItem* CVoiceMailPlayAdapter::PrepareItem(xListView* pListView, int nDataIndex,
        xListViewItem* pItem)

{
    xListViewItemPtr pNewItem = CSettingUIAdapter::PrepareItem(pListView, nDataIndex, pItem);

    if (NULL == m_pSettingUIDelegate)
    {
        return pNewItem;
    }

    CVoiceMailTimerItemPtr pVMTimerItem = CVoiceMailTimerItem::GetVoiceMailTimerItem(pNewItem);

    if (NULL != pVMTimerItem)
    {
        xTimerPieceView* pTimerPiece = pVMTimerItem->GetTimerPiece();
        if (NULL != pTimerPiece)
        {
            SLOT_CONNECT(pTimerPiece, signalTextChanged, this, CVoiceMailPlayAdapter::OnTimerChanged);
            m_pTimerPiece = pTimerPiece;
        }
    }

    return pNewItem;
}

void CVoiceMailPlayAdapter::OnTimerChanged(xSignal* sender, WPARAM wParam,
        LPARAM lParam, int nDataBytes, LPVOID pData)

{
    if (NULL == m_pSettingUIDelegate || NULL == m_pTimerPiece ||
            (
#if IF_FEATURE_METASWITCH_VOICEMAIL
                kszPageMTSWVoiceMailPlay != m_pSettingUIDelegate->GetPageID() &&
#endif
                kszPageIDVoiceMailPlay != m_pSettingUIDelegate->GetPageID()
            ))
    {
        return;
    }

    CVoiceMailPlayBaseDelegate* pVMPlayDelegate = static_cast<CVoiceMailPlayBaseDelegate*>(m_pSettingUIDelegate);
    if (NULL == pVMPlayDelegate)
    {
        return;
    }

    int nDuration = m_pTimerPiece->GetDurationTime();
    int nMaxTick = m_pTimerPiece->GetMaxTick();
    if (nDuration >= nMaxTick)
    {
        pVMPlayDelegate->OnTimeOut();
    }
}
#endif