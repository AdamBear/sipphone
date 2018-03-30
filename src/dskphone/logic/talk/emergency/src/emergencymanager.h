#ifndef __EMERGENCY_MANAGER_H__
#define __EMERGENCY_MANAGER_H__

#include "singletonclassdefine.h"
#include "talk/talklogic/include/talklogic_common.h"
#include <yllist.h>

#ifdef IF_FEATURE_EMERGENCY
#define MAX_EM_MODE_COUNT 2
#else
#define MAX_EM_MODE_COUNT 1
#endif

// 紧急呼叫管理类。。 内部会缓存结果（当前是单线程，不能放在异步线程）
class CEmergencyManager
{
public:
    static CEmergencyManager & GetInstance()
    {
        static CEmergencyManager s_emManager;
        return s_emManager;
    }

    CEmergencyManager();

public:

    void Init();

    static LRESULT OnMessage(msgObject & msg);

    int IsMatched(const char * cszTarget, int iTypeMask, bool IsCharCmp = false);

    bool CallEmergency();

    bool IsEmergencyMode(LPCSTR lpNum, bool IsCharCmp = false);

    LPCSTR GetAssertedId();

    int GetPosIndex(LPCSTR lpNum);

    bool IsEmpty(int iMode);

    bool IsInWindows()
    {
        return m_iWindow != STOP_TIMER_VALUE;
    }

    void StartWindows();

private:

    // 外部消息响应入口
    bool OnMessageImp(msgObject & msg);

    bool ReloadConfig(int iMode, int iType);

    CBaseEmergency * GetEmergencyMode(int iMode);

private:
    CBaseEmergency   *  m_arrMode[MAX_EM_MODE_COUNT];
    CMatchedInfo        tCacheInfo;                    // 结果缓存
    int                 m_iWindow;              // 紧急呼叫窗口期
};
// 实例
#define _Emergency GET_SINGLETON_INSTANCE(CEmergencyManager)

#endif // __EMERGENCY_MANAGER_H__
