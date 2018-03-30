#include "dsskeydatamodule.h"
#include "dsskeydataentrymanager.h"
#include "dsskey_common.h"
#include "dsskeydata.h"
#include "dsskeycommonfunc.h"
#include "../include/dsskey_enumtypes.h"
#include "../include/dsskey_def.h"
#include <xmlparser/xmlparser.hpp>
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#include "account/include/modaccount.h"
#include "../include/moddsskey.h"
#include "configserver/configserverinc.h"
#include "extended/bluetoothmessagedefine.h"
#include "wireless/bluetoothNew/include/btdefines.h"
#include "extended/wifimessagedefine.h"
#include "record/include/recordmessagedefine.h"
#include "hotplug/commondefine.h"
//#include "service_autop.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "blf/include/modblf.h"
#include "service_dsk_define.h"
#include "ylstringalgorithm.h"
#include <dlfcn.h>
#include "autoset/include/modautoset.h"

CDSSKeyDataModule::CDSSKeyDataModule(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex):
    m_vecDsskeyData(30)
{
    m_moduleType = moduleType;
    m_moduleIndex = iModuleIndex;
    m_maxValidIndex = -1;
    m_LockKeySize = 0;
    m_bIsDirty = false;
}
CDSSKeyDataModule::~CDSSKeyDataModule(void)
{
    Clear();

}

CDSSkeyData * CDSSKeyDataModule::GetDsskeyDataByIndex(int index)
{
    if (index >= 0 && index < m_vecDsskeyData.size())
    {
        return m_vecDsskeyData[index];
    }
    else
    {
        return NULL;
    }
}

int CDSSKeyDataModule::GetSupportDsskeyNum(DSSKEY_MODULE_TYPE moduleType)
{
    int iDsskeyNum = 0;

    switch (moduleType)
    {
    case DMT_LINEKEY_MODULE:
        iDsskeyNum = configParser_GetConfigInt(kszLinekeyNum);
        break;
    case DMT_PROGRAMEKEY_MODULE:
        iDsskeyNum = configParser_GetConfigInt(kszProgrameKeyNum);
        break;
    case DMT_NOTIFYKEY_MODULE:
    case DMT_MEMKEY_MODULE:
        break;
    default:
        iDsskeyNum = configParser_GetConfigInt(kszExpKeyNum);
    }

    return iDsskeyNum;
}

int CDSSKeyDataModule::GetSupportExpNum()
{
    return configParser_GetConfigInt(kszExpNum);
}

bool CDSSKeyDataModule::GetConfigFile(yl::string & strOutput)
{
    if (m_moduleIndex < 0)
    {
        return false;
    }

    switch (m_moduleType)
    {
    case DMT_LINEKEY_MODULE:
        strOutput = LINEKEY_TMP_FILE;
        break;
    case DMT_MEMKEY_MODULE:
        strOutput = MEMKEY_TMP_FILE;
        break;
    case DMT_PROGRAMEKEY_MODULE:
        strOutput = PROGRAME_TMP_FILE;
        break;
    case DMT_NOTIFYKEY_MODULE:
        strOutput = NOTIFY_TMP_FILE;
        break;
    default:
        {
            if (IsEXPModule(m_moduleType))
            {
                char Path[128] = {0};
                sprintf(Path, "%s%d.xml", EXPKEY_TMP_FILE, m_moduleIndex);
                strOutput = Path;
            }
        }
        break;
    }

    return true;
}

