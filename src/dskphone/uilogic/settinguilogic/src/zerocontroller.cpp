#include "zerocontroller.h"

#include <configiddefine.h>
#include "commonapi/common_api_enumtypes.h"
#include "interfacedefine.h"

#include "devicelib/phonedevice.h"
#include "devicelib/networkserver.h"
#include "settinguilogic/src/networksettingmanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include "adapterbox/include/modadapterbox.h"
#include "autopcontrol/include/modautopcontrol.h"


// 初始化静态成员变量
CZeroController * CZeroController::m_pZeroController = NULL;

namespace
{
#define COMPLICE_CONTROL
}

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
    : m_bAysncAutop(false)
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
    UninitAction();
}

// Zero是否可用
bool CZeroController::IsZeroEnable()
{
    return (0 == configParser_GetConfigInt(kszForbidZeroSwitch));
}

// 初始化界面类型与action的对应关系
void CZeroController::InitAction()
{
    UninitAction();

    ACTION_ITEM_MAP mapAction;
    mapAction.clear();
    // 初始倒计时页面
    mapAction.insert(ZERO_NEXT, ZERO_NETWORK_PAGE);
    mapAction.insert(ZERO_STATUS, ZERO_STATUS_PAGE);
    m_mapAction.insert(ZERO_MAIN_PAGE, mapAction);

    mapAction.clear();
    // 初始化网络页面
    mapAction.insert(ZERO_NEXT, ZERO_AUTOP_PAGE);
    mapAction.insert(ZERO_BACK, ZERO_MAIN_PAGE);
    m_mapAction.insert(ZERO_NETWORK_PAGE, mapAction);

    mapAction.clear();
    // 初始化Autop页面
    mapAction.insert(ZERO_NEXT, ZERO_UPDATING_PAGE);
    mapAction.insert(ZERO_BACK, ZERO_NETWORK_PAGE);
    m_mapAction.insert(ZERO_AUTOP_PAGE, mapAction);

    mapAction.clear();
    // 初始化更新页面
    m_mapAction.insert(ZERO_UPDATING_PAGE, mapAction);

    mapAction.clear();
    // 初始化更新完成页面
    m_mapAction.insert(ZERO_UPDATEFINISH_PAGE, mapAction);

    mapAction.clear();
    // 初始化状态页面
    mapAction.insert(ZERO_NEXT, ZERO_MAIN_PAGE);
    mapAction.insert(ZERO_BACK, ZERO_MAIN_PAGE);
    m_mapAction.insert(ZERO_STATUS_PAGE, mapAction);
}

// 清除资源
void CZeroController::UninitAction()
{
    ACTION_MAP::iterator it = m_mapAction.begin();
    for (; it != m_mapAction.end(); ++it)
    {
        it->second.clear();
    }

    m_mapAction.clear();
}

// 根据当前页面及softkey类型,返回下一个页面
yl::string CZeroController::GetNextPage(const yl::string & strCurrentPageAction,
                                        const yl::string & strSoftkeyAction)
{
    // 查找当前页面所对应的action map
    ACTION_MAP::iterator it = m_mapAction.find(strCurrentPageAction.c_str());
    if (it == m_mapAction.end())
    {
        return "";
    }

    // 在找到的map中,根据softkey类型查找相应的页面
    ACTION_ITEM_MAP::iterator itItem = it->second.find(strSoftkeyAction.c_str());
    if (itItem == it->second.end())
    {
        return "" ;
    }

    // 返回下一个页面的Action
    return itItem->second;
}

// 进入Zero
void CZeroController::Enter()
{
#if ZERO_TOUCH_DEBUG
    ZERO_INFO("Zero enter");
#endif

    Autop_Start(AUT_ZERO);
    // 先判断是否是立即更新
    if (1 == configParser_GetConfigInt(kszAutopNow))
    {
        // 重置标志
        Autop_SetAutopNow(true);
        configParser_SetConfigInt(kszAutopNow, 0);

        if (CheckingNetwork())
        {
            // 启动autoServer
            if (!Autop_StartZeroConnect())
            {
                AdapterBox_ZeroOpenDlg(ZERO_DLG_UPDATEFAIL);
            }
        }
    }
    else
    {
        // 先判断zero是否可用
        if (IsZeroLaunched()
                && !IsZeroEnable())
        {
#if ZERO_TOUCH_DEBUG
            ZERO_INFO("zero is forbided");
#endif
            AdapterBox_ZeroOpenDlg(ZERO_DLG_EXIT);
            return;
        }

        if (1 == configParser_GetConfigInt(kszZeroSimplification))
        {
            // 进入zero的初始界面
            AdapterBox_ZeroOpenDlg(ZERO_DLG_AUTOPCONFIG);
        }
        else
        {
            // 进入zero的初始界面
            AdapterBox_ZeroOpenDlg(ZERO_DLG_MAIN);
        }
    }
}

