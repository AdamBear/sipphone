#include "record_inc.h"

#ifdef IF_USB_SUPPORT
#include "hotplug/storagemanager.h"

// 辅助函数：比较大小.
typedef bool (*FuncCompare)(const yl::string & lhs, const yl::string & rhs);
static bool TimeCompare(const yl::string & lhs, const yl::string & rhs)
{
    // 分钟数大小
    return (atoi(lhs.c_str()) > atoi(rhs.c_str()));
}

// 辅助函数：时间转换为提示信息
static void TransTimeToWarningInfo(long lTime, int & iWarningLev, long & lWarningTime)
{
    // 读取提示的警告值,单位是分钟
    YLList<yl::string> lstWaringMinite;
    yl::string strWarningMinite = configParser_GetConfigString(kszUSBRecordWarningTime);
    commonAPI_splitStringBySeparator(strWarningMinite, ';', lstWaringMinite);
    Sort<yl::string, FuncCompare>(lstWaringMinite, TimeCompare);

    // 初始化
    iWarningLev = 0;
    lWarningTime = 0;

    YLList<yl::string>::iterator itr = lstWaringMinite.begin();
    for (; itr != lstWaringMinite.end(); itr++)
    {
        // 转成秒数比较
        long lValue = atoi((*itr).c_str()) * 60;
        if (lTime > lValue)
        {
            break;
        }

        ++iWarningLev;
        lWarningTime = lValue;
    }
}

// 辅助函数：声道切换
static CHANNEL_MODE SwitchChannel(CHANNEL_MODE eChannel, bool bDirectSwitch = true)
{
    CHANNEL_MODE eTargetChannel = CHANNEL_NULL;

    switch (eChannel)
    {
    case CHANNEL_IDLE:
        {
            if (voice_GetHeadsetStatus())
            {
                eTargetChannel = CHANNEL_HEADSET;
            }
            else if (!voice_GetHandsetStatus())
            {
                //eTargetChannel = CHANNEL_RING;
                eTargetChannel = CHANNEL_HANDFREE;
            }
        }
        break;
    case CHANNEL_HANDFREE:
        {
            //eTargetChannel = CHANNEL_RING;
            eTargetChannel = CHANNEL_HANDFREE;

            // 灭耳麦灯和图标
            ledlamp_PostEvent(LAMP_EVENTTYPE_HEADSET, false);
            idleScreen_DropStatus(PS_STATE_HEADSET);
        }
        break;
    case CHANNEL_AUTO:
        {
            eTargetChannel = CHANNEL_AUTO;
        }
        break;
    case CHANNEL_RING:
        {
            eTargetChannel = CHANNEL_RING;
        }
        break;
    default:
        break;
    }

    if (CHANNEL_NULL != eTargetChannel && bDirectSwitch)
    {
        voice_SwitchChannel(eTargetChannel);

        APP_INFO("USBRecord SwtichChanel to [%d]\n", eTargetChannel);
    }

    return eTargetChannel;
}

///////////////////////////////////////////////////////////////////////
CAudioRecord::CAudioRecord()
{
    m_eType = RECORD_AUDIO;
    m_iWarningLevel = 0;
}

CAudioRecord::~CAudioRecord()
{
}

// 文件夹名
yl::string CAudioRecord::GetFolder()
{
    return "Audio Record";
}

// 额外的文件夹名
yl::string CAudioRecord::GetFolderEx()
{
    return yl::string("/") + yl::string(",") + yl::string("CallRecording");
}

// 后缀名
yl::string CAudioRecord::GetSuffix()
{
    if (PT_T49 == devicelib_GetPhoneType())
    {
        return ".aac";
    }

    return ".wav";
}

// 支持的格式
yl::string CAudioRecord::GetSupportFormat()
{
    if (PT_T49 == devicelib_GetPhoneType())
    {
        return yl::string("wav") + yl::string(",") + yl::string("aac");
    }

    return yl::string("wav");
}

