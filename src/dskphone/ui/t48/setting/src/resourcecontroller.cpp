#include "resourcecontroller.h"

#include "basemethod.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CResourceController.
CResourceController * CResourceController::m_pController = NULL;

CResourceController * CResourceController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CResourceController();
    }
    return m_pController;
}

void CResourceController::ReleaseInstance()
{
    // Release the singleton instance if existed.
    DELETE_OBJ(m_pController);
}

bool CResourceController::GetPageMenuListByClickAction(
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

const yl::string CResourceController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CResourceController::GetUsbFileListData(UsbFileListData * pData)
{
    // TODO: Call other module to get USB file list.
    return true;
}

bool CResourceController::GetSdFileListData(SdFileListData * pData)
{
    // TODO: Call other module to get SD file list.
    return true;
}

CResourceController::CResourceController()
{

}

CResourceController::~CResourceController()
{

}

}  // namespace NS_SETTING
