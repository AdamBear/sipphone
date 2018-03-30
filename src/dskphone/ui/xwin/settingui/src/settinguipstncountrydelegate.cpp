#include "settinguipstncountrydelegate.h"

#ifdef IF_FEATURE_PSTN

#include "service_config.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguiradioitemdata.h"
#include "dsklog/log.h"
#include "path_define.h"
#include "xmlparser/xmlparser.hpp"
#include "etlmsghandler/modetlmsghandler.h"
#include "interfacedefine.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
const char * const cszCountryFile = RESOURCE_PATH "system/country.xml";
const char * const cszRootName = "CountryList";
const char * const cszNodeName = "Country";
const char * const cszCountryNameAttr = "name";
const char * const cszCountryCodeAttr = "code";
}

CSetttinguiPSTNCountryDelegate::CSetttinguiPSTNCountryDelegate()
{

}

CSetttinguiPSTNCountryDelegate::~CSetttinguiPSTNCountryDelegate()
{

}

CSettingUIDelegateBase * CSetttinguiPSTNCountryDelegate::CreatePSTNCountryDelegate()
{
    CSetttinguiPSTNCountryDelegate * pDelegate = new CSetttinguiPSTNCountryDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSetttinguiPSTNCountryDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    LoadListCountryItem();

    if (NULL != m_pPageData)
    {
        yl::string strCurrentValue = configParser_GetConfigString(kszCurrentCountry);
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

bool CSetttinguiPSTNCountryDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
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
            configParser_SetConfigString(kszCurrentCountry, strValue.c_str(), CONFIG_LEVEL_PHONE);
        }
    }

    m_bBackToPreviousInterface = bBackToPrevious;
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_SAVING_CONFIG_FILE)
                                           , SETTING_MESSAGEBOX_NOTE, "", "", NS_SET::g_SetPopNormalTime
                                           , -1, (void *)(&m_bBackToPreviousInterface));

    msgSendMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), CONFIG_MSG_BCAST_CHANGED, ST_COUNTRY,
                       0);

    return true;
}

void CSetttinguiPSTNCountryDelegate::LoadListCountryItem()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    xml_document doc;
    if (!doc.load_file(cszCountryFile))
    {
        SETTINGUI_ERR("Cannot load country config file!");
        return ;
    }
    // 查找根节点
    xml_node nodeRoot = doc.child(cszRootName);
    if (!nodeRoot)
    {
        SETTINGUI_ERR("Cannot find root node of country config file!");
        return;
    }
    // 遍历子节点
    for (xml_node nodeChild = nodeRoot.child(cszNodeName); nodeChild;
            nodeChild = nodeChild.next_sibling())
    {
        yl::string strID = nodeChild.attribute(cszCountryCodeAttr).value();
        yl::string strDisplay = nodeChild.attribute(cszCountryNameAttr).value();
        AddRadioListItem(strID, strDisplay);
    }
}

void CSetttinguiPSTNCountryDelegate::AddRadioListItem(const yl::string & strID,
        const yl::string & strName)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIRadioItemDataPtr pItem = new CSettingUIRadioItemData;
    if (NULL != pItem)
    {
        pItem->SetShowIndex(true);
        pItem->SetItemID(strID);
        pItem->SetDisplayName(strName);
        if (!m_pPageData->AddItem(pItem))
        {
            delete pItem;
        }
    }
}

#endif
