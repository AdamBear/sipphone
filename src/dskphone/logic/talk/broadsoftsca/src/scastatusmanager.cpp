#include "scastatusmanager.h"
#include "scamanager.h"
#include "account/include/modaccount.h"
#include "dsskey/include/moddsskey.h"
#include "scamessagedefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talkdsskey/include/modtalkdsskey.h"

#ifdef IF_FEATURE_BROADSOFT_SCA

#define MAX_SHARELINE_INDEX         200
#define IS_STATUS_IDLE(status) (CN_BLA_STATUS_IDLE == status || status == CN_BLA_STATUS_UNKOWN)

CSCAStatusManager::CSCAStatusManager(void)
{
    m_eBlaType = BLA_NONE;
    m_iLineID = -1;
    m_iAppearanceNum = 0;
    m_peAppearanceStatus = NULL;
}

CSCAStatusManager::~CSCAStatusManager(void)
{
    Release();
}

// 初始化类
VOID CSCAStatusManager::InitBSSCAManager(int iID)
{
    m_iLineID = iID;
    ReloadConfig();
}

// 解析sip转发的Notify消息,以便确认账号的分机状态信息
BOOL CSCAStatusManager::ParseSCANotify(int iIndex, void * pData, int iSize)
{
    SCA_INFO("SCA ParseSCANotify iUpdateNum [%d] [%d] [%d]", m_eBlaType, iIndex, iSize);
    BOOL bRet = TRUE;
    BOOL bUpdate = bRet;

    switch (m_eBlaType)
    {
    case BLA_GENBAND:
        {
            if (pData == NULL || iSize <= 0)
            {
                return FALSE;
            }

            bUpdate = bRet = m_blfParse.Parse((LPCSTR)pData, iSize, this);
        }
        break;
    case BLA_DRAFT_BLA:
        {
            if (pData == NULL || iIndex < 0)
            {
                return FALSE;
            }

            BLF_STATUS_CODE eStatus = *((BLF_STATUS_CODE *)pData);
            // sip 发过来的index从0开始，逻辑层为了保持和BSFT SCA一致从1开始，一次只带一个appearance的状态
            SetStatus2Array(iIndex + 1, Comm_MapSipBLFStatus(eStatus));
        }
        break;
    case BLA_BROADSOFT_SCA:
        {
            // 清空链表且将状态设置成Idle
            ClearList();
            SetAllSameStatus(m_peAppearanceStatus, CN_BLA_STATUS_IDLE, m_iAppearanceNum);
            sip_sca_status_t * pStateArray = (sip_sca_status_t *)pData;
            int iUpdateNum = iIndex;

            if (iUpdateNum <= 0
                    || NULL == pStateArray)
            {
                bRet = FALSE;
                break;
            }

            // 解析账号的状态并将必要的信息放入链表
            SCA_Info sInfo;
            int i = 0;
            while (i < iUpdateNum)
            {
                sip_sca_status_t & scaStatus = pStateArray[i];
                sInfo.iIndex = scaStatus.appearance_index;
                sInfo.eStatus = Comm_MapSipBLFStatus(scaStatus.appearance_state);
                sInfo.strDisplay = scaStatus.remote_user_info.display_name;
                sInfo.strURI = scaStatus.remote_user_info.user_name;

                AddSCAInfo(sInfo);
                i++;
            }
        }
        break;
    default:
        bUpdate = bRet = FALSE;
        break;
    }

    if (bUpdate)
    {
        UpDateLightInfo();
        _BSSCAManager.FlashShowMenu(m_iLineID);

#ifdef _DECT
        etl_NotifyApp(false, BLA_MESSAGE_UPDATE_SCA_INFO, m_iLineID, 0);
#endif
    }

    return bRet;
}

// 获取第一个空闲线路的Index值
int CSCAStatusManager::GetFirstFreeIndex()
{
    PrintAppearance();

    bool bNotBSFTSCA = m_eBlaType != BLA_BROADSOFT_SCA;
#if IF_BUG_39041
    // 占线失败仍然可以呼出
    bNotBSFTSCA = true;
#endif
    // 判断当前的线路是否正常
    if (m_peAppearanceStatus != NULL
            && (bNotBSFTSCA || m_peAppearanceStatus[0] != CN_BLA_STATUS_UNKOWN))
    {
        for (int i = 0; i < m_iAppearanceNum; i++)
        {
            if (m_peAppearanceStatus[i] == CN_BLA_STATUS_IDLE
                    // Draft BLA unkown状态也可用
                    || (bNotBSFTSCA && m_peAppearanceStatus[i] == CN_BLA_STATUS_UNKOWN))
            {
                return (i + 1);
            }
        }

        // 当前没有空闲线路,则扩展线路数组
        int iOldNum = m_iAppearanceNum;
        ReAllocArray();
        return (iOldNum + 1);
    }

    return 0;
}

