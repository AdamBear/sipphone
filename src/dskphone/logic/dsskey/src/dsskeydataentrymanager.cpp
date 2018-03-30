#include "dsskeydataentrymanager.h"
#include <ETLLib.hpp>
#include <ylvector.h>
#include <ylalgorithm.h>
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#include <account/include/accountmessagedefine.h>
#include <account/include/modaccount.h>

#include "dsskeydata.h"
#include "dsskeyaction.h"
#include "dsskeydatamodule.h"
#include "dsskeycommonfunc.h"
#include "dsskeyactionmanager.h"
#include "../include/dsskeymsgdefine.h"
#include "../include/moddsskey.h"
#include "../include/dsskey_def.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "wireless/wifi/include/modwifi.h"
#include "hotplug/modhotplug.h"
#include "record/include/modrecord.h"
#include "blf/include/modblf.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/broadsoftsca/include/scamessagedefine.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "hotplug/modhotplug.h"
#include "autoset/include/modautoset.h"
#include <configserver/configserverinc.h>

#include "exp/include/modexp.h"
//获取BLF后5为的值，Idle,Ring,Talk时增加2种情况的处理，
#define GetBLFStatusNoMode(i)   ((i) & 0x1f)
#define GetBLFMode(i)           ((i) & 0x60)



CDSSKeyDataEntryManager::CDSSKeyDataEntryManager(void)
//:m_mapDataModules(false, 10, 100)
{
#if IF_FEATRUE_PTT
    m_listLongPressDsskeyID.clear();
#endif
}

CDSSKeyDataEntryManager & CDSSKeyDataEntryManager::GetInstance()
{
    static CDSSKeyDataEntryManager s_Instance;
    return s_Instance;
}

CDSSKeyDataEntryManager::~CDSSKeyDataEntryManager(void)
{
    clear();
}

/*
bool CDSSKeyDataEntryManager::SetDSSKeyAction( int dsskeyID, DssKey_Type keyType )
{
    CDSSkeyData *pDsskeyData = GetDsskeyDataByID(dsskeyID);
    if (pDsskeyData)
    {
        return pDsskeyData->SetDsskeyAction(keyType);
    }
    return false;
}*/

CDSSKeyDataModule * CDSSKeyDataEntryManager::GetModuleByDsskeyID(int dsskeyID)
{
    int moduleID = GetModuleID(dsskeyID);
    if (moduleID != -1)
    {
        return GetModuleByModuleID(moduleID);
    }
    else
    {
        return NULL;
    }
}

CDSSKeyDataModule * CDSSKeyDataEntryManager::GetDssKeyModule(DSSKEY_MODULE_TYPE eType,
        int iEXPIndex/* = -1*/)
{
    int nModuleID = GetModuleIDByModuleIndex(eType, iEXPIndex);
    if (nModuleID < 0)
    {
        return NULL;
    }
    else
    {
        return GetModuleByModuleID(nModuleID);
    }
}

MapModuleItem CDSSKeyDataEntryManager::FindModule(int iModuleID)
{
    MapModuleItem it = m_mapDataModules.begin();

    for (; it != m_mapDataModules.end(); ++it)
    {
        if ((*it)->first == iModuleID)
        {
            break;
        }
    }

    return it;
}

CDSSKeyDataModule * CDSSKeyDataEntryManager::GetModuleByModuleID(int moduleID)
{
    MapModuleItem  iter = FindModule(moduleID);
    if (iter != m_mapDataModules.end())
    {
        return (*iter)->second;
    }
    else
    {
        return NULL;
    }
}

bool CDSSKeyDataEntryManager::InitModules()
{
    ResetModuleDsskeyData(DMT_LINEKEY_MODULE);
    // V80机型中不存在memkey
    ResetModuleDsskeyData(DMT_MEMKEY_MODULE);
    ResetModuleDsskeyData(DMT_PROGRAMEKEY_MODULE);
#ifdef _T49
    ResetModuleDsskeyData(DMT_NOTIFYKEY_MODULE);
#endif

    for (int i = 0; i < exp_GetExpCounts(DT_ALL); i++)
    {
        ResetModuleDsskeyData((DSSKEY_MODULE_TYPE)exp_GetExpTypeByIndex(i), i);
    }
    return true;
}

void CDSSKeyDataEntryManager::ResetModuleDsskeyData(DSSKEY_MODULE_TYPE eType, int iEXPIndex /*= 0*/)
{
    if (!IsSupportModuleType(eType))
    {
        return;
    }

    int iPrimaryKey = GetModuleIDByModuleIndex(eType, iEXPIndex);
    if (iPrimaryKey < 0)
    {
        return;
    }
    CDSSKeyDataModule * pModule = GetModuleByModuleID(iPrimaryKey);
    if (pModule == NULL)
    {
        DSSKEY_INFO("alloc new CDSSKeyDataModule type[%d] index[%d]", eType, iEXPIndex);
        pModule = new CDSSKeyDataModule(eType, iEXPIndex);
        if (pModule)
        {
            DsskeyModuleInfo * pInfo = new DsskeyModuleInfo;
            pInfo->first = iPrimaryKey;
            pInfo->second = pModule;

            m_mapDataModules.push_back(pInfo);
        }
    }

    if (pModule != NULL
            && pModule->LoadFromMemory())
    {
        yl::string strConfigFile;
        if (pModule->GetConfigFile(strConfigFile))
        {
            SaveDsskeyInfoToFile(strConfigFile, DSS_FILE_TYPE_XML, iPrimaryKey);
        }
    }
}

void CDSSKeyDataEntryManager::RemoveModule(DSSKEY_MODULE_TYPE eType, int iEXPIndex /*= 0*/)
{
    int iPrimaryKey = GetModuleIDByModuleIndex(eType, iEXPIndex);
    if (iPrimaryKey < 0)
    {
        return;
    }
    MapModuleItem  iter = FindModule(iPrimaryKey);
    if (iter != m_mapDataModules.end())
    {
        DsskeyModuleInfo * pModule = *iter;
        if (NULL == pModule)
        {
            return;
        }

        DSSKEY_INFO("free CDSSKeyDataModule type[%d] index[%d]", eType, iEXPIndex);

        if ((*iter)->second)
        {
            delete (*iter)->second;
        }

        delete pModule;
        pModule = NULL;

        m_mapDataModules.erase(iter);
    }
}

