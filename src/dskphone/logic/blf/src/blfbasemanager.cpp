#include "blfbasemanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "blf_inc.h"
#include "callpickupmanager.h"
#include "blfpraseaction.h"
#include "edk/include/modedk.h"
#include "customdatamanager.h"
#if IF_FEATURE_SHOW_BLF_CALLID
#include "adapterbox/include/modadapterbox.h"
#endif

CBLFBaseManager::CBLFBaseManager()
{
    for (int i = 0; i < acc_AccountNum(); i++)
    {
        ListMonitor listMonitor;

        m_vBlfAccoutData.push_back(listMonitor);
    }

    m_eManagerType = BLF_Base_Manager;
}

CBLFBaseManager::~CBLFBaseManager()
{
    for (int i = 0; i < acc_AccountNum(); i++)
    {
        ClearAllData(i);
    }
}

bool CBLFBaseManager::ClearAllData(int nAccount)
{
    APP_INFO("CBLFBaseManager::ClearAllData account[%d]", nAccount);
    if (m_vBlfAccoutData.size() <= nAccount)
    {
        return false;
    }

    ListMonitor & refList = m_vBlfAccoutData[nAccount];

    if (refList.size() == 0)
    {
        return false;
    }

    for (ListMonitor::ListIterator iter = refList.begin(); iter != refList.end(); iter++)
    {
        yl::string & strKey = *iter;

        RemoveIterByKey(strKey, nAccount, -1, false);
    }

    refList.clear();

    return true;
}

// 清除某个账号所有监控方的状态
void CBLFBaseManager::ClearDialogInfoByAccId(int nAccount)
{
    if (m_vBlfAccoutData.size() <= nAccount)
    {
        return ;
    }

    ListMonitor & refList = m_vBlfAccoutData[nAccount];

    for (ListMonitor::ListIterator iter = refList.begin(); iter != refList.end(); ++iter)
    {
        yl::string & strKey = *iter;

        BLFBaseData * pData = GetBLFDataBykey(strKey);

        if (NULL == pData)
        {
            continue;
        }

        int iIndex = 0;
        for (; iIndex < pData->m_nBindDsskeyId.size(); ++iIndex)
        {
            int iLineId = dsskey_GetDsskeyLine(pData->m_nBindDsskeyId[iIndex]);

            if (acc_IsAccountAvailable(iLineId))
            {
                break;
            }
        }

        // 所有账号都不可用时, 清除数据
        if (iIndex == pData->m_nBindDsskeyId.size()
                && pData->m_ListDialgInfo.size() > 0)
        {
            Dsskey_Status eStatus = pData->GetShowStatus();

            pData->ClearDialogList();

            // 设置PickUP
            if (DS_BLF_STATUS_RINGING == eStatus)
            {
                g_objPickupManager.UpdateCallPickup(pData->m_strKey, m_eManagerType, Opt_Remove);
            }

            FlushDsskey(pData);
        }
    }

    if (configParser_GetConfigInt(kszCallParkBlfAudioSwitch) == 1)
    {
        g_objPickupManager.ClearCallParkList(nAccount);
    }
}

void CBLFBaseManager::RemoveIterByKey(const yl::string & strKey, int nAccount,
                                      int nDsskeyId /* = -1 */, bool bListRemove /* = true */)
{
    APP_INFO("CBLFBaseManager::RemoveIterByKey strKey[%s]", strKey.c_str());
    BLFDataMap::iterator iter = m_mapData.find(strKey);

    bool bRemoveData = true;

    if (iter != m_mapData.end())
    {
        BLFBaseData * pData = iter->second;
        if (pData != NULL)
        {
            if (nDsskeyId != -1)
            {
                pData->UbindDsskeyByDsskeyId(nDsskeyId);
            }
            else
            {
                pData->UbindDsskeyByLine(nAccount);
            }

            // 设置PickUP
            if (pData->GetShowStatus() == DS_BLF_STATUS_RINGING)
            {
                g_objPickupManager.UpdateCallPickup(pData->m_strKey, m_eManagerType, Opt_Remove);
            }
            else if (configParser_GetConfigInt(kszCallParkBlfAudioSwitch) == 1
                     && pData->GetShowStatus() == DS_BLF_STATUS_TALKING)
            {
                g_objPickupManager.ClearCallParkList(-1, nDsskeyId, pData->m_strKey);
            }

            bRemoveData = (pData->m_nBindDsskeyId.size() == 0);

            if (bRemoveData)
            {
                delete pData;
            }
        }
        if (bRemoveData)
        {
            m_mapData.erase(strKey);
        }
    }

    if (bListRemove && m_vBlfAccoutData.size() > nAccount)
    {
        for (YLList<yl::string>::ListIterator iter =  m_vBlfAccoutData[nAccount].begin();
                iter !=  m_vBlfAccoutData[nAccount].end(); iter++)
        {
            if (strKey.compare(*iter) == 0)
            {
                m_vBlfAccoutData[nAccount].erase(iter);
                break;
            }
        }
    }
}

