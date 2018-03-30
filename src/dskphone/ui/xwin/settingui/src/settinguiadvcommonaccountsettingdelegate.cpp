#include "settinguiadvcommonaccountsettingdelegate.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguiwizardmanager.h"
#include "editcheckmethod.h"
#include "settinguilogic/src/advsettingcontroller.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define ITEM_MAX_LENGTH 99
#define MIN_PROXY_FALLBACK_INTERVAL_NUM 30
#define MAX_PROXY_FALLBACK_INTERVAL_NUM 2147483647

#define SET_IME_ALL "all"
#define SET_IME_PASSWORD "pwd"
#define SET_DEFAULT_2aB "2aB"
#define SET_DEFAULT_PWD_2aB "2aB_PWD"
#define SET_DEFAULT_abc "abc"
#define SET_DEFAULT_PWD_abc "abc_PWD"
#define SET_DEFAULT_123 "123"
}

CAdvCommonAccountSettingDelegate::CAdvCommonAccountSettingDelegate()
{

}

CAdvCommonAccountSettingDelegate::~CAdvCommonAccountSettingDelegate()
{

}

CSettingUIDelegateBase * CAdvCommonAccountSettingDelegate::CreateAdvCommonAccountSettingDelegate()
{
    CAdvCommonAccountSettingDelegate * pDelegate = new CAdvCommonAccountSettingDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CAdvCommonAccountSettingDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData || !bReLoad)
    {
        return m_pPageData;
    }

    LoadCommonAccountPageData(m_pPageData);

    AccountDetailData stData;
    if (GetAccountDetailData(&stData, m_iLineID))
    {
        SetCommonAccountData(stData);
    }

    return m_pPageData;
}

bool CAdvCommonAccountSettingDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable() || !GetShowStatus())
    {
#ifdef IF_SETTING_WIZARD
        if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsInWizard())
        {
            return true;
        }
#endif
        return false;
    }

    CSettingUIDataValidInfo dataValid;
    if (!PageDataValid(pPageData, dataValid))
    {
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        return false;
    }

    AccountDetailData stData;

    GetCommonAccountData(stData);

    SetAccountDetailData(m_iLineID, stData);

    if (bPopHint)
    {
        ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE_AND_REFRESH);
    }

    return true;
}

bool CAdvCommonAccountSettingDelegate::PageDataValid(CSettingUIPageDataBase * pPagedata,
        CSettingUIDataValidInfo & validInfo)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    int nActive = GetComboboxValue(ADV_ACCOUNT_ACTIVATION);
    yl::string strUsrName = GetValue(ADV_ACCOUNT_USER);
    if (1 == nActive && strUsrName.empty())
    {
        return SetInvalidData(validInfo, ADV_ACCOUNT_USER, _LANG(TRID_USER_NAME_IS_INVALID));
    }

    yl::string strSip1 = GetValue(ADV_ACOCUNT_SIP_SERVER1);
    yl::string strSip2 = GetValue(ADV_ACOCUNT_SIP_SERVER2);
    if (1 == nActive && strSip1.empty() && strSip2.empty())
    {
        return SetInvalidData(validInfo, ADV_ACOCUNT_SIP_SERVER1, _LANG(TRID_SIP_PROXY_CANNOT_BE_EMPTY));
    }

    int nOutboundEnable = GetComboboxValue(ADV_ACOCUNT_OUTBOUNDSTATUS);
    yl::string strOutBound1 = GetValue(ADV_ACOCUNT_OUTBOUNDPROXY1);
    yl::string strOutBound2 = GetValue(ADV_ACOCUNT_OUTBOUNDPROXY2);
    if (1 == nOutboundEnable && strOutBound1.empty() && strOutBound2.empty())
    {
        return SetInvalidData(validInfo, ADV_ACOCUNT_OUTBOUNDPROXY1, _LANG(TRID_OUTBOUND_PROXY_IS_INVALID));
    }

    yl::string strProxyFallbackInterval = GetValue(ADV_ACCOUNT_PROXYFALLBACKINTERVAL);
    yl::string strTrim = strProxyFallbackInterval;
    strTrim.trim_both(" ");
    if (strTrim != strProxyFallbackInterval
            || !isInRange(strProxyFallbackInterval, MIN_PROXY_FALLBACK_INTERVAL_NUM,
                          MAX_PROXY_FALLBACK_INTERVAL_NUM))
    {
        return SetInvalidData(validInfo, ADV_ACCOUNT_PROXYFALLBACKINTERVAL,
                              _LANG(TRID_PROXY_FALLBACK_INTERVAL_INVALID));
    }

    return true;
}

