#include "settinguicallcontrollacdinitialdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

#include "setuidatadefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define VALUE_AVAILABLE "1"
#define VALUE_UNAVAILABLE "0"
}

CSettinguiCallControllACDinitialDelegate::CSettinguiCallControllACDinitialDelegate()
{

}

CSettinguiCallControllACDinitialDelegate::~CSettinguiCallControllACDinitialDelegate()
{

}

CSettingUIDelegateBase * CSettinguiCallControllACDinitialDelegate::CreateACDinitialDelegate()
{
    CSettinguiCallControllACDinitialDelegate * pDelegate = new CSettinguiCallControllACDinitialDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettinguiCallControllACDinitialDelegate::LoadPageData(
    bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        ACD_STATUS eACDInitStatus = ACD_GetInitStatus();
        yl::string strValue = (AS_AVAILABLE ==  eACDInitStatus) ? VALUE_AVAILABLE : VALUE_UNAVAILABLE;
        SetValue(CALLCONTROLL_ACDSTATUSINITIAL, strValue);
    }

    return m_pPageData;
}

bool CSettinguiCallControllACDinitialDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == m_pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    yl::string strValue = GetValue(CALLCONTROLL_ACDSTATUSINITIAL);
    ACD_STATUS eACDInitStatus = (VALUE_UNAVAILABLE == strValue) ? AS_UNAVAILABLE : AS_AVAILABLE;
    ACD_SetInitStatus(eACDInitStatus);
    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);
    return true;
}

#endif
