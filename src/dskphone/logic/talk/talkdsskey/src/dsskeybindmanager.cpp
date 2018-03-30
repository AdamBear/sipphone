#include "talkdsskey_inc.h"

#ifdef IF_FEATURE_DSSKEY

bool CompareLineKeyCallNum(int lParam, int rParam)
{
    return _DsskeyBindManager.GetLineKeyCallNum(lParam) <= _DsskeyBindManager.GetLineKeyCallNum(rParam);
}

CDsskeyBindManager::FindRebindVistor::FindRebindVistor(const CSessionProxy& p, int i)
    : old(p), iDsskey(i)
{
    iLineID = dsskey_GetDsskeyLine(iDsskey);
}

bool CDsskeyBindManager::FindRebindVistor::operator()(CSessionProxy& proxy)
{
    if (!proxy.IsSingle())
    {
        return false;
    }

    bool bSCASession = proxy.IsBla();
    // http://10.2.1.199/Bug.php?BugID=58552
    // SCA账号建立本地会议时会将两个hide session也绑定到dsskey上
    // http://bugfree.yealink.com/Bug.php?BugID=120735
    // 多路情况建立会议绑定关系没有在建立会议两路中，重新绑定则要能绑定到与会的话路
    if (!bSCASession
            && proxy.IsHide()
            && !proxy.IsInConf())
    {
        return false;
    }

    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    int iAccountID = pCallInfo->GetAccountID();
    // 判断所用的账号是否相同
    if (iAccountID != iLineID)
    {
        return false;
    }

    // SCA还得判断blaindex和dsskeyid是否匹配
    if (bSCASession)
    {
        if (dsskey_GetDsskeyByBLAIndex(pCallInfo->m_iBLAIndex, iAccountID) != iDsskey)
        {
            return false;
        }
    }

    // 如果有顶层session,则将绑定关系交给最顶层session
    CSessionProxy host = proxy.GetTopestHost();
    CSessionProxy bind = (host && !bSCASession) ? host : proxy;

    return bind != old;
}

CDsskeyBindManager::CDsskeyBindManager()
{
    SetCallBack(MAKE_TIMER_CALL_BACK(CDsskeyBindManager::OnTimer));
}

bool CDsskeyBindManager::RebindLinekey(int iDsskeyID, const CSessionProxy& proxy)
{
    if (iDsskeyID == INVALID_DSSKEY_ID || !proxy)
    {
        return false;
    }

    FindRebindVistor visitor(proxy, iDsskeyID);
    CSessionProxy target = talklogic_TraverseSession(visitor, false);
    return target && AddBinder(target, iDsskeyID);
}

// 增加绑定关系, 绑定的是Session与非账号的dsskey
bool CDsskeyBindManager::AddBinder(const CSessionProxy& proxy, int iDsskey, bool bLineDsskey/* = true*/)
{
    AUTO_SCOPED_TIMER;
    if (INVALID_HANDLE == iDsskey || !proxy)
    {
        return false;
    }

    // call park notify不绑定dsskey，V73需求
    if (bLineDsskey
            && proxy.GetRoutineState() == ROUTINE_PARK_NOTIFY)
    {
        return false;
    }

    // dsskey发起的通话,则数量绑定在该dsskey上
    // 更新操作失败,则不需要绑定亮灯
    if (bLineDsskey && !UpdateLineKeyCallNum(iDsskey, proxy, true))
    {
        if (iDsskey != GetLineKeyIDBySession(proxy))
        {
            return false;
        }
    }

    ITER_BINDMAP it;
    if (!FindItemBySession(it, proxy))
    {
        // 如果不存在session,则新建立一个关系
        DsskeyBindData& bindData = m_mapSessionDsskey[proxy];
        int& key = bLineDsskey ? bindData.iFirst : bindData.iSecond;
        key = iDsskey;
    }
    else
    {
        // 如果存在,则判断dsskey是否为空
        int& key = bLineDsskey ? it->second.iFirst : it->second.iSecond;
        // 已存在绑定关系,不再绑定
        if (INVALID_HANDLE != key)
        {
            TALK_WARN("AddBinder failed, dsskey is already exist key[%d] session[%d] old[%d] line[%d]", iDsskey, (int)proxy, key, bLineDsskey);
            return false;
        }

        key = iDsskey;
    }

    TALK_INFO("AddBinder key[%d] session[%d] line[%d]", iDsskey, (int)proxy, bLineDsskey);
    // http://10.2.1.199/Bug.php?BugID=58576
    // 重新绑定后将之前的定时器清掉(CDsskeyBindAction::OnDebinded中设置的定时器)
    // 已存在绑定关系，不再绑定的情况不用清掉，比如上面的return false
    dsskey_KillTimer(iDsskey);
    ProcessAfterBind(proxy);

    return true;
}

