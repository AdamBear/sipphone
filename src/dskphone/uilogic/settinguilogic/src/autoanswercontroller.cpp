#include "autoanswercontroller.h"

#include "commonapi/stringhelperapi.h"
#include "settinguilogic/include/common.h"
#include "account/include/modaccount.h"

#include "interfacedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

//////////////////////////////////////////////////////////////////////////
// Class CAutoAnswerController.
CAutoAnswerController * CAutoAnswerController::m_pAutoAnswerController = NULL;

CAutoAnswerController * CAutoAnswerController::GetInstance()
{
    // Instantiate the singleton instance if not exists.
    if (NULL == m_pAutoAnswerController)
    {
        m_pAutoAnswerController = new CAutoAnswerController();

        // Do initialization.
        if (NULL != m_pAutoAnswerController)
        {
            m_pAutoAnswerController->MyInit();
        }
    }
    return m_pAutoAnswerController;
}

void CAutoAnswerController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (NULL != m_pAutoAnswerController)
    {
        // Do finalization.
        m_pAutoAnswerController->MyFinal();

        delete m_pAutoAnswerController;
        m_pAutoAnswerController = NULL;
    }
}

bool CAutoAnswerController::GetAutoAnswerDataById(AutoAnswerData * pAutoAnswerData, int nLineId)
{
#ifdef _CP
    if (NULL == pAutoAnswerData)
    {
        return false;
    }

    pAutoAnswerData->m_isOn = acc_IsAccountAutoAnswerEnable(nLineId);
    pAutoAnswerData->m_IsAutoAnswerMute = acc_IsAccountAutoAnswerMute(nLineId);
    pAutoAnswerData->m_nLineId = nLineId;
#endif
    return true;
}

void CAutoAnswerController::SetAutoAnswerData(const AutoAnswerData & refData)
{
    // Set account auto answer.
#if IF_BUG_21893
    bool bChanged = acc_SetAccountAutoAnswer(refData.m_nLineId, refData.m_isOn, CONFIG_LEVEL_PHONE);
#else
    bool bChanged = acc_SetAccountAutoAnswer(refData.m_nLineId, refData.m_isOn);
#endif

#ifdef _CP
    if (acc_SetAccountAutoAnswerMute(refData.m_nLineId, (refData.m_isOn && refData.m_IsAutoAnswerMute), CONFIG_LEVEL_PHONE))
    {
        bChanged = true;
    }
#endif

    if (bChanged)
    {
        //Notify Config Change
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, refData.m_nLineId);
        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, refData.m_nLineId);
    }
    return;
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
