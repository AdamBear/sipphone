#ifndef __MOD_VOICE_H__
#define __MOD_VOICE_H__

/************************************************************************/
/* 声音通道模块对外的接口                                               */
/************************************************************************/

#include "voicecommon.h"
#include "service_ipvp_types.h"

/*******************************************************************
** 函数描述:    是否呼叫中心模式
** 参数:        无
** 返回:        TRUE: 呼叫中心模式, FALSE:非呼叫中心模式
********************************************************************/
bool voice_IsCallCenterMode();

/*******************************************************************
** 函数描述:    初始化
** 参数:        无
** 返回:        无
********************************************************************/
void voice_Init();

/*******************************************************************
** 函数描述:    摘挂机一定会更改摘挂状态
** 参数:        [in] iKey:按键键值
** 返回:        无
********************************************************************/
void voice_ChangeHandsetStatus(int iKey);

/*******************************************************************
** 函数描述:    手柄摘挂按键处理
** 参数:        [in] iKey:按键键值
** 返回:        是否处理
********************************************************************/
bool voice_OnKeyPress(int iKey);

/*******************************************************************
** 函数描述:    播放按键音
** 参数:        [in] iKey:按键键值
** 返回:        无
********************************************************************/
void voice_PlayKeyTone(int iKey, bool bPlayNow = false);

/*******************************************************************
** 函数描述:    播放信号音
** 参数:        [in] iToneType：信号音类型
**              [in] iAutoStopTime：播放信号音的时长，0表示一直播放ms
**              [in] eChannel：播放信号音所用通道
**              [in] iStrategy：预通话Tone策略(1：ignore，tone与RTP混合;
**                   2：force，丢弃RTP包，强制播放tone;
**                   3：skip，跳过tone，播放RTP)
**              [in] bVpmTimer：是否由VPM控制播放信号音时长（true：VPM控制；false：voice模块定时器控制）
** 返回:        无
********************************************************************/
void voice_PlayTone(int iToneType, int iAutoStopTime = 0, CHANNEL_MODE eChannel = CHANNEL_AUTO,
                    int iStrategy = 1, bool bVpmTimer = false);

/*******************************************************************
** 函数描述:    停止信号音
** 参数:        [in] iToneType：信号音类型
** 返回:        无
********************************************************************/
void voice_StopTone(int iToneType = -1);

/*******************************************************************
** 函数描述:    设置是否静音
** 参数:        [in] iCallID: call id
**              [in] bMute: 是否mute
** 返回:        无
********************************************************************/
void voice_SetMute(int iCallID, bool bMute);

/*******************************************************************
** 函数描述:    播放DTMF音(local)
** 参数:        [in] iKey：按键
** 返回:        无
********************************************************************/
void voice_PlayLocalDTMF(int iKey);

/*******************************************************************
** 函数描述:    发送remote DTMF
** 参数:        [in] iCallID：通话的标识
**              [in] iKey：发送DTMF的按键
** 返回:        无
********************************************************************/
void voice_SendRemoteDTMF(int iCallID, int iKey, DTMF_DIR eDtmfDir = DTMF_DIR_TOBOTH);

/*******************************************************************
** 函数描述:    播放铃声
** 参数:        [in] lpszRingFile：铃声文件名
**              [in] lpszDefaultRingFlieName：默认铃声文件名
** 返回:        无
********************************************************************/
void voice_PlayRing(LPCSTR lpszRingFile, LPCSTR lpszDefaultRingFlieName = "Ring1.wav");

/*******************************************************************
** 函数描述:    通过绝对路径播放铃声
** 参数:        [in] lpszRingPath：铃声文件路径
** 返回:        无
********************************************************************/
void voice_PlayRingFromFilePath(LPCSTR lpszRingPath);

/*******************************************************************
** 函数描述:    播放默认铃声
** 参数:        无
** 返回:        无
********************************************************************/
void voice_PlayDefaultRing();

/*******************************************************************
** 函数描述:    停止播放铃声
** 参数:        无
** 返回:        无
********************************************************************/
void voice_StopRing(bool bForceStopEHS = TRUE);

/*******************************************************************
** 函数描述:    在菜单中停止播放铃声
** 参数:        无
** 返回:        无
********************************************************************/
void voice_StopRingInMenu();

/*******************************************************************
** 函数描述:    开启通话
** 参数:        [in] iCallID：通话的标识
**              [in] pTalkParam：通话的语音参数
** 返回:        无
********************************************************************/
void voice_StartTalk(int iCallID, const ipvp_speech_t& talkParam);