// 增加绑定关系, 绑定的是Session与账号的dsskey
bool CDsskeyBindManager::AddBinderByAccount(const CSessionProxy& proxy, int iAccount)
{
    AUTO_SCOPED_TIMER;
    //蓝牙话路
#ifdef IF_BT_SUPPORT_PHONE
    if (Bluetooth_GetBtAccountID() == iAccount)
    {
        YLList<int> listID;
        if (dsskey_getAllDsskeys(listID, DT_BTLINE))
        {
            return AddBinder(proxy, *(listID.begin()), false);
        }
    }
#endif

    if (!proxy || !acc_IsAccountIDValid(iAccount))
    {
        return false;
    }

    int iBindDsskeyID = INVALID_HANDLE;
    // SCA绑定dsskey需要根据BlaIndex
    if (proxy.IsBla())
    {
        ITER_BINDMAP it;
        if (!FindItemBySession(it, proxy, true))
        {
            CCallInfo* pCallInfo = proxy.GetCallInfo();
            if (pCallInfo != NULL)
            {
                if (pCallInfo->m_iBLAIndex == 0)
                {
                    // 如果允许新的dsskey绑定的话，则取下一个空闲的索引，否则需要取原来dsskey对应的下一个可用索引
                    if (IsBindNewDsskeyWithNewCall(proxy, iAccount))
                    {
                        pCallInfo->m_iBLAIndex = bsSCA_GetFirstFreeIndex(pCallInfo->GetAccountID());
                    }
                    else
                    {
                        // 获取之前话路绑定的dsskey的下一个索引值去占线
                        int iDsskeyId = GetDsskeyIdRelationSession(iAccount, proxy);
                        pCallInfo->m_iBLAIndex = (iDsskeyId != INVALID_HANDLE) ? bsSCA_GetBLAIndex(iDsskeyId) : bsSCA_GetFirstFreeIndex(pCallInfo->GetAccountID());
                    }
                    TALK_INFO("SCA Get FirstFreeIndex [%d]", pCallInfo->m_iBLAIndex);
                }

                iBindDsskeyID = dsskey_GetDsskeyByBLAIndex(pCallInfo->m_iBLAIndex, pCallInfo->GetAccountID());
                TALK_INFO("Get Dsskey SCA Need to Bind [%d]", iBindDsskeyID);
            }
        }
    }

    return BindLineKey(proxy, iAccount, iBindDsskeyID);
}

// 修改绑定关系中的Dsskey对象
bool CDsskeyBindManager::ModifyDsskey(int iOldDsskey, int iNewDsskey, bool bLineDsskey/* = true*/)
{
    AUTO_SCOPED_TIMER;
    if (INVALID_HANDLE == iNewDsskey)
    {
        return false;
    }

    ITER_BINDMAP it;
    if (!FindItemByDsskey(it, iOldDsskey))
    {
        return false;
    }

    // 保存旧的dsskey对象
    DsskeyBindData& oldBindData = it->second;
    // 替换为新的dsskey对象
    int& key = bLineDsskey ? it->second.iFirst : it->second.iSecond;
    key = iNewDsskey;

    // 添加新的绑定关系
    m_mapSessionDsskey.insert(it->first, oldBindData);
    // 删除旧的绑定关系
    m_mapSessionDsskey.erase(it);
    return true;
}

