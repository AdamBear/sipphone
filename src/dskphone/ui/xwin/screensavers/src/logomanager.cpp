#include "logomanager.h"

#if IF_IDLE_SUPPORT_LOGO
#include "idle/idle_data_define.h"
#include <configiddefine.h>

#include "account/include/modaccount.h"
#include "idlescreen/include/modidlescreen.h"
#include "idleuilogic/include/modidleul.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "contacts/directory/include/moddirinterface.h"
#include "commonunits/modcommonunits.h"
#include "devicelib/networkserver.h"
#include "record/include/modrecord.h"
#include "uimanager/moduimanager.h"
#include "idlescreenui/idlescreenui.h"
#include "resource/modresource.h"
#include "commonunits/t2xpath.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "messagebox/modmessagebox.h"

namespace
{
#define LOGO_TYPE_OFF                       0
#define LOGO_TYPE_SYSTEM                    1
#define LOGO_TYPE_CUSTOM                    2

#define LOGO_SHOW_TIME_DEFAULT              3
#define LOGO_DELAY_SHOW_TIME_DEFAULT        3
#define LOGO_TIME_ONE_SECOND                1000
}

IMPLEMENT_GETINSTANCE(CLogoManager)
CLogoManager::CLogoManager()
    : m_uDelayShowLogoTime(LOGO_DELAY_SHOW_TIME_DEFAULT)
    , m_uShowLogoTime(LOGO_SHOW_TIME_DEFAULT)
    , m_bShowLogo(false)
    , m_bCurShowLogo(false)
    , m_strLogoPic("")
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CLogoManager::OnConfigChangeMsg);
}

CLogoManager::~CLogoManager()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CLogoManager::OnConfigChangeMsg);
}

LRESULT CLogoManager::OnConfigChangeMsg(msgObject & msg)
{
    if (ST_LOGO != msg.wParam)
    {
        return FALSE;
    }
    // 根据配置重置logo属性
    _LogoManager.InitLogo();
    return TRUE;
}

void CLogoManager::InitLogo()
{
    //上电后读取显示logo的延迟时间
    m_uDelayShowLogoTime = configParser_GetConfigInt(kszLogoDelayTime) * LOGO_TIME_ONE_SECOND;
    //显示Logo时间
    m_uShowLogoTime = configParser_GetConfigInt(kszLogoShowTime) * LOGO_TIME_ONE_SECOND;
    if (m_uShowLogoTime <= 0)
    {
        m_uShowLogoTime = LOGO_SHOW_TIME_DEFAULT * LOGO_TIME_ONE_SECOND;
    }

    int iLogoType = LOGO_TYPE_OFF;
    //上电后读取是否显示logo
    iLogoType = configParser_GetConfigInt(kszLCDLogo);
    m_bShowLogo = (LOGO_TYPE_OFF != iLogoType);

    yl::string strSysLogPath = res_getPathByName(RESOURCE_PATH_SYSTEMLOGO);
    yl::string strLogoFilePath = commonAPI_FormatString("%s%s", strSysLogPath.c_str(),
                                 SYSTELM_LOGO_FILE);

    if (LOGO_TYPE_SYSTEM == iLogoType)
    {
        if (pathFileExist(strLogoFilePath.c_str()))
        {
            m_strLogoPic = strLogoFilePath;
        }
    }
    else if (LOGO_TYPE_CUSTOM == iLogoType)
    {
        if (pathFileExist(szCustomLogoFile))
        {
            m_strLogoPic = szCustomLogoFile;
        }
        else if (pathFileExist(strLogoFilePath.c_str()))
        {
            m_strLogoPic = strLogoFilePath;
        }
    }
    CheckLogoShow();
}

bool CLogoManager::IsNeedToShowLogo()
{
    //如果未开启显示logo功能
    if (!m_bShowLogo)
    {
        return false;
    }

    bool bHasNewVoice = false;
    for (int iIndex = 0; iIndex < acc_AccountNum(); iIndex++)
    {
        if (acc_GetUnReadVoiceMailNum(iIndex) > 0)
        {
            bHasNewVoice = true;
            break;
        }
    }

    if (m_uDelayShowLogoTime > 0
            && UIManager_IsInIdle()
            && 0 == MessageBox_GetMessageBoxCount()
            && 0 == Dir_GetContactCount(kszMissNotifId, LOCAL_LOG)
            && 0 == Dir_GetContactCount(kszForwardNotifId, LOCAL_LOG)
            && 0 == SettingUILogic_GetTotalUnReadMessage()
            && !bHasNewVoice
            && netIsNetworkReady()
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
            && !modRecord_DetectFailed()
#endif
            && !commonUnits_IsSystemMaintenanceStatus()
       )
    {
        return true;
    }
    return false;
}

void CLogoManager::ResetLogo()
{
    //Idle界面显示logo
    if (m_bCurShowLogo)
    {
        CancelLogo();
    }

    CheckLogoShow();
}

void CLogoManager::CancelLogo()
{
    m_bCurShowLogo = false;
    ShowLogo(false, "");
    m_timerLogo.KillTimer();
}

void CLogoManager::CheckLogoShow()
{
    if (!IsNeedToShowLogo())
    {
        CancelLogo();
        return;
    }

    m_timerLogo.SingleShot(m_uDelayShowLogoTime, TIMER_METHOD(this, CLogoManager::OnLogoTimer));
}

void CLogoManager::OnLogoTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    if (IsNeedToShowLogo())
    {
        if (m_bCurShowLogo)
        {
            ShowLogo(false, "");
            m_bCurShowLogo = false;
            m_timerLogo.SingleShot(m_uDelayShowLogoTime, TIMER_METHOD(this, CLogoManager::OnLogoTimer));
        }
        else
        {
            ShowLogo(true, m_strLogoPic);
            m_bCurShowLogo = true;
            m_timerLogo.SingleShot(m_uShowLogoTime, TIMER_METHOD(this, CLogoManager::OnLogoTimer));
        }
    }
}

void CLogoManager::ShowLogo(bool bShow, const yl::string & strPic)
{
    CIdleScreenUI * pIdleUI = g_pIdleScreenUI;
    if (NULL == pIdleUI)
    {
        return;
    }
    pIdleUI->ShowLogo(bShow, strPic);
}
#endif
