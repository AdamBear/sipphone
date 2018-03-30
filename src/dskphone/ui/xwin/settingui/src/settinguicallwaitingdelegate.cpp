#include "settinguicallwaitingdelegate.h"
#include "setuidatadefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/callwaitingcontroller.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiCallWaitingDelegate::CSettinguiCallWaitingDelegate()
{

}

CSettinguiCallWaitingDelegate::~CSettinguiCallWaitingDelegate()
{

}

CSettingUIDelegateBase * CSettinguiCallWaitingDelegate::CreateCallWaitingDelegate()
{
    CSettinguiCallWaitingDelegate * pCallWaitingDelegate = new CSettinguiCallWaitingDelegate;

    return (CSettingUIDelegateBasePtr)pCallWaitingDelegate;
}

CSettingUIPageDataBase * CSettinguiCallWaitingDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        CallWaitingData stCallWaiting;
        if (g_pCallWaitingController->GetCallWaitingData(&stCallWaiting))
        {
            SetComboboxValue(CALLWAITING_ENABLE, stCallWaiting.m_isOn);
            SetComboboxValue(CALLWAITING_PLAYTONE, stCallWaiting.m_isPlayTone);
            SetValue(CALLWAITING_ONCODE, stCallWaiting.m_strOnCode);
            SetValue(CALLWAITING_OFFCODE, stCallWaiting.m_strOffCode);
        }
    }

    return m_pPageData;
}


bool CSettinguiCallWaitingDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettinguiDataDelegateBase::PageDataFilter(pPagedata, bReload);
    //broadsoft开启之后不显示code
    if (1 == configParser_GetConfigInt(kszBroadsoftActive)
#if IF_FEATURE_METASWITCH
            || configParser_GetConfigInt(kszMetaSwitchActive) != 0
#endif
       )
    {
        SetItemShow(pPagedata, CALLWAITING_ONCODE);
        SetItemShow(pPagedata, CALLWAITING_OFFCODE);
    }

    return bResult;
}

bool CSettinguiCallWaitingDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == m_pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    CallWaitingData stCallWaiting;
    stCallWaiting.m_isOn = GetComboboxValue(CALLWAITING_ENABLE);
    stCallWaiting.m_isPlayTone = GetComboboxValue(CALLWAITING_PLAYTONE);
    stCallWaiting.m_strOnCode = GetValue(CALLWAITING_ONCODE);
    stCallWaiting.m_strOffCode = GetValue(CALLWAITING_OFFCODE);

    g_pCallWaitingController->SetCallWaitingData(stCallWaiting);

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);

    return true;
}
