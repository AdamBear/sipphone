#include "modzerotouchui.h"
#include "dlgzerotouch.h"

#include "uimanager/uimanager_inc.h"

#include "talk/talklogic/include/modtalklogic.h"
#include "dsklog/log.h"
#include "autopcontrol/include/modautopcontrol.h"

#include "delegatefactory.h"
#include "settingdialogfactory.h"
#include "settingadapterfactory.h"
#include "settinguicomboboxadapter.h"

#include "zerostatusdelegate.h"
#include "zeronetworkdelegate.h"
#include "zeroautopdelegate.h"
#include "zeroredirectdelegate.h"
#include "zero_common.h"

#include "settingui/include/pageiddefine.h"
#include "settingui/include/modsettingui.h"
#include "settinguilogic/include/modsetuilogic.h"

#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/dsskey_enumtypes.h"

class CDssKeyZero : public IDsskeyImplAction
{
public:
    CDssKeyZero(): IDsskeyImplAction(DT_ZERO) {}

    // 按键按下的处理
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
    {
        // 如果存在通话
        if (talklogic_SessionExist())
        {
            return false;
        }
        SettingUILogic_EnterZero();
        return true;
    }
};


void ZeroTouchUI_Init()
{
    REGISTER_DLG(CDlgZeroTouch);

    g_SettingDialogFactory.RegisterSettingDialog(kszPageIDZeroTouch, DLG_CDlgZeroTouch);
    // zero delegate
    g_DelegateFactory.RegisterDelegate(kszPageIDZeroStatus,
                                       &CZeroStatusDelegate::CreateZeroStatusDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDZeroNetwork,
                                       &CZeroNetworkDelegate::CreateZeroNetworkDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDZeroAutoP,
                                       &CZeroAutopDelegate::CreateZeroAutopDelegate);
    g_DelegateFactory.RegisterDelegate(kszPageIDZeroRedirect,
                                       &CZeroRedirectDelegate::CreateZeroRedirectDelegate);
    // adpater
    g_SettingAdapterFactory.RegisterSettingAdapter(kszPageIDZeroNetwork,
            CSettingUIComboboxAdapter::CreateComboboxAdapter);

    new CDssKeyZero();
}

bool ZeroTouchUI_EnterPage(const yl::string & strPageAction)
{
    // 有话路时，不进行Zero相关界面操作 http://10.3.5.199/Bug.php?BugID=62617
    if (talklogic_SessionExist())
    {
        return true;
    }

    // 获取窗口的指针
    CBaseDialog * pDlgBase = UIManager_GetPrivateDlg(DLG_CDlgZeroTouch);
    if (NULL == pDlgBase)
    {
        pDlgBase = UIManager_CreatePrivateDlg(DLG_CDlgZeroTouch);
    }

    CDlgZeroTouch * pDlgZeroTouch = static_cast<CDlgZeroTouch *>(pDlgBase);
    if (NULL != pDlgZeroTouch)
    {
        pDlgZeroTouch->UpdateWnd();
        pDlgZeroTouch->InitExtraData(NULL);
        pDlgZeroTouch->SetPageType(strPageAction);
        return true;
    }
    else
    {
        ZEROUI_ERR("Open DlgZeroSPTouch Error!");
        return false;
    }

    return true;
}

bool ZeroTouchUI_Destroy()
{
    ZEROUI_INFO("ZeroTouchUI_Destroy");
    if (!commonUnits_IsSystemMaintenanceStatus())
    {
        // 非系统维护状态才处理。
        UIManager_ReturnToIdle();
    }

    return true;
}

bool ZeroTouchUI_EnterPageByType(int iType)
{
#ifdef ZERO_TOUCH_DEBUG
    ZEROUI_INFO("ZeroTouchUI OpenDlg Type [%d]", iType);
#endif
    switch (iType)
    {
    case ZERO_DLG_AUTOPCONFIG:
        {
            ZeroTouchUI_EnterPage(ZERO_AUTOP_PAGE);
        }
        break;
    case ZERO_DLG_UNAUTHORIZED:
        {
            ZeroTouchUI_EnterPage(ZERO_UNAUTHORIZED_PAGE);
        }
        break;
    case ZERO_DLG_REDIRECTOR_AUTHENTICATION:
        {
            // 弹出认证界面
            ZeroTouchUI_EnterPage(ZERO_REDIRECTOR_CONFIG);
        }
        break;
    case ZERO_DLG_UPDATEING:
        {
            ZeroTouchUI_EnterPage(ZERO_UPDATING_PAGE);
        }
        break;
    case ZERO_DLG_UPDATEFAIL:
        {
            // 弹出连接失败提示
            ZeroTouchUI_EnterPage(ZERO_UPDATEFAIL_PAGE);
        }
        break;
    case ZERO_DLG_UPDATEFINISH:
        {
            ZeroTouchUI_EnterPage(ZERO_UPDATEFINISH_PAGE);
        }
        break;
    case ZERO_DLG_REBOOTING:
        {
            ZeroTouchUI_EnterPage(ZERO_REBOOTING_PAGE);
        }
        break;
    case ZERO_DLG_RPS:
        {
            SettingUILogic_EnterZeroRPS();
        }
        break;
    case ZERO_DLG_READY_REDIRECT:
        {
            ZeroTouchUI_EnterPage(ZERO_REDIRECTOR_NOTE);
        }
        break;
    case ZERO_DLG_EXIT:
        {
            ZeroTouchUI_Destroy();
        }
        break;
    case ZERO_DLG_CHECK_NETWORK:
        {
            ZeroTouchUI_EnterPage(ZERO_NETWORK_CHECK_PAGE);
        }
        break;
    case ZERO_DLG_NETWORK_FAIL:
        {
            ZeroTouchUI_EnterPage(ZERO_NETWORK_FAIL_PAGE);
        }
        break;
    case ZERO_DLG_MAIN:
        {
            ZeroTouchUI_EnterPage(ZERO_MAIN_PAGE);
        }
        break;
    case ZERO_DLG_STATUS:
        {
            ZeroTouchUI_EnterPage(ZERO_STATUS_PAGE);
        }
        break;
    case ZERO_DLG_NETWORK:
        {
            ZeroTouchUI_EnterPage(ZERO_NETWORK_PAGE);
        }
        break;
    default:
        break;
    }
    return true;
}

#if IF_FEATURE_METASWITCH
bool ZeroTouchUI_EnterAutopDirect()
{
    ZEROUI_INFO("ZeroSPTouch_EnterAutopDirect()");
    Autop_Start(AUT_MTSW_HTTP_DIGEST);
    return ZeroTouchUI_EnterPageByType(ZERO_DLG_AUTOPCONFIG);
}
#endif
