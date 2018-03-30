#include "settinguiadvancedchangepwddelegate.h"
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "translateiddefine.h"
#include "setuidatadefine.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/src/advsettingcontroller.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "feature/include/modfeature.h"
#include "commonunits/modcommonunits.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

namespace
{
#define MSGBOX_ID_IF_SAVE_PASSWORD 1
}

CSettingUIAdvancedChangePwdDelegate::CSettingUIAdvancedChangePwdDelegate()
{

}

CSettingUIAdvancedChangePwdDelegate::~CSettingUIAdvancedChangePwdDelegate()
{

}

CSettingUIDelegateBase * CSettingUIAdvancedChangePwdDelegate::CreateChangePwdDelegate()
{
    CSettingUIAdvancedChangePwdDelegate * pDelegate = new CSettingUIAdvancedChangePwdDelegate;
    return (CSettingUIDelegateBase *)pDelegate;
}

CSettingUIPageDataBase * CSettingUIAdvancedChangePwdDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

#if IF_FEATURE_AUTOP_IME
    if (NULL != m_pPageData && bReLoad)
    {
        for (int i = 0; i < m_pPageData->GetTotalItemNumber(); ++i)
        {
            CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                        m_pPageData->GetItemByDataIndex(i));
            if (NULL != pEditItem)
            {
                yl::string strImeType = commonUnits_GetIMEByType(kszAdvancedSettingPasswordIME);
                pEditItem->SetDefaultIME(strImeType);
            }
        }
    }
#endif

    return m_pPageData;
}

bool CSettingUIAdvancedChangePwdDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (NULL == m_pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    PageSetPwdSave();

    return true;
}

bool CSettingUIAdvancedChangePwdDelegate::OnAction(int eActionType, int iDataIndex)
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

bool CSettingUIAdvancedChangePwdDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /* = NULL */)
{
    if (MSGBOX_ID_IF_SAVE_PASSWORD == nID && SETTING_MessageBox_OK == nResult)
    {
        SavePwd();
        return true;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

void CSettingUIAdvancedChangePwdDelegate::PageSetPwdSave()
{
    yl::string strCurrentPWD = GetValue(SETTINGUI_CHANGE_PWD_CURRENT_PWD);
    yl::string strNewPWD = GetValue(SETTINGUI_CHANGE_PWD_NEW_PWD);
    yl::string strConfirmPWD = GetValue(SETTINGUI_CHANGE_PWD_CONFIRM_PWD);

    yl::string strUserName = feature_UserModeGetUserName(feature_UserModeGetCurrType());

    //先检查旧账号密码是否正确
    if (!commonAPI_CheckPassword(strUserName.c_str(), strCurrentPWD.c_str()))
    {
        SetValidData(_LANG(TRID_THE_OLD_PASSWORD_IS_WRONG), 0);
        return;
    }

    //在检查新密码是否为admin
    if (0 != configParser_GetConfigInt(kszUserPwdLimit)
            && UT_ADMIN != feature_UserModeGetCurrType()
            && strUserName == strNewPWD)
    {
        SetValidData(_LANG(TRID_NEED_TO_CHANG_PASSWD), 0);
        return;
    }

    //再检查两次密码是否相同
    if (strNewPWD != strConfirmPWD)
    {
        SetValidData(_LANG(TRID_CONFIRM_PASSWORD_MISMATCH), 2);
        return;
    }

    // 密码为空
    if (strNewPWD.empty())
    {
        ShowOKCancleMsgBox(_LANG(TRID_ARE_YOU_SURE_TO_SET_BLANK_PASSWORD));
        return;
    }

    // 密码是账号的倒叙或与账号相同
    yl::string strUserNameFlip = "";
    int nLength = strUserName.length();
    for (int i = 0; i < nLength; ++i)
    {
        strUserNameFlip.insert(0, 1, strUserName[i]);
    }

    if (strNewPWD == strUserName || strNewPWD == strUserNameFlip)
    {
        ShowOKCancleMsgBox(_LANG(TRID_NEW_PASSWORD_IS_SIMILAR_TO_USERNAME_ARE_YOU_SURE_TO_SET_IT));
        return;
    }

    // 密码中包含字母和数字
    bool bHaveDigit = false, bHaveAlpha = false;
    nLength = strNewPWD.length();
    for (int i = 0; i < nLength; ++i)
    {
        if (isdigit(strNewPWD[i]))
        {
            bHaveDigit = true;
        }
        else if (isalpha(strNewPWD[i]))
        {
            bHaveAlpha = true;
        }
    }

    if (!bHaveAlpha || !bHaveDigit || nLength < 6)
    {
        ShowOKCancleMsgBox(_LANG(TRID_NEW_PASSWORD_IS_TOO_SIMILAR_ARE_YOU_SURE_TO_SET_IT));
        return;
    }

    SavePwd();
}

void CSettingUIAdvancedChangePwdDelegate::SavePwd()
{
    yl::string strNewPWD = GetValue(SETTINGUI_CHANGE_PWD_NEW_PWD);

    yl::string strUserName = feature_UserModeGetUserName(feature_UserModeGetCurrType());

    PasswordData passwordData;
    passwordData.m_strPasswordNew = strNewPWD;
    SetPassword(passwordData, strUserName.c_str());

    // 密码文件修改完之后, 将Admin用户的密码拷贝到/tmp/.htpasswd
    // /tmp/.htpasswd文件是用来限制download目录的权限的
    // 调用脚本实现密码的拷贝
    commonUnits_System("/phone/scripts/PasswdCopy.sh");

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);
}

void CSettingUIAdvancedChangePwdDelegate::SetValidData(const yl::string & strNote, int nIndex)
{
    CSettingUIDataValidInfo dataValid;
    dataValid.SetValidIndex(nIndex);
    dataValid.SetValidNote(strNote);
    g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
}

void CSettingUIAdvancedChangePwdDelegate::ShowOKCancleMsgBox(const yl::string & strNote)
{
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING), strNote
                                           , SETTING_MESSAGEBOX_OkCancel, "", "", -1, MSGBOX_ID_IF_SAVE_PASSWORD, NULL);
}
