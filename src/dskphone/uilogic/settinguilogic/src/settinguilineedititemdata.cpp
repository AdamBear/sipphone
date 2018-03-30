#include "settinguilineedititemdata.h"
#include "baseui/include/inputmethod.h"
#include <settinguilogic/include/settingdefine.h>
#include "commonapi/stringhelperapi.h"

namespace
{
const char * SET_IME_ALL = "all";
const char * SET_IME_DIAL = "dial";
const char * SET_IME_NUMBER = "number";
const char * SET_IME_LOWER = "lower";
const char * SET_IME_UPPER = "upper";
const char * SET_IME_DIGIT = "digit";
const char * SET_IME_NAME = "name";
const char * SET_IME_IP = "123_IP";
const char * SET_IME_PASSWORD = "pwd";
const char * SET_IME_STARONLYDIAL = "staronlydial";
const char * SET_IME_DIAL_2aB = "dial_2aB";
const char * SET_IME_CUSTOM = "custom";

const char * SET_DEFAULT_2aB = "2aB";
const char * SET_DEFAULT_abc = "abc";
const char * SET_DEFAULT_123 = "123";
const char * SET_DEFAULT_123_Dial = "123_Dial";
const char * SET_DEFAULT_Abc = "Abc";
const char * SET_DEFAULT_DIY = "diy";
const char * SET_DEFAULT_ABC = "ABC";
const char * SET_DEFAULT_123_IP = "123_IP";
const char * SET_DEFAULT_PWD_2aB = "2aB_PWD";
const char * SET_DEFAULT_PWD_abc = "abc_PWD";
const char * SET_DEFAULT_PWD_ABC = "ABC_PWD";
const char * SET_DEFAULT_PWD_123 = "123_PWD";
}

CSettingUILineEditItemData::CSettingUILineEditItemData()
    : CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE)
    , m_iMaxLength(-1)
    , m_nMinValue(-1)
    , m_nMaxValue(-1)
    , m_bPassword(false)
    , m_bAllowEmpty(false)
    , m_bDeleteSoftkey(true)
    , m_nDeleteSoftkeyIndex(-1)
    , m_strIME("")
    , m_strDefaultIME("")
    , m_strRE("")
    , m_strCheckBaseID("")
    , m_strCheckBaseValue("")
    , m_strHintText("")
    , m_strCheckMethodType("")
{
    m_eItemType |= SETTING_ITEM_TYPE_LINEEDIT;
}

CSettingUILineEditItemData::~CSettingUILineEditItemData()
{
}

int CSettingUILineEditItemData::GetLineEditDataItemType()
{
    return (SETTING_ITEM_TYPE_LINEEDIT | SETTING_ITEM_TYPE_BASE);
}

CSettingUILineEditItemData * CSettingUILineEditItemData::GetLineEditDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUILineEditItemData::GetLineEditDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUILineEditItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUILineEditItemData::CreateLineEditDataItem()
{
    CSettingUILineEditItemDataPtr pInputDataItem = new CSettingUILineEditItemData();

    return pInputDataItem;
}

bool CSettingUILineEditItemData::ParserLineEditItemData(const xml_node & node,
        CSettingUIItemDataBase * pItemInfo)
{
    CSettingUILineEditItemData * pInputInfo = CSettingUILineEditItemData::GetLineEditDataItem(
                pItemInfo);
    if (NULL != pInputInfo && !node.empty())
    {
        pInputInfo->SetPassword(0 == strcmp(node.attribute("password").as_string(), "yes"));
        pInputInfo->SetIME(node.attribute("ime").as_string());
        pInputInfo->SetDefaultIME(node.attribute("defaultime").as_string());
        pInputInfo->SetMaxLength(node.attribute("maxlength").as_int(-1));
        pInputInfo->SetRegularExpressions(node.attribute("regex").as_string());
#if IF_SETTING_FILTER_DEBUG
        yl::string strTmp = node.attribute("regex").as_string();
        printf("------- regex node[%s], set[%s] =====\n"
               , strTmp.c_str(), pInputInfo->GetRegularExpressions().c_str());
#endif
        pInputInfo->SetValueRange(node.attribute("range").as_string());
        //
        yl::string strAllowemptyvalue = node.attribute("allowemptyvalue").as_string();
        if ("yes" == strAllowemptyvalue)
        {
            pInputInfo->SetAllowEmpty(true);
        }
        else
        {
            pInputInfo->SetAllowEmpty(false);
        }

        yl::string strValidateSwitchInfo = node.attribute("validateSwitch").as_string();
        if (!strValidateSwitchInfo.empty())
        {
            unsigned int iPos = strValidateSwitchInfo.find(':');
            if (yl::string::npos != iPos)
            {
                pInputInfo->SetCheckBaseID(strValidateSwitchInfo.substr(0, iPos).trim_both());
                if (strValidateSwitchInfo.length() - 1 != iPos)
                {
                    pInputInfo->SetCheckBaseValue(strValidateSwitchInfo.substr(iPos + 1,
                                                  strValidateSwitchInfo.length() - iPos - 1));
                }
            }
        }
        pInputInfo->SetHintText(node.attribute("hint").as_string());
        pInputInfo->SetCheckMethodType(node.attribute("checkmethodtype").as_string());
    }

    return CSettingUIItemDataBase::ParserItemData(node, pItemInfo);
}

