#include "resetandrebootcontroller.h"
#include <unistd.h>
#include "ETLLib.hpp"
#include "interfacedefine.h"
#include "setting/include/common.h"
#include "setting/include/modsetting.h"

#include "basemethod.h"
#include "resetandrebootmodel.h"

#include "ETLLib.hpp"
#include "devicelib/phonedevice.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "path_define.h"

#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "commonunits/modcommonunits.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "adapterbox/include/modadapterbox.h"

// 通话中重启的每次的检测时间
#define  REBOOT_IN_TALKING_CHECK_TIME  3000

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CResetAndRebootController.
CResetAndRebootController * CResetAndRebootController::m_pController = NULL;

CResetAndRebootController * CResetAndRebootController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CResetAndRebootController();
        // Do initialization.
        if (m_pController != NULL)
        {
            m_pController->MyInit();
        }
    }
    return m_pController;
}

void CResetAndRebootController::ReleaseInstance()
{
    // Release the singleton instance if existed.
    if (m_pController != NULL)
    {
        // Do finalization.
        m_pController->MyFinal();

        delete m_pController;
        m_pController = NULL;
    }
}

// Static.
LRESULT CResetAndRebootController::OnMessage(msgObject & refObj)
{
    switch (refObj.message)
    {
    case TM_TIMER:
        {
            if (NULL != m_pController && refObj.wParam == m_pController->GetRebootMsgID())
            {
                m_pController->OnRebootTimer();
                return TRUE;
            }
        }
        break;
    default:
        break;
    }

    return FALSE;
}

bool CResetAndRebootController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    // Check the click action.
    if (strClickAction.empty() || pData == NULL)
    {
        return false;
    }

    // Call the base method.
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CResetAndRebootController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CResetAndRebootController::GetResetData(ResetToFactoryData * pData)
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Read reset-option from config file.
    pData->m_eType = static_cast<ResetOptionType>(m_pModel->GetResetOption());
    return true;
}

void CResetAndRebootController::SetResetData(const ResetToFactoryData & refData)
{
    if (m_pModel != NULL)
    {
        // Set reset-option to config file.
        m_pModel->SetResetOption(refData.m_eType);
    }
}

bool CResetAndRebootController::ResetNow(int nOption)
{
    AdapterBox_ResetFactory(nOption);
    return true;
}

// 延迟重启定时器响应函数
void CResetAndRebootController::OnRebootTimer()
{
    if (talklogic_SessionExist())
    {
        timerSetThreadTimer(GetRebootMsgID(), REBOOT_IN_TALKING_CHECK_TIME);
        return;
    }
    SETTING_INFO("On Reboot time out");
    timerKillThreadTimer(GetRebootMsgID());

    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LIGHT_STATUS_CHANGE, 1, 1);

    // 设备重启
    ShowRebootTip(TRID_REBOOTING_PLEASE_WAIT);
    commonUnits_Reboot(m_iReboot);
}

bool CResetAndRebootController::RebootNow(int iTimeDelay/* = 1*/)
{
    SETTINGUI_INFO("CResetAndRebootController::RebootNow");
    m_iReboot = chMAX(iTimeDelay, 1);
    if (talklogic_SessionExist())
    {
        SETTING_WARN("Warning : Talking now, end talk will reboot!");
#if IF_BUG_20000
        if (configParser_GetConfigInt(kszRebootInTalking) != 0)
        {
            talklogic_ExitAllSession();
            timerSetThreadTimer(GetRebootMsgID(), 1);
        }
        else
#endif  //IF_BUG_20000
        {
            timerSetThreadTimer(GetRebootMsgID(), REBOOT_IN_TALKING_CHECK_TIME);
        }
        return false;
    }

    // 设备重启
    ShowRebootTip(TRID_REBOOTING_PLEASE_WAIT);
    commonUnits_Reboot(m_iReboot);
    return true;
}

CResetAndRebootController::CResetAndRebootController()
    : m_pModel(NULL)
    , m_funRebootTip(NULL)
    , m_iReboot(1)
{

}

CResetAndRebootController::~CResetAndRebootController()
{

}

bool CResetAndRebootController::MyInit()
{
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnMessage);

    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CResetAndRebootModel();
    }
    return m_pModel != NULL;
}

bool CResetAndRebootController::MyFinal()
{
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnMessage);

    // Release the model instance.
    if (m_pModel != NULL)
    {
        delete m_pModel;
        m_pModel = NULL;
    }
    return m_pModel == NULL;
}

void CResetAndRebootController::RegisterRebootTipCallBack(FunRebootTipCallBack fun)
{
    m_funRebootTip = fun;
}

void CResetAndRebootController::ShowRebootTip(const yl::string & strTip)
{
    if (NULL != m_funRebootTip)
    {
        m_funRebootTip(strTip);
    }
}

}  // namespace NS_SETTING


