#include "blfmanager.h"
#include "blf_inc.h"
#include "blflistmsgdefine.h"
#include "callpickupmanager.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"

#define BLF_REQSTATUS_TIMER 6
#define BLF_REQSTATUS_COUNT 3

CBLFManager & CBLFManager::GetInstance()
{
    static CBLFManager Instace;
    return Instace;
}

CBLFManager::CBLFManager()
{
    m_eManagerType = BLF_Manager;

    etl_RegisterMsgHandle(SIP_SUB_UPDATE_BLF_STATUS, SIP_SUB_UPDATE_BLF_STATUS,
                          &CBLFManager::OnBlfMsgCallBack);
    etl_RegisterMsgHandle(SIP_SUB_UPDATE_BLF_STATUS_EXT, SIP_SUB_UPDATE_BLF_STATUS_EXT,
                          &CBLFManager::OnBlfMsgCallBack);

    etl_RegisterMsgHandle(BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE, BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE,
                          &CBLFManager::OnBlfMsgCallBack);
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                          &CBLFManager::OnBlfMsgCallBack);
    etl_RegisterMsgHandle(ACCOUNT_CONFIG_CHANGE, ACCOUNT_CONFIG_CHANGE, &CBLFManager::OnBlfMsgCallBack);
	etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
		CONFIG_MSG_BCAST_CHANGED, &CBLFManager::OnBlfMsgCallBack);

    etl_RegisterMsgHandle(DSK_MSG_EXT_CHANGE, DSK_MSG_EXT_CHANGE, &CBLFManager::OnBlfMsgCallBack);

    etl_RegisterMsgHandle(BLF_CUSTOM_DATA_CHANAGE, BLF_CUSTOM_DATA_CHANAGE,
                          &CBLFManager::OnBlfMsgCallBack);
    etl_RegisterMsgHandle(TALK_MSG_TALK_START, TALK_MSG_TALK_START, &CBLFManager::OnBlfMsgCallBack);
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, &CBLFManager::OnBlfMsgCallBack);
}

CBLFManager::~CBLFManager()
{

}

void CBLFManager::Init()
{
    APP_INFO("CBLFManager::Init()");
    YLList<int> listDsskey;

    dsskey_getAllDsskeys(listDsskey);

    CreatDataByDsskeyId(listDsskey);
}