bool CDSSKeyDataModule::LoadFromMemory()
{
    Clear();

    int iDsskeyNum = GetSupportDsskeyNum(m_moduleType);

    if (iDsskeyNum <= 0 
        || (m_moduleIndex >= GetSupportExpNum() && GetSupportExpNum() > 0)
        )
    {
        DSSKEY_ERR("dsskey num <=0 ,load [%d] [%d] failed", m_moduleType, m_moduleIndex);
        return false;
    }

    int iIdx = 0;
    int maxValidIndex = -1;//最大的有效dsskey值

    while (iIdx < iDsskeyNum)
    {
        CDSSkeyData * pDsskeyData = new CDSSkeyData();
        if (pDsskeyData)
        {
            int dsskeyID = GetDsskeyDataID(m_moduleType, m_moduleIndex, iIdx);
            pDsskeyData->SetDSSKeyDataID(dsskeyID);

            pDsskeyData->ParserFromMemory(m_moduleType, iIdx, m_moduleIndex);

            pDsskeyData->setDsskeyModuleType(m_moduleType);

            _DsskeyManager.FreeNativeData(pDsskeyData->GetDsskeyDataID());

            bool bCorrect = pDsskeyData->CorrectDsskeyType(m_moduleType, iIdx);

            if (bCorrect)
            {
                //纠正
                //pDsskeyData->ToMemory(m_moduleType, iIdx, m_moduleIndex);
            }

            if (pDsskeyData->GetDsskeyType() != DT_NA)
            {
                maxValidIndex = iIdx;
            }
            if (pDsskeyData->IsLocked())
            {
                m_LockKeySize++;
            }

            m_vecDsskeyData.push_back(pDsskeyData);
        }
        ++iIdx;
        SetPageInfo(maxValidIndex);
    }
    return true;
}

bool CDSSKeyDataModule::SaveToMemory(int iDsskeyID /* = -1 */, int iFlags/* = 0*/)
{
    for (int i = 0; i < m_vecDsskeyData.size(); ++i)
    {
        CDSSkeyData * pDsskeyData = m_vecDsskeyData[i];
        if (iDsskeyID == -1
                || iDsskeyID == pDsskeyData->GetDsskeyDataID())
        {
            // 写新的数据到节点
            pDsskeyData->ToMemory(m_moduleType, i, m_moduleIndex, iFlags);
            pDsskeyData->CorrectDsskeyType(m_moduleType, i);
        }
    }

    return true;
}

void CDSSKeyDataModule::Clear()
{
    m_strConfigFile.clear();
    for (int i = 0; i < m_vecDsskeyData.size(); i ++)
    {
        CDSSkeyData * pDsskeyData = m_vecDsskeyData[i];
        if (pDsskeyData)
        {
            delete pDsskeyData;
        }
    }
    m_vecDsskeyData.clear();
    m_maxValidIndex = -1;
    m_LockKeySize = 0;
    timerKillThreadTimer(TIMER_ID(m_vecDsskeyData));
}

int CDSSKeyDataModule::size()
{
    return m_vecDsskeyData.size();
}


//设置EXP长按定时器状态
void CDSSKeyDataModule::SetEXPLPTimerStatus(bool status)
{
    m_bEXPKeyLPTimerIsOn = status;
}
void CDSSKeyDataModule::ResetBLFListDsskeyByLineId(int iLineId)
{
    for (int i = 0;  i < m_vecDsskeyData.size(); i++)
    {
        CDSSkeyData * pDsskeyData = m_vecDsskeyData[i];
        if (pDsskeyData && pDsskeyData->GetDsskeyType() == DT_BLFLIST)
        {
            if (pDsskeyData->GetLineIndex() == iLineId)
            {
                pDsskeyData->SetLabel("BLF List");
                // TODO 更新EXP39显示和灯状态, Fix me
                pDsskeyData->UpdateRemoteStatus(CN_BLA_STATUS_UNKOWN);
                pDsskeyData->SetValue("");
                pDsskeyData->SetExtension("");
            }
        }
    }
}


bool CDSSKeyDataModule::GetDsskeyDataList(int fromIndex, int AmountToGet,
        YLList<CDSSkeyData * > & listOutput)
{
    int bRet = false;

    listOutput.clear();
    if (fromIndex >= 0 && fromIndex < m_vecDsskeyData.size() && AmountToGet  > 0)
    {
        for (int i = fromIndex; i < m_vecDsskeyData.size() && (i - fromIndex) < AmountToGet; i++)
        {
            CDSSkeyData * pDsskeyData = m_vecDsskeyData[i];
            if (pDsskeyData)
            {
                int dsskeyID = pDsskeyData->GetDsskeyDataID();
                if (m_moduleType == DMT_LINEKEY_MODULE)//lock处理
                {
                    pDsskeyData = GetRelatedLockedDsskey(pDsskeyData);//获取对应界面的被锁定的DSSKeyID
                }
                if (pDsskeyData)
                {
                    bRet = true;
                    listOutput.push_back(pDsskeyData);
                }
            }
        }
    }

    return bRet;
}

