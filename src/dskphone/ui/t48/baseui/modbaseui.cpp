#include "modbaseui.h"
#include "dsklog/log.h"
#include "commonapi/stringhelperapi.h"
#include "uimanager/basedialog.h"
#include "uimanager/uimanager_header.h"
#include "uimanager/moduimanager.h"
#include "settingui/src/dlgmenu.h"
#include <ylstring.h>
#include "keyguard/include/modkeyguard.h"
#include "edkui/edkuimanager.h"
#include "edkui/edkmapdefine.h"
#include "uicommon/qtcommon/qmisc.h"
#include "transwidgetbgcontroler.h"

static std::map<yl::string, yl::string> s_mapPageName;

void AddPageNameMap(const yl::string & strAction
                    , const yl::string & strPageName)
{
    // 添加页面Action到PageName的映射
    s_mapPageName.insert(pair<yl::string, yl::string>(strAction, strPageName));
}

namespace
{
// rect of hotdesking
const QRect RECT_HOTDESK(135, 70, 600, 410);
}

void BaseUI_Init()
{
    // 初始化透明度控件对象
    g_TransWidgetBgControler;
}

/************************************************************************/
/* 接口  ： BaseUI_RegisterDlgClass()                                 */
/* 功能  ： 实现窗体类注册                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void BaseUI_RegisterDlgClass()
{
    REGISTER_DLG(CDlgMenu);

    // 添加页面Action到PageName的映射
    AddPageNameMap("hoteling_login", DLG_CDlgHotelingLogin);
    AddPageNameMap("disposition_code", DLG_CDlgACDDispositionCode);
    AddPageNameMap("emergency_escalation", DLG_CDlgACDEscalateTo);
    AddPageNameMap("unavailable_reason", DLG_CDlgACDUnavailableReason);
}

/************************************************************************/
/* 接口  ： BaseUI_CreateMenu                                           */
/* 功能  ： 实现菜单显示                                                */
/* 参数说明 ：strMenuAction 类型：str::string                           */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
bool BaseUI_CreateMenu(yl::string strMenuAction,  bool bTopWindow/* = false*/)
{
    if (!keyGuard_IsEnableModuld(KGM_MODULE_SETTING))
    {
        return true;
    }

    // 获取Menu界面的指针
    CBaseDialog * pMenuBase = UIManager_CreatePrivateDlg(DLG_CDlgMenu, DLG_CDlgMenu);
    if (pMenuBase == NULL)
    {
        return false;
    }

    // 设置Menu数据
    pMenuBase->SetData(&strMenuAction);

    // 将窗口设置为顶层窗口
    if (bTopWindow)
    {
        UIManager_SetTopWindow(pMenuBase);
    }

    //设置当前窗口的提示命令
    pMenuBase->SetHintTitle(toQString(EDK_UI_ACTION_OPEN_MENU));
    // 刷新窗口
    UIManager_UpdateWnd(pMenuBase);

    return true;
}

/************************************************************************/
/* 接口  ： BaseUI_DestroyMenu                                           */
/* 功能  ： 实现退出菜单                                                */
/* 返回值 ： 无                                                            */
/************************************************************************/
void BaseUI_DestroyMenu()
{
    CBaseDialog * pMenuBase = UIManager_GetPrivateDlg(DLG_CDlgMenu);

    if (NULL != pMenuBase)
    {
        UIManager_PutPrivateDlg(pMenuBase);
    }
}