CDSSkeyData * CDSSKeyDataEntryManager::GetDsskeyInfoByKeyIndex(int iKeyIndex)
{
    MapModuleItem it = m_mapDataModules.begin();
    for (; it != m_mapDataModules.end() && (*it)->second != NULL; ++it)
    {
        if (iKeyIndex > (*it)->second->size())
        {
            iKeyIndex -= (*it)->second->size();
        }
        else if (iKeyIndex < 0)
        {
            return NULL;
        }
        else
        {
            return (*it)->second->GetDsskeyDataByIndex(iKeyIndex);
        }
    }

    return NULL;
}

//获取Dsskey总数
int CDSSKeyDataEntryManager::GetAllDsskeyNum()
{
    int nNum = 0;
    MapModuleItem it = m_mapDataModules.begin();
    for (; it != m_mapDataModules.end() && (*it)->second != NULL; ++it)
    {
        nNum += (*it)->second->size();
    }

    return nNum;
}

int CDSSKeyDataEntryManager::GetModuleDsskeyNum(DSSKEY_MODULE_TYPE eType, int iEXPIndex/* = 0*/)
{
    int moduleID = GetModuleIDByModuleIndex(eType, iEXPIndex);
    if (moduleID < 0)
    {
        return 0;
    }

    MapModuleItem  iter = FindModule(moduleID);
    if (iter != m_mapDataModules.end())
    {
        return (*iter)->second->size();
    }
    return 0;
}

// 对方状态更新的消息
void CDSSKeyDataEntryManager::ProcessRemoteStatus(msgObject & msg)
{
    void * pData = msg.GetExtraData();
    if (pData == NULL)
    {
        return;
    }

    sip_dsskey_info_t * pDsskeyInfo = (sip_dsskey_info_t *)(msg.GetExtraData());
    int iIndex = 0;
    while (iIndex < msg.lParam)
    {
        if (pDsskeyInfo[iIndex].index >= MAX_DSSANDEXP_KEYS_NUM)
        {
            iIndex++;
            continue;
        }

        m_cRemoteStatusArray[pDsskeyInfo[iIndex].index] = Comm_MapSipBLFStatus(pDsskeyInfo[iIndex].status);
        iIndex++;
    }
}

// 处理定时器消息
bool CDSSKeyDataEntryManager::OnTimer(UINT uTimerID)
{
    // 定时器是否被处理
#if 0
    bool bHandled = _EXPHelper.OnTimer(uTimerID);
#else
    bool bHandled = FALSE;
#endif
    if (bHandled)
    {
        return bHandled;
    }

    // 定时器消息交给Module来处理
    for (MapModuleItem iter = m_mapDataModules.begin();
            iter != m_mapDataModules.end(); ++iter)
    {
        (*iter)->second->OnTimer(uTimerID);
    }

    bHandled = m_mapDataModules.size() != 0;

    return bHandled;
}

void CDSSKeyDataEntryManager::UpdateRemoteStatusByModule(CDSSKeyDataModule * pModule,
        char * lpStatus, int iNum)
{
    if (NULL == pModule
            || NULL == lpStatus)
    {
        return;
    }

    for (int i = 0; i < iNum; ++i)
    {
        CDSSkeyData * pDsskey = pModule->GetDsskeyDataByIndex(i);
        if (!pDsskey)
        {
            continue;
        }

        DssKey_Type eType = pDsskey->GetDsskeyType();
        if (eType != DT_BLF
#ifdef IF_FEATURE_GENBAND
                && eType != DT_BLFGROUP
#endif
           )
        {
            // BLF List通过UpDateBlfListRemoteStatus来更新按键状态
            continue;
        }

        pDsskey->UpdateRemoteStatus(lpStatus[i]);
    }
}

