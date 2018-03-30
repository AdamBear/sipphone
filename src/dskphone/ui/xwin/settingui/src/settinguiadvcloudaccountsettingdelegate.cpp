#include "settinguiadvcloudaccountsettingdelegate.h"

#if IF_FEATURE_CLOUD_ACCOUNT

#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settinguiwizardmanager.h"
#include "settinguiaccountlogindatainfo.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/advsettingcontroller.h"
#include "account/include/modaccount.h"
#include "account/include/account_datadefine.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define SET_DEFAULT_2aB "2aB"
#define SET_DEFAULT_123 "123"
#define SET_DEFAULT_PWD_2aB "2aB_PWD"
#define ITEM_MAX_LENGTH 99
#define PINCODE_LENGTH 9
#define INDEX_PINCODE 2
#define INDEX_USERNAME 2
#define INDEX_PASSWORD 3
#define MSGBOX_ID_CANCLE_LOGIN 1004
#define MSGBOX_ID_LOGOUT 1005
#define MSGBOX_ID_IN_THE_LOGOUT 1006
#define MSGBOX_ID_LOGIN_ERROR 1007
}

CAdvCloudAccountSettingDelegate::CAdvCloudAccountSettingDelegate()
    : m_nAccountStatus(ACCOUNT_STATUS_COMMON)
    , m_nLoginMode(CLOUD_LOGIN_TYPE_ERROR)
    , m_bComboboxChange(false)
    , m_pAccountInfo(NULL)
    , m_pCommonInfo(NULL)
    , m_bLogout(false)
#ifdef IF_SETTING_WIZARD
    , m_bFinish(false)
#endif
{
    etl_RegisterMsgHandle(ACCOUNT_CLOUD_LOGIN_RESULT, ACCOUNT_CLOUD_LOGIN_RESULT
                          , &CAdvCloudAccountSettingDelegate::OnMessage);
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                          , &CAdvCloudAccountSettingDelegate::OnMessage);
}

CAdvCloudAccountSettingDelegate::~CAdvCloudAccountSettingDelegate()
{
    if (NULL != m_pAccountInfo)
    {
        delete m_pAccountInfo;
        m_pAccountInfo = NULL;
    }

    if (NULL != m_pCommonInfo)
    {
        delete m_pCommonInfo;
        m_pCommonInfo = NULL;
    }

    etl_UnregisterMsgHandle(ACCOUNT_CLOUD_LOGIN_RESULT, ACCOUNT_CLOUD_LOGIN_RESULT
                            , &CAdvCloudAccountSettingDelegate::OnMessage);
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                            , &CAdvCloudAccountSettingDelegate::OnMessage);
}