LRESULT CBLFManager::OnBlfMsgCallBack(msgObject & msg)
{
    if (msg.message == TM_TIMER)
    {
        g_objBLFManager.ProcessTimer(msg.wParam);
        return 0;
    }

    APP_INFO("CBLFManager::OnBlfMsgCallBack,message[%d] wparm[%d],lparm[%d]", msg.message, msg.wParam,
             msg.lParam);
    switch (msg.message)
    {
    case BLF_CUSTOM_DATA_CHANAGE:
		{
			g_objBLFManager.FlushALLDsskeyLight();
		}
		break;
	case CONFIG_MSG_BCAST_CHANGED:
        {
			if (msg.wParam == ST_BLF_LED_CONFIG)
			{
				g_objBLFManager.FlushALLDsskeyLight();
			}
        }
        break;
    case SIP_SUB_UPDATE_BLF_STATUS:
        {
            void * pData = msg.GetExtraData();
            if (NULL == pData)
            {
                break ;
            }

            // 获取更新数量
            int nUpdateCount = msg.GetExtraSize() / sizeof(sip_dsskey_info_t);
            const sip_dsskey_info_t * blfInfo = (sip_dsskey_info_t *)msg.GetExtraData();

            // 遍历Sip的更新消息
            for (int nIndex = 0; nIndex < nUpdateCount; ++nIndex)
            {
                int nDsskeyId = dsskey_GetDsskeyByKeyIndex(blfInfo[nIndex].index);

                // Dsskey不是BLF类型
                if (!g_objBLFManager.IsDsskeySupport(nDsskeyId))
                {
                    continue;
                }

                // 账号未注册, 不更新
                if (!acc_IsAccountAvailable(dsskey_GetDsskeyLine(nDsskeyId)))
                {
                    g_objBLFManager.ReqBlfStatus(nDsskeyId);
                    continue;
                }

                Dsskey_Status eStatus = MapSipBLFStatusToDsskeyStatus(blfInfo[nIndex].status);
#if IF_FEATURE_SHOW_BLF_CALLID
                // 配置开启, 状态由_EXT消息处理
                if (DS_BLF_STATUS_IDLE != eStatus
                        && DS_BLF_STATUS_UNKOWN != eStatus
                        && configParser_GetConfigInt(kszBLFShowCallId) != 0)
                {
                    continue;
                }
#endif
                // 配置开启, Ringing状态由_EXT消息处理
                if (DS_BLF_STATUS_RINGING == eStatus
                        && (configParser_GetConfigInt(kszIndicationBLFPickup) != 0
                            || configParser_GetConfigInt(kszPickupAllowBlfRing) != 0))
                {
                    continue;
                }

                // 更新数据
                BLFBaseData sData;
                sData.m_strKey = GetBLFkeyByDsskeyId(nDsskeyId);
                sData.m_bIsFullStatus = true;

                APP_INFO("------------------nUpdateSize[%d], index[%d], m_strKey[%s], nDsskeyId[%d], eStatus[%d]",
                         nUpdateCount, blfInfo[nIndex].index, sData.m_strKey.c_str(), nDsskeyId, blfInfo[nIndex].status);

                DialgInfo * pDialog = new DialgInfo;
                if (NULL == pDialog)
                {
                    continue;
                }

                pDialog->m_eStatus = eStatus;
                pDialog->m_strDialgId = commonAPI_FormatString("%d", nDsskeyId);
                pDialog->m_nMode = g_objBLFManager.GetMode(blfInfo[nIndex].flag);
                pDialog->m_bSipPickUp = (blfInfo[nIndex].flag & SIP_BLF_AUTO_PICKUP_MASK != 0);
                sData.AddDialog(pDialog);

                if (!g_objBLFManager.ProcessStatusChange(&sData, nDsskeyId))
                {
                    g_objBLFManager.ReqBlfStatus(nDsskeyId);
                }
                else
                {
                    g_objBLFManager.CancelReqBlfStatus(nDsskeyId);
                }
            }
        }
        break;
    case SIP_SUB_UPDATE_BLF_STATUS_EXT:
        {
            void * pData = msg.GetExtraData();
            if (NULL == pData)
            {
                break ;
            }

            // 获取更新数量
            int nUpdateCount = msg.GetExtraSize() / sizeof(sip_dsskey_blf_info_t);
            const sip_dsskey_blf_info_t * blfInfo = (sip_dsskey_blf_info_t *)msg.GetExtraData();
#if IF_FEATURE_SHOW_BLF_CALLID
            bool bShowBLFCall = configParser_GetConfigInt(kszBLFShowCallId) != 0;
#endif
            YLVector<BLFBaseData> m_vecData;
            YLVector<int> m_vecDsskeyID;
            for (int nIndex = 0; nIndex < nUpdateCount; ++nIndex)
            {
                int nDsskeyId = dsskey_GetDsskeyByKeyIndex(blfInfo[nIndex].key.index);
                Dsskey_Status eStatus = MapSipBLFStatusToDsskeyStatus(blfInfo[nIndex].key.status);

                // 过滤状态不是Ringing
                if (DS_BLF_STATUS_RINGING != eStatus
#if IF_FEATURE_SHOW_BLF_CALLID
                        && !bShowBLFCall
#endif
                   )
                {
                    continue;
                }

                // 过滤账号不可用
                int iLineId = dsskey_GetDsskeyLine(nDsskeyId);
                if (!acc_IsAccountAvailable(iLineId))
                {
                    continue;
                }

                BLFBaseData sData;
                sData.m_bIsFullStatus = true;
                sData.m_strDisplayName = blfInfo[nIndex].local_name;
                sData.m_strKey = GetBLFkeyByDsskeyId(nDsskeyId);

                DialgInfo * pDialog = new DialgInfo;
                if (NULL == pDialog)
                {
                    continue;
                }

                // 新数据
                pDialog->m_eStatus = eStatus;
                pDialog->m_nMode = g_objBLFManager.GetMode(blfInfo[nIndex].key.flag);
                pDialog->m_strDisplayName = blfInfo[nIndex].remote_name;
                pDialog->m_strNumber = blfInfo[nIndex].remote_name;
                pDialog->m_bSipPickUp = (blfInfo[nIndex].key.flag & SIP_BLF_AUTO_PICKUP_MASK != 0);
                pDialog->m_sSipPickUpInfo = g_objBLFManager.SpliSipUri(blfInfo[nIndex].local_target_uri);
                pDialog->m_strDialgId = pDialog->m_strDisplayName;
#if IF_FEATURE_SHOW_BLF_CALLID
                if (bShowBLFCall)
                {
                    g_objBLFManager.GetDialogCallType(sData.m_strKey, pDialog);
                }
#endif

                APP_INFO("---------nUpdateSize[%d], IsFullStatus[%d],, m_strKey[%s], nDsskeyId[%d], strDialogId[%s]",
                         nUpdateCount, sData.m_bIsFullStatus, sData.m_strKey.c_str(), nDsskeyId,
                         pDialog->m_strDialgId.c_str());

                // 相同监控方整合
                int i = 0;
                for (; i < m_vecData.size(); ++i)
                {
                    // 监控方不同则过滤
                    if (sData.m_strKey != m_vecData[i].m_strKey)
                    {
                        continue;
                    }

                    // Dsskey多个监听相同账号的相同号码
                    if (m_vecData[i].IsMatch(pDialog))
                    {
                        delete pDialog;
                        pDialog = NULL;
                    }
                    else
                    {
                        m_vecData[i].AddDialog(pDialog);
                    }
                    break;
                }

                if (i == m_vecData.size())
                {
                    sData.AddDialog(pDialog);
                    m_vecData.push_back(sData);
                    m_vecDsskeyID.push_back(nDsskeyId);
                    sData.m_ListDialgInfo.clear();
                }

                g_objBLFManager.CancelReqBlfStatus(nDsskeyId);
            }

            // 执行更新, 一次刷新一个监控方
            for (int i = 0; i < m_vecData.size(); ++i)
            {
                g_objBLFManager.ProcessStatusChange(&m_vecData[i], m_vecDsskeyID[i]);
            }
        }
        break;
    case BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE:
        {
            if (!g_objBLFManager.ProcessConfigChange(msg.wParam))
            {
                if (!g_objBLFManager.IsDsskeySupport(msg.wParam))
                {
                    dsskey_SetDsskeyStatus(msg.wParam, DS_NON);
                    dsskey_refreshDSSKeyUI(msg.wParam);
                }
                else
                {
                    yl::string strKey = GetBLFkeyByDsskeyId(msg.wParam);

                    BLFBaseData * pData = g_objBLFManager.GetBLFDataBykey(strKey);

                    // http://10.2.1.199/Bug.php?BugID=89083
                    if (NULL != pData && pData->GetShowStatus() != dsskey_GetDsskeyStatus(msg.wParam))
                    {
                        dsskey_SetDsskeyStatus(msg.wParam, pData->GetShowStatus());
                        dsskey_refreshDSSKeyUI(msg.wParam);
                    }
                }
            }
        }
        break;
    case ACCOUNT_CONFIG_CHANGE:
    case ACCOUNT_STATUS_CHANGED:
        {
            // 可能Server发生改变
            g_objBLFManager.ProcessServerChange(msg.wParam);

            // 若账号变为非注册状态, 则清空该账号数据
            if (!acc_IsAccountAvailable(msg.wParam))
            {
                g_objBLFManager.ClearDialogInfoByAccId(msg.wParam);
            }
            // 通过查询使账号注册上, 重新请求缓存
            // 主要是解决网络恢复重新刷新的问题
            else if (NULL != msg.GetExtraData()
                     && *((bool *)msg.GetExtraData()))
            {
                g_objBLFManager.QueryBlfStatus(msg.wParam);
            }
        }
        break;
    case DSK_MSG_EXT_CHANGE:
        {
            g_objBLFManager.ProcessExpStatusChange(msg.lParam);
        }
        break;
    case TALK_MSG_TALK_START:
        {
            APP_INFO("blf: talk start\n");
            g_objBLFManager.TalkStart();
        }
        break;
    default:
        break;
    }

    return true;
}

