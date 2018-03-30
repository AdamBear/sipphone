#include "logic_inc.h"
#include "idlescreen/include/modidlescreen.h"


#ifdef IF_FEATURE_XMLBROWSER

CElementStatus::CElementStatus()
{
}

CElementStatus::~CElementStatus()
{
}

// 创建之后的处理
bool CElementStatus::AfterCreate()
{
    // 向map中增加成员
    // AddToTimeoutMap(m_dataAttr.strSessionID, m_dataAttr.iTimeOut);

    // 取消提示框
    _XMLBrowserManager.ExitMessageBox();
    // 播放提示音
    if (IsNeedPlayBeep())
    {
        _XMLBrowserManager.PlayBeep();
    }

    XBStatusData * pData = (XBStatusData *)CElementBase::TakeoutData();
    if (pData == NULL)
    {
        return false;
    }
    if (CElementStatus::ReceiveStatusData(pData))
    {
        UpdateStatusView();
    }
    else
    {
        ReleaseStatusData(pData);
    }
    return true;
}

// 销毁之前的处理
void CElementStatus::BeforeDestroy()
{
    ReleaseStatusList();
}

// 是否播放提示音
bool CElementStatus::IsNeedPlayBeep()
{
    return _GetXData().stAttr.bBeep;
}

//
void CElementStatus::UpdateStatusView()
{
    typeCusNotifyList listCusNotify;

    // 遍历
    typeListStatus::iterator iter = m_s_listStatus.begin();
    for (; iter != m_s_listStatus.end(); ++iter)
    {
        if ((*iter) == NULL)
        {
            continue;
        }

        XBStatusData * pExistData = (*iter);
        List_Status_Item::iterator iterBegin = pExistData->listItem.begin();
        List_Status_Item::iterator iterEnd = pExistData->listItem.end();
        for (; iterBegin != iterEnd; ++iterBegin)
        {
            StatusItem * pItem = (*iterBegin);
            if (pItem == NULL)
            {
                continue;
            }

#if IF_BUG_31595
            if (pItem->eType == ST_MESSAGE_DSSKEY)
            {
                continue;
            }
#endif

            CustomNotify objNotify;
            objNotify.iAlign = pItem->iAlign;
            objNotify.iSize = pItem->eSize;
            objNotify.iColor = pItem->eColor;
            objNotify.strAccount = pItem->strAccount;
            objNotify.strIcon = pItem->strIconPath;
            objNotify.strText = pItem->strMsg;

            listCusNotify.push_back(objNotify);
        }
    }
    // 更新

    idleScreen_UpdateCustomNotify(listCusNotify);
    // _XMLBrowserManager.UpdateWnd();
}

typeListStatus CElementStatus::m_s_listStatus;