CSettingUIDelegateBase * CAdvCloudAccountSettingDelegate::CreateAdvCloudAccountSettingDelegate()
{
    CAdvCloudAccountSettingDelegate * pDelegate = new CAdvCloudAccountSettingDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CAdvCloudAccountSettingDelegate::OnMessage(msgObject & msg)
{
    CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(
            kszPageIDAdvCloudAccountSetting);
#ifdef IF_SETTING_WIZARD
    if (NULL == pDelegate)
    {
        pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDGuideCloudSipLine);
    }
#endif

    CAdvCloudAccountSettingDelegate * pCloudDelegate = static_cast<CAdvCloudAccountSettingDelegate *>
            (pDelegate);
    if (NULL == pCloudDelegate || pCloudDelegate != g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

#ifdef IF_SETTING_WIZARD
    if (pCloudDelegate->IsFinish())
    {
        return FALSE;
    }
#endif

    if (ACCOUNT_CLOUD_LOGIN_RESULT == msg.message)
    {
        pCloudDelegate->ProcessLoginResult(msg.wParam, msg.lParam);
        return TRUE;
    }
    else if (ACCOUNT_STATUS_CHANGED == msg.message)
    {
        pCloudDelegate->ProcessAccountStatusChange();
        return TRUE;
    }

    return FALSE;
}

void CAdvCloudAccountSettingDelegate::ProcessAccountStatusChange()
{
    //如果处于云登录成功状态，如果是Logout引发的，则刷新界面(弹窗回调时刷新)
    //如果是网页等操作引起的则返回上一层界面，非云登录成功不处理
    if (ACCOUNT_STATUS_YEALINK_LOGINED == m_nAccountStatus && !m_bLogout)
    {
        g_SetUICallBackFun.FallBackToPrevousPage();
        g_SetUICallBackFun.RefreshUI(true, GetPageID());
    }
}

void CAdvCloudAccountSettingDelegate::ProcessLoginResult(int nAccountId, int iResult)
{
    if (nAccountId != m_iLineID)
    {
        return;
    }

    //关闭正在登录弹窗
    MessageBox_RemoveMessageBox(this, MessageBox_OK, MSGBOX_ID_CANCLE_LOGIN);

    //判断是否登录成功
    yl::string strHint;
    int nIndex = -1;
    //添加登录是否成功的标志，并赋值
    switch (iResult)
    {
    case REG_SUCC:              //登录成功
        {
            g_SetUICallBackFun.FallBackToPrevousPage();
            g_SetUICallBackFun.RefreshUI(true, GetPageID());
            return;
        }
        break;
    case REG_NETWORK_UNAVAILABLE:   //网络无效
        strHint = TRID_NETWORK_UNAVAILABLE;
        break;
    case REG_PHONE_NOT_FOUND:   //用户名不存在
        strHint = TRID_PHONE_NOT_FIND;
        if (INDEX_USERNAME != GetFocusIndex())
        {
            nIndex = INDEX_USERNAME;
        }
        break;
    case REG_PIN_NOT_FOUND:     //pin不存在
        strHint = TRID_PIN_NOT_FIND;
        if (INDEX_PINCODE != GetFocusIndex())
        {
            nIndex = INDEX_PINCODE;
        }
        break;
    case REG_UNAUTHORIZED:      //SIP 401 密码错误
        strHint = TRID_CLOUD_USER_OR_PWD_WRONG;
        break;
    case REG_FORBIDDEN:     //SIP 403
    case REG_SERVER_ERROR:  // SIP 500
        strHint = TRID_CLOUD_SERVER_ERROR;
        break;
    case REG_NOT_FOUND:     // SIP 404
    case REG_OTHER_ERROR:
        strHint = TRID_CLOUD_OTHER_ERROR;
        break;
    default:
        break;
    }

    if (!strHint.empty())
    {
        g_SetUICallBackFun.RefreshUI(false);
        if (-1 != nIndex)
        {
            SetInvalidData(nIndex, _LANG(strHint.c_str()));
        }
        else
        {
            ShowNoteMessgaeBox(_LANG(strHint.c_str()), MSGBOX_ID_LOGIN_ERROR);
        }
    }
}

void CAdvCloudAccountSettingDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nID = pMessageBox->GetID();
    int nResult = pMessageBox->GetResult();

    if (MSGBOX_ID_CANCLE_LOGIN == nID && MessageBox_Cancel == nResult)
    {
        acc_CancelCloudLogin(m_iLineID);
    }
    else if (MSGBOX_ID_LOGOUT == nID && MessageBox_OK == nResult)
    {
        m_bLogout = true;
        ShowMessageBox(MESSAGEBOX_NOTE, 1500, _LANG(TRID_CLOUD_LOGOUTING), MSGBOX_ID_IN_THE_LOGOUT,
                       _LANG(TRID_NOTE));
        acc_CloudAccountLogout(m_iLineID);
    }
    else if (MSGBOX_ID_IN_THE_LOGOUT == nID && MessageBox_TimeOut == nResult)
    {
        g_SetUICallBackFun.RefreshUI(true, GetPageID());
    }
    else if (MSGBOX_ID_LOGIN_ERROR == nID && MessageBox_TimeOut == nResult)
    {
        g_SetUICallBackFun.RefreshUI(false);
    }
}

