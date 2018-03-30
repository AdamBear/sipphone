#include "callwaitingcontroller.h"

// #include "CommonAPI/CommonAPI.h"
#include "setting/include/common.h"

#include "basemethod.h"
#include "callwaitingmodel.h"
#include "ETLLib.hpp"
#include "etlmsghandler/modetlmsghandler.h"
#include "service_config.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CallWaitingController.
CCallWaitingController * CCallWaitingController::m_pCallWaitController = NULL;

CCallWaitingController * CCallWaitingController::GetInstance()
{
    // Instantiate the singleton instance if not exists.
    if (m_pCallWaitController == NULL)
    {
        m_pCallWaitController = new CCallWaitingController();
        if (m_pCallWaitController != NULL)
        {
            // Do initialization.
            m_pCallWaitController->MyInit();
        }
    }
    return m_pCallWaitController;
}

void CCallWaitingController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (m_pCallWaitController != NULL)
    {
        // Do finalization.
        m_pCallWaitController->MyFinal();

        delete m_pCallWaitController;
        m_pCallWaitController = NULL;
    }
}

bool CCallWaitingController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    // Check the click action.
    if (pData == NULL || strClickAction.empty())
    {
        return false;
    }

    // Call the base method.
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CCallWaitingController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CCallWaitingController::GetCallWaitingData(CallWaitingData * pCallWaitData) const
{
    if (pCallWaitData == NULL || m_pCallWaitingModel == NULL)
    {
        return false;
    }

    // Retrieve the call waiting data.
    pCallWaitData->m_isOn = m_pCallWaitingModel->IsOn();
    pCallWaitData->m_isPlayTone = m_pCallWaitingModel->IsPlayTone();
    pCallWaitData->m_strOnCode = m_pCallWaitingModel->GetOnCode();
    pCallWaitData->m_strOffCode = m_pCallWaitingModel->GetOffCode();
    return true;
}

void CCallWaitingController::SetCallWaitingData(const CallWaitingData & refData)
{
    if (m_pCallWaitingModel != NULL)
    {
        // Set call waiting data.
        m_pCallWaitingModel->SetStatus(refData.m_isOn);
        m_pCallWaitingModel->SetPlayTone(refData.m_isPlayTone);
        m_pCallWaitingModel->SetCode(refData.m_strOnCode, refData.m_strOffCode);

        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_CALLWAIT, 0);
    }
}

CCallWaitingController::CCallWaitingController()
    : m_pCallWaitingModel(NULL)
{

}

CCallWaitingController::~CCallWaitingController()
{

}

bool CCallWaitingController::MyInit()
{
    // Initialize the call waiting model.
    if (m_pCallWaitingModel == NULL)
    {
        m_pCallWaitingModel = new CCallWaitingModel();
    }
    return m_pCallWaitingModel != NULL;
}

bool CCallWaitingController::MyFinal()
{
    // Release the call waiting model.
    if (m_pCallWaitingModel != NULL)
    {
        delete m_pCallWaitingModel;
        m_pCallWaitingModel = NULL;
    }
    return m_pCallWaitingModel == NULL;
}


}  // namespace NS_SETTING