/*******************************************************************
** 函数描述:    停止通话
** 参数:        [in] iCallID：通话的标识
** 返回:        无
********************************************************************/
void voice_StopTalk(int iCallID);

#ifdef IF_BT_SUPPORT_PHONE
/*******************************************************************
** 函数描述:    开启蓝牙通话
** 参数:        无
** 返回:        无
********************************************************************/
void voice_StartBtTalk();

/*******************************************************************
** 函数描述:    停止蓝牙通话
** 参数:        无
** 返回:        无
********************************************************************/
void voice_StopBtTalk();
void voice_SetBtFarMute(int iCallID, bool bFarMute);
#endif // IF_BT_SUPPORT_PHONE

/*******************************************************************
** 函数描述:    重置通话参数
** 参数:        [in] iCallID：通话的标识
**              [in] pTalkParam：通话的语音参数
** 返回:        无
********************************************************************/
void voice_ResetTalk(int iCallID, const ipvp_speech_t& talkParam);

// 设置远端静音
void voice_SetFarMute(int iCallID, const ipvp_speech_t& talkParam, bool bFarMute);

/*******************************************************************
** 函数描述:    更改通话中hold的状态
** 参数:        [in] iCallID：通话的标识
** 返回:        无
********************************************************************/
void voice_Hold(int iCallID, bool bHold = true);

/*******************************************************************
** 函数描述:    建立会议
** 参数:        [in] iCallSum：要加入会议的通话数目
**              [in] pCallIDList：通话ID的列表
** 返回:        无
********************************************************************/
void voice_StartConf(int iCallSum, int * pCallIDList, bool isVideoConf);

/*******************************************************************
** 函数描述:    停止会议
** 参数:        [in] iCallSum：要加入会议的通话数目
**              [in] pCallIDList：通话ID的列表
** 返回:        无
********************************************************************/
void voice_StopConf(int iCallSum, int * pCallIDList, bool isVideoConf);

/*******************************************************************
** 函数描述:    手动打开通道
** 参数:        [in] eMode：要调整的通道
** 返回:        无
********************************************************************/
void voice_SwitchChannel(CHANNEL_MODE eMode = CHANNEL_AUTO);

/*******************************************************************
** 函数描述:    获取当前通道类型
** 参数:        无
** 返回:        当前通道类型
********************************************************************/
CHANNEL_MODE voice_GetCurrentChannel();

/*******************************************************************
** 函数描述:       设置当前通道类型
** 参数:        通道类型
** 返回:
********************************************************************/
void voice_SetCurrentChannel(CHANNEL_MODE eMode, bool bNotify = false);

/*******************************************************************
** 函数描述:    重置声音通道
** 参数:        无
** 返回:        无
********************************************************************/
void voice_ResetChannel();

/*******************************************************************
** 函数描述:    是否正在播放铃声
** 参数:        无
** 返回:        TRUE正在播放 FALSE未播放
********************************************************************/
bool voice_IsPlayingRing();

/*******************************************************************
** 函数描述:    是否耳麦响铃
** 参数:        无
** 返回:        TRUE耳麦响铃 FALSE免提响铃
********************************************************************/
bool voice_IsRingHeadset();

/*******************************************************************
** 函数描述:    获取手柄的状态
** 参数:        无
** 返回:        手柄是否摘起
********************************************************************/
bool voice_GetHandsetStatus();

/*******************************************************************
** 函数描述:    获取免提
** 参数:        [in] bOffHook: 手柄是否摘起
** 返回:        免提是否打开
********************************************************************/
bool voice_GetHandfreeStatus();

/*******************************************************************
** 函数描述:    获取耳麦状态
** 参数:        无
** 返回:        耳麦是否打开
********************************************************************/
bool voice_GetHeadsetStatus();

/*******************************************************************
** 函数描述:    判断输入按键是否通道键
** 参数:        [in] iKey：要处理的按键
** 返回:        输入按键是否通道键
********************************************************************/
bool voice_IsChannelKey(int iKey);

/*******************************************************************
** 函数描述:    设置指定类型及大小的音量
** 参数:        [in] iVolume：要设置的音量值
**              [in] eType：要设置音量的通道类型
**              [in] bWriteToFile：是否将音量值写入配置文件
** 返回:        无
********************************************************************/
void voice_SetVolume(int iVolume, VOLUME_TYPE eType = VT_AUTO, bool bWriteToFile = FALSE);

/*******************************************************************
** 函数描述:    获取指定类型的音量
** 参数:        [in] eType：要获取音量的通道类型
** 返回:        音量值
********************************************************************/
int voice_GetVolume(VOLUME_TYPE eType = VT_AUTO);

