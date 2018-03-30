#include "setaeskeycontroller.h"

#include "setting/include/common.h"

#include "basemethod.h"
#include "setaeskeymodel.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CSetAesKeyController.
CSetAesKeyController * CSetAesKeyController::m_pController = NULL;

CSetAesKeyController * CSetAesKeyController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CSetAesKeyController();
        // Do initialization.
        if (m_pController != NULL)
        {
            m_pController->MyInit();
        }
    }
    return m_pController;
}

void CSetAesKeyController::ReleaseInstance()
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

bool CSetAesKeyController::GetPageMenuListByClickAction(
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

const yl::string CSetAesKeyController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CSetAesKeyController::GetAesKeyData(AesKeyData * pData) const
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Get AES key.
    pData->m_strAesKey = m_pModel->GetAesKey();
    // Get AES MAC key.
    pData->m_strAesMacKey = m_pModel->GetAesMacKey();
    return true;
}

void CSetAesKeyController::SetAesKeyData(const AesKeyData & refData)
{
    if (m_pModel != NULL)
    {
        // Set AES key.
        m_pModel->SetAesKey(refData.m_strAesKey);
        // Set AES MAC key.
        m_pModel->SetAesMacKey(refData.m_strAesMacKey);
    }
}

CSetAesKeyController::CSetAesKeyController()
    : m_pModel(NULL)
{

}

CSetAesKeyController::~CSetAesKeyController()
{

}

bool CSetAesKeyController::MyInit()
{
    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CSetAesKeyModel();
    }
    return m_pModel != NULL;
}

bool CSetAesKeyController::MyFinal()
{
    // Release the model instance.
    DELETE_OBJ(m_pModel);
    return m_pModel != NULL;
}

}  // namespace NS_SETTING
