#include "settingmsgboxmgr.h"
#include "setuidatadefine.h"
#include "translateiddefine.h"
#include "commonunits/modcommonunits.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "dsklog/log.h"
#include "baseui/include/fullscreenmessagebox.h"
#include "messagebox/modmessagebox.h"
#include "account/include/modaccount.h"
#include "xwindow/xWindowManager.h"

#if IF_UNLOCK_CHECK_POPBOX
#include "keyguard/include//keyguardmessagedefine.h"
#include "keyguard/include/modkeyguard.h"
#include "settingui/include/modsettingui.h"
#include "uimanager/moduimanager.h"
#include "messagebox/passwordmessagebox.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/include/settinguicommondata.h"
#endif


namespace
{
// 重启、重置、升级中弹出框显示时间
#define POPUP_BOX_SHOWING_TIME 500
#if IF_UNLOCK_CHECK_POPBOX
#define MSG_ID_UNLOCK_CHECK         1501
#define MSG_ID_UNLOCK_CHECK_ERROR   1502
#endif
}


///////////////////////////////////CAsyncOperateAction///////////////////////////////////////
CAsyncOperateAction::CAsyncOperateAction()
{

}

CAsyncOperateAction::~CAsyncOperateAction()
{
    g_SettingMsgBoxMgr.RemoveAsyncOperate(this);
}
///////////////////////////////////CSettingMsgBoxMgr///////////////////////////////////////
IMPLEMENT_GETINSTANCE(CSettingMsgBoxMgr)

CSettingMsgBoxMgr::CSettingMsgBoxMgr()
    : m_bWaitSystemOperate(false)
    , m_nSystemOperateData(0)
    , m_bReseted(false)
#if IF_UNLOCK_CHECK_POPBOX
    , m_bEnterBySetting(false)
#endif
{

}

CSettingMsgBoxMgr::~CSettingMsgBoxMgr()
{

}

void CSettingMsgBoxMgr::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    switch (pMessageBox->GetID())
    {
    case MSG_ID_RESETTING:
        {
            OnResetting();
        }
        break;
    case MSG_ID_FIRWARE_UPDATING:
        {
            OnUpdateing();
        }
        break;
    case MSG_ID_UPLOAD_CONFIG:
        {
            OnUploading();
        }
        break;
#if IF_UNLOCK_CHECK_POPBOX
    case MSG_ID_UNLOCK_CHECK_ERROR:
        {
            ShowUnlockPwdMsgBox();
        }
        break;
    case MSG_ID_UNLOCK_CHECK:
        {
            OnUnlockCheckCallback(pMessageBox);
        }
        break;
#endif
    default:
        break;
    }
}

void CSettingMsgBoxMgr::ShowReboot(int nTimeDelay)
{
    ShowRebootMsgBox(nTimeDelay, _LANG(TRID_REBOOTING_PLEASE_WAIT));
}

void CSettingMsgBoxMgr::ShowRebootMsgBox(int nTimeDelay, const yl::string & strNote)
{
    if (m_bWaitSystemOperate)
    {
        return;
    }

    MessageBox_RemoveAllMessageBox(MessageBox_Destory);

    m_bWaitSystemOperate = true;
    m_nSystemOperateData = nTimeDelay;

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = (MESSAGEBOX_TYPE)MESSAGEBOX_FULLSCREEN;
    stMsgInfo.strNote = strNote;
    stMsgInfo.nMsTime = -1;
    stMsgInfo.pAction = &g_SettingMsgBoxMgr;
    stMsgInfo.iID = MSG_ID_REBOOTING;

    MessageBox_ShowCommonMsgBox(stMsgInfo);

    OnRebooting();
}