bool CDSSKeyDataModule::LockDsskey(CDSSkeyData * pDsskeyData, DssKeyLockType lockType,
                                   bool bOriginalLocked)
{
    if (!IsBelongDSSKey(pDsskeyData))
    {
        return false;
    }

    int keyIndex = GetDsskeyIndex(pDsskeyData->GetDsskeyDataID());
    return false;
}

CDSSkeyData * CDSSKeyDataModule::GetRelatedLockedDsskey(CDSSkeyData * pDsskeyData)
{

    return NULL;
}


CDSSkeyData * CDSSKeyDataModule::GetDsskeyDataByType(DssKey_Type eType)
{
    //只用于获取通知中心Dsskey，因为Notify中相同类型的Dsskey只会存在一个
    if (DMT_NOTIFYKEY_MODULE != m_moduleType)
    {
        return NULL;
    }

    for (int i = 0; i < m_vecDsskeyData.size(); ++i)
    {
        CDSSkeyData * pDsskeyData = m_vecDsskeyData[i];
        if (NULL != pDsskeyData
                && pDsskeyData->GetDsskeyType() == eType)
        {
            return pDsskeyData;
        }
    }

    return NULL;
}

/*
CDSSkeyData * CDSSKeyDataModule::GetFirstDsskeyDataByLineID( int lineIndex, DssKey_Type dsskeyType )
{
        for (int i = 0 ; i < m_vecDsskeyData.size(); i ++)
        {
            CDSSkeyData *pDsskeyData = GetDsskeyDataByIndex(i);
            if (pDsskeyData)
            {
                if (pDsskeyData->GetDsskeyType() == dsskeyType && pDsskeyData->GetLineIndex() == lineIndex)
                {
                    return pDsskeyData;
                }
            }
        }
        return NULL;
}

void CDSSKeyDataModule::GetPageDataByDSSKey( CDSSkeyData *pDsskeyData ,YLList<CDSSkeyData * > &listOutput)
{
    listOutput.clear();
    if (pDsskeyData && IsBelongDSSKey(pDsskeyData))
    {
        int keyIndex = GetDsskeyIndex(pDsskeyData->GetDsskeyDataID());
        if (m_pRelatedPageProcessor)
        {
            int perpageNum = m_pRelatedPageProcessor->GetValidAmountPerPage(true);//获取每页数量
            int pageIndex = keyIndex / perpageNum;
            int startIndex = pageIndex * perpageNum;
            GetDsskeyDataList(startIndex, perpageNum, listOutput);
        }
    }
}*/

bool CDSSKeyDataModule::IsBelongDSSKey(CDSSkeyData * pDsskeyData)
{
    if (pDsskeyData)
    {
        int keyIndex = GetDsskeyIndex(pDsskeyData->GetDsskeyDataID());
        CDSSkeyData * pTemp = GetDsskeyDataByIndex(keyIndex);
        return pDsskeyData == pTemp;
    }
    return false;

}

int CDSSKeyDataModule::GetPageIndexByDSSKey(CDSSkeyData * pDsskeyData)
{
    if (pDsskeyData && IsBelongDSSKey(pDsskeyData))
    {
        int keyIndex = GetDsskeyIndex(pDsskeyData->GetDsskeyDataID());
    }
    return -1;
}

bool CDSSKeyDataModule::GetCurrentPageData(YLList<CDSSkeyData *> & output)
{
    return false;
}

bool CDSSKeyDataModule::GetPageDataByPageIndex(YLList<CDSSkeyData *> & output, int pageIndex)
{

    return false;
}

// 更新账号的图标显示，如Fwd、Dnd
void CDSSKeyDataModule::UpdateLinekeyLabel(int iAccount /*= -1*/)
{
    for (int i = 0; i < m_vecDsskeyData.size(); ++i)
    {
        if (m_vecDsskeyData[i] == NULL
                || m_vecDsskeyData[i]->GetDsskeyType() != DT_LINE)
        {
            continue;
        }

        int iRealAccount = m_vecDsskeyData[i]->GetLineIndex();

        if (iAccount != -1
                && iAccount != iRealAccount)
        {
            continue;
        }

        // 更新相应的Line状态
        Dsskey_Status eNewStatus = MapLineState(acc_GetState(iRealAccount), iRealAccount);
        if (eNewStatus != m_vecDsskeyData[i]->GetStatus())
        {
            m_vecDsskeyData[i]->UpdateLineStatus(eNewStatus);
            m_vecDsskeyData[i]->UpdateStatus();
        }
    }
}

