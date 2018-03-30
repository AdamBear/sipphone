#ifndef __EMERGENCY_H__
#define __EMERGENCY_H__

#include "emergencybase.h"
#include <ylvector.h>


// 原始的紧急呼叫类，更确切的叫法应该是白名单
class CWhitelistMode
    : public CBaseEmergency
{
public:
    CWhitelistMode();
    virtual ~CWhitelistMode() {}

    virtual void Init();

    virtual bool Reload(int iType, int lParam);

    virtual bool IsMatched(const char * cszTarget, int iType, CMatchedInfo * pInfo = NULL,
                           bool IsCharCmp = false);

private:
    typedef YLVector<yl::string> ListEmergency;

    void LoadEmergency();

    bool IsEmergency(LPCSTR lpNum, bool IsCharCmp = false);
    bool IsEmergencyIncomingCall(LPCSTR lpNum);
    bool IsForwardEmergency(LPCSTR lpNum);

    static bool IsEmergencyImp(ListEmergency & listData, LPCSTR lpNum, bool IsCharCmp = false);
    static void CheckValid(ListEmergency & listData);

private:
    ListEmergency       m_listEmergency;    // 紧急呼叫号码的列表
    ListEmergency       m_listDNDEmergency;
    ListEmergency       m_listForwardEmergency;
};

#endif // __EMERGENCY_H__
