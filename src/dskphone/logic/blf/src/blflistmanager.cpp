#include "blf_inc.h"
#include "blflistmanager.h"
#include "blfpraseaction.h"
#include "blflistmsgdefine.h"
#include "callpickupmanager.h"

CBLFlistManager & CBLFlistManager::GetInstance()
{
    static CBLFlistManager Instace;
    return Instace;
}

CBLFlistManager::CBLFlistManager() : m_pEnabled(new char[acc_AccountNum()])
{
    m_eManagerType = BLFLIST_Manager;
    memset(m_pEnabled, 1, acc_AccountNum());
}

CBLFlistManager::~CBLFlistManager()
{
    delete[] m_pEnabled;
}

void CBLFlistManager::Init()
{
    etl_RegisterMsgHandle(SIP_SUB_UPDATE_BLFLIST, SIP_SUB_UPDATE_BLFLIST,
                          &CBLFlistManager::OnBlflistMsgCallBack);

    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                          &CBLFlistManager::OnBlflistMsgCallBack);

    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
		CONFIG_MSG_BCAST_CHANGED, &CBLFlistManager::OnBlflistMsgCallBack);

    etl_RegisterMsgHandle(BLF_CUSTOM_DATA_CHANAGE,
                          BLF_CUSTOM_DATA_CHANAGE, &CBLFlistManager::OnBlflistMsgCallBack);
}

LRESULT CBLFlistManager::OnBlflistMsgCallBack(msgObject & msg)
{
    APP_INFO("CBLFlistManager::OnBlflistMsgCallBack,msg[%d],wparm[%d],lparm[%d]", msg.message,
             msg.wParam, msg.lParam);
    switch (msg.message)
    {
    case BLF_CUSTOM_DATA_CHANAGE:
		{
			g_objBLFListManager.FlushALLDsskeyLight();
		}
		break;
	case CONFIG_MSG_BCAST_CHANGED:
        {
			if (msg.wParam == ST_BLF_LED_CONFIG)
			{
				g_objBLFListManager.FlushALLDsskeyLight();
			}
        }
        break;
    case SIP_SUB_UPDATE_BLFLIST:

        if (msg.lParam == SIP_BLF_LIST_NOTIFY_TERMINATED)
        {
            DSSKEY_INFO("Unbound ALL BLFList");
            g_objBLFListManager.ResetBlfListByLineId(msg.wParam);
        }
        else
        {
            char * lpBuffer = (char *)msg.GetExtraData();
            if (NULL == lpBuffer)
            {
                DSSKEY_ERR("Blflist get data failed!");
                return true;
            }

            CBLFPraseAction * pAction = CreateBLFPraseAction(lpBuffer,
                                        SIP_BLF_LIST_NOTIFY_IN_FILE == msg.lParam);

            if (NULL != pAction)
            {
                pAction->SetCallback(&g_objBLFListManager);
                pAction->SetExtID(msg.wParam);

                TaskAction_ExecAction(pAction);

                g_objBLFListManager.SetFlag(msg.wParam, true);
            }

            if (SIP_BLF_LIST_NOTIFY_IN_FILE == msg.lParam)
            {
                remove(lpBuffer);
            }
        }
        break;

    case ACCOUNT_STATUS_CHANGED:
        {
            g_objBLFListManager.OnLineStateChange(msg);
        }
        break;
    default:
        break;
    }

    return true;
}

bool CBLFlistManager::OnLineStateChange(msgObject & objMsg)
{
    if (objMsg.wParam < 0
            || objMsg.wParam >= acc_AccountNum())
    {
        return false;
    }

    LINE_STATE eState = acc_GetState(objMsg.wParam);
    DSSKEY_INFO(" blflist on line state change:[iAccountID = %d， eState = %d]", objMsg.wParam,
                eState);

    BOOL bStateChanging = eState != LS_REGISTERED
                          && eState != LS_BE_UNREGISTERED
                          && eState != LS_DISABLE
                          && eState != LS_REGISTER_FAIL;

    if (bStateChanging)
    {
        DSSKEY_INFO("Skip line state change!");
        return false;
    }

    if (!acc_IsAccountAvailable(objMsg.wParam))
    {
        // 清空BLFList状态
        ResetBlfListByLineId(objMsg.wParam);
    }
    // 通过查询使账号注册上, 重新请求缓存
    // 主要是解决网络恢复重新刷新的问题
    else if (*((bool *)objMsg.GetExtraData()))
    {
        QueryBlfList(objMsg.wParam);
    }

    return true;
}

