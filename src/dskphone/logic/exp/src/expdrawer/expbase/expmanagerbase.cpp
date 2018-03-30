#include "expmanagerbase.h"
#include "expdefine.h"

#include "devicelib/phonedevice.h"
#include "updatelistthread.h"
#include <sys/time.h>
#include "taskaction/modtaskaction.h"
#include "commonunits/commonunits_def.h"

namespace
{
#define VP_PHONE_THREAD_NAME                    "app_vpPhone"
#define EXP_LIST_FLUSH_DEFAULT_INDEX            (-1)
}

EXPManagerBase::EXPManagerBase()
    : m_uInitDelayTime(0)
    , m_bInit(false)
{
    m_vecExpInfoReceive.clear();

    for (int i = 0; i < EXP_MAX_COUNT; ++i)
    {
        m_arrRefreshBGFlush[i] = false;
    }
}


EXPManagerBase::~EXPManagerBase()
{

    m_mutexListReceive.Unlock();
}

void EXPManagerBase::Init()
{

}

void EXPManagerBase::Uninit()
{
    m_mutexListReceive.Unlock();
}

bool EXPManagerBase::SetDelayTimer()
{
    timerKillThreadTimer((UINT)&m_uInitDelayTime);
    UINT ret = timerSetThreadTimer((UINT)&m_uInitDelayTime, m_uInitDelayTime);
    return ret > 0;
}

bool EXPManagerBase::OnDelayTimer(UINT uTimerId)
{
    if (uTimerId != (UINT)&m_uInitDelayTime)
    {
        return false;
    }

    EXP_INFO("[Exp-Info]EXPManager::OnDelayTimer\n");

    timerKillThreadTimer((UINT)&m_uInitDelayTime);

    TaskAction_CreateSpecialThread(NS_TA::TA_REQUEST_EXP_UPDATE_LIST,
                                   TA_THREAD_EXP_UPDATE_LIST,
                                   CUpdateListThread::OnMessage);
    _ExpUpdateListThread.SetEXPManager(this);

    msgPostMsgToThreadEx(
        msgGetThreadByName(VP_PHONE_THREAD_NAME)
        , EXP_MESSAGE_PROCESS_READY
        , 0
        , 0
        , 0
        , NULL);

    return true;
}

void EXPManagerBase::UpdateMsgDispose(vecExpItemInfo & vecExpInfoFlush, int & nEXPNum)
{
    int nSize = vecExpInfoFlush.size();

    //该数组用以存储刷新的信息，第一维数值为EXP的索引，第二维数值为EXP对应的按键值
    memset(m_arrFlushInfo, EXP_LIST_FLUSH_DEFAULT_INDEX, sizeof(m_arrFlushInfo));
    memset(m_arrbWithData, false, sizeof(m_arrbWithData));

    nEXPNum = EXP_LIST_FLUSH_DEFAULT_INDEX;

    //遍历刷新列表，以此来对二维数组进行赋值
    for (int i = 0; i < nSize; ++i)
    {
        int iKeyIndex = vecExpInfoFlush[i].m_nRol * 2 + (vecExpInfoFlush[i].m_nCol + 1);
        if (iKeyIndex < 0 || iKeyIndex > EXP_LIST_FLUSH_KEY_MAX_NUM
                || vecExpInfoFlush[i].m_nIndex < 0
                || vecExpInfoFlush[i].m_nIndex >= EXP_LIST_FLUSH_MAX_NUM)
        {
            EXP_WARN("Array Index Error: KeyIndex[%d], Exp Port[%d]\n", iKeyIndex, vecExpInfoFlush[i].m_nIndex);

            continue;
        }

        if (EXP_LIST_FLUSH_DEFAULT_INDEX == m_arrFlushInfo[vecExpInfoFlush[i].m_nIndex][iKeyIndex])
        {
            //统计要刷新的dsskey个数，相同dsskey不重复统计
            if (iKeyIndex > 0 && iKeyIndex <= EXP_LIST_FLUSH_DSSKEY_MAX_NUM)
            {
                ++m_arrFlushInfo[vecExpInfoFlush[i].m_nIndex][0];
            }

            if (vecExpInfoFlush[i].m_nIndex > nEXPNum)
            {
                nEXPNum = vecExpInfoFlush[i].m_nIndex;
            }

            m_arrbWithData[vecExpInfoFlush[i].m_nIndex] = true;
            m_arrFlushInfo[vecExpInfoFlush[i].m_nIndex][iKeyIndex] = i;
        }
        else
        {
            if (m_arrFlushInfo[vecExpInfoFlush[i].m_nIndex][iKeyIndex] >= 0
                    && m_arrFlushInfo[vecExpInfoFlush[i].m_nIndex][iKeyIndex] < nSize)
            {
                ExpDisplayInfoMerge(vecExpInfoFlush[m_arrFlushInfo[vecExpInfoFlush[i].m_nIndex][iKeyIndex]],
                                    vecExpInfoFlush[i]);
            }
        }
    }

    if (nEXPNum >= EXP_LIST_FLUSH_MAX_NUM)
    {
        nEXPNum = EXP_LIST_FLUSH_MAX_NUM - 1;
    }
}