/*******************************************************************
** 函数描述:        查询EHS是否接上
** 参数:
** 返回:        TRUE:已经接上EHS,FALSE:没接上EHS
********************************************************************/
bool voice_IsEHSInserted();

/*******************************************************************
** 函数描述:    获取EHS在EXP模块中的序号
** 参数:        无
** 返回:         EHS在EXP模块中的序号
********************************************************************/
int voice_GetEHSIndex();

/*******************************************************************
** 函数描述:    调整EHS的状态，如果耳麦打开则打开，否则关闭
** 参数:        无
** 返回:        无
********************************************************************/
void voice_SwitchEHSStatus();

/*******************************************************************
** 函数描述:    关闭无线耳麦
** 参数:        无
** 返回:        无
********************************************************************/
void voice_CloseEHS();

/*******************************************************************
** 函数描述:    开启无线耳麦
** 参数:        无
** 返回:        无
********************************************************************/
void voice_OpenEHS();

/*******************************************************************
** 函数描述:    在menu菜单中播放dtmf
** 参数:        iKeyCode: 按键值
** 返回:        TRUE:播放成功,FALSE:播放失败
********************************************************************/
bool voice_PlayDTMFInMenu(int iKeyCode, bool bPlayNow = false);

/*******************************************************************
** 函数描述:    处理通道按键权限
** 参数:     iKey 按键
** 返回:     TRUE:成功,FALSE:失败
********************************************************************/
bool voice_ProcessChannelKeyAccess(int iKey);

#ifdef IF_SUPPORT_BLUETOOTH
/*******************************************************************
** 函数描述:      蓝牙模式是否可用
** 参数:       无
** 返回:       true是 false否
********************************************************************/
bool voice_IsBTChannelAvaliable();

/*******************************************************************
** 函数描述:    是否是蓝牙模式
** 参数:        无
** 返回:        true是 false否
********************************************************************/
bool voice_IsBluetoothMode();

/*******************************************************************
** 函数描述:    开启蓝牙通道
** 参数:        无
** 返回:        无
********************************************************************/
void voice_OpenBluetoothChannel();

/*******************************************************************
** 函数描述:    关闭蓝牙通道
** 参数:        无
** 返回:        无
********************************************************************/
void voice_CloseBluetoothChannel();
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_USB_AUDIO
/*******************************************************************
** 函数描述:    是否是usb耳机通道
** 参数:        无
** 返回:        无
********************************************************************/
bool voice_IsUSBHeadsetMode();

/*******************************************************************
** 函数描述:    usb耳机通道是否可用
** 参数:        无
** 返回:        无
********************************************************************/
bool voice_IsUSBHeadsetChannelAvaliable();
#endif // IF_SUPPORT_USB_AUDIO

#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
/*******************************************************************
** 函数描述:    是否是耳机模式
** 参数:        无
** 返回:        无
********************************************************************/
bool voice_IsHeadsetDeviceMode();

/*******************************************************************
** 函数描述:    开启耳机通道
** 参数:        无
** 返回:        无
********************************************************************/
void voice_OpenHeadsetDeviceChannel();

/*******************************************************************
** 函数描述:    切换至耳机通道
** 参数:        无
** 返回:        无
********************************************************************/
bool voice_SetHeadsetDevice(int iType, bool bOpen);
#endif //#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)

bool voice_IsTheMinVolume();

/*******************************************************************
** 函数描述:	播放远端声音(手机音乐)
** 参数:        [in] bPlay: 播放/停止
** 返回:        无
********************************************************************/
void voice_PlayA2dp(bool bPlay);

/*******************************************************************
** 函数描述:    注册/注销回调函数，主要用于通知UI刷新
** 参数:        pCallBack ：回调的函数
                bRegister ：注册/注销
** 返回:        无
********************************************************************/
void voice_InitUpdateChannelInfoCallBack(FunUpdateChannelInfo pCallBack, bool bRegister);

#ifdef IF_SUPPORT_LINE_DEVICE
/*******************************************************************
** 函数描述:    设置多模通道
** 参数:        [in] iSlaveMode：模式类型
** 返回:        无
********************************************************************/
bool voice_SetLineDeviceChannel(int iSlaveMode);
#endif // IF_SUPPORT_LINE_DEVICE

/*******************************************************************
** 函数描述:    音量是否可以改变
** 参数:        无
** 返回:        true 可以 ， false 不可以
********************************************************************/
bool voice_IsVolumeCanChange();

/*******************************************************************
** 函数描述:    重置通话音量
** 参数:        无
** 返回:        无
********************************************************************/
void voice_ResetTalkVolume();

#endif // __MOD_VOICE_H__