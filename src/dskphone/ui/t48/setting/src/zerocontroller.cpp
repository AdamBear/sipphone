#include "zero_common.h"
#include "zerocontroller.h"
#include "autopcontrol/include/modautopcontrol.h"

#include "zerosptouchui/include/modzerosptouch.h"
#include "commonapi/common_api_enumtypes.h"
#include "interfacedefine.h"
#include "devicelib/phonedevice.h"
#include "devicelib/networkserver.h"
#include "baseui/modbaseui.h"
#include "settingui/src/networksettingmanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "setting/include/common.h"
#include "configparser/modconfigparser.h"
#include "settingui/include/modsettingui.h"
#include <configiddefine.h>
#include "adapterbox/include/modadapterbox.h"

#include "menu/menucontroller.h"

namespace
{
#define ACTIONPAIR      std::pair<yl::string, yl::string>
#define ACTIONMAP       std::map<yl::string, std::map<yl::string, yl::string> >
#define ACTIONITEMMAP   std::map<yl::string, yl::string>
}

// 初始化静态成员变量
CZeroController * CZeroController::m_pZeroController = NULL;

CZeroController * CZeroController::GetInstance()
{
    if (NULL == m_pZeroController)
    {
        m_pZeroController = new CZeroController();
    }

    return m_pZeroController;
}

void CZeroController::ReleaseInstance()
{
    if (NULL != m_pZeroController)
    {
        delete m_pZeroController;
        m_pZeroController = NULL;
    }
}

CZeroController::CZeroController()
{
    Init();
}

CZeroController::~CZeroController()
{
    UnInit();
}

// 初始化
void CZeroController::Init()
{
    m_bZeroLauched = false;
    InitAction();
}

// 清除资源
void CZeroController::UnInit()
{
    UnitAction();
}

// Zero是否可用
bool CZeroController::IsZeroEnable()
{
    return (0 == configParser_GetConfigInt(kszForbidZeroSwitch));
}

// 初始化界面类型与action的对应关系
void CZeroController::InitAction()
{

    UnitAction();

    ACTIONITEMMAP mapAction;
    mapAction.clear();
    if (1 == configParser_GetConfigInt(kszZeroNetworkAndURLEnable))
    {
        // 初始倒计时页面
#if IF_BUG_30885
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_UPDATING_PAGE));
#else
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_NETWORK_PAGE));
#endif
        mapAction.insert(ACTIONPAIR(ZERO_STATUS, ZERO_STATUS_PAGE));
        m_mapAction[ZERO_MAIN_PAGE] = mapAction;

        mapAction.clear();
        // 初始化网络页面
#if IF_BUG_30885
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_UPDATING_PAGE));
#else
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_AUTOP_PAGE));
#endif
        mapAction.insert(ACTIONPAIR(ZERO_BACK, ZERO_MAIN_PAGE));
        m_mapAction[ZERO_NETWORK_PAGE] = mapAction;

        mapAction.clear();
        // 初始化Autop页面
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_UPDATING_PAGE));
#if IF_BUG_30885
        mapAction.insert(ACTIONPAIR(ZERO_BACK, ZERO_MAIN_PAGE));
#else
        mapAction.insert(ACTIONPAIR(ZERO_BACK, ZERO_NETWORK_PAGE));
#endif
        m_mapAction[ZERO_AUTOP_PAGE] = mapAction;

#if IF_FEATURE_METASWITCH
        mapAction.clear();
        // 初始化Autop页面
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_UPDATING_PAGE));
        m_mapAction[ZERO_AUTOP_PAGE_DIRECT] = mapAction;
#endif
    }
    else
    {
        // 初始倒计时页面
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_AUTOP_PAGE));
        mapAction.insert(ACTIONPAIR(ZERO_STATUS, ZERO_STATUS_PAGE));
        m_mapAction[ZERO_MAIN_PAGE] = mapAction;

        mapAction.clear();
        // 初始化Autop页面
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_UPDATING_PAGE));
        mapAction.insert(ACTIONPAIR(ZERO_BACK, ZERO_MAIN_PAGE));
        m_mapAction[ZERO_AUTOP_PAGE] = mapAction;

        mapAction.clear();
        // 鉴权页面
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_UPDATING_PAGE));
        m_mapAction[ZERO_AUTHENTICATION] = mapAction;

#if IF_FEATURE_METASWITCH
        mapAction.clear();
        // 初始化Autop页面
        mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_UPDATING_PAGE));
        m_mapAction[ZERO_AUTOP_PAGE_DIRECT] = mapAction;
