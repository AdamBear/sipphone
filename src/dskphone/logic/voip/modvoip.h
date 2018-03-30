/************************************************************************/
/*
* 协议接口封装：sip、h323
1、负责sip server、h323 server程序的启动与停止
2、呼出号码与具体协议的转换
3、根据账号选择对应的协议接口
*/
/************************************************************************/



#ifndef __MOD_VOIP_H__
#define __MOD_VOIP_H__

#include "service_sip_define.h"
#include "account/include/account_datadefine.h"
#include "media/commondefine.h"

struct CallOutInfo;

/************************************************************************/
/* 功能  ： 初始接口，负责启动sip、h323                                    */
/* 参数说明 ：                                                          */
/* 返回值 ： 无                                                         */
/************************************************************************/
void modVoip_init();

/************************************************************************/
/* 功能  ： 停止sip、h323服务                                           */
/* 返回值 ： 无                                                         */
/************************************************************************/
void modVoip_StopService();

/************************************************************************/
/* 功能  ： 从sip、h323 server读取账号的状态                            */
/* 参数说明 ：                                                          */
/*     [in]iAccountID                                                   */
/* 返回值 ： 账号状态                                                   */
/************************************************************************/
LINE_STATE modVoip_QueryLineState(int iAccountID);

/************************************************************************/
/* 功能  ： 向sip请求生成call id                                        */
/* 参数说明 ：                                                          */
/*     无                                                              */
/* 返回值 ： 无                                                         */
/************************************************************************/
int modVoip_NewCallId();

bool modVoip_MakeCall(CallOutInfo & callInfo);

bool modVoip_AnswerCall(int iAccountID, int iCallId, int iType, bool bMainStreamLimit);

bool modVoip_HangupCall(int iAccountID, int iCallId, int lParam = 0);

bool modVoip_ResponseIncoming(int iAccountID, int iCallId, int iResponseCode);

bool modVoip_RefuseCall(int iAccountID, int iCallId, int iCode, CallInUserInfo * pUserInfo = NULL);

bool modVoip_HoldCall(int iAccountID, int iCallId, bool bHold, bool bNeedNego = false);

bool modVoip_MuteCall(int iAccountID, int iCallId, bool bMute);

#ifdef IF_SUPPORT_VIDEO
// 设置通话媒体类型：视频/音频
bool modVoip_SetCallMedia(int iAccountID, int iCallId, bool bVideo);

// 音视频切换
bool modVoip_VideoSharing(int iAccountID, int iCallID, bool bShare);

// 重协商
bool modVoip_NegoH264(int iAccountID, int iCallId, const VideoSizeInfo & videoSize, int iType,
                      bool bNegoNow, const VideoSizeInfo & extvideoSize = VideoSizeInfo(1920, 1080, 15));

// 请求关键帧
bool modVoip_RequestKeyFrame(int iAccountID, int iCallID, bool bShare);

// 请求比特率
bool modVoip_RequestBitrate(int iAccountID, int iCallID, int iBandwidth, bool bShare);

bool modVoip_IsSupportVideo();

// 重协商解hold
bool modVoip_CallNegoResume(int iAccountID, int iCallID);
#endif //#ifdef IF_SUPPORT_VIDEO

bool modVoip_SendDtmf(int iAccountID, int iCallID, int iKey);
//
bool modVoip_RequestSrtpLifetime(int iAccountID, int iCallID);

// 需要使用IP直播的云账号
bool modVoip_NeedDirectIPCloudAccount(SipAccountType eType);

//int modVoip_ReplaceAccount(int iAccountID);

/************************************************************************/
/* 功能  ： 将两路通话转接起来                                          */
/* 参数说明 ：                                                          */
/*     [in]iAccountId：所使用的帐号ID                                    */
/*     [in]iTraneeId：第二路                                            */
/*     [in]iTranerId：第一路                                            */
/*     [in]lpszTranTarget：对象                                          */
/* 返回值 ： 无                                                         */
/************************************************************************/
bool modVoip_RequestTransfer(int iAccountID, int iTraneeId, int iTranerId, LPCSTR lpszTranTarget = "");

