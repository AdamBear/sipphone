#ifdef IF_SUPPORT_BLUETOOTH
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "record/include/recordmessagedefine.h"
#include "voice/include/modvoice.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "bluetooth_inc.h"

IMPLEMENT_GETINSTANCE(CBtA2dp)

// 处理消息
LRESULT OnA2dpMsg(msgObject& msg)
{
    if (!_BTA2DP.IsA2dpConnected())
    {
        return false;
    }

    int iPlayAction = -1;
    switch (msg.message)
    {
    case TALK_MSG_SESSION_FOUCS_CHANGE:
        break;
    case RECORD_STORAGE_RECORD_RESULT:
        // 开始录制/恢复录制/开始播放/恢复播放则暂停播放
        /*if ((RECORD_RESULT_STARTRECORD == msg.wParam
            || RECORD_RESULT_RESUMERECORD == msg.wParam
            || RECORD_RESULT_STARTPLAY == msg.wParam
            || RECORD_RESULT_RESUMEPLAY == msg.wParam)
            && 1 == msg.lParam)
        {
            iPlayAction = A2DP_PLAY_STOP;
        }*/
        // 停止录制/暂停录制/停止播放/暂停播放且处于暂停状态、无通话则开始播放
        //else if ((RECORD_RESULT_STOPRECORD == msg.wParam
        //  || RECORD_RESULT_RESUMERECORD == msg.wParam
        //  || RECORD_RESULT_STOPPLAY == msg.wParam
        //  || RECORD_RESULT_PAUSEPLAY == msg.wParam)
        //  && 1 == msg.lParam && _BTA2DP.IsPlayPaused()
        //  && !talklogic_SessionExist()/* 通话中也存在录音操作 */)
        //{
        //  iPlayAction = A2DP_PLAY_RESUME;
        //}
        break;
    case TALK_MSG_TALK_HIDE:
        if (0 == msg.lParam
                && talklogic_SessionExist())
        {
            WIRE_INFO("Talk session hide resume.");
            iPlayAction = _BTA2DP.IsPlaying() ? A2DP_PLAY_PAUSE : -1;
        }
    default:
        break;
    }

    // 处理操作
    _BTA2DP.PlayOperation(iPlayAction);

    return true;
}

CBtA2dp::CBtA2dp()
    : m_ulHandle(BT_DEVICE_HANDLE_INVAILED),
      m_bConnected(false)
{
    SingleMsgReg(TALK_MSG_SESSION_FOUCS_CHANGE, OnA2dpMsg);
    SingleMsgReg(RECORD_STORAGE_RECORD_RESULT, OnA2dpMsg);
    SingleMsgReg(TALK_MSG_TALK_HIDE, OnA2dpMsg);
}

CBtA2dp::~CBtA2dp()
{
    SingleMsgUnReg(TALK_MSG_SESSION_FOUCS_CHANGE, OnA2dpMsg);
    SingleMsgUnReg(RECORD_STORAGE_RECORD_RESULT, OnA2dpMsg);
    SingleMsgUnReg(TALK_MSG_TALK_HIDE, OnA2dpMsg);
}