// 修改绑定关系中的Session对象
bool CDsskeyBindManager::ModifySession(const CSessionProxy& oldProxy, const CSessionProxy& newProxy, bool bLinekey/* = false*/, bool bDelOldLineSession /*= true*/)
{
    if (!oldProxy || !newProxy)
    {
        return false;
    }

    ITER_BINDMAP it;
    if (!FindItemBySession(it, oldProxy, bLinekey))
    {
        return false;
    }

    //是否需要重新绑定新通话到Dsskey
    if (bDelOldLineSession)
    {
        // 如果新的通话session有绑定，则把之前绑定的dsskey绑定到新的session，避免存在一个session有多个映射关系
        ITER_BINDMAP itNew;
        if (FindItemBySession(itNew, newProxy))
        {
            if (bLinekey)
            {
                itNew->second.iFirst = it->second.iFirst;
            }
            else
            {
                itNew->second.iSecond = it->second.iSecond;
            }
        }
        else
        {
            // 添加新的绑定关系
            m_mapSessionDsskey.insert(newProxy, it->second);
        }
        // 删除旧的绑定关系
        m_mapSessionDsskey.erase(it);
        ProcessAfterBind(newProxy);
    }

    // SCA账号切换话路焦点会更换绑定关系,话路数量统计关系不用更换
    // 存在一路SCA，按line key新起一路,切换焦点,但新起的一路还没设置callinfo
    if (CSessionProxy(oldProxy).IsBla()
            || CSessionProxy(newProxy).IsBla())
    {
        return true;
    }

    ChangeRelationSession(oldProxy, newProxy, bDelOldLineSession);
    return true;
}

// 改变旧的session计算关系删除，增加新的session计算关系
void CDsskeyBindManager::ChangeRelationSession(const CSessionProxy& oldProxy, const CSessionProxy& newProxy, bool bDelOldLineSession /*= true*/)
{
    if (!oldProxy || !newProxy)
    {
        return;
    }

    int iLineKeyID = GetLineKeyIDBySession(oldProxy);
    if (iLineKeyID != INVALID_HANDLE)
    {
        if (bDelOldLineSession)
        {
            UpdateLineKeyCallNum(iLineKeyID, oldProxy, false);
        }
        UpdateLineKeyCallNum(iLineKeyID, newProxy, true);
    }
}

// 根据dsskey删除绑定关系
bool CDsskeyBindManager::DeleteBindByDsskey(int iDsskey, bool bRemove/* = true*/)
{
    ITER_BINDMAP it;
    if (!FindItemByDsskey(it, iDsskey))
    {
        return false;
    }

    if (bRemove)
    {
        // 从map中删除
        m_mapSessionDsskey.erase(it);
    }
    else
    {
        // 只删除某个特定Dsskey的绑定关系
        DsskeyBindData& bindData = it->second;
        if (bindData.iFirst == iDsskey)
        {
            bindData.iFirst = INVALID_HANDLE;
        }
        else if (bindData.iSecond == iDsskey)
        {
            bindData.iSecond = INVALID_HANDLE;
        }
    }

    // 处理取消绑定时的处理
    dsskey_OnUnbindProcess(iDsskey);
    return true;
}

// 根据Session删除绑定关系
bool CDsskeyBindManager::DeleteBindBySession(const CSessionProxy& proxy)
{
    if (!proxy)
    {
        return false;
    }

    // 取消统计话路的绑定
    int iLineKeyID = GetLineKeyIDBySession(proxy);
    TALK_INFO("Delete session[%d] that bind on linekey[%d]", (int)proxy, iLineKeyID);
    if (iLineKeyID != INVALID_HANDLE)
    {
        UpdateLineKeyCallNum(iLineKeyID, proxy, false);
    }

    ITER_BINDMAP it;
    if (!FindItemBySession(it, proxy))
    {
        return false;
    }

    if (INVALID_HANDLE != it->second.iFirst)
    {
        // 处理取消绑定时的处理
        dsskey_OnUnbindProcess(it->second.iFirst);
    }

    if (INVALID_HANDLE != it->second.iSecond)
    {
        // 远程控制的灯不能灭 http://10.2.1.199/Bug.php?BugID=21777
        dsskey_OnUnbindProcess(it->second.iSecond);
    }

    // 从map中删除
    m_mapSessionDsskey.erase(it);
    return true;
}

