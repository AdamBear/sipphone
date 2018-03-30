#include "dndcontroller.h"

#include <ETLLib/ETLLib.hpp>
#include "etlmsghandler/modetlmsghandler.h"
#include "setting/include/common.h"

#include "basemethod.h"

#include <path_define.h>
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#include "talk/fwd_dnd/include/modfwddnd.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CDndController.
CDndController * CDndController::m_pController = NULL;

CDndController * CDndController::GetInstance()
{
    // Instantiate the dnd instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CDndController();
    }
    return m_pController;
}

void CDndController::ReleaseInstance()
{
    // Release the dnd instance.
    DELETE_OBJ(m_pController);
}

bool CDndController::GetPageMenuListByClickAction(
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

const yl::string CDndController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CDndController::GetDndData(DndData * pDndData, int iAccountID/* = -1*/)
{
    if (pDndData == NULL)
    {
        return false;
    }

    if (iAccountID == -1)
    {
        // Phone Mode
        pDndData->m_bEnabled = (configParser_GetConfigInt(kszDndStatus) != 0);
        pDndData->m_strOnCode = configParser_GetConfigString(kszDndOnCode);
        pDndData->m_strOffCode = configParser_GetConfigString(kszDndOffCode);
    }
    else
    {
        // Custom Mode
        pDndData->m_bEnabled = configParser_GetCfgItemIntValue(kszAccountDNDStatus, iAccountID) != 0;
        pDndData->m_strOnCode = configParser_GetCfgItemStringValue(kszAccountDNDOnCode, iAccountID);
        pDndData->m_strOffCode = configParser_GetCfgItemStringValue(kszAccountDNDOffCode, iAccountID);
    }
    return true;
}

bool CDndController::SetDndData(const DndData & refDndData, int iAccountID/* = -1*/)
{
    bool bChanged = false;
    if (iAccountID == -1)
    {
        // Phone Mode
        bChanged = configParser_SetConfigInt(kszDndStatus, refDndData.m_bEnabled, CONFIG_LEVEL_PHONE)
                   || bChanged;
        bChanged = configParser_SetConfigString(kszDndOnCode, refDndData.m_strOnCode.c_str(),
                                                CONFIG_LEVEL_PHONE) || bChanged;
        bChanged = configParser_SetConfigString(kszDndOffCode, refDndData.m_strOffCode.c_str(),
                                                CONFIG_LEVEL_PHONE) || bChanged;
    }
    else
    {
        // Custom Mode
        bChanged = configParser_SetCfgItemIntValue(kszAccountDNDStatus, iAccountID, refDndData.m_bEnabled,
                   CONFIG_LEVEL_PHONE) || bChanged;
        bChanged = configParser_SetCfgItemStringValue(kszAccountDNDOnCode, iAccountID,
                   refDndData.m_strOnCode, CONFIG_LEVEL_PHONE) || bChanged;
        bChanged = configParser_SetCfgItemStringValue(kszAccountDNDOffCode, iAccountID,
                   refDndData.m_strOffCode, CONFIG_LEVEL_PHONE) || bChanged;
    }

    if (bChanged)
    {
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_DND, 0);
    }
    return bChanged;
}

CDndController::CDndController()
{

}

CDndController::~CDndController()
{

}

}  // namespace NS_SETTING