void CBLFManager::ProcessTimer(int iTimerID)
{
    if (iTimerID != (UINT)&m_listReqDsskeyID)
    {
        return;
    }

    for (YLHashMap<int, int>::iterator it = m_listReqDsskeyID.begin();
            it != m_listReqDsskeyID.end();)
    {
        int iDsskeyID = it->first;

        //如果超过次数未获取到直接丢弃，避免死循环
        if (m_listReqDsskeyID[iDsskeyID] > BLF_REQSTATUS_COUNT)
        {
            it = m_listReqDsskeyID.erase(it);
            continue;
        }

        int iAccountID = dsskey_GetDsskeyLine(iDsskeyID);
        yl::string strKey = GetBLFkeyByDsskeyId(iDsskeyID);

        if (acc_IsAccountAvailable(iAccountID)
                && NULL != GetBLFDataBykey(strKey))
        {
            int iKeyIndex = dsskey_GetDsskeyIndexByID(iDsskeyID);
            //重新请求
            APP_INFO("reqblfstatus:[%d][%d] iKeyIndex[%d]", iAccountID, iDsskeyID, iKeyIndex);
            sip_blf_get_key_status(mkGetCurrent(), iAccountID, iKeyIndex);
            //删除
            it = m_listReqDsskeyID.erase(it);
        }
        else
        {
            m_listReqDsskeyID[iDsskeyID] ++;
            ++it;
        }
    }


    if (m_listReqDsskeyID.size() == 0)
    {
        timerKillThreadTimer(iTimerID);
    }
}