// 根据Session获取对应的Dsskey
int CDsskeyBindManager::GetDsskeyBySession(const CSessionProxy& proxy, bool bLineDsskey/* = true*/)
{
    ITER_BINDMAP it;
    if (!FindItemBySession(it, proxy))
    {
        return INVALID_HANDLE;
    }

    return bLineDsskey ? it->second.iFirst : it->second.iSecond;
}

// 根据Dsskey获取对应的Session
CSessionProxy CDsskeyBindManager::GetSessionByDsskey(int iDsskey, bool bLineDsskey/* = false*/)
{
    ITER_BINDMAP it;
    if (!FindItemByDsskey(it, iDsskey, bLineDsskey))
    {
        return INVALID_HANDLE;
    }

    // 返回session对象指针
    return it->first;
}

// 根据dsskey查找绑定关系项
bool CDsskeyBindManager::FindItemByDsskey(ITER_BINDMAP& it, int iDsskey, bool bLineDsskey/* = false*/)
{
    if (INVALID_HANDLE == iDsskey)
    {
        return false;
    }

    it = m_mapSessionDsskey.begin();
    // 遍历整个map, 找到指定dsskey的项
    while (it != m_mapSessionDsskey.end())
    {
        DsskeyBindData& bindData = it->second;
        if ((bindData.iFirst == iDsskey)
                || (!bLineDsskey && bindData.iSecond == iDsskey))
        {
            return true;
        }

        ++it;
    }

    return false;
}

// 根据Session查找绑定关系项
bool CDsskeyBindManager::FindItemBySession(ITER_BINDMAP& it, const CSessionProxy& proxy, bool bLineKey/* = false*/)
{
    if (!proxy)
    {
        return false;
    }

    it = m_mapSessionDsskey.find(proxy);
    // 遍历整个map, 找到指定pSession的项
    if (it == m_mapSessionDsskey.end())
    {
        return false;
    }

    // 找绑定的dsskey为linekey类型的
    return !bLineKey || it->second.iFirst != INVALID_HANDLE;
}

// 绑定之后的操作
void CDsskeyBindManager::ProcessAfterBind(const CSessionProxy& proxy)
{
    if (!proxy)
    {
        return;
    }

    m_listSessionID.push_back(proxy);
    SetTimerOnce(50);
}

// 绑定之后的操作
int CDsskeyBindManager::OnTimer(CScopedTimer& timer)
{
    AUTO_SCOPED_TIMER;
    /* 去重复sesssion */
    Sort(m_listSessionID, yl::less<int>());
    YLList<int>::iterator it = yl::unique(m_listSessionID.begin(), m_listSessionID.end());
    while (it != m_listSessionID.end())
    {
        it = m_listSessionID.erase(it);
    }

    for (it = m_listSessionID.begin(); it != m_listSessionID.end(); ++it)
    {
        CSessionProxy proxy(*it);
        if (!proxy)
        {
            continue;
        }

        Dsskey_Status eStatus = DS_SESSION_NORMAL_USING;
        if (proxy.IsTalking())
        {
            if (proxy.IsHold())
            {
                // Hold状态闪灯
                eStatus = DS_SESSION_HOLD;
            }
            else
            {
                eStatus = DS_SESSION_TALK; // 普通通话状态
            }
        }
        else
        {
            // 来电闪灯
            if (proxy.IsRinging())
            {
                eStatus = DS_SESSION_CALL_IN;
            }
        }

        int iDsskeyLine = GetDsskeyBySession(proxy, true);
        if (INVALID_HANDLE != iDsskeyLine)
        {
            if (proxy.IsHide() && !proxy.IsDraftBla())
            {
                // 关DssKey灯
                eStatus = DS_SESSION_HIDE;
            }
            else
            {
                SESSION_STATE eState = proxy.GetSessionState();
                if (SESSION_PREDIAL == eState
                        || SESSION_DIALING == eState)
                {
                    eStatus = DS_SESSION_DIAL;
                }
                else if (SESSION_CONNECTING == eState
                         || SESSION_RINGBACK == eState
                         || SESSION_CONNECTING_TRAN == eState
                         || SESSION_RINGBACK_TRAN == eState)
                {
                    eStatus = DS_SESSION_CALL_Out;
                }

                dsskey_SetDsskeyStatus(iDsskeyLine, eStatus);
                dsskey_SetLineStatus(iDsskeyLine, DS_LINE_USING);
                dsskey_refreshDSSKeyUI(iDsskeyLine);

                // 拒接之后，dsskey重新找新的通话session绑定之后，需要重新刷新sca的灯状态
                int iLineID = dsskey_GetDsskeyLine(iDsskeyLine);
                bsSCA_UpdateLightInfo(iLineID);
            }
        }

        int iSecDsskey = GetDsskeyBySession(proxy, false);
        if (iSecDsskey != INVALID_HANDLE)
        {
            if (proxy.IsHide())
            {
                eStatus = DS_SESSION_HIDE;
            }
            else
            {
#ifdef IF_BT_SUPPORT_PHONE
                //蓝牙DSSKEY需要处理
                if (proxy.IsBTTalk() && dsskey_GetDsskeyType(iSecDsskey) == DT_BTLINE)
                {
                    eStatus = DS_BTLINE_USING;
                }
#endif
            }

            dsskey_SetDsskeyStatus(iSecDsskey, eStatus);
            dsskey_refreshDSSKeyUI(iSecDsskey);
        }
    }

    m_listSessionID.clear();
    return 1;
}

