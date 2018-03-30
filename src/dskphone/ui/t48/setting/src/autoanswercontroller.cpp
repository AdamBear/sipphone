#include "autoanswercontroller.h"

#include "commonapi/stringhelperapi.h"
#include "setting/include/common.h"
#include "account/include/modaccount.h"

#include "interfacedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

namespace NS_SETTING
{

//////////////////////////////////////////////////////////////////////////
// Class CAutoAnswerController.
CAutoAnswerController * CAutoAnswerController::m_pAutoAnswerController = NULL;

CAutoAnswerController * CAutoAnswerController::GetInstance()
{
    // Instantiate the singleton instance if not exists.
    if (m_pAutoAnswerController == NULL)
    {
        m_pAutoAnswerController = new CAutoAnswerController();

        // Do initialization.
        if (m_pAutoAnswerController != NULL)
        {
            m_pAutoAnswerController->MyInit();
        }
    }
    return m_pAutoAnswerController;
}

void CAutoAnswerController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (m_pAutoAnswerController != NULL)
    {
        // Do finalization.
        m_pAutoAnswerController->MyFinal();

        delete m_pAutoAnswerController;
        m_pAutoAnswerController = NULL;
    }
}

bool CAutoAnswerController::GetAutoAnswerStatusByLineId(int nLineId) const
{
    // Read auto answer status from config.
    return configParser_GetCfgItemIntValue(kszAutoAnswerSwitch, nLineId) == CONFIG_ON;
}

void CAutoAnswerController::SetAutoAnswerStatusByLineId(int nLineId, bool isOn)
{
    // Set auto answer status to config.
    configParser_SetCfgItemIntValue(kszAutoAnswerSwitch, nLineId, isOn ? CONFIG_ON : CONFIG_OFF,
                                    CONFIG_LEVEL_PHONE);

    // 获取当前默认账号.
    int nDefaultAccountId = acc_GetDefaultAccount();
    if (nDefaultAccountId == nLineId)
    {
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_AUTOANSWER, 0);
    }
}

#ifdef IF_FEATURE_AUTO_ANSWER_TIMER
int CAutoAnswerController::GetAutoAnswerTimer() const
{
    int iTime = configParser_GetConfigInt(kszAutoAnswerDelay);
    if (iTime <= 0 || iTime > 4)
    {
        iTime = 3;
    }

    return iTime;
}

void CAutoAnswerController::SetAutoAnswerTimer(int iTimer)
{
    configParser_SetConfigInt(kszAutoAnswerDelay, iTimer, CONFIG_LEVEL_PHONE);
}
#endif

CAutoAnswerController::CAutoAnswerController()
{
    //
}

CAutoAnswerController::~CAutoAnswerController()
{
    //
}

bool CAutoAnswerController::MyInit()
{
    return true;
}

bool CAutoAnswerController::MyFinal()
{
    return true;
}
}