#endif
    }

    mapAction.clear();
    // 初始化更新页面
    m_mapAction[ZERO_UPDATING_PAGE] = mapAction;

    mapAction.clear();
    // 初始化更新完成页面
    m_mapAction[ZERO_UPDATEFINISH_PAGE] = mapAction;

    mapAction.clear();
    // 初始化状态页面
    mapAction.insert(ACTIONPAIR(ZERO_NEXT, ZERO_MAIN_PAGE));
    mapAction.insert(ACTIONPAIR(ZERO_BACK, ZERO_MAIN_PAGE));
    m_mapAction[ZERO_STATUS_PAGE] = mapAction;
}

// 清除资源
void CZeroController::UnitAction()
{
    ACTIONMAP::iterator it = m_mapAction.begin();
    for (; it != m_mapAction.end(); ++it)
    {
        (*it).second.clear();
    }

    m_mapAction.clear();
}

// 根据当前页面及softkey类型,返回下一个页面
yl::string CZeroController::GetNextPage(const yl::string & strCurrentPageAction,
                                        const yl::string & strSoftkeyAction)
{
    // 查找当前页面所对应的action map
    ACTIONMAP::iterator it = m_mapAction.find(strCurrentPageAction.c_str());
    if (it == m_mapAction.end())
    {
        return "";
    }

    // 在找到的map中,根据softkey类型查找相应的页面
    ACTIONITEMMAP::iterator itemMap = (*it).second.find(strSoftkeyAction.c_str());
    if (itemMap == (*it).second.end())
    {
        return "" ;
    }

    // 返回下一个页面的Action
    return (*itemMap).second;
}

// 进入Zero
void CZeroController::Enter()
{
    ZERO_INFO("Zero enter");
    Autop_Start(AUT_ZERO);

    // 先判断是否是立即更新
    if (1 == configParser_GetConfigInt(kszAutopNow))
    {
        // 重置标志
        Autop_SetAutopNow(true);
        configParser_SetConfigInt(kszAutopNow, 0);

        if (CheckingNetwork())
        {
#if IF_BUG_30885
            BeginAutop();
#else
            // 启动autoServer
            if (!Autop_StartZeroConnect())
            {
                EnterPage(ZERO_UPDATEFAIL_PAGE);
            }
#endif
        }
    }
    else
    {
        // 先判断zero是否可用
        if (IsZeroLaunched()
                && !IsZeroEnable())
        {
            ZERO_INFO("zero is forbided");
            ZeroSPTouch_Destroy();
            return;
        }

        if (configParser_GetConfigInt(kszZeroSimplification) == 1)
        {
            EnterPage(ZERO_AUTOP_PAGE);
        }
        else
        {
            // 进入zero的初始界面
            EnterPage(ZERO_MAIN_PAGE);
        }
    }
}

// 退出Zero
void CZeroController::Exit()
{
    Autop_Exit();

#if IF_BUG_30885
    ZEROUI_INFO("Zero Exit");
    ExitTelstraAutop();
#endif
}

// 根据当前页面及softkey类型,进入下一个页面
void CZeroController::EnterNextStage(const yl::string & strCurrentPageAction,
                                     const yl::string & strSoftkeyAction)
{
    // 在进入页面前,先做一些检查,如之前在网络页面,需检查网络是否发生改变,如果改变,要重启话机
    if (DoSpecialCheck(strCurrentPageAction, strSoftkeyAction))
    {
        return;
    }

    ZERO_INFO("[Zero]::EnterPage[ current page = %s, softkey action = %s ]",
              strCurrentPageAction.c_str(), strSoftkeyAction.c_str());
    // 进入下一个页面
    EnterPage(GetNextPage(strCurrentPageAction, strSoftkeyAction));
}

// 根据action,调用相应接口,进入到指定界面
void CZeroController::EnterPage(const yl::string & strPageAction/* = ""*/)
{
    ZERO_INFO("[Zero]::EnterPage[ next page is = %s ]", strPageAction.c_str());
    if (strPageAction.empty())
    {
        // 解析失败,进入到默认界面
        if (configParser_GetConfigInt(kszZeroSimplification) == 1)
        {
            ZeroSPTouch_OpenDlg(ZERO_AUTOP_PAGE);
        }
        else
        {
            ZeroSPTouch_OpenDlg(ZERO_MAIN_PAGE);
        }
        return;
    }

    if (ZERO_STATUS_PAGE == strPageAction)
    {
        SettingUI_OpenPage(SubMenuData(ZERO_STATUS_PAGE, PAGE_MODE_SETTING), true);
    }
    else if (ZERO_REBOOTING_PAGE == strPageAction
             || ZERO_NETWORK_CHECK_PAGE == strPageAction
             || ZERO_NETWORK_FAIL_PAGE == strPageAction)
    {
        // 重启界面需要立即刷新给用户看
        ZeroSPTouch_OpenDlg(strPageAction, true);
    }
    else
    {
        // 调用由zeroUI提供的接口,更新界面
        ZeroSPTouch_OpenDlg(strPageAction);
    }
}