int CDsskeyBindManager::GetUnboundDsskeyByAccountID(int iAccount, int iDsskeyID/* = INVALID_HANDLE*/)
{
    YLList<int> list;
    dsskey_GetDsskeysByAccID(list, iAccount);

    //只取DT_LINE
    for (YLList<int>::iterator it = list.begin();
            it != list.end(); ++it)
    {
        // SCA要通过blaindex找对应的linekey去绑定
        if (iDsskeyID != INVALID_HANDLE
                && *it != iDsskeyID
                && dsskey_IsSCATypeLinekey(*it))
        {
            continue;
        }

        //未找到绑定，返回该id
        if (!GetSessionByDsskey(*it, true))
        {
            return *it;
        }
    }

    return INVALID_HANDLE;
}

/*
** 通过linekey ID判断是否允许建立通话
*/
bool CDsskeyBindManager::IsAllowNewCallByLineKey(int iLineKeyID)
{
    if (INVALID_HANDLE == iLineKeyID)
    {
        return false;
    }

    int iAccountId = dsskey_GetDsskeyLine(iLineKeyID);
    int iMaxCallNumber = GetAccountMaxCallNum(iAccountId);

    if (dsskey_GetDsskeyType(iLineKeyID) == DT_LINE)
    {
        if (dsskey_IsSCATypeLinekey(iLineKeyID))
        {
            int nIndex = 0;
            dsskey_GetBLAIndexByDsskey(nIndex, iLineKeyID, iAccountId);
            int nCurrentNum = bsSCA_GetCurrntCallNumByAccount(iAccountId, nIndex);
            TALK_DBG("CDsskeyBindManager::IsAllowNewCallByLineKey--SCA current Call Num[%d] Max Num[%d]", nCurrentNum, iMaxCallNumber);
            // SCA当前话路数量大于line key最大允许话路数量,则不允许新建话路
            return nCurrentNum < iMaxCallNumber;
        }

        int iCurCounts = GetLineKeyCallNum(iLineKeyID);
        TALK_DBG("MCA Check [iAccountId:%d] [DsskeyId:%d] [iCurCounts:%d] [iMaxCallNumber:%d] [bAllowCall:%d]", iAccountId, iLineKeyID, iCurCounts, iMaxCallNumber, iCurCounts < iMaxCallNumber);
        return (iCurCounts < iMaxCallNumber);
    }
    else
    {
        // http://10.2.1.199/Bug.php?BugID=85155
        // 传入的dsskey如果不是linekey类型，需要获取该类型关联的账号的id去判断该账号是否已经超出最大的限制线路
        return IsAllowNewCallByAccount(iAccountId);
    }
}

