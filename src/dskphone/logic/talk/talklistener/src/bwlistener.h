#ifndef __BW_LISTENER_H__
#define __BW_LISTENER_H__

#include "basetalklistener.h"

struct DataNetConfUserInfo;

/* bw业务：park、retrieve、park notify、Executive-Assistant、callid */

class CBWListener : public CBaseTalkListener
{
public:
    CBWListener();
protected:
    /* ETLmsg消息回调函数 */
    virtual bool OnMessage(msgObject& msg);

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    //事件处理
    DECL_SIGNAL_HANDLER(OnCallInitiationNumber);
#endif
    //创建rountine 事件处理
    DECL_SIGNAL_HANDLER(OnRoutineChanged);
    //销毁rountine 事件处理
    DECL_SIGNAL_HANDLER(OnSessionDestroy);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    //网络会议添加成员
    DECL_SIGNAL_HANDLER(OnSessionAddToNetConf);
#endif

    DECL_SIGNAL_HANDLER(OnUIAction);

    DECL_SIGNAL_HANDLER(OnCallOut);

    DECL_SIGNAL_HANDLER(OnRetrivePark);

    DECL_SIGNAL_HANDLER(OnGetDataToUI);

private:

#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    // 处理CallParkNotify消息
    bool ProcessCallParkNotifyMsg(msgObject& msg);

    bool EnterRetrieveParkUI(const yl::string& strNumber, int iAccountId);

    int EnterCallParkNotify(msgObject& msg);
    // 处理CallPark事件
    void ProcessCallPark(CCallInfo* pCallInfo, const yl::string& strNumber);
    // 处理GPark事件
    void ProcessGroupCallPark(CCallInfo* pCallInfo, const yl::string& strNumber);

#endif
    // park失败提示
    void ParkFailed(const CSessionProxy& session, const yl::string& strFailedInfo, bool bCallPush = false);

    // 解析conf event数据
    bool ParseConfEventInfo(CListenInfo& priv, int iAccountID, const yl::string& strEventInfo);
    //通过userid删除与会者list信息
    bool DeleteParticipantByUserId(CListenInfo& priv, int iAccountID, const yl::string& strUserId);

};

#endif //__BW_LISTENER_H__
