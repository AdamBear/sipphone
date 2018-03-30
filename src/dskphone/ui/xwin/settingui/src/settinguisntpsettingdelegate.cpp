#include "settinguisntpsettingdelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguilogic/include/settinguicommondata.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/dateandtimecontroller.h"
#include "editcheckmethod.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "baselibs/include/etlmsghandler/modetlmsghandler.h"

namespace
{
#define INDEX_SERVER1 1
#define INDEX_SERVER2 2
#define AutoDSTXmlFileName "AutoDST.xml"
}

CSettingUISNTPSettingDelegate::CSettingUISNTPSettingDelegate()
    : m_bReloadZoneCity(false)
{
    SingleMsgReg(MSG_UPDATE_TIMEZONE, &CSettingUISNTPSettingDelegate::OnTimeZoneChangeMessage);
}

CSettingUISNTPSettingDelegate::~CSettingUISNTPSettingDelegate()
{
    ClearTimeZoneMap();
    SingleMsgUnReg(MSG_UPDATE_TIMEZONE, &CSettingUISNTPSettingDelegate::OnTimeZoneChangeMessage);

}

CSettingUIDelegateBase* CSettingUISNTPSettingDelegate::CreateSNTPSettingDelegate()
{
    CSettingUISNTPSettingDelegate* pDelegate = new CSettingUISNTPSettingDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase* CSettingUISNTPSettingDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    if (!bReLoad)
    {
        if (m_bReloadZoneCity)
        {
            yl::string strValue = GetValue(SNTP_TIME_ZONE);
            AddZoneCityItem(m_pPageData, strValue);
            if (0 == strValue.compare(g_pDateAndTimeController.GetCurrentTimeZone()))
            {
                SetValue(SNTP_NTP_LOCATION, g_pDateAndTimeController.GetCurrentTimeZoneCity());
            }

            m_bReloadZoneCity = false;
        }
        return m_pPageData;
    }

    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    DateTimeGeneralData stData;
    if (!g_pDateAndTimeController.GetGeneralData(&stData))
    {
        m_bReloadZoneCity = false;
        return m_pPageData;
    }

    ClearTimeZoneMap();
    LoadDSTXmlFile(AutoDSTXmlFileName);
    AddZoneCityItem(m_pPageData, stData.m_strTimeZone);

    //设置数据
    SetValue(SNTP_TIME_ZONE, stData.m_strTimeZone);
    SetValue(SNTP_NTP_SERVER_PRI, stData.m_strNtpServer1);
    SetValue(SNTP_NTP_SERVER_SEC, stData.m_strNtpServer2);
    SetValue(SNTP_NTP_LOCATION, stData.m_strTimeZoneCity);
    SetComboboxValue(SNTP_NTP_DAYLIGHTSAVING, stData.m_eDayLightType);

    m_bReloadZoneCity = false;
    return m_pPageData;
}

bool CSettingUISNTPSettingDelegate::SavePageData(CSettingUIPageDataBase* pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    yl::string strNTPServer1 = GetValue(SNTP_NTP_SERVER_PRI);
    if (IsContentEmpty(strNTPServer1) || !IsValidServerName(strNTPServer1))
    {
        return SetValidInfo(_LANG(TRID_NTP_SERVER1_IS_INVALID), INDEX_SERVER1);
    }

    yl::string strNTPServer2 = GetValue(SNTP_NTP_SERVER_SEC);
    if (IsContentEmpty(strNTPServer2) || !IsValidServerName(strNTPServer2))
    {
        return SetValidInfo(_LANG(TRID_NTP_SERVER2_IS_INVALID), INDEX_SERVER2);
    }

    DateTimeGeneralData stData;
    stData.m_eDateTimeType = DATE_AND_TIME_SNTP;
    stData.m_strTimeZone = GetValue(SNTP_TIME_ZONE);
    stData.m_strNtpServer1 = strNTPServer1;
    stData.m_strNtpServer2 = strNTPServer2;
    stData.m_strTimeZoneCity = GetValue(SNTP_NTP_LOCATION);
    stData.m_eDayLightType = (DaylightSavingType)GetComboboxValue(SNTP_NTP_DAYLIGHTSAVING);

    g_pDateAndTimeController.SetGeneralData(stData);

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE_AND_REFRESH);

    return true;
}

bool CSettingUISNTPSettingDelegate::PageDataFilter(CSettingUIPageDataBase* pPagedata,
        bool bReload /* = false */)
{
    bool bResult = CSettinguiDataDelegateBase::PageDataFilter(pPagedata, bReload);
    if (NULL != pPagedata)
    {
        int nValue = GetComboboxValue(SNTP_NTP_DAYLIGHTSAVING);
        if (DAYLIGHTING_SAVING_AUTO != nValue)
        {
            SetItemShow(pPagedata, SNTP_NTP_LOCATION, false);
        }
        else
        {
            SetItemShow(pPagedata, SNTP_NTP_LOCATION, true);
        }
    }

    return bResult;
}