bool CAdvCommonAccountSettingDelegate::OnPageAction(int eActionType, int iDataIndex)
{
#ifdef IF_SETTING_WIZARD
    if (SET_ACTION_TYPE_CLICK == eActionType && kszPageIDGuideCommonSipLine == GetPageID())
    {
        return true;
    }
#endif

    return CSettingUIAccountDelegateBase::OnPageAction(eActionType, iDataIndex);
}

bool CAdvCommonAccountSettingDelegate::SetInvalidData(CSettingUIDataValidInfo & validInfo,
        const yl::string& strItemID, const yl::string & strNote)
{
    int nRealIndex = GetItemDataIndexByItemId(strItemID);
    if (-1 == nRealIndex)
    {
        nRealIndex = 0;
    }

    validInfo.SetValidIndex(nRealIndex);
    validInfo.SetValidNote(strNote);
    return false;
}

void CAdvCommonAccountSettingDelegate::LoadCommonAccountPageData(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    LoadComboboxItem(pPageData, ADV_ACCOUNT_ACTIVATION, _LANG(TRID_LINE_ACTIVE), _LANG(TRID_DISABLED),
                     _LANG(TRID_ENABLED), ADV_ACCOUNT_ACTIVATION);
    LoadLineEditItem(pPageData, ADV_ACCOUNT_LABEL, _LANG(TRID_LABEL), ITEM_MAX_LENGTH, SET_DEFAULT_2aB,
                     false, ADV_ACCOUNT_LABEL);
    LoadLineEditItem(pPageData, ADV_ACOCUNT_DISPALY, _LANG(TRID_DISPLAY_NAME), ITEM_MAX_LENGTH,
                     SET_DEFAULT_2aB, false, ADV_ACOCUNT_DISPALY);
    LoadLineEditItem(pPageData, ADV_ACOCUNT_REGISTER, _LANG(TRID_REGISTER_NAME), ITEM_MAX_LENGTH,
                     SET_DEFAULT_2aB, false, ADV_ACOCUNT_REGISTER);
    LoadLineEditItem(pPageData, ADV_ACCOUNT_USER, _LANG(TRID_USER_NAME), ITEM_MAX_LENGTH,
                     SET_DEFAULT_2aB, false, ADV_ACCOUNT_USER);
    LoadLineEditItem(pPageData, ADV_ACOCUNT_PASSWD, _LANG(TRID_PASSWORD), ITEM_MAX_LENGTH,
                     SET_DEFAULT_PWD_abc, true, ADV_ACOCUNT_PASSWD);
    LoadLineEditItem(pPageData, ADV_ACOCUNT_SIP_SERVER1, _LANG(TRID_SIP_SERVER1), ITEM_MAX_LENGTH,
                     SET_DEFAULT_2aB, false, ADV_ACOCUNT_SIP_SERVER);
    LoadLineEditItem(pPageData, ADV_ACOCUNT_SIP_SERVER2, _LANG(TRID_SIP_SERVER2), ITEM_MAX_LENGTH,
                     SET_DEFAULT_2aB, false, ADV_ACOCUNT_SIP_SERVER);
    LoadComboboxItem(pPageData, ADV_ACOCUNT_OUTBOUNDSTATUS, _LANG(TRID_OUTBOUND_STATUS),
                     _LANG(TRID_DISABLED), _LANG(TRID_ENABLED), ADV_ACOCUNT_OUTBOUNDSTATUS);
    LoadLineEditItem(pPageData, ADV_ACOCUNT_OUTBOUNDPROXY1, _LANG(TRID_OUTBOUND_PROXY1),
                     ITEM_MAX_LENGTH, SET_DEFAULT_2aB, false, ADV_ACOCUNT_OUTBOUNDPROXY);
    LoadLineEditItem(pPageData, ADV_ACOCUNT_OUTBOUNDPROXY2, _LANG(TRID_OUTBOUND_PROXY2),
                     ITEM_MAX_LENGTH, SET_DEFAULT_2aB, false, ADV_ACOCUNT_OUTBOUNDPROXY);
    LoadLineEditItem(pPageData, ADV_ACCOUNT_PROXYFALLBACKINTERVAL, _LANG(TRID_FALLBACK_INTERVAL),
                     ITEM_MAX_LENGTH, SET_DEFAULT_123, false);
    LoadComboboxItem(pPageData, ADV_ACCOUNT_NAT_STATUS, _LANG(TRID_NAT_STATUS), _LANG(TRID_DISABLED),
                     _LANG(TRID_STUN));
}

