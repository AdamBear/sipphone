#include "settinguinetworkchangebasedelegate.h"

#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/networksettingmanager.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "uicallbackfun.h"
#include "settingui/include/setuidatadefine.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "settingui/include/modsettingui.h"

namespace
{
#define     MSG_ID_IF_REBOOT  1025
}

CSettingUINetworkChangeBaseDelegate::CSettingUINetworkChangeBaseDelegate()
{

}

CSettingUINetworkChangeBaseDelegate::~CSettingUINetworkChangeBaseDelegate()
{

}

CSettingUIDelegateBase* CSettingUINetworkChangeBaseDelegate::CreatNetworkChangeBaseDelegate()
{
    CSettingUINetworkChangeBaseDelegate* pDelegate = new CSettingUINetworkChangeBaseDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUINetworkChangeBaseDelegate::ProcessMsgCallBack(int nResult, int nID, void* pExtraData /*= NULL*/)
{
    if (MSGBOX_ID_SAVE == nID && //设置网络改变标记
            _NetworkChangeFlagManager.GetNetworkChangeFlag())
    {
        _NetworkChangeFlagManager.ClearChangeFlag();
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING), _LANG(TRID_NETWORK_CHANGED_IF_TAKE_EFFECT_AFTER_RESTART)
                                               , SETTING_MESSAGEBOX_OkCancel, "", "", -1
                                               , MSG_ID_REBOOT);
        return true;
    }
    else if (MSG_ID_REBOOT == nID && SETTING_MessageBox_OK == nResult)
    {
        SettingUI_Reboot(0);
        return true;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);

}

#endif
