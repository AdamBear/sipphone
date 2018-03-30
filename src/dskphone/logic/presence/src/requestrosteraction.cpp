
#include "requestrosteraction.h"
#include "uccontactdata.h"
#include "dsklog/log.h"

#if FEATURE_BROADTOUCH_UC

//test to do -syl
#define  TA_PRESENCE_REQUSET 10

CPresenceRequestAction::CPresenceRequestAction()
    : CTaskAction()
    , m_strRosterData("")
    , m_pConn(NULL)
    , m_pConnInfo(NULL)
{
    m_eTARequestType = (TaskActionRequestType)TA_PRESENCE_REQUSET;
}

void CPresenceRequestAction::SetExtraInfo(XMPP_CONN * conn, XMPP_CONN_INFO * conn_info)
{
    if (conn == NULL || conn_info == NULL)
    {
        return;
    }

    m_pConn = conn;
    m_pConnInfo = conn_info;
}

bool CPresenceRequestAction::Exec()
{
    APP_INFO("CPresenceRequestAction::Exec()");
    if (m_eReqType == REQ_CARD)
    {
        return ReqAllCard();
    }
    else if (m_eReqType == REQ_INIT)
    {
        return DoRequestRoster(m_pConn, m_pConnInfo);
    }

    return false;
}

void CPresenceRequestAction::SetReqType(ReqType eReqType)
{
    m_eReqType = eReqType;
}

bool CPresenceRequestAction::DoRequestRoster(XMPP_CONN * conn, XMPP_CONN_INFO * conn_info)
{
    SetTaskActionErrorType(TA_NO_ERROR);

    if (conn == NULL || conn_info == NULL)
    {
        SetTaskActionErrorType(TA_OTHER_ERROR);
        return false;
    }

    // get contacts info first
    char  *  xml_stream = NULL;
    size_t  stream_len;

    if (xmpp_open(conn, conn_info) != 0)
    {
        APP_INFO("-----------------------------------------xmpp_open() fail!!");
        SetTaskActionErrorType(TA_CONNECTION_REFUSED_ERROR);
        //连接失败，直接回调
        return false;
    }

    g_pUCPresence->SetXmppFeaturesAppend();
    g_pUCPresence->SetXmppFeaturesManual();

    g_pUCPresence->ReqRosterAll();

    //获取favorite联系人
    if (!g_pUCPresence->ReqAddressbookList())
    {
        SetTaskActionErrorType(TA_TIMEOUT_ERROR);
        return false;
    }

    //获取自己的头像
    g_pUCPresence->ReqMyAvatar();

    return true;
}

void CPresenceRequestAction::PushPresenceInfo(const PresenceInfo & refData)
{
    m_listPresence.push_back(refData);
}

bool CPresenceRequestAction::ReqAllCard()
{
    if (m_listPresence.size() == 0)
    {
        return false;
    }

    for (YLList<PresenceInfo>::iterator iter = m_listPresence.begin(); iter != m_listPresence.end();
            iter++)
    {
        g_pUCPresence->ReqContactAvatar(*iter);
    }

    return true;

}

CPresenceRequestAction::~CPresenceRequestAction()
{
}


#endif //FEATURE_BROADTOUCH_UC
