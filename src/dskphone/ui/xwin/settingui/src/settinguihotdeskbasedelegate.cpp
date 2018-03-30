#include "settinguihotdeskbasedelegate.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguilogic/src/advsettingcontroller.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "translateiddefine.h"
#include <configiddefine.h>
#include "settinguimanager.h"

#include "languagehelper/modlangtranslate.h"
#include "configparser/modconfigparser.h"
#include "messagebox/modmessagebox.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "account/include/modaccount.h"


#ifdef IF_FEATURE_HOTDESKING

namespace
{
#define ITEM_MAX_LENGTH 99
#define ITEM_URL_MAX_LENGTH 512

#define INDEX_USERNAME 1
#define INDEX_SIP_PROXY 3
#define INDEX_OUTBOUND 9

#define SET_IME_ALL "all"
#define SET_IME_PASSWORD "pwd"

#define SET_DEFAULT_2aB "2aB"
#define SET_DEFAULT_abc_PWD "abc_PWD"
#define SET_DEFAULT_123 "123"

#define NOTE_MSGBOX_SHOW_TIME_LOGINING      10 * 1000
#define MSGBOX_ID_HOTDESK_LOGINING          101
#define MSGBOX_ID_HOTDESK_LOGIN_FAILED      102
#define INCREASE_TWO_ITEMS                  2
}

CHotDeskBaseSettingDelegate::CHotDeskBaseSettingDelegate()
    : m_bRegisterName(false)
    , m_bUserName(false)
    , m_bPassword(false)
    , m_bServer(false)
    , m_bOutBound(false)
    , m_bAutopUrl(false)
    , m_eHotdeskType(HDT_BASE)
    , m_nIndexRegisterName(-1)
    , m_nIndexUserName(-1)
    , m_nIndexPassword(-1)
    , m_nIndexServer(-1)
    , m_nIndexOutBound(-1)
    , m_nIndexAutopUrl(-1)
{
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                          , &CHotDeskBaseSettingDelegate::OnMessage);
}

CHotDeskBaseSettingDelegate::~CHotDeskBaseSettingDelegate()
{
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                            , &CHotDeskBaseSettingDelegate::OnMessage);
}

CSettingUIDelegateBase * CHotDeskBaseSettingDelegate::CreateHotDeskBaseSettingDelegate()
{
    CHotDeskBaseSettingDelegate * pDelegate = new CHotDeskBaseSettingDelegate;

    return static_cast<CSettingUIDelegateBasePtr>(pDelegate);
}

CSettingUIPageDataBase * CHotDeskBaseSettingDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (HDT_LOGON_WIZRAD == m_eHotdeskType || HDT_LOGON_WIZRAD_WITH_DATA == m_eHotdeskType)
    {
#ifndef _WIN32
#warning
#endif
//      m_pPageData->SetPageTitle(TRID_SET_HOT_DESKING);
    }
    else
    {
        m_pPageData->SetPageTitle(TRID_HOT_DESKING);
    }

    if (NULL == m_pPageData || !bReLoad)
    {
        return m_pPageData;
    }

    if (NULL != m_pPageData)
    {
        LoadCommonAccountPageData(m_pPageData);
    }

    return m_pPageData;
}

bool CHotDeskBaseSettingDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    CSettingUIDataValidInfo dataValid;
    if (!PageDataValid(pPageData, dataValid))
    {
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        return false;
    }

    HotDeskBaseData stHotDeskData;

    GetHotDeskData(stHotDeskData);

    SetHotDeskBaseData(stHotDeskData);

    if (0 == configParser_GetConfigInt(kszExitWizardAfterLogin))
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE),
                                               _LANG(TRID_REGISTERING_PLEASE_WAIT)
                                               , SETTING_MESSAGEBOX_NOTE, "", "", NOTE_MSGBOX_SHOW_TIME_LOGINING, MSGBOX_ID_HOTDESK_LOGINING, NULL,
                                               this);
    }
    else
    {
        ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE_AND_REFRESH);
    }

    return true;
}

bool CHotDeskBaseSettingDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        eActionType = SET_ACTION_TYPE_SAVE;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