CSettingUIPageDataBase * CAdvCloudAccountSettingDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData)
    {
        return m_pPageData;
    }

    if (bReLoad)
    {
        ReloadPageData();
        m_bLogout = false;
        return m_pPageData;
    }

    int nAccountStatus = GetComboboxValue(ADV_CLOUD_ACCOUNT_TYPE);
    if (-1 == nAccountStatus)
    {
        return m_pPageData;
    }

    if (nAccountStatus != m_nAccountStatus)
    {
        //账号模式改变
        m_pPageData->DelFromItem(ADV_CLOUD_ACCOUNT_TYPE);
        m_nAccountStatus = nAccountStatus;
        LoadFromPlatForm();
    }
    else if (ACCOUNT_STATUS_YEALINK == nAccountStatus)
    {
        int nLoginMode = GetComboboxValue(ADV_CLOUD_ACCOUNT_LOGINTYPE);
        if (-1 == nLoginMode)
        {
            return m_pPageData;
        }

        if (m_nLoginMode != nLoginMode)
        {
            //平台改变
            m_pPageData->DelFromItem(ADV_CLOUD_ACCOUNT_LOGINTYPE);
            m_nLoginMode = nLoginMode;
            LoadFromYealinkLoginMode();
        }
    }

    RefreshPageData(false);

    m_bLogout = false;

    return m_pPageData;
}

void CAdvCloudAccountSettingDelegate::ReloadPageData()
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    if (ACCOUNT_YEALINK == acc_GetRegisterAccountTypeById(m_iLineID))
    {
        m_nAccountStatus = ACCOUNT_STATUS_YEALINK_LOGINED;
    }
    else if (m_bLogout || ACCOUNT_YEALINK == acc_GetAccountType(m_iLineID))
    {
        m_nAccountStatus = ACCOUNT_STATUS_YEALINK;
    }
    else
    {
        m_nAccountStatus = ACCOUNT_STATUS_COMMON;
    }

    CloudAccountInfo stInfo;
    stInfo.m_eType = ACCOUNT_YEALINK;
    stInfo.m_nAccountId = m_iLineID;
    if (acc_GetCloudAccount(stInfo))
    {
        m_nLoginMode = CLOUD_LOGIN_TYPE_USERNAME_LOGIN;
    }
    else
    {
        m_nLoginMode = CLOUD_LOGIN_TYPE_PIN_LOGIN;
    }

    if (ACCOUNT_STATUS_YEALINK_LOGINED == m_nAccountStatus)
    {
        LoadCloudLoginedPageData();
    }
    else
    {
        //根据当前平台加载数据
        LoadAccountTypeItem();

        LoadFromPlatForm();
    }

    RefreshPageData(true);
}

bool CAdvCloudAccountSettingDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
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

    if (ACCOUNT_STATUS_COMMON == m_nAccountStatus)
    {
        return SaveCommonPageData(bBackToPrevious, bPopHint);
    }
    else if (ACCOUNT_STATUS_YEALINK == m_nAccountStatus)
    {
        return OnCloudLogin(bBackToPrevious, bPopHint);
    }

    return true;
}

bool CAdvCloudAccountSettingDelegate::OnPageAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
#ifdef IF_SETTING_WIZARD
        if (kszPageIDGuideCloudSipLine == GetPageID())
        {
            return true;
        }
#endif
        //根据当前模式决定处理方式
        if (ACCOUNT_STATUS_COMMON == m_nAccountStatus)
        {
            SaveCommonPageData();
        }
        else if (ACCOUNT_STATUS_YEALINK == m_nAccountStatus)
        {
            OnCloudLogin();
        }
        else if (ACCOUNT_STATUS_YEALINK_LOGINED == m_nAccountStatus)
        {
            OnCloudLogout();
        }

        return true;
    }

    return false;
}

