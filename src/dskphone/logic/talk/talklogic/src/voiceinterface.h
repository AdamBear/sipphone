#ifndef IF_HOLD_INTERFACE_H_
#define IF_HOLD_INTERFACE_H_

#include <yllist.h>
#include <netinet/in.h>
#include "service_ipvp_types.h"

enum HOLD_TALK_STATE
{
    HOLD_TALK_TALKING = 0,
    HOLD_TALK_HOLD,
    HOLD_TALK_STOP
};

struct HoldInfo
{
    int iCallID;
    HOLD_TALK_STATE eState;
};


class CVoiceInterface
{
public:
    CVoiceInterface();
    ~CVoiceInterface();

    static CVoiceInterface & GetInstance();

    // hold话路
    bool HoldSession(int iCallID, bool bStatusOnly = false);

    // unhold
    bool ResumeSession(int iCallID, bool bStatusOnly = false);

    // stop话路
    bool StopSession(int iCallID);

    //stop 蓝牙话路
    bool StopBtSession(int iCallID);

    // 去掉指定话路信息
    bool RemoveSession(int iCallID);

    // 停掉超过最大hold数量的话路
    bool StopMoreHoldSession(int iExceptCallID = -1);

    // 增加一路hold信息,返回增加的对象
    bool AddSession(int iCallID, HOLD_TALK_STATE eState = HOLD_TALK_TALKING);

    // 播放铃声,因为vpm中,铃声也是占用一路
    bool PlayRing(const char * pRingPath);

    //FarMute
    bool SetFarMute(int iCallID, const ipvp_speech_t& talkParam, bool bFarMute);

    // 重新开启通话
    void ResetTalk(int iCallID, const ipvp_speech_t& talkParam);

private:

    // 获取当前设置的状态
    HoldInfo * GetStateInfo(int iCallID);

    // 当前hold个数
    int GetHoldAndTalkingNum();

private:
    // 支持hold的话路个数
    int m_iHoldMax;

    // 当前hold的信息链接
    YLList<HoldInfo> m_listHoldInfo;
};

// 获取全局唯一的Session管理对象
#define _VoiceInterface GET_SINGLETON_INSTANCE(CVoiceInterface)
#endif // IF_HOLD_INTERFACE_H_
