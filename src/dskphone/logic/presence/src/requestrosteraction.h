#ifndef TASKACTION_REQUESTROSTERACTION_H
#define TASKACTION_REQUESTROSTERACTION_H

#if FEATURE_BROADTOUCH_UC

#include "ETLLib.hpp"
#include "yllist.h"
//#include "Common.h"
#include "taskaction/taskaction.h"


#ifdef UC_TEST
#include "Presence/Test.h"
#endif


#ifdef  __cplusplus
extern "C" {
#endif
#include "xmpp/include/libxmpp.h"
#ifdef  __cplusplus
};
#endif


using namespace NS_TA;

struct PresenceInfo;

class CPresenceRequestAction : public CTaskAction
{
public:
    enum ReqType
    {
        REQ_INIT,
        REQ_CARD
    };
public:
    explicit CPresenceRequestAction();
    virtual ~CPresenceRequestAction();

    void SetExtraInfo(XMPP_CONN * conn, XMPP_CONN_INFO * conn_info);

    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed)
    {
        return false;
    }

    //获取请求数据
    inline yl::string GetRosterData()
    {
        return m_strRosterData;
    }

    inline ReqType GetReqType()
    {
        return m_eReqType;
    }

    void SetReqType(ReqType eReqType);

    void PushPresenceInfo(const PresenceInfo & refData);

    inline YLList<PresenceInfo> & GetPresenceList()
    {
        return m_listPresence;
    }

private:

    bool DoRequestRoster(XMPP_CONN * conn, XMPP_CONN_INFO * conn_info);

    bool ReqAllCard();

private:
    yl::string m_strRosterData;

    XMPP_CONN * m_pConn;
    XMPP_CONN_INFO * m_pConnInfo;

    ReqType m_eReqType;

    YLList<PresenceInfo> m_listPresence;
};

#endif //FEATURE_BROADTOUCH_UC

#endif