/*
** 通过Account判断是否允许建立通话
*/
bool CDsskeyBindManager::IsAllowNewCallByAccount(int iAccount)
{
    // 配置没有设置，不用去遍历，直接返回默认的
    if (GetAccountMaxCallNum(iAccount) == talklogic_MaxSessionNum())
    {
        return talklogic_BeAllowNewCall();
    }

    YLList<int> list;
    dsskey_GetDsskeysByAccID(list, iAccount);
    bool bAllow = false;

    // http://10.2.1.199/Bug.php?BugID=85218
    // 如果没有设置dsskey为账号，则mca功能不生效
    if (list.begin() == list.end())
    {
        bAllow = true;
    }
    else
    {
        for (YLList<int>::iterator it = list.begin();
                it != list.end(); ++it)
        {
            if (IsAllowNewCallByLineKey(*it))
            {
                bAllow = true;
                break;
            }
        }
    }

    TALK_DBG("Check Account[%d] bAllowNewCall[%d]", iAccount, bAllow);
    return bAllow;
}

/*
** 判断是否允许建立会议
*/
bool CDsskeyBindManager::IsAllowConf(const CSessionProxy& proxy)
{
    if (!proxy.IsSingle())
    {
        return false;
    }

    // IP直拨账号没有绑定linekey,特殊处理
    if (acc_IsDirectIPAccount(proxy.GetAccountID()))
    {
        return true;
    }

    int iLineKeyID = GetLineKeyIDBySession(proxy);
    return INVALID_HANDLE == iLineKeyID || ForConfSplit(iLineKeyID);
}

/*
**针对会议拆分之后的关系特殊处理
*/
bool CDsskeyBindManager::ForConfSplit(int iLineKeyID)
{
    if (iLineKeyID == INVALID_HANDLE)
    {
        return false;
    }

    int iNoFollowNum = 0;
    ITER_LINEKEY_INFO_MAP itMap;
    if (!FindItemByLineKeyID(itMap, iLineKeyID))
    {
        return false;
    }

    typeSessionIDList& list = itMap->second;
    for (ITER_SESSOINID_LIST it = list.begin();
            it != list.end(); ++it)
    {
        CSessionProxy proxy(*it);
        //AccountMaxCallNumber为1，Transferring通话界面要Conference键
        //http://bugfree.yealink.com/Bug.php?BugID=118028
        if (!proxy.GetSponsor())
        {
            iNoFollowNum++;
        }
    }

    int iLineID = dsskey_GetDsskeyLine(iLineKeyID);
    return iNoFollowNum <= GetAccountMaxCallNum(iLineID);
}

/*
** 通过Session ID获取绑定的linekey ID
*/
int CDsskeyBindManager::GetLineKeyIDBySession(const CSessionProxy& proxy)
{
    if (!proxy)
    {
        return INVALID_HANDLE;
    }

    for (ITER_LINEKEY_INFO_MAP it = m_mapLineKeySession.begin();
            it != m_mapLineKeySession.end(); ++it)
    {
        typeSessionIDList& list = it->second;
        if (yl::find(list.begin(), list.end(), proxy) != list.end())
        {
            return it->first;
        }
    }

    return INVALID_HANDLE;
}

/*
** 计算line key上的通话数量
*/
int CDsskeyBindManager::GetLineKeyCallNum(int iLineKeyID)
{
    if (INVALID_HANDLE == iLineKeyID)
    {
        return 0;
    }

    ITER_LINEKEY_INFO_MAP it;
    if (!FindItemByLineKeyID(it, iLineKeyID))
    {
        return 0;
    }

    //TALK_INFO("Check line key[%d], Current call num[%d]", iLineKeyID, listSessionID->size());
    return it->second.size();
}