// 更新状态数据
bool CElementStatus::ReceiveStatusData(XBStatusData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    bool bExist = false;
    // 遍历
    typeListStatus::iterator iter = m_s_listStatus.begin();
    for (; iter != m_s_listStatus.end(); ++iter)
    {
        if ((*iter) == NULL)
        {
            continue;
        }

        XBStatusData * pExistData = (*iter);
        if (pExistData->stAttr.strSessionID != pData->stAttr.strSessionID)
        {
            continue;
        }
        // 找到相同的
        bExist = true;

        XMLBROWSER_INFO("remove status data. sessionId[%s], timeout[%d]",
                        pExistData->stAttr.strSessionID.c_str(), pExistData->stAttr.iTimeOut);
        // 停掉定时器
        timerKillThreadTimer(TIMER_ID(pExistData->stAttr.iTimeOut));
        // 释放旧的
        ReleaseStatusData(pExistData);
        // 移出链表
        m_s_listStatus.erase(iter);
        break;
    }
    // 新的数据检查
    if (pData->listItem.size() == 0)
    {
        ReleaseStatusData(pData);
        return true;
    }

#if IF_BUG_31595
    yl::string strInfo = "";
#endif
    // 为空的判断
    bool bEmpty = true;
    List_Status_Item::iterator iterBegin = pData->listItem.begin();
    List_Status_Item::iterator iterEnd = pData->listItem.end();
    for (; iterBegin != iterEnd; ++iterBegin)
    {
        StatusItem * pItem = (*iterBegin);
        if (pItem == NULL)
        {
            continue;
        }
#if IF_BUG_31595
        if (pItem->eType == ST_MESSAGE_DSSKEY)
        {
            //更新xml info状态方式一：按顺序更新XML info类型的dsskey的标签值，不改变类型
            if (!pItem->strMsg.empty())
            {
                strInfo += pItem->strMsg;
                strInfo += '\n';
            }

            //更新xml info状态方式二：将指定的dsskey，修改为xml info，并更新标签，不保存到文件中
            if (!pItem->dsskeyData.strLabel.empty()) // && dsskey_IsValidDsskeyType(pItem->dsskeyData.iDssKeyID, pItem->dsskeyData.eDKType))
            {
                dsskey_SetDsskeyData(pItem->dsskeyData.iDssKeyID, pItem->dsskeyData,
                                     DSSKEY_SET_NOTIFY | DSSKEY_SET_REFRESH);
            }

            continue;
        }
#endif
        if (!pItem->strMsg.empty())
        {
            bEmpty = false;
            break;
        }
    }

#if IF_BUG_31595
    if (!strInfo.empty())
    {
        dsskey_ShowXMLBrowserInfo(strInfo);
    }
#endif

    if (!bEmpty)
    {
        if (pData->stAttr.iTimeOut > 0)
        {
            // 单位为秒
            timerSetThreadTimer(TIMER_ID(pData->stAttr.iTimeOut), pData->stAttr.iTimeOut * 1000);
        }
        // 新的数据，添加到链表
        XMLBROWSER_INFO("add status data. sessionId[%s], timeout[%d]", pData->stAttr.strSessionID.c_str(),
                        pData->stAttr.iTimeOut);
        m_s_listStatus.push_front(pData);
    }
    else
    {
        ReleaseStatusData(pData);
        if (pData->stAttr.strSessionID.empty())
        {
            // SessionID为空，且都是为空的Message，则清空状态列表
            XMLBROWSER_INFO("clean status data.");
            ReleaseStatusList();
        }
    }
    return true;
}
// 释放
void CElementStatus::ReleaseStatusList()
{
    // 遍历
    typeListStatus::iterator iter = m_s_listStatus.begin();
    for (; iter != m_s_listStatus.end(); ++iter)
    {
        if ((*iter) == NULL)
        {
            continue;
        }

        // 停掉定时器
        timerKillThreadTimer(TIMER_ID((*iter)->stAttr.iTimeOut));

        // 释放
        ReleaseStatusData((*iter));
    }
    m_s_listStatus.clear();
}

// 处理定时器
bool CElementStatus::OnTimer(UINT uTimerID)
{
    // 遍历
    typeListStatus::iterator iter = m_s_listStatus.begin();
    for (; iter != m_s_listStatus.end(); ++iter)
    {
        if ((*iter) == NULL)
        {
            continue;
        }

        XBStatusData * pExistData = (*iter);
        if (uTimerID != TIMER_ID(pExistData->stAttr.iTimeOut))
        {
            continue;
        }
        XMLBROWSER_INFO("status timeout. sessionId[%s], timeout[%d]",
                        pExistData->stAttr.strSessionID.c_str(), pExistData->stAttr.iTimeOut);
        // 停掉定时器
        timerKillThreadTimer(TIMER_ID(pExistData->stAttr.iTimeOut));
        // 释放旧的
        ReleaseStatusData(pExistData);
        // 移出链表
        m_s_listStatus.erase(iter);

        UpdateStatusView();
        return true;
    }
    return false;
}

#if IF_FEATURE_START2START_ACD
void CElementStatus::ReleaseStatus(const yl::string & strSessionId)
{
    // 遍历
    typeListStatus::iterator iter = m_s_listStatus.begin();
    for (; iter != m_s_listStatus.end(); iter++)
    {
        if (*iter == NULL)
        {
            continue;
        }

        XBStatusData * pExistData = *iter;
        if (strSessionId != pExistData->stAttr.strSessionID)
        {
            continue;
        }

        XMLBROWSER_INFO("status timeout. sessionId[%s], timeout[%d]",
                        pExistData->stAttr.strSessionID.c_str(), pExistData->stAttr.iTimeOut);
        // 停掉定时器
        timerKillThreadTimer(TIMER_ID(pExistData->stAttr.iTimeOut));
        // 释放旧的
        ReleaseStatusData(pExistData);
        // 移出链表
        m_s_listStatus.erase(iter);

        UpdateStatusView();

        break;
    }
}
#endif

#endif // #ifdef IF_FEATURE_XMLBROWSER