void EXPManagerBase::ExpDisplayInfoMerge(ExpDisplayItemInfo & objDstData,
        ExpDisplayItemInfo & objMergeData)
{
    EXP_INFO("ExpDisplayInfoMerge Index: Dst %d, Merge %d\n"
             , objDstData.m_nIndex, objMergeData.m_nIndex);

    objDstData.bDown        = objMergeData.bDown;
    objDstData.bEditing     = objMergeData.bEditing;
    objDstData.flag         = objMergeData.flag;
    objDstData.m_eBgType    = objMergeData.m_eBgType;

    objDstData.nCurrentPage = objMergeData.nCurrentPage;
    //如果新的刷新信息有重复，覆盖
    if (objMergeData.bPaintIcon)
    {
        EXP_INFO("ExpDisplayInfoMerge Icon: Dst %d, Merge %d\n"
                 , objDstData.m_nIndex, objMergeData.m_nIndex);

        objDstData.bPaintIcon = true;
        objDstData.icon = objMergeData.icon;
        strcpy(objDstData.szIcon, objMergeData.szIcon);
        objDstData.nStatus = objMergeData.nStatus;
    }
    if (objMergeData.bPaintText)
    {
        EXP_INFO("ExpDisplayInfoMerge Text: Dst %s, Merge %s\n"
                 , objDstData.szLabel, objMergeData.szLabel);

        objDstData.bPaintText = true;
        strcpy(objDstData.szLabel, objMergeData.szLabel);
    }
    if (objMergeData.bLight)
    {
        EXP_INFO("ExpDisplayInfoMerge Light: Dst %d, Merge %d\n"
                 , objDstData.ledStatus, objMergeData.ledStatus);

        objDstData.bLight = true;
        objDstData.ledStatus = objMergeData.ledStatus;
    }
}

void EXPManagerBase::PostToUpdateList()
{
    msgPostMsgToThread(msgGetThreadByName(TA_THREAD_EXP_UPDATE_LIST),
                       EXP_MESSAGE_UPDATE_KEY, 0, 0);
}

LRESULT EXPManagerBase::OnEXPMessage(msgObject & objMessage)
{

    switch (objMessage.message)
    {
    case TM_TIMER:
        {
            OnDelayTimer(objMessage.wParam);
        }
        break;
    case EXP_MESSAGE_UPDATE_KEY:
        {
            EXP_INFO("on [EXP_MESSAGE_UPDATE_KEY] w:[%d] l[%d]", objMessage.wParam, objMessage.lParam);
            if (!m_bInit)
            {
                break;
            }

            int nCount = objMessage.lParam;
            if (nCount <= 0)
            {
                break;
            }

            ExpDisplayItemInfo * pEXPDisplayInfo = NULL;
            pEXPDisplayInfo = (ExpDisplayItemInfo *)objMessage.GetExtraData();

            if (NULL != pEXPDisplayInfo)
            {
                SaveToList(pEXPDisplayInfo, nCount);
            }
        }
        break;

    default:
        break;
    }

    return 0;
}

void EXPManagerBase::SaveToList(ExpDisplayItemInfo * objUpdateInfo, int nCount)
{
    if (NULL == objUpdateInfo || nCount <= 0)
    {
        return;
    }

    m_mutexListReceive.Lock();

    ExpDisplayItemInfo * tmpUpdateInfo = objUpdateInfo;

    //判断传递过来的信息头是否为需要刷新一整页
    bool bIsFirstIsFlag = ((*tmpUpdateInfo).m_eDataType == EXP_UPDATE_DATA_FLASH_PAGE);

    if (bIsFirstIsFlag)
    {
        //如需要刷新一整页
        if (0 <= (*tmpUpdateInfo).m_nIndex && (*tmpUpdateInfo).m_nIndex < EXP_MAX_COUNT)
        {
            m_arrRefreshBGFlush[(*tmpUpdateInfo).m_nIndex] = true;
            ++tmpUpdateInfo;
        }
    }

    for (int i = (bIsFirstIsFlag ? 1 : 0); i < nCount; ++i)
    {
        m_vecExpInfoReceive.push_back(*tmpUpdateInfo);
        ++tmpUpdateInfo;
    }
    EXP_INFO("EXP drawer saved to receive list size[%d] ", m_vecExpInfoReceive.size());
    m_mutexListReceive.Unlock();

    PostToUpdateList();

}

void EXPManagerBase::SetDelayTimeNum(UINT uTimeNum)
{
    m_uInitDelayTime = uTimeNum;
}