// 获取指定账号响铃线路
int CSCAStatusManager::GetAlertingIndex()
{
    // 判断当前的线路是否正常
    if (m_peAppearanceStatus != NULL
            && m_peAppearanceStatus[0] != CN_BLA_STATUS_UNKOWN)
    {
        for (int i = 0; i < m_iAppearanceNum; i++)
        {
            if (CN_BLA_STATUS_ALERTING == m_peAppearanceStatus[i]
                    && !talklogic_IsBlaIndexUsing(i + 1))
            {
                return (i + 1);
            }
        }
    }

    return GetFirstFreeIndex();
}

// 清空存储账号分线信息的单链表
VOID CSCAStatusManager::ClearList()
{
    ListSCAInfo::iterator it = m_listSCAInfo.begin();
    while (it != m_listSCAInfo.end())
    {
        delete *it;
        it = m_listSCAInfo.erase(it);
    }
}

// 将所有分线状态都设置成指定状态
VOID CSCAStatusManager::SetAllSameStatus(BLF_STATUS_CODE * pStatus, BLF_STATUS_CODE eStatus,
        int isize)
{
    if (NULL == pStatus || isize <= 0)
    {
        return;
    }

    for (int i = 0; i < isize; i++)
    {
        pStatus[i] = eStatus;
    }
}

// 初始化分配账号分线状态数组内存
VOID CSCAStatusManager::AllocArray()
{
    if (NULL == m_peAppearanceStatus)
    {
        m_iAppearanceNum = 25;
        m_peAppearanceStatus = new BLF_STATUS_CODE[25];
        SetAllSameStatus(m_peAppearanceStatus, CN_BLA_STATUS_UNKOWN, m_iAppearanceNum);
    }
}

// 从新分配账号分线状态数组内存
VOID CSCAStatusManager::ReAllocArray()
{
    if (NULL == m_peAppearanceStatus)
    {
        return;
    }

    // 从新申请增加25个成员的数组内存,并给申请内存赋初值为Idle
    BLF_STATUS_CODE * pStatus = new BLF_STATUS_CODE[m_iAppearanceNum + 25];
    SetAllSameStatus(pStatus, CN_BLA_STATUS_IDLE, m_iAppearanceNum + 25);

    // 将原有数组状态拷贝到新数组并将原有数组内存释放
    memcpy(pStatus, m_peAppearanceStatus, m_iAppearanceNum * sizeof(BLF_STATUS_CODE));
    delete[] m_peAppearanceStatus;

    m_iAppearanceNum += 25;
    m_peAppearanceStatus = pStatus;
}

// 判断该状态是否要加入链表
BOOL CSCAStatusManager::BeAddStatus2List(BLF_STATUS_CODE eStatus)
{
    return (CN_BLA_STATUS_ALERTING <= eStatus && eStatus <= CN_BLA_STATUS_BRIDGE_HELD);
}

// 设置账号分线状态到数组
VOID CSCAStatusManager::SetStatus2Array(int iIndex, BLF_STATUS_CODE eStatus)
{
    SCA_INFO("SetStatus2Array index(%d) eStatus (%d) m_iAppearanceNum(%d)", iIndex, eStatus,
             m_iAppearanceNum);
    if (iIndex > 0)
    {
        if (iIndex >= MAX_SHARELINE_INDEX)
        {
            SCA_ERR("WARN:BlaIndex >= 200 ");
            return;
        }

        // 如果设置状态的序号大于当前数组的范围,则扩展数组
        while (iIndex > m_iAppearanceNum)
        {
            ReAllocArray();
        }
        m_peAppearanceStatus[iIndex - 1] = eStatus;
    }
}

// 清空状态数组
VOID CSCAStatusManager::ClearStatusArray()
{
    if (m_peAppearanceStatus != NULL)
    {
        m_iAppearanceNum = 0;
        delete[] m_peAppearanceStatus;
        m_peAppearanceStatus = NULL;
    }
}

