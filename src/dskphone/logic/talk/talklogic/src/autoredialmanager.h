#ifndef __AUTOREDIAL_MANAGER_H__
#define __AUTOREDIAL_MANAGER_H__

class CAutoRedialManager
{
    DECLARE_SINGLETON_CLASS(CAutoRedialManager)

public:
    // 初始化重拨信息
    void Init();

public:
    // 返回是否允许自动重拨
    bool BeAllowAutoRedial();
    // 停止自动重拨
    void StopAutoRedial(bool bForce = true);
    // 返回当前状态
    AUTOREDIAL_STATUS GetStatus();
    // 返回两次重拨的间隔
    int GetRedialInterval();
    // 设置状态
    void SetStatus(AUTOREDIAL_STATUS eStatus);
    // 重拨次数减一
    void DescreaseTimers();
    // 倒计时次数是否结束
    bool BeCountDownFinished();
    //剩余的重拨次数
    int GetRemainRedialTimes() const
    {
        return  m_iRedialTimers;
    }
    // 配置文件改变,重新加载
    void OnConfigChange();

private:
    // 加载自动重拨的配置
    void LoadConfig();

private:
    // 保存当前状态
    AUTOREDIAL_STATUS   m_eCurrentStatus;
    // 保存redial的次数
    int         m_iRedialTimers;
    // 保存每次Redial的间隔
    int         m_iRedialInterval;
};


// 获取全局唯一的AutoRedial管理对象
#define _AutoRedialManager GET_SINGLETON_INSTANCE(CAutoRedialManager)

#endif // __AUTOREDIAL_MANAGER_H__
