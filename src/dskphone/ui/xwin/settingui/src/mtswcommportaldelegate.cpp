#include "mtswcommportaldelegate.h"

#if IF_FEATURE_METASWITCH

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include "settinguimanager.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/moduimanager.h"

#include "account/include/modaccount.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#include "baselibs/include/taskaction/common.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"


namespace
{
#define MTSW_NOTE_MSGBOX_SHOW_TIME     1500
#define MSGBOX_ID_MTSW_LOGINFAILED     1210
}

CMtswCommportalDelegate::CMtswCommportalDelegate()
    : m_pReturnCallBack(NULL)
{
    etl_RegisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END,
                          &CMtswCommportalDelegate::OnMTSWCommportalMsg);
}

CMtswCommportalDelegate::~CMtswCommportalDelegate()
{
    etl_UnregisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END,
                            &CMtswCommportalDelegate::OnMTSWCommportalMsg);
}

CSettingUIDelegateBase* CMtswCommportalDelegate::CreatMtswCommportalDelegate()
{
    CMtswCommportalDelegate* pDelegate = new CMtswCommportalDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CMtswCommportalDelegate::OnMTSWCommportalMsg(msgObject& msg)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate || kszPageMTSWCommportal != pDelegate->GetPageID())
    {
        return FALSE;
    }

    CMtswCommportalDelegate* pCommportalDelegate = static_cast<CMtswCommportalDelegate*>(pDelegate);
    if (NULL == pCommportalDelegate)
    {
        return FALSE;
    }

    if (MTSW_MESSAGE_LOGIN == msg.message)
    {
        MessageBox_RemoveAllMessageBox();
        pCommportalDelegate->OnReturnCallback(true);
        g_SetUICallBackFun.FallBackToPrevousPage();
        g_SetUICallBackFun.RefreshUI(true);
    }
    else if (MTSW_MESSAGE_AUTH_ERR == msg.message)
    {
        MessageBox_RemoveAllMessageBox();
        NS_TA::TaskActionErrorType eErrCode = (NS_TA::TaskActionErrorType)msg.lParam;
        if (NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED == eErrCode
                ||  NS_TA::TA_HTTP_ERR403 == eErrCode)
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWCommportal, _LANG(TRID_LOGIN_FAILED), _LANG(TRID_COMMPORTAL_LOGIN_FAILED),
                                                   SETTING_MESSAGEBOX_Cancel, "", "", -1, MSGBOX_ID_MTSW_LOGINFAILED);
        }
        else
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWCommportal, _LANG(TRID_LOGIN_FAILED), _LANG(TRID_LOGIN_FAILED),
                                                   SETTING_MESSAGEBOX_Cancel, "", "", -1, MSGBOX_ID_MTSW_LOGINFAILED);
        }
    }

    return TRUE;
}

bool CMtswCommportalDelegate::SavePageData(CSettingUIPageDataBase*  pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    //从配置读取Server
    yl::string strServer = configParser_GetConfigString(kszMTSWCommportalServer);

    if (strServer.empty())
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_LOGIN_FAILED), _LANG(TRID_COMMPORTAL_SERVER_EMPTY),
                                               SETTING_MESSAGEBOX_Cancel, "", "", -1);
        return false;
    }

    yl::string strUserName = GetValue(MESW_USER_NAME);
    yl::string strPassword = GetValue(MESW_PASSWORD);

    //先提示用户名
    if (strUserName.empty())
    {
        CSettingUIDataValidInfo dataValid;
        int nDataIndex = GetItemDataIndexByItemId(MESW_USER_NAME);
        dataValid.SetValidIndex(nDataIndex);
        dataValid.SetValidNote(_LANG(TRID_USER_ID_CANNOT_BE_EMPTY));
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);

        return false;
    }

    configParser_SetConfigString(kszMTSWCommportalUser, strUserName.c_str());

    if (strPassword.empty())
    {
        CSettingUIDataValidInfo dataValid;
        int nDataIndex = GetItemDataIndexByItemId(MESW_PASSWORD);
        dataValid.SetValidIndex(nDataIndex);
        dataValid.SetValidNote(_LANG(TRID_PASSWORD_CANNOT_BE_EMPTY));
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        return false;
    }

    configParser_SetConfigString(kszMTSWCommportalPassword, strPassword.c_str());

    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_LOGINING),
                                           SETTING_MESSAGEBOX_Cancel, "", "", -1);
    //登入
    MTSW_Commprotal_Login();

    return true;
}

bool CMtswCommportalDelegate::OnAction(int eActionType, int iDataIndex)
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

CSettingUIPageDataBase* CMtswCommportalDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        yl::string strUserID = configParser_GetConfigString(kszMTSWCommportalUser);
        SetValue(MESW_USER_NAME, strUserID);
    }

    return pPageData;
}

void CMtswCommportalDelegate::InitExtraData(void* pData)
{
    FunMTSWLoginReturnCallBack pReturnCallBack = (FunMTSWLoginReturnCallBack)pData;
    if (NULL != pReturnCallBack)
    {
        m_pReturnCallBack = pReturnCallBack;
    }
}

bool CMtswCommportalDelegate::ProcessMsgCallBack(int nResult, int nID, void* pExtraData /*= NULL*/)
{
    if (MSGBOX_ID_MTSW_LOGINFAILED != nID)
    {
        return false;
    }

    SetValue(MESW_PASSWORD, "");
    g_SetUICallBackFun.RefreshUI(false, GetPageID());
    return true;
}

bool CMtswCommportalDelegate::OnBackAction()
{
    OnReturnCallback(false);
    return true;
}

void CMtswCommportalDelegate::OnReturnCallback(bool bLoginSuccess)
{
    if (NULL != m_pReturnCallBack)
    {
        m_pReturnCallBack(bLoginSuccess);
    }
}

#endif