bool CSettingUILineEditItemData::SetItemData(const CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUILineEditItemData::GetItemData(CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUILineEditItemData::LoadItemData(CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUILineEditItemData::SaveItemData(const CSettingUIItemDataBase & itemDatas)
{
    return true;
}

int CSettingUILineEditItemData::GetMaxLength()
{
    return m_iMaxLength;
}

void CSettingUILineEditItemData::SetMaxLength(int iMaxLength)
{
    // if eItemType valid
    m_iMaxLength = iMaxLength;
}

bool CSettingUILineEditItemData::IsAllowEmpty()
{
    return m_bAllowEmpty;
}

void CSettingUILineEditItemData::SetAllowEmpty(bool bAllowEmpty)
{
    m_bAllowEmpty = bAllowEmpty;
}

int CSettingUILineEditItemData::GetMinValue()
{
    return m_nMinValue;
}

void CSettingUILineEditItemData::SetMinValue(int nMinValue)
{
    m_nMinValue = nMinValue;
}

int CSettingUILineEditItemData::GetMaxValue()
{
    return m_nMaxValue;
}

void CSettingUILineEditItemData::SetMaxValue(int nMaxValue)
{
    m_nMaxValue = nMaxValue;
}

bool CSettingUILineEditItemData::IsPassword()
{
    return m_bPassword;
}

void CSettingUILineEditItemData::SetPassword(bool bPassword)
{
    m_bPassword = bPassword;
}

const yl::string & CSettingUILineEditItemData::GetIME()
{
    return m_strIME;
}

void CSettingUILineEditItemData::SetIME(const yl::string & strIME)
{
    m_strIME = SettingIMEType2InputIMEType(strIME);
}

const yl::string & CSettingUILineEditItemData::GetDefaultIME()
{
    return m_strDefaultIME;
}

void CSettingUILineEditItemData::SetDefaultIME(const yl::string& strDefaultIME, bool bConfig /*= true */)
{
    if (bConfig)
    {
        m_strDefaultIME = SettingDefaultType2InputType(strDefaultIME);
    }
    else
    {
        m_strDefaultIME = strDefaultIME;
    }
}

void CSettingUILineEditItemData::SetDefaultIMEByLogicData(const yl::string& strDefaultIme, bool bPassword)
{
    if (strDefaultIme.empty())
    {
        return;
    }

    if (!bPassword)
    {
        m_strDefaultIME = strDefaultIme;
        return;
    }

    if (SET_DEFAULT_2aB == strDefaultIme || SET_DEFAULT_PWD_2aB == strDefaultIme)
    {
        m_strDefaultIME = SET_DEFAULT_PWD_2aB;
    }
    else if (SET_DEFAULT_abc == strDefaultIme || SET_DEFAULT_PWD_abc == strDefaultIme)
    {
        m_strDefaultIME = SET_DEFAULT_PWD_abc;
    }
    else if (SET_DEFAULT_123 == strDefaultIme || SET_DEFAULT_PWD_123 == strDefaultIme)
    {
        m_strDefaultIME = SET_DEFAULT_PWD_123;
    }
    else if (SET_DEFAULT_ABC == strDefaultIme || SET_DEFAULT_PWD_ABC == strDefaultIme)
    {
        m_strDefaultIME = SET_DEFAULT_PWD_ABC;
    }
    else
    {
        m_strDefaultIME = strDefaultIme;
    }
}

const yl::string & CSettingUILineEditItemData::GetRegularExpressions()
{
    return m_strRE;
}

void CSettingUILineEditItemData::SetRegularExpressions(const yl::string & strRE)
{
#if IF_SETTING_FILTER_DEBUG
    printf("------------ set regex be[%s], af[%s] =========\n"
           , m_strRE.c_str(), strRE.c_str());
#endif
    m_strRE = strRE;
}

const yl::string & CSettingUILineEditItemData::GetCheckBaseID()
{
    return m_strCheckBaseID;
}

void CSettingUILineEditItemData::SetCheckBaseID(const yl::string & strCheckBaseID)
{
    m_strCheckBaseID = strCheckBaseID;
}

const yl::string & CSettingUILineEditItemData::GetCheckBaseValue()
{
    return m_strCheckBaseValue;
}

void CSettingUILineEditItemData::SetCheckBaseValue(const yl::string & strCheckBaseValue)
{
    m_strCheckBaseValue = strCheckBaseValue;
}

const yl::string & CSettingUILineEditItemData::GetHintText()
{
    return m_strHintText;
}

void CSettingUILineEditItemData::SetHintText(const yl::string & strHintText)
{
    m_strHintText = strHintText;
}

const yl::string & CSettingUILineEditItemData::GetCheckMethodType()
{
    return m_strCheckMethodType;
}

void CSettingUILineEditItemData::SetCheckMethodType(const yl::string & strCheckMethodType)
{
    m_strCheckMethodType = strCheckMethodType;
}

bool CSettingUILineEditItemData::IsDeleteSoftkeyEnable()
{
    return m_bDeleteSoftkey;
}

void CSettingUILineEditItemData::SetDeleteSoftkeyEnable(bool bEnable)
{
    m_bDeleteSoftkey = bEnable;
}

int CSettingUILineEditItemData::GetDeleteSoftkeyIndex()
{
    return m_nDeleteSoftkeyIndex;
}

void CSettingUILineEditItemData::SetDeleteSoftkeyIndex(int nIndex)
{
    m_nDeleteSoftkeyIndex = nIndex;
}

yl::string CSettingUILineEditItemData::SettingIMEType2InputIMEType(const yl::string & strSetType)
{
    yl::string strIme(IME_ALL);
    if (SET_IME_DIAL == strSetType)
    {
        strIme = IME_DIAL;
    }
    else if (SET_IME_NUMBER == strSetType)
    {
        strIme = IME_Number;
    }
    else if (SET_IME_LOWER == strSetType)
    {
        strIme = IME_LOWER;
    }
    else if (SET_IME_UPPER == strSetType)
    {
        strIme = IME_UPPER;
    }
    else if (SET_IME_DIGIT == strSetType)
    {
        strIme = IME_DIGIT;
    }
    else if (SET_IME_NAME == strSetType)
    {
        strIme = IME_Name;
    }
    else if (SET_IME_IP == strSetType)
    {
        strIme = IME_IP;
    }
    else if (SET_IME_PASSWORD == strSetType)
    {
        strIme = IME_PWD;
    }
    else if (SET_IME_STARONLYDIAL == strSetType)
    {
        strIme = IME_STAR_ONLY_DIAL;
    }
    else if (SET_IME_DIAL_2aB == strSetType)
    {
        strIme = IME_DIAL_2AB;
    }
    else if (SET_IME_CUSTOM == strSetType)
    {
        strIme = IME_CUSTOM;
    }
    else if (SET_DEFAULT_PWD_123 == strSetType)
    {
        strIme = IME_123_Pwd;
    }

    return strIme;
}

yl::string CSettingUILineEditItemData::SettingDefaultType2InputType(const yl::string & strSetType)
{
    yl::string strDefaultIme(IME_abc);
    if (SET_DEFAULT_2aB == strSetType)
    {
        strDefaultIme = IME_2aB;
    }
    if (SET_DEFAULT_abc == strSetType)
    {
        strDefaultIme = IME_abc;
    }
    else if (SET_DEFAULT_123 == strSetType)
    {
        strDefaultIme = IME_123;
    }
    else if (SET_DEFAULT_123_Dial == strSetType)
    {
        strDefaultIme = IME_123_Dial;
    }
    else if (SET_DEFAULT_DIY == strSetType)
    {
        strDefaultIme = IME_DIY;
    }
    else if (SET_DEFAULT_ABC == strSetType)
    {
        strDefaultIme = IME_ABC;
    }
    else if (SET_DEFAULT_123_IP == strSetType)
    {
        strDefaultIme = IME_123_IP;
    }
    else if (SET_DEFAULT_PWD_2aB == strSetType)
    {
        strDefaultIme = IME_2aB_Pwd;
    }
    else if (SET_DEFAULT_PWD_abc == strSetType)
    {
        strDefaultIme = IME_abc_Pwd;
    }
    else if (SET_DEFAULT_PWD_ABC == strSetType)
    {
        strDefaultIme = IME_ABC_Pwd;
    }
    else if (SET_DEFAULT_PWD_123 == strSetType)
    {
        strDefaultIme = IME_123_Pwd;
    }

    return strDefaultIme;
}

void CSettingUILineEditItemData::SetValueRange(const yl::string & strValueRange)
{
    if (!strValueRange.empty())
    {
        int nMin = -1;
        int nMax = -1;
        yl::string strRange = commonAPI_TrimString(strValueRange, "[");
        strRange = commonAPI_TrimString(strRange, "]");
        strRange = commonAPI_replaceSubStringByTag(strRange, " ", "");
        sscanf(strRange.c_str(), "%d, %d", &nMin, &nMax);
        SetMinValue(nMin);
        SetMaxValue(nMax);
    }
}
