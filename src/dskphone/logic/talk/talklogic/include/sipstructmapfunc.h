#ifndef _IF_SIPSTRUCT_MAP_FUNC_H_
#define _IF_SIPSTRUCT_MAP_FUNC_H_

#include <ylstl/ylstring.h>
#include "service_sip_define.h"

#include "service_h323_define.h"

// 号码的基本信息
struct BasicUserInfo
{
    yl::string strDisplay; // 显示名
    yl::string strNumber; // 号码
    yl::string strServer; // 服务器名
    yl::string strFromNumber;   // From头域号码

    yl::string strToDisplay; // 显示名
    yl::string strToNumber; // 号码
    yl::string strToServer; // 服务器名

    void Reset()
    {
        strDisplay.clear();
        strNumber.clear();
        strServer.clear();
        strFromNumber.clear();

        strToDisplay.clear();
        strToNumber.clear();
        strToServer.clear();
    }
};

// 来电号码的信息
struct CallInUserInfo
{
    int iAutoAnswer; // 自动接起
    int iBlaIndex;
    unsigned int uAutoAnswerType;
    bool bForceAutoAnswer;
    unsigned int uAutoAnswerDevice;
    bool bIsEmergency; // 是否是紧急呼叫
    bool bIsFwdEnable; // 是否允许fwd

    bool bMic;
    bool bSpeak;

    bool bVideo;

#if IF_FEATURE_LOOPBACK
    bool bIsLoopBackCall;
#endif

    yl::string strDisplay; // 显示名
    yl::string strNumber; // 号码
    yl::string strServer; // 服务器名
    yl::string strFromNumber;   //From头域号码(username)

    yl::string strToDisplay;
    yl::string strToNumber;
    yl::string strToServer;

    yl::string strFwdDisplay;
    yl::string strFwdNumber;
    yl::string strFwdServer;

    yl::string strRingLocal; //指定本地铃声
    yl::string strRingWeb;  // 指定网页配置铃声
    yl::string strRingURL;  // 服务器下载铃声

    yl::string strPhotoURL; // 指定来电显示的头像

    yl::string strDoorlineUrl;
    yl::string strDoorlineDTMF;

    yl::string strEmergFwdTarget; // 内部使用


    CallInUserInfo()
    {
        Reset();
    }

    void Reset()
    {
        iAutoAnswer = -1; // 注意：此值为0有意义intercom
        iBlaIndex = 0;
        uAutoAnswerType = 0;
        bForceAutoAnswer = false;
        uAutoAnswerDevice = 0;
        bIsEmergency = false; // 是否是紧急呼叫
        bIsFwdEnable = false; // 是否允许fwd
        bMic = false;
        bSpeak = false;
        bVideo = false;
#if IF_FEATURE_LOOPBACK
        bIsLoopBackCall = false;
#endif

        strDisplay.clear(); // 显示名
        strNumber.clear(); // 号码
        strServer.clear(); // 服务器名
        strFromNumber.clear();

        strToDisplay.clear();
        strToNumber.clear();
        strToServer.clear();

        strFwdDisplay.clear();
        strFwdNumber.clear();
        strFwdServer.clear();

        strRingLocal.clear(); //指定本地铃声
        strRingWeb.clear(); // 指定网页配置铃声
        strRingURL.clear(); // 服务器下载铃声

        strPhotoURL.clear();    // 指定来电显示的头像

        strDoorlineUrl.clear();
        strDoorlineDTMF.clear();
    }
};

struct SipReinviteInfo
{
    int iTID;
    int iType;
};

// 从sip的结构体中获取来电信息
bool MapCallIdentifyInfo_CallInInfo(sip_call_identify_info_t* pSrcInfo, CallInUserInfo& dstInfo);

// 从来电的结构体中获取用户基本信息
bool MapCallInUserInfo_BasicInfo(CallInUserInfo* pSrcInfo, BasicUserInfo& dstInfo);

// 从来电的结构体中获取fwd的信息
bool MapCallInUserInfo_FwdInfo(CallInUserInfo* pSrcInfo, BasicUserInfo& dstInfo);

// 转换用户基本信息
bool MapSipUserInfo_BasicInfo(sip_user_info_t* pSrcInfo, BasicUserInfo& dstInfo);

// 从H323的结构体中获取来电信息
bool MapH323CallInfo_CallInInfo(h323_call_info_t* pSrcInfo, CallInUserInfo& dstInfo);

// 转换用户基本信息
bool MapSipReInviteInfo(sip_reinvite_t* pSrcInfo, SipReinviteInfo& dstInfo);
#endif // _IF_SIPSTRUCT_MAP_FUNC_H_
