#include "powersavingmanager.h"

#include <devicelib/networkserver.h> // netGetLocalTime
#include <etlmsghandler/modetlmsghandler.h> // SingleMsgReg SingleMsgUnReg
#include <service_config.h> // CONFIG_MSG_BCAST_CHANGED
#include <common/common_data_define.h> // TIMER_ID

#include "powersavingmsgdefine.h"
#include "datetimehelper.h"


#define PRE_SAVING_INTERVAL 15 // 预节电模式持续时间（单位：s）

// 设置/取消定时器的同时输出日志
#define SetThreadTimer(timerId, interval)\
    do\
    {\
        POWERSAVING_DEBUG("SetThreadTimer(#%lx interval=%ld)", timerId, interval);\
        timerSetThreadTimer(timerId, interval);\
    } while (false)
#define KillThreadTimer(timerId)\
    do\
    {\
        POWERSAVING_DEBUG("KillThreadTimer(#%lx)", timerId);\
        timerKillThreadTimer(timerId);\
    } while (false)


IMPLEMENT_GETINSTANCE(CPowerSavingManager)

CPowerSavingManager::CPowerSavingManager()
    : m_timeout(0)
    , m_status(POWERSAVING_STATUS_WAITING)
    , m_bSmartWakeTimerEnable(false)
{
}

CPowerSavingManager::~CPowerSavingManager()
{
}

void CPowerSavingManager::Init()
{
    // 初始化成员变量
    m_timeout = 0;
    m_status = POWERSAVING_STATUS_WAITING;
    m_bSmartWakeTimerEnable = false;
    // 加载配置
    m_config.Load();
    // 设置是否启动智能点亮定时器
    SetSmartWakeTimer(m_config.IsSmartWakeEnable());
    // 监听消息
    SingleMsgReg(TM_TIMER, CPowerSavingManager::OnMessage); // 定时器
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CPowerSavingManager::OnMessage); // 配置改变
    SingleMsgReg(SYS_MSG_NOTIFY_TIME_CHANGED, CPowerSavingManager::OnMessage); // 系统时间改变
    SingleMsgReg(POWERSAVIGNMEG_NOTIFY_OPERATE, CPowerSavingManager::OnMessage); // 主动/被动操作
    if (m_config.IsFailBackMode())
    {
        // 容灾模式下应该触发主动操作：主动操作的节电时间为容灾值（被动操作时间为休息时间被动操作值）
        OnOperation(OPERATION_ACTIVE);
    }
    else
    {
        // 普通模式：触发被动操作
        OnOperation(OPERATION_PASSIVE);
    }
}

void CPowerSavingManager::Uninit()
{
    // 取消消息监听
    SingleMsgUnReg(TM_TIMER, CPowerSavingManager::OnMessage); // 定时器
    SingleMsgUnReg(CONFIG_MSG_BCAST_CHANGED, CPowerSavingManager::OnMessage); // 配置改变
    SingleMsgUnReg(SYS_MSG_NOTIFY_TIME_CHANGED, CPowerSavingManager::OnMessage); // 系统时间改变
    SingleMsgUnReg(POWERSAVIGNMEG_NOTIFY_OPERATE,
                   CPowerSavingManager::OnMessage); // 主动/被动操作
    // 停止所有定时器
    KillThreadTimer(TIMER_ID(m_timeout));
    KillThreadTimer(TIMER_ID(m_bSmartWakeTimerEnable));
}

POWERSAVING_STATUS CPowerSavingManager::GetStatus() const
{
    return m_status;
}

void CPowerSavingManager::SetAvoidPowerSaving(bool bAvoid)
{
    POWERSAVING_INFO("SetAvoidPowerSaving(%d)", bAvoid);
    if (bAvoid)
    {
        SetStatus(POWERSAVING_STATUS_FORBIDDEN);
    }
    else
    {
        SetStatus(POWERSAVING_STATUS_WAITING);
        OnOperation(OPERATION_PASSIVE);
    }
}

void CPowerSavingManager::SetSmartWakeTimer(bool bSmartWakeEnable)
{
    if (m_bSmartWakeTimerEnable)
    {
        // 停止智能点亮定时器
        KillThreadTimer(TIMER_ID(m_bSmartWakeTimerEnable));
        m_bSmartWakeTimerEnable = false;
    }
    if (bSmartWakeEnable)
    {
        // 启动智能点亮定时器
        StartSmartWakeTimer();
        m_bSmartWakeTimerEnable = true;
    }
}

