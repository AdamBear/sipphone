#ifndef __H323_SENDER_H__
#define __H323_SENDER_H__
#ifdef IF_FEATURE_H323

#include "singletonclassdefine.h"
#include "service_h323_define.h"
#include "media/commondefine.h"


struct CallOutInfo;

class CH323Sender
{
    DECLARE_SINGLETON_CLASS(CH323Sender)
public:
    // 处理定时器
    bool OnTimer(UINT uTimeID);

    // 初始化
    bool InitH323();

    // 启动H323
    int StartH323();

    // 停止H323
    int StopH323();

    // 根据开关，开启或者停止H323服务
    int SwitchH323();

    bool IsSupportVideo();

    // 配置视频能力
    int SetOption(int option, void * value, unsigned int size);

    int QueryLineState(int iAccountID);

    int RequestCallID();

    // 接起来电，eType代表接起方式
    bool AcceptCall(int iAccountID, int iCallId, int iType, bool bSupportH264, bool bMainStreamLimit);

    // 拒接来电
    bool RefuseCall(int iAccountID, int iCallId, int iRefuseCode);

    // 邀请通话目标
    bool CallInvite(CallOutInfo * pCallInfo, bool bMainStreamLimit);

    // 通话挂断
    bool CallHangUp(int iAccountID, int iCallId, int lParam = 0);

    // 设置通话Hold，bHold代表是否Hold对方，bEnableQCIF为Video通话是否采用QCIF进行协商
    bool SetHold(int iAccountID, int iCallId, bool bHold, bool bNeedNego);

    // 给Sip发送DTMF按键消息,iKey为按键键值
    bool SendDTMF(int iAccountID, int iCallID, int iKey);

    // 拒接来电
    bool MuteCall(int iAccountID, int iCallId, bool bMute);

    // 来电时响应SIP
    bool ResponseIncoming(int iAccountID, int iCallID, int iResponseCode);

    // 请求对方发送关键帧
    bool ReqKeyframe(int iAccountID, int iCallID, bool bShare);

    // 请求对方发送关键帧
    bool ReqBitrate(int iAccountID, int iCallID, int iBandwidth, bool bShare);

    int SetCallMedia(int iAccountID, int iCallID, bool bVideo);

    // 通话协商
    bool CallNegoH264(int iAccountID, int iCallId, const VideoSizeInfo & videoSize,
                      const VideoSizeInfo & extvideoSize, int iType, bool bNegoNow);

    // Type ：1为主流， 2为辅流
    bool SetVideoH264Param(int iAccountID, int iCallId, const VideoSizeInfo & videoSize,
                           const VideoSizeInfo & extvideoSize, int iType);

    yl::string GetUserAgent(int iAccountID, int iCallId);

private:
    // 延迟启动service的定时器:解决同时重启造成服务器负载大
    int m_iStartServiceTimer;

    // 等待autop更新完成
    int m_iWaitAutopTimer;

};

#define _H323Sender GET_SINGLETON_INSTANCE(CH323Sender)

#endif //#ifdef IF_FEATURE_H323
#endif // __H323_SENDER_H__