void CBLFManager::ReqBlfStatus(int iDsskeyID)
{
    if (!IsDsskeySupport(iDsskeyID))
    {
        return;
    }

    if (m_listReqDsskeyID.find(iDsskeyID) != m_listReqDsskeyID.end())
    {
        return;
    }

    APP_INFO("add reqblfstatus:[%d]", iDsskeyID);

    m_listReqDsskeyID[iDsskeyID] = 0;

    timerSetThreadTimer((UINT)&m_listReqDsskeyID, BLF_REQSTATUS_TIMER * 1000);
}

void CBLFManager::CancelReqBlfStatus(int iDsskeyID)
{
    YLHashMap<int, int>::iterator iter = m_listReqDsskeyID.find(iDsskeyID);
    if (iter != m_listReqDsskeyID.end())
    {
        APP_INFO("cancel reqblfstatus:[%d]", iDsskeyID);
        m_listReqDsskeyID.erase(iter);
        timerKillThreadTimer((UINT)&m_listReqDsskeyID);
        if (m_listReqDsskeyID.size() > 0)
        {
            timerSetThreadTimer((UINT)&m_listReqDsskeyID, BLF_REQSTATUS_TIMER * 1000);
        }
        return;
    }
}

bool CBLFManager::ProcessConfigChange(int nDsskeyId)
{
    APP_INFO("BLFConfig Change id[%d], type[%d]", nDsskeyId, dsskey_GetDsskeyType(nDsskeyId));

    yl::string strOldKey, strNewKey;

    // 先到缓存里面找
    if (m_mapKey.find(nDsskeyId) != m_mapKey.end())
    {
        strOldKey = m_mapKey[nDsskeyId].strKey;
    }

    // 新类型是BLF
    if (IsDsskeySupport(nDsskeyId))
    {
        strNewKey = GetBLFkeyByDsskeyId(nDsskeyId);
    }

    // 关键字没变化, 则不更新
    if (strOldKey == strNewKey)
    {
        return false;
    }

    // 旧数据先清除
    if (!strOldKey.empty())
    {
        RemoveIterByKey(strOldKey, m_mapKey[nDsskeyId].iLineId, nDsskeyId);
        m_mapKey.erase(nDsskeyId);
    }

    // 添加
    if (!strNewKey.empty())
    {
        CreatDataByDsskeyId(nDsskeyId);
    }

    return true;
}

