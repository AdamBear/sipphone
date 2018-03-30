#ifndef __ZEROCONTROLLER_H__
#define __ZEROCONTROLLER_H__

#include "ylstring.h"
#include <ETLLib.hpp>
#include <ylhashmap.h>
#include "zero_common.h"

typedef YLHashMap<yl::string, yl::string>               ACTION_ITEM_MAP;
typedef YLHashMap<yl::string, ACTION_ITEM_MAP>          ACTION_MAP;

class CZeroController
{
public:
    static CZeroController * GetInstance();
    static void ReleaseInstance();

    // 初始化
    void Init();
    // 清除资源
    void UnInit();
    // 进入Zero
    void Enter();
    // 退出Zero
    void Exit();
    // 处理消息
    bool ProcessMsg(msgObject & objMsg);
    // 根据当前页面及softkey类型,进入下一个页面
    void EnterNextStage(const yl::string & strCurrentPageAction, const yl::string & strSoftkeyAction);
    // 设置是否是被zeroMain启动
    void SetZeroLaunched(bool bZeroLaunched);
    // 是否是被zeroMain启动
    bool IsZeroLaunched()
    {
        return m_bZeroLauched;
    }
    // 根据action,调用相应接口,进入到指定界面
    void EnterPage(const yl::string & strPageAction = "");
    // 处理重启
    void ProcessReboot();
    // 设置重定向鉴权用户信息
    void SetRedirectData(yl::string strUserName, yl::string strPassword);
// 检查网络
    bool CheckingNetwork();
    // 异步autop
    void AsyncAutop();

private:
    // Zero是否可用
    bool IsZeroEnable();
    // 初始化界面类型与action的对应关系
    void InitAction();
    // 清除资源
    void UninitAction();
    // 根据当前页面及softkey类型,返回下一个页面
    yl::string GetNextPage(const yl::string & strCurrentPageAction,
                           const yl::string & strSoftkeyAction);
    // 特殊检查操作
    bool DoSpecialCheck(const yl::string & strCurrentPageAction, const yl::string & strSoftkeyAction);

private:
    CZeroController();
    ~CZeroController();

private:
    // 保存界面与action的对应关系
    ACTION_MAP              m_mapAction;
    bool                    m_bZeroLauched;
    static CZeroController* m_pZeroController;
    bool                    m_bAysncAutop;
};

#define g_pZeroController (CZeroController::GetInstance())

#endif // __ZEROCONTROLLER_H__