int CDSSKeyDataModule::GetPageAmount()
{
    return 0;
}

// 进入下一个页面
bool CDSSKeyDataModule::GotoNextPage()
{

    return false;
}

void CDSSKeyDataModule::GetDsskeyListByType(YLVector<CDSSkeyData *> & vecDsskey, DssKey_Type eType)
{
    for (int i = 0; i < m_vecDsskeyData.size(); ++i)
    {
        if (m_vecDsskeyData[i] && m_vecDsskeyData[i]->GetDsskeyType() == eType)
        {
            vecDsskey.push_back(m_vecDsskeyData[i]);
        }
    }
}

bool CDSSKeyDataModule::ReloadDssKeyAttrib(YLList<int> & lstDsskeyIndex)
{
    // 对数据进行排序
    // 目前网页和Autop都是顺序发过来, 故几乎不花时间
    Sort(lstDsskeyIndex, yl::less<int>());

    YLList<int>::ListIterator itr = lstDsskeyIndex.begin();
    for (;  itr != lstDsskeyIndex.end(); ++itr)
    {
        ReloadDssKeyAttrib(*itr);
    }

    return true;
}

bool CDSSKeyDataModule::ReloadDssKeyAttrib(int index)
{
    CDSSkeyData * pDsskeyData = GetDsskeyDataByIndex(index);
    if (NULL == pDsskeyData)
    {
        return false;
    }

    pDsskeyData->ParserFromMemory(m_moduleType, index, m_moduleIndex);

    if (pDsskeyData->CorrectDsskeyType(m_moduleType, index))
    {
        //pDsskeyData->ToMemory(m_moduleType, index, m_moduleIndex);
    }

    return true;
}

void CDSSKeyDataModule::SetPageInfo(int keyIndex)
{
}


bool CDSSKeyDataModule::SetDsskeyData(int iDsskeyID, const DssKeyLogicData & dsskeyLogicData,
                                      int iFlags/* = DSSKEY_SET_NORMAL*/)
{
    int iDsskeyIndex = GetDsskeyIndex(iDsskeyID);
    CDSSkeyData * pDsskey = GetDsskeyDataByIndex(iDsskeyIndex);
    if (pDsskey)
    {

        bool bLocked = pDsskey->IsLocked();
        const DssKeyLogicData & LogicData = pDsskey->GetLogicData();
        // 状态是否改变
        bool bUpdateStatus = LogicData.isStatusChanged(dsskeyLogicData);
        // 写入配置的值是否改变
        bool bConfigChanged = LogicData.isConfigChanged(dsskeyLogicData);
        DSSKEY_INFO("SetDsskeyData id[%d] flag[%d] chang[%d][%d] type[%d]>[%d] line[%d]>[%d] lab[%s]>[%s] value[%s]>[%s] ext[%s]>[%s]",
                    iDsskeyID, iFlags, bUpdateStatus, bConfigChanged,
                    LogicData.eDKType, dsskeyLogicData.eDKType,
                    LogicData.iLineIndex, dsskeyLogicData.iLineIndex,
                    LogicData.strLabel.c_str(), dsskeyLogicData.strLabel.c_str(),
                    LogicData.strValue.c_str(), dsskeyLogicData.strValue.c_str(),
                    LogicData.strExtension.c_str(), dsskeyLogicData.strExtension.c_str());

        if (bConfigChanged)
        {
            // 保存原始数据
            if (iFlags & DSSKEY_SET_WRITE)
            {
                _DsskeyManager.FreeNativeData(iDsskeyID);
            }
            else if (iFlags & DSSKEY_SET_ONLY)
            {
                _DsskeyManager.ModifyNativeData(iDsskeyID);
            }

            // 保存到临时文件
            SetDirty(true);
        }

        DssKey_Type dkType = pDsskey->GetDsskeyType();
        pDsskey->SetData(dsskeyLogicData);

        // 当改变的是 value， extension，line， dsskey类型的时候才清空状态
        if (bUpdateStatus)
        {
#if IF_BUG_XMLBROWSER_EXP_LED
            pDsskey->ClearLightType();
#endif
            pDsskey->ClearStatus();
        }

        //具有保存标志时保存
        if (iFlags & DSSKEY_SET_WRITE)
        {
            SaveToMemory(iDsskeyID, iFlags);
        }

        //具有通知标识时通知变化
        if (iFlags & DSSKEY_SET_NOTIFY)
        {
            if (dkType != dsskeyLogicData.eDKType)
            {
                pDsskey->OnTypeModify(dkType, dsskeyLogicData.eDKType, NULL);
            }

            if (bConfigChanged)
            {
                pDsskey->OnConfigChange(NULL);
            }
        }

        // 同步刷新
        if (iFlags & DSSKEY_SET_REFRESH)
        {
            dsskey_refreshDSSKeyUI(pDsskey->GetDsskeyDataID());
        }

        return true;
    }

    return false;
}

