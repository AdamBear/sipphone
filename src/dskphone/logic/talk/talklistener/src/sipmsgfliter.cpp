#include "talklistener_inc.h"

// 打印sip信息的函数声明
static void PrintSImsg(msgObject& msg)
{
    TALK_INFO("[MSG:SIP==>Talklogic] message=[%s][%#x] wParam=[%d] lParam=[%d]",
              GetETLMSGName(msg.message), msg.message, msg.wParam, msg.lParam);
}

CSipMsgFliter::CSipMsgFliter()
    : LSN_CLS_LOG_NAME
{
    RegETLMsg(SIP_CALL_START, SIP_CALL_CODEC_LIST);
    RegETLMsg(SIP_CALL_TRANSFERED_RESULT, SIP_CALL_EXT_INFO);
    RegETLMsg(SIP_CMD_CALL_OUT, SIP_CMD_CALL_CONFERENCE);

    RegETLMsg(SIP_CALL_REINVITE_RECIEVE);
    RegETLMsg(SIP_CALL_REINVITE_FAIL);
    RegETLMsg(DSK_MSG_REDIRECTCALL);
    RegETLMsg(DSK_MSG_GETWORKSTATUS);
    RegETLMsg(DSK_MSG_HANG_UP);

    RegETLMsg(IPVP_MSG_AUDIO_SRTP_REINVITE);
    RegETLMsg(IPVP_MSG_AUDIO_FIRST_RTP);
    RegETLMsg(IPVP_MSG_AUDIO_DEAD);

#ifdef IF_FEATURE_PSTN
    RegETLMsg(PSTN_MSG_INCOMING_CALL, PSTN_MSG_DIAL_RESULT);
    RegETLMsg(ACCOUNT_PSTN_CREATED, ACCOUNT_PSTN_REMOVED);
    RegETLMsg(PSTN_MSG_FLASH_RESULT);
#endif // IF_FEATURE_PSTN
#ifdef IF_BTTALK_UPDATECALLINFO
    RegETLMsg(BLUETOOTH_CONTACT_RELOAD);
    RegETLMsg(BLUETOOTH_CONTACT_DOWNLOAD_RESULT);
#endif

}