bool CBLFBaseManager::AddIter(BLFBaseData * pData, int nAccount)
{
    if (pData->m_strKey.empty())
    {
        return false;
    }

    AddKeyToList(pData->m_strKey, nAccount);

    if (m_mapData.find(pData->m_strKey) == m_mapData.end())
    {
        m_mapData.put(pData->m_strKey, pData);
        return true;
    }

    return false;
}

void CBLFBaseManager::AddKeyToList(const yl::string & strKey, int nAccount)
{
    if (m_vBlfAccoutData.size() > nAccount)
    {
        ListMonitor & refList = m_vBlfAccoutData[nAccount];

        refList.push_back(strKey);
    }
    else
    {
        DSSKEY_ERR("CBLFBaseManager::AddIter error");
    }
}

void CBLFBaseManager::UpdateDialog(BLFBaseData * pData, BLFBaseData * pOldData)
{
    if (pData->m_bIsFullStatus)
    {
        pOldData->ClearDialogList();

        pOldData->m_ListDialgInfo = pData->m_ListDialgInfo;

        pData->m_ListDialgInfo.clear();
        return;
    }

    for (YLList<DialgInfo *>::ListIterator iter = pData->m_ListDialgInfo.begin();
            iter != pData->m_ListDialgInfo.end(); iter++)
    {
        DialgInfo * pNewDialog = *iter;

        bool bFound = false;

        for (YLList<DialgInfo *>::ListIterator it = pOldData->m_ListDialgInfo.begin();
                it != pOldData->m_ListDialgInfo.end();)
        {
            DialgInfo * pOldDialog = *it;

            APP_INFO("newid[%s],oldId[%s], isMatch[%d]\n", pNewDialog->m_strDialgId.c_str(),
                     pOldDialog->m_strDialgId.c_str(), pOldDialog->IsMatch(pNewDialog));

            if (pOldDialog->IsMatch(pNewDialog))
            {
                bFound = true;
                *pOldDialog = *pNewDialog;

                if (pOldDialog->m_eStatus == DS_BLF_STATUS_IDLE && pOldData->m_ListDialgInfo.size() != 1)
                {
                    delete pOldDialog;

                    it = pOldData->m_ListDialgInfo.erase(it);
                    continue;
                }
            }

            it++;

            if (bFound)
            {
                break;
            }
        }

        if (!bFound)
        {
            if (pNewDialog->m_eStatus != DS_BLF_STATUS_IDLE || pOldData->m_ListDialgInfo.size() == 0)
            {
                pOldData->AddDialog(pNewDialog);

                *iter = NULL;
            }
            else
            {
                APP_WARN("CBLFBaseManager::UpdateIterDialog id[%s]", pNewDialog->m_strDialgId.c_str());
            }
        }
    }


    Sort(pOldData->m_ListDialgInfo, DialgInfo::Comp);
}