void CPowerSavingManager::StartSmartWakeTimer()
{
    // 获取本地时间
    Time time = CDateTimeHelper::GetLocalTime();
    // 获取当前所在时段
    TimeRange range = m_config.GetTimeRangeOfTime(time);
    if (TIME_RANGE_WORK == range.type)
    {
        // 当前时段为工作时段，则下两个时段也是工作时段
        range = m_config.GetNextTimeRange(range);
        range = m_config.GetNextTimeRange(range);
    }
    else
    {
        // 当前时段为休息时段，则下一个时段就是工作时段
        range = m_config.GetNextTimeRange(range);
    }
    // 容错：判断目标时段是否为对应类型（避免不存在工作时段的情况），而且起始时间有效
    if (TIME_RANGE_WORK == range.type && range.begin > time)
    {
        // 设置定时器
        SetThreadTimer(TIMER_ID(m_bSmartWakeTimerEnable), (range.begin - time) * 1000);
    }
    else
    {
        POWERSAVING_WARN("Cannot get next work time range! [%ld~%ld type=%d]",
                         range.begin, range.end, range.type);
    }
}

LRESULT CPowerSavingManager::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case TM_TIMER:
        return _PowerSavingManager.OnTimer(msg);
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        if (ST_SETTIME == msg.wParam)
        {
            return _PowerSavingManager.OnSystemTimeChanged(msg);
        }
        else if (ST_POWERSAVING == msg.wParam
                 || ST_BACK_LIGHT ==  msg.wParam)
        {
            return _PowerSavingManager.OnConfigChanged(msg);
        }
        else
        {
            POWERSAVING_DEBUG("Reload config, passive operation.");
            return _PowerSavingManager.OnOperation(OPERATION_PASSIVE);
        }
        break;
    case SYS_MSG_NOTIFY_TIME_CHANGED:
        return _PowerSavingManager.OnSystemTimeChanged(msg);
        break;
    case POWERSAVIGNMEG_NOTIFY_OPERATE:
        if (0 == msg.wParam) // 被动操作
        {
            return _PowerSavingManager.OnOperation(OPERATION_PASSIVE);
        }
        else if (1 == msg.wParam) // 主动操作
        {
            return _PowerSavingManager.OnOperation(OPERATION_ACTIVE);
        }
        else
        {
            POWERSAVING_WARN("Unknown message[POWERSAVIGNMEG_NOTIFY_OPERATE] param: wParam=%d, lParam=%d",
                             msg.wParam, msg.lParam);
        }
        break;
    default:
        break;
    }
    return FALSE;
}

bool CPowerSavingManager::OnOperation(OPERATION_TYPE operation)
{
	POWERSAVING_INFO("Operation[%d] status[%d] IsFailBackMode[%d] to power saving", operation, GetStatus(), m_config.IsFailBackMode());
    // 当前是容灾模式+等待进入节电状态+被动操作，则不做任何处理
    if (m_config.IsFailBackMode()
            && GetStatus() == POWERSAVING_STATUS_WAITING
            && operation == OPERATION_PASSIVE)
    {
        POWERSAVING_INFO("Omit passive operation in fail back mode when waiting.");
        return true;
    }
    // 有操作先点亮屏幕（禁止进入节电模式不需要点亮屏幕，也不应该修改状态）
    if (GetStatus() != POWERSAVING_STATUS_FORBIDDEN)
    {
        SetStatus(POWERSAVING_STATUS_WAITING);
    }
    // 获取当前操作的时间，并计算当前操作对应的进入节电时间
    Time op_time = CDateTimeHelper::GetLocalTime();
    Time timeout = GetTimeoutOfOperation(operation, op_time);
    ResetPowerSavingTimer(timeout);
    return true;
}

