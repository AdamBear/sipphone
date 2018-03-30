#include "sipstructmapfunc.h"

// 从sip的结构体中获取用户基本信息
bool MapCallIdentifyInfo_CallInInfo(sip_call_identify_info_t* pSrcInfo, CallInUserInfo& dstInfo)
{
    if (NULL == pSrcInfo)
    {
        return false;
    }

    dstInfo.iAutoAnswer = pSrcInfo->answer_after;
    dstInfo.iBlaIndex = pSrcInfo->sca_appearance_index;
    dstInfo.uAutoAnswerType = pSrcInfo->p_auto_answer;
    dstInfo.bForceAutoAnswer = 1 == pSrcInfo->force_auto_answer;
    dstInfo.uAutoAnswerDevice = pSrcInfo->auto_answer_device;
    dstInfo.bIsEmergency = 1 == pSrcInfo->is_emergency;

#if IF_FEATURE_LOOPBACK
    dstInfo.bIsLoopBackCall = (pSrcInfo->inc_call_type & SIP_CALL_INCOMING_TYPE_LOOPBACK) != 0;
#endif

    dstInfo.strDisplay = (char*)pSrcInfo->display_name.ptr;
    dstInfo.strNumber = (char*)pSrcInfo->user_name.ptr;
    dstInfo.strServer = (char*)pSrcInfo->server_name.ptr;
    dstInfo.strFromNumber = (char*)pSrcInfo->from_user_name.ptr;

    dstInfo.strToDisplay = (char*)pSrcInfo->to_diaplay_name.ptr;
    dstInfo.strToNumber = (char*)pSrcInfo->to_user_name.ptr;
    dstInfo.strToServer = (char*)pSrcInfo->to_server_name.ptr;


    dstInfo.strFwdDisplay = (char*)pSrcInfo->diversion_diaplay_name.ptr;
    // NFON display时需要显示line
    yl::string strLine = (char*)pSrcInfo->diversion_line.ptr;
    if (!strLine.empty())
    {
        dstInfo.strFwdDisplay = strLine + " " +  dstInfo.strFwdDisplay;
    }
    dstInfo.strFwdNumber = (char*)pSrcInfo->diversion_user_name.ptr;
    dstInfo.strFwdServer = (char*)pSrcInfo->diversion_server_name.ptr;

    dstInfo.strRingLocal = (char*)pSrcInfo->alert_local.ptr;
    dstInfo.strRingWeb = (char*)pSrcInfo->alert_web.ptr;
    dstInfo.strRingURL = (char*)pSrcInfo->alert_url.ptr;

    dstInfo.strPhotoURL = (char*)pSrcInfo->pict_info.ptr;

    dstInfo.bIsFwdEnable = !(1 == pSrcInfo->force_no_forward);

    dstInfo.bMic = 1 == pSrcInfo->enable_mic;
    dstInfo.bSpeak = 1 == pSrcInfo->enable_speaker;

    dstInfo.strDoorlineUrl = (char*)pSrcInfo->doorline_uri.ptr;
    dstInfo.strDoorlineDTMF = (char*)pSrcInfo->doorline_dtmf.ptr;

    dstInfo.bVideo = (pSrcInfo->inc_video == SIP_CALL_INCOMING_IS_VIDEO);

    return true;
}

// 从sip的结构体中获取用户基本信息
bool MapCallInUserInfo_BasicInfo(CallInUserInfo* pSrcInfo, BasicUserInfo& dstInfo)
{
    if (NULL == pSrcInfo)
    {
        return false;
    }

    dstInfo.strDisplay = pSrcInfo->strDisplay;
    dstInfo.strNumber = pSrcInfo->strNumber;
    dstInfo.strServer = pSrcInfo->strServer;
    dstInfo.strFromNumber = pSrcInfo->strFromNumber;

    dstInfo.strToDisplay = pSrcInfo->strToDisplay;
    dstInfo.strToNumber = pSrcInfo->strToNumber;
    dstInfo.strToServer = pSrcInfo->strToServer;
    return true;
}

// 从来电的结构体中获取fwd的信息
bool MapCallInUserInfo_FwdInfo(CallInUserInfo* pSrcInfo, BasicUserInfo& dstInfo)
{
    if (NULL == pSrcInfo)
    {
        return false;
    }

    dstInfo.strDisplay = pSrcInfo->strFwdDisplay;
    dstInfo.strNumber = pSrcInfo->strFwdNumber;
    dstInfo.strServer = pSrcInfo->strFwdServer;
    return true;
}

// 转换用户基本信息
bool MapSipUserInfo_BasicInfo(sip_user_info_t* pSrcInfo, BasicUserInfo& dstInfo)
{
    if (NULL == pSrcInfo)
    {
        return false;
    }

    dstInfo.strDisplay = pSrcInfo->display_name;
    dstInfo.strNumber = pSrcInfo->user_name;
    dstInfo.strServer = pSrcInfo->server_name;
    return true;
}


bool MapSipReInviteInfo(sip_reinvite_t* pSrcInfo, SipReinviteInfo& dstInfo)
{
    if (pSrcInfo == NULL)
    {
        return false;
    }

    printf("MapSipReInviteInfo tid[%d] type[%d]", pSrcInfo->tid, pSrcInfo->type);
    dstInfo.iTID = pSrcInfo->tid;
    dstInfo.iType = pSrcInfo->type;
    return true;
}