#ifdef IF_SUPPORT_BLUETOOTH
//处理Bluetooth 状态改变消息
bool CDSSKeyDataModule::OnBluetoothStateChange(msgObject & msg)
{
    bool bHandle = false;

    for (int i = 0; i < m_vecDsskeyData.size(); ++i)
    {
        CDSSkeyData * pDsskey = m_vecDsskeyData[i];
        if (pDsskey == NULL
                || (pDsskey->GetDsskeyType() != DT_BTLINE
                    && pDsskey->GetDsskeyType() != DT_BLUETOOTH))
        {
            continue;
        }

        if (DT_BTLINE == pDsskey->GetDsskeyType())
        {
            if (msg.message != BLUETOOTH_MSG_CONNECT_STATE
                    || !_ISBTPHONE(msg.lParam))
            {
                //只处理蓝牙手机的连接状态
                continue;
            }

            if (msg.wParam == 0)
            {
                //连接失败
                pDsskey->SetStatus(DS_BTLINE_OFFLINE);
            }
            else if (msg.wParam == 1)
            {
                //连接成功
                pDsskey->SetStatus(DS_BTLINE_ONLINE);
            }
            else if (msg.wParam == 2)
            {
                //连接中
                pDsskey->SetStatus(DS_BTLINE_CONNECTTING);
            }
        }
        else if (DT_BLUETOOTH == pDsskey->GetDsskeyType())
        {
            //通话中不处理
            if (talklogic_SessionExist())
            {
                return false;
            }

            Dsskey_Status eStatus = DS_NOTIFY_DISCONNECT;

            if (msg.message == BLUETOOTH_MSG_STATE_CHANGE)
            {
                eStatus = (msg.wParam == 1) ? DS_NOTIFY_CONNECT : DS_NOTIFY_DISCONNECT;
                pDsskey->SetStatus(eStatus);
            }
        }

        pDsskey->UpdateStatus();
        bHandle = true;
    }

    return bHandle;
}
#endif //#ifdef IF_SUPPORT_BLUETOOTH

//处理Notify Dsskey状态改变
bool CDSSKeyDataModule::OnNotifyDsskeyStateChange(msgObject & msg)
{
    bool bHandle = true;
    Dsskey_Status eStatus = DS_NOTIFY_DISCONNECT;
    DssKey_Type eType = DT_NONE;

    //通话中不处理Wi-Fi/USB
    if (talklogic_SessionExist()
            && (msg.message == WIFI_MSG_UPDATE_UI
                || msg.message == RECORD_STORAGE_STATE_CHANGE))
    {
        return false;
    }

#ifdef IF_SUPPORT_VIDEO
    //音频通话不处理camera
    if (talklogic_SessionExist()
            && !talklogic_IsVideoSessionExist()
            && msg.message == CAMERA_MSG_STATUS_CHANGE)
    {
        return false;
    }
#endif // IF_SUPPORT_VIDEO

    switch (msg.message)
    {
    case WIFI_MSG_UPDATE_UI:
        {
            if (msg.wParam != OP_WIFI_OPEN_RESULT
                    && msg.wParam != OP_WIFI_CLOSE_RESULT)
            {
                return false;
            }

            eType = DT_WIFI;

            if (msg.wParam == OP_WIFI_OPEN_RESULT
                    && (msg.lParam == WIRELESS_OP_OPEN_RESULT_SUCCESS
                        || msg.lParam == WIRELESS_OP_OPEN_RESULT_ING))
            {
                eStatus = DS_NOTIFY_CONNECT;
            }
        }
        break;
    case RECORD_STORAGE_STATE_CHANGE:
        {
            if (msg.wParam != 0
                    && msg.wParam != 1)
            {
                return false;
            }

            eType = DT_USB;

            if (msg.wParam == 1)
            {
                eStatus = DS_NOTIFY_CONNECT;
            }
        }
        break;
#ifdef _T49
    case CAMERA_MSG_STATUS_CHANGE:
        {
            eType = DT_CAMERA;
            eStatus = _DsskeyManager.GetCameraDSStatus(msg.wParam);
        }
        break;
    case HDMI_MSG_STATUS_CHANGE:
    case HDMI_MSG_SWITCH_CHANGE:
        {
            if (msg.wParam != 0
                    && msg.wParam != 1)
            {
                return false;
            }

            eType = DT_EXT_DISPLAY;
            eStatus = _DsskeyManager.GetExtDisplayStatus();
        }
        break;
#endif
    default:
        bHandle = false;
        break;
    }

    CDSSkeyData * pDsskey = GetDsskeyDataByType(eType);
    if (bHandle
            && NULL != pDsskey)
    {
        pDsskey->SetStatus(eStatus);
        pDsskey->UpdateStatus();
    }

    return bHandle;
}