LRESULT CBtA2dp::OnA2dpEvent(msgObject& msg)
{
    const btkit_msg_t* pMsg = (const btkit_msg_t*)msg.GetExtraData();
    if (NULL == pMsg)
    {
        return false;
    }

    bool bHandle = true;
    switch (pMsg->bt_event)
    {
    case BT_DEV_PROFILE_CONNECTED:
        {
            InitA2dp(pMsg->bt_callback_param.pf_info.dev_hnd);
        }
        break;
    case BT_DEV_PROFILE_CONNECT_FAIL:
    case BT_DEV_PROFILE_DISCONNECTED:
        {
            ExitA2dp();
        }
        break;
    case BT_A2DP_PLAY_START:
        {
            PlayOperation(talklogic_SessionExist() ?
                          A2DP_PLAY_PAUSE : A2DP_PLAY_START);
        }
        break;
    case BT_A2DP_PLAY_STOP:
        {
            PlayOperation(A2DP_PLAY_STOP);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

// 是否a2dp消息
bool CBtA2dp::IsA2dpMsg(unsigned long ulMsg)
{
    return BT_A2DP_PLAY_START == ulMsg
           || BT_A2DP_PLAY_STOP == ulMsg;
}

// 初始化
bool CBtA2dp::InitA2dp(unsigned long ulHandle)
{
    // 更新a2dp开关状态
    // 所有profile开关都为关, 底层默认打开开关, 并连接 --- 需求
    _BTData.UpdateProfilesActivity(ulHandle, PROFILE_A2DP);

    if (!_BTDriver.IsSupportProfile(ulHandle, PROFILE_A2DP)
            || !_BTData.GetProfileEnable(ulHandle, PROFILE_A2DP))
    {
        WIRE_WARN("[A2dp] Device a2dp not supported or disabled");
        return false;
    }

    if (IsA2dpConnected())
    {
        return false;
    }

    WIRE_INFO("[A2dp] Init start");

    m_ulHandle = ulHandle;

    SetA2dpConnect(true);

    // 对外通知a2dp已连接
    etl_NotifyApp(FALSE, BLUETOOTH_MSG_A2DP_STATE_CHANGE, 1, 0);

    return _BTData.SetDevA2dpState(ulHandle, LOGIC_A2DP_STATE_CONNECTED);
}

// 注销
bool CBtA2dp::ExitA2dp()
{
    if (!IsA2dpConnected())
    {
        return false;
    }

    WIRE_INFO("[A2dp] Exit start");

    if (IsPlaying() || IsPlayPaused())
    {
        PlayOperation(A2DP_PLAY_STOP);
    }

    SetA2dpConnect(false);
    _BTData.SetDevA2dpState(m_ulHandle, LOGIC_A2DP_STATE_DISCONNECTED);

    // 对外通知a2dp退出
    etl_NotifyApp(FALSE, BLUETOOTH_MSG_A2DP_STATE_CHANGE, 0, 0);

    m_ulHandle = BT_DEVICE_HANDLE_INVAILED;

    return true;
}

// 播放操作
bool CBtA2dp::PlayOperation(int iOperate)
{
    bool bHandle = false;
    switch (iOperate)
    {
    case A2DP_PLAY_START:
    case A2DP_PLAY_RESUME:
        bHandle = StartPlay(A2DP_PLAY_RESUME == iOperate);
        break;
    case A2DP_PLAY_PAUSE:
    case A2DP_PLAY_STOP:
        bHandle = StopPlay(A2DP_PLAY_PAUSE == iOperate);
        break;
    default:
        break;
    }

    return bHandle;
}

// 开始播放
bool CBtA2dp::StartPlay(bool bResume /*= false*/)
{
    bool bHandle = false;

    if (IsPlayStoped() || IsPlayPaused())
    {
        etl_NotifyApp(true, BLUETOOTH_MSG_A2DP_PLAY_ACTION,
                      !bResume ? A2DP_PLAY_START : A2DP_PLAY_RESUME, 0);

//#ifndef _CP920
        voice_SwitchChannel(CHANNEL_RING);
//#endif

        // 播放前调整音量为铃声音量:VT_RING
        // http://10.2.1.199/Bug.php?BugID=121571
        int iVolume = voice_GetVolume(VT_RING);
        voice_SetVolume(iVolume, VT_RING);

        // 调用vpm播放接口
        voice_PlayA2dp(true);
    }

    bHandle = _BTData.SetDevA2dpState(m_ulHandle, LOGIC_A2DP_STATE_PLAYING);

    return bHandle;
}

// 停止播放
bool CBtA2dp::StopPlay(bool bPause /*= false*/)
{
    bool bHandle = false;

    if (IsPlaying() || IsPlayPaused())
    {
        // 调用vpm停止接口
        voice_PlayA2dp(false);

        etl_NotifyApp(false, BLUETOOTH_MSG_A2DP_PLAY_ACTION,
                      !bPause ? A2DP_PLAY_STOP : A2DP_PLAY_PAUSE, 0);
    }

    // 音乐停止播放, 声道切回idle, 避免按键音
    if (IsPlaying())
    {
        voice_SwitchChannel(CHANNEL_AUTO);
    }

    bHandle = _BTData.SetDevA2dpState(m_ulHandle,
                                      bPause ? LOGIC_A2DP_STATE_PAUSED : LOGIC_A2DP_STATE_CONNECTED);

    return bHandle;
}

// 是否正在播放
bool CBtA2dp::IsPlaying()
{
    DeviceInfo* pDevice = _BTData.GetDeviceInfoByHandle(m_ulHandle);
    if (!pDevice)
    {
        return false;
    }

    if (LOGIC_A2DP_STATE_PLAYING == pDevice->a2dpInfo.eState)
    {
        return true;
    }

    return false;
}

// 是否暂停播放
bool CBtA2dp::IsPlayPaused()
{
    DeviceInfo* pDevice = _BTData.GetDeviceInfoByHandle(m_ulHandle);
    if (!pDevice)
    {
        return false;
    }

    if (LOGIC_A2DP_STATE_PAUSED == pDevice->a2dpInfo.eState)
    {
        return true;
    }

    return false;
}

// 是否停止播放
bool CBtA2dp::IsPlayStoped()
{
    DeviceInfo* pDevice = _BTData.GetDeviceInfoByHandle(m_ulHandle);
    if (!pDevice)
    {
        return false;
    }

    if (LOGIC_A2DP_STATE_CONNECTED == pDevice->a2dpInfo.eState
            || LOGIC_A2DP_STATE_DISCONNECTED == pDevice->a2dpInfo.eState)
    {
        return true;
    }

    return false;
}

#endif // IF_SUPPORT_BLUETOOTH
