#include "talklogic_inc.h"

IMPLEMENT_GETINSTANCE(CAutoRedialManager)

CAutoRedialManager::CAutoRedialManager()
{
    m_iRedialTimers = 0;
    m_iRedialInterval = 0;
    m_eCurrentStatus = AUTOREDIAL_IDLE;

    LoadConfig();
}

CAutoRedialManager::~CAutoRedialManager()
{
}

// 加载自动重拨的配置
void CAutoRedialManager::LoadConfig()
{
    m_iRedialTimers = configParser_GetConfigInt(kszAutoRedialTimes);
    m_iRedialInterval = configParser_GetConfigInt(kszAutoRedialInterval);
}

// 返回是否允许自动重拨
bool CAutoRedialManager::BeAllowAutoRedial()
{
    return (configParser_GetConfigInt(kszAutoRedialSwitch) != 0);
}

// 倒计时次数是否结束
bool CAutoRedialManager::BeCountDownFinished()
{
    if (!BeAllowAutoRedial())
    {
        return false;
    }

    // 当前处于倒计时,而计时次数已到,则不再自动重拨
    if (AUTOREDIAL_COUNTDOWN == m_eCurrentStatus && m_iRedialTimers <= 0)
    {
        // 为下一次重拨设置空闲状态
        m_eCurrentStatus = AUTOREDIAL_IDLE;
        return true;
    }

    return false;
}

// 返回当前状态
AUTOREDIAL_STATUS CAutoRedialManager::GetStatus()
{
    return m_eCurrentStatus;
}

// 返回两次重拨的间隔
int CAutoRedialManager::GetRedialInterval()
{
    return m_iRedialInterval;
}

// 设置状态
void CAutoRedialManager::SetStatus(AUTOREDIAL_STATUS eStatus)
{
    m_eCurrentStatus = eStatus;
}

// 初始化重拨信息
void CAutoRedialManager::Init()
{
    LoadConfig();
}

// 重拨次数减一
void CAutoRedialManager::DescreaseTimers()
{
    m_iRedialTimers--;
}

// 停止自动重拨
void CAutoRedialManager::StopAutoRedial(bool bForce/* = true*/)
{
    // 如果不强制退出，则不停止处于倒计时的状态(目前没有false的情况)
    if (!bForce && (!BeAllowAutoRedial() || AUTOREDIAL_COUNTDOWN == m_eCurrentStatus))
    {
        return;
    }

    // 重置状态和配置
    m_eCurrentStatus = AUTOREDIAL_IDLE;
}

void CAutoRedialManager::OnConfigChange()
{
    LoadConfig();
}