bool CHotDeskBaseSettingDelegate::PageDataValid(CSettingUIPageDataBase * pPagedata,
        CSettingUIDataValidInfo & validInfo)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    yl::string strRegisterName = GetValue(HOTDESKBASE_AUTH);
    if (m_bRegisterName && strRegisterName.empty())
    {
        return SetInvalidData(validInfo, m_nIndexRegisterName, _LANG(TRID_REGISTER_NAME_IS_INVALID));
    }

    yl::string strUsrName = GetValue(HOTDESKBASE_USER);
    if (m_bUserName && strUsrName.empty())
    {
        return SetInvalidData(validInfo, m_nIndexUserName, _LANG(TRID_USER_NAME_IS_INVALID));
    }

    yl::string strPassword = GetValue(HOTDESKBASE_PWD);
    if (m_bPassword && strPassword.empty() && 0 == configParser_GetConfigInt(kszEnablePwdEmpty))
    {
        return SetInvalidData(validInfo, m_nIndexPassword, _LANG(TRID_PASSWORD_CANNOT_BE_EMPTY));
    }

    yl::string strSip1 = GetValue(HOTDESKBASE_SERVER1);
    yl::string strSip2 = GetValue(HOTDESKBASE_SERVER2);
    if (m_bServer && strSip1.empty() && strSip2.empty())
    {
        return SetInvalidData(validInfo, m_nIndexServer, _LANG(TRID_SIP_PROXY_CANNOT_BE_EMPTY));
    }

    yl::string strOutBound1 = GetValue(HOTDESKBASE_OUTBOUND1);
    yl::string strOutBound2 = GetValue(HOTDESKBASE_OUTBOUND2);
    if (m_bOutBound && strOutBound1.empty() && strOutBound2.empty())
    {
        return SetInvalidData(validInfo, m_nIndexOutBound, _LANG(TRID_OUTBOUND_PROXY_IS_INVALID));
    }

#if IF_BUG_22024_LogonWizard_AutopURL
    LoginWizardAutopRULType eAutopUrl = (LoginWizardAutopRULType)GetComboboxValue(HOTDESKBASE_AUTOPURL);
    if (m_bAutopUrl && LW_AUTOP_URL_NONE == eAutopUrl)
    {
        return SetInvalidData(validInfo, m_nIndexAutopUrl, _LANG(TRID_AUTOP_URL_CAN_NOT_BE_EMPTY));
    }
#endif

    return true;
}

bool CHotDeskBaseSettingDelegate::SetInvalidData(CSettingUIDataValidInfo & validInfo, int nIndex,
        const yl::string & strNote)
{
    int nRealIndex = nIndex;
    validInfo.SetValidIndex(nRealIndex);
    validInfo.SetValidNote(strNote);
    return false;
}

