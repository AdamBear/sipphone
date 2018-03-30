#ifndef __COMMON_FUNCTIONS_H__
#define __COMMON_FUNCTIONS_H__

#include "talklogic_common.h"
#include "uiandlogic_common.h"
#include "service_ipvp.h"
#include "dsskey/include/dsskey_def.h"
#include "media/commondefine.h"
#include "service_h323_define.h"
#include "talk/fwd_dnd/include/fwddndtypes.h"

class CCallInfo;

struct LocalLogItemRecord;

// 从strURI中解析出SIP账号和SIP Server
void GetSipInfoFromURI(yl::string strURI, char* szSipName, char* szServerName);
// 存储呼叫记录时获取当前时间tick
UINT64 GetCurrentTick();
// 去掉呼出号码中的非法字符
bool RemoveInvalidChar(yl::string& strNumber);
// 获取数字键对应的字符
yl::string GetDigitKeyString(int iKey);
// 打印通话参数信息
void PrintVoice(const ipvp_speech_t& talkParam);
// 判断是否数字键
bool IsDigitKey(int iKey);

void SetTimer(int& iTimerID, const char* file, int line);
// 设置定时器
#define SetTimerEx(timerID) ::SetTimer(timerID,__FILE__,__LINE__)
// 取消定时器
bool CancelTimer(int& iTimerID);
//判断字符串中的字符是否都是0~9
bool Comm_IsAllDigit(const char* pstr);
//判断输入的IP地址是否有效
bool Comm_IsValidIP(LPCSTR lpszIpAddress);

struct SipUserInfo;
struct CallInUserInfo;
// 设置用户属性
bool Comm_SetRemoteInfo(SipUserInfo* pUserInfo, CallInUserInfo* pCallIn,
                        const char* cszSipServerName = "");
// 保存到通话记录
bool Comm_SaveToCallList(int iThisAccount, CallInUserInfo* pCallIn, int m_iCallID, int Type);
bool Comm_AddToCallList(LocalLogItemRecord& stRecord);
bool Comm_GetCallList(int iThisAccount, CallInUserInfo* pCallIn,
                      int Type, LocalLogItemRecord& sCallRecord);
// 保存Forword通话记录
bool Comm_SaveFwdCallList(int iThisAccount, CallInUserInfo* pCallIn, FWD_TYPE eFwdType);

yl::string Comm_GetCodeShowInfo(int iErrorType, LPCSTR lpDefault = "");

// 从sip消息数据中获取状态，仅用于收到DSK_MSG_CALL_ESTABLISHED消息
bool Comm_GetSipCodeState(msgObject& objMsg, int& iState);

// 判断sip消息是否是建立通话的消息
bool Comm_IsEstablishMsg(msgObject& objMsg);

bool Comm_IsCanHold(msgObject& objMsg);

// 是否是hold消息
bool Comm_IsHoldMsg(msgObject& objMsg);

// 本地hold
bool Comm_IsLocalHold(sip_channels_v20_t& sipChannelInfo);

// remote hold
bool Comm_IsRemoteHold(sip_channels_v20_t& sipChannelInfo);

// hold是否可用
bool Comm_IsCanHold(sip_channels_v20_t& sipChannelInfo);

// 将sip的通话参数转换成vpm的格式
int Comm_VoiceParam_sip2ipvp(const sip_channel_t* sip_chan, const yl::string& strUserAgent,
                             ipvp_speech_t* speech, const sip_channel_ext_t* sip_chan_v2 = NULL);

#ifdef IF_FEATURE_PSTN
// 获取VMP格式的PSTN通话参数
int Comm_VoiceParam_pstn2ipvp(int pstn_line_id, ipvp_speech_t* speech);
#endif // IF_FEATURE_PSTN

// 将sip的通话参数转换成vpm的格式:T49在兼容性上处理，导致需要使用call info的内容
int Comm_MediaParam_sip2ipvp(sip_channels_v20_t* sip_chan, const yl::string& strUserAgent,
                             ipvp_speech_t* speech,
                             VideoExtInfo* pVideoExt = NULL);

//将sip的回铃参数转换成VPM格式
int Comm_RingbackParam_sip2ipvp(sip_call_ringback_info_t* sip_ringback, ipvp_speech_t* speech,
                                VideoExtInfo* pVideoExt);

int Comm_MediaParamArray_sip2ipvp(sip_media_codec_array_t& src, const yl::string& strUserAgent,
                                  MediaStreamInfo& media);

// 从消息中获取sip数据
bool Comm_GetSipData(void* pDst, const void* pSrc, int iType);

bool Comm_GetSipChannelsData(sip_channels_v20_t& dst, msgObject& msg);

// 转换sip的bla,blf状态
BLF_STATUS_CODE Comm_MapSipBLFStatus(int iStatus);

// 转换blf的状态到bla状态
BLF_STATUS_CODE Comm_MapBLF2BLAStatus(BLF_STATUS_CODE eStatus);

bool Comm_UpdateSysTime(msgObject& objMsg);

bool ParseString(YLVector<yl::string>& veclist, const yl::string& strSrc,
                 const yl::string& filterStr);

bool GetDPickupCallOutNumber(int iDsskeyID, yl::string& strCallOutNumber,
                             const yl::string& strValue, const yl::string& strExtension, int iAccountID);
// 将sip传过来的参数codec list转换为vpm格式
int Comm_AudioCodec_sip2ipvp(AudioStreamInfo& audioInfo, ipvp_speech_t* speech);

int Comm_VideoParam_sip2ipvp(const sip_channel_t* sip_chan, ipvp_speech_t* speech,
                             VideoExtInfo* pVideoExt);

int Comm_VideoParam_sip2ipvp(const sip_channel_t* sip_chan, ipvp_video_t* video,
                             VideoExtInfo* pVideoExt);

int Comm_ShareVideo_sip2ipvp(const sip_channel_t* sip_chan, bool& bHasShareVideo,
                             ipvp_video_t* videoInfo, VideoExtInfo* pVideoExt = NULL);

bool Comm_ShareData_sip2ipvp(const sip_channel_t* sip_chan);

// 打印音视频参数
void Comm_PrintMedia(ipvp_speech_t* speech);

/* Routine之间的继承关系 */
bool Comm_IsRoutineInherits(ROUTINE_STATE base, ROUTINE_STATE herit, ROUTINE_STATE except = ROUTINE_UNKNOWN);

/* Session之间的继承关系 */
bool Comm_IsSessionInherits(SESSION_STATE base, SESSION_STATE herit);

/* Session到Routine的映射关系 */
ROUTINE_STATE Comm_SessionMap2Routine(SESSION_STATE eSession);

/* Routine类别，将相似的Routine划分到一类 */
ROUTINE_STATE Comm_GetInheritsRoutine(ROUTINE_STATE herit);

#endif //__COMMON_FUNCTIONS_H__