void CBLFlistManager::ProcessTaskActionCallBack(CBLFPraseAction * pAction)
{
    if (NULL == pAction)
    {
        return ;
    }

    int nLine = pAction->GetExtId();

    if (!IsEnable(nLine))
    {
        DSSKEY_WARN("cancel blflist line[%d]", nLine);
        return;
    }

    YLList<BLFBaseData *> refListData = pAction->GetListData();

    DSSKEY_INFO("pAction->IsFullStatus[%d]", pAction->IsFullStatus());

    // 更新完全数据的处理
    if (pAction->IsFullStatus())
    {
        // http://192.168.1.99/Bug.php?BugID=20717
        // 小包的特别在于第一次推送分多次而已
        if (0 == configParser_GetConfigInt(kszBLFListVersionActive)
                || refListData.size() > 0 && (*refListData.begin())->m_nVersion == 0)
        {
            ClearAllData(nLine);
        }

        for (YLList<BLFBaseData *>::ListIterator iter = refListData.begin(); iter != refListData.end();
                iter++)
        {
            BLFBaseData * pData = *iter;

            if (NULL == pData)
            {
                continue;
            }

            if (!AddIter(pData, nLine))
            {
                delete pData;
                pData = NULL;
            }
        }

        // 设置都全autoset模块处理，此处发送blflist数据更新消息出去
        etl_NotifyApp(false, BLFLIST_DATA_UPDATE, nLine, 0);
        return ;
    }

    // 更新部分数据的处理
    for (YLList<BLFBaseData *>::ListIterator iter = refListData.begin(); iter != refListData.end();
            iter++)
    {
        BLFBaseData * pData = *iter;

        if (pData == NULL)
        {
            continue;
        }

#if IF_FEATURE_SHOW_BLF_CALLID
        if (configParser_GetConfigInt(kszBLFShowCallId) != 0
                && pData->m_bIsFullStatus)
        {
            for (YLList<DialgInfo*>::ListIterator iter = pData->m_ListDialgInfo.begin(); iter != pData->m_ListDialgInfo.end(); iter++)
            {
                DialgInfo* pTempDialog = *iter;
                if (NULL != pTempDialog)
                {
                    GetDialogCallType(pData->m_strKey, pTempDialog);
                }
            }
        }
#endif
        ProcessStatusChange(pData);
    }

    pAction->ClearListInfo();
}

// 重置BLFListDsskey
void CBLFlistManager::ResetBlfListByLineId(int iLineID)
{
    SetFlag(iLineID, false);

    // 需要清除所有blflist，先把数据全部清除掉
    if (ClearAllData(iLineID))
    {
        etl_NotifyApp(FALSE, BLFLIST_DATA_UPDATE, iLineID, 0);
    }
}

// 获取blflist的数据
bool CBLFlistManager::GetBlfListData(YLVector<AccountMoitorData> & refVector)
{
    for (int i = 0; i < m_vBlfAccoutData.size(); ++i)
    {
        AccountMoitorData objData;

        objData.m_nAccount = i;
        ListMonitor & keyList = m_vBlfAccoutData[i];
        for (ListMonitorIter iter = keyList.begin(); iter != keyList.end(); ++iter)
        {
            BLFBaseData * pData = GetBLFDataBykey(*iter);
            if (pData != NULL)
            {
                objData.m_listData.push_back(pData);
            }
        }

        if (objData.m_listData.size() != 0)
        {
            refVector.push_back(objData);
        }
    }
    return true;
}