// line状态改变消息处理
bool CDSSKeyDataModule::OnLineStateChange(msgObject & msg)
{
    for (int i = 0; i < m_vecDsskeyData.size(); ++i)
    {
        CDSSkeyData * pDsskey = m_vecDsskeyData[i];
        if (pDsskey == NULL
                || pDsskey->GetDsskeyType() != DT_LINE)
        {
            continue;
        }

        int iRealAccount = pDsskey->GetRealLine();
        if (iRealAccount >= 0)
        {
            // 非Auto类型的
            if ((int)msg.wParam == iRealAccount)
            {
                Dsskey_Status eStatus = MapLineState(acc_GetState((int)msg.wParam), (int)msg.wParam);

                pDsskey->UpdateLineStatus(eStatus);
                pDsskey->UpdateStatus();
            }
        }
        else
        {
            // 如果是账号是auto,则让它自动再选择可用账号进行更新
            pDsskey->CorrectRealBindAccount();
            Dsskey_Status eStatus = MapLineState(acc_GetState(pDsskey->GetRealLine()), pDsskey->GetRealLine());
            pDsskey->UpdateLineStatus(eStatus);
            pDsskey->UpdateStatus();
        }
    }

    return true;
}

bool CDSSKeyDataModule::OnTimer(UINT uTimerID)
{
    if (uTimerID == TIMER_ID(m_bIsDirty))
    {
        SetDirty(false);
        yl::string strModuleFile;
        GetConfigFile(strModuleFile);
        dsskey_SaveDsskeyInfoToFile(strModuleFile, DSS_FILE_TYPE_XML, m_moduleType, m_moduleIndex);
        // 临时文件写完了通知sip
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_BLF, 0);
        return true;
    }

    for (int i = 0; i < m_vecDsskeyData.size(); i ++)
    {
        CDSSkeyData * pDsskey = m_vecDsskeyData[i];
        if (pDsskey)
        {
            pDsskey->OnTimer(uTimerID);
        }
    }

    return true;
}

int CDSSKeyDataModule::GetUnboundDsskeyByAccountID(int accountID)
{
    for (int i = 0; i < m_vecDsskeyData.size(); i ++)
    {
        CDSSkeyData * pDsskey = m_vecDsskeyData[i];
        if (pDsskey && pDsskey->GetDsskeyType() == DT_LINE)
        {
            return pDsskey->GetDsskeyDataID();
        }
    }
    return -1;
}

int CDSSKeyDataModule::CountsDsskeyByAccID(int iAccountID, bool bNative/* = false*/)
{
    int iCounts = 0;
    for (int i = 0; i < m_vecDsskeyData.size(); i++)
    {
        CDSSkeyData * pDsskey = m_vecDsskeyData[i];
	    if (pDsskey == NULL)
	    {
	        continue;
	    }

	    if (bNative)
	    {
	        int iDsskeyID = pDsskey->GetDsskeyDataID();
	        if (dsskey_GetDsskeyType(iDsskeyID, DSS_ATTR_NATIVE) == DT_LINE
	                && dsskey_GetDsskeyLine(iDsskeyID, DSS_ATTR_NATIVE) == iAccountID)
	        {
	            ++iCounts;
	        }
	    }
	    else if (pDsskey->GetDsskeyType() == DT_LINE
                && iAccountID == pDsskey->GetRealLine())
        {
	        ++iCounts;
        }
    }
    return iCounts;
}