bool CAdvCloudAccountSettingDelegate::OnAction(int eActionType, int iDataIndex)
{
#ifdef IF_SETTING_WIZARD
    if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsInWizard())
    {
        if (SET_ACTION_TYPE_FINISH == eActionType)
        {
            if (SavePageData(m_pPageData, false, false))
            {
                m_bFinish = true;
                g_pSettingUIWizardManager->FinishWizard();
            }
            return true;
        }
    }
#endif

    if (SET_ACTION_TYPE_LOGOUT == eActionType)
    {
        OnCloudLogout();
    }
    else if (SET_ACTION_TYPE_SAVE == eActionType)
    {
        SaveCommonPageData();
    }
    else if (SET_ACTION_TYPE_LOGIN == eActionType)
    {
        OnCloudLogin();
    }
    else
    {
        return CAdvCommonAccountSettingDelegate::OnAction(eActionType, iDataIndex);
    }

    return true;
}

void CAdvCloudAccountSettingDelegate::OnComboBoxValueChange(int nIndex)
{
    CSettingUIItemDataBase * pItemData = GetItemDataByDataIndex(nIndex);
    if (NULL != pItemData)
    {
        yl::string strItemID = pItemData->GetItemID();
        if (ADV_CLOUD_ACCOUNT_TYPE == strItemID)
        {
            //缓存当前平台数据
            if (ACCOUNT_STATUS_COMMON == m_nAccountStatus)
            {
                CacheCommonAccoutData();
            }

            g_SetUICallBackFun.RefreshUI(false, GetPageID());
        }
        else if (ADV_CLOUD_ACCOUNT_LOGINTYPE == strItemID)
        {
            //缓存当前登录模式数据
            if (CLOUD_LOGIN_TYPE_PIN_LOGIN == m_nLoginMode)
            {
                CacheYealinkPinCodeData();
            }
            else if (CLOUD_LOGIN_TYPE_USERNAME_LOGIN == m_nLoginMode)
            {
                CacheYealinkUsernameData();
            }

            g_SetUICallBackFun.RefreshUI(false, GetPageID());
        }
    }
}

void CAdvCloudAccountSettingDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
#ifdef IF_SETTING_WIZARD
    //开机引导界面第4个softkey固定
    if (kszPageIDGuideCloudSipLine == GetPageID())
    {
        return;
    }
#endif

    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }
    int nIndex = SOFTKEY_NUM - 1;
    if (ACCOUNT_STATUS_YEALINK_LOGINED == m_nAccountStatus)
    {
        vecSoftKey[nIndex].m_iType = ST_LOGOUT;
        vecSoftKey[nIndex].m_strSoftkeyTitle = _LANG(TRID_LOGOUT);
    }
    else if (ACCOUNT_STATUS_COMMON == m_nAccountStatus)
    {
        vecSoftKey[nIndex].m_iType = ST_SAVE;
        vecSoftKey[nIndex].m_strSoftkeyTitle = _LANG(TRID_SAVE);
    }
    else if (ACCOUNT_STATUS_YEALINK == m_nAccountStatus)
    {
        vecSoftKey[nIndex].m_iType = ST_LOGIN;
        vecSoftKey[nIndex].m_strSoftkeyTitle = _LANG(TRID_LOGIN);
    }
}

