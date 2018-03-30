#include "autoprovisioncontroller.h"

#include "setting/include/common.h"

#include "autoprovisionmodel.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "basemethod.h"
#include "ETLLib/ETLLib.hpp"
#include "interfacedefine.h"

#if IF_BUG_30885
#include "modzero.h"
#endif

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CAutoProvisionController.
CAutoProvisionController * CAutoProvisionController::m_pController = NULL;

CAutoProvisionController * CAutoProvisionController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CAutoProvisionController();
        if (m_pController != NULL)
        {
            // Do initialization.
            m_pController->MyInit();
        }
    }
    return m_pController;
}

void CAutoProvisionController::ReleaseInstance()
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

bool CAutoProvisionController::GetPageMenuListByClickAction(
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

const yl::string CAutoProvisionController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CAutoProvisionController::GetAutoProvisionData(AutoProvisionData * pData)
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Read auto provision URL.
    pData->m_strUrl = m_pModel->GetUrl();
    // Read auto provision username.
    pData->m_strUsername = m_pModel->GetPUserName();
    // Read auto provision password.
    pData->m_strPassword = m_pModel->GetPassword();
    // Get AES key.
    pData->m_strAesKey = m_pModel->GetAesKey();
    // Get AES MAC key.
    pData->m_strAesMacKey = m_pModel->GetAesMacKey();

    return true;
}

void CAutoProvisionController::SetAutoProvisionData(
    const AutoProvisionData & refData, bool bSaveUrl /*= true*/)
{
    if (m_pModel != NULL)
    {
        if (bSaveUrl)
        {
            // Set auto provision URL.
            m_pModel->SetUrl(refData.m_strUrl);
        }

        Autop_SetAuthInfo(refData.m_strUsername, refData.m_strPassword);

        // Set AES key.
        m_pModel->SetAesKey(refData.m_strAesKey);
        // Set AES MAC key.
        m_pModel->SetAesMacKey(refData.m_strAesMacKey);
    }
}

bool CAutoProvisionController::BeginAutoProvision()
{
#if IF_BUG_30885
    zero_BeginAutop();
#else
    // TODO: Begin auto provision.
    static char kszAutopThreadNamep[] = "autoServer";
    msgPostMsgToThread(msgGetThreadByName(kszAutopThreadNamep), ATP_MSG_REQ_START_BY_DEFAULT, 0, 0);
#endif
    return true;
}

bool CAutoProvisionController::GetAuthenticationInfo(AutoProvisionData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    pData->m_strUsername = m_pModel->GetAuthenticationUserName();
    pData->m_strPassword = m_pModel->GetAuthenticationassword();

    return true;
}

void CAutoProvisionController::StartCurrentConnect(AutoProvisionData * pData)
{
    if (NULL == pData)
    {
        return;
    }

    m_pModel->StartCurrentConnect(pData->m_strUsername, pData->m_strPassword);
}

CAutoProvisionController::CAutoProvisionController()
    : m_pModel(NULL)
{

}

CAutoProvisionController::~CAutoProvisionController()
{

}

bool CAutoProvisionController::MyInit()
{
    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CAutoProvisionModel();
    }
    return m_pModel != NULL;
}

bool CAutoProvisionController::MyFinal()
{
    // Release the model instance.
    DELETE_OBJ(m_pModel);
    return m_pModel == NULL;
}

}  // namespace NS_SETTING
