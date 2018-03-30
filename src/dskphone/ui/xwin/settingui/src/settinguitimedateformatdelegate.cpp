#include "settinguitimedateformatdelegate.h"
#include "setuidatadefine.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/dateandtimecontroller.h"
#include "language/input/include/modInput.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "etlmsghandler/modetlmsghandler.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiTimeDataFormatDelegate::CSettinguiTimeDataFormatDelegate()
{

}

CSettinguiTimeDataFormatDelegate::~CSettinguiTimeDataFormatDelegate()
{

}

CSettingUIDelegateBase * CSettinguiTimeDataFormatDelegate::CreateTimeDataFormatDelegate()
{
    CSettinguiTimeDataFormatDelegate * pDelegate = new CSettinguiTimeDataFormatDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettinguiTimeDataFormatDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    AddDataFormatOptionList();

    DateAndTimeFormatData stData;
    if (g_pDateAndTimeController.GetDateAndTimeFormat(&stData))
    {
        SetComboboxValue(TIME_DATE_FORAMT_TIME, stData.m_eTimeFormat);
        SetComboboxValue(TIME_DATE_FORAMT_DATE, stData.m_eDateFormat);
    }

    return m_pPageData;
}

bool CSettinguiTimeDataFormatDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == m_pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    DateAndTimeFormatData stData;
    stData.m_eTimeFormat = (TimeFormatType)GetComboboxValue(TIME_DATE_FORAMT_TIME);
    stData.m_eDateFormat = (DateFormatType)GetComboboxValue(TIME_DATE_FORAMT_DATE);

    g_pDateAndTimeController.SetDateAndTimeFormat(stData);

    // 刷新时间的显示
    etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);

    return true;
}

void CSettinguiTimeDataFormatDelegate::AddDataFormatOptionList()
{
    CSettingUIComboBoxItemDataPtr pComboxBox = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                m_pPageData->GetItemByItemId(TIME_DATE_FORAMT_DATE));
    if (NULL == pComboxBox)
    {
        return;
    }

    if (Lang_IsChineseStyleDate())
    {
        AddDataFormatOption(DATE_FORMAT_MM_DD_WWW_SPACE, _LANG(TRID_DATE_FORMAT_MM_DD_WWW), pComboxBox);
        AddDataFormatOption(DATE_FORMAT_YY_MM_DD_HYPHEN, _LANG(TRID_DATE_FORMAT_YY_DASH_MM_DASH_DD),
                            pComboxBox);
        AddDataFormatOption(DATE_FORMAT_YYYY_MM_DD, _LANG(TRID_DATE_FORMAT_YYYY_DASH_MM_DASH_DD),
                            pComboxBox);
        AddDataFormatOption(DATE_FORMAT_YYYY_MM_DD_VIRGULE, _LANG(TRID_DATE_FORMAT_YYYY_SLASH_MM_SLASH_DD),
                            pComboxBox);
        AddDataFormatOption(DATE_FORMAT_YY_MM_DD_VIRGULE, _LANG(TRID_DATE_FORMAT_YY_SLASH_MM_SLASH_DD),
                            pComboxBox);
        AddDataFormatOption(DATE_FORMAT_YYYY_MM_DD_SPACE, _LANG(TRID_DATE_FORMAT_YYYY_MM_DD), pComboxBox);
        AddDataFormatOption(DATE_FORMAT_MM_DD_WW_SPACE, _LANG(TRID_DATE_FORMAT_MM_DD_WW), pComboxBox);
    }
    else
    {
        AddDataFormatOption(DATE_FORMAT_WWW_MMM_DD, _LANG(TRID_DATE_FORMAT_WWW_MMM_DD), pComboxBox);
        AddDataFormatOption(DATE_FORMAT_DD_MMM_YY, _LANG(TRID_DATE_FORMATE_DD_DASH_MMM_DASH_YYYY),
                            pComboxBox);
        AddDataFormatOption(DATE_FORMAT_YYYY_MM_DD, _LANG(TRID_DATE_FORMAT_YYYY_DASH_MM_DASH_DD),
                            pComboxBox);
        AddDataFormatOption(DATE_FORMAT_DD_MM_YYYY, _LANG(TRID_DATE_FORMATE_DD_SLASH_MM_SLASH_YYYY),
                            pComboxBox);
        AddDataFormatOption(DATE_FORMAT_MM_DD_YY, _LANG(TRID_DATE_FORMATE_MM_SLASH_DD_SLASH_YY),
                            pComboxBox);
        AddDataFormatOption(DATE_FORMAT_DD_MMM_YYYY, _LANG(TRID_DATE_FORMATE_DD_MMM_YYYY), pComboxBox);
        AddDataFormatOption(DATE_FORMAT_WWW_DD_MMM, _LANG(TRID_DATE_FORMAT_WWW_DD_MMM), pComboxBox);
    }
}

void CSettinguiTimeDataFormatDelegate::AddDataFormatOption(int Index, const yl::string & strValue,
        CSettingUIComboBoxItemData * pComboxBox)
{
    if (NULL == pComboxBox)
    {
        return;
    }
    CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;

    if (NULL == pOption)
    {
        return;
    }

    pOption->m_strDisplayName = strValue;
    pOption->m_strItemVale = commonAPI_FormatString("%d", Index);

    if (!pComboxBox->AddOptionItem(pOption))
    {
        delete pOption;
    }
}
