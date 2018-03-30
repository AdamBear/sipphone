#ifndef __HOTDESKING_H__
#define __HOTDESKING_H__

#include "singletonclassdefine.h"

#ifdef IF_FEATURE_HOTDESKING
class CHotDesking
{
    DECLARE_SINGLETON_CLASS(CHotDesking)

public:
    // 初始化
    VOID Init();
    // 启用普通hotdesking模式
    void StartLocalHotDesking();
#ifdef IF_ADVANCE_HOTDESKING
    // 启用高级hotdesking模式
    void StartRemoteHotDesking(LPCSTR lpURL, int iDsskeyKeyCode);
    // 设置进入高级hotdesking模式标志
    void SetAdvMode(BOOL bAdvMode);
    // 是否处于高级模式
    BOOL IsAdvMode()
    {
        return m_IsAdvMode;
    }
#endif  //IF_ADVANCE_HOTDESKING
    // 设置m_bIsLogonWizard标志,用于开启或者关闭注册向导功能
    VOID SetLogonWizard(BOOL bLogonWizard)
    {
        m_bIsLogonWizard = bLogonWizard;
    }
    // 响应ST_DESKLINE
    VOID OnDesklineChange(int iAccount);
    // 注册向导功能是否生效
    BOOL IsLogonWizard()
    {
        return m_bIsLogonWizard;
    }
    // 进入界面
    BOOL EnterLoginWizard();
    // 清除用户信息
    VOID ClearUserInfo();

private:
    // 是否启用HotDesking功能
    BOOL    m_bAllowHotDesking;
    // 是否开启注册向导
    BOOL    m_bIsLogonWizard;
#ifdef IF_ADVANCE_HOTDESKING
    // 当前是否已经处于HotDesking状态
    BOOL    m_IsAdvMode;
#endif  //IF_ADVANCE_HOTDESKING
};

#define _HotDesking GET_SINGLETON_INSTANCE(CHotDesking)
#endif // #ifdef IF_FEATURE_HOTDESKING
#endif