bool CSipMsgFliter::OnMessage(msgObject & msg)
{
    PrintSImsg(msg);

    int iCallID = msg.lParam;

    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(iCallID);

    switch (msg.message)
    {
    case SIP_CMD_CALL_ACCEPT:
        {
            //将SIP_CMD_CALL_ACCEPT当作Intercom通话则不做处理,由RingRoutine进行处理
            if (configParser_GetConfigInt(kszCMDAnswerAsIntercom) == 1)
            {
                break;
            }
            else
            {
                // wparam是line id  lparam是call id,传入callid，解决不能接起第2路通话
                if (talklogic_AcceptIncomingCall(iCallID))
                {
#if !IF_BUG_28019
                    // 要用免提应答
                    voice_SwitchChannel(CHANNEL_HANDFREE);
#endif
                }
            }

            return true;
        }
        break;
    case SIP_CMD_CALL_CONFERENCE:
        {
            //添加到会议中
            if (talklogic_GetSessionNum() == 2)
            {
                // http://192.168.1.99/Bug.php?BugID=6806
                int iSessionID = talklogic_GetSessionIdByCallID(iCallID);
                return talklogic_UIEvent(iSessionID, TALK_ACTION_CONF);
            }
        }
        break;
    case SIP_CALL_ESTABLISH:
        {
            sip_channels_v20_t sipChannelInfo;
            Comm_GetSipChannelsData(sipChannelInfo, msg);

            if (pCallInfo != NULL)
            {
                pCallInfo->SetSipChannels(sipChannelInfo);
            }
        }
        break;
    case DSK_MSG_GETWORKSTATUS:
        {
            // 网页请求通话状态返回结果
            // msgkit下如果GUI死掉，不会响应这个消息，msgkit会返回0，导致认为是talking状态，这里更改返回值,以前是反过来的
            int iRet = (talklogic_SessionExist()) ? 1 : 0;
            msg.ReplyMessage(iRet);
            return iRet;
        }
        break;
    case DSK_MSG_HANG_UP:
        {
            if (msg.wParam == 1)
            {
                talklogic_ExitAllSession();
            }
            else
            {
                int iSessionID = talklogic_GetFocusedSessionID();
                if (iSessionID == INVALID_SESSION_ID)
                {
                    return false;
                }
                // Hold的时候不能挂断,显示transfer失败时只能退出提示,这些判断交给具体routine
                talklogic_UIEvent(iSessionID, TALK_ACTION_CANCEL, TAT_DSK_EXTERN);
            }
        }
        break;
    case DSK_MSG_REDIRECTCALL: //网页下发的消息处理
        {
            bool bAllowThisMsg = accessCtr_Process(msg);
            if (!bAllowThisMsg)
            {
                return false;
            }

            //redirectcall=303&call_id=1234
            LPCSTR lpMsg = (LPCSTR)msg.GetExtraData();
            if (NULL == lpMsg || strlen(lpMsg) == 0)
            {
                return false;
            }

            char lpNewTarget[128] = { 0 };
            char lpCallID[128] = { 0 };

            if (sscanf(lpMsg, "redirectcall=%[^&]&call_id=%s", lpNewTarget, lpCallID) != 2)
            {
                return false;
            }

            return talklogic_Redirect(atoi(lpCallID), lpNewTarget);
        }
        break;
    case IPVP_MSG_AUDIO_SRTP_REINVITE:
        {
            // 请求重新协商srtp key
            TALK_INFO("IPVP_MSG_AUDIO_SRTP_REINVITE[%d][%d]", msg.wParam, msg.lParam);
            if (1 != msg.lParam)
            {
                break;
            }

            CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(iCallID);
            if (NULL != pCallInfo)
            {
                modVoip_SrtpLifetimeReinvite(pCallInfo->GetAccountID(), iCallID);
            }

            return false;
        }
        break;
    case SIP_CMD_CALL_OUT:
        {
            // http://10.2.1.199/Bug.php?BugID=21404
            // http://10.2.1.199/Bug.php?BugID=57636
            sip_ext_params_t* pParams = (sip_ext_params_t*)msg.GetExtraData();
            if (pParams == NULL)
            {
                return false;
            }

            char* lpNumber = (char*)sip_ext_params_get(pParams, SIP_MAKE_CALL_CALLEE, NULL);
            if (textEMPTY(lpNumber))
            {
                break;
            }

            talklogic_CallOut(lpNumber, "", msg.wParam);
        }
        break;
    case SIP_CALL_INFORMATION_UPDATE:   //显示Call Information
        {
            sip_call_infomation_header_t* pInfo = (sip_call_infomation_header_t*)msg.GetExtraData();
            if (NULL == pInfo || pCallInfo == NULL)
            {
                return false;
            }

            if (SIP_CALL_INFORMATION_REFERRED_BY == pInfo->type)
            {
                sip_user_info_t* pUserInfo = (sip_user_info_t*)pInfo->payload;
                if (pUserInfo != NULL)
                {
                    pCallInfo->SetRelationInfo(CR_NONE, pUserInfo->user_name);
                }

                return true;
            }
        }
        break;
#if IF_BUG_35381
    case SIP_CALL_EXT_INFO:
        {
            if (pCallInfo != NULL && (pCallInfo->m_tRemoteInfo.sSIPName.find("anonymous") != yl::string::npos))
            {
                sip_user_info_t* pInfo = (sip_user_info_t*)msg.GetExtraData();

                if (pInfo != NULL)
                {
                    TALK_INFO("SIP_CALL_EXT_INFO DisplayName[%s] SipName[%s] Server[%s]",
                              pInfo->display_name, pInfo->user_name, pInfo->server_name);

                    pCallInfo->m_tRealRemoteInfoByAnonymous.sDisplayName = pInfo->display_name;
                    pCallInfo->m_tRealRemoteInfoByAnonymous.sSIPName = pInfo->user_name;
                    pCallInfo->m_tRealRemoteInfoByAnonymous.sServerName = pInfo->server_name;
                }
            }
        }
        break;
#endif
    case SIP_CALL_RECV_FRAME_SYNC:
        {
            return etl_NotifyApp(false, TALK_MSG_VIDEO_KEY_FRAME_REQUEST, msg.lParam, IPVP_VIDEO_ID_SPEECH);
        }
        break;
    case SIP_CALL_CODEC_LIST:
        {
            // 通话建立之前会先发codec_list
            sip_media_codec_array_t* pArray = (sip_media_codec_array_t*)msg.GetExtraData();
            if (NULL == pArray)
            {
                return false;
            }

            if (pCallInfo == NULL)
            {
                break;
            }

            return Comm_MediaParamArray_sip2ipvp(*pArray, pCallInfo->m_strUserAgent, pCallInfo->m_stMediaInfo);
        }
        break;
    case SIP_CALL_REPLACED_BY_REMOTE:
        {
            sip_call_be_replace_t* pReplaceInfo = (sip_call_be_replace_t*)(msg.GetExtraData());
            if (NULL == pReplaceInfo)
            {
                return false;
            }

            if (pCallInfo == NULL)
            {
                break;
            }

            pCallInfo->m_strRelationNum = pCallInfo->m_tRemoteInfo.sSIPName;

            BasicUserInfo userInfo;
            MapSipUserInfo_BasicInfo(&pReplaceInfo->user_info, userInfo);

            pCallInfo->SetRemoteInfo(userInfo);
        }
        break;
    case SIP_CALL_FORWARDED_BY_REMOTE:
        {
            sip_call_be_forward_t* pFwdInfo = (sip_call_be_forward_t*)(msg.GetExtraData());
            if (NULL == pFwdInfo)
            {
                return false;
            }

            if (pCallInfo == NULL)
            {
                break;
            }

            // 根据配置保存的是否为源号码
            if (!configParser_GetConfigInt(kszSaveOriginalNum2History)
                    && Comm_IsRoutineInherits(ROUTINE_CONNECTING, pCallInfo->GetRoutine()))
            {
                pCallInfo->SetOriginalRemoteInfo(true);
            }
            else
            {
                // 如果历史记录保存的是源号码,则Relation取当前号码
                pCallInfo->SetRelationInfo(CR_NONE, pCallInfo->m_tRemoteInfo.sSIPName);
            }

            BasicUserInfo userInfo;
            MapSipUserInfo_BasicInfo(&pFwdInfo->user_info, userInfo);

            if (!userInfo.strDisplay.empty()
                    && configParser_GetConfigInt(kszSkipDisplayName) != 0)
            {
                //不去使用sip发送过来的display名称。http://192.168.1.99/Bug.php?BugID=21856
                userInfo.strDisplay.clear();
            }

            pCallInfo->SetRemoteInfo(userInfo);
        }
        break;
    case SIP_CALL_RINGBACK:
        {
            // 不处理回铃的消息，等待通话建立
            if (pCallInfo == NULL
                    || pCallInfo->GetRoutine() == ROUTINE_NETWORKCONF)
            {
                return false;
            }
        }
        break;
    case SIP_CALL_UPDATE_INFO: // 暂和forward相同处理
        {
            sip_user_info_t* pUserInfo = (sip_user_info_t*)(msg.GetExtraData());
            if (pUserInfo == NULL)
            {
                return false;
            }

            if (pCallInfo == NULL
                    || pCallInfo->GetRoutine() == ROUTINE_NETWORKCONF)
            {
                return false;
            }

            BasicUserInfo userInfo;
            MapSipUserInfo_BasicInfo(pUserInfo, userInfo);
            pCallInfo->SetRemoteInfo(userInfo);
        }
        break;
#ifdef IF_FEATURE_PSTN
    case ACCOUNT_PSTN_CREATED:
    case ACCOUNT_PSTN_REMOVED:
        {
            int iSessionID = talklogic_GetSessionIdByCallID(iCallID, false);
            CSessionProxy proxy(iSessionID);

            if (proxy.IsInherits(ROUTINE_DIALING))
            {
                CCallInfo* pCallInfo = proxy.GetCallInfo();
                if (pCallInfo != NULL)
                {
                    // 当前账号被删除，需要切换账号
                    pCallInfo->SetAccountID(acc_GetUsableAccount(false, ACCOUNT_TYPE_ALL));
                }

                modtklsn_emit(TALK_SIG_SESSION_INFO_CHANGED, iSessionID);
            }

            return true;
        }
        break;
#endif
    default:
        break;
    }

    return talklogic_OnSIPMsg(msg);
}