bool CDSSKeyDataModule::GetBLAIndexByDsskey(int & iIndex, int iDsskeyID, int iAccountID)
{
    for (int i = 0; i < m_vecDsskeyData.size(); i++)
    {
        CDSSkeyData * pLinekey = m_vecDsskeyData[i];
        if (pLinekey->GetDsskeyType() == DT_LINE
                && pLinekey->GetRealLine() == iAccountID)
        {
            iIndex++;
            if (pLinekey->GetDsskeyDataID() == iDsskeyID)
            {
                return true;
            }
        }
    }

    return false;
}

void CDSSKeyDataModule::ResetModuleDataToFactory()
{
    CFG_ITEM_MAP * mapDsskeyData = CDSSkeyData::GetDsskeyConfigIDMapData(m_moduleType);

    if (NULL == mapDsskeyData)
    {
        return;
    }

    DSSKEY_INFO("reset [%d] to default", m_moduleType);

    int iMaxKeyIndex = GetSupportDsskeyNum(m_moduleType);

    for (int iKeyIndex = 0; iKeyIndex < iMaxKeyIndex; ++iKeyIndex)
    {
        for (CFG_ITEM_MAP::iterator itBeg = mapDsskeyData->begin()
                ; itBeg != mapDsskeyData->end(); ++itBeg)
        {
            LPCSTR strAttr = itBeg->first.c_str();

            //获取出厂值
            yl::string strFactoryValue = CDSSkeyData::GetStringConfig(m_moduleType, strAttr, iKeyIndex,
                                         m_moduleIndex, "", CONFIG_LEVEL_FACTORY);

            //出厂值设置到配置
            CDSSkeyData::SetStringConfig(m_moduleType, strAttr, iKeyIndex, m_moduleIndex,
                                         strFactoryValue, CONFIG_LEVEL_IGNORE);
        }
    }
}

LPCSTR CDSSKeyDataModule::GetModuleName()
{
    switch (m_moduleType)
    {
    case DMT_LINEKEY_MODULE:
        return "linekey";

    case DMT_MEMKEY_MODULE:
        return "memorykey";

    case DMT_PROGRAMEKEY_MODULE:
        return "programablekey";

    default:
        if (IsEXPModule(m_moduleType))
        {
            return "expansion_module";
        }

        break;
    }

    return NULL;
}

// 保存Dsskey信息到指定文件中，用于上传给服务器
bool CDSSKeyDataModule::SaveDsskeyInfoToFile(chFileStdio & doc)
{
    LPCSTR lpName = GetModuleName();

    if (lpName == NULL)
    {
        return false;
    }

#define M7_FMT_TEMPLATE "%s.%d.%s = "
#define M7_FMT_INT_TEMPLATE M7_FMT_TEMPLATE "%d\n"
#define M7_FMT_STR_TEMPLATE M7_FMT_TEMPLATE "%s\n"
    // commonAPI_FormatString 效率较低
    char buff[1024] = {0};
    char szKeyName[256] = {0};
    strcpy(szKeyName, lpName);

    if (IsEXPModule(m_moduleType))
    {
        sprintf(szKeyName + strlen(szKeyName), ".%d.key", m_moduleIndex + 1);
    }

    yl::string strOutput;

    for (int i = 1 ; i <= m_vecDsskeyData.size(); ++i)
    {
        CDSSkeyData * pDsskeyData = m_vecDsskeyData[i - 1];

        if (pDsskeyData == NULL)
        {
            continue;
        }

        const DssKeyLogicData & sData = pDsskeyData->GetLogicData();
        sprintf(buff, M7_FMT_INT_TEMPLATE, szKeyName, i, "type", sData.eDKType);
        strOutput += buff;
        sprintf(buff, M7_FMT_INT_TEMPLATE, szKeyName, i, "line", sData.iLineIndex + 1);
        strOutput += buff;
        sprintf(buff, M7_FMT_STR_TEMPLATE, szKeyName, i, "value", sData.strValue.c_str());
        strOutput += buff;
        sprintf(buff, M7_FMT_STR_TEMPLATE, szKeyName, i, "pickup_value", sData.strExtension.c_str());
        strOutput += buff;
        sprintf(buff, M7_FMT_STR_TEMPLATE, szKeyName, i, "label", sData.strLabel.c_str());
        strOutput += buff;

        // 只有Linekey才有shortlabel
        if (m_moduleType == DMT_LINEKEY_MODULE)
        {
            sprintf(buff, M7_FMT_STR_TEMPLATE, szKeyName, i, "shortlabel", sData.strShortLabel.c_str());
            strOutput += buff;
        }
    }

    doc.WriteString(strOutput);
    return true;
}