void CSettingUISNTPSettingDelegate::OnComboBoxValueChange(int nIndex)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIComboBoxItemDataPtr pComboxBox = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                m_pPageData->GetItemByDataIndex(nIndex));
    if (NULL == pComboxBox)
    {
        return;
    }

    yl::string strItemID = pComboxBox->GetItemID();
    if (SNTP_TIME_ZONE == strItemID)
    {
        m_bReloadZoneCity = true;
        g_SetUICallBackFun.RefreshUI(false, GetPageID());
    }
    else if (SNTP_NTP_DAYLIGHTSAVING == strItemID)
    {
        g_SetUICallBackFun.RefreshUI(false, GetPageID());
    }

}

void CSettingUISNTPSettingDelegate::AddOption(CSettingUIComboBoxItemData* pComboxBox,
        const yl::string& strContent /* = "None"*/)
{
    if (NULL == pComboxBox)
    {
        return;
    }

    CSettingUIComboBoxOption* pOption = new CSettingUIComboBoxOption;

    if (NULL == pOption)
    {
        return;
    }

    pOption->m_strDisplayName = strContent;
    pOption->m_strItemVale = strContent;

    if (!pComboxBox->AddOptionItem(pOption))
    {
        delete pOption;
    }
}

bool CSettingUISNTPSettingDelegate::SetValidInfo(const yl::string& strNote, int nIndex)
{
    CSettingUIDataValidInfo dataValid;
    dataValid.SetValidNote(strNote);
    dataValid.SetValidIndex(nIndex);
    g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
    return false;
}

bool CompareCity(TimeZoneData* pLeftCity, TimeZoneData* pRightCity)
{
    if (NULL == pLeftCity || NULL == pRightCity)
    {
        return false;
    }
    return strcasecmp(pLeftCity->m_strZone.c_str(), pRightCity->m_strZone.c_str()) < 0;
}
void CSettingUISNTPSettingDelegate::LoadDSTXmlFile(const yl::string& strXmlFileName)
{
    const char * const kszRootDSTData = "DSTData"; // The root node name.
    //const char* const kszNodeNameDST = "DST";  // The DST node name.

#ifdef PHONE_T6X
    yl::string strFullXmlFilePath = "/";
    strFullXmlFilePath = CONFIG_PATH + strFullXmlFilePath + strXmlFileName;
#else
    yl::string strFullXmlFilePath = SETTING_PATH + strXmlFileName;
#endif

    if (!pathFileExist(strFullXmlFilePath.c_str()))
    {
        SETTING_WARN("file not exist [%s]", strFullXmlFilePath.c_str());
        return;
    }

    xml_document fdoc;

    fdoc.load_file(strFullXmlFilePath.c_str());
    xml_node nodeRoot = fdoc.child(kszRootDSTData);
    if (nodeRoot.empty())
    {
        return;
    }
    // Iterate to load child items.
    xml_node nodeDst = nodeRoot.first_child();
    for (; !nodeDst.empty(); nodeDst = nodeDst.next_sibling())
    {
        TimeZoneData* pTimeZoneData = new TimeZoneData();
        if (NULL == pTimeZoneData)
        {
            continue;
        }

        // Reset the struct.
        pTimeZoneData->Reset();
        LoadTimeZoneData(nodeDst, pTimeZoneData);
        //存储数据
        MAP_TIME_ZONE_WITH_CITY::iterator iterPos = m_mapTimeZoneWithCityData.find(pTimeZoneData->m_strTime);
        if (iterPos == m_mapTimeZoneWithCityData.end())
        {
            VEC_TIME_ZONE vecTimeZone;

            vecTimeZone.push_back(pTimeZoneData);
            m_mapTimeZoneWithCityData[pTimeZoneData->m_strTime] = vecTimeZone;
            AddTimeZoneItem(m_pPageData, pTimeZoneData->m_strTime);
        }
        else
        {
            VEC_TIME_ZONE& vecTimeZone = iterPos->second;
            vecTimeZone.push_back(pTimeZoneData);
        }

    }
    MAP_TIME_ZONE_WITH_CITY::iterator iterEnd = m_mapTimeZoneWithCityData.end();
    for (MAP_TIME_ZONE_WITH_CITY::iterator iterNow = m_mapTimeZoneWithCityData.begin(); iterNow != iterEnd; ++iterNow)
    {
        Sort(iterNow->second, CompareCity);
    }

}

void CSettingUISNTPSettingDelegate::AddTimeZoneItem(CSettingUIPageDataBase* pPageData, const yl::string& strTimeZone)
{
    if (NULL == pPageData)
    {
        return;
    }
    CSettingUIItemDataBasePtr pItemData = pPageData->GetItemByItemId(SNTP_TIME_ZONE);
    if (NULL == pItemData)
    {
        return;
    }
    CSettingUIComboBoxItemDataPtr pZoneComboxBox = CSettingUIComboBoxItemData::GetComboBoxDataItem(pItemData);
    if (NULL == pZoneComboxBox)
    {
        return;
    }
    AddOption(pZoneComboxBox, strTimeZone);
}

