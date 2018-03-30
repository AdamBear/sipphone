#ifndef __TALK_ACTION_H__
#define __TALK_ACTION_H__
#include "edkactionbase.h"
#include <ETLLib.hpp>

struct EdkTalkInfo
{
    int m_iActionId;
    bool m_bWaitTalkEstablish;

    EdkTalkInfo()
    {
        m_bWaitTalkEstablish = false;
        m_iActionId = -1;
    }
};

typedef YLHashMap<int, EdkTalkInfo> MAP_ID2EDKINFO;
typedef MAP_ID2EDKINFO::iterator MAP_ID2EDKINFO_ITER;

class CTalkAction
    : public CEdkActionBase
{
public:
    CTalkAction();
    virtual ~CTalkAction();

    virtual bool DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult);

    // 处理消息
    virtual bool OnMessage(msgObject & msg);

private:

    // 消息处理函数
    static LRESULT OnTalkActionMessage(msgObject & msg);
    //
    bool InsertSessionID2EdkInfo(const edk_action_data_t & tEdkAction, int iActionId);
private:

    // 所关心的sessionid，进通话后记住，结束通话清除
    // int m_iSessionId;
    MAP_ID2EDKINFO m_mapSessionId2EdkInfo;

};

#endif // __TALK_ACTION_H__