#if IF_BUG_32812
//获取对应账号监控列表
bool CBLFlistManager::GetBLFListMonitorList(const int iLineID, YLList<yl::string> & listMonitor)
{
    if (iLineID < 0
            || iLineID >= m_vBlfAccoutData.size())
    {
        return false;
    }

    listMonitor.clear();

    ListMonitor & keyList = m_vBlfAccoutData[iLineID];
    for (ListMonitorIter iter = keyList.begin(); iter != keyList.end(); ++iter)
    {
        BLFBaseData * pData = GetBLFDataBykey(*iter);
        if (pData == NULL)
        {
            return false;
        }

        listMonitor.push_back(pData->m_strNumber);
    }

    return listMonitor.size() > 0;
}

bool CBLFlistManager::IsAccountHaveBLFListData(const int iLineID)
{
    if (iLineID < 0
            || iLineID >= m_vBlfAccoutData.size())
    {
        return false;
    }

    return m_vBlfAccoutData[iLineID].size() > 0;
}
#endif

// 记住监控该号码是用哪个dsskey来绑定的
void CBLFlistManager::SetDsskeyId(int iDsskeyId, int iAccountId, const yl::string & strKey)
{
    BLFBaseData * pData = GetBLFDataBykey(strKey);
    if (NULL != pData)
    {
        pData->CheckBindDsskey(iDsskeyId, iAccountId, pData->m_strNumber);
        pData->m_nBindDsskeyId.push_back(iDsskeyId);
        DSSKEY_INFO("BlfList Set DsskeyId[%d] Url[%s]", iDsskeyId, strKey.c_str());

        // 设置PickUP
        if (DS_BLF_STATUS_RINGING == pData->GetShowStatus())
        {
            g_objPickupManager.UpdateCallPickup(pData->m_strKey, m_eManagerType, Opt_Add);
        }
    }
}

bool CBLFlistManager::QueryBlfList(int iAccount)
{
    if (acc_IsAccountAvailable(iAccount))
    {
        DSSKEY_INFO("QueryBlfList [%d]", iAccount);
        sip_blflist_get_body(mkGetCurrent(), iAccount, NULL, 0);
    }
    return true;
}

// 解绑dsskeyid，由网页把blflist设置成别的类型，需要把该dsskeyid解绑掉
void CBLFlistManager::UnBindDsskeyId(int iDsskeyId, int iAccountId)
{
    DSSKEY_INFO("BlfList UnBindDsskeyId [%d] [%d]", iDsskeyId, iAccountId);

    if (iAccountId == -1)
    {
        for (int i = 0; i < m_vBlfAccoutData.size(); ++i)
        {
            UnBindDsskeyByAccountID(iDsskeyId, i);
        }
    }

    if (iAccountId < 0
            || iAccountId >= m_vBlfAccoutData.size())
    {
        return;
    }

    UnBindDsskeyByAccountID(iDsskeyId, iAccountId);

}

void CBLFlistManager::UnBindDsskeyByAccountID(int iDsskeyId, int iAccountId)
{
    ListMonitor & keyList = m_vBlfAccoutData[iAccountId];
    for (ListMonitorIter iter = keyList.begin(); iter != keyList.end(); ++iter)
    {
        BLFBaseData * pData = GetBLFDataBykey(*iter);
        if (pData == NULL)
        {
            return;
        }

        for (int i = 0; i < pData->m_nBindDsskeyId.size(); ++i)
        {
            if (pData->m_nBindDsskeyId[i] == iDsskeyId)
            {
                pData->m_nBindDsskeyId.removeAt(i);

                // 设置PickUP
                if (DS_BLF_STATUS_RINGING == pData->GetShowStatus())
                {
                    g_objPickupManager.UpdateCallPickup(pData->m_strKey, m_eManagerType, Opt_Remove);
                }
            }
        }
    }
}

void CBLFlistManager::SetFlag(int iAccountID, bool bEnable)
{
    if (iAccountID >= 0 && iAccountID < acc_AccountNum())
    {
        m_pEnabled[iAccountID] = bEnable;
    }
}

bool CBLFlistManager::IsEnable(int iAccountID)
{
    return iAccountID >= 0 && iAccountID < acc_AccountNum() && m_pEnabled[iAccountID];
}

