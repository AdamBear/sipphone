#ifndef __NEGO_HELPER_H__
#define __NEGO_HELPER_H__

#ifdef IF_SUPPORT_VIDEO

#include <ETLLib.hpp>
#include "etlmsghandler/modetlmsghandler.h"
#include "singletonclassdefine.h"
//#include "talk/talklogic/include/callinfo.h"
#include "voip/modvoip.h"
// #include "voip/sipsender.h"
// #include "voip/h323sender.h"
#include "service_h323_message.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "media/commondefine.h"

enum CaculateResult
{
    CAL_RES_OK = 0, // 不需要重协商
    CAL_RES_SEND_480P = 1 << 0, // 需要调整发送分辨率448P
    CAL_RES_SEND_720P = 1 << 1, // 需要调整发送分辨率720P
    CAL_RES_SEND_1080P = 1 << 2, // 需要调整发送分辨率1080P
    CAL_RES_RCV_448P = 1 << 3, // 需要重协商让对方发送448P
    CAL_RES_RCV_720P = 1 << 4, // 需要重协商让对方发送720P
    CAL_RES_RCV_1080P = 1 << 5, // 需要重协商让对方发送1080P
};

enum NegoAction
{
    ACT_NONE,
    // 录制通话视频
    ACT_START_RECORD,
    // 停止录制通话视频
    ACT_STOP_RECORD,
    // 建立会议
    ACT_ESTABLISH_CONF,
    // 拆分会议
    ACT_SPLIT_CONF,
    // 开启辅流
    ACT_START_EXTVIDEO,
    // 关闭辅流
    ACT_STOP_EXTVIDEO,
    // 让对方分辨率提升
    ACT_INCREASE_REMOTE,
    // 让对方分辨率下降
    ACT_REDUCE_REMOTE,
};

enum NegoState
{
    STS_NONE,
    //重协商中
    STS_NEGOING,
    // 等待重协商中，排到了就进行协商
    STS_NEGO_WAITING,
    // 排到了也不重协商，Resume时才重协商
    STS_NEGO_WAIT,
    // 协商无效，主要是针对需要多路协商都成功才OK的情况下，先有一路失败
    STS_NEGO_INVALID,
};

enum NegoResult
{
    RES_NONE,
    // 重协商提示,协商过一段时间没回来会先触发这个
    RES_NEGO_WAITING,
    // 重协商OK
    RES_NEGO_OK,
    // 重协商失败
    RES_NEGO_FAIL,
};

enum NegoFailCode
{
    // 重协商分辨率错误
    NEGO_RESOLUTION_ERROR,
    // 重协商超时
    NEGO_TIMEOUT,
};

enum NegoProcessType
{
    NEGO_NO_NEED,
    NEGO_WITHOUT_WAIT,
    NEGO_WAIT,
};

enum DeleteType
{
    DLT_WAIT,
    DLT_FIRST,
    DLT_ALL,
};

struct NegoResParam
{
    int iCallId;
    ipvp_speech_t voiceParam;
    NegoAction eAction;
    NegoResult eNegoRes;
    VideoExtInfo videoinfo;

    NegoResParam()
    {
        iCallId = -1;
        eAction = ACT_NONE;
        eNegoRes = RES_NONE;
        memset(&voiceParam, 0, sizeof(ipvp_speech_t));
        memset(&videoinfo, 0, sizeof(VideoExtInfo));
    }
};

struct CallNegoInfo
{
    int iAccountId;
    int iCallId;
    NegoState eState;
    NegoAction eAction;
    int iRes;

    CallNegoInfo()
    {
        iAccountId = -1;
        iCallId = -1;
        eState = STS_NONE;
        eAction = ACT_NONE;
        int iRes = 0;
    }
};

struct VideoNegoInfo
{
    // 视频参数
    ipvp_video_t videoInfo;

    // 视频扩展参数
    VideoExtInfo videoExt;

    // 可用视频codec参数
    VideoStreamInfo videoStream;
};

struct NegoInfo
{
    int iCallId;

    int iAccountId;

    bool bSupportRenego;

    bool bNegoNow;
};

typedef void (*FunNegoHelperCallBack)(NegoResParam & stNegoRes);
// 重协商相关处理
class CNegoHelper
{
    DECLARE_SINGLETON_CLASS(CNegoHelper)
public:

    LRESULT OnVoipMessage(msgObject & msg);

    // 根据当前情况计算操作需要调整的分辨率
    NegoProcessType CaculateCurrentAbility(int iCallId, VideoNegoInfo & pVideoInfo, NegoAction eAction,
                                           int & iRes);

    // 重协商
    bool Renegotiation(int iAccountID, int iCallId, NegoAction eAction, int iRes, int iType,
                       bool bNegoNow);

    bool DeleteCallNegoInfo(int iCallId, DeleteType eType = DLT_FIRST);

    bool Ontimer(UINT uTimerID);

    bool OnVoipMsg(msgObject msg);

    NegoState GetFirstNegoState(int iCallId);

    CallNegoInfo * GetFirstNegoInfo(int iCallId);

    CallNegoInfo * GetNegoInfoByTimer(UINT uTimerId);

    void RegisterCallBack(FunNegoHelperCallBack fun);

    void NegoCallBack(NegoResParam & negoParam);

    bool ExistAction(NegoAction eAction);

    bool RemoveAction(NegoAction eAction);

    bool ExistNego(int iCallId);

    bool StartFirstNego(int iCallId, int iAccountId);
private:

    LRESULT ProcessNegoBack(int iCallId, sip_channels_v20_t & sipChannelInfo);

#ifdef IF_FEATURE_H323
    LRESULT ProcessNegoBack(int iCallId, h323_channel_t * pChannelInfo);
#endif  // IF_FEATURE_H323

    LRESULT ProcessRenegoResult(int iCallId, ipvp_speech_t & voiceParam, VideoExtInfo & ExtInfo,
                                MediaStreamInfo & StreamInfo);

    bool AddWaitingAction(CallNegoInfo & negoInfo);

    LRESULT OnTimer(UINT iTimerID);

    bool SetRenegoTimer(UINT iTimerID);

    void SetNegoActionState(NegoAction eAction, NegoState eState);

    VideoSizeInfo GetVideoSize(int iRes);

private:

    YLList<CallNegoInfo> listCallNegoInfo;

    int m_iRenegoTime;

    int m_iShowTipTimer;

    FunNegoHelperCallBack m_CallBackFun;
};

// 获取全局唯一的Negohelper管理对象
#define _Negohelper GET_SINGLETON_INSTANCE(CNegoHelper)
#endif //#ifdef IF_SUPPORT_VIDEO

#endif //__NEGO_HELPER_H__
