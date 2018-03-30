#include "settinguiradioitemdata.h"
#include "configparser/modconfigparser.h"

CSettingUIRadioItemData::CSettingUIRadioItemData()
    : m_bCheck(false)
{
    m_eItemType |= SETTING_ITEM_TYPE_RADIO;
}

CSettingUIRadioItemData::~CSettingUIRadioItemData()
{
}

int CSettingUIRadioItemData::GetRadioDataItemType()
{
    return (SETTING_ITEM_TYPE_RADIO | SETTING_ITEM_TYPE_MENU | SETTING_ITEM_TYPE_BASE);
}

CSettingUIRadioItemData * CSettingUIRadioItemData::GetRadioListDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIRadioItemData::GetRadioDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUIRadioItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUIRadioItemData::CreateRadioListDataItem()
{
    CSettingUIRadioItemDataPtr pDataItem = new CSettingUIRadioItemData();

    return pDataItem;
}

bool CSettingUIRadioItemData::ParserRadioItemData(const xml_node & node,
        CSettingUIItemDataBase * pItemInfo)
{
    CSettingUIRadioItemData * pRadioInfo = CSettingUIRadioItemData::GetRadioListDataItem(pItemInfo);
    if (NULL != pRadioInfo && !node.empty())
    {
        yl::string strCheck = node.attribute("check").as_string();

        pRadioInfo->SetCheck(pRadioInfo->IsCheckByParserItemData(strCheck, pRadioInfo));

        pRadioInfo->SetClickAction(node.attribute("action").as_string());
        pRadioInfo->SetFocusAction(node.attribute("focusaction").as_string());
    }

    return CSettingUIMenuItemData::ParserItemData(node, pItemInfo);
}

bool CSettingUIRadioItemData::IsCheckByParserItemData(const yl::string & strCheck,
        CSettingUIRadioItemData * pRadioInfo)
{
    if (strCheck.empty() || NULL == pRadioInfo)
    {
        return false;
    }

    yl::string strConfigId = "";
    yl::string strValue = "";
    if (!strCheck.empty())
    {
        yl::string::size_type iPos = strCheck.find(':');
        if (iPos != yl::string::npos)
        {
            strConfigId = strCheck.substr(0, iPos);

            if (strCheck.size() > iPos + 1)
            {
                ++iPos;
                strValue = strCheck.substr(iPos, strCheck.size() - iPos);
            }
        }
    }

    if (!strConfigId.empty() && !strValue.empty()
            && strValue == configParser_GetConfigString(strConfigId.c_str()))
    {
        return true;
    }

    return false;
}

bool CSettingUIRadioItemData::IsCheck()
{
    return m_bCheck;
}

void CSettingUIRadioItemData::SetCheck(bool bCheck)
{
    m_bCheck = bCheck;
}

void CSettingUIRadioItemData::Reset()
{
    CSettingUIMenuItemData::Reset();

    m_bCheck = false;
}