// 退出Zero
void CZeroController::Exit()
{
    Autop_Exit();
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
#if ZERO_TOUCH_DEBUG
    ZERO_INFO("[Zero]::EnterPage[ current page = %s, softkey action = %s ]",
              strCurrentPageAction.c_str(), strSoftkeyAction.c_str());
#endif
    // 进入下一个页面
    EnterPage(GetNextPage(strCurrentPageAction, strSoftkeyAction));
}

// 根据action,调用相应接口,进入到指定界面
void CZeroController::EnterPage(const yl::string & strPageAction/* = ""*/)
{
#if ZERO_TOUCH_DEBUG
    ZERO_INFO("[Zero]::EnterPage[ next page is = %s ]", strPageAction.c_str());
#endif
    if (strPageAction.empty())
    {
        // 解析失败,进入到默认界面
        AdapterBox_ZeroOpenDlg(ZERO_DLG_MAIN);
        return;
    }

    ZeroSpDlgType eZeroDlgType = ZERO_DLG_MAIN;
    if (ZERO_NETWORK_PAGE == strPageAction)
    {
        eZeroDlgType = ZERO_DLG_NETWORK;
    }
    else if (ZERO_STATUS_PAGE == strPageAction)
    {
        eZeroDlgType = ZERO_DLG_STATUS;
    }
    else if (ZERO_AUTOP_PAGE == strPageAction)
    {
        eZeroDlgType = ZERO_DLG_AUTOPCONFIG;
    }
    else if (ZERO_MAIN_PAGE == strPageAction)
    {
        eZeroDlgType = ZERO_DLG_MAIN;
    }
    else if (ZERO_UPDATING_PAGE == strPageAction)
    {
        eZeroDlgType = ZERO_DLG_UPDATEING;
    }
    else if (ZERO_UPDATEFINISH_PAGE == strPageAction)
    {
        eZeroDlgType = ZERO_DLG_UPDATEFINISH;
    }

    // 调用settingUI提供的回调接口
    AdapterBox_ZeroOpenDlg(eZeroDlgType);
}

// 特殊检查操作
bool CZeroController::DoSpecialCheck(const yl::string & strCurrentPageAction,
                                     const yl::string & strSoftkeyAction)
{
    bool bHandled = false;
    if (ZERO_AUTOP_PAGE == strCurrentPageAction && ZERO_NEXT == strSoftkeyAction)
    {
#if ZERO_TOUCH_DEBUG
        ZERO_INFO("special check, current page = %s, softkey action = %s", strCurrentPageAction.c_str(),
                  strSoftkeyAction.c_str());
#endif

        // 网络发生改变,需要重启
        if (_NetworkChangeFlagManager.GetNetworkChangeFlag())
        {
#if ZERO_TOUCH_DEBUG
            ZERO_INFO("network is changed, need reboot");
#endif
            ProcessReboot();
        }
        else
        {
            if (CheckingNetwork())
            {
                // 启动autoServer
                if (!Autop_StartZeroConnect())
                {
                    AdapterBox_ZeroOpenDlg(ZERO_DLG_UPDATEFAIL);
                }
            }
            m_bAysncAutop = true;
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
                AdapterBox_ZeroOpenDlg(ZERO_DLG_AUTOPCONFIG);
            }
            bHandled = true;
        }
    }
    else if (ZERO_NETWORK_PAGE == strCurrentPageAction && AUT_RPS == Autop_GetUpdateType())
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
            AdapterBox_ZeroOpenDlg(ZERO_DLG_EXIT);
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
                    AdapterBox_ZeroOpenDlg(ZERO_DLG_UPDATEFAIL);
                }
            }
        }
        else if (ZERO_BACK == strSoftkeyAction)
        {
            AdapterBox_ZeroOpenDlg(ZERO_DLG_EXIT);
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
    configParser_SetConfigInt(kszAutopNow, 1, CONFIG_LEVEL_PHONE);
    // 设置为立即更新
    Autop_SetAutopNow(true);
    _NetworkChangeFlagManager.ClearChangeFlag();
    // 界面更改
    AdapterBox_ZeroOpenDlg(ZERO_DLG_REBOOTING);
}

// 设置是否是被zeroMain启动
void CZeroController::SetZeroLaunched(bool bZeroLaunched)
{
    m_bZeroLauched = bZeroLaunched;
}

bool CZeroController::CheckingNetwork()
{
    // 检查网络
    AdapterBox_ZeroOpenDlg(ZERO_DLG_CHECK_NETWORK);

    // 网络不可用,显示提示
    if (!Autop_WaitingForNetwork())
    {
        AdapterBox_ZeroOpenDlg(ZERO_DLG_NETWORK_FAIL);
        return false;
    }

    return true;
}

void CZeroController::AsyncAutop()
{
    if (!m_bAysncAutop)
    {
        return;
    }

    m_bAysncAutop = false;
    // 网络不可用,显示提示
    if (!Autop_WaitingForNetwork())
    {
        AdapterBox_ZeroOpenDlg(ZERO_DLG_NETWORK_FAIL);
    }
    else
    {
        // 启动autoServer
        if (!Autop_Start(AUT_NORMAL))
        {
            AdapterBox_ZeroOpenDlg(ZERO_DLG_UPDATEFAIL);
        }
    }
}