bool modVoip_EstablishNetworkConf(int iAccountID, int iCallID, int iCount, int * pConfCallIDList,
                                  int iMask = 0);

bool modVoip_EstablishSCANetworkConf(int iAccountID, int iCallID, int iCount, int * pConfCallIDList,
                                     int iBlaIndex, int iMask = 0);

bool modVoip_AddToNetworkConf(int iAccountID, int iSrcCallID, int iAddCallID);

bool modVoip_RequestForward(int iAccountID, int iCallId, LPCSTR lpszForwardNumber,
                            int iForwardType = 0, CallInUserInfo * pUserInfo = NULL);

bool modVoip_SendOEMFlash(int iAccoutID, int iCallID);

bool modVoip_BLAHold(int iAccountID, bool bPublic, int iCallId, bool bHold, int iBLAIndex = 0);

bool modVoip_DraftBLAHold(int iAccountID, bool bPublic, int iCallId, bool bHold);

bool modVoip_StartURLRecord(int iAccountID, int iCallID, bool bStart);

bool modVoip_GetCallInfo(int iAccountID, int iCallID, sip_call_proto_info2_t * lpszData);

bool modVoip_SendReportVQSession(int iAccountID, int iCallID, LPCSTR lpszData);


int modVoip_SetOption(int option, void * value, unsigned int size, bool bSip);

bool modVoip_DraftBLASeizeLine(int iAccountID, int iCallID, bool bSeize, int iIndex);


bool modVoip_BLASeizeLine(int iAccountID, int iCallID, bool bSeize, int iIndex);


bool modVoip_SubscribCallCompletion(int iAccountID, int iCallID, BOOL bSubscribe,
                                    const char * pNumber);

bool modVoip_SendCSTAResponse(int iAccountID, int iCallID, int iTid, LPCSTR lpszData);

bool modVoip_SendCSTAAccptCall(int iAccountID, int iCallID, LPCSTR lpszData);

bool modVoip_SendAnsRequest(int iAccountID, int iCallID, LPCSTR lpszData);

bool modVoip_SendCSTARequest(int iAccountID, int iCallID, LPCSTR lpszData);

bool modVoip_FeatureKeySync(int iAccountID, int iTargetAccountID, LPCSTR lpszData);

bool modVoip_SendAcdInfoReq(int iAccountID, int iCallID, LPCSTR lpszData);

bool modVoip_ReqQueueStatus(int iAccountID, bool IsOn);

bool modVoip_ACDAction(int iAccountID, int iKeyIndex, int iType, const char * pUser = NULL,
                       const char * pPwd = NULL);

bool modVoip_ReqHoteling(int iAccountID, int iStatus, const char * pUser, const char * pPwd);

bool modVoip_CallInvite(int iAccountID, int iCallId, LPCSTR lpszFullName);

bool modVoip_SendTextMessage(int iAccountID, const char * pText, const char * pTo);

yl::string modVoip_GetUserAgent(int iAccountID, int iCallId);

// 开始或关闭FortinetRecord
bool modVoip_StartFortinetRecord(int iAccountID, int iCallID, BOOL bStart);

bool modVoip_ReInviteRespose(int iAccountID, int iCallID, int iTID, bool bAccept);

bool modVoip_SrtpLifetimeReinvite(int iAccountID, int iCallID);

#ifdef IF_FEATURE_EMERGENCY
// 挂断 紧急通话
bool modVoip_EmergencyCall_HangupReInvite(int iAccoutID, int iCallID);
#endif

//删除网络会议与会者
bool modVoip_DeleteNetConfParty(int iAccountID, int iCallID, const char* pUserId);

#endif // __MOD_VOIP_H__
