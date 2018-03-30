#ifndef __DSSKEYBIND_MANAGER_H__
#define __DSSKEYBIND_MANAGER_H__

#include "talk/talkproxy/include/scopedtimer.h"
#include "talk/talkproxy/include/helpclass.h"
#include "talk/talklogic/include/sessionproxy.h"

// 绑定关系中Session对应的dsskey结构体
struct DsskeyBindData
{
    DsskeyBindData()
        : iFirst(INVALID_HANDLE), iSecond(INVALID_HANDLE)
    {
    }

    int iFirst;
    int iSecond;
};

typedef YLHashMap<int, DsskeyBindData>      typeBindMap;
typedef typeBindMap::iterator               ITER_BINDMAP;
typedef YLList<int>                         typeSessionIDList;
typedef YLHashMap<int, typeSessionIDList>   typeLinekeyInfoMap;
typedef typeLinekeyInfoMap::iterator        ITER_LINEKEY_INFO_MAP;
typedef typeSessionIDList::iterator         ITER_SESSOINID_LIST;

class CDsskeyBindManager
    : public CScopedTimer, public app::SingletonObject<CDsskeyBindManager>
{
public:
    CDsskeyBindManager();

    /* 逆序查找第一路来电 */
    struct FindRebindVistor
        : public CBaseSessionTraver
    {
        FindRebindVistor(const CSessionProxy& p, int i);

        virtual bool operator()(CSessionProxy& proxy);

        CSessionProxy   old;
        int             iDsskey;
        int             iLineID;
    };

public:

    // 重新建立绑定关系
    bool RebindLinekey(int iDsskeyID, const CSessionProxy& proxy);
    // 增加绑定关系
    bool AddBinder(const CSessionProxy& proxy, int iDsskey, bool bLineDsskey = true);
    // 增加绑定关系, 绑定的是Session与账号的dsskey
    bool AddBinderByAccount(const CSessionProxy& proxy, int iAccount);
    // 修改绑定关系中的Dsskey对象
    bool ModifyDsskey(int iOldDsskey, int iNewDsskey, bool bLineDsskey = true);
    // 修改绑定关系中的Session对象
    bool ModifySession(const CSessionProxy& oldProxy, const CSessionProxy& newProxy, bool bLinekey = false, bool bDelOldLineSession = true);
    // 根据dsskey删除绑定关系
    bool DeleteBindByDsskey(int iDsskey, bool bRemove = true);
    // 根据Session删除绑定关系
    bool DeleteBindBySession(const CSessionProxy& proxy);
    // 根据Session获取对应的Dsskey
    int GetDsskeyBySession(const CSessionProxy& proxy, bool bLineDsskey = true);
    // 根据Dsskey获取对应的Session
    CSessionProxy GetSessionByDsskey(int iDsskey, bool bLineDsskey = false);
    // 绑定之后的操作
    void ProcessAfterBind(const CSessionProxy& proxy);
    // 查找一个没有绑定的Dsskey
    int GetUnboundDsskeyByAccountID(int iAccount, int iDsskeyID = INVALID_HANDLE);

    // 判断是否允许建立会议
    bool IsAllowConf(const CSessionProxy& proxy);
    // 通过linekey ID判断是否允许建立通话
    bool IsAllowNewCallByLineKey(int iLineKeyID);
    // 通过Account判断是否允许建立通话
    bool IsAllowNewCallByAccount(int iAccount);
    // 计算line key上的通话数量
    int  GetLineKeyCallNum(int iLineKeyID);
    // 绑定session到linekey上
    bool BindLineKey(const CSessionProxy& proxy, int iAccount, int iBindDsskeyID = INVALID_HANDLE);
    // 改变关联的话路
    void ChangeRelationSession(const CSessionProxy& oldProxy, const CSessionProxy& newProxy, bool bDelOldLineSession = true);

    // 获取linekey限制的通话数量
    int GetAccountMaxCallNum(int iAccountID);

    // 从配置文件获取linekey限制的通话数量
    int GetAccountMaxCallNumFromCFG(int iAccountID);

private:
    // 根据dsskey查找绑定关系项
    bool FindItemByDsskey(ITER_BINDMAP& it, int iDsskey, bool bLineDsskey = false);
    // 根据Session查找绑定关系项
    bool FindItemBySession(ITER_BINDMAP& it, const CSessionProxy& proxy, bool bLineKey = false);
    // 绑定之后的操作
    int OnTimer(CScopedTimer& timer);

    // 通过Linekey ID找到item
    bool FindItemByLineKeyID(ITER_LINEKEY_INFO_MAP& it, int iLineKeyID);
    // 通过Session ID获取绑定的linekey ID
    int GetLineKeyIDBySession(const CSessionProxy& proxy);
    // 更新line key上的通话数量
    bool UpdateLineKeyCallNum(int iLineKeyID, const CSessionProxy& proxy, bool bAdd);
    // 针对会议拆分之后的关系特殊处理
    bool ForConfSplit(int iLineKeyID);
    // 是否用新的dsskey来绑定话路
    bool IsBindNewDsskeyWithNewCall(const CSessionProxy& proxy, int iAccountId);
    // 获取线路最少的dsskey
    int GetDsskeyIdOfLeastSession(int iAccoutId);
    // 没有被dsskey绑定的话路，获取一个dsskey来关联该通话
    int GetDsskeyIdRelationSession(int iAccounId, const CSessionProxy& proxy);

private:
    // 保存Session与Dsskey关系的map变量
    typeBindMap        m_mapSessionDsskey;
    // 需要延迟处理的SessionID
    typeSessionIDList  m_listSessionID;
    // 保存line key和Session ID链表的关系,Session ID链表用来统计通话数量
    typeLinekeyInfoMap m_mapLineKeySession;
};

// 获取全局唯一的Session管理对象
#define _DsskeyBindManager GET_SINGLETON_INSTANCE(CDsskeyBindManager)

#endif //__DSSKEYBIND_MANAGER_H__
