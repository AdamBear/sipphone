#ifndef __BSFT_RECORD_LISTENER_H__
#define __BSFT_RECORD_LISTENER_H__
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING

#include "basetalklistener.h"

/* bw录音业务 */

class CBroadsoftRecordListener : public CBaseTalkListener
{
public:
    CBroadsoftRecordListener();

protected:
    bool OnMessage(msgObject& msg);

    DECL_SIGNAL_HANDLER(OnUIAction);
    DECL_SIGNAL_HANDLER(OnSessionCreate);
    DECL_SIGNAL_HANDLER(OnRoutineChanged);

    //添加话路到网络会议中
    DECL_SIGNAL_HANDLER(OnAddToNetConf);
    DECL_SIGNAL_HANDLER(OnGetDataToUI);

private:
    bool OnModeConfigChanged(msgObject& msg);
    bool OnXSIReqResult(msgObject& msg);
    bool OnSIPRecordStatus(msgObject& msg);

    bool ProcessUIAction(const CSessionProxy& sessionProxy, TALK_ACTION_TYPE eAction);

    //网络会议创建相关状态变化，
    //原本流程在talkroutine 组建会议操作过程中，进入ROUTINE_NETWORKCONF_CONNECT前
    bool OnPreNetworkConfRoutineCreate(const CSessionProxy& sessionProxy);

    //本地会议创建后录音状态同步
    void AllConfCallRecordStatueSync(const CSessionProxy& sessionProxyconst);
};

#endif // IF_FEATURE_BROADSOFT_CALLRECORDING
#endif // __BSFT_RECORD_LISTENER_H__
