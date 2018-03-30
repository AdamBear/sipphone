#ifndef __HOTELING_MODEL_H__
#define __HOTELING_MODEL_H__

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

#include <ylstring.h>

#include "cccommon.h"

class CDssKeyHoteling;

class HotelingModel
{
public:
    //用于收到服务器下发状态后本地设置
    void SetStatus(HOTELING_STATUS eState);

    HOTELING_STATUS GetHotelingStatus();

    //本地用于登陆登出的接口，参数refData，如refData.IsAuto 开启，则去配置中获取数据，不然密码和账号不能为空
    bool ReqChangeHotelingStatus(hoteling_Request & refData);

    //该接口只是用于Ui界面如何显示，true打钩，密码账号添冲。
    bool IsAutoLogin();

    //返回FlexibleSeating是否启用
    bool IsFlexibleSeatingEnable(int iAccountId = -1);

    // 是否是Guest话机
    bool IsGuestPhone();

    // Guest话机是否支持键盘锁
    bool EnableGuestLock();

    // Guest话机的键盘锁是否生效
    bool IsGuestLock()
    {
        return m_IsGuestLock;
    }

    // Guest话机开启/关闭键盘锁
    void GuestLock(bool bLock)
    {
        m_IsGuestLock = bLock;
    }

    // Guest话机的键盘锁密码是否匹配
    bool IsMatchGuestLockPin(const yl::string & strUnlockPin);

    void ClearFlexibleSeating();

    //用于auto登陆时UI显示
    void GetUserId(yl::string & strUserId);

    //获取绑定账号Id
    int GetAccountId();

    //设置绑定ID
    void SetAccountId(int nId);

    //获取dispalyname
    const yl::string & GetDisPlayName();

    //设置displayName
    void SetDisPlayName(const yl::string & strName);

#if IF_BUG_25622
    //更新hoteling订阅状态
    //返回值代表更新成功与否
    bool UpdateHotelingSubscibeResult(int nAccountId, bool bSubscibeResult);

    //获取第一个开启hoteling的账号Id
    int GetFirstHotelingOn();
#endif
private:
    HotelingModel();
    virtual ~HotelingModel();

private:
    //初始化时获取。账号配置有改变，需再次获取，避免照成错乱和需多次遍历账号链表（原先逻辑这个调用还狠平凡），如账号有改变，其他账号开启功能等就会错乱。
    int m_nAccountId;

    yl::string m_strUserName;//本次登陆的userName

    yl::string m_strPassword;//本次登陆的密码

    HOTELING_STATUS m_eStatus;//Hoteling当前状态

    yl::string m_strDisPalyName;//显示名

#if IF_BUG_25622
    YLVector<bool> m_vecHotelingSubscibeResult;//Hoteling订阅状态
#endif

    friend class CallCenterController;

    CDssKeyHoteling * m_pDsskeyHoteling;

    bool m_IsGuestLock; // Guest话机是否处于上锁状态

};

#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

#endif