void CAdvCommonAccountSettingDelegate::SetCommonAccountData(const AccountDetailData & stData)
{
    SetComboboxValue(ADV_ACCOUNT_ACTIVATION, stData.m_isActivated);
    SetValue(ADV_ACCOUNT_LABEL, stData.m_strLabel);
    SetValue(ADV_ACOCUNT_DISPALY, stData.m_strDisplayName);
    SetValue(ADV_ACOCUNT_REGISTER, stData.m_strRegisterName);
    SetValue(ADV_ACCOUNT_USER, stData.m_strUserName);
    SetValue(ADV_ACOCUNT_PASSWD, stData.m_strPassword);
    SetValue(ADV_ACOCUNT_SIP_SERVER1, stData.m_strSipServer1);
    SetValue(ADV_ACOCUNT_SIP_SERVER2, stData.m_strSipServer2);
    SetComboboxValue(ADV_ACOCUNT_OUTBOUNDSTATUS, stData.m_isOutboundEnable);
    SetValue(ADV_ACOCUNT_OUTBOUNDPROXY1, stData.m_strOutboundProxy1);
    SetValue(ADV_ACOCUNT_OUTBOUNDPROXY2, stData.m_strOutboundProxy2);
    SetComboboxValue(ADV_ACCOUNT_PROXYFALLBACKINTERVAL, stData.m_nProxyFallbackInterval);
    SetComboboxValue(ADV_ACCOUNT_NAT_STATUS, stData.m_isStunEnable);
}

void CAdvCommonAccountSettingDelegate::GetCommonAccountData(AccountDetailData & stData)
{
    stData.m_isActivated = GetComboboxValue(ADV_ACCOUNT_ACTIVATION);
    stData.m_strLabel = GetValue(ADV_ACCOUNT_LABEL);
    stData.m_strDisplayName = GetValue(ADV_ACOCUNT_DISPALY);
    stData.m_strRegisterName = GetValue(ADV_ACOCUNT_REGISTER);
    stData.m_strUserName = GetValue(ADV_ACCOUNT_USER);
    stData.m_strPassword = GetValue(ADV_ACOCUNT_PASSWD);
    stData.m_strSipServer1 = GetValue(ADV_ACOCUNT_SIP_SERVER1);
    stData.m_strSipServer2 = GetValue(ADV_ACOCUNT_SIP_SERVER2);
    stData.m_isOutboundEnable = GetComboboxValue(ADV_ACOCUNT_OUTBOUNDSTATUS);
    stData.m_strOutboundProxy1 = GetValue(ADV_ACOCUNT_OUTBOUNDPROXY1);
    stData.m_strOutboundProxy2 = GetValue(ADV_ACOCUNT_OUTBOUNDPROXY2);
    stData.m_nProxyFallbackInterval = GetComboboxValue(ADV_ACCOUNT_PROXYFALLBACKINTERVAL);
    stData.m_isStunEnable = GetComboboxValue(ADV_ACCOUNT_NAT_STATUS);
}

void CAdvCommonAccountSettingDelegate::LoadLineEditItem(CSettingUIPageDataBase * pPageData
        , const yl::string & strID
        , const yl::string & strName
        , int nMaxLength
        , const yl::string & strDefaultIme
        , bool bPassword /*= false*/
        , const yl::string & strAuthorid /*= ""*/)
{
    if (NULL == pPageData)
    {
        return;
    }

    yl::string strIme = bPassword ? SET_IME_PASSWORD : SET_IME_ALL;
    AddLineEditItemData(pPageData, strID, strName, "", nMaxLength, strIme, strDefaultIme, bPassword,
                        strAuthorid);
}

void CAdvCommonAccountSettingDelegate::LoadComboboxItem(CSettingUIPageDataBase * pPageData
        , const yl::string & strID
        , const yl::string & strName
        , const yl::string strValue1
        , const yl::string strValue2
        , const yl::string & strAuthorid /*= ""*/)
{
    if (NULL == pPageData)
    {
        return;
    }

    CSettingUIComboBoxItemDataPtr pComboboxItem = new CSettingUIComboBoxItemData;
    if (NULL != pComboboxItem)
    {
        pComboboxItem->SetItemID(strID);
        pComboboxItem->SetAuthorityID(strAuthorid);
        pComboboxItem->SetDisplayName(strName);
        pComboboxItem->SetShowIndex(true);

        for (int i = 0; i < 2; ++i)
        {
            CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;

            if (NULL == pOption)
            {
                continue;
            }

            pOption->m_strDisplayName = (0 == i) ? strValue1 : strValue2;
            pOption->m_strItemVale = commonAPI_FormatString("%d", i);

            if (!pComboboxItem->AddOptionItem(pOption))
            {
                delete pOption;
            }
        }

        if (!pPageData->AddItem(pComboboxItem))
        {
            delete pComboboxItem;
        }
    }
}