// 账号的Server发生变化
void CBLFManager::ProcessServerChange(int nAccountId)
{
    // 获取所有的监控数据
    YLList<int> listDsskey;
    dsskey_getAllDsskeys(listDsskey);

    for (YLList<int>::ListIterator itr = listDsskey.begin(); itr != listDsskey.end(); ++itr)
    {
        int iDsskeyid = *itr;
        bool bDsskeySupport = IsDsskeySupport(iDsskeyid);
        if (!bDsskeySupport
                && m_mapKey.find(iDsskeyid) == m_mapKey.end())
        {
            continue;
        }

        if (dsskey_GetDsskeyLine(iDsskeyid) == nAccountId)
        {
            ProcessConfigChange(iDsskeyid);
            if (!bDsskeySupport)
            {
                dsskey_SetDsskeyStatus(iDsskeyid, DS_NON);
                dsskey_refreshDSSKeyUI(iDsskeyid);
            }
        }
    }
}

// EXP状态变化
bool CBLFManager::ProcessExpStatusChange(int iExpIndex)
{
    bool bStatusChange = true;
    YLList<int> listDsskey;
    dsskey_GetAllDsskeyDataByModule(listDsskey, exp_GetEXPModuleType(), iExpIndex);
    // 扩展台拔出, 清空数据
    DEVICE_TYPE_DEF eType = exp_GetExpTypeByIndex(iExpIndex);
    APP_INFO("ProcessExpStatusChange [%d] eType [%d]", iExpIndex, eType);
    if (DT_INVALID == eType)
    {
        // 拔出要取消延迟订阅的dsskey，快速插拔状态会出错
        for (YLList<int>::iterator it = listDsskey.begin(); it != listDsskey.end(); ++it)
        {
            CancelReqBlfStatus(*it);
        }

        g_objBLFManager.RemoveExtIter(iExpIndex);
    }
    else // 扩展台插上
    {
        // 加载Dsskey
        for (YLList<int>::iterator it = listDsskey.begin(); it != listDsskey.end(); ++it)
        {
            int iDsskeyID = *it;
            if (IsDsskeySupport(iDsskeyID)
                    && CreatDataByDsskeyId(iDsskeyID))
            {
                ReqBlfStatus(iDsskeyID);
                bStatusChange = true;
            }
        }
    }

    // 向SIP发送BLF配置改变消息
    if (bStatusChange)
    {
        // http://10.2.1.199/Bug.php?BugID=93146
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, 0);
    }

    return bStatusChange;
}

bool CBLFManager::CreatDataByDsskeyId(YLList<int> & lstDsskeyId)
{
    bool bCreate = false;
    for (YLList<int>::ListIterator it = lstDsskeyId.begin(); it != lstDsskeyId.end(); ++it)
    {
        int iDsskey = *it;

        if (IsDsskeySupport(iDsskey))
        {
            bCreate |= CreatDataByDsskeyId(iDsskey);
        }
    }
    return bCreate;
}