// 根据序号获取线路信息
SCA_Info * CSCAStatusManager::GetScaInfoByIndex(int iIndex)
{
    if (iIndex > 0)
    {
        ListSCAInfo::iterator it = m_listSCAInfo.begin();
        ListSCAInfo::iterator itEnd = m_listSCAInfo.end();
        for (; it != itEnd; ++it)
        {
            SCA_Info * pInfo = (SCA_Info *)(*it);
            if (NULL != pInfo
                    && pInfo->iIndex == iIndex)
            {
                return pInfo;
            }
        }
    }
    return NULL;
}

// 根据状态获取链表中第一个处于该状态的分线信息
SCA_Info * CSCAStatusManager::GetFirstInfoByStatus(BLF_STATUS_CODE eStatus, int iDsskeyIndex)
{
    ListSCAInfo::iterator it = m_listSCAInfo.begin();
    ListSCAInfo::iterator itEnd = m_listSCAInfo.end();
    for (; it != itEnd; ++it)
    {
        SCA_Info * pInfo = (SCA_Info *)(*it);
        if (pInfo != NULL
                && pInfo->eStatus == eStatus
                && pInfo->iDsskeyIndex == iDsskeyIndex)
        {
            return pInfo;
        }
    }
    return NULL;
}

// 根据状态获取处于该状态的本路通话
CSessionProxy CSCAStatusManager::GetSessionBySCAStatus(BLF_STATUS_CODE eStatus)
{
    for (int i = 0; i < m_iAppearanceNum; i++)
    {
        if (m_peAppearanceStatus[i] == eStatus)
        {
            return talklogic_GetSessionByBLAIndex(m_iLineID, i + 1);
        }
    }

    return CSessionProxy();
}

// 更新share line账号指示灯状态
VOID CSCAStatusManager::UpDateLightInfo()
{
    // 不是SCA账号过滤掉
    if (m_eBlaType == BLA_NONE)
    {
        //SCA_WARN("Can't get account object by lineID[%d]", m_iLineID);
        return;
    }

    YLList<int> listLinekey;
    dsskey_GetDsskeysByAccID(listLinekey, m_iLineID);
    if (listLinekey.size() == 0)
    {
        return;
    }

    int iDsskeyIndex = 1;
    int iLineNum = listLinekey.size();
    YLList<int>::iterator iter = listLinekey.begin();
    for (; iter != listLinekey.end(); iter++)
    {
        // 获取线路状态
        BLF_STATUS_CODE eOldSCAStatus = GetOldSCAStatus(*iter);
        BLF_STATUS_CODE eSCAStatus  = CN_BLA_STATUS_UNKOWN;

        CSessionProxy proxy = talk_GetSessionByDsskey(*iter);
        if (!proxy)
        {
            // 处于idle获取该Dsskey最优状态
            eSCAStatus = GetHighSCAStatus(iDsskeyIndex, iLineNum);
        }
        else
        {
            // 存在通话以通话的状态为准
            if (proxy.IsSingle())
            {
                int iSCAIndex = proxy.GetCallInfo()->m_iBLAIndex;

                SCA_INFO("Get iSCAIndex [%d] SessionID [%d]", iSCAIndex, (int)proxy);

                if (iSCAIndex > 0 && iSCAIndex - 1 <= m_iAppearanceNum)
                {
                    eSCAStatus = m_peAppearanceStatus[iSCAIndex - 1];
                }
            }
        }

        // 线路状态有变化才去刷新
        if (eOldSCAStatus != eSCAStatus)
        {
            SCA_INFO("Update Dsskey [DsskeyID = %d  SCAStatus = %d   old = %d]", *iter,  eSCAStatus,
                     eOldSCAStatus);
            dsskey_UpdateStatus(*iter, dsskey_MapStatusToDsskeyStatus(eSCAStatus), DS_BLF_FIRST, DS_BLF_LAST);
        }

        iDsskeyIndex++;
    }
}

int CSCAStatusManager::GetDsskeyIndex(YLList<int> & ListDsskey, int iDsskeyIndex)
{
    // 获取更新指示灯的按键
    if (iDsskeyIndex <= 0
            || ListDsskey.size() == 0)
    {
        SCA_WARN("Can't get DsskeyID \n");
        return -1;
    }

    iDsskeyIndex = iDsskeyIndex % ListDsskey.size();
    if (iDsskeyIndex == 0)
    {
        iDsskeyIndex = ListDsskey.size();
    }

    YLList<int>::iterator iter = ListDsskey.begin();
    for (int i = 1; iter != ListDsskey.end(); iter++, i++)
    {
        if (i == iDsskeyIndex)
        {
            return *iter;
        }
    }

    return -1;
}

