#include "settinguixsipassworddelegate.h"
#include "uicallbackfun.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "commonunits/modcommonunits.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define EXTRA_DATA_STRING_COUNT 2
#define INDEX_PASSWORD 1
#define STR_VALUE_SPLIT_TOKEN "+"
}

CSettinguiXSIPasswordDelegate::CSettinguiXSIPasswordDelegate()
    : m_strNextPage("")
{

}

CSettinguiXSIPasswordDelegate::~CSettinguiXSIPasswordDelegate()
{

}

CSettingUIDelegateBase * CSettinguiXSIPasswordDelegate::CreateXSIPasswordDelegate()
{
    CSettinguiXSIPasswordDelegate * pDelegate = new CSettinguiXSIPasswordDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CSettinguiXSIPasswordDelegate::InitExtraData(void * pData)
{
    yl::string strExtraData = "";
    yl::string * pstrData = (yl::string *)pData;
    if (NULL != pstrData)
    {
        strExtraData = *pstrData;
    }

    //格式为 界面名称+ID
    if (!strExtraData.empty())
    {
        VEC_STRING_DATA vecStrValue = yl::string_split<yl::string::value_type>(strExtraData,
                                      STR_VALUE_SPLIT_TOKEN, true);
        if (vecStrValue.size() == EXTRA_DATA_STRING_COUNT)
        {
            m_strNextPage = vecStrValue[0];
            m_iLineID = atoi(vecStrValue[1].c_str());
        }
    }
}

CSettingUIPageDataBase * CSettinguiXSIPasswordDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    bool bIsAllowSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

    CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                m_pPageData->GetItemByItemId(XSI_PASSWORD));
    if (NULL != pEditItem)
    {
        yl::string strDisp = bIsAllowSipAuth ? TRID_SIP_PASSWORD : TRID_PASSWORD;
        pEditItem->SetDisplayName(_LANG(strDisp));
#if IF_FEATURE_AUTOP_IME
        yl::string strImeType = commonUnits_GetIMEByType(kszXsiPasswordIME);
        pEditItem->SetDefaultIME(strImeType);
#endif
    }

    CSettingUIItemDataBasePtr pUserId = m_pPageData->GetItemByItemId(XSI_USERID);
    if (NULL != pUserId)
    {
        yl::string strValue = "";
        yl::string strDisplayName = "";

        if (bIsAllowSipAuth)
        {
            strValue = configParser_GetCfgItemStringValue(kszAccountRegisterName, m_iLineID);
            strDisplayName = _LANG(TRID_SIP_REGISTER_NAME);
        }
        else
        {
            strValue = configParser_GetCfgItemStringValue(kszAccountXSIUserID, m_iLineID);
            strDisplayName = _LANG(TRID_USER_ID);
        }

        pUserId->SetValue(strValue);
        pUserId->SetDisplayName(strDisplayName);
    }

    yl::string strTitle = bIsAllowSipAuth ? TRID_PLEASE_ENTER_YOUR_SIP_CREDENTIAL_PASSWROD :
                          TRID_INPUT_YOUR_WEB_ACCESS_PASSWORD;
    m_pPageData->SetPageTitle(_LANG(strTitle));

    return m_pPageData;
}

bool CSettinguiXSIPasswordDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    yl::string strPassword = "";

    CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                m_pPageData->GetItemByItemId(XSI_PASSWORD));
    if (NULL != pEditItem)
    {
        strPassword = pEditItem->GetValue();
    }

    //密码不能为空
    if (strPassword.empty())
    {
        CSettingUIDataValidInfo validInfo;
        validInfo.SetValidIndex(INDEX_PASSWORD);
        validInfo.SetValidNote(_LANG(TRID_PASSWORD_CANNOT_BE_EMPTY));
        g_SetUICallBackFun.DealWithValidData(validInfo);
        return false;
    }

    BWCallControl_SetAccountXSIPassword(strPassword.c_str(), m_iLineID);

    yl::string strPageID = m_strNextPage;
    int nAccountId = m_iLineID;
    g_SetUICallBackFun.FallBackToPrevousPage();

    if (!strPageID.empty())
    {
        PageOpenData opPageData(strPageID);
        opPageData.pExtraData = (void *)&nAccountId;
        g_SetUICallBackFun.EnterPage(opPageData);

    }
    return true;
}