bool CBLFManager::CreatDataByDsskeyId(int nDsskeyId)
{
    // 缓存里面已存在, 不再处理
    if (m_mapKey.find(nDsskeyId) != m_mapKey.end())
    {
        return false;
    }

    yl::string strKey = GetBLFkeyByDsskeyId(nDsskeyId);
    if (strKey.empty())
    {
        return false;
    }

    //  非法值, 则不处理
    int iLineId = dsskey_GetDsskeyLine(nDsskeyId);
    if (iLineId < 0 || iLineId >= acc_AccountNum())
    {
        return false;
    }

    BLFBaseData * pData = GetBLFDataBykey(strKey);
    if (pData == NULL)
    {
        pData = new BLFBaseData();

        pData->m_strKey = GetBLFkeyByDsskeyId(nDsskeyId);
        pData->m_strNumber = dsskey_GetDsskeyValue(nDsskeyId);
        pData->m_strExten = dsskey_GetDsskeyExtension(nDsskeyId);

        APP_INFO("Creat BLF Dsskey key[%s]", pData->m_strKey.c_str());
    }

    pData->m_nBindDsskeyId.push_back(nDsskeyId);
    AddIter(pData, iLineId);

    m_mapKey[nDsskeyId].strKey = pData->m_strKey;
    m_mapKey[nDsskeyId].iLineId = iLineId;
    m_mapKey[nDsskeyId].iExtIndex = exp_IsEXPKey(nDsskeyId) ? dsskey_GetModuleIndex(nDsskeyId) : -1;

    FlushDsskey(pData, nDsskeyId);

    return true;
}

// 删除某个扩展台的数据
void CBLFManager::RemoveExtIter(int iExtIndex)
{
    for (YLHashMap<int, ItemData>::iterator itr = m_mapKey.begin(); itr != m_mapKey.end();)
    {
        if (itr->second.iExtIndex == iExtIndex)
        {
            RemoveIterByKey(itr->second.strKey, itr->second.iLineId, itr->first);
            itr = m_mapKey.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
}

bool CBLFManager::QueryBlfStatus(int iAccount)
{
    if (acc_IsAccountAvailable(iAccount))
    {
        sip_blf_get_key_status(mkGetCurrent(), iAccount, -1);
        return true;
    }
    return false;
}

bool CBLFManager::QueryBlfStatus()
{
    int nAccountCount = acc_AccountNum();
    for (int iAccount = 0; iAccount < nAccountCount; ++iAccount)
    {
        QueryBlfStatus(iAccount);
    }
    return true;
}

// wildix客户特制的亮灯模式
// 该值是通过XML下发, SIP解析, 动态变化的
// 目前先做一层转换, 提供给UI使用
int CBLFManager::GetMode(int flag)
{
    bool bMode0 = (flag & SIP_BLF_WILDIX_LED_MODE0_MASK != 0);
    bool bMode1 = (flag & SIP_BLF_WILDIX_LED_MODE1_MASK != 0);

    if (!bMode0 && !bMode1)
    {
        return 0;
    }
    else if (!bMode0 && bMode1)
    {
        return 2;
    }
    else if (bMode0 && !bMode1)
    {
        return 1;
    }

    return 3;
}

yl::string CBLFManager::SpliSipUri(const yl::string & strInfo)
{
    yl::string strUri = strInfo;

    if (strInfo.substr(0, 4) == "sip:")
    {
        strUri = strInfo.substr(4, strInfo.size() - 4);
    }

    yl::string::size_type nPos = strUri.find(':');
    if (nPos != yl::string::npos)
    {
        strUri = strUri.substr(0, nPos);
    }

    return strUri;
}

bool CBLFManager::IsDsskeySupport(int iDsskeyId)
{
    DssKey_Type eType = dsskey_GetDsskeyType(iDsskeyId);

    return (DT_BLF == eType || DT_CALLPARK == eType
            || (DT_INTERCOM == eType && configParser_GetConfigInt(kszIntercomLedEnable) != 0)
#ifdef IF_FEATURE_GENBAND
            || DT_BLFGROUP == eType
#endif
           );
}

void CBLFManager::ProcessTalkFoucsChange()
{
    g_objPickupManager.BlfPlayVoice();
}

void CBLFManager::TalkStart()
{
    g_objPickupManager.BlfPlayVoice();
}
