#include "settinguipstnflashtimedelegate.h"

#ifdef IF_FEATURE_PSTN

#include "service_config.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguiradioitemdata.h"
#include "etlmsghandler/modetlmsghandler.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSetttinguiPSTNFlashTimeDelegate::CSetttinguiPSTNFlashTimeDelegate()
{

}

CSetttinguiPSTNFlashTimeDelegate::~CSetttinguiPSTNFlashTimeDelegate()
{

}

CSettingUIDelegateBase * CSetttinguiPSTNFlashTimeDelegate::CreatePSTNFlashTimeDelegate()
{
    CSetttinguiPSTNFlashTimeDelegate * pDelegate = new CSetttinguiPSTNFlashTimeDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSetttinguiPSTNFlashTimeDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        yl::string strCurrentValue = configParser_GetConfigString(kszFlashDuration);
        CSettingUIRadioItemDataPtr pItemData = (CSettingUIRadioItemData *)m_pPageData->GetItemByItemId(
                strCurrentValue);
        if (NULL == pItemData)
        {
            //找不到默认为第0个
            pItemData = (CSettingUIRadioItemData *)m_pPageData->GetItemByDataIndex(0);
        }
        if (NULL != pItemData)
        {
            pItemData->SetCheck(true);
            m_pPageData->SetFocusIndex(m_pPageData->GetItemDataIndexByItemId(pItemData->GetItemID()));
        }
    }

    return m_pPageData;
}

bool CSetttinguiPSTNFlashTimeDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    CSettingUIItemDataBasePtr pItem = pPageData->GetItemByDataIndex(pPageData->GetFocusIndex());
    if (NULL != pItem)
    {
        yl::string strValue = pItem->GetItemID();
        if (!strValue.empty())
        {
            configParser_SetConfigString(kszFlashDuration, strValue.c_str(), CONFIG_LEVEL_PHONE);
        }
    }

    m_bBackToPreviousInterface = bBackToPrevious;
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_SAVING_CONFIG_FILE)
                                           , SETTING_MESSAGEBOX_NOTE, "", "", NS_SET::g_SetPopNormalTime
                                           , -1, (void *)(&m_bBackToPreviousInterface));

    etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_FLASH_DURATION, 0);

    return true;
}

#endif
