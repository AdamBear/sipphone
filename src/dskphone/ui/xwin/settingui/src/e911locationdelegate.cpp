#include "e911locationdelegate.h"

#if IF_FEATURE_GENBAND_E911
#include "settinguilogic/src/settinguibasepagedata.h"
#include "translateiddefine.h"
#include "settingui/include/pageiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguimanager.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "e911address/include/mode911address.h"
#include "settinguilogic/include/common.h"

CE911LocationDelegate::CE911LocationDelegate()
{

}

CE911LocationDelegate::~CE911LocationDelegate()
{

}

void CE911LocationDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    vecSoftKey[3].m_iType = ST_RESET;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_RESET);
}

bool CE911LocationDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_RESET == iType)
    {
        OnResetAction();
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

CSettingUIDelegateBase* CE911LocationDelegate::CreateE911LocationDelegate()
{
    CE911LocationDelegate* pDelegate = new CE911LocationDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase* CE911LocationDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);
    if (NULL == m_pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        yl::string strLocation = e911_GetCurrentLocation();
        if (!strLocation.empty())
        {
            AddMenuItem(m_pPageData, "", strLocation, "", false);
        }
    }

    return m_pPageData;
}

bool CE911LocationDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (bPress && PHONE_KEY_OK == nKeyCode)
    {
        OnResetAction();
        return true;
    }

    return false;
}

void CE911LocationDelegate::OnResetAction()
{
    SetRefreshDataWhenTopDelegate(true);
    PageOpenData opPageData(kszPageIDSetE911Location);
    g_SetUICallBackFun.EnterPage(opPageData);
}

#endif // IF_FEATURE_GENBAND_E911