bool CBLFBaseManager::ProcessStatusChange(BLFBaseData * pData, int nDsskeyID/* = -1*/)
{
    BLFBaseData * pOldData = GetBLFDataBykey(pData->m_strKey);

    if (pOldData == NULL)
    {
        return false;
    }
    Dsskey_Status oldStatus = pOldData->GetShowStatus();

    Dsskey_Status eDsskeyOldStatus = oldStatus;

    //http://bugfree.yealink.com/Bug.php?BugID=104072
    if (nDsskeyID != -1)
    {
        eDsskeyOldStatus = dsskey_GetDsskeyStatus(nDsskeyID);
    }

    // 将新数据更新到旧数据中
    UpdateDialog(pData, pOldData);

    Dsskey_Status newStatus = pOldData->GetShowStatus();


    APP_INFO("url[%s] old status[%d], newStatus[%d], dsskey[%d] OldStatus[%d] nSize[%d]", pOldData->m_strKey.c_str(), oldStatus,
             newStatus, nDsskeyID, eDsskeyOldStatus, pOldData->m_ListDialgInfo.size());

    //设置PickUP
    if (newStatus == DS_BLF_STATUS_RINGING)
    {
        g_objPickupManager.UpdateCallPickup(pData->m_strKey, m_eManagerType, Opt_Add);
    }
    else if (oldStatus == DS_BLF_STATUS_RINGING)
    {
        g_objPickupManager.UpdateCallPickup(pData->m_strKey, m_eManagerType, Opt_Remove);
    }

    if (configParser_GetConfigInt(kszCallParkBlfAudioSwitch) == 1
            && dsskey_GetDsskeyType(nDsskeyID) == DT_CALLPARK)
    {
        //设置call park 通话状态改变响铃
        if (newStatus == DS_BLF_STATUS_TALKING)
        {
            g_objPickupManager.UpdateCallPark(pData->m_strKey, nDsskeyID, Opt_Add);
        }
        else
        {
            g_objPickupManager.UpdateCallPark(pData->m_strKey, nDsskeyID, Opt_Remove);
        }
    }

    bool bChange = false;
    if (pOldData->m_strDisplayName != pData->m_strDisplayName
            || pOldData->m_strExten != pData->m_strExten)
    {
        bChange = true;
    }

#if IF_FEATURE_SHOW_BLF_CALLID
    // 可能存在多个状态,最终显示的状态不变,但是实际有其中的一个状态是改变的,这时需要刷新界面
    if (configParser_GetConfigInt(kszBLFShowCallId) != 0
            && AdapterBox_IsExistBLFMenu(pOldData->m_strKey))
    {
        bChange = true;
    }
#endif

    if (pOldData->m_strDisplayName.empty()
            || pOldData->m_strDisplayName != pData->m_strDisplayName)
    {
        pOldData->m_strDisplayName = pData->m_strDisplayName;
    }

    if (!pData->m_strExten.empty())
    {
        pOldData->m_strExten = pData->m_strExten;
    }

    // 这里应该要比较对应dsskey id 的状态
    if (oldStatus != newStatus
            || eDsskeyOldStatus != newStatus
            || bChange)
    {
        FlushDsskey(pOldData);
    }

    return true;
}

#if IF_BUG_32812
bool CBLFBaseManager::GetBLFAutoSetDataBykey(const int iDssKeyID,
        autoset_dsskeydata_t & tAutoSetDssKeyData)
{
    yl::string strKey = GetBLFkeyByDsskeyId(iDssKeyID);
    BLFBaseData * pData = GetBLFDataBykey(strKey);
    if (pData != NULL)
    {
        tAutoSetDssKeyData << *pData;

        yl::string strLabel = dsskey_GetDsskeyLabel(iDssKeyID);
        if (!strLabel.empty())
        {
            tAutoSetDssKeyData.strLabel = strLabel;
        }

        DSSKEY_INFO("GetBLFAutoSetDataBykey[%d] strKey[%s] strValue[%s] strLabel[%s] iStatus[%d]",
                    iDssKeyID, strKey.c_str(), pData->m_strNumber.c_str(), tAutoSetDssKeyData.strLabel.c_str(),
                    tAutoSetDssKeyData.iStatus);
        return true;
    }
    DSSKEY_INFO("GetBLFAutoSetDataBykey[%d] can no find strKey[%s]", iDssKeyID, strKey.c_str());

    return false;
}
#endif

BLFBaseData * CBLFBaseManager::GetBLFDataBykey(const yl::string & strKey)
{
    BLFDataMap::iterator iter = m_mapData.find(strKey);

    if (iter == m_mapData.end())
    {
        return NULL;
    }

    return iter->second;
}