void CSettingMsgBoxMgr::ShowResetFactory(int nResetType)
{
    if (m_bWaitSystemOperate)
    {
        return;
    }

    if (RESET_OPTION_ALL == nResetType)
    {
        MessageBox_RemoveAllMessageBox(MessageBox_Destory);
    }

    if (m_bReseted)
    {
        m_bReseted = false;
        commonUnits_ResetToFatory((ResetOptionType)nResetType);
        return;
    }

    m_bWaitSystemOperate = true;
    m_nSystemOperateData = nResetType;

    MSGBOX_INFO stMsgInfo;
    stMsgInfo.eType = (MESSAGEBOX_TYPE)MESSAGEBOX_FULLSCREEN;
    stMsgInfo.strNote = _LANG(GetResetStringByType(nResetType));
    stMsgInfo.nMsTime = POPUP_BOX_SHOWING_TIME;
    stMsgInfo.pAction = &g_SettingMsgBoxMgr;
    stMsgInfo.iID = MSG_ID_RESETTING;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

void CSettingMsgBoxMgr::ShowUploadConfig()
{
    if (m_bWaitSystemOperate)
    {
        return;
    }

    m_bWaitSystemOperate = true;

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = (MESSAGEBOX_TYPE)MESSAGEBOX_FULLSCREEN;
    stMsgInfo.strNote = _LANG(TRID_IMPORT_CONFIGURATION_PLEASE_DO_NOT_POWER_OFF);
    stMsgInfo.nMsTime = POPUP_BOX_SHOWING_TIME;
    stMsgInfo.pAction = &g_SettingMsgBoxMgr;
    stMsgInfo.iID = MSG_ID_UPLOAD_CONFIG;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

void CSettingMsgBoxMgr::ShowUpdateFirware()
{
    if (m_bWaitSystemOperate)
    {
        return;
    }

    m_bWaitSystemOperate = true;

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = (MESSAGEBOX_TYPE)MESSAGEBOX_FULLSCREEN;
    stMsgInfo.strNote = _LANG(TRID_UPDATING_FIRMWARE);
    stMsgInfo.nMsTime = POPUP_BOX_SHOWING_TIME;
    stMsgInfo.pAction = &g_SettingMsgBoxMgr;
    stMsgInfo.iID = MSG_ID_FIRWARE_UPDATING;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

void CSettingMsgBoxMgr::OnRebooting()
{
    int nTimeDelay = m_nSystemOperateData;

    m_bWaitSystemOperate = false;

    SETTING_INFO("Rebooting, Delay Time is %d", nTimeDelay);

    commonUnits_Reboot(nTimeDelay);
}

void CSettingMsgBoxMgr::OnResetting()
{
    int nResetType = m_nSystemOperateData;

    m_bWaitSystemOperate = false;

    if (RESET_OPTION_USERDATA_USERCFG == nResetType)
    {
        MSGBOX_INFO stMsgInfo;
        stMsgInfo.eType = (MESSAGEBOX_TYPE)MESSAGEBOX_FULLSCREEN;
        stMsgInfo.strNote = _LANG(GetResetStringByType(nResetType));
        stMsgInfo.nMsTime = -1;
        stMsgInfo.pAction = &g_SettingMsgBoxMgr;
        stMsgInfo.iID = -1;
        MessageBox_ShowCommonMsgBox(stMsgInfo);
        m_bReseted = true;
    }

    SETTING_INFO("Resetting, Reset Type is %d", nResetType);

#if IF_BUG_20555
    if (RESET_OPTION_ALL == nResetType)
    {
        acc_LogoutBeforeReset();
    }
#endif  //IF_BUG_20555

    commonUnits_ResetToFatory((ResetOptionType)nResetType);
}

void CSettingMsgBoxMgr::OnUploading()
{
    m_bWaitSystemOperate = false;

    etlSleep(1000 * 1000);
}

void CSettingMsgBoxMgr::OnUpdateing()
{
    m_bWaitSystemOperate = false;

    // 等待1小时
    etlSleep(3600000);
}

yl::string CSettingMsgBoxMgr::GetResetStringByType(int nResetType)
{
    yl::string strRebootTip = "";
    if (RESET_OPTION_USERDATA_ONLY == nResetType)
    {
        strRebootTip = TRID_RESETTING_USEDATA_PLEASE_WAIT;
    }
    else if (RESET_OPTION_CONFIG_ONLY == nResetType)
    {
        strRebootTip = TRID_RESETTING_CONFIGURATION_PLEASE_WAIT;
    }
    else if (RESET_OPTION_ALL == nResetType)
    {
        strRebootTip = TRID_RESETTING_TO_FACTORY_PLEASE_WAIT;
    }
    else if (RESET_OPTION_USERDATA_USERCFG == nResetType)
    {
        strRebootTip = TRID_RESETTING_USERDATA_CFG_PLEASE_WAIT;
    }
    else
    {
        SETTINGUI_WARN("qApp_ResetFactory Type Error, ResetType[%d].", nResetType);
        strRebootTip = TRID_RESETTING_TO_FACTORY_PLEASE_WAIT;
    }

    return strRebootTip;
}

void CSettingMsgBoxMgr::AddAsyncOperate(CAsyncOperateAction* pAsyncOperateAction)
{
    if (NULL == pAsyncOperateAction)
    {
        return;
    }

    m_lsAysncOperateAction.push_back(pAsyncOperateAction);
    g_WorkStation.postThreadMsgEx(VIEW_EVENT_NONAME, 0, 0,
                                  MSG_METHOD(this, CSettingMsgBoxMgr::OnAsyncOperate));
}

void CSettingMsgBoxMgr::RemoveAsyncOperate(CAsyncOperateAction* pAsyncOperateAction /*= NULL*/)
{
    if (NULL == pAsyncOperateAction)
    {
        return;
    }

    LIST_ASYNC_OPERATE_ACTION::iterator it = m_lsAysncOperateAction.begin();
    for (; it != m_lsAysncOperateAction.end(); ++it)
    {
        CAsyncOperateAction* pAction = *it;
        if (NULL != pAction && pAction == pAsyncOperateAction)
        {
            m_lsAysncOperateAction.erase(it);
            break;
        }
    }
}

#if IF_UNLOCK_CHECK_POPBOX
void CSettingMsgBoxMgr::ShowUnlockCheckMsgBox(bool bEnterBySetting /*= false*/)
{
    m_bEnterBySetting = bEnterBySetting;
    ShowUnlockPwdMsgBox();
}

void CSettingMsgBoxMgr::ShowUnlockPwdMsgBox()
{
    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = MESSAGEBOX_PWD;
    stMsgInfo.strNote = _LANG(TRID_PHONE_LOCK);
    stMsgInfo.nMsTime = -1;
    stMsgInfo.pAction = &g_SettingMsgBoxMgr;
    stMsgInfo.iID = MSG_ID_UNLOCK_CHECK;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

void CSettingMsgBoxMgr::OnUnlockCheckCallback(CMessageBoxBase* pMessageBox)
{
    if (NULL == pMessageBox || MESSAGEBOX_PWD != pMessageBox->GetType()
            || MessageBox_OK != pMessageBox->GetResult())
    {
        return;
    }

    CPasswordMessageBox* pPwdMsgBox = (CPasswordMessageBox*)pMessageBox;

    if (NULL == pPwdMsgBox)
    {
        return;
    }

    yl::string strPwd = pPwdMsgBox->getPassword();
    PinType ePinType = keyGuard_GetPinType(strPwd);

    if (PIN_INCORRECT == ePinType)
    {
        MSGBOX_INFO stMsgInfo;

        stMsgInfo.eType = MESSAGEBOX_Cancel;
        stMsgInfo.strNote = _LANG(TRID_CURRENT_PIN_IS_INCORRECT);
        stMsgInfo.nMsTime = NS_SET::g_SetPopNormalTime;
        stMsgInfo.pAction = &g_SettingMsgBoxMgr;
        stMsgInfo.iID = MSG_ID_UNLOCK_CHECK_ERROR;

        MessageBox_ShowCommonMsgBox(stMsgInfo);
    }
    else if (PIN_CORRECT_ADMIN == ePinType)
    {
        // 输入了admin密码,解锁密码
        keyGuard_Lock(false);

#if IF_BUG_21722
        if (KG_MENU_KEY == keyGuard_GetKeyGuardType())
        {
            UIManager_ReturnToIdle();
        }
#else
        // basic setting进入phone lock, 输入admin密码就可以进入PAGE_ADV_LOCK界面
        if (m_bEnterBySetting)
        {
            SettingUI_EnterPage(kszPageIDPhoneLock);
        }
#endif
        // 进入重置密码界面
        bool bPwdVerified = true;
        SettingUI_EnterPage(kszPageIDChangePin, (void *)(&bPwdVerified));
    }
    else if (PIN_CORRECT_LOCK == ePinType)
    {
        // 普通解锁密码验证通过,解锁并退出界面
        keyGuard_Lock(false);
#if IF_BUG_21722
        if (KG_MENU_KEY == keyGuard_GetKeyGuardType())
        {
            UIManager_ReturnToIdle();
            SettingUI_EnterPage(kszPageIDMainMenu);
        }
#else
        if (m_bEnterBySetting)
        {
            SettingUI_EnterPage(kszPageIDPhoneLock);
        }
#endif
    }
}

#endif

BOOL CSettingMsgBoxMgr::OnAsyncOperate(msgObject & objMessage)
{
    while (0 != m_lsAysncOperateAction.size())
    {
        CAsyncOperateAction* pAction = m_lsAysncOperateAction.back();
        m_lsAysncOperateAction.pop_back();
        if (NULL != pAction)
        {
            pAction->AsyncOperateCallBack();
        }
    }
    return TRUE;
}
