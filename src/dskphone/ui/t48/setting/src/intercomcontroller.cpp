#include "intercomcontroller.h"

#include "setting/include/common.h"

#include "basemethod.h"
#include "intercommodel.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CIntercomController
CIntercomController * CIntercomController::m_pIntercomController = NULL;

CIntercomController * CIntercomController::GetInstance()
{
    // Instantiate the singleton instance if not exists.
    if (m_pIntercomController == NULL)
    {
        m_pIntercomController = new CIntercomController();
        if (m_pIntercomController != NULL)
        {
            // Do initialize.
            m_pIntercomController->MyInit();
        }
    }
    return m_pIntercomController;
}

void CIntercomController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (m_pIntercomController != NULL)
    {
        // Do finalize.
        m_pIntercomController->MyFinal();

        delete m_pIntercomController;
        m_pIntercomController = NULL;
    }
}

bool CIntercomController::GetPageMenuListByClickAction(
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

const yl::string CIntercomController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CIntercomController::GetIntercomData(IntercomData * pData)
{
    if (pData == NULL || m_pIntercomModel == NULL)
    {
        return false;
    }

    // Read config value from model.
    pData->m_isAllowed = m_pIntercomModel->IsAllowed();
    pData->m_isMuteOn = m_pIntercomModel->IsMuteOn();
    pData->m_isToneOn = m_pIntercomModel->IsToneOn();
    pData->m_isBargeOn = m_pIntercomModel->IsBargeOn();
    return true;
}

void CIntercomController::SetIntercomData(const IntercomData & refData)
{
    if (m_pIntercomModel != NULL)
    {
        // Set config value to model.
        m_pIntercomModel->SetAllowance(refData.m_isAllowed);
        m_pIntercomModel->SetMute(refData.m_isMuteOn);
        m_pIntercomModel->SetTone(refData.m_isToneOn);
        m_pIntercomModel->SetBarge(refData.m_isBargeOn);
    }
}

bool CIntercomController::MyInit()
{
    // Instantiate the intercom model.
    if (m_pIntercomModel == NULL)
    {
        m_pIntercomModel = new CIntercomModel();
    }
    return m_pIntercomModel != NULL;
}

bool CIntercomController::MyFinal()
{
    // Release the intercom model.
    if (m_pIntercomModel != NULL)
    {
        delete m_pIntercomModel;
        m_pIntercomModel = NULL;
    }
    return m_pIntercomModel == NULL;
}

CIntercomController::CIntercomController()
    : m_pIntercomModel(NULL)
{

}

CIntercomController::~CIntercomController()
{

}

}  // namespace NS_SETTING
