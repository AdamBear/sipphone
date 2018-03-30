#ifndef _SETTINGUI_SNTP_SETTING_DELEGATE_H_
#define _SETTINGUI_SNTP_SETTING_DELEGATE_H_

#include "settinguidatadelegatebase.h"
#include "settinguilogic/include/common.h"

typedef YLList<yl::string> LIST_STRING_DATA;
typedef YLVector<TimeZoneData*> VEC_TIME_ZONE;
class CSettingUIComboBoxItemData;

class CSettingUISNTPSettingDelegate : public CSettinguiDataDelegateBase
{
public:
    typedef YLHashMap<yl::string, VEC_TIME_ZONE> MAP_TIME_ZONE_WITH_CITY;
public:
    CSettingUISNTPSettingDelegate();
    virtual ~CSettingUISNTPSettingDelegate();

    static CSettingUIDelegateBase* CreateSNTPSettingDelegate();

    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    void LoadDSTXmlFile(const yl::string& strXmlFileName);
    // 界面数据过滤
    virtual bool PageDataFilter(CSettingUIPageDataBase* pPagedata, bool bReload = false);
    void AddTimeZoneItem(CSettingUIPageDataBase* pPageData, const yl::string& strTimeZone);
    void AddZoneCityItem(CSettingUIPageDataBase* pPageData, const yl::string& strTimeZone);
    void AddCityOption(CSettingUIItemDataBase* pItemData, const yl::string& strTimeZone,
        bool bAddNone = false);
    static LRESULT OnTimeZoneChangeMessage(msgObject& refMsg);
    void ClearTimeZoneMap();
    void LoadTimeZoneData(const xml_node& nodeDst,TimeZoneData* pTimeZoneData);

public:
    // ComboBox值改变响应函数
    virtual void OnComboBoxValueChange(int nIndex);
private: 
    void AddOption(CSettingUIComboBoxItemData* pComboxBox, const yl::string& strContent = "None");
    bool SetValidInfo(const yl::string& strNote, int nIndex);
private:
    bool m_bReloadZoneCity;
    MAP_TIME_ZONE_WITH_CITY  m_mapTimeZoneWithCityData;

};

#endif //_SETTINGUI_SNTP_SETTING_DELEGATE_H_
