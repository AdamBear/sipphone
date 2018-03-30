#ifndef __ZEROCONTROLLER_H__
#define __ZEROCONTROLLER_H__

#include "ylstring.h"
#include <map>
#include <ETLLib.hpp>

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

#if IF_BUG_30885
    bool BeginAutop();
#endif

private:
    // Zero是否可用
    bool IsZeroEnable();
    // 初始化界面类型与action的对应关系
    void InitAction();
    // 清除资源
    void UnitAction();
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
    std::map<yl::string, std::map<yl::string, yl::string> > m_mapAction;
    bool                    m_bZeroLauched;

    static CZeroController * m_pZeroController;
};

#define g_pZeroController (CZeroController::GetInstance())

#endif // __ZEROCONTROLLER_H__