/*
** 更新line key上的通话数量
*/
bool CDsskeyBindManager::UpdateLineKeyCallNum(int iLineKeyID, const CSessionProxy& proxy, bool bAdd)
{
    //TALK_INFO("MCA UpdateLineKeyCallNum [DsskeyId:%d]", iLineKeyID);
    if (INVALID_HANDLE == iLineKeyID || !proxy)
    {
        return false;
    }

    typeSessionIDList* pTargetList = NULL;
    if (bAdd)
    {
        for (ITER_LINEKEY_INFO_MAP it = m_mapLineKeySession.begin();
                it != m_mapLineKeySession.end(); ++it)
        {
            typeSessionIDList& list = it->second;

            if (yl::find(list.begin(), list.end(), proxy) != list.end())
            {
                return false;
            }

            if (it->first == iLineKeyID)
            {
                pTargetList = &list;
            }
        }
    }
    else
    {
        ITER_LINEKEY_INFO_MAP it;
        if (!FindItemByLineKeyID(it, iLineKeyID))
        {
            return false;
        }

        pTargetList = &it->second;
    }

    if (pTargetList == NULL)
    {
        if (bAdd)
        {
            m_mapLineKeySession[iLineKeyID].push_back(proxy);
            TALK_DBG("MCA Update LineKey Bind Session Relationship, [DsskeyId:%d]  [session num:1]", iLineKeyID);
        }

        return true;
    }
    else
    {
        typeSessionIDList& list = *pTargetList;
        ITER_SESSOINID_LIST it = yl::find(list.begin(), list.end(), proxy);

        if (it != list.end())
        {
            list.erase(it);
            TALK_DBG("MCA Update LineKey Bind Session Relationship, [DsskeyId:%d]  [session num:1]", iLineKeyID, list.size());

            // 通话数量为0,从表中删除
            if (list.begin() == list.end())
            {
                m_mapLineKeySession.erase(iLineKeyID);
            }
        }
        else if (bAdd)
        {
            list.push_back(proxy);
            TALK_DBG("MCA Update LineKey Bind Session Relationship, [DsskeyId:%d]  [session num:%d]", iLineKeyID, list.size());
        }
    }

    //PrintMapInfo(m_mapLineKeySession);
    return true;
}

/*
** 绑定session到linekey上
*/
bool CDsskeyBindManager::BindLineKey(const CSessionProxy& proxy, int iAccount, int iBindDsskeyID/* = INVALID_HANDLE*/)
{
    AUTO_SCOPED_TIMER;
    if (!proxy)
    {
        return false;
    }

    int iLineKeyID = INVALID_HANDLE;
    // 允许用新的dsskey来绑定通话
    if (IsBindNewDsskeyWithNewCall(proxy, iAccount))
    {
        // 有未绑定的dsskey，则绑定
        iLineKeyID = GetUnboundDsskeyByAccountID(iAccount, iBindDsskeyID);
        if (iLineKeyID != INVALID_HANDLE)
        {
            AddBinder(proxy, iLineKeyID, true);
        }

        if (iBindDsskeyID != INVALID_HANDLE && acc_IsBLAEnable(iAccount))
        {
            // SCA账号的dsskey已经有绑定的通话, 因此找不到可绑定的dsskey
            // 该通话统计到SCA的index计算出来的dsskey
            // http://10.2.1.199/Bug.php?BugID=117462
            iLineKeyID = iBindDsskeyID;
        }
        else if (IsAllowNewCallByAccount(iAccount))
        {
            // 获取最少的dsskey来关联，用于统计dsskey关联的通话数
            iLineKeyID = GetDsskeyIdOfLeastSession(iAccount);
        }
    }

    // 没找到关联的dsskeyid，则需要去获取顶层通话的dsskeyid或关联通话最少的dsskey来统计到该session
    if (iLineKeyID == INVALID_HANDLE)
    {
        // 没有被dsskey绑定的话路，获取一个dsskey来关联该通话
        iLineKeyID = GetDsskeyIdRelationSession(iAccount, proxy);
    }

    return UpdateLineKeyCallNum(iLineKeyID, proxy, true);
}

// 没有被dsskey绑定的话路，获取一个dsskey来关联该通话
int CDsskeyBindManager::GetDsskeyIdRelationSession(int iAccounId, const CSessionProxy& proxy)
{
    AUTO_SCOPED_TIMER;
    int iDsskeyId = INVALID_HANDLE;
    CSessionProxy sponsor = proxy.GetSponsor();

    // 规则是：当有sponsor话机的，则把sponsor话路关联的dsskey当成新通话的关联dsskey，否则取最少话路的dsskey
    // 第1步
    if (sponsor)
    {
        // 找到顶层session,把session绑定到顶层session的line key上
        CSessionProxy host = sponsor.GetThisOrTopestHost();
        iDsskeyId = GetLineKeyIDBySession(host);

        //http://bugfree.yealink.com/Bug.php?BugID=120795
        //拆分会议时，两路为transfer绑定关系的，为绑定dsskey的需要绑定相同的dsskey
        if (iDsskeyId == INVALID_HANDLE
                && proxy.GetTranSponsor() == sponsor)
        {
            iDsskeyId = GetLineKeyIDBySession(sponsor);
        }
    }

    // 第2步
    // 找话路数量最少的dsskey来关联
    iDsskeyId = (iDsskeyId == INVALID_HANDLE) ? GetDsskeyIdOfLeastSession(iAccounId) : iDsskeyId;
    return iDsskeyId;
}