void CHotDeskBaseSettingDelegate::LoadCommonAccountPageData(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    yl::string strIme = SET_IME_ALL;
    yl::string strPwdIme = SET_IME_PASSWORD;

    yl::string strDefaultIme = SET_DEFAULT_2aB;
    yl::string strDefaultUserIme = SET_DEFAULT_2aB;
    yl::string strDefaultPwdIme = SET_DEFAULT_abc_PWD;

#if IF_FEATURE_AUTOP_IME
    strDefaultIme = commonUnits_GetIMEByType(kszHotDeskingRegisterIME);
    strDefaultUserIme = commonUnits_GetIMEByType(kszHotDeskingUserNameIME);
    strDefaultPwdIme = commonUnits_GetIMEByType(kszHotDeskingPasswordIME);
#endif

    int nIndex = 0;

    if (m_bRegisterName)
    {
        m_nIndexRegisterName = nIndex;
        LoadLineEditItem(pPageData, HOTDESKBASE_AUTH, _LANG(TRID_REGISTER_NAME), ITEM_MAX_LENGTH, strIme,
                         strDefaultIme, false, HOTDESKBASE_AUTH);
        nIndex++;
    }

    if (m_bUserName)
    {
        m_nIndexUserName = nIndex;
        LoadLineEditItem(pPageData, HOTDESKBASE_USER, _LANG(TRID_USER_NAME), ITEM_MAX_LENGTH, strIme,
                         strDefaultUserIme, false, HOTDESKBASE_USER);
        if (HDT_BASE_WITH_DATA == m_eHotdeskType || HDT_LOGON_WIZRAD_WITH_DATA == m_eHotdeskType)
        {
            SetValue(HOTDESKBASE_USER, configParser_GetCfgItemStringValue(kszAccountRegisterName, 0));
        }
        nIndex++;
    }

    if (m_bPassword)
    {
        m_nIndexPassword = nIndex;
        LoadLineEditItem(pPageData, HOTDESKBASE_PWD, _LANG(TRID_PASSWORD), ITEM_MAX_LENGTH, strPwdIme,
                         strDefaultPwdIme, true, HOTDESKBASE_PWD);
        if (HDT_BASE_WITH_DATA == m_eHotdeskType || HDT_LOGON_WIZRAD_WITH_DATA == m_eHotdeskType)
        {
            SetValue(HOTDESKBASE_PWD, configParser_GetCfgItemStringValue(kszAccountPassword, 0));
        }
        nIndex++;
    }

    if (m_bServer)
    {
        m_nIndexServer = nIndex;
        LoadLineEditItem(pPageData, HOTDESKBASE_SERVER1, _LANG(TRID_SIP_SERVER1), ITEM_MAX_LENGTH, strIme,
                         strDefaultIme, false, HOTDESKBASE_SERVER1);
        LoadLineEditItem(pPageData, HOTDESKBASE_SERVER2, _LANG(TRID_SIP_SERVER2), ITEM_MAX_LENGTH, strIme,
                         strDefaultIme, false, HOTDESKBASE_SERVER2);
        nIndex = nIndex + INCREASE_TWO_ITEMS;
    }

    if (m_bOutBound)
    {
        m_nIndexOutBound = nIndex;
        LoadLineEditItem(pPageData, HOTDESKBASE_OUTBOUND1, _LANG(TRID_OUTBOUND_PROXY1), ITEM_MAX_LENGTH,
                         strIme, strDefaultIme, false, ADV_ACOCUNT_OUTBOUNDPROXY);
        LoadLineEditItem(pPageData, HOTDESKBASE_OUTBOUND2, _LANG(TRID_OUTBOUND_PROXY2), ITEM_MAX_LENGTH,
                         strIme, strDefaultIme, false, ADV_ACOCUNT_OUTBOUNDPROXY);
        nIndex = nIndex + INCREASE_TWO_ITEMS;
    }

#if IF_BUG_22024_LogonWizard_AutopURL
    if (m_bAutopUrl)
    {
        m_nIndexAutopUrl = nIndex;
        CSettingUIComboBoxItemDataPtr pComboboxItem = new CSettingUIComboBoxItemData;
        if (NULL != pComboboxItem)
        {
            pComboboxItem->SetItemID(HOTDESKBASE_AUTOPURL);
            pComboboxItem->SetDisplayName(_LANG(TRID_SEVER_URL));
            pComboboxItem->SetShowIndex(true);

            int i = 0;
            AddComboboxItemValue(pComboboxItem, _LANG(TRID_NONE_URL), i);
            i++;
            AddComboboxItemValue(pComboboxItem, _LANG(TRID_OFFICE_PHONE), i);
            i++;
            AddComboboxItemValue(pComboboxItem, _LANG(TRID_CALL_CENTER_PHONE), i);

            if (!pPageData->AddItem(pComboboxItem))
            {
                delete pComboboxItem;
            }
        }
    }

    int iIndex = LW_AUTOP_URL_NONE;
    if (HDT_LOGON_WIZRAD == m_eHotdeskType)
    {
        SetComboboxValue(HOTDESKBASE_AUTOPURL, iIndex);
    }
    else if (HDT_LOGON_WIZRAD_WITH_DATA == m_eHotdeskType)
    {
        yl::string strURL = configParser_GetConfigString(kszAutoProvisionUrl);
        int iIndex = LW_AUTOP_URL_NONE;
        if (configParser_GetConfigString(kszStart_HotDeskingOfficeURL) == strURL)
        {
            iIndex = LW_AUTOP_URL_OFFICE;
        }
        else if (configParser_GetConfigString(kszStart_HotDeskingCallCenterURL) == strURL)
        {
            iIndex = LW_AUTOP_URL_CALLCENTER;
        }
        SetComboboxValue(HOTDESKBASE_AUTOPURL, iIndex);
    }

#endif
}

void CHotDeskBaseSettingDelegate::GetHotDeskData(HotDeskBaseData & stData)
{
    if (m_bRegisterName)
    {
        stData.m_strRegisterName = GetValue(HOTDESKBASE_AUTH);
    }

    if (m_bUserName)
    {
        stData.m_strUserName = GetValue(HOTDESKBASE_USER);
    }

    if (m_bPassword)
    {
        stData.m_strPassword = GetValue(HOTDESKBASE_PWD);
    }

    if (m_bServer)
    {
        stData.m_strSipServer1 = GetValue(HOTDESKBASE_SERVER1);
        stData.m_strSipServer2 = GetValue(HOTDESKBASE_SERVER2);
    }

    if (m_bOutBound)
    {
        stData.m_strOutboundProxy1 = GetValue(HOTDESKBASE_OUTBOUND1);
        stData.m_strOutboundProxy2 = GetValue(HOTDESKBASE_OUTBOUND2);
    }
#if IF_BUG_22024_LogonWizard_AutopURL
    if (m_bAutopUrl)
    {
        stData.m_eAutopURL = (LoginWizardAutopRULType)GetComboboxValue(HOTDESKBASE_AUTOPURL);
    }
#endif
}

