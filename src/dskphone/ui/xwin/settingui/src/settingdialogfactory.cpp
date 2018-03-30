#include "settingdialogfactory.h"
#include "settingui/include/setuidatadefine.h"

#include "uimanager/moduimanager.h"
#include "uimanager/uimanager_common.h"

IMPLEMENT_GETINSTANCE(CSettingDialogFactory)

CSettingDialogFactory::CSettingDialogFactory()
{
}

CSettingDialogFactory::~CSettingDialogFactory()
{
}

void CSettingDialogFactory::RegisterSettingDialog(const yl::string & strPageID,
        const yl::string & strDialogName)
{
    if (strDialogName.empty() || strPageID.empty())
    {
        return;
    }

    m_mapDialogName[strPageID] = strDialogName;
}

CDlgSettingUI * CSettingDialogFactory::GetSettingDialog(const yl::string & strPageID)
{
    if (m_mapDialogName.end() != m_mapDialogName.find(strPageID))
    {
        return static_cast<CDlgSettingUI *>(UIManager_CreatePrivateDlg(m_mapDialogName[strPageID], ""));
    }

    return NULL;
}

CDlgSettingUI * CSettingDialogFactory::GetDefaultSettingDialog()
{
    return (static_cast<CDlgSettingUI *>(UIManager_CreatePrivateDlg(DLG_SETTINGUI, "")));
}
