#include <ETLLib/ETLLib.hpp>
#include "keyjudder.h"
#include "ckeymap.h"
#include "modinputhook.h"

CKeyJudder::CKeyJudder(void)
{
    m_bDelayResendKey = false;
    m_nHookKey = PHONE_KEY_NONE;
    m_bHookPairs = false;
    connect(&m_timerHandsetCheck, SIGNAL(timeout()), this, SLOT(slotCheckHandset()));
}

CKeyJudder::~CKeyJudder(void)
{
    m_timerHandsetCheck.stop();
}

bool CKeyJudder::setKeyCheck(int iKey, bool bPress)
{
    switch (iKey)
    {
    case PHONE_KEY_HANDSET_ON_HOOK:
    case PHONE_KEY_HANDSET_OFF_HOOK:
        {
            //手柄防抖
            return setHandsetCheck(iKey, bPress);
        }
        break;
    case PHONE_KEY_HANDFREE:
        {
            //免提防抖
            return setHandfreeCheck(iKey, bPress);
        }
        break;
    default:
        break;
    }
    return false;
}

bool CKeyJudder::setHandfreeCheck(int iKey, bool bPress)
{
    if (iKey != PHONE_KEY_HANDFREE)
    {
        return false;
    }

    if (bPress)
    {
        static UINT64 uLastHandfreeTick = 0;
        UINT64 uThisTick = etlGetTickCount();
        if (uLastHandfreeTick != 0
                && uThisTick - uLastHandfreeTick <= TIMER_HANDFREE_JUDDER)
        {
            //两次免提按键事件间隔时间过短，拦截，避免频繁响应，出现痴呆情况
            uLastHandfreeTick = uThisTick;
            return true;
        }
        uLastHandfreeTick = uThisTick;
    }
    return false;
}

bool CKeyJudder::setHandsetCheck(int iKey, bool bPress)
{
    if (iKey != PHONE_KEY_HANDSET_ON_HOOK
            && iKey != PHONE_KEY_HANDSET_OFF_HOOK)
    {
        return false;
    }

    if (!m_timerHandsetCheck.isActive()
            && iKey == PHONE_KEY_HANDSET_OFF_HOOK)
    {
        //若尚未开始防抖，同时，按键为摘机键，则不开始防抖，加快接起与进入拨号界面速度
        m_nHookKey = iKey;
        return false;
    }

    if (m_timerHandsetCheck.isActive())
    {
        if (m_nHookKey != iKey)
        {
            m_bHookPairs = !m_bHookPairs;
        }
    }
    else
    {
        if (m_bDelayResendKey)
        {
            //为超时重传按键，不拦截
            m_bDelayResendKey = false;
            return false;
        }
        m_bHookPairs = false;
    }

    if (m_nHookKey != iKey)
    {
        DelayHandsetCheck(TIMER_HANDSET_JUDDER);
    }
    m_nHookKey = iKey;
    return true;
}

void CKeyJudder::DelayHandsetCheck(int nMillionSeconds)
{
    m_timerHandsetCheck.start(nMillionSeconds);
}

void CKeyJudder::slotCheckHandset()
{
    m_timerHandsetCheck.stop();

    if (!m_bHookPairs)
    {
        //手柄状态并没有被成对丢弃，应该重新传送最后一次按键事件
        m_bHookPairs = false;
        //设置超时重传按键标志为真，避免重传时被拦截
        m_bDelayResendKey = true;
        //重传按键
        inputHook_SendKey(m_nHookKey, true);
    }
}