// 获取状态的优先级,最高优先级为1,,其次为2,然后依次类推
int CSCAStatusManager::GetStatusPriority(BLF_STATUS_CODE eStatus)
{
    int iRet = 1000;
    switch (eStatus)
    {
    case CN_BLA_STATUS_ALERTING:
        {
            // 来电
            iRet = 1;
        }
        break;
    case CN_BLA_STATUS_HELD:
        {
            // public hold，群组内任何人都可以retrieve
            iRet = 2;
        }
        break;
    case CN_BLA_STATUS_ACTIVE:
        {
            // 接通
            iRet = 3;
        }
        break;
    case CN_BLA_STATUS_HELD_PRIVATE:
        {
            // prive hold，只有hold的那个人能retrieve
            iRet = 4;
        }
        break;
    case CN_BLA_STATUS_BRIDGE_ACITVE:
        {
            // 多方通话，至少有两方处于通话状态
            iRet = 5;
        }
        break;
    case CN_BLA_STATUS_BRIDGE_HELD:
        {
            // 多方通话，群组成员都按hold
            iRet = 6;
        }
        break;
    case CN_BLA_STATUS_SEIZED:
        {
            // 占线
            iRet = 7;
        }
        break;
    case CN_BLA_STATUS_PROGRESSING:
        {
            // 回铃
            iRet = 8;
        }
        break;
    default:
        {
            iRet = 1000;
        }
        break;
    }

    return iRet;
}

// 获取链表中该Dsskey亮灯优先级最高的账号分线信息
BLF_STATUS_CODE CSCAStatusManager::GetHighSCAStatus(int iDsskeyId, int iLineNum)
{
#if IF_BUG_39041
    if (!acc_IsSupportSCA(m_iLineID))
    {
        return CN_BLA_STATUS_UNKOWN;
    }
#endif
    // 从属于该dsskey的SCA状态中获取最优状态
    BLF_STATUS_CODE eStatus = CN_BLA_STATUS_IDLE;
    if (iLineNum <= 0)
    {
        return eStatus;
    }

    for (int i = iDsskeyId - 1; i < m_iAppearanceNum; i = i + iLineNum)
    {
        if (GetStatusPriority(m_peAppearanceStatus[i]) < GetStatusPriority(eStatus))
        {
            eStatus = m_peAppearanceStatus[i];
        }
    }

    //SCA_INFO("GetHighSCAStatus iDsskeyId [%d], eSCAStatus = %d", iDsskeyId, eStatus);
    return eStatus;
}