bool CPowerSavingManager::OnTimer(const msgObject & msg)
{
    if (msg.message != TM_TIMER)
    {
        return false;
    }
    if (TIMER_ID(m_timeout) == msg.wParam) // 节电定时器
    {
        POWERSAVING_INFO("Timer of power saving is up.");
        // 停止当前定时器
        KillThreadTimer(msg.wParam);
        if (POWERSAVING_STATUS_WAITING == GetStatus())
        {
            // 当前是等待节电状态，则进入预节电
            SetStatus(POWERSAVING_STATUS_PRE_SAVING);
        }
        else if (POWERSAVING_STATUS_PRE_SAVING == GetStatus())
        {
            // 当前是预节电状态，则进入节电
            SetStatus(POWERSAVING_STATUS_SAVING);
        }
        else
        {
            // POWERSAVING_STATUS_FORBIDDEN状态不能进入节电，POWERSAVING_STATUS_SAVING状态不用重复进入节电
            POWERSAVING_INFO("Won't go into power saving mode in status[%d]", GetStatus());
        }
    }
    else if (TIMER_ID(m_bSmartWakeTimerEnable) == msg.wParam) // 智能点亮定时器
    {
        POWERSAVING_INFO("Timer of smart wake is up.");
        // 停止当前定时器
        KillThreadTimer(TIMER_ID(m_bSmartWakeTimerEnable));
        // 通过被动操作点亮屏幕，并重新计算进入节电时间
        OnOperation(OPERATION_PASSIVE);
        // 设置下一个点亮时间点
        StartSmartWakeTimer();
    }
    return false;
}

bool CPowerSavingManager::OnSystemTimeChanged(msgObject & msg)
{
    if ((msg.message != SYS_MSG_NOTIFY_TIME_CHANGED) // rtServer发送
            && (msg.message != CONFIG_MSG_BCAST_CHANGED
                || msg.wParam != ST_SETTIME)) // 手动修改时间，GUI发送
    {
        return false;
    }
    // rtServer发送的消息需要判断时间变化范围，防抖
    if (SYS_MSG_NOTIFY_TIME_CHANGED == msg.message)
    {
        //调用tzset来同步时区
        tzset();
        if (msg.GetExtraData() && msg.GetExtraSize() != 0)
        {
            char * pchKey = (char *) msg.GetExtraData();
            POWERSAVING_DEBUG("SYS_MSG_NOTIFY_TIME_CHANGED extra data: [size=%d][data=%c]",
                              msg.GetExtraSize(), *pchKey);
            if ('0' == *pchKey)
            {
                // 防抖：时间变化不超过1s，忽略
                POWERSAVING_DEBUG("Omit system time change.");
                return true;
            }
        }
    }
    POWERSAVING_INFO("System time changed.");
    // 重置智能点亮定时器
    SetSmartWakeTimer(m_bSmartWakeTimerEnable);

	//节电模式下，无论什么状态下,都需要重置定时器
	if (!m_config.IsFailBackMode())
	{
		// 清空当前节电定时器
		m_timeout = 0;
		KillThreadTimer(TIMER_ID(m_timeout));
	}
	// 触发被动操作
	return OnOperation(OPERATION_PASSIVE);
}

bool CPowerSavingManager::OnConfigChanged(const msgObject & msg)
{
    if (msg.message != CONFIG_MSG_BCAST_CHANGED)
    {
        return false;
    }
    POWERSAVING_DEBUG("Reload config of power saving.");
    bool bIsFailBackMode = m_config.IsFailBackMode();
    // 重新加载配置
    m_config.Load();
    // 重新设定是否启动智能唤醒定时器
    SetSmartWakeTimer(m_config.IsSmartWakeEnable());
	
	//容灾模式下场景下区分处理
	if (m_config.IsFailBackMode())
	{
		if (!bIsFailBackMode)
		{
			// 进入容灾模式需要触发主动操作，进入节电时间按容灾时间计算
			return OnOperation(OPERATION_ACTIVE);
		}
	}
	else //节电模式下，无论熄屏还是亮屏，都需要重置定时器，重新进行计算
	{
		// 清空当前节电定时器
		m_timeout = 0;
		KillThreadTimer(TIMER_ID(m_timeout));
	}
	// 触发被动操作
	return OnOperation(OPERATION_PASSIVE);
}

