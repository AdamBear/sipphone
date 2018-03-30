#ifndef __SESSION_MANAGER_H__
#define __SESSION_MANAGER_H__

#include <yllist.h>

class CBaseRoutine;
class CBaseSession;
class CLocalConfSession;

// 定义存储Session的链表类型
typedef YLList<CBaseSession*> LIST_CALL_SESSION;

#define FOR_SESSION_LIST(it) \
for (LIST_CALL_SESSION::iterator it = m_ListCallSession.begin(); it != m_ListCallSession.end(); ++it)

#define FOR_SESSION_LIST_EXPORT(it) \
LIST_CALL_SESSION& session_list = _SessionManager.GetSessionList(); \
for (LIST_CALL_SESSION::iterator it = session_list.begin(); it != session_list.end(); ++it)

#define MAX_SESSION_SIZE  50

struct SessionHash
{
    unsigned int operator()(int value) const
    {
        return (size_t)(value) % MAX_SESSION_SIZE;
    }
};

// Session的管理类
class CSessionManager
{
    DECLARE_SINGLETON_CLASS(CSessionManager)

public:
    typedef YLHashMap<int, CBaseSession*, SessionHash>  MAP_SESSION;

public:

    /* 查找sesssion，param2 是否依据继承关系查找 */
    bool IsSessionExist(ROUTINE_STATE eRoutine, bool bInherits = true, CBaseSession* pExcept = NULL);

    /* 退出sesssion，param2 是否依据继承关系查找 */
    bool ExitSession(ROUTINE_STATE eRoutine, bool bInherits = true, CBaseSession* pExcept = NULL);

    // 增加Session，加入到SessionList中(加入到pBaseSession的下一个位置)
    bool AddSession(CBaseSession* pSession, const CBaseSession* pBaseSession = NULL,
                    bool bCheckMax = true);

    CSingleSession* AllocSession(int iCallID = -1, bool bHide = false, CBaseSession* pBaseSession = NULL);

    // 删除Session，并处理删除Session之后的焦点
    bool DeleteSession(CBaseSession* pSession);
    // 通过SessionID获取Session
    CBaseSession* GetSession(int iSessionID);
    // 通过SessionID获取Session
    CSingleSession* GetSingleSession(int iSessionID);
    // 根据CallID找到对应的Routine
    CBaseRoutine* FindRoutineByCallID(int iCallID);
    // 根据CallID找到对应的Session: bHostSession 是否返回id对应话路的主session
    CBaseSession* GetSessionByCallID(int iCallID, bool bHostSession = true);
    // 根据CallID找到对应的Session: bHostSession 是否返回id对应话路的主session
    CSingleSession* GetRelateSessionByCallID(int iCallID, bool bExceptSubSession = true);
    // 退出所有Session(除了pExceptThisSession)
    void ExitAllSession(CBaseSession* pExcept = NULL);

    // 是否存在通话中的话路
    bool IsTalkingSessionExist();

    // hold所有话路
    void HoldAllSession(CBaseSession* pExcept = NULL, HOLD_TYPE eHoldType  = HT_NORMAL);
    void HoldAllSession(int iExceptID, HOLD_TYPE eHoldType = HT_NORMAL);

    // 获取通话Session
    CBaseSession* GetTalkSession();
    // 获取当前话路总数
    int GetSessionListSize(bool bExceptHide = true) const;
    // 返回Session list
    inline LIST_CALL_SESSION& GetSessionList()
    {
        return m_ListCallSession;
    }

    // 显示被加入网络会议的话路
    void ShowSessionAddToConf(bool bCheckTalkState = false);
    // 返回AutoRedial话路的个数，目前为1个
    int GetAutoRedialSessionNum();
    // 是否全部是响铃话路
    bool IsAllSessionRinging(bool bIgnoreAnswered = false);

    // 重新开启其它的Paging通话
    bool RestartOtherTalkPaging(int iExceptSessionId);
    // 是否有Pagin的呼出sesssion
    bool BeHavePageOutCall();

    // 解hold指定session
    bool UnHoldSessionByID(int iSessionID);
    // 是否允许建立本地会议
    bool IsAllowCreateLocalConf(CBaseSession* pSession);
    // 建立本地会议
    bool EstablishLocalConf(CBaseSession* pAddtoConfSession, bool bNego = true);
    //建立网络会议
    bool EstablishNetworkConf(CBaseSession* pAddtoConfSession);

    // 获取首路来电Session
    CSingleSession* GetFirstIncomingCallSession(bool bVideo = false);

    // 是否全部是Hold状态
    bool IsAllSessionHold();

    // 判断除了某个Session外，其它是否全部是Hold状态
    bool IsAllOtherSessionHold(int nSessionID);

    // 判断所有通话是否处于Hold/held状态
    bool IsAllTalkSessionHoldOrHeld();

    // 是否有Hold通话(包括Held状态)
    bool BeHaveSessionHold();

    // 获取所有SessionID
    // !!返回是在vecSessionID列表中对应焦点的序号
    bool GetAllSessionID(YLVector<int>& vecSessionID, bool bExceptHide = true);

    // 判断所有话路中，指定的Codec的话路个数
    int GetCodecSessionNum(const char* pCodec);
    // 判断所有话路中，属于srtp话路的个数
    int GetSRTPSessionNum();
    // 返回当前要建立会议的错误码
    ERROR_INFO_TYPE GetConferenceErrorCode();


    // 通过id查找top session
    CBaseSession* GetTopHostSessionByCallID(int iCallID);

    // 是否存在LocalConf
    bool IsLocalConfExist();

    // 是否存在网络会议
    bool IsNetConfExist();

    // 获取通话中的话路数量
    int GetTalkingNumber(bool bVideo);

    CLocalConfSession* GetConfSession();

    // 是否为会议中的
    bool IsSponsorSessionInConf(CSingleSession* pSession);

    // 清除不是当前添加到会议话路的会议绑定关系
    bool ClearInvalidConfSponsor(CSingleSession* pSponsorSession, CBaseSession* pAddtoConfSession);

    //可建立本地会议数是否超过会议最大支持路数
    bool IsAllowAddLocalConfNumOverMax();

    //获取对应账号主持方网络会议的session
    CSingleSession* GetHostNetConfSession(int nAccountID);
    //对应账号允许加入网络会议的话路总数
    int GetTotalNumOfRoutineAllowAddNetConf(int nAccountID);
    //获取对应账号话路总数
    int GetSingleLineSessionListSize(int nAccountID, bool bExceptHide = true);
    //获取当前允许加入本地会议的话路数
    int GetAllowAddLocalConfRoutineNum();

private:
    void AddIndex(CBaseSession* pSession);

    void RemoveIndex(CBaseSession* pSession);

    void OnCallIDChanged(CSingleSession* pSession, int iNewCallID);

    CBaseSession* FindSessionByID(int iSessionID);

    CSingleSession* FindSessionByCallID(int iCallID);

public:

    int GenSessionID()
    {
        TALK_INFO("Alloc New SessionID %d", m_iGenSessionID);
        return m_iGenSessionID++;
    }

    void OnHideChanged(CBaseSession* pSession);

private:
    int                 m_iGenSessionID;
    int                 m_iUnHideSessionSize;
    // 存储Session的链表
    LIST_CALL_SESSION   m_ListCallSession;
    MAP_SESSION         m_mapSessionID;
    MAP_SESSION         m_mapCallID;
};

// 获取全局唯一的Session管理对象
#define _SessionManager GET_SINGLETON_INSTANCE(CSessionManager)

#endif // __SESSION_MANAGER_H__
