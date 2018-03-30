#include "voicemailplaybasedelegate.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "settingui/include/pageiddefine.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "baseui/include/t2xpicpath.h"
#include "voicemailtimeritemdata.h"
#include "uicallbackfun.h"


CVoiceMailPlayBaseDelegate::CVoiceMailPlayBaseDelegate()
{

}

CVoiceMailPlayBaseDelegate::~CVoiceMailPlayBaseDelegate()
{

}

CSettingUIDelegateBase* CVoiceMailPlayBaseDelegate::CreateVoiceMailPlayBaseDelegate()
{
    CVoiceMailPlayBaseDelegate* pDelegate = new CVoiceMailPlayBaseDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CVoiceMailPlayBaseDelegate::OnTimeOut()
{

}

void CVoiceMailPlayBaseDelegate::LoadVoiceMailPlayBoxItem(CSettingUIPageDataBase* pPageData,
        const yl::string& strTime, int nMaxTicks, bool bShowIcon, bool bShowStatus, int nPlayStatus,
        const yl::string& strID)
{
    if (NULL == pPageData)
    {
        return;
    }

    CVoiceMailTimerItemDataPtr pVoiceMailTimerItemData = new CVoiceMailTimerItemData();
    if (NULL == pVoiceMailTimerItemData)
    {
        return;
    }

    pVoiceMailTimerItemData->SetIconShow(bShowIcon);
    pVoiceMailTimerItemData->SetStatusTextShow(bShowStatus);

    SetVoiceMailPlay(pVoiceMailTimerItemData, nPlayStatus);

    pVoiceMailTimerItemData->SetTotalTimeString(strTime);
    pVoiceMailTimerItemData->SetTimerMaxTicks(nMaxTicks);
    pVoiceMailTimerItemData->SetItemID(strID);

    if (!pPageData->AddItem(pVoiceMailTimerItemData))
    {
        delete pVoiceMailTimerItemData;
        pVoiceMailTimerItemData = NULL;
    }
}

void CVoiceMailPlayBaseDelegate::SetVoiceMailPlayByItemID(const yl::string& strItemID, int nPlayStatus)
{
    CSettingUIItemDataBase* pItemData = GetItemByItemId(strItemID);
    CVoiceMailTimerItemDataPtr pVMPlayItemData = CVoiceMailTimerItemData::GetVoiceMailTimerDataItem(pItemData);

    if (NULL == pVMPlayItemData)
    {
        return;
    }

    SetVoiceMailPlay(pVMPlayItemData, nPlayStatus);
}

void CVoiceMailPlayBaseDelegate::SetVoiceMailPlay(CVoiceMailTimerItemData* pVMItemData, int nPlayStatus)
{
    if (NULL == pVMItemData)
    {
        return;
    }

    switch (nPlayStatus)
    {
    case VMPS_STOP:
        {
            pVMItemData->SetStatusText(_LANG(TRID_STOP));
            pVMItemData->SetStatusIcon(PIC_ICON_PAUSE);
        }
        break;
    case VMPS_PLAY:
        {
            pVMItemData->SetStatusText(_LANG(TRID_PLAY));
            pVMItemData->SetStatusIcon(PIC_ICON_PLAY);
        }
        break;
    case VMPS_PAUSE:
        {
            pVMItemData->SetStatusText(_LANG(TRID_PAUSE));
            pVMItemData->SetStatusIcon(PIC_ICON_PAUSE);
        }
        break;
    case VMPS_RESUME:
        {
            pVMItemData->SetStatusText(_LANG(TRID_PLAY));
            pVMItemData->SetStatusIcon(PIC_ICON_PLAY);
        }
        break;
    default:
        break;
    }

    pVMItemData->SetPlayStatus(nPlayStatus);
    g_SetUICallBackFun.RefreshUI(false);
}