// 配置改变消息处理
bool CDSSKeyDataEntryManager::OnConfigChange(msgObject & msg)
{
    DSSKEY_INFO("CDssKeyManager::OnConfigChange msg.wParam=%d lParam=%d", msg.wParam, msg.lParam);

    bool bHandled = true;
    switch (msg.wParam)
    {
    case  ST_PROGRAMABLE_KEY:
        {
            // 更新某些按键
            if (msg.GetExtraData() != NULL)
            {
                DsskeyChangeInfo * pChangeInfo = (DsskeyChangeInfo *) msg.GetExtraData();

                //重置
                if (0 == pChangeInfo->iLength)
                {
                    dsskey_ResetModuleDataToFactory(DMT_PROGRAMEKEY_MODULE);

                    //通知配置程序，清除配置
                    if (INI_FILE_PROCESS == GET_CONFIG_PROCESS_TYPE())
                    {
                        msgPostMsgToThreadEx(msgGetThreadByName(CONFIG_SERVER_THREAD_NAME), CSM_DSSKEY_CHANGE, 3, 0, sizeof(DsskeyChangeInfo), pChangeInfo);
                    }
                    else if (CFG_SERVER_PROCESS == GET_CONFIG_PROCESS_TYPE())
                    {
                        //TODO. 增加cfgserver 的消息发送
                    }
                }
            }

            ResetModuleDsskeyData(DMT_PROGRAMEKEY_MODULE);

            YLList<int> programekey;
            dsskey_GetAllDsskeyDataByModule(programekey, DMT_PROGRAMEKEY_MODULE);
            YLList<int>::ListIterator iter = programekey.begin();
            for (; iter != programekey.end(); iter++)
            {
                DssKeyLogicData data;
                dsskey_GetData(*iter, data);
                dsskey_refreshDSSKeyUI(*iter);
            }
            etl_NotifyApp(TRUE, DSSKEY_CONF_CHANGE, msg.wParam, msg.lParam);
        }
        break;
    case ST_VPPHONE:
        {
            // 更新某些按键
            if (msg.GetExtraData() == NULL)
            {
                DSSKEY_WARN("pKeys == NULL");
                return false;
            }


            DsskeyChangeInfo * pChangeInfo = (DsskeyChangeInfo *)msg.GetExtraData();

            UpdateKeysInfo(DMT_LINEKEY_MODULE, pChangeInfo->szKey, 0);

            etl_NotifyApp(false, BLA_MESSAGE_DSSKEY_CHANGED, msg.wParam, msg.lParam);
        }
        break;
    case ST_PAGE_TIP:
        {
            // 先Unbound

        }
        break;
    case ST_EXT:
        {
            // 更新某些按键
            if (msg.GetExtraData() == NULL)
            {
                DSSKEY_WARN("pKeys == NULL");
                return false;
            }

            DsskeyChangeInfo * pChangeInfo = (DsskeyChangeInfo *) msg.GetExtraData();

            UpdateKeysInfo((DSSKEY_MODULE_TYPE)exp_GetExpTypeByIndex(msg.lParam), pChangeInfo->szKey,
                           msg.lParam);

            etl_NotifyApp(false, BLA_MESSAGE_DSSKEY_CHANGED, msg.wParam, msg.lParam);

            // 根据新的配置重新设置FWD和DND灯状态
            //dsskey_UpdateDNDStatus(dnd_IsDNDOn());
            //dsskey_UpdateFWDStatus(fwd_IsForwardOn());
        }
        break;
    case ST_BLACKLIST:
        break;
    case ST_ACD:
        {
            //SetAutoACDAvailTimer();
        }
        break;
#ifdef _T49
    case ST_AUTOANSWER:
        {
            //AutoAnswer
            UpdateStatus(DT_AUTO_ANSWER, acc_IsAccountAutoAnswerEnable() ? DS_AUTOANSWER_USING : DS_NON);
        }
        break;
#endif
    case ST_SMS:
        {
            // 刷新 SMS 类型的DSSKEY
            dsskey_RefressDsskeyByType(DT_SMS);

            // 刷新 ProgramKey
            YLList<int> programekey;
            dsskey_GetAllDsskeyDataByModule(programekey, DMT_PROGRAMEKEY_MODULE);
            YLList<int>::ListIterator iter = programekey.begin();
            for (; iter != programekey.end(); iter++)
            {
                dsskey_refreshDSSKeyUI(*iter);
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

// 更新Dsskey灯
void CDSSKeyDataEntryManager::RefreshDsskeyStatusByType(DssKey_Type eType)
{
    for (MapModuleItem iter = m_mapDataModules.begin();
            iter != m_mapDataModules.end(); ++iter)
    {
        YLVector<CDSSkeyData *> vecDsskey;
        (*iter)->second->GetDsskeyListByType(vecDsskey, eType);

        for (int i = 0; i < vecDsskey.size(); i++)
        {
            vecDsskey[i]->UpdateStatus();
        }
    }
}

typedef YLVector<DssKey_Type> TypeVec;
//添加eDKType监听的类型到vecTypes中
bool AppendNotifyType(TypeVec & vecTypes, DssKey_Type eDKType)
{
    typedef CDSSKeyAction::DsskeyTypeList TypeList;
    CDSSKeyAction * pAction = _DSSKeyActionManger.GetDSSKeyAction(eDKType);
    if (pAction)
    {
        TypeList listNewTypes = pAction->GetListenTypes();
        for (TypeList::iterator iterPos = listNewTypes.begin(); iterPos != listNewTypes.end(); ++iterPos)
        {
            vecTypes.push_back(*iterPos);
        }
        return true;
    }
    return false;
}

// 更新按键的信息,iEXPIndex:第几个扩展台
void CDSSKeyDataEntryManager::UpdateKeysInfo(DSSKEY_MODULE_TYPE eType, char * pKeysCode,
        int iEXPIndex/* = -1*/)
{
    CDSSKeyDataModule * pModule = GetDssKeyModule(eType, iEXPIndex);
    if (NULL == pModule)
    {
        return ;
    }

    DSSKEY_INFO("UpdateKeysInfo Start");

    YLList<int> lstDsskeyIndex;
    YLVector<DssKeyLogicData> vecOldData;

    // 得到旧数据
    int iKeyNum = strlen(pKeysCode);
    for (int iKeyIndex = 0; iKeyIndex < iKeyNum; ++iKeyIndex)
    {
        // pKeysCode中存储的按键序号都是从1开始
        int dsskeyIndex = pKeysCode[iKeyIndex] - 1;
        DSSKEY_INFO("CDSSKeyDataEntryManager::UpdateKeysInfo[%d]", dsskeyIndex);
        CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(dsskeyIndex);
        if (NULL != pDsskeyData)
        {
            DssKeyLogicData oldLogicData;
            pDsskeyData->GetLogicData(oldLogicData);
            vecOldData.push_back(oldLogicData);
            FreeNativeData(pDsskeyData->GetDsskeyDataID());
            lstDsskeyIndex.push_back(dsskeyIndex);
        }
    }

    // 加载数据, 更新内存值
    pModule->ReloadDssKeyAttrib(lstDsskeyIndex);
    pModule->SetDirty(true);
    // http://10.2.1.199/Bug.php?BugID=86022
    // 解决方案：UI尽量提前此消息，WEB尽量提后查询BLFList；改变它们到达主线程消息队列的顺序
    if (vecOldData.size() == 0)
    {
        return;
    }

    dsskey_change_info_t * arrChangeDsskey = new dsskey_change_info_t [vecOldData.size()];
    for (int i = 0; i < vecOldData.size(); ++i)
    {
        dsskey_change_info_t & tChangeDssKeyInfo = arrChangeDsskey[i];
        tChangeDssKeyInfo.iDsskeyID = vecOldData[i].iDssKeyID;
        tChangeDssKeyInfo.iOldLineID = vecOldData[i].iLineIndex;
        tChangeDssKeyInfo.eOldType = vecOldData[i].eDKType;
        tChangeDssKeyInfo.eNewType = dsskey_GetDsskeyType(tChangeDssKeyInfo.iDsskeyID);
    }
    DSSKEY_INFO("UpdateKeysInfo Reload Send!");
    etl_NotifyAppEx(FALSE, DSSKEY_RELOAD_FINISHED, 0, 0, sizeof(dsskey_change_info_t)*iKeyNum,
                    arrChangeDsskey);

    // 以前有此变量的用途是：
    // 用来区分是网页修改还是液晶修改
    // 网页导致的配置更改或类型更改, 不再去发送消息
    static int iChangeID = 0;

    // 通知配置变更，刷新Dsskey
    YLList<int>::ListIterator itr = lstDsskeyIndex.begin();
    for (int j = 0; itr != lstDsskeyIndex.end(); ++j, ++itr)
    {
        CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(*itr);
        if (NULL != pDsskeyData)
        {
            if (vecOldData[j].eDKType != pDsskeyData->GetDsskeyType())
            {
                pDsskeyData->ClearStatus();
                pDsskeyData->OnTypeModify(vecOldData[j].eDKType, pDsskeyData->GetDsskeyType(), &iChangeID);
            }

            DssKeyLogicData newLogicData;
            pDsskeyData->GetLogicData(newLogicData);
            if (!vecOldData[j].isDataEqual(newLogicData))
            {
                pDsskeyData->OnConfigChange(&iChangeID);
            }

            dsskey_refreshDSSKeyUI(pDsskeyData->GetDsskeyDataID());
        }
    }

    // 发一个同步消息出去，edk需要把灯命令清除掉
    etl_NotifyAppEx(TRUE, DSSKEY_CONF_CHANGE, 0, 0, sizeof(dsskey_change_info_t)*iKeyNum,
                    arrChangeDsskey);
    delete [] arrChangeDsskey;
}

#ifdef IF_SUPPORT_BLUETOOTH
//Bluetooth 状态改变消息
bool CDSSKeyDataEntryManager::OnBluetoothStateChange(msgObject & msg)
{
    if (msg.message != BLUETOOTH_MSG_CONNECT_STATE
            && msg.message != BLUETOOTH_MSG_STATE_CHANGE)
    {
        return false;
    }

    bool bHandle = false;
    for (MapModuleItem iter = m_mapDataModules.begin();
            iter != m_mapDataModules.end(); ++iter)
    {
        bHandle |= (*iter)->second->OnBluetoothStateChange(msg);
    }

    return bHandle;
}
#endif

#ifdef _T49
//Notify Dsskey状态改变
bool CDSSKeyDataEntryManager::OnNotifyDsskeyStateChange(msgObject & msg)
{
    CDSSKeyDataModule * pModule = GetDssKeyModule(DMT_NOTIFYKEY_MODULE);
    if (NULL == pModule)
    {
        return false;
    }

    if ((msg.message == TALK_MSG_SESSION_FOUCS_CHANGE
            || msg.message == TALK_MSG_TALK_START)
            && ProcessNotifyDsskeyInTalking(msg))
    {
        return true;
    }

    return pModule->OnNotifyDsskeyStateChange(msg);
}

Dsskey_Status CDSSKeyDataEntryManager::GetCameraDSStatus(int iStatus)
{
    if (iStatus == CST_CONNECTED)
    {
        return DS_NOTIFY_CONNECT;
    }
    else if (iStatus == CST_READY)
    {
        return DS_NOTIFY_USING;
    }
    else
    {
        return DS_NOTIFY_DISABLE;
    }
}

Dsskey_Status CDSSKeyDataEntryManager::GetExtDisplayStatus()
{
#ifdef IF_SUPPORT_HDMI
    if (!modhotplug_IsHdmiConnected()
            || !modhotplug_IsHdmiEnable())
    {
        return DS_NOTIFY_DISABLE;
    }

    Dsskey_Status eStatus = modhotplug_GetHdmiSwitch() ? DS_NOTIFY_USING : DS_NOTIFY_CONNECT;

    DSSKEY_INFO("CDSSKeyDataEntryManager::GetExtDisplayStatus[%d]", eStatus);

    return eStatus;
#else
    return DS_NOTIFY_USING;
#endif //#ifdef IF_SUPPORT_HDMI
}

//处理通话中通知中心dsskey状态变化
bool CDSSKeyDataEntryManager::ProcessNotifyDsskeyInTalking(msgObject & objMsg)
{
    if (objMsg.message != TALK_MSG_SESSION_FOUCS_CHANGE
            && objMsg.message != TALK_MSG_TALK_START)
    {
        return false;
    }

    if (talklogic_SessionExist())
    {
        UpdateStatus(DT_WIFI, DS_NOTIFY_DISABLE);
        UpdateStatus(DT_USB, DS_NOTIFY_DISABLE);
        UpdateStatus(DT_BLUETOOTH, DS_NOTIFY_DISABLE);
        UpdateStatus(DT_AUTO_ANSWER, DS_AUTOANSWER_DISABLE);
        UpdateStatus(DT_FWD, DS_FWD_DISABLE);

        //camera 需要判断是否存在视频通话
        if (!talklogic_IsVideoSessionExist())
        {
            UpdateStatus(DT_CAMERA, DS_NOTIFY_DISABLE);
            return true;
        }
    }
    else
    {
        UpdateStatus(DT_BLUETOOTH, Bluetooth_IsEnable() ? DS_NOTIFY_CONNECT : DS_NOTIFY_DISCONNECT);
        UpdateStatus(DT_WIFI, WIFI_IsEnable() ? DS_NOTIFY_CONNECT : DS_NOTIFY_DISCONNECT);
        UpdateStatus(DT_USB, modRecord_IsStorageConnect() ? DS_NOTIFY_CONNECT : DS_NOTIFY_DISABLE);
        UpdateStatus(DT_AUTO_ANSWER, acc_IsAccountAutoAnswerEnable() ? DS_AUTOANSWER_USING : DS_NON);
        UpdateStatus(DT_FWD, fwd_HasAvaliableAccountForwardOn() ? DS_FWD_USING : DS_NON);
        UpdateStatus(DT_DND, dnd_HasAvailableAccountDNDOn() ? DS_DND_USING : DS_NON);
    }

    UpdateStatus(DT_CAMERA, GetCameraDSStatus(modhotplug_GetCameraStatus()));
    return true;
}
#endif
// line状态改变消息处理
bool CDSSKeyDataEntryManager::OnLineStateChange(msgObject & msg)
{
    //DSSKEY_INFO(" msg.wParam=%d msg.lParam=%d", msg.wParam, msg.lParam);
    if (msg.message != ACCOUNT_STATUS_CHANGED)
    {
        //若非对应消息，则不予处理
        return false;
    }

    int nAccountID = (int)msg.wParam;

    LINE_STATE eState = acc_GetState(nAccountID);

    if (LS_SYSTEM_DEFAULT_ACCOUNT == eState
            && LS_REGISTERED == msg.lParam
            || LS_SYSTEM_DEFAULT_ACCOUNT == msg.lParam
            && LS_REGISTERED == eState)
    {
        // 只是默认账号改变不处理
        return false;
    }

#ifdef _T49
    UpdateStatus(DT_AUTO_ANSWER, acc_IsAccountAutoAnswerEnable() ? DS_AUTOANSWER_USING : DS_NON);
#endif

    if (!acc_IsAccountAvailable(nAccountID))
    {
        // 清空BLFList状态
        //_BLFListProcessor.ResetBLFListByLineID(nAccountID);
        // 清空ACD状态
        // FlashACDStatus(-1, AS_IDLE, msg.wParam);
        // 账号未注册上时重置监控对方的状态
        ResetRemoteStatusByAccount(nAccountID);
    }

    for (MapModuleItem iter = m_mapDataModules.begin();
            iter != m_mapDataModules.end(); ++iter)
    {
        (*iter)->second->OnLineStateChange(msg);
    }

    return true;
}

// 账号状态异常时重置bla状态
void CDSSKeyDataEntryManager::ResetRemoteStatusByAccount(int iLineId)
{
    if (iLineId < 0
            || iLineId >= acc_AccountNum())
    {
        return;
    }

    CDSSkeyData    *    pDsskey = NULL;
    CDSSKeyDataModule * pModule = NULL;

    for (MapModuleItem iter = m_mapDataModules.begin();
            iter != m_mapDataModules.end(); ++iter)
    {
        pModule = (*iter)->second;
        if (NULL == pModule)
        {
            continue;
        }

        for (int i = 0; i < pModule->size(); i++)
        {
            pDsskey = pModule->GetDsskeyDataByIndex(i);
            if (pDsskey == NULL)
            {
                continue;
            }

            if (!pDsskey->IsStatusCtrlByRemote())
            {
                continue;
            }

            int iAccountID = pDsskey->GetRealLine();
            if (iAccountID != iLineId)
            {
                continue;
            }

            DssKey_Type eType = pDsskey->GetDsskeyType();
            if (eType == DT_BLA)
            {
                m_cRemoteStatusArray[i] = CN_BLA_STATUS_UNKOWN;
                pDsskey->UpdateRemoteStatus(m_cRemoteStatusArray[i]);
            }
        }
    }//end for iter
}

#ifdef _T49
//查询通知中心dsskey状态
void CDSSKeyDataEntryManager::QueryNotifyModuleStatus()
{
    //Bluetooth
    UpdateStatus(DT_BLUETOOTH, Bluetooth_IsEnable() ? DS_NOTIFY_CONNECT : DS_NOTIFY_DISCONNECT);

    //WIFI
    UpdateStatus(DT_WIFI, WIFI_IsEnable() ? DS_NOTIFY_CONNECT : DS_NOTIFY_DISCONNECT);

    //USB
    UpdateStatus(DT_USB, modRecord_IsStorageConnect() ? DS_NOTIFY_CONNECT : DS_NOTIFY_DISABLE);

    //Camera
    UpdateStatus(DT_CAMERA, GetCameraDSStatus(modhotplug_GetCameraStatus()));

    //HDMI
    UpdateStatus(DT_EXT_DISPLAY, GetExtDisplayStatus());

    //AutoAnswer
    UpdateStatus(DT_AUTO_ANSWER, acc_IsAccountAutoAnswerEnable() ? DS_AUTOANSWER_USING : DS_NON);

    //DND&&FWD 只刷新通知中心DND/FWD dsskey的状态
    UpdateStatus(DT_DND, dnd_HasAvailableAccountDNDOn() ? DS_DND_USING : DS_NON);
    UpdateStatus(DT_FWD, fwd_HasAvaliableAccountForwardOn() ? DS_FWD_USING : DS_NON);
}
#endif

// 获取某类型的Dsskey列表
void CDSSKeyDataEntryManager::GetDsskeyListByType(YLVector<CDSSkeyData *> & vecDsskey,
        DssKey_Type eType)
{
    vecDsskey.clear();
    MapModuleItem it = m_mapDataModules.begin();
    for (; it != m_mapDataModules.end(); it++)
    {
        (*it)->second->GetDsskeyListByType(vecDsskey, eType);
    }
}

CDSSkeyData * CDSSKeyDataEntryManager::GetDsskeyDataByID(int dsskeyID)
{
    CDSSKeyDataModule * pModule = GetModuleByDsskeyID(dsskeyID);
    if (pModule)
    {
        int keyIndex = GetDsskeyIndex(dsskeyID);
        return pModule->GetDsskeyDataByIndex(keyIndex);
    }
    return NULL;
}

// 初始化
void CDSSKeyDataEntryManager::Init()
{
    //DSSKEY_INFO("CDssKeyManager::Init()");

    // 从配置文件中读取各Module按键信息
    InitModules();

#ifdef _T49
    QueryNotifyModuleStatus();
#endif



    //向所有线程广播配置文件更改的消息
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_VPPHONE, 0);
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, 0);
}

bool CDSSKeyDataEntryManager::GetDataForUI(int iDssKeyID, DssKeyLogicData & sData)
{
    CDSSkeyData * pDssKey = GetDsskeyDataByID(iDssKeyID);
    if (NULL != pDssKey)
    {
        pDssKey->GetLogicData(sData);
        return true;
    }
    return false;
}

bool CDSSKeyDataEntryManager::GetDataForUI(DSSKEY_MODULE_TYPE eType, int iBeginKey, int iNum,
        YLList<DssKeyLogicData> & listData, int iEXPIndex /*= -1*/)
{
    CDSSKeyDataModule * pModule = GetDssKeyModule(eType, iEXPIndex);

    if (NULL == pModule)
    {
        return false;
    }

    for (int i = 0; i < iNum; ++i)
    {
        DssKeyLogicData sData;
        CDSSkeyData * pDssKey = pModule->GetDsskeyDataByIndex(iBeginKey + i);
        if (NULL != pDssKey)
        {
            pDssKey->GetLogicData(sData);
            listData.push_back(sData);
        }
    }

    return true;
}

// 更新DSSkey状态
bool CDSSKeyDataEntryManager::UpdateStatus(DssKey_Type eType, Dsskey_Status eStatus,
    const yl::string& strValue/* = ""*/)
{
    YLVector<CDSSkeyData *> vecDsskey;
    GetDsskeyListByType(vecDsskey, eType);
    for (int i = 0; i < vecDsskey.size(); i ++)
    {
        UpdateStatus(vecDsskey[i], eStatus, strValue);
    }
    return true;
}

bool CDSSKeyDataEntryManager::UpdateStatus(CDSSkeyData * pData, Dsskey_Status eStatus,
        const yl::string& strValue)
{
    if (pData
            //http://10.3.5.199/Bug.php?BugID=61768 不刷新programkey的状态，节省刷屏时间
            && pData->getDsskeyModuleType() != DMT_PROGRAMEKEY_MODULE)
    {
        if (!strValue.empty())
        {
            yl::string strDsskeyValue;
            pData->GetValue(strDsskeyValue);
            if (strValue != strDsskeyValue)
            {
                return false;
            }
        }
        pData->SetStatus(eStatus);
        int iDsskeyID = pData->GetDsskeyDataID();
        if (exp_IsEXPKey(iDsskeyID))
        {
            if (exp_IsExpKeyInCurrentPage(iDsskeyID))
            {
                dsskey_refreshDSSKeyUI(iDsskeyID);
            }
        }
        else
        {
            dsskey_refreshDSSKeyUI(iDsskeyID);
        }
    }
    return true;
}

// 根据Fwd/Dnd的改变，更新linekey的显示
void CDSSKeyDataEntryManager::UpdateLinekeyLabel(int iAccount/* = -1*/,
        DSSKEY_MODULE_TYPE eType/* = DMT_LINEKEY_MODULE*/, int iModuleIndex/* = 0*/)
{
    CDSSKeyDataModule * pLineKeyModule = GetDssKeyModule(eType, iModuleIndex);
    if (NULL == pLineKeyModule)
    {
        return;
    }

    pLineKeyModule->UpdateLinekeyLabel(iAccount);
}

VOID CDSSKeyDataEntryManager::ProcessCallParkNotify(int iAccountID, bool bOn)
{
    MapModuleItem itr = m_mapDataModules.begin();
    MapModuleItem itrEnd = m_mapDataModules.end();

    for (; itr != itrEnd; ++itr)
    {
        CDSSKeyDataModule * pModule = (*itr)->second;
        if (NULL == pModule)
        {
            //DSSKEY_ERR("Module is null!");
            continue;
        }

        int iKeyNum = pModule->size();
        for (int i = 0; i < iKeyNum; ++i)
        {
            CDSSkeyData * pDsskey = pModule->GetDsskeyDataByIndex(i);
            if (pDsskey->GetDsskeyType() != DT_LINE)
            {
                continue;
            }

            if (pDsskey->GetLineIndex() == iAccountID)
            {
                if (bOn)
                {
                    pDsskey->SetLineStatus(DS_SCA_CALLPARK, DS_SCA_CALLPARK_FIRST, DS_SCA_CALLPARK_END);
                }
                else
                {
                    // 关闭SCACallPARK状态
                    pDsskey->RemoveLineStatus(DS_SCA_CALLPARK_FIRST, DS_SCA_CALLPARK_END);
                }
            }
        }
    }
}

int CDSSKeyDataEntryManager::GetModulesSize()
{
    return m_mapDataModules.size();
}

void CDSSKeyDataEntryManager::GetModulesList(YLList<int> & listOutput)
{
    listOutput.clear();
    for (MapModuleItem iter = m_mapDataModules.begin(); iter != m_mapDataModules.end(); iter++)
    {
        listOutput.push_back((*iter)->first);
    }
}

void CDSSKeyDataEntryManager::clear()
{
    for (MapModuleItem iter = m_mapDataModules.begin(); iter != m_mapDataModules.end(); iter++)
    {
        DsskeyModuleInfo * pInfo = *iter;
        if (NULL == pInfo)
        {
            continue;
        }
        if ((*iter)->second)
        {
            delete (*iter)->second;
        }

        delete pInfo;
        pInfo = NULL;
    }
    m_mapDataModules.clear();
}

bool CDSSKeyDataEntryManager::GetAllDsskeys(YLList<int> & listOutput,
        DssKey_Type eType/* = DT_NONE*/, bool bNative/* = false*/)
{
    for (MapModuleItem iter = m_mapDataModules.begin(); iter != m_mapDataModules.end(); iter++)
    {
        CDSSKeyDataModule * pModule = (*iter)->second;
        if (pModule == NULL)
        {
            continue;
        }

        for (int i = 0; i < pModule->size(); i ++)
        {
            CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(i);
            if (pDsskeyData == NULL)
            {
                continue;
            }

            int iDsskeyID = pDsskeyData->GetDsskeyDataID();

            // DT_NONE 类型的话将所有dsskey均附上
            if (DT_NONE != eType)
            {
                DssKey_Type eNativeType = pDsskeyData->GetDsskeyType();

                if (bNative)
                {
                    eNativeType = dsskey_GetDsskeyType(iDsskeyID, true);
                }

                if (eNativeType != eType)
                {
                    continue;
                }
            }

            listOutput.push_back(iDsskeyID);
        }
    }

    return listOutput.size() > 0;
}

bool CDSSKeyDataEntryManager::GetAllDsskeyByType(YLList<int> & listOutput, DSSKEY_MODULE_TYPE eType)
{
    for (MapModuleItem iter = m_mapDataModules.begin(); iter != m_mapDataModules.end(); iter++)
    {
        CDSSKeyDataModule * pModule = (*iter)->second;
        if (pModule && pModule->GetModuleType() == eType)
        {
            for (int i = 0; i < pModule->size(); i ++)
            {
                CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(i);
                if (pDsskeyData)
                {
                    listOutput.push_back(pDsskeyData->GetDsskeyDataID());
                }
            }
        }
    }

    return listOutput.size() > 0;
}

void CDSSKeyDataEntryManager::GetAllDsskeyByAccID(YLList<int> & listOutput, int accID)
{
    for (MapModuleItem iter = m_mapDataModules.begin(); iter != m_mapDataModules.end(); iter++)
    {
        CDSSKeyDataModule * pModule = (*iter)->second;
        if (pModule)
        {
            for (int i = 0; i < pModule->size(); i ++)
            {
                CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(i);
                if (pDsskeyData && pDsskeyData->GetDsskeyType() == DT_LINE && pDsskeyData->GetRealLine() == accID)
                {
                    listOutput.push_back(pDsskeyData->GetDsskeyDataID());
                }
            }
        }
    }
}

int CDSSKeyDataEntryManager::CountsDsskeyByAccID(int iAccountID, bool bNative/* = false*/)
{
    int iCounts = 0;
    for (MapModuleItem iter = m_mapDataModules.begin(); iter != m_mapDataModules.end(); iter++)
    {
        CDSSKeyDataModule * pModule = (*iter)->second;
        if (pModule)
        {
            iCounts += pModule->CountsDsskeyByAccID(iAccountID, bNative);
        }
    }
    return iCounts;
}

bool CDSSKeyDataEntryManager::GetBLAIndexByDsskey(int & iIndex, int iDsskeyID, int iAccountID)
{
    for (MapModuleItem iter = m_mapDataModules.begin(); iter != m_mapDataModules.end(); iter++)
    {
        CDSSKeyDataModule * pModule = (*iter)->second;
        if (pModule)
        {
            if (pModule->GetBLAIndexByDsskey(iIndex, iDsskeyID, iAccountID))
            {
                return true;
            }
        }
    }
    return false;
}

int CDSSKeyDataEntryManager::GetDsskeyByBLAIndex(int iIndex, int iAccountID)
{
    if (iIndex <= 0)
    {
        return -1;
    }

    YLList<int> listDsskey;
    GetAllDsskeyByAccID(listDsskey, iAccountID);
    if (listDsskey.size() <= 0)
    {
        return -1;
    }

    iIndex = iIndex % listDsskey.size();
    if (iIndex == 0)
    {
        iIndex = listDsskey.size();
    }
    YLList<int>::ListIterator iter = listDsskey.begin();
    for (int i = 1; iter != listDsskey.end(); iter++, i++)
    {
        if (i == iIndex)
        {
            DSSKEY_INFO("GetDsskeyByBLAIndex BlaIndex[%d], DsskeyID[%d]", iIndex, *iter);
            return (*iter);
        }
    }

    return -1;
}

void CDSSKeyDataEntryManager::BeforeRemoveModule(DSSKEY_MODULE_TYPE eType, int iEXPIndex /*= 0*/)
{
    int iPrimaryKey = GetModuleIDByModuleIndex(eType, iEXPIndex);
    if (iPrimaryKey < 0)
    {
        return;
    }
    MapModuleItem  iter = FindModule(iPrimaryKey);
    if (iter != m_mapDataModules.end())
    {
        CDSSKeyDataModule * pModule = (*iter)->second;
        if (NULL != pModule)
        {
            if (pModule->size() == 0)
            {
                return;
            }

            int * arrDsskeyId = new int [pModule->size()];
            for (int i = 0; i < pModule->size(); ++i)
            {
                CDSSkeyData * pDsskeyData = pModule->GetDsskeyDataByIndex(i);
                if (NULL != pDsskeyData)
                {
                    arrDsskeyId[i] = pDsskeyData->GetDsskeyDataID();
                    FreeNativeData(arrDsskeyId[i]);
                }
            }

            // 把该模块的dsskeyid发出去
            etl_NotifyAppEx(false, DSSKEY_MODULE_REMOVE, eType, iEXPIndex, sizeof(int)*pModule->size(),
                            arrDsskeyId);
            delete [] arrDsskeyId;
        }
    }
}

#if IF_BUG_31595
void CDSSKeyDataEntryManager::ShowXMLBrowserInfo(yl::string strXMLinfo)
{

    int iDsskeyNum = _DsskeyManager.GetAllDsskeyNum();

    for (int i = 0; i < iDsskeyNum; i++)
    {
        CDSSkeyData * pData = GetDsskeyInfoByKeyIndex(i);
        if (NULL == pData)
        {
            continue;
        }

        if (DT_XMLBROWSER_INFO != pData->GetDsskeyType())
        {
            continue;
        }

        size_t uPosFound = strXMLinfo.find('\n');
        yl::string strLabel = "";
        int iDsskeyID = -1;

        if (exp_IsEXPKey(dsskey_GetDsskeyByKeyIndex(i)))
        {
            iDsskeyID = dsskey_GetDsskeyByKeyIndex(i);
        }
        else
        {
            iDsskeyID = i;
        }

        if (uPosFound != strXMLinfo.npos)
        {
            strLabel = strXMLinfo.substr(0, uPosFound);
            strXMLinfo = strXMLinfo.substr(uPosFound + 1);

            dsskey_SetDsskeyLabel(iDsskeyID, strLabel);
            dsskey_refreshDSSKeyUI(iDsskeyID);

            if (strXMLinfo.empty())
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
}

#endif

#if IF_FEATRUE_PTT
bool CDSSKeyDataEntryManager::IsDsskeyInLongPress(int iDsskeyID)
{
    YLList<int>::ListIterator iter = m_listLongPressDsskeyID.begin();
    for (; iter != m_listLongPressDsskeyID.end(); iter++)
    {
        if (*iter == iDsskeyID)
        {
            return true;
        }
    }
    return false;
}

bool CDSSKeyDataEntryManager::AddLongPress(int iDsskeyID)
{
    YLList<int>::ListIterator iter = m_listLongPressDsskeyID.begin();
    for (; iter != m_listLongPressDsskeyID.end(); iter++)
    {
        if (*iter == iDsskeyID)
        {
            return true;
        }
    }
    m_listLongPressDsskeyID.push_back(iDsskeyID);
    return true;
}

bool CDSSKeyDataEntryManager::RemoveLongPress(int iDsskeyID)
{
    YLList<int>::ListIterator iter = m_listLongPressDsskeyID.begin();
    for (; iter != m_listLongPressDsskeyID.end(); iter++)
    {
        if (*iter == iDsskeyID)
        {
            m_listLongPressDsskeyID.erase(iter);
            return true;
        }
    }
    return false;
}

bool CDSSKeyDataEntryManager::IsExistLongPress()
{
    if (m_listLongPressDsskeyID.size() > 0)
    {
        return true;
    }
    return false;
}
#endif

// 保存Dsskey信息到指定文件中
bool CDSSKeyDataEntryManager::SaveDsskeyInfoToFile(const yl::string & strFile, int iFileType,
        int nModuleID/* = INVALID_DSSKEY_ID*/)
{
    bool bFull = nModuleID == INVALID_DSSKEY_ID;

    if (iFileType == DSS_FILE_TYPE_M7)
    {
        chFileStdio doc(strFile.c_str(), chFileStdio::modeCreateWrite);
        if (!doc.IsFileOpened())
        {
            return false;
        }

        doc.WriteString("#!version:1.0.0.1\n");

        for (MapModuleItem it = m_mapDataModules.begin(); it != m_mapDataModules.end(); it++)
        {
            DsskeyModuleInfo * pModule = *it;
            if (pModule == NULL || pModule->second == NULL)
            {
                continue;
            }

            if (bFull || (nModuleID >= 0 && pModule->first == nModuleID))
            {
                pModule->second->SaveDsskeyInfoToFile(doc);
            }
        }

        doc.Close();
        return true;
    }
    else if (iFileType == DSS_FILE_TYPE_XML)
    {
        using namespace pugi;
        xml_document doc;

        for (MapModuleItem it = m_mapDataModules.begin(); it != m_mapDataModules.end(); it++)
        {
            DsskeyModuleInfo * pModule = *it;
            if (pModule == NULL || pModule->second == NULL)
            {
                continue;
            }

            if (bFull || (nModuleID >= 0 && pModule->first == nModuleID))
            {
                pModule->second->SaveDsskeyInfoToFile(doc);
            }
        }

        return doc.save_file(strFile.c_str());
    }
    else if (iFileType == DSS_FILE_TYPE_INI)
    {
        iniFile doc;

        for (MapModuleItem it = m_mapDataModules.begin(); it != m_mapDataModules.end(); it++)
        {
            DsskeyModuleInfo * pModule = *it;
            if (pModule == NULL || pModule->second == NULL)
            {
                continue;
            }

            if (bFull || (nModuleID >= 0 && pModule->first == nModuleID))
            {
                pModule->second->SaveDsskeyInfoToFile(doc);
            }
        }

        return doc.SaveToFile(strFile.c_str());
    }

    return false;
}

bool CDSSKeyDataEntryManager::ReloadModifyDSSKeyData(DssKey_Type eType/* = DT_NONE*/)
{
    int iSize = m_listModify.size();
    if (iSize == 0)
    {
        return false;
    }

    DSSKEY_INFO("ReloadModifyDSSKeyData key[%d] size[%d]", eType, iSize);
    ++iSize;
    char * pKeysCode = new char[iSize];

    /* 转化为对UpdateKeysInfo的调用，构造其参数 */
    for (MapModuleItem it = m_mapDataModules.begin(); it != m_mapDataModules.end(); it++)
    {
        DsskeyModuleInfo * pModuleInfo = *it;
        if (pModuleInfo == NULL || pModuleInfo->second == NULL)
        {
            continue;
        }

        int iIndex = 0;
        int iModuleInfo = pModuleInfo->first;
        memset(pKeysCode, 0, sizeof(char)*iSize);

        YLList<int>::iterator iter = m_listModify.begin();
        for (; iter != m_listModify.end(); ++iter)
        {
            if (GetModuleID(*iter) == iModuleInfo)
            {
                if (eType != DT_NONE
                    && dsskey_GetDsskeyType(*iter, DSS_ATTR_NATIVE) != eType)
                {
                    continue;
                }

                pKeysCode[iIndex++] = (char)(GetDsskeyIndex(*iter) + 1);
            }
        }

        if (iIndex == 0)
        {
            continue;
        }

        UpdateKeysInfo(GetModuleType(iModuleInfo), pKeysCode, GetModuleIndex(iModuleInfo));
    }

    delete [] pKeysCode;
    DSSKEY_INFO("ReloadModifyDSSKeyData finished key[%d] size[%d]", eType, m_listModify.size());

    return true;
}

void CDSSKeyDataEntryManager::ModifyNativeData(int iDssKeyID)
{
    if (yl::find(m_listModify.begin(), m_listModify.end(), iDssKeyID) == m_listModify.end())
    {
        m_listModify.push_back(iDssKeyID);
    }
}

void CDSSKeyDataEntryManager::FreeNativeData(int iDssKeyID)
{
    YLList<int>::iterator it = yl::find(m_listModify.begin(), m_listModify.end(), iDssKeyID);
    if (it != m_listModify.end())
    {
        m_listModify.erase(it);
    }
}