// 操作执行
bool CAudioRecord::DoOperation(OPERATION_TYPE eOprType, const char * pFile/* = NULL*/,
                               void * pExtData/* = NULL*/, int iId /*= STORAGE_DEFAULT*/)
{
    bool bSuccess = false;

    switch (eOprType)
    {
    case OPERATION_RECORD_START:
        {
            if (STATE_RECORDING == GetRecordState()
                    || STATE_RECORD_PAUSE == GetRecordState()
                    || STATE_RECORD_AUTO_PAUSE == GetRecordState())
            {
                break ;
            }

            ProcessIdleRecord(OPERATION_RECORD_START);

            m_strRecordFile = GenerateFullPath(iId, GetFolder(), GetSuffix(), pFile);
            if (RecordInterface_RecordStart(m_strRecordFile.c_str()))
            {
                bSuccess = true;
                SetRecordState(STATE_RECORDING);
                m_timepiece.StartTimer();

                // 将文件加入信息请求列表
                AddNewFile(m_strRecordFile);
            }
            else
            {
                m_strRecordFile.clear();
                ProcessIdleRecord(OPERATION_RECORD_STOP);
            }

            etl_NotifyApp(false, RECORD_STORAGE_RECORD_RESULT, 0, bSuccess);
            // 开启容量查询
            _StorageManager.StartVolumeCheck(true);
        }
        break;
    case OPERATION_RECORD_STOP:
        {
            if (STATE_RECORDING != GetRecordState()
                    && STATE_RECORD_PAUSE != GetRecordState()
                    && STATE_RECORD_AUTO_PAUSE != GetRecordState())
            {
                break ;
            }

            //暂停录制要恢复,不然VPM会记录当前还是为暂停状态,
            //待VPM提交stop录音自动去掉记录之后去掉该代码
            if (STATE_RECORD_PAUSE == GetRecordState()
                    || STATE_RECORD_AUTO_PAUSE == GetRecordState())
            {
                RecordInterface_RecordResume();
            }

            if (RecordInterface_RecordStop())
            {
                ProcessIdleRecord(OPERATION_RECORD_STOP);
#ifdef IF_FEATURE_DIAGNOSE
                // 通知录音结束:诊断程序根据配置控制是否上传服务器
                event_SendEvent(EVENT_HOTPLUG_USB_RECORDED, 0, 0, m_strRecordFile, EMT_DIAG);
#endif //#ifdef IF_FEATURE_DIAGNOSE

                bSuccess = true;
                SetRecordState(STATE_IDLE);
                m_eScene = RECORD_SCENE_NONE;
                m_strRecordFile = "";
                m_timepiece.StopTimer();

                // 发起文件信息获取请求
                RequestLoadFileList(iId);
            }

            if (bSuccess && pExtData != NULL)
            {
                int iRet = *((int *)pExtData);
                etl_NotifyApp(false, RECORD_STORAGE_RECORD_RESULT, 1, iRet);
            }
            else
            {
                etl_NotifyApp(false, RECORD_STORAGE_RECORD_RESULT, 1, bSuccess);
            }

            // 停止容量查询
            _StorageManager.StartVolumeCheck(false);
        }
        break;
    case OPERATION_RECORD_PAUSE:
        {
            if (STATE_RECORDING != GetRecordState())
            {
                break ;
            }

            bool bAuto = false;
            if (pExtData)
            {
                bAuto = *((bool *)pExtData);
            }

            if (RecordInterface_RecordPause())
            {
                bSuccess = true;
                if (bAuto)
                {
                    SetRecordState(STATE_RECORD_AUTO_PAUSE);
                }
                else
                {
                    SetRecordState(STATE_RECORD_PAUSE);
                }

                m_timepiece.PauseTimer();
            }

            etl_NotifyApp(false, RECORD_STORAGE_RECORD_RESULT, 3, bSuccess);
        }
        break;
    case OPERATION_RECORD_RESUME:
        {
            ProcessIdleRecord(OPERATION_RECORD_RESUME);

            if (STATE_RECORD_PAUSE != GetRecordState()
                    && STATE_RECORD_AUTO_PAUSE != GetRecordState())
            {
                break ;
            }

            bool bAuto = *((bool *)pExtData);
            if (bAuto && STATE_RECORD_AUTO_PAUSE != GetRecordState())
            {
                break;
            }

            if (RecordInterface_RecordResume())
            {
                bSuccess = true;
                SetRecordState(STATE_RECORDING);

                m_timepiece.StartTimer();
            }

            etl_NotifyApp(false, RECORD_STORAGE_RECORD_RESULT, 4, bSuccess);
        }
        break;
    case OPERATION_PLAY_START:
        {
            if (STATE_PLAY_IDLE != GetRecordPlayState()
                    && STATE_PLAY_PAUSE != GetRecordPlayState())
            {
                break ;
            }

            RecordFileInfo* pInfo = GetFileInfo(pFile, iId);
            if (NULL == pInfo)
            {
                break ;
            }

            bSuccess = true;
            SetRecordPlayState(STATE_PLAYING);

            // 切换声道
            SwitchChannel(voice_GetCurrentChannel());

            // 播放文件
            RecordInterface_PlayStart(pFile);

            if (pInfo->lBreak > 0)
            {
                RecordInterface_PlaySeek(pInfo->lBreak);
                pInfo->lBreak = 0;
            }
        }
        break;
    case OPERATION_PLAY_PAUSE:
        {
            if (STATE_PLAYING != GetRecordPlayState())
            {
                break ;
            }

            unsigned int nPos = *((unsigned int *)pExtData);
            // 若正在播放铃声, GUI不能暂停, VPM会自动停止, 但状态需要更新
            if (voice_IsPlayingRing() || RecordInterface_PlayPause())
            {
                bSuccess = true;
                SetRecordPlayState(STATE_PLAY_PAUSE);
                SetPausePos(nPos, pFile);

                // 声道切回
                //SwitchChannel(CHANNEL_AUTO);
            }
        }
        break;
    case OPERATION_PLAY_RESUME:
        {
            RESUME_TYPE eResumeType = *((RESUME_TYPE *)pExtData);
            if (RESUME_GENERAL == eResumeType)
            {
                bSuccess = true;
                SetRecordPlayState(STATE_PLAYING);
                SetPausePos(0, pFile);

                SwitchChannel(voice_GetCurrentChannel());
                RecordInterface_PlayResume();
            }
            else if (RESUME_PLAY_SEEK == eResumeType)
            {
                bSuccess = DoOperation(OPERATION_PLAY_START, pFile, pExtData, iId);
            }
        }
        break;
    case OPERATION_PLAY_STOP:
        {
            if (STATE_PLAYING != GetRecordPlayState()
                    && STATE_PLAY_PAUSE != GetRecordPlayState())
            {
                break ;
            }

            unsigned int nPos = 0;
            if (NULL != pExtData)
            {
                nPos = *((unsigned int *)pExtData);
            }

            // 若正在播放铃声, GUI不能停止, VPM会自动停止, 但状态需要更新
            if (voice_IsPlayingRing() || RecordInterface_PlayStop())
            {
                bSuccess = true;
                SetRecordPlayState(STATE_PLAY_IDLE);
                SetPausePos(nPos, pFile);

                // 声道切回
                SwitchChannel(CHANNEL_AUTO);
            }
        }
        break;
    case OPERATION_PLAY_POSITION:
        {
            unsigned int nPos = *((unsigned int *)pExtData);

            if (RecordInterface_PlaySeek(nPos))
            {
                bSuccess = true;
                SetPausePos(nPos, pFile);
            }
        }
        break;
    default:
        {
            bSuccess = CBaseRecord::DoOperation(eOprType, pFile, pExtData, iId);
        }
        break;
    }

    return bSuccess;
}