void CHotDeskBaseSettingDelegate::LoadLineEditItem(CSettingUIPageDataBase * pPageData
        , const yl::string & strID
        , const yl::string & strName
        , int nMaxLength
        , const yl::string & strIme
        , const yl::string & strDefaultIme
        , bool bPassword /*= false*/
        , const yl::string & strAuthorid /*= ""*/)
{
    if (NULL == pPageData)
    {
        return;
    }

    AddLineEditItemData(pPageData, strID, strName, "", nMaxLength, strIme, strDefaultIme, bPassword,
                        strAuthorid);
}

void CHotDeskBaseSettingDelegate::InitExtraData(void * pData)
{
    if (0 != configParser_GetConfigInt(kszHotDeskingRegisterName))
    {
        m_bRegisterName = true;
    }

    if (0 != configParser_GetConfigInt(kszHotDeskingUserName))
    {
        m_bUserName = true;
    }

    if (0 != configParser_GetConfigInt(kszHotDeskingPassword))
    {
        m_bPassword = true;
    }

    if (0 != configParser_GetConfigInt(kszHotDeskingSipServer))
    {
        m_bServer = true;
    }

    if (0 != configParser_GetConfigInt(kszHotDeskingOutbound))
    {
        m_bOutBound = true;
    }

#if IF_BUG_22024_LogonWizard_AutopURL
    if (NULL != pData)
    {
        int nHotdeskType = *((int *)pData);
        m_eHotdeskType = (HotDeskType)nHotdeskType;
    }
    if ((HDT_LOGON_WIZRAD == m_eHotdeskType || HDT_LOGON_WIZRAD_WITH_DATA == m_eHotdeskType)
            && 0 != configParser_GetConfigInt(kszStart_HotDeskingAutopurl))
    {
        m_bAutopUrl = true;
    }
#endif
}

void CHotDeskBaseSettingDelegate::AddComboboxItemValue(CSettingUIComboBoxItemData * pComboBoxItem,
        const yl::string & strDisplayName, const int & nValue)
{
    if (NULL == pComboBoxItem)
    {
        return;
    }

    CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;

    if (NULL == pOption)
    {
        return;
    }

    pOption->m_strDisplayName = strDisplayName;
    pOption->m_strItemVale = commonAPI_FormatString("%d", nValue);

    if (!pComboBoxItem->AddOptionItem(pOption))
    {
        delete pOption;
    }
}

LRESULT CHotDeskBaseSettingDelegate::OnMessage(msgObject & msg)
{
    CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDHotDeskBase);
    CHotDeskBaseSettingDelegate * pHotdeskDelegate = static_cast<CHotDeskBaseSettingDelegate *>
            (pDelegate);
    if (NULL == pHotdeskDelegate || pHotdeskDelegate != g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    bool bGuiQuery = *((bool *)msg.GetExtraData());
    if (ACCOUNT_STATUS_CHANGED == msg.message
            && 0 == configParser_GetConfigInt(kszExitWizardAfterLogin) && !bGuiQuery)
    {
        int iAccountID = msg.wParam;
        if (acc_IsAccountAvailable(iAccountID))
        {
            MessageBox_RemoveMessageBox(pHotdeskDelegate);
            pHotdeskDelegate->ShowNoteMessgaeBox(_LANG(TRID_REGISTERING_SUCCED),
                                                 MSGBOX_ID_SAVE_AND_REFRESH, SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
        }
        else if (LS_REGISTER_FAIL == acc_GetState(iAccountID)
                 || LS_UNKNOW == acc_GetState(iAccountID))
        {
            //退出提示框
            MessageBox_RemoveMessageBox(pHotdeskDelegate);
            pHotdeskDelegate->ShowNoteMessgaeBox(_LANG(TRID_REGISTERING_FAILED),
                                                 MSGBOX_ID_HOTDESK_LOGIN_FAILED);
            pHotdeskDelegate->AfterLogonWizardRegisterFail();
        }
    }
    return TRUE;
}

void CHotDeskBaseSettingDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nID = pMessageBox->GetID();
    int nResult = pMessageBox->GetResult();
    SETTING_INFO("CHotDeskBaseSettingDelegate::MessageBoxCallBack nID[%d], nResult[%d].", nID, nResult);

    if (MSGBOX_ID_HOTDESK_LOGINING == nID && SETTING_MessageBox_TimeOut == nResult)
    {
        ShowNoteMessgaeBox(_LANG(TRID_REGISTERING_FAILED), MSGBOX_ID_HOTDESK_LOGIN_FAILED);
        AfterLogonWizardRegisterFail();
    }
}

void CHotDeskBaseSettingDelegate::AfterLogonWizardRegisterFail()
{
    if (m_bPassword)
    {
        SetValue(HOTDESKBASE_PWD, "");
    }

    if (m_bUserName)
    {
        SetFocusIndex(m_nIndexUserName);
        g_SetUICallBackFun.RefreshUI(false);
    }
}

#endif