bool CAdvCloudAccountSettingDelegate::OnCloudLogin(bool bBackToPrevious /* = true */,
        bool bPopHint /* = true */)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (CLOUD_LOGIN_TYPE_PIN_LOGIN == m_nLoginMode)
    {
        CacheYealinkPinCodeData();

        yl::string strPinCode = GetValue(ADV_CLOUD_ACCOUNT_PINCODE);
        if (strPinCode.size() < PINCODE_LENGTH)
        {
            return SetInvalidData(INDEX_PINCODE, _LANG(TRID_PIN_LESS_NINE));
        }

        if (bPopHint)
        {
            ShowMessageBox(MESSAGEBOX_Cancel, -1, _LANG(TRID_CLOUD_LOGINING), MSGBOX_ID_CANCLE_LOGIN,
                           _LANG(TRID_NOTE));
        }

        acc_CloudPinLogin(m_iLineID, strPinCode);
    }
    else
    {
        CloudAccountInfo stCloudData;
        GetYealinkUserNamePageData(&stCloudData);
        if (stCloudData.m_strUserName.empty())
        {
            CacheYealinkUsernameData();
            return SetInvalidData(INDEX_USERNAME, _LANG(TRID_USERNAME_CANNOT_EMPTY));
        }

        if (stCloudData.m_strPassWord.empty())
        {
            CacheYealinkUsernameData();
            return SetInvalidData(INDEX_PASSWORD, _LANG(TRID_PASSWORD_CANNOT_EMPTY));
        }

        CacheYealinkUsernameData(true);

        if (bPopHint)
        {
            ShowMessageBox(MESSAGEBOX_Cancel, -1, _LANG(TRID_CLOUD_LOGINING), MSGBOX_ID_CANCLE_LOGIN,
                           _LANG(TRID_NOTE));
        }

        acc_CloudAccountLogin(stCloudData);
    }

    return true;
}

void CAdvCloudAccountSettingDelegate::OnCloudLogout()
{
    ShowMessageBox(MESSAGEBOX_OkCancel, -1, _LANG(TRID_IF_EXIT_CLOUD), MSGBOX_ID_LOGOUT,
                   _LANG(TRID_NOTE));
}

bool CAdvCloudAccountSettingDelegate::SaveCommonPageData(bool bBackToPrevious /* = true */,
        bool bPopHint /* = true */)
{
    CacheCommonAccoutData();

    return CAdvCommonAccountSettingDelegate::SavePageData(m_pPageData, bBackToPrevious, bPopHint);
}

bool CAdvCloudAccountSettingDelegate::SetInvalidData(int nIdex, const yl::string & strNote)
{
    CSettingUIDataValidInfo validInfo;
    validInfo.SetValidIndex(nIdex);
    validInfo.SetValidNote(strNote);
    g_SetUICallBackFun.DealWithValidData(validInfo, m_strPageID);
    return false;
}

void CAdvCloudAccountSettingDelegate::CacheCommonAccoutData()
{
    AccountDetailData stData;
    GetCommonAccountData(stData);
    if (NULL == m_pCommonInfo)
    {
        m_pCommonInfo = new CCommonAccountInfo();
    }
    if (NULL != m_pCommonInfo)
    {
        m_pCommonInfo->SetCommonAccountInfo(stData);
        m_pCommonInfo->SetProxyFallbackInterval(GetValue(ADV_ACCOUNT_PROXYFALLBACKINTERVAL));
    }
}

void CAdvCloudAccountSettingDelegate::CacheYealinkPinCodeData()
{
    yl::string strPincode = GetValue(ADV_CLOUD_ACCOUNT_PINCODE);

    if (NULL == m_pAccountInfo)
    {
        m_pAccountInfo = new CCloudAccountInfo;
    }

    if (NULL != m_pAccountInfo)
    {
        m_pAccountInfo->SetCloudPincode(strPincode);
    }
}

void CAdvCloudAccountSettingDelegate::CacheYealinkUsernameData(bool bCheckSave /*= false*/)
{
    CloudAccountInfo stCloudData;
    GetYealinkUserNamePageData(&stCloudData);

    if (!stCloudData.m_bSavePass && bCheckSave)
    {
        stCloudData.m_strPassWord = "";
    }

    if (NULL == m_pAccountInfo)
    {
        m_pAccountInfo = new CCloudAccountInfo;
    }

    if (NULL != m_pAccountInfo)
    {
        m_pAccountInfo->SetCloudAccountInfo(stCloudData);
    }
}

