#include "settinguiusermodedelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "commonunits/modcommonunits.h"
#include "feature/include/featurecommon.h"
#include "feature/include/modfeature.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "commonapi/systemhelperapi.h"

namespace
{
#define VALUE_USER 0
#define VALUE_VAR 1
#define VALUE_ADMIN 2
#define MSGBOX_INDEX_PSW_ERROR 111
#define INDEX_PWD 1
}

CSettinguiUserModeDelegate::CSettinguiUserModeDelegate()
{

}

CSettinguiUserModeDelegate::~CSettinguiUserModeDelegate()
{

}

CSettingUIDelegateBase * CSettinguiUserModeDelegate::CreateUserModeDelegate()
{
    CSettinguiUserModeDelegate * pDelegate = new CSettinguiUserModeDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiUserModeDelegate::ProcessMsgCallBack(int nResult, int nID, void* pExtraData /* = NULL */)
{
    if (MSGBOX_INDEX_PSW_ERROR == nID && SETTING_MessageBox_TimeOut == nResult)
    {
        SetFocusIndex(INDEX_PWD);
        g_SetUICallBackFun.RefreshUI(false);
        return true;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}


CSettingUIPageDataBase * CSettinguiUserModeDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        LoadUserTypeData(m_pPageData);

#if IF_FEATURE_AUTOP_IME
        yl::string strIme = commonUnits_GetIMEByType(kszAdvancedSettingPasswordIME);
        CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                    GetItemByItemId(USER_PASSWORD));
        if (!strIme.empty() && NULL != pEditItem)
        {
            pEditItem->SetDefaultIME(strIme);
        }
#endif
    }

    return m_pPageData;
}

bool CSettinguiUserModeDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    yl::string strPassword = GetValue(USER_PASSWORD);
    yl::string strUserName = GetValue(USER_MODE);
    USER_TYPE eUserType = feature_UserModeGetTypeByName(strUserName);

    bool bPasswdCorrent = commonAPI_CheckPassword(strUserName.c_str(), strPassword.c_str());
    if (bPasswdCorrent && configParser_GetConfigInt(kszUserPwdLimit) != 0 && UT_ADMIN != eUserType)
    {
        if (strPassword == strUserName)
        {
            // 密码和用户名相同时，提示用户更改密码
            SetValidInfo(_LANG(TRID_NEED_TO_CHANG_PASSWD));
            return false;
        }
    }
    // 判断用户名密码是否匹配
    if (bPasswdCorrent)
    {
        yl::string strOldName = feature_UserModeGetUserName(feature_UserModeGetCurrType());
        if (strOldName == strUserName)
        {
            ShowNoteMessgaeBox(_LANG(TRID_USER_MODE_CHANGED), MSGBOX_ID_SAVE);
        }
        else
        {
            ShowNoteMessgaeBox(_LANG(TRID_USER_MODE_CHANGED), MSGBOX_ID_SAVE_AND_REFRESH);
            // 更改当前的用户权限
            feature_UserModeSetCurrType(eUserType);
            etl_NotifyApp(false, DSK_MSG_USER_LEVEL_CHANGED, eUserType, 0);
            commonUnits_System("/phone/scripts/PasswdCopy.sh");
        }
    }
    else
    {
        SetValidInfo(_LANG(TRID_PASSWORD_IS_WRONG));
    }

    return true;
}

void CSettinguiUserModeDelegate::LoadUserTypeData(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    yl::string strCurrName = feature_UserModeGetUserName(feature_UserModeGetCurrType());

    // user权限显示名
    yl::string strUserName = feature_UserModeGetUserName(UT_USER);
    if (!strUserName.empty())
    {
        AddUserTypeOption(m_pPageData, strUserName, strCurrName);
    }

    if (0 != configParser_GetConfigInt(kszVarEnable))
    {
        // var权限显示名
        yl::string strVarName = feature_UserModeGetUserName(UT_VAR);
        if (!strVarName.empty())
        {
            AddUserTypeOption(m_pPageData, strVarName, strCurrName);
        }
    }

    // admin权限显示名
    yl::string strAdminName = feature_UserModeGetUserName(UT_ADMIN);
    if (!strAdminName.empty())
    {
        AddUserTypeOption(m_pPageData, strAdminName, strCurrName);
    }
}

void CSettinguiUserModeDelegate::AddUserTypeOption(CSettingUIPageDataBase * pPageData,
        const yl::string & strValue, const yl::string & strCurrentName)
{
    if (NULL == pPageData)
    {
        return;
    }

    CSettingUIComboBoxItemDataPtr pCmbItem = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                pPageData->GetItemByItemId(USER_MODE));
    if (NULL != pCmbItem)
    {
        CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;
        if (NULL != pOption)
        {
            pOption->m_strDisplayName = _LANG(strValue);
            pOption->m_strItemVale = strValue;
        }

        if (!pCmbItem->AddOptionItem(pOption))
        {
            delete pOption;
            return;
        }

        if (strValue == strCurrentName)
        {
            pCmbItem->SetValue(strValue);
        }
    }
}

void CSettinguiUserModeDelegate::SetValidInfo(const yl::string strNote)
{
    yl::string strEmpty = "";
    SetValue(USER_PASSWORD, strEmpty);
    ShowNoteMessgaeBox(strNote, MSGBOX_INDEX_PSW_ERROR);
}
