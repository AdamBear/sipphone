#include "record_inc.h"

#ifdef IF_USB_SUPPORT
#include "recordmanager.h"
#include "hotplug/storagemanager.h"
#include "hotplug/commondefine.h"

#define SAFE_DELETE(x) if(NULL != x) {delete x; x = NULL;}
#define HOTPLUG_USBDEVICE_DETECTTIMER 30 * 1000

static HotPlugUsbDeviceType GetDeviceType(int iHpdevType)
{
    switch (iHpdevType)
    {
    case HPD_TYP_USB_STORAGE:
        return HOTPLUG_USB_USBSTORAGE;
    case HPD_TYP_USB_PSTN:
        return HOTPLUG_USB_PSTNBOX;
    case HPD_TYP_USB_BT:
        return HOTPLUG_USB_BTDONGLE;
    case HPD_TYP_USB_WLAN:
        return HOTPLUG_USB_WIFIDONGLE;
    case HPD_TYP_USB_EXP:
        return HOTPLUG_USB_EXP50;
    case HPD_TYP_USB_HEADSET:
        return HOTPLUG_USB_USBHEADSET;
    default:
        break;
    }

    return HOTPLUG_USB_USBUNKNOW;
}

static LRESULT OnMessage(msgObject & msg)
{
    bool bHandled = true;
    switch (msg.message)
    {
    case HOTPLUG_USB_DEVICER_STATE_CHANGE:
        _RecordManager.ProcessDeviceMsg(msg);
        break;
    case HOTPLUG_USB_STORAGE_STATE_CHANGING:
        //case HOTPLUG_DEVICE_STATE_CHANGE:
        _RecordManager.ProcessDeviceMsg(msg);
        _RecordManager.SendMessageToRecordObject(msg);
        break;
    case HOTPLUG_USB_STORAGE_VOLUME_CHANGE:
    case HOTPLUG_STORAGE_FILE_LOAD_RESULE:
    case RECORD_STORAGE_RECORD_RESULT:
#ifdef IF_SUPPORT_HDMI
    case HDMI_MSG_STATUS_CHANGE:
    case HDMI_MSG_SWITCH_CHANGE:
#endif //#ifdef IF_SUPPORT_HDMI
    case IPVP_MSG_VIDEO_SNAPSHOT_DONE:
    case IPVP_MSG_RECORD_FAIL:
        //case RECORD_STORAGE_STATE_CHANGE:
        {
            _RecordManager.SendMessageToRecordObject(msg);
        }
        break;
    case TM_TIMER:
        {
            bHandled = _RecordManager.OnTimer(msg.wParam);
        }
        break;
    case VOICE_VPM_START:
        {
            ipvp_request_notify(mkGetHandle(), IPVP_MSG_VIDEO_SNAPSHOT_DONE);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

CRecordManager::CRecordManager()
{
    Init();
}

CRecordManager::~CRecordManager()
{
}

bool CRecordManager::DoOperation(OPERATION_TYPE eOprType, RECORD_TYPE eRecordType,
                                 const char * pFile, void * pExtData, int iId)
{
    bool bRet = false;
    YLList<CBaseRecord *>::iterator itr = m_lstRecordObject.begin();
    for (; itr != m_lstRecordObject.end(); itr++)
    {
        if (NULL != *itr
                && (RECORD_NONE == eRecordType
                    || eRecordType == (*itr)->GetRecordType()))
        {
            bRet = bRet || (*itr)->DoOperation(eOprType, pFile, pExtData, iId);
        }
    }

    return bRet;
}

bool CRecordManager::GetRecordDetailInfo(RecordFileInfo & fileInfo, const char * pFile,
        RECORD_TYPE eRecordType, int iId)
{
    YLList<CBaseRecord *>::iterator itr = m_lstRecordObject.begin();
    for (; itr != m_lstRecordObject.end(); itr++)
    {
        if (NULL == (*itr)
                || (RECORD_NONE != eRecordType
                    && eRecordType != (*itr)->GetRecordType()))
        {
            continue ;
        }

        int iId = _StorageManager.GetActiveStorageIndex();
        RecordFileInfo * pFileInfo = (*itr)->GetFileInfo(pFile, iId);
        if (NULL != pFileInfo)
        {
            fileInfo = *pFileInfo;
            return true;
        }
    }

    return false;
}

bool CRecordManager::GetRecordInfoByIndex(RecordFileInfo & fileInfo, int iIndex,
        RECORD_TYPE eRecordType, int iId)
{
    // 获取具体对象
    CBaseRecord * pObject = GetRecordObject(eRecordType);
    if (NULL == pObject)
    {
        return false;
    }

    return pObject->GetFileInfo(fileInfo, iIndex, iId);
}

int CRecordManager::GetRecordInfoByFile(RecordFileInfo & fileInfo, const char * pFile,
                                        RECORD_TYPE eRecordType, int iId)
{
    // 获取具体对象
    CBaseRecord * pObject = GetRecordObject(eRecordType);
    if (NULL == pObject)
    {
        return -1;
    }

    return pObject->GetFileInfo(fileInfo, pFile, iId);
}


bool CRecordManager::GetRecordFileList(listRecordFileInfo & lstFileInfo, RECORD_TYPE eRecordType,
                                       int iStartPos, int iNum, int iId)
{
    // 获取具体对象
    CBaseRecord * pObject = GetRecordObject(eRecordType);
    if (NULL == pObject)
    {
        return false;
    }

    return pObject->GetFileList(lstFileInfo, iStartPos, iNum, iId);
}

bool CRecordManager::GetRecordVolumeInfo(StorageInfo & sStorageInfo, int iId)
{
    StorageVolumeInfo sVolumeInfo;

    _StorageManager.GetStorageVolume(iId, sVolumeInfo);
    sStorageInfo.m_ullTotal = sVolumeInfo.ulTotalSize;
    sStorageInfo.m_ullFree = sVolumeInfo.ulFreeSize;
    sStorageInfo.m_ullUsed = sVolumeInfo.ulUsedSize;

    CBaseRecord * pObject = GetRecordObject(RECORD_AUDIO);
    if (NULL != pObject)
    {
        sStorageInfo.m_ulAudioRecordTime = pObject->GetAvailableTime(sStorageInfo.m_ullFree);
    }

    pObject = GetRecordObject(RECORD_VIDEO);
    if (NULL != pObject)
    {
        sStorageInfo.m_ulVideoRecordTime = pObject->GetAvailableTime(sStorageInfo.m_ullFree);
    }

    return true;
}

yl::string CRecordManager::GetAvailableRecordingTime(int iId)
{
    StorageInfo sInfo;
    GetRecordVolumeInfo(sInfo, iId);

    return TransTimeToString(sInfo.m_ulAudioRecordTime);
}

yl::string CRecordManager::TransSizeToString(const unsigned long long & ullSize)
{
#define KBSize (1024)
#define MBSize (1024l*1024l)
#define GBSize (1024l*1024l*1024l)

    yl::string strOutBuff;

    if (ullSize <= 0)
    {
        strOutBuff = "0B";
        return strOutBuff;
    }

    char szBuff[64] = {0};
    // GB
    if (ullSize > GBSize)
    {
        sprintf(szBuff, "%.1f", (double)ullSize / GBSize);
        strcat(szBuff, "GB");
    }
    // MB
    else if (ullSize > MBSize)
    {
        sprintf(szBuff, "%.1f", (double)ullSize / MBSize);
        strcat(szBuff, "MB");
    }
    // KB
    else if (ullSize > KBSize)
    {
        sprintf(szBuff, "%.1f", (double)ullSize / KBSize);
        strcat(szBuff, "KB");
    }
    // B
    else
    {
        sprintf(szBuff, "%.1d", ullSize);
        strcat(szBuff, "B");
    }

    strOutBuff = szBuff;
    return strOutBuff;
}

yl::string CRecordManager::TransTimeToString(const unsigned long & lTime)
{
    yl::string strOutBuff;
    UINT uHour, uMinute, uSecond;
    uHour = lTime / 3600;
    uMinute = (lTime / 60) % 60;
    uSecond = lTime % 60;

    strOutBuff = commonAPI_FormatString("%.2d:%.2d:%.2d", uHour, uMinute, uSecond);
    return strOutBuff;
}

bool CRecordManager::IsLoadingFile(RECORD_TYPE eRecordType, int iId)
{
    // 获取具体对象
    CBaseRecord * pObject = GetRecordObject(eRecordType);
    if (NULL == pObject)
    {
        return false;
    }

    return  FILE_LOAD_STATE_LOADING == pObject->GetFileLoadState(iId);
}

bool CRecordManager::IsStorageFull(RECORD_TYPE eRecordType, int iId)
{
    YLList<CBaseRecord *>::iterator itr = m_lstRecordObject.begin();
    for (; itr != m_lstRecordObject.end(); itr++)
    {
        if (NULL != (*itr)
                && (RECORD_NONE == eRecordType
                    || eRecordType == (*itr)->GetRecordType()))
        {
            StorageInfo sVolumeInfo;
            _RecordManager.GetRecordVolumeInfo(sVolumeInfo, iId);

            if (!(*itr)->IsAvaliable(sVolumeInfo.m_ullFree))
            {
                return true;
            }
        }
    }

    return false;
}

bool CRecordManager::IsInRecordState(int iRecordState, RECORD_TYPE eRecordType)
{
    bool bRet = false;
    YLList<CBaseRecord *>::iterator itr = m_lstRecordObject.begin();
    for (; itr != m_lstRecordObject.end(); itr++)
    {
        if (NULL != (*itr)
                && (RECORD_NONE == eRecordType
                    || eRecordType == (*itr)->GetRecordType()))
        {
            bRet = iRecordState == (*itr)->GetRecordState()
                   || iRecordState == (*itr)->GetRecordPlayState();
        }
    }
    return bRet;
}

bool CRecordManager::IsInRecordScene(RECORD_SCENE eRecordScene, RECORD_TYPE eRecordType)
{
    bool bRet = false;
    YLList<CBaseRecord *>::iterator itr = m_lstRecordObject.begin();
    for (; itr != m_lstRecordObject.end(); itr++)
    {
        if (NULL != (*itr)
                && (RECORD_NONE == eRecordType
                    || eRecordType == (*itr)->GetRecordType()))
        {
            bRet = bRet || (eRecordScene == (*itr)->GetRecordScene());
        }
    }
    return bRet;
}

void CRecordManager::SetRecordScene(RECORD_SCENE eRecordScene, RECORD_TYPE eRecordType)
{
    CBaseRecord * pObject = GetRecordObject(eRecordType);
    if (NULL == pObject)
    {
        return;
    }

    pObject->SetRecordScene(eRecordScene);
}

void CRecordManager::GetRecordTime(RECORD_TYPE eRecordType, unsigned int & uHour,
                                   unsigned int & uMinute, unsigned int & uSecond)
{
    CBaseRecord * pObject = GetRecordObject(eRecordType);
    if (NULL == pObject)
    {
        return;
    }

    pObject->GetRecordTime(uHour, uMinute, uSecond);
}

bool CRecordManager::OnTimer(unsigned int iTimerID)
{
    if ((UINT)&m_eCurrentDevice == iTimerID)
    {
        if (STATE_DETECTING == GetStorageState())
        {
            m_bDetectFailed = true;
            ChangeStorageState(STATE_UNRECOGNIZED, m_eCurrentDevice, HOTPLUG_USB_REMOVE);
        }

        CancelDetectTimer();
        return true;
    }

    return false;
}

// 发送消息给对象
bool CRecordManager::SendMessageToRecordObject(msgObject & msg,
        RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    bool bRet = false;
    YLList<CBaseRecord *>::iterator itr = m_lstRecordObject.begin();
    for (; itr != m_lstRecordObject.end(); itr++)
    {
        if (NULL != (*itr)
                && (RECORD_NONE == eRecordType
                    || eRecordType == (*itr)->GetRecordType()))
        {
            bRet = true;
            (*itr)->ProcessMsg(msg);
        }
    }
    return bRet;
}

bool CRecordManager::ProcessDeviceMsg(msgObject & msg)
{
    bool bHandle = true;
    switch (msg.message)
    {
    case HOTPLUG_USB_DEVICER_STATE_CHANGE:
        {
            HotPlugUsbDeviceType eDeviceType = GetDeviceType(msg.lParam);

            //识别到U盘了，不再处理其他设备
            if (m_eStorageState == STATE_CONNECT
                    && eDeviceType != HOTPLUG_USB_USBSTORAGE)
            {
                USBDEVICE_INFO("current device is storage,ignore other device");
                return bHandle;
            }

            // usb 接入时，会先识别成unknow，再识别成真正的设备类型
            if (eDeviceType == HOTPLUG_USB_USBUNKNOW)
            {
                if (HOTPLUG_USB_REMOVE == msg.wParam)
                {
                    ChangeStorageState(STATE_DISCONNECT, eDeviceType, (HotPlugUsbDeviceState)msg.wParam);
                    CancelDetectTimer();
                }
                else if (HOTPLUG_USB_ADD == msg.wParam)
                {
                    ChangeStorageState(STATE_DETECTING, eDeviceType, (HotPlugUsbDeviceState)msg.wParam);
                    SetDetectTimer(eDeviceType);
                    //timerSetThreadTimer((UINT)&m_iDetectingTimerID, m_iDetectingTimerID);
                }
            }
            else
            {
                if (HOTPLUG_USB_ADD == msg.wParam)
                {
                    CancelDetectTimer();
                    // 其他设备,不需要USB相关提示
                    ChangeStorageState(STATE_OTHER_DEVICE, eDeviceType, (HotPlugUsbDeviceState)msg.wParam);
                }
                else if (HOTPLUG_USB_REMOVE == msg.wParam)
                {

                }
            }
        }
        break;
    case HOTPLUG_USB_STORAGE_STATE_CHANGING:
        {
            m_bDetectFailed = false;
            if (HOTPLUG_USB_REMOVE == msg.wParam)
            {
                msgObject msgObject;
                msgObject.message = RECORD_STORAGE_STATE_CHANGE;
                msgObject.wParam = 0;
                SendMessageToRecordObject(msgObject);

                // 不存在存储设备, 释放record对象
                if (_StorageManager.GetStorageDevNum() <= 0)
                {
                    YLList<CBaseRecord *>::iterator itr = m_lstRecordObject.begin();
                    for (; itr != m_lstRecordObject.end();)
                    {
                        SAFE_DELETE(*itr);
                        itr = m_lstRecordObject.erase(itr);
                    }
                }

                if (NULL != msg.GetExtraData())
                {
                    yl::string strStorageDir = (char *)msg.GetExtraData();

                    ChangeStorageState(STATE_DISCONNECT, (HotPlugUsbDeviceType)msg.lParam,
                                       (HotPlugUsbDeviceState)msg.wParam);
                }
            }
            else if (HOTPLUG_USB_ADD == msg.wParam)
            {
                if (NULL != msg.GetExtraData())
                {
                    yl::string strMountPoint = (char *)msg.GetExtraData();
                    USBDEVICE_INFO("Storage Device connect, point:%s", strMountPoint.c_str());

                    // 识别到第一个设备的时候实例化record对象
                    if (_StorageManager.GetStorageDevNum() == 1)
                    {
                        // 实例化文件对象
                        new CAudioRecord();
                        new CPictureRecord();
#ifdef IF_SUPPORT_VIDEO
                        new CVideoRecord();
#endif //#ifdef IF_SUPPORT_VIDEO
                    }
                }

                // 关闭超时计时器
                CancelDetectTimer();

                // 设置状态,发送消息
                ChangeStorageState(STATE_CONNECT, (HotPlugUsbDeviceType)msg.lParam,
                                   (HotPlugUsbDeviceState)msg.wParam);
            }

            //USB设备拔插，状态改变发送事件信息
            event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

void CRecordManager::SetDetectTimer(HotPlugUsbDeviceType eType)
{
    m_eCurrentDevice = eType;
    timerSetThreadTimer((UINT)&m_eCurrentDevice, HOTPLUG_USBDEVICE_DETECTTIMER);
}

void CRecordManager::CancelDetectTimer()
{
    timerKillThreadTimer((UINT)&m_eCurrentDevice);
}

void CRecordManager::Init()
{
    m_eCurrentDevice = HOTPLUG_USB_USBUNKNOW;
    m_eStorageState = STATE_NONE;
    m_bDetectFailed = false;

    SingleMsgReg(HOTPLUG_USB_DEVICER_STATE_CHANGE, OnMessage);
    SingleMsgReg(HOTPLUG_USB_STORAGE_STATE_CHANGING, OnMessage);
    SingleMsgReg(HOTPLUG_USB_STORAGE_VOLUME_CHANGE, OnMessage);
    SingleMsgReg(RECORD_STORAGE_RECORD_RESULT, OnMessage);
#ifdef IF_SUPPORT_HDMI
    SingleMsgReg(HDMI_MSG_STATUS_CHANGE, OnMessage);
    SingleMsgReg(HDMI_MSG_SWITCH_CHANGE, OnMessage);
#endif //#ifdef IF_SUPPORT_HDMI
    SingleMsgReg(TM_TIMER, OnMessage);
    SingleMsgReg(VOICE_VPM_START, OnMessage);
    SingleMsgReg(IPVP_MSG_VIDEO_SNAPSHOT_DONE, OnMessage);
    SingleMsgReg(IPVP_MSG_RECORD_FAIL, OnMessage);
    SingleMsgReg(RECORD_STORAGE_STATE_CHANGE, OnMessage);
    SingleMsgReg(HOTPLUG_STORAGE_FILE_LOAD_RESULE, OnMessage);
    //SingleMsgReg(HOTPLUG_DEVICE_STATE_CHANGE, OnMessage);
}

void CRecordManager::AddRecordObject(CBaseRecord * pRecordObject)
{
    if (NULL != pRecordObject)
    {
        m_lstRecordObject.push_back(pRecordObject);
    }
}

CBaseRecord * CRecordManager::GetRecordObject(RECORD_TYPE eRecordType)
{
    YLList<CBaseRecord *>::iterator itr = m_lstRecordObject.begin();
    for (; itr != m_lstRecordObject.end(); itr++)
    {
        if ((*itr)->GetRecordType() == eRecordType)
        {
            return (*itr);
        }
    }

    return NULL;
}

void CRecordManager::ChangeStorageState(USB_STORAGE_STATE eStorangeState,
                                        HotPlugUsbDeviceType eDeviceType,
                                        HotPlugUsbDeviceState eUsbState)
{
    USB_STORAGE_STATE eOldState = m_eStorageState;
    m_eStorageState = eStorangeState;

    HotPlugUsbModuleInfo moduleInfo;
    moduleInfo.eOldRecordState = eOldState;
    moduleInfo.eNewRecordState = eStorangeState;
    moduleInfo.eUsbDeviceType = eDeviceType;
    moduleInfo.eUsbDeviceState = eUsbState;

    etl_NotifyAppEx(false, RECORD_STORAGE_STATE_CHANGE, m_eStorageState, eOldState,
                    sizeof(HotPlugUsbModuleInfo), &moduleInfo);

    USBDEVICE_INFO("change device [%d] [%d] state[%d] to state [%d] \n", eDeviceType, eUsbState,
                   eOldState, eStorangeState);
}

// 文件个数
int CRecordManager::GetRecordFileNum(RECORD_TYPE eRecordType, int iId)
{
    // 获取具体对象
    CBaseRecord * pObject = GetRecordObject(eRecordType);
    if (NULL == pObject)
    {
        return false;
    }

    return pObject->GetRecordFileNum(iId);
}

#endif // IF_USB_RECORD