void CSettingUISNTPSettingDelegate::AddZoneCityItem(CSettingUIPageDataBase* pPageData,
        const yl::string& strTimeZone)
{
    if (NULL == pPageData)
    {
        return;
    }

    CSettingUIItemDataBasePtr pItemData = pPageData->GetItemByItemId(SNTP_NTP_LOCATION);
    if (NULL == pItemData)
    {
        return;
    }

    AddCityOption(pItemData, strTimeZone, true);
}

void CSettingUISNTPSettingDelegate::AddCityOption(CSettingUIItemDataBase* pItemData,
        const yl::string& strTimeZone, bool bAddNone /*= false*/)
{
    CSettingUIComboBoxItemDataPtr pComboxBox = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                pItemData);
    if (NULL == pComboxBox)
    {
        return;
    }

    pComboxBox->ClearOptionItems();

    if (bAddNone)
    {
        AddOption(pComboxBox);
    }
    MAP_TIME_ZONE_WITH_CITY::iterator iterPos = m_mapTimeZoneWithCityData.find(strTimeZone);
    VEC_TIME_ZONE vecTimezone = iterPos->second;
    VEC_TIME_ZONE::iterator it;
    for (it = vecTimezone.begin(); it != vecTimezone.end(); ++it)
    {
        TimeZoneData* pData = *it;
        if (NULL == pData)
        {
            continue;
        }
        yl::string strContent = pData->m_strZone;
        AddOption(pComboxBox, strContent);
    }
}

LRESULT CSettingUISNTPSettingDelegate::OnTimeZoneChangeMessage(msgObject& refMsg)
{
    switch (refMsg.message)
    {
    case MSG_UPDATE_TIMEZONE:
        {
            g_SetUICallBackFun.RefreshUI(true);
            return netSendTimeMsg();
        }
        break;
    default:
        break;
    }

    return FALSE;
}

void CSettingUISNTPSettingDelegate::ClearTimeZoneMap()
{
    MAP_TIME_ZONE_WITH_CITY::iterator iterMap = m_mapTimeZoneWithCityData.begin();
    for (; iterMap != m_mapTimeZoneWithCityData.end(); ++iterMap)
    {
        VEC_TIME_ZONE vecTimezone = iterMap->second;

        for (int i = 0; i < vecTimezone.size(); ++i)
        {
            TimeZoneData* pTimeZone = vecTimezone[i];
            if (NULL != pTimeZone)
            {
                delete pTimeZone;
                pTimeZone = NULL;
            }
        }
        vecTimezone.clear();
    }
    m_mapTimeZoneWithCityData.clear();
}

void CSettingUISNTPSettingDelegate::LoadTimeZoneData(const xml_node& nodeDst, TimeZoneData* pTimeZoneData)
{
    const char * const kszAttributeTimeszTime = "szTime"; // The szTime attribute name.
    const char * const kszAttributeZoneszZone = "szZone"; // The szZone attribute name.
    const char * const kszAttributeTypeiType = "iType"; // The iType attribute name.
    const char * const kszAttributeStartszStart = "szStart"; // The szStart attribute name.
    const char * const kszAttributeEndszEnd = "szEnd"; // The szEND attribute name.
    const char * const kszAttributeOffszOffset = "szOffset"; // The szOffset attribute name.

    yl::string strAttributeName;

    strAttributeName = nodeDst.attribute(kszAttributeTimeszTime).as_string("");
    if (!strAttributeName.empty())
    {
        pTimeZoneData->m_strTime = strAttributeName;
    }

    // Read szZone attribute.
    strAttributeName = nodeDst.attribute(kszAttributeZoneszZone).as_string("");
    if (!strAttributeName.empty())
    {
        pTimeZoneData->m_strZone = strAttributeName;
    }
    // Read iType attribute.
    strAttributeName = nodeDst.attribute(kszAttributeTypeiType).as_string("");
    if (!strAttributeName.empty())
    {
        pTimeZoneData->m_nType = atoi(strAttributeName.c_str());
    }
    // Read szStart attribute.
    strAttributeName = nodeDst.attribute(kszAttributeStartszStart).as_string("");
    if (!strAttributeName.empty())
    {
        pTimeZoneData->m_strBegin = strAttributeName;
    }
    // Read szEnd attribute.
    strAttributeName = nodeDst.attribute(kszAttributeEndszEnd).as_string("");
    if (!strAttributeName.empty())
    {
        pTimeZoneData->m_strEnd = strAttributeName;
    }
    // Read szOffset attribute.
    strAttributeName = nodeDst.attribute(kszAttributeOffszOffset).as_string("");
    if (!strAttributeName.empty())
    {
        pTimeZoneData->m_strOffset = strAttributeName;
    }

}
