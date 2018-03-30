#include "phonelockcontroller.h"

#include "basemethod.h"
#include "phonelockmodel.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CPhoneLockController.
CPhoneLockController * CPhoneLockController::m_pController = NULL;

CPhoneLockController * CPhoneLockController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CPhoneLockController();
        // Do initialization.
        if (m_pController != NULL)
        {
            m_pController->MyInit();
        }
    }
    return m_pController;
}

void CPhoneLockController::ReleaseInstance()
{
    // Release the singleton instance if existed.
    if (m_pController != NULL)
    {
        // Do finalization.
        m_pController->MyFinal();

        DELETE_OBJ(m_pController);
    }
}

bool CPhoneLockController::GetPageMenuListByClickAction(
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

const yl::string CPhoneLockController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CPhoneLockController::GetPhoneLockData(PhoneLockData * pData)
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Get phone lock type from model.
    pData->m_ePhoneLockType = m_pModel->GetPhoneLockType();
    return true;
}

void CPhoneLockController::SetPhoneLockData(const PhoneLockData & refData)
{
    if (m_pModel == NULL)
    {
        return;
    }

    // Set phone lock type.
    m_pModel->SetPhoneLockType(refData.m_ePhoneLockType);
}

CPhoneLockController::CPhoneLockController()
    : m_pModel(NULL)
{

}

CPhoneLockController::~CPhoneLockController()
{

}

bool CPhoneLockController::MyInit()
{
    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CPhoneLockModel();
    }
    return m_pModel != NULL;
}

bool CPhoneLockController::MyFinal()
{
    // Release the model instance.
    DELETE_OBJ(m_pModel);
    return m_pModel == NULL;
}

};  // namespace NS_SETTING