bool CDSSKeyDataModule::SaveDsskeyInfoToFile(pugi::xml_document & doc)
{
    using namespace pugi;

    LPCSTR lpNode = m_moduleType == DMT_PROGRAMEKEY_MODULE ? NODE_PROGRAMEKEY : NODE_DSSKEY;
    xml_node nodeRoot = doc.append_child(lpNode);

    if (IsEXPModule(m_moduleType))
    {
        nodeRoot.append_attribute("index", m_moduleIndex);
    }

    for (int i = 0 ; i < m_vecDsskeyData.size(); ++i)
    {
        CDSSkeyData * pDsskeyData = m_vecDsskeyData[i];

        if (pDsskeyData == NULL)
        {
            continue;
        }

        const DssKeyLogicData & sData = pDsskeyData->GetLogicData();
        xml_node nodeItem = nodeRoot.append_child("item");
        nodeItem.append_attribute("index", i);
        nodeItem.append_attribute("type", sData.eDKType);
        nodeItem.append_attribute("id", GetUniDsskeyID(sData.iDssKeyID));

        xml_node nodeSubItem = nodeItem.append_child("itemdata");
        nodeSubItem.append_attribute("line", sData.iLineIndex);
        nodeSubItem.append_attribute("value", sData.strValue.c_str());
        nodeSubItem.append_attribute("extension", sData.strExtension.c_str());
        nodeSubItem.append_attribute("label", sData.strLabel.c_str());

        if (m_moduleType == DMT_LINEKEY_MODULE)
        {
            nodeSubItem.append_attribute("shortlabel", sData.strShortLabel.c_str());
        }
    }

    return true;
}

bool CDSSKeyDataModule::SaveDsskeyInfoToFile(iniFile & doc)
{
    LPCSTR lpName = GetModuleName();

    if (lpName == NULL)
    {
        return false;
    }

    // 是否采用长格式，当写exp到同一个文件时必须是用长格式
    bool bFullMode = IsEXPModule(m_moduleType);
    char szValue[1024] = {0};

    for (int i = 0 ; i < m_vecDsskeyData.size(); ++i)
    {
        CDSSkeyData * pDsskeyData = m_vecDsskeyData[i];

        if (pDsskeyData == NULL)
        {
            continue;
        }

        if (bFullMode)
        {
            sprintf(szValue, "%s.%d.key.%d", lpName, m_moduleIndex, i);
        }
        else
        {
            sprintf(szValue, "%s.%d", lpName, i);
        }

        iniSection & section = doc.GetSection(szValue);
        const DssKeyLogicData & sData = pDsskeyData->GetLogicData();
        sprintf(szValue, "%d", sData.eDKType);
        section.GetKey("type").SetValue(szValue);
        sprintf(szValue, "%d", sData.iLineIndex);
        section.GetKey("line").SetValue(szValue);
        section.GetKey("value").SetValue(sData.strValue.c_str());
        section.GetKey("extension").SetValue(sData.strExtension.c_str());
        section.GetKey("label").SetValue(sData.strLabel.c_str());

        if (m_moduleType == DMT_LINEKEY_MODULE)
        {
            section.GetKey("shortlabel").SetValue(sData.strShortLabel.c_str());
        }
    }

    return true;
}

void CDSSKeyDataModule::SetDirty(bool bDirty)
{
    if(m_bIsDirty == bDirty)
    {
        return;
    }
    DSSKEY_INFO("set dirty [%d]",bDirty);
    m_bIsDirty = bDirty;

    if (m_bIsDirty)
    {
        timerSetThreadTimer(TIMER_ID(m_bIsDirty), 10);
    }
    else
    {
        timerKillThreadTimer(TIMER_ID(m_bIsDirty));
    }
}