void CAdvCloudAccountSettingDelegate::LoadFromPlatForm()
{
    if (ACCOUNT_STATUS_YEALINK == m_nAccountStatus)
    {
        LoadComboboxItem(m_pPageData, ADV_CLOUD_ACCOUNT_LOGINTYPE, _LANG(TRID_CLOUD_LOGIN_TYPE),
                         _LANG(TRID_PINCODE_LOGIN), _LANG(TRID_ACCOUNT_LOGIN));

        LoadFromYealinkLoginMode();
    }
    else
    {
        LoadCommonAccountPageData(m_pPageData);
    }
}

void CAdvCloudAccountSettingDelegate::LoadAccountTypeItem()
{
    LoadComboboxItem(m_pPageData, ADV_CLOUD_ACCOUNT_TYPE, _LANG(TRID_TYPE), _LANG(TRID_ACCOUNT_COMMON),
                     _LANG(TRID_ACCOUNT_YEALINK_CLOUD));
}

void CAdvCloudAccountSettingDelegate::LoadFromYealinkLoginMode()
{
    if (CLOUD_LOGIN_TYPE_PIN_LOGIN == m_nLoginMode)
    {
        LoadLineEditItem(m_pPageData, ADV_CLOUD_ACCOUNT_PINCODE, _LANG(TRID_PINCODE_LOGIN), PINCODE_LENGTH,
                         SET_DEFAULT_123, false);
    }
    else
    {
        //user name
        LoadLineEditItem(m_pPageData, ADV_CLOUD_ACCOUNT_USERNAME, _LANG(TRID_CLOUD_ACCOUNT_NAME),
                         ITEM_MAX_LENGTH, SET_DEFAULT_2aB, false);
        //password
        LoadLineEditItem(m_pPageData, ADV_CLOUD_ACCOUNT_PASSWORF, _LANG(TRID_PASSWORD), ITEM_MAX_LENGTH,
                         SET_DEFAULT_PWD_2aB, true);
        //save password
        LoadComboboxItem(m_pPageData, ADV_CLOUD_ACCOUNT_SAVEPAAWORD, _LANG(TRID_SAVE_PASSWORD),
                         _LANG(TRID_DISABLED), _LANG(TRID_ENABLED));
    }
}

void CAdvCloudAccountSettingDelegate::LoadCloudLoginedPageData()
{
    //类型
    LoadBaseDataItem(_LANG(TRID_TYPE), _LANG(TRID_ACCOUNT_YEALINK_CLOUD));
    //状态
    yl::string strName = commonAPI_FormatString("%s %s", acc_GetAccountShowText(m_iLineID).c_str(),
                         _LANG(TRID_CLOUD_LOGIN_SUCCESS).c_str());
    AddMenuItem(m_pPageData, "", strName, "");
}

void CAdvCloudAccountSettingDelegate::LoadBaseDataItem(const yl::string & strDisplayName,
        const yl::string & strValue)
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    AddBaseItemData(m_pPageData, strDisplayName, strValue);
}