void CBLFBaseManager::FlushDsskey(BLFBaseData * pData, int nDsskeyId)
{
    if (NULL == pData)
    {
        return ;
    }

    if (m_eManagerType == BLFLIST_Manager)
    {
#if IF_BUG_32812
        if (configParser_GetConfigInt(kszAutoBlfListEnable) != 0
                || configParser_GetConfigInt(kszBLFListCustomSet) != 1)
#endif
        {
            dsskey_SetDsskeyLabel(nDsskeyId,
                                  pData->m_strDisplayName.empty() ? pData->m_strNumber : pData->m_strDisplayName);
            dsskey_SetDsskeyValue(nDsskeyId, pData->m_strNumber);
            dsskey_SetDsskeyExtension(nDsskeyId, pData->m_strExten);
        }
    }

    Dsskey_Status eStatus = pData->GetShowStatus();

    // 更新Dsskey状态
    dsskey_SetDsskeyStatus(nDsskeyId, eStatus);

    APP_INFO("BLFLIST FlushDsskey DsskeyId[%d] Number[%s],Status[%d]", nDsskeyId,
             pData->m_strNumber.c_str(), pData->GetShowStatus());

    m_listFlushDsskeyID.push_back(nDsskeyId);

    SetFlushDsskeyUITimer();
}

void CBLFBaseManager::FlushDsskey(BLFBaseData * pData)
{
    if (NULL == pData)
    {
        return ;
    }

    for (int i = 0; i < pData->m_nBindDsskeyId.size(); i++)
    {
        int nDsskeyId = pData->m_nBindDsskeyId[i];

        FlushDsskey(pData, nDsskeyId);
    }
}

bool CBLFBaseManager::SetFlushDsskeyUITimer()
{
    if (!m_FlushDsskeyTimer.IsTimerRuning())
    {
        m_FlushDsskeyTimer.SetTimer(200, NULL, MK_MSG_ENTRY(this, CBLFBaseManager::OnBLFBaseTimer));
        return true;
    }
    return false;
}

BOOL CBLFBaseManager::OnBLFBaseTimer(msgObject & refObj)
{
    APP_INFO("---------CBLFBaseManager::OnBLFBaseTimer--------------");

    // 真正刷新的时候才需要去亮
    if (m_listFlushDsskeyID.size() > 0)
    {
        // 点亮背光灯,重启节电
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), BLF_MESSAGE_FLUSH, 0, 0);
    }

    m_FlushDsskeyTimer.KillTimer();
    CBLFBaseManager::FlushDsskeyUI(m_listFlushDsskeyID);
    m_listFlushDsskeyID.clear();
    return TRUE;
}

void CBLFBaseManager::FlushDsskeyUI(YLList<int> & lstFlushDsskey)
{
    APP_INFO("CBLFBaseManager::FlushDsskeyUI()");
    for (YLList<int>::ListIterator iter = lstFlushDsskey.begin(); iter != lstFlushDsskey.end(); iter++)
    {
        FlushDsskeyUI(*iter);
    }
}

bool CBLFBaseManager::FlushDsskeyUI(const int iDsskeyId, DsskeyExpRefresh dsskeyRefresh/* = DSS_KEY_REFRESH_ALL*/)
{
    Dsskey_Status eStatus = dsskey_GetDsskeyStatus(iDsskeyId);
    yl::string strCusmLedInfo = g_objBLFCustomDataManager.GetLedInfo(eStatus);

    bool bHandle = false;

    // 有自定义, 则交由EDK处理
    if (!strCusmLedInfo.empty())
    {
        bHandle = EDK_DoAction(strCusmLedInfo, iDsskeyId, BLF_TRIGGER);
    }

    if (!bHandle)
    {
        // 从自定义变为非自定义时执行灭灯
        EdkCtrlDsskeyDispData objDispData;
        if (EDK_GetEdkCtrlDsskeyDispData(iDsskeyId, objDispData))
        {
            EDK_DoAction("$LEDo$", iDsskeyId, BLF_TRIGGER);
        }

        dsskey_refreshDSSKeyUI(iDsskeyId, dsskeyRefresh, false);
#if IF_FEATURE_SHOW_BLF_CALLID
        if (configParser_GetConfigInt(kszBLFShowCallId) != 0)
        {
            yl::string strKey = GetBLFkeyByDsskeyId(iDsskeyId);
            if (AdapterBox_IsExistBLFMenu(strKey))
            {
                if (eStatus == DS_BLF_STATUS_IDLE
                        || eStatus == DS_BLF_STATUS_UNKOWN)
                {
                    AdapterBox_ExitBLFMenu();
                }
                else
                {
                    int nDsskeyId = iDsskeyId;
                    AdapterBox_ShowBLFMenu(&nDsskeyId);
                }
            }
        }
#endif
    }
    return true;
}

