#include "calibrationcontroller.h"

#include "basemethod.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CCalibrationController
CCalibrationController * CCalibrationController::m_pController = NULL;

CCalibrationController * CCalibrationController::GetInstance()
{
    // Instantiate the singleton instance.
    if (m_pController == NULL)
    {
        m_pController = new CCalibrationController();
    }
    return m_pController;
}

void CCalibrationController::ReleaseInstance()
{
    // Release the singleton instance.
    DELETE_OBJ(m_pController);
}

bool CCalibrationController::GetPageMenuListByClickAction(
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

const yl::string CCalibrationController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

CCalibrationController::CCalibrationController()
{

}

CCalibrationController::~CCalibrationController()
{

}

}  // namespace NS_SETTING