// 消息处理
bool CAudioRecord::ProcessMsg(msgObject & msg)
{
    bool bHandle = true;
    switch (msg.message)
    {
    case HOTPLUG_USB_STORAGE_STATE_CHANGING:
        {
            // usb 存储设备连接上, 则去获取文件列表信息
            if (1 == msg.wParam)
            {
                RequestLoadFileList(msg.lParam);
            }

            bHandle = false;
        }
        break;
    case RECORD_STORAGE_STATE_CHANGE:
        {
            // 只处理断开设备
            if (0 != msg.wParam)
            {
                return false;
            }

            if (STATE_RECORDING == GetRecordState()
                    || STATE_RECORD_PAUSE == GetRecordState()
                    || STATE_RECORD_AUTO_PAUSE == GetRecordState())
            {
                DoOperation(OPERATION_RECORD_STOP);
            }
            else if (STATE_PLAYING == GetRecordPlayState()
                     || STATE_PLAY_PAUSE == GetRecordPlayState())
            {
                DoOperation(OPERATION_PLAY_STOP);
            }
        }
        break;
    case HOTPLUG_USB_STORAGE_VOLUME_CHANGE:
        {
            StorageInfo sVolumeInfo;
            _RecordManager.GetRecordVolumeInfo(sVolumeInfo, msg.wParam);

            // 容量临界值
            long lWarningTime = 0;
            long lAvailableRecordTime = GetAvailableTime(sVolumeInfo.m_ullFree);
            int iLastWarnLev = m_iWarningLevel;
            TransTimeToWarningInfo(lAvailableRecordTime, m_iWarningLevel, lWarningTime);

            // 没有在录制中, 不对外通知
            if (STATE_RECORDING != GetRecordState())
            {
                return false;
            }

            // 已不可录制
            if (lAvailableRecordTime <= 0)
            {
                int iRet = 2;
                DoOperation(OPERATION_RECORD_STOP, m_strRecordFile.c_str(), (void *)&iRet);
                etl_NotifyApp(false, RECORD_STORAGE_SPACE_NOTENOUGH, 0, 0);
                return true;
            }

            if (m_iWarningLevel < iLastWarnLev)
            {
                return true;
            }

            if (m_iWarningLevel != iLastWarnLev)
            {
                etl_NotifyApp(false, RECORD_STORAGE_VOLUME_WARNING, lWarningTime, 0);
            }
        }
        break;
    case IPVP_MSG_RECORD_FAIL:
        {
            SetRecordState(STATE_IDLE);
            m_eScene = RECORD_SCENE_NONE;
            m_strRecordFile = "";
            m_timepiece.StopTimer();

            //发送录制失败消息
            etl_NotifyApp(false, RECORD_STORAGE_RECORD_RESULT, 0, 0);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    if (!bHandle)
    {
        CBaseRecord::ProcessMsg(msg);
    }

    return true;
}

// 空间转换成时长
long CAudioRecord::GetAvailableTime(unsigned long long dSize)
{
    double dDivider = (PT_T49 == devicelib_GetPhoneType() ? KB(7.094) : 32000);
    long lRet = dSize / dDivider - 120;
    USBDEVICE_INFO("AudioRecord::GetAvailableTime[%lld] lRet[%ld]", dSize, lRet);
    return (lRet < 0 ? 0 : lRet);
}

void CAudioRecord::SetPausePos(int iPausePos, const char * pFile)
{
    RecordFileInfo * pInfo = GetFileInfo(pFile);
    if (NULL != pInfo)
    {
        pInfo->lBreak = iPausePos;
    }
}

// 设置状态
void CAudioRecord::SetRecordState(RECORD_STATE eState)
{
    m_eRecordState = eState;
    bool bRegister = (STATE_PLAYING == eState || STATE_PLAY_PAUSE == eState);
    voice_InitUpdateChannelInfoCallBack(CAudioRecord::UpdateChannelInfo, bRegister);
}

void CAudioRecord::UpdateChannelInfo(CHANNEL_MODE & eMode)
{
    CHANNEL_MODE eTmp = SwitchChannel(eMode, false);
    eMode = (CHANNEL_NULL == eTmp) ? eMode : eTmp;
}

// 设置播放状态
void CAudioRecord::SetRecordPlayState(RECORD_STATE eState)
{
    m_eRecordPlayState = eState;
}

void CAudioRecord::ProcessIdleRecord(OPERATION_TYPE eOprType)
{
    if (m_eScene != RECORD_SCENE_IDLE)
    {
        return;
    }

    APP_INFO("ProcessIdleRecord eOprType[%d]", eOprType);
    switch (eOprType)
    {
    case OPERATION_RECORD_START:
        {
            ipvp_speech_t voiceParam;
            memset(&voiceParam, 0, sizeof(ipvp_speech_t));

            voiceParam.audio.flag = IPVP_ADDR_FL_SENDONLY;

            /*address*/
            voiceParam.audio.addr_rtp.disable = 0;
            voiceParam.audio.addr_rtp.af_type = AF_INET;
            voiceParam.audio.addr_rtp.src.sa4.sin_family = AF_INET;
            voiceParam.audio.addr_rtp.src.sa4.sin_port = 2928;
            inet_pton(AF_INET, "127.0.0.1", &voiceParam.audio.addr_rtp.src.sa4.sin_addr);
            voiceParam.audio.addr_rtp.dst.sa4.sin_family = AF_INET;
            voiceParam.audio.addr_rtp.dst.sa4.sin_port = 2928;
            inet_pton(AF_INET, "127.0.0.1", &voiceParam.audio.addr_rtp.dst.sa4.sin_addr);

            voiceParam.audio.addr_rtcp.disable = 1;

            /*codec*/
            strncpy(voiceParam.audio.codec_name, "PCMU", IPVP_MAX_CODECNAME);
            voiceParam.audio.codec_ptype = 0;
            voiceParam.audio.codec_clock = 8000;
            voiceParam.audio.event_ptype = 101;
            voiceParam.audio.event_clock = 8000;
            voiceParam.audio.ptime = 20;

            voiceParam.mode = IPVP_FL_VAM_AUDIO;

            voice_StartTalk(-1, voiceParam);

            SwitchChannel(voice_GetCurrentChannel());
        }
        break;
    case OPERATION_RECORD_STOP:
        {
            voice_StopTalk(-1);
            SwitchChannel(CHANNEL_AUTO);
        }
        break;
    case OPERATION_RECORD_RESUME:
        {
            if (!talklogic_SessionExist())
            {
                SwitchChannel(voice_GetCurrentChannel());
            }
        }
        break;
    default:
        break;
    }

}

#endif // IF_USB_RECORD