// 特殊检查操作
bool CZeroController::DoSpecialCheck(const yl::string & strCurrentPageAction,
                                     const yl::string & strSoftkeyAction)
{
    bool bHandled = false;
    if (ZERO_AUTOP_PAGE == strCurrentPageAction
            && ZERO_NEXT == strSoftkeyAction)
    {
        ZERO_INFO("special check, current page = %s, softkey action = %s", strCurrentPageAction.c_str(),
                  strSoftkeyAction.c_str());

        // 网络发生改变,需要重启
        if (_NetworkChangeFlagManager.GetNetworkChangeFlag())
        {
            ZERO_INFO("network is changed, need reboot");
            ProcessReboot();
        }
        else
        {
            if (CheckingNetwork())
            {
#if IF_BUG_30885
                BeginAutop();
#else
                // 启动autoServer
                if (!Autop_StartZeroConnect())
                {
                    EnterPage(ZERO_UPDATEFAIL_PAGE);
                }
#endif
            }
        }
        bHandled = true;
    }
    else if (ZERO_MAIN_PAGE == strCurrentPageAction && ZERO_NEXT == strSoftkeyAction)
    {
        // 不显示网络和url时判断网络是否可用
        if (0 == configParser_GetConfigInt(kszZeroNetworkAndURLEnable))
        {
            // 网络不可用不需要进入autop界面
            if (CheckingNetwork())
            {
                EnterPage(ZERO_AUTOP_PAGE);
            }
            bHandled = true;
        }
    }
    else if (ZERO_NETWORK_PAGE == strCurrentPageAction && Autop_GetUpdateType() == AUT_RPS)
    {
        if (ZERO_NEXT == strSoftkeyAction)
        {
            // 网络发生改变,需要重启
            if (_NetworkChangeFlagManager.GetNetworkChangeFlag())
            {
                ZERO_INFO("network is changed, need reboot");
                ProcessReboot();
            }
            else
            {
                if (CheckingNetwork())
                {
                    Autop_StartRPSConnect();
                }
            }
            bHandled = true;
        }
        else if (ZERO_BACK == strSoftkeyAction)
        {
            ZeroSPTouch_Destroy();
            bHandled = true;
        }
    }
#if IF_FEATURE_METASWITCH
    else if (ZERO_AUTOP_PAGE_DIRECT == strCurrentPageAction)
    {
        if (ZERO_NEXT == strSoftkeyAction)
        {
            if (CheckingNetwork())
            {
                if (!Autop_StartHttpDigestTConnect())
                {
                    EnterPage(ZERO_UPDATEFAIL_PAGE);
                }
            }
        }
        else if (ZERO_BACK == strSoftkeyAction)
        {
            ZeroSPTouch_Destroy();
        }
        bHandled = true;
    }
#endif

    return bHandled;
}

// 设置重定向鉴权用户信息
void CZeroController::SetRedirectData(yl::string strUserName, yl::string strPassword)
{
    Autop_SetAuthInfo(strUserName, strPassword);
}

// 处理重启
void CZeroController::ProcessReboot()
{
    // 设置重启进入后马上Autop
    configParser_SetConfigInt(kszAutopNow, 1);

    // 设置为立即更新
    Autop_SetAutopNow(true);

    // 界面更改
    EnterPage(ZERO_REBOOTING_PAGE);

    // 延迟3s,等待显示重启界面,再重启话机
    etlSleep(5 * 1000);

    AdapterBox_Reboot(0);
}

// 设置是否是被zeroMain启动
void CZeroController::SetZeroLaunched(bool bZeroLaunched)
{
    m_bZeroLauched = bZeroLaunched;
}

// 检查网络
bool CZeroController::CheckingNetwork()
{
    // 检查网络
    EnterPage(ZERO_NETWORK_CHECK_PAGE);
    // 网络不可用,显示提示
    if (!Autop_WaitingForNetwork())
    {
        EnterPage(ZERO_NETWORK_FAIL_PAGE);
        return false;
    }

    return true;
}

#if IF_BUG_30885
bool CZeroController::BeginAutop()
{
    if (Autop_StartOption66Connect()
            || Autop_StartDMSConnect())
    {
        Autop_Start(AUT_ZERO);
        // 在检测是否需要输入鉴权信息时需要显示正在更新
        EnterPage(ZERO_UPDATING_PAGE);
        return true;
    }

    return false;
}
#endif