void CBLFBaseManager::GetPickUpShowContentByKey(YLList<yl::string> & listMenuData,
        YLVector<int> & listDsskeyID, const yl::string & strKey)
{

    BLFBaseData * pData = GetBLFDataBykey(strKey);

    if (NULL == pData)
    {
        return ;
    }

    for (YLList<DialgInfo *>::ListIterator iter = pData->m_ListDialgInfo.begin();
            iter != pData->m_ListDialgInfo.end(); iter++)
    {
        DialgInfo * pDialog = *iter;
        if (NULL == pDialog)
        {
            continue;
        }

        if (pDialog->m_eStatus != DS_BLF_STATUS_RINGING)
        {
            return;
        }

        yl::string strLocal = pData->GetDisplayName();

        yl::string strName;
        Dir_GetCotactNameByNumber(pDialog->m_strNumber, strName);
        if (strName.empty())
        {
            strName = pDialog->m_strDisplayName;
        }
        yl::string strRemote = GetPickUpDisplayName(strName, pDialog->m_strNumber);

        yl::string strContent;
        strContent = commonAPI_FormatString("%s <- %s", strLocal.c_str(), strRemote.c_str());

        // 多个Dsskey监控, 提示多路, 账号可能不同
        // BLF可能配置两个Dsskey监听相同账号的相同号码, 用户场景并不会如此配置, 暂不处理
        for (int iIndex = 0; iIndex < pData->m_nBindDsskeyId.size(); ++iIndex)
        {
            listMenuData.push_back(strContent);
            listDsskeyID.push_back(pData->m_nBindDsskeyId[iIndex]);
        }
    }
}

void CBLFBaseManager::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    APP_INFO("CBLFBaseManager::OnTaskActionCallback");

    CBLFPraseAction * pAction = static_cast<CBLFPraseAction *>(pTaskAction);

    if (pAction == NULL)
    {
        return;
    }

    if (!pAction->IsOperationSucceed())
    {
        APP_WARN("Prase Fail!");
        pAction->ClearListInfo();
        return;
    }

    ProcessTaskActionCallBack(pAction);
}

bool CBLFBaseManager::GetDialogInfoByDsskeyId(DialgInfo & refData, int nDsskeyId,
        int index /* = 0 */)
{
    BLFBaseData * pData = GetBLFDataBykey(GetBLFkeyByDsskeyId(nDsskeyId));

    if (pData == NULL)
    {
        return false;
    }

    DialgInfo * pDialog = pData->GetDialogInfoByIndex(index);

    if (pDialog != NULL)
    {
        refData = *pDialog;
    }

    return true;
}

void CBLFBaseManager::FlushALLDsskeyLight()
{
    BLFDataMap::iterator iter = m_mapData.begin();

    for (; iter != m_mapData.end(); iter++)
    {
        BLFBaseData * pData = iter->second;

        if (pData != NULL)
        {
            FlushDsskey(pData);
        }
    }
}
yl::string CBLFBaseManager::GetPickUpDisplayName(const yl::string & strName,
        const yl::string & strNumber)
{
    yl::string strDisplayName = "";
    int nDisplayMethod = configParser_GetConfigInt(kszPickupDisplayMethod);
    switch ((eBLFDisplayMethod)nDisplayMethod)
    {
    case BLF_DM_NUM:
        {
            if (!strNumber.empty())
            {
                strDisplayName = strNumber;
            }
            else
            {
                strDisplayName = strName;
            }
        }
        break;
    case BLF_DM_NAME_AND_NUMBER:
        {
            if (strName.empty()
                    || strName == strNumber)
            {
                strDisplayName = strNumber;
            }
            else
            {
                strDisplayName = strName;
                if (!strNumber.empty())
                {
                    strDisplayName.append(" ");
                    strDisplayName.append(strNumber);
                }
            }
        }
        break;
    case BLF_DM_NUMBER_AND_NAME:
        {
            if (strNumber.empty()
                    || strName == strNumber)
            {
                strDisplayName = strName;
            }
            else
            {
                strDisplayName = strNumber;
                if (!strName.empty())
                {
                    strDisplayName.append(" ");
                    strDisplayName.append(strName);
                }
            }
        }
        break;
    default:
        {
            if (!strName.empty())
            {
                strDisplayName = strName;
            }
            else
            {
                strDisplayName = strNumber;
            }
        }
        break;
    }

    return strDisplayName;
}