void CAdvCloudAccountSettingDelegate::RefreshPageData(bool bReLoad)
{
    if (ACCOUNT_STATUS_COMMON == m_nAccountStatus)
    {
        SetComboboxValue(ADV_CLOUD_ACCOUNT_TYPE, ACCOUNT_STATUS_COMMON);
        if (NULL != m_pCommonInfo)
        {
            const AccountDetailData & stData = m_pCommonInfo->GetCommonAccountInfo();
            SetCommonAccountData(stData);
            SetValue(ADV_ACCOUNT_PROXYFALLBACKINTERVAL, m_pCommonInfo->GetProxyFallbackInterval());
        }
        else
        {
            AccountDetailData stData;
            if (GetAccountDetailData(&stData, m_iLineID))
            {
                SetCommonAccountData(stData);
            }
        }
    }
    else if (ACCOUNT_STATUS_YEALINK == m_nAccountStatus)
    {
        SetComboboxValue(ADV_CLOUD_ACCOUNT_TYPE, ACCOUNT_STATUS_YEALINK);
        if (CLOUD_LOGIN_TYPE_PIN_LOGIN == m_nLoginMode)
        {
            SetComboboxValue(ADV_CLOUD_ACCOUNT_LOGINTYPE, CLOUD_LOGIN_TYPE_PIN_LOGIN);
            if (NULL != m_pAccountInfo)
            {
                SetValue(ADV_CLOUD_ACCOUNT_PINCODE, m_pAccountInfo->GetCloudPinCode());
            }
        }
        else if (CLOUD_LOGIN_TYPE_USERNAME_LOGIN == m_nLoginMode)
        {
            SetComboboxValue(ADV_CLOUD_ACCOUNT_LOGINTYPE, CLOUD_LOGIN_TYPE_USERNAME_LOGIN);
            if (NULL != m_pAccountInfo && m_pAccountInfo->IsHasAccountInfo())
            {
                CloudAccountInfo stInfo = m_pAccountInfo->GetCloudAccountInfo();
                RefreshYealinkUsertNameData(&stInfo, bReLoad);
            }
            else
            {
                CloudAccountInfo stInfo;
                stInfo.m_eType = ACCOUNT_YEALINK;
                stInfo.m_nAccountId = m_iLineID;
                if (acc_GetCloudAccount(stInfo))
                {
                    if (!stInfo.m_bSavePass)
                    {
                        stInfo.m_strPassWord = "";
                    }
                    RefreshYealinkUsertNameData(&stInfo, bReLoad);
                }
                else
                {
                    SetValue(ADV_CLOUD_ACCOUNT_SAVEPAAWORD, "1");
                }
            }
        }
    }
}

void CAdvCloudAccountSettingDelegate::RefreshYealinkUsertNameData(CloudAccountInfo * pCloudInfo,
        bool bReLoad)
{
    if (NULL != pCloudInfo)
    {
        SetValue(ADV_CLOUD_ACCOUNT_USERNAME, pCloudInfo->m_strUserName);
        if (!bReLoad ||  pCloudInfo->m_bSavePass)
        {
            SetValue(ADV_CLOUD_ACCOUNT_PASSWORF, pCloudInfo->m_strPassWord);
        }
        SetComboboxValue(ADV_CLOUD_ACCOUNT_SAVEPAAWORD, pCloudInfo->m_bSavePass);
    }
}

void CAdvCloudAccountSettingDelegate::GetYealinkUserNamePageData(CloudAccountInfo * pCloudInfo)
{
    if (NULL == pCloudInfo)
    {
        return;
    }

    pCloudInfo->m_strUserName = GetValue(ADV_CLOUD_ACCOUNT_USERNAME);
    pCloudInfo->m_strPassWord = GetValue(ADV_CLOUD_ACCOUNT_PASSWORF);
    pCloudInfo->m_bSavePass = GetComboboxValue(ADV_CLOUD_ACCOUNT_SAVEPAAWORD);
    pCloudInfo->m_nAccountId = m_iLineID;
    pCloudInfo->m_eType = ACCOUNT_YEALINK;
}

void CAdvCloudAccountSettingDelegate::ShowMessageBox(int nType, int nTime,
        const yl::string & strNote, int nID, const yl::string strTitle)
{
    MSGBOX_INFO infoMsgBox;

    infoMsgBox.pAction = this;
    infoMsgBox.eType = (MESSAGEBOX_TYPE)nType;
    infoMsgBox.nMsTime = nTime;
    infoMsgBox.strTitle = strTitle;
    infoMsgBox.strNote = strNote;
    infoMsgBox.iID = nID;

    MessageBox_ShowCommonMsgBox(infoMsgBox);
}

#endif
