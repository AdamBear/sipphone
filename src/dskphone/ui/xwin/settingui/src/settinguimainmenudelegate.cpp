#include "settinguimainmenudelegate.h"
#include "settinguihistorydelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingui/include/itemnamedefine.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "feature/include/modfeature.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "pageiddefine.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"


namespace
{
#define ID_ADVANCE_PASSWORD_MSGBOX 10
const char * kszHistory = "call_info";
}

CSettingUIMainMenuDelegate::CSettingUIMainMenuDelegate()
{

}

CSettingUIMainMenuDelegate::~CSettingUIMainMenuDelegate()
{

}

CSettingUIDelegateBase * CSettingUIMainMenuDelegate::CreateMainMenuDelegate()
{
    CSettingUIMainMenuDelegate * pMainMenuDelegate = new CSettingUIMainMenuDelegate;

    return pMainMenuDelegate;
}

bool CSettingUIMainMenuDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        const yl::string & strID = GetItemIDByDataIndex(iDataIndex);

        if (kszHistory == strID && CSettingUIHistoryDelegate::EnterHistoryDirectly())
        {
            return true;
        }
#if IF_BUG_29376
        if (REBOOT == strID)
        {
            if (!CheckTalking())
            {
                g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING),
                                                       _LANG(TRID_IF_REBOOT_THE_PHONE),
                                                       SETTING_MESSAGEBOX_OkCancel, "", "", -1, MSG_ID_REBOOT, NULL);
            }

            return true;
        }
#endif
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

bool CSettingUIMainMenuDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

#ifdef IF_FEATURE_BROADSOFT_BASE
    //Brosoft和xsi同时开启下才显示Call Controll
    if (1 == configParser_GetConfigInt(kszBroadsoftActive)
            && 1 == configParser_GetConfigInt(kszXSISwitch))
    {
        SetItemShow(pPagedata, MAINMENU_CALLCONTROL, true);
    }
    else
#endif
    {
        SetItemShow(pPagedata, MAINMENU_CALLCONTROL, false);
    }

#if IF_BUG_29376
    SetItemShow(pPagedata, REBOOT, true);
#else
    SetItemShow(pPagedata, REBOOT, false);
#endif

    bool bShowMode = feature_UserModeIsOpen();
    SetItemShow(pPagedata, USER_MODE_ENTRY, bShowMode);

    return bResult;
}


void CSettingUIMainMenuDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() >= 4)
    {
        vecSoftKey[3].m_iType = ST_ENTER;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);

    }

}

void CSettingUIMainMenuDelegate::OnEnterPage(const yl::string & strPage, int iDataIndex)
{
    if (0 == strPage.compare(kszPageIDAdvancedSetting))
    {
        PopUpPasswordCheckMsgBox();
    }
    else
    {
        PageOpenData opPageData(strPage);
        opPageData.pExtraData = GetItemExtraDataByDataIndex(iDataIndex);
        g_SetUICallBackFun.EnterPage(opPageData);
    }
}

void CSettingUIMainMenuDelegate::PopUpPasswordCheckMsgBox()
{
    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = MESSAGEBOX_PWD;
    if (feature_UserModeIsOpen())
    {
        yl::string strUserName = feature_UserModeGetUserName(feature_UserModeGetCurrType());
        if (!strUserName.empty())
        {
            stMsgInfo.strTitle = strUserName;
        }
    }

    yl::string strNode = _LANG(TRID_PASSWORD);
    strNode += ":";
    stMsgInfo.strNote = strNode;
    stMsgInfo.pAction = this;
    stMsgInfo.iID = ID_ADVANCE_PASSWORD_MSGBOX;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

void CSettingUIMainMenuDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL != pMessageBox
            && MESSAGEBOX_PWD == pMessageBox->GetType()
            && ID_ADVANCE_PASSWORD_MSGBOX == pMessageBox->GetID()
            && MessageBox_OK == pMessageBox->GetResult())
    {
        PageOpenData opPageData(kszPageIDAdvancedSetting);
        g_SetUICallBackFun.EnterPage(opPageData);
    }
}