#if IF_FEATURE_SHOW_BLF_CALLID
bool CBLFBaseManager::GetMenuList(int nDsskeyId, YLList<yl::string>& listMenu, YLList<int>& listStatus)
{
    yl::string strKey = GetBLFkeyByDsskeyId(nDsskeyId);
    if (strKey.empty())
    {
        return false;
    }

    BLFBaseData* pData = GetBLFDataBykey(strKey);
    if (pData == NULL)
    {
        return false;
    }

    for (YLList<DialgInfo*>::ListIterator iter = pData->m_ListDialgInfo.begin(); iter != pData->m_ListDialgInfo.end(); iter++)
    {
        DialgInfo* pDialog = *iter;
        if (NULL == pDialog)
        {
            continue;
        }

        if (pDialog->m_eStatus == DS_BLF_STATUS_IDLE
                || pDialog->m_eStatus == DS_BLF_STATUS_UNKOWN)
        {
            continue;
        }

        yl::string strLocal = pData->GetDisplayName();
        yl::string strRemote;
        Dir_GetCotactNameByNumber(pDialog->m_strNumber, strRemote);
        if (strRemote.empty())
        {
            strRemote = pDialog->m_strDisplayName;
        }

        yl::string strContent;
        if (pDialog->m_bCallIn)
        {
            strContent = commonAPI_FormatString("%s <- %s", strLocal.c_str(), strRemote.c_str());
        }
        else
        {
            strContent = commonAPI_FormatString("%s <- %s", strRemote.c_str(), strLocal.c_str());
        }

        listMenu.push_back(strContent);
        listStatus.push_back(pDialog->m_eStatus);
    }

    return (listMenu.size() > 0 && listMenu.size() == listStatus.size());
}

bool CBLFBaseManager::ShowBLFMenu(int nDsskeyId)
{
    yl::string strKey = GetBLFkeyByDsskeyId(nDsskeyId);
    BLFBaseData* pData = GetBLFDataBykey(strKey);
    if (pData == NULL)
    {
        return false;
    }

    if (pData->GetShowStatus() == DS_BLF_STATUS_IDLE
            || pData->GetShowStatus() == DS_BLF_STATUS_UNKOWN)
    {
        return false;
    }

    return AdapterBox_ShowBLFMenu(&nDsskeyId);
}

bool CBLFBaseManager::GetDialogCallType(const yl::string& strKey, DialgInfo* pDialog)
{
    if (pDialog == NULL)
    {
        return false;
    }

    if (pDialog->m_eStatus == DS_BLF_STATUS_CALLOUT)
    {
        pDialog->m_bCallIn = false;
        return true;
    }
    else if (pDialog->m_eStatus == DS_BLF_STATUS_RINGING)
    {
        pDialog->m_bCallIn = true;
        return true;
    }

    BLFBaseData* pData = GetBLFDataBykey(strKey);
    if (pData == NULL)
    {
        return false;
    }

    for (YLList<DialgInfo*>::ListIterator iter = pData->m_ListDialgInfo.begin(); iter != pData->m_ListDialgInfo.end(); iter++)
    {
        DialgInfo* pTempDialog = *iter;
        if (NULL == pTempDialog)
        {
            continue;
        }

        if (pTempDialog->IsMatch(pDialog))
        {
            pDialog->m_bCallIn = pTempDialog->m_bCallIn;
            return true;
        }
    }

    return false;
}
#endif