// 获取线路最少的dsskey
int CDsskeyBindManager::GetDsskeyIdOfLeastSession(int iAccoutId)
{
    AUTO_SCOPED_TIMER;
    if (iAccoutId == INVALID_HANDLE)
    {
        return INVALID_HANDLE;
    }

    YLList<int> list;
    dsskey_GetDsskeysByAccID(list, iAccoutId);

    for (YLList<int>::iterator it = list.begin(); it != list.end();)
    {
        if (IsAllowNewCallByLineKey(*it))
        {
            ++it;
        }
        else
        {
            it = list.erase(it);
        }
    }

    Sort(list, CompareLineKeyCallNum);

    if (list.begin() != list.end())
    {
        return list.front();
    }
    return INVALID_HANDLE;
}

/*
** 通过Linekey ID找到item
*/
bool CDsskeyBindManager::FindItemByLineKeyID(ITER_LINEKEY_INFO_MAP& it, int iLineKeyID)
{
    if (INVALID_HANDLE == iLineKeyID)
    {
        return false;
    }

    it = m_mapLineKeySession.find(iLineKeyID);
    return it != m_mapLineKeySession.end();
}

/*
** linekey限制的通话数量
*/
int CDsskeyBindManager::GetAccountMaxCallNum(int iAccountID)
{
    if (iAccountID == INVALID_ACCOUNTID)
    {
        return 0;
    }

    // 读取配置,账号限制通话数量
    yl::string strMaxCallNumber = configParser_GetCfgItemStringValue(kszAccountMaxCallNumber, iAccountID);
    if (strMaxCallNumber.empty())
    {
        // 获取全局的通话数量
        strMaxCallNumber = configParser_GetConfigString(kszAccountMaxCallNumPerLine);
    }

    int iMaxCallNum = atoi(strMaxCallNumber.c_str());
    // 若通话数量为0, 则line话路数量默认为最大数量
    if (iMaxCallNum <= 0
            || iMaxCallNum > 24)
    {
        iMaxCallNum = talklogic_MaxSessionNum();
    }

    return iMaxCallNum;
}

// 从配置文件获取linekey限制的通话数量
int CDsskeyBindManager::GetAccountMaxCallNumFromCFG(int iAccountID)
{
    // 读取配置,账号限制通话数量
    yl::string strMaxCallNumber = configParser_GetCfgItemStringValue(kszAccountMaxCallNumber, iAccountID);
    if (strMaxCallNumber.empty())
    {
        // 获取全局的通话数量
        strMaxCallNumber = configParser_GetConfigString(kszAccountMaxCallNumPerLine);
    }

    return atoi(strMaxCallNumber.c_str());
}

bool CDsskeyBindManager::IsBindNewDsskeyWithNewCall(const CSessionProxy& proxy, int iAccountId)
{
    if (proxy.GetConfSponsor())
    {
        yl::string strNewLine = configParser_GetCfgItemStringValue(kszAccountConferenceByNewLine, iAccountId);
        if (strNewLine.empty())
        {
            strNewLine = configParser_GetConfigString(kszConferenceByNewLine);
        }
        return (1 == atoi(strNewLine.c_str()));
    }
    else if (proxy.GetTranSponsor())
    {
        yl::string strNewLine = configParser_GetCfgItemStringValue(kszAccountTransferByNewLine, iAccountId);
        if (strNewLine.empty())
        {
            strNewLine = configParser_GetConfigString(kszTransferByNewLine);
        }
        return (1 == atoi(strNewLine.c_str()));
    }
    return true;
}
#endif  //IF_FEATURE_DSSKEY
