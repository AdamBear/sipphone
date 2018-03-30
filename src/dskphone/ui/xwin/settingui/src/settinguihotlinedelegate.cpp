#include "settinguihotlinedelegate.h"

#if IF_SUPPORT_SELECT_CONTACT_INFO
#include "settinguimanager.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"

CSettingUIHotLineDelegate::CSettingUIHotLineDelegate()
{

}

CSettingUIHotLineDelegate::~CSettingUIHotLineDelegate()
{

}

CSettingUIDelegateBase* CSettingUIHotLineDelegate::CreateSettingUIHotLineDelegate()
{
    CSettingUIHotLineDelegate* pDelegate = new CSettingUIHotLineDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CSettingUIHotLineDelegate::UpdateContactInfo(const yl::string& strContactName, const yl::string& strContactNumber)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDHotLine);
    if (NULL == pDelegate)
    {
        return;
    }

    CSettingUIHotLineDelegate* pHotLineDelegate = static_cast<CSettingUIHotLineDelegate*>(pDelegate);
    if (NULL == pHotLineDelegate)
    {
        return;
    }

    pHotLineDelegate->UpdateHotlineData(strContactNumber);
}

void CSettingUIHotLineDelegate::UpdateHotlineData(const yl::string& strContactNumber)
{
    SetValue(HOTLINE_NUMBER, strContactNumber);
}

bool CSettingUIHotLineDelegate::IsShowDirectorySoftkey()
{
    yl::string strItemID = GetItemIDByDataIndex(GetFocusIndex());
    yl::string strValue = GetValue(strItemID);
    if (HOTLINE_NUMBER == strItemID && strValue.empty())
    {
        return true;
    }

    return false;
}

bool CSettingUIHotLineDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_DIRECTORY == iType)
    {
        SettingUI_EnterPage(kszPageIDDirectory, (void*)&CSettingUIHotLineDelegate::UpdateContactInfo);
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}
#endif
