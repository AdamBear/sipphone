#include "talkasync_inc.h"

bool talksync_OnIncomingCall(int iCallID, int iAccountID, CallInUserInfo& userInfo)
{
    TALK_INFO("CallPic callid[%d] acc[%d] photo[%s] num[%s] local[%s] web[%s] url[%s]",
              iCallID, iAccountID, userInfo.strPhotoURL.c_str(), userInfo.strNumber.c_str(),
              userInfo.strRingLocal.c_str(), userInfo.strRingWeb.c_str(), userInfo.strRingURL.c_str());

    /* 下载头像 */
    if (!userInfo.strPhotoURL.empty())
    {
        CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(iCallID);
        if (pCallInfo != NULL)
        {
            pCallInfo->SetCallerImageURL(userInfo.strPhotoURL);
        }

        talkasync_CreateUpdateCallerImage(iCallID, userInfo.strPhotoURL);
    }

    yl::string strPath;
    int iStatus = TALK_RING_LS_LOADED;
    int iRingIndex = 0;

    /* 获取铃声 */
    bool bRet = talksync_GetIncomingCallRing(iCallID, iAccountID, &userInfo,
                strPath, iStatus, iRingIndex);

    TALK_INFO("CallRing callid[%d] path[%s] load[%d] index[%d] ret[%d]",
              iCallID, strPath.c_str(), iStatus, iRingIndex, bRet);

    int iSessionID = talklogic_GetSessionIdByCallID(iCallID, false);
    if (iSessionID != INVALID_SESSION_ID)
    {
        talklogic_UIEvent(iSessionID, TALK_ACTION_PLAY_TONE, iStatus, iRingIndex, (void*)strPath.c_str());
    }

#if IF_BUG_38979
    yl::string strContactPattern = configParser_GetConfigString(kszContactReplacePattern);
    yl::string strIncomePattern = configParser_GetConfigString(kszIncomeCallRelacePattern);
    if (!strContactPattern.empty()
            && !strIncomePattern.empty())
    {
        //调用异步接口去匹配联系人
        talkasync_GetCotactNameByNumber(iCallID, userInfo.strNumber);
    }
#endif

    return true;
}

bool talksync_GetIncomingCallRing(int iCallId, int iAccount, CallInUserInfo* pCallData,
                                  yl::string& strPath, int& iStatus, int& iRingIndex)
{
    /*紧急呼叫铃声>个人铃声>群组铃声>sip参数指定铃声>账号铃声>默认铃声*/
    // step0:先获取紧急呼叫铃声
    if (pCallData == NULL)
    {
        return false;
    }

    if (pCallData->bIsEmergency)//紧急呼叫
    {
        strPath = commonUnit_SearchRingPath(
                      configParser_GetConfigString(kszEmergencyRing), "");

        // 如果存在，则不再继续处理
        if (pathFileExist(strPath.c_str()))
        {
            return true;
        }
    }

    // step1:获取联系人中对应的铃声信息
    if (talkhelp_GetContractRing(strPath, pCallData->strNumber.c_str()))
    {
        return true;
    }

    // step2:判断sip参数指定的铃声
    if (CRingHelpImpl::GetRingFromSIPInfo(iCallId, iAccount, pCallData, strPath, iStatus, iRingIndex))
    {
        return true;
    }

    // step3:如果sip指定铃声不存在，则判断账号指定铃声是否存在
    if (CRingHelpImpl::GetAccountRing(iAccount, strPath))
    {
        return true;
    }

    // 此处兼容铃声可能为Ring1.wav或者Resource:Ring1.wav情况
    // step4:如果账号指定铃声不存在，则判断配置中的默认铃声
    return talkhelp_GetDefaultRing(strPath);
}

bool talksync_GetIncomingCallRing(int iAccount, const yl::string& strNumber, yl::string& strPath)
{
    TALK_DBG("OnRoutineChange acc[%d] num[%s]", iAccount, strNumber.c_str());
    // 播放铃声优先级：个人铃声>群组铃声>账号铃声>默认铃声
    // step1:获取联系人中对应的铃声信息
    if (strNumber.empty())
    {
        return false;
    }

    // 如果联系人中存在，则不再继续处理,目前为全路径
    if (talkhelp_GetContractRing(strPath, strNumber.c_str()))
    {
        return true;
    }

    // step2:判断账号指定铃声是否存在
    if (CRingHelpImpl::GetAccountRing(iAccount, strPath))
    {
        return true;
    }

    // 此处兼容铃声可能为Ring1.wav或者Resource:Ring1.wav情况
    // step3:如果账号指定铃声不存在，则判断配置中的默认铃声
    return talkhelp_GetDefaultRing(strPath);
}