// 判断是否要显示Share Line通话选择界面
BOOL CSCAStatusManager::BeShowSharedCalls()
{
    // 非BSFT 不支持SCA MENU
    if (m_eBlaType != BLA_BROADSOFT_SCA)
    {
        return FALSE;
    }

    ListSCAInfo::iterator it = m_listSCAInfo.begin();
    ListSCAInfo::iterator itEnd = m_listSCAInfo.end();
    for (; it != itEnd; ++it)
    {
        SCA_Info * pInfo = (SCA_Info *)(*it);
        if (pInfo != NULL
                && pInfo->eStatus != CN_BLA_STATUS_HELD_PRIVATE)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void CSCAStatusManager::PrintAppearance()
{
    if (m_peAppearanceStatus != NULL)
    {
        for (int i = 0; i < m_iAppearanceNum; i++)
        {
            if (!IS_STATUS_IDLE(m_peAppearanceStatus[i]))
            {
                SCA_INFO("m_peAppearanceStatus[%d] = [%d]", i, m_peAppearanceStatus[i]);
            }
        }
    }
}

BLF_STATUS_CODE CSCAStatusManager::GetStatusByIndex(int iIndex)
{
    PrintAppearance();
    SCA_INFO("CSCAStatusManager::GetStatusByIndex(%d) m_iAppearanceNum(%d)", iIndex, m_iAppearanceNum);

    if (m_peAppearanceStatus != NULL
            && iIndex > 0
            && iIndex < m_iAppearanceNum)
    {
        return m_peAppearanceStatus[iIndex - 1];
    }

    return CN_BLA_STATUS_UNKOWN;
}

VOID CSCAStatusManager::SetFreeIndex(int iIndex)
{
    if (m_peAppearanceStatus != NULL
            && iIndex < m_iAppearanceNum
            && iIndex > 0)
    {
        // DraftBLA 本地释放Index时，sip并未释放，因此不是标志位IDLE，标志Unkown等待sip上报IDLE消息
        m_peAppearanceStatus[iIndex - 1] = m_eBlaType != BLA_BROADSOFT_SCA ? CN_BLA_STATUS_UNKOWN :
                                           CN_BLA_STATUS_IDLE;
    }
}

int CSCAStatusManager::GetBLAIndex(int & iBLAIndex, int iKeyCounts)
{
    SCA_INFO("GetBLAIndex [%d][%d]", iBLAIndex, iKeyCounts);
    if (m_peAppearanceStatus == NULL || iBLAIndex <= 0)
    {
        return 0;
    }

    while (1)
    {
        while (iBLAIndex > m_iAppearanceNum)
        {
            if (m_iAppearanceNum >= MAX_SHARELINE_INDEX)
            {
                SCA_WARN("WARN:BlaIndex >= 200 ");
                return 0;
            }

            ReAllocArray();
        }

        if (IS_STATUS_IDLE(m_peAppearanceStatus[iBLAIndex - 1]))
        {
            if (m_eBlaType != BLA_DRAFT_BLA || !talklogic_IsBlaIndexUsing(iBLAIndex))
            {
                SCA_INFO("iBLAIndex = %d", iBLAIndex);
                return iBLAIndex;
            }
        }

        iBLAIndex = iBLAIndex + iKeyCounts;
    }

    return 0;
}

// 获取dsskeyid对应的线路状态
BLF_STATUS_CODE CSCAStatusManager::GetStatusByDsskeyID(int iDsskeyID)
{
    BLF_STATUS_CODE eStatus = CN_BLA_STATUS_UNKOWN;

    YLList<int> listLinekey;
    dsskey_GetDsskeysByAccID(listLinekey, m_iLineID);
    if (listLinekey.size() == 0)
    {
        return eStatus;
    }

    int iIndex = 1;
    YLList<int>::ListIterator iter = listLinekey.begin();
    for (; iter != listLinekey.end(); iter++, iIndex++)
    {
        if (*iter == iDsskeyID)
        {
            break;
        }
    }

    return GetHighSCAStatus(iIndex, listLinekey.size());
}

BLF_STATUS_CODE CSCAStatusManager::GetOldSCAStatus(int iDsskeyID)
{
    BLF_STATUS_CODE eStatus = CN_BLA_STATUS_UNKOWN;

    YLList<Dsskey_Status> listOutput;
    dsskey_GetDsskeyStatus(iDsskeyID, listOutput);

    YLList<Dsskey_Status>::ListIterator iter = listOutput.begin();
    for (; iter != listOutput.end(); iter++)
    {
        if (*iter > DS_BLF_FIRST
                && *iter < DS_BLF_LAST)
        {
            eStatus = (BLF_STATUS_CODE)(*iter - DS_BLA_STATUS_IDLE + CN_BLA_STATUS_IDLE);
            break;
        }
    }

    return eStatus;
}

void CSCAStatusManager::ReloadConfig()
{
    BLA_TYPE eBlaType = (BLA_TYPE)acc_GetBLAType(m_iLineID);

    if (m_eBlaType != eBlaType)
    {
        SCA_INFO("ReloadConfig line=[%d] type=[%d]->[%d]", m_iLineID, m_eBlaType, eBlaType);
        m_eBlaType = eBlaType;
        Release();
        AllocArray();
    }

    UpDateLightInfo();
}

int CSCAStatusManager::GetBlaIndexByStatus(BLF_STATUS_CODE eStatus, int iDsskeyIndex)
{
    YLList<int> listLinekey;
    dsskey_GetDsskeysByAccID(listLinekey, m_iLineID);
    int iLineNum = listLinekey.size();

    if (iLineNum == 0)
    {
        return 0;
    }

    int iIndex = 0;
    bool bFind = false;
    for (YLList<int>::iterator it = listLinekey.begin(); it != listLinekey.end(); ++it, ++iIndex)
    {
        // 先找到对应的Dsskey
        if (*it == iDsskeyIndex)
        {
            bFind = true;
            break;
        }
    }

    if (!bFind)
    {
        return 0;
    }

    SCA_INFO("GetBlaIndexByStatus index=[%d] status=[%d] dsskey=[%d]", iIndex, eStatus, iDsskeyIndex);

    for (int i = 0; i < m_iAppearanceNum; ++i)
    {
        if (m_peAppearanceStatus[i] == eStatus && (i % iLineNum) == iIndex)
        {
            return iIndex + 1;
        }
    }

    return 0;
}

int CSCAStatusManager::GetCurrentCallNum(int iAccount/* = 0*/, int nDsskeyIndex /*= 0*/)
{
    if (m_peAppearanceStatus == NULL || nDsskeyIndex == 0)
    {
        return 0;
    }

    int iDssNum = dsskey_CountsDsskeyByAccID(iAccount);

    if (iDssNum <= 0)
    {
        return 0;
    }

    int iCallNum = 0;
    for (int iIndex = 0; iIndex < m_iAppearanceNum; iIndex++)
    {
        if (nDsskeyIndex < 0 || (iIndex % iDssNum) == (nDsskeyIndex - 1))
        {
            if (!IS_STATUS_IDLE(m_peAppearanceStatus[iIndex]))
            {
                iCallNum++;
            }
        }
    }

    return iCallNum;
}

BLF_STATUS_CODE CSCAStatusManager::GetStatus2UIShow(int iAccountId, int iIndex)
{
    if (iIndex > 0 && iIndex - 1 <= m_iAppearanceNum)
    {
        if (m_peAppearanceStatus[iIndex - 1] == CN_BLA_STATUS_BRIDGE_HELD)
        {
            // 显示规则为如果是处于通话的话机则显示为bargain held，其他监控话机显示为bargain active
            YLVector<int> vecSessionId;
            talklogic_GetAllSessionID(vecSessionId);
            for (int i = 0; i < vecSessionId.size(); ++i)
            {
                CCallInfo * pCallInfo = talklogic_GetCallInfoBySessionID(vecSessionId[i]);
                if (NULL != pCallInfo && pCallInfo->m_iBLAIndex == iIndex)
                {
                    return CN_BLA_STATUS_BRIDGE_HELD;
                }
            }

            return CN_BLA_STATUS_BRIDGE_ACITVE;
        }
        else
        {
            return m_peAppearanceStatus[iIndex - 1];
        }
    }
    return CN_BLA_STATUS_IDLE;
}

void CSCAStatusManager::Release()
{
    ClearStatusArray();
    ClearList();
}

void CSCAStatusManager::AddSCAInfo(SCA_Info & sInfo)
{
    SetStatus2Array(sInfo.iIndex, sInfo.eStatus);

    if (BeAddStatus2List(sInfo.eStatus))
    {
        YLList<int> listLinekey;
        dsskey_GetDsskeysByAccID(listLinekey, m_iLineID);
        sInfo.iDsskeyIndex = GetDsskeyIndex(listLinekey, sInfo.iIndex);

        SCA_INFO("[SCA] AddScaInfo: BlaIndex[%d] status[%d] display[%s] uri[%s] DsskeyIndex[%d]",
                 sInfo.iIndex, sInfo.eStatus, sInfo.strDisplay.c_str(), sInfo.strURI.c_str(), sInfo.iDsskeyIndex);

        ListSCAInfo::iterator it = m_listSCAInfo.begin();
        for (; it != m_listSCAInfo.end(); ++it)
        {
            if ((*it)->iIndex == sInfo.iIndex)
            {
                **it = sInfo;
                return;
            }
        }

        SCA_Info * pTemp = new SCA_Info;
        *pTemp = sInfo;
        m_listSCAInfo.push_back(pTemp);

        int iCallid = _BSSCAManager.GetCallIdBySCAInfo(m_iLineID, sInfo.iIndex);
        if (iCallid != -1)
        {
            talklogic_UpdateRemoteDispaly(iCallid, sInfo.strDisplay, sInfo.strURI);
        }
    }
    else
    {
        ListSCAInfo::iterator it = m_listSCAInfo.begin();
        for (; it != m_listSCAInfo.end(); ++it)
        {
            if ((*it)->iIndex == sInfo.iIndex)
            {
                m_listSCAInfo.erase(it);
                break;
            }
        }
    }
}

#endif // #ifdef IF_FEATURE_BROADSOFT_SCA