void CPowerSavingManager::SetStatus(POWERSAVING_STATUS status)
{
    // 状态没有变化，不做处理
    if (status == m_status)
    {
        return;
    }
    POWERSAVING_INFO("Power saving status: %d -> %d", m_status, status);
    switch (status)
    {
    case POWERSAVING_STATUS_WAITING:
        if (POWERSAVING_STATUS_PRE_SAVING == m_status ||
                POWERSAVING_STATUS_SAVING == m_status)
        {
            // 节电/预节电状态唤醒，需要广播消息点亮屏幕
            etl_NotifyApp(false, POWERSAVINGMEG_SCREEN_AWAKE, 0, 0);
        }
        m_status = status;
        break;
    case POWERSAVING_STATUS_FORBIDDEN:
        /* 进入“禁止节电”状态时不停止（清空）节电定时器：
        ** 如果在“禁止节电”状态触发定时器，会在OnTimer中过滤状态。
        ** 如果离开“禁止节电”状态，则进入“禁止节电”状态之前的定时器依然有效，
        **   用于和其他操作比较进入节电的时间点早晚。
        ** 例子：主动操作进入节电延时2min，被动操作进入节电延时1min。
        **      1.DUT主动操作话机，如按数字键
        **      2.等待30S后，有一个未接来电
        **      [预期结果]
        **      1.操作后，DUT等待2分钟可进入休眠
        **      2.上一次主动操作剩余1分30S进入节电，被动操作1分钟可进入，则DUT会按较长时间计算经过1分30后进入节电。
        */
        if (POWERSAVING_STATUS_PRE_SAVING == m_status ||
                POWERSAVING_STATUS_SAVING == m_status)
        {
            // 节电/预节电状态唤醒，需要广播消息点亮屏幕
            etl_NotifyApp(false, POWERSAVINGMEG_SCREEN_AWAKE, 0, 0);
        }
        m_status = status;
        break;
    case POWERSAVING_STATUS_PRE_SAVING:
        if (POWERSAVING_STATUS_WAITING == m_status)
        {
            // 进入预节电状态
            m_status = status;
            etl_NotifyApp(false, POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_PRE_CLOSE,
                          m_config.GetPreSavingBacklightLevel());
            // 设置进入节电状态的定时器
            SetThreadTimer(TIMER_ID(m_timeout), PRE_SAVING_INTERVAL * 1000);
        }
        else
        {
            // 禁止节电/节电状态无法直接进入预节电状态
            POWERSAVING_WARN("Cannot set powersaving status: %d -> %d", m_status, status);
        }
        break;
    case POWERSAVING_STATUS_SAVING:
        if (POWERSAVING_STATUS_PRE_SAVING == m_status)
        {
            // 进入节电状态
            m_status = status;
            // 广播消息关闭屏幕
            etl_NotifyApp(false, POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_CLOSE, 0);
        }
        break;
    default:
        POWERSAVING_WARN("Unknown powersaving status: %d", status);
        break;
    }
}

Time CPowerSavingManager::GetTimeoutOfOperation(OPERATION_TYPE operation, Time time) const
{
    POWERSAVING_DEBUG("CPowerSavingManager::GetTimeoutOfOperation(%d, %ld)", operation, time);
    // 获取时间对应的时段
    TimeRange range = m_config.GetTimeRangeOfTime(time);
    POWERSAVING_DEBUG("Time range: %ld~%ld type=%d", range.begin, range.end, range.type);
    // 进入节电的时间由当前时间和延时相加得到
    TimeDiff interval = m_config.GetIntervalOfTimeRange(range, operation);
    Time timeout = time + interval;
    POWERSAVING_DEBUG("Timeout to power saving: %ld, interval=%ld", timeout, interval);
    // 循环判断是否跨时间段
    while (timeout > range.end)
    {
        // 跨时间段则重新计算下一时段的延时
        range = m_config.GetNextTimeRange(range);
        POWERSAVING_DEBUG("Next time range: %ld~%ld type=%d", range.begin, range.end, range.type);
        timeout = time + m_config.GetIntervalOfTimeRange(range, operation);
        POWERSAVING_DEBUG("Timeout to power saving: %ld", timeout);
        // 如果下一时段对应的进入节电时间早于时段开始时间，则下一时段一开始就进入节电
        if (timeout < range.begin)
        {
            timeout = range.begin;
            POWERSAVING_DEBUG("Timeout to power saving: %ld", timeout);
            break;
        }
    }
    return timeout;
}

void CPowerSavingManager::ResetPowerSavingTimer(Time timeout)
{
    // 当前操作的进入节电时间与定时器时间对比，进入节电的时间更晚则重置定时器
    if (timeout > m_timeout)
    {
        m_timeout = timeout;
        // 重置定时器
        TimeDiff interval = (m_timeout - CDateTimeHelper::GetLocalTime() - PRE_SAVING_INTERVAL) * 1000;
        POWERSAVING_DEBUG("Power saving timer reset: %d", interval);
        KillThreadTimer(TIMER_ID(m_timeout));
        SetThreadTimer(TIMER_ID(m_timeout), interval);
    }
    else
    {
        POWERSAVING_DEBUG("Timeout[%ld] less than original timeout[%ld], will not change.", timeout,
                          m_timeout);
    }
}
