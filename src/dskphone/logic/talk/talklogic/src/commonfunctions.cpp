#include "talklogic_inc.h"

#define COMPATDEVICEINFO_PATH RESOURCE_PATH "deviceCompatible.xml"
#define DEFAULT_H460_ALIVE_TIME 25

extern bool g_bAfterInit;

// 设备兼容性信息
struct CompatDeviceInfo
{
    yl::string strName;
    //yl::string strVersion;
    int nByteOrder;
    //yl::string strKeyframe;
    bool    bSip;
    CompatDeviceInfo()
    {
        strName.clear();
        //strVersion.clear();
        nByteOrder = 0;
        bSip = false;
        //  strKeyframe.clear();
    }
};

//判断字符串中的字符是否都是0~9
bool Comm_IsAllDigit(const char* pstr)
{
    for (; *pstr; pstr++)
    {
        if (*pstr < '0' || *pstr > '9')
        {
            return false;
        }
    }
    return true;
}

// 从strURI中解析出SIP账号和SIP Server
void GetSipInfoFromURI(yl::string strURI, char* szSipName, char* szServerName)
{
    if (strURI.substr(0, 4) == "sip:")
    {
        strURI = strURI.substr(4, strURI.size() - 4);
    }

    if (strURI.find("@") != yl::string::npos)
    {
        sscanf(strURI.c_str(), "%[^@]@%s", szSipName, szServerName);
    }
    else
    {
        strcpy(szSipName, strURI.c_str());
    }
}

// 存储呼叫记录时获取当前时间tick
UINT64 GetCurrentTick()
{
    return netGetLocalTime();
}

// 去掉呼出号码中的非法字符
bool RemoveInvalidChar(yl::string& strNumber)
{
    yl::wstring strWFilter = yl::to_utf16(configParser_GetConfigString(kszCallNumFilter));
    if (strNumber.empty()
            || strWFilter.empty())
    {
        return false;
    }

    bool bHandled = false;
    //考虑中文等特殊字符，现用UNICODE编码进行筛选
    yl::wstring strWNum = yl::to_utf16(strNumber);

    //擦出在strWFilter里的字符
    yl::wstring::size_type stPos = 0;
    yl::wstring::size_type stFilterPos = 0;
    while (strWNum.size() > 0
            && stFilterPos < strWFilter.size())
    {
        stPos = 0;
        while (1)
        {
            stPos = strWNum.find_first_of(strWFilter.at(stFilterPos), stPos);
            if (stPos == yl::wstring::npos)
            {
                break;
            }

            bHandled = true;
            strWNum.erase(stPos, 1);
        }

        stFilterPos++;
    }

    strNumber = yl::to_utf8(strWNum);
    return bHandled;
}

// 获取数字键对应的字符
yl::string GetDigitKeyString(int iKey)
{
    char lpFirstChar[2] = {0, 0};
    lpFirstChar[0] = '0' + iKey - PHONE_KEY_0;
    yl::string strIME = lpFirstChar;
    return strIME;
}

// 打印通话参数信息
void PrintVoice(const ipvp_speech_t& talkParam)
{
    TALK_INFO("Voice Param:");
    TALK_INFO("Local IP Address:%s", inet_ntoa(talkParam.video.addr_rtp.src.sa4.sin_addr));
    TALK_INFO("Audio Param:");
    TALK_INFO("DTMF Payload:%d", talkParam.audio.event_ptype);
    TALK_INFO("Local IP Address:%s", inet_ntoa(talkParam.audio.addr_rtp.src.sa4.sin_addr));
    TALK_INFO("Audio Remote IP Address:%s", inet_ntoa(talkParam.audio.addr_rtp.dst.sa4.sin_addr));
    TALK_INFO("Audio Local Port:%d", talkParam.audio.addr_rtp.src.sa4.sin_port);
    TALK_INFO("Audio Bandwidth:%d", talkParam.audio.codec_clock);
    TALK_INFO("Audio Local RTCP Port:%d", talkParam.audio.addr_rtcp.src.sa4.sin_port);
    TALK_INFO("Audio Remote Port:%d", talkParam.audio.addr_rtp.dst.sa4.sin_port);
    TALK_INFO("Audio Remote RTCP Port:%d", talkParam.audio.addr_rtcp.dst.sa4.sin_port);
    TALK_INFO("Audio Codec Name:%s", talkParam.audio.codec_name);
    TALK_INFO("Audio Ptime:%d", talkParam.audio.ptime);
    TALK_INFO("Audio Codec Payload:%d", talkParam.audio.codec_ptype);
    TALK_INFO("Audio SRTP Encode Option:(%s)", talkParam.audio.enc_key);
    TALK_INFO("Audio SRTP Decode Option:(%s)", talkParam.audio.dec_key);
    TALK_INFO("Audio Send Recv:%d", talkParam.audio.flag);
}

// 判断是否数字键
// 判断是否数字键
bool IsDigitKey(int iKey)
{
    return (iKey == PHONE_KEY_POUND || iKey == PHONE_KEY_STAR
            || (iKey >= PHONE_KEY_0 && iKey <= PHONE_KEY_9)
#if IF_FEATURE_DTMF_FLASH
            || (iKey >= PHONE_KEY_A && iKey <= PHONE_KEY_E));
#else
            || (iKey >= PHONE_KEY_A && iKey <= PHONE_KEY_D));
#endif
}

// 设置定时器
void SetTimer(int& iTimerID, const char* file, int line)
{
    if (iTimerID <= DEFAULT_TIMER_VALUE)
    {
        iTimerID = DEFAULT_TIMER_VALUE;
    }

    //timerSetThreadTimer((UINT)&iTimerID, iTimerID);
    timerSetThreadTimer1((UINT) &iTimerID, iTimerID, file, line);
}

// 取消定时器
bool CancelTimer(int& iTimerID)
{
    if (iTimerID != STOP_TIMER_VALUE)
    {
        iTimerID = STOP_TIMER_VALUE;
        timerKillThreadTimer((UINT)&iTimerID);
        return true;
    }

    return false;
}


// 判断是否合法的ipv6 ip
bool Comm_IsValidIPV6(LPCSTR lpszIpAddress)
{
    return commonAPI_IsValidIPV6(lpszIpAddress);
}

//判断输入的IP地址是否有效
bool Comm_IsValidIP(LPCSTR lpszIpAddress)
{
    return commonAPI_IsValidIP(lpszIpAddress);
}

// 设置用户属性
bool Comm_SetRemoteInfo(SipUserInfo* pUserInfo, CallInUserInfo* pCallIn,
                        const char* cszSipServerName /*= ""*/)
{
    if (pUserInfo == NULL
            || pCallIn == NULL)
    {
        return false;
    }

    pUserInfo->sSIPName = pCallIn->strNumber;
    trim_both(pUserInfo->sSIPName, "\" \t");

    pUserInfo->sDisplayName = pCallIn->strDisplay;
    trim_both(pUserInfo->sDisplayName, "\" \t");
    // displayname为空用username代替
    if (pUserInfo->sDisplayName.empty())
    {
        pUserInfo->sDisplayName = pUserInfo->sSIPName;
    }

    pUserInfo->sServerName = pCallIn->strServer ;
    trim_both(pUserInfo->sServerName);

    if (pUserInfo->sServerName.empty() && cszSipServerName != NULL)
    {
        pUserInfo->sServerName = cszSipServerName;
    }
    return true;
}

// 保存到通话记录
bool Comm_SaveToCallList(int iThisAccount, CallInUserInfo* pCallIn, int iCallID, int Type)
{
    LocalLogItemRecord sCallRecord;
    if (!Comm_GetCallList(iThisAccount, pCallIn, Type, sCallRecord))
    {
        return false;
    }

    return Comm_AddToCallList(sCallRecord);
}

bool Comm_AddToCallList(LocalLogItemRecord& stRecord)
{
    if (LocalLog_AddLog(stRecord) < 0)
    {
        TALK_WARN("CallLog_AddLog SAVE FAIL!");
        return false;
    }
    return true;
}


bool Comm_GetCallList(int iThisAccount, CallInUserInfo* pCallIn, int Type, LocalLogItemRecord& sCallRecord)
{
    if (!acc_IsAccountIDValid(iThisAccount)
            || pCallIn == NULL
            || (configParser_GetCfgItemIntValue(kszAccountMissCallLog, iThisAccount) == 0
                && CT_MISSCALL == Type)
            || !g_bAfterInit)
    {
        return false;
    }

    // 通话对方的信息
    SipUserInfo     m_tRemoteUser;
    // 通过公共函数设置，以便统一
    Comm_SetRemoteInfo(&m_tRemoteUser, pCallIn, acc_GetServerName(iThisAccount).c_str());

    sCallRecord.SetLogType((CALL_TYPE)Type);

    //写入当前时间
    char szBuf[128] = { 0 };

    sprintf(szBuf, "%d", (int)netGetLocalTime());
    sCallRecord.m_strBornTick = szBuf;
    sCallRecord.m_strLocalSipName = acc_GetUsername(iThisAccount);
    sCallRecord.m_strLocalSipServer = acc_GetServerName(iThisAccount);
    sCallRecord.m_strDisplayName = m_tRemoteUser.sDisplayName;
    sCallRecord.SetNumber(m_tRemoteUser.sSIPName);
    sCallRecord.m_strRemoteSipServer = m_tRemoteUser.sServerName;
    sCallRecord.m_nLine = iThisAccount;

    return true;
}

bool Comm_SaveFwdCallList(int iThisAccount, CallInUserInfo* pCallIn, FWD_TYPE eFwdType)
{
    LocalLogItemRecord sCallRecord;
    if (!Comm_GetCallList(iThisAccount, pCallIn, CT_FORWARDCALL, sCallRecord))
    {
        return false;
    }

    yl::string strFwdNum;
    if (fwd_GetInfo(iThisAccount, eFwdType, &strFwdNum))
    {
        sCallRecord.SetRelationInfo(CR_FWD_TO, strFwdNum);
    }

    return Comm_AddToCallList(sCallRecord);
}

yl::string Comm_GetCodeShowInfo(int iErrorType, LPCSTR lpDefault /*= ""*/)
{
    LPCSTR lpErrorStr = talkhelp_GetErrorInfo(iErrorType);
    if (lpErrorStr != NULL)
    {
        return yl::string(lpErrorStr);
    }

    // 以下代码为了兼容，可以考虑去掉
    // 如果自定义403 Code显示内容不为空,则用自定义的
    if (403 == iErrorType)
    {
        yl::string strCode403Info = configParser_GetConfigString(kszCode403Info);
        if (!strCode403Info.empty())
        {
            return strCode403Info;
        }
    }

    switch (iErrorType)
    {
    case TALK_ERROR_CALLOUT_SLU:
        lpErrorStr = TRID_SHARE_LINE_UNAVAILABLE;
        break;
    case TALK_ERROR_CALLOUT_MSE:
        lpErrorStr = TRID_MAX_SESSION_ERROR;
        break;
    case TALK_ERROR_CALLOUT_NU:
        lpErrorStr = TRID_NET_UNAVAILABLE;
        break;
    case TALK_ERROR_CALLOUT_IA:
        lpErrorStr = TRID_INVALID_ACCOUNT;
        break;
    case TALK_ERROR_CALLOUT_BH:
        lpErrorStr = TRID_BUSY_HERE;
        break;
    case TALK_ERROR_CALLOUT_FN:
        lpErrorStr = TRID_THE_NUMBER_IS_FORBIDDEN;
        break;
    case TALK_ERROR_CALLOUT_TO:
        lpErrorStr = TRID_TIME_OUT;
        break;
    case TARK_ERROR_CALL_RELEASED:
        lpErrorStr = lpDefault;
        break;
    case TALK_ERROR_CALLOUT_IN:
        lpErrorStr = TRID_INVALID_NUMBER;
        break;
    case TALK_ERROR_H323:
        lpErrorStr = "";    // h323协议使用h323传入的字符串
        break;
    case TALK_ERROR_BT_CALLOUT_MSE:
        lpErrorStr = TRID_YOU_CAN_MAKE_TWO_CALLS_AT_MOST;
        break;
    default:
        break;
    }

    if (textEMPTY(lpErrorStr))
    {
        return yl::string(lpDefault);
    }

    return yl::string(lpErrorStr);
}

bool Comm_GetSipCodeState(msgObject& objMsg, int& iState)
{
    if (SIP_CALL_ESTABLISH != objMsg.message)
    {
        return false;
    }

    sip_channels_v20_t sipChannelInfo;
    Comm_GetSipChannelsData(sipChannelInfo, objMsg);

    iState = sipChannelInfo.hold_state;
    return true;
}

bool Comm_IsEstablishMsg(msgObject& objMsg)
{
    if (SIP_CALL_ESTABLISH != objMsg.message)
    {
        return false;
    }

    sip_channels_v20_t sipChannelInfo;
    Comm_GetSipChannelsData(sipChannelInfo, objMsg);
    return SIP_CALL_HOLD_DEFAULT == sipChannelInfo.hold_state;
}

bool Comm_IsCanHold(msgObject& objMsg)
{
    if (SIP_CALL_ESTABLISH != objMsg.message)
    {
        return false;
    }

    sip_channels_v20_t sipChannelInfo;
    Comm_GetSipChannelsData(sipChannelInfo, objMsg);
    return (SIP_CALL_FLAG_CAN_HOLD & sipChannelInfo.call_flag) != 0;
}

// 是否是hold消息
bool Comm_IsHoldMsg(msgObject& objMsg)
{
    if (SIP_CALL_ESTABLISH != objMsg.message)
    {
        return false;
    }

    sip_channels_v20_t sipChannelInfo;
    Comm_GetSipChannelsData(sipChannelInfo, objMsg);
    if (SIP_CALL_HOLD_DEFAULT != sipChannelInfo.hold_state)
    {
        TALK_INFO("This is hold msg, state = %d", sipChannelInfo.hold_state);
    }
    return SIP_CALL_HOLD_DEFAULT != sipChannelInfo.hold_state;
}

bool Comm_IsLocalHold(sip_channels_v20_t& sipChannelInfo)
{
    TALK_INFO("hold state [%d]", sipChannelInfo.hold_state);

    return (SIP_CALL_HOLD_BY_LOCAL & sipChannelInfo.hold_state) != 0;
}

// remote hold
bool Comm_IsRemoteHold(sip_channels_v20_t& sipChannelInfo)
{
    TALK_INFO("Is remote hold? [%d]", sipChannelInfo.hold_state);

    return (SIP_CALL_HOLD_BY_REMOTE & sipChannelInfo.hold_state) != 0;
}

bool Comm_IsCanHold(sip_channels_v20_t& sipChannelInfo)
{
    return (SIP_CALL_FLAG_CAN_HOLD & sipChannelInfo.call_flag) != 0;
}

static yl::string toH460KeepAlive(int nkeepAliveTime)
{
    if (nkeepAliveTime > 0)
    {
        return commonAPI_FormatString("%d:%d", 127, nkeepAliveTime * 1000);
    }
    else if (configParser_GetConfigInt(kszIntelligentPenetrationEnable) != 0)
    {
        return commonAPI_FormatString("%d:%d", 127, DEFAULT_H460_ALIVE_TIME * 1000);
    }

    return "";
}

static bool GetH460KeepAlive(int nkeepAliveTime, unsigned short& nType, unsigned short& nTime)
{
    if (nkeepAliveTime > 0)
    {
        nType = 127;
        nTime = nkeepAliveTime * 1000;
    }
    else if (configParser_GetConfigInt(kszIntelligentPenetrationEnable) != 0)
    {
        nType = 127;
        nTime = DEFAULT_H460_ALIVE_TIME * 1000;
    }
    else
    {
        nType = 127;
        nTime = 0;
    }

    return true;
}

bool GetCompatDeviceInfo(CompatDeviceInfo& info, const yl::string& strUserAgent, bool bSip)
{
    static YLList<CompatDeviceInfo*> listInfo;
    if (listInfo.size() == 0)
    {
        if (!pathFileExist(COMPATDEVICEINFO_PATH))
        {
            return false;
        }

        xml_document doc;
        doc.load_file(COMPATDEVICEINFO_PATH);

        xml_node rootNode = doc.child("deviceList");
        if (!rootNode)
        {
            return false;
        }

        for (xml_node node = rootNode.child("device");
                node;
                node = node.next_sibling("device"))
        {
            xml_attribute attrTmp = node.attribute("name");
            yl::string strName = attrTmp.value();
            //attrTmp = node.attribute("version");
            //yl::string strVersion = attrTmp.value();

            xml_node nodeChild = node.child("sip_audio_byte_order");
            if (!nodeChild.empty())
            {
                CompatDeviceInfo* pInfo = new CompatDeviceInfo;
                pInfo->strName = strName;
                pInfo->bSip = true;
                pInfo->nByteOrder = (strcmp("g722.1-big-ending=0", nodeChild.child_value()) == 0 ? 0 : 1);
                listInfo.push_back(pInfo);
            }

            nodeChild = node.child("h323_audio_byte_order");
            if (!nodeChild.empty())
            {
                CompatDeviceInfo* pInfo = new CompatDeviceInfo;
                pInfo->strName = strName;
                pInfo->bSip = false;
                // g722.1-big-ending=0 表示小端
                pInfo->nByteOrder = (strcmp("g722.1-big-ending=0", nodeChild.child_value()) == 0 ? 1 : 0);
                listInfo.push_back(pInfo);
            }
        }
    }

    // 取小写比较
    yl::string strTmp = strUserAgent;
    strTmp = strTmp.to_lower();
    YLList<CompatDeviceInfo*>::iterator it = listInfo.begin();
    while (it != listInfo.end())
    {
        CompatDeviceInfo* pInfoTmp = *it;

        // 只判断自己协议类型
        if (bSip != pInfoTmp->bSip)
        {
            it++;
            continue;
        }

        if (strTmp.find(pInfoTmp->strName.c_str()) != yl::string::npos)
        {
            info = *pInfoTmp;
            return true;
        }
        it++;
    }

    return false;
}

int FixOctetAlign(char* codecName, const yl::string& strUserAgent, bool bSip)
{
    int nOctetAligen = 0;
    if (strUserAgent.empty())
    {
        return nOctetAligen;
    }

#if 1
    TALK_INFO("codec[%s]", codecName);
#endif

    if (strcasecmp(codecName, "G7221") != 0
            && strcasecmp(codecName, "G7221C") != 0)
    {
        return nOctetAligen;
    }

    // 是否要特殊编码字节序:出厂的配置默认未正常字节序，客户环境出现异常，需要屏蔽此处功能时，开启开关
    if (configParser_GetConfigInt(kszSwitchAudioByte) != 0)
    {
        return nOctetAligen;
    }

    CompatDeviceInfo info;
    if (GetCompatDeviceInfo(info, strUserAgent, bSip))
    {
        nOctetAligen = info.nByteOrder;
    }

    return nOctetAligen;
}

void PrintAudioCodec(ipvp_acodec_t& codec)
{
    TALK_INFO("~~~~~~~~~~~~Audio Code info~~~~~~~~");
    TALK_INFO("codec_name[%s]", codec.codec_name);
    TALK_INFO("codec_ptype[%d]", codec.codec_ptype);
    TALK_INFO("codec_bitrate[%d]", codec.codec_bitrate);
    TALK_INFO("codec_ptime[%d]", codec.codec_ptime);
    TALK_INFO("codec_clock[%d]", codec.codec_clock);

    TALK_INFO("ext_octet_align[%d]", codec.ext_octet_align);
    TALK_INFO("ext_toc_mode[%d]", codec.ext_toc_mode);
    TALK_INFO("ext_annexa[%d]", codec.ext_annexa);

    TALK_INFO("local_maxplaybackrate[%d]", codec.local_maxplaybackrate);
    TALK_INFO("local_maxcapturerate[%d]", codec.local_maxcapturerate);
    TALK_INFO("remote_maxplaybackrate[%d]", codec.remote_maxplaybackrate);
    TALK_INFO("remote_maxcapturerate[%d]", codec.remote_maxcapturerate);

    TALK_INFO("event_ptype [%d]", codec.event_ptype);
    TALK_INFO("event_clock [%d]", codec.event_clock);

    TALK_INFO("rtp_ka_ptype[%d]", codec.rtp_ka_ptype);
    TALK_INFO("rtp_ka_interval[%d]", codec.rtp_ka_interval);

    TALK_INFO("~~~~~~~~~~~~Audio Code info end~~~~~");

    return;
}

bool AudioCodecFrom_sip_codec_t(const sip_codec_t* sip_codec, ipvp_acodec_t* ipvp_codec,
                                const yl::string& strUserAgent)
{
    if (sip_codec == NULL || ipvp_codec == NULL)
    {
        return false;
    }

    if (strlen(sip_codec->codec_name) == 0)
    {
        TALK_INFO("Sip audio codec name is empty");
        return false;
    }

    strncpy(ipvp_codec->codec_name, sip_codec->codec_name, IPVP_MAX_CODECNAME);

    ipvp_codec->codec_ptype = sip_codec->payload;
    ipvp_codec->codec_clock = sip_codec->samplerate;
    ipvp_codec->codec_bitrate = sip_codec->ext_param.audio.bitrate;
    ipvp_codec->codec_ptime = 20;
    ipvp_codec->ext_annexa = 0;
    ipvp_codec->ext_toc_mode = 0;
    ipvp_codec->ext_octet_align = FixOctetAlign(ipvp_codec->codec_name, strUserAgent, true);

    //OPUS对clock和rate特殊处理
    const sip_param_audio_t& sip_ext_audio = sip_codec->ext_param.audio;
    if (sip_ext_audio.audio_codec_type == SIP_AUDIO_TYPE_OPUS)
    {
        ipvp_codec->codec_clock = sip_ext_audio.param.opus.sprop_max_capture_rate;
        ipvp_codec->codec_bitrate = sip_ext_audio.param.opus.max_average_bitrate;

        //opus采样率支持：T49:支持codec list因此理论上只需要写此处
        ipvp_codec->local_maxplaybackrate = sip_ext_audio.param.opus.max_playback_rate;
        ipvp_codec->local_maxcapturerate = sip_ext_audio.param.opus.sprop_max_capture_rate;

        // T49:支持codec list因此理论上此处要让ipvp去掉
        ipvp_codec->remote_maxplaybackrate = sip_ext_audio.param.opus.max_playback_rate;
        ipvp_codec->remote_maxcapturerate = sip_ext_audio.param.opus.sprop_max_capture_rate;
    }

    //http://bugfree.yealink.com/Bug.php?BugID=82340
    if (0 != ipvp_codec->codec_bitrate)
    {
        ipvp_codec->codec_bitrate /= 10;
    }

    if (sip_ext_audio.audio_codec_type == SIP_AUDIO_TYPE_AMR_WB
            || sip_ext_audio.audio_codec_type == SIP_AUDIO_TYPE_AMR)
    {
        ipvp_codec->ext_octet_align = sip_ext_audio.param.amr.octet_align;

        ipvp_codec->ext_toc_mode = 0;
        int iPos = 0;
        while (iPos < 18 && sip_ext_audio.param.amr.mode_set[iPos] != '\0')
        {
            char ch = sip_ext_audio.param.amr.mode_set[iPos];
            if (ch <= '8' && ch >= '0')
            {
                ipvp_codec->ext_toc_mode |= 1 << (ch - '0');
            }
            iPos++;
        }
    }

#if 0
    PrintAudioCodec(ipvp_codec);
#endif

    return true;
}

int Comm_VoiceParam_sip2ipvp(const sip_channel_t* sip_chan, const yl::string& strUserAgent,
                             ipvp_speech_t* speech, const sip_channel_ext_t* sip_chan_v2 /*= NULL*/)
{
    if (sip_chan == NULL || speech == NULL)
    {
        return -1;
    }

    TALK_INFO("Map Voice type [%d]", sip_chan->channel_type);

    //http://10.3.5.199/Bug.php?BugID=71108
    memset(&speech->audio, 0, sizeof(ipvp_audio_t));

    /*
    * audio only
    */
    if (sip_chan->channel_type == SIP_CALL_CHANNEL_TYPE_AUDIO)
    {
        speech->mode = IPVP_FL_VAM_AUDIO;
    }
    else
    {
        return -1;
    }

    ipvp_audio_t* audio;
    audio = &speech->audio;

    /*
    * send & recv flag
    */
    switch (sip_chan->local_media.media_direction)
    {
    case SIP_CALL_SEND_RECV:
        {
            audio->flag  =  IPVP_ADDR_FL_SENDRECV;
            break;
        }
    case SIP_CALL_SEND_ONLY:
        {
            audio->flag  =  IPVP_ADDR_FL_SENDONLY;
            break;
        }
    case SIP_CALL_RECV_ONLY:
        {
            audio->flag  =  IPVP_ADDR_FL_RECVONLY;
            break;
        }
    case SIP_CALL_INACTIVE:
    default:
        {
            audio->flag  =  IPVP_ADDR_FL_INACTIVE;
            break;
        }
    }

    /*
    * address, the rtp/rtcp , src/dst MUST work in the same socket family
    */
    audio->addr_rtp.af_type  = sip_chan->remote_media.rtp_addr.sa_family;
    audio->addr_rtp.sa_len   = (audio->addr_rtp.af_type == AF_INET6) ? sizeof(
                                   struct sockaddr_in6) : sizeof(struct sockaddr_in);

    memcpy(&audio->addr_rtp.src, &sip_chan->local_media.rtp_addr, audio->addr_rtp.sa_len);
    memcpy(&audio->addr_rtp.dst, &sip_chan->remote_media.rtp_addr, audio->addr_rtp.sa_len);

    audio->addr_rtcp.af_type = audio->addr_rtp.af_type;
    audio->addr_rtcp.sa_len  = audio->addr_rtp.sa_len;

    memcpy(&audio->addr_rtcp.src, &sip_chan->local_media.rtcp_addr, audio->addr_rtcp.sa_len);
    memcpy(&audio->addr_rtcp.dst, &sip_chan->remote_media.rtcp_addr, audio->addr_rtcp.sa_len);


    /*
    * codec parameters, we only admit local settings
    * if the remote is different from local, it may be wrong
    */
    // http://10.2.1.199/Bug.php?BugID=96159 ：sip协议使用远端的codec数据当做编码
    strncpy(audio->codec_name, sip_chan->remote_media.codec.codec_name, IPVP_MAX_CODECNAME);
    audio->codec_ptype = sip_chan->remote_media.codec.payload;
    audio->codec_clock = sip_chan->remote_media.codec.samplerate;
    audio->codec_rate = sip_chan->remote_media.codec.ext_param.audio.bitrate;

    // OPUS对clock和rate特殊处理
    const sip_param_audio_t& sip_ext_audio = sip_chan->remote_media.codec.ext_param.audio;
    if (sip_ext_audio.audio_codec_type == SIP_AUDIO_TYPE_OPUS)
    {
        audio->codec_clock = sip_ext_audio.param.opus.sprop_max_capture_rate;
        audio->codec_rate = sip_ext_audio.param.opus.max_average_bitrate;
        //opus采样率支持：T49:支持codec list因此理论上此处不需要赋值
        audio->local_maxplaybackrate =
            sip_chan->local_media.codec.ext_param.audio.param.opus.max_playback_rate;
        audio->local_maxcapturerate =
            sip_chan->local_media.codec.ext_param.audio.param.opus.sprop_max_capture_rate;

        // T49:支持codec list因此理论上只需要写此处
        audio->remote_maxplaybackrate =
            sip_chan->remote_media.codec.ext_param.audio.param.opus.max_playback_rate;
        audio->remote_maxcapturerate =
            sip_chan->remote_media.codec.ext_param.audio.param.opus.sprop_max_capture_rate;
    }

    //http://bugfree.yealink.com/Bug.php?BugID=82340
    if (0 != audio->codec_rate)
    {
        audio->codec_rate /= 10;
    }

    audio->event_ptype = sip_chan->remote_media.rtpev_payload;
    audio->event_clock = 8000;  /*undefined in the sip stack*/
    audio->ptime = sip_chan->remote_media.rtp_ptime;

    // http://10.2.1.199/Bug.php?BugID=96411
    audio->ext_octet_align = FixOctetAlign(audio->codec_name, strUserAgent, true);

    if (sip_chan->remote_media.codec.ext_param.audio.audio_codec_type == SIP_AUDIO_TYPE_AMR_WB
            || sip_chan->remote_media.codec.ext_param.audio.audio_codec_type == SIP_AUDIO_TYPE_AMR)
    {
        audio->ext_octet_align = sip_chan->remote_media.codec.ext_param.audio.param.amr.octet_align;

        audio->ext_toc_mode = 0;
        int iPos = 0;
        while (iPos < 18 && sip_chan->remote_media.codec.ext_param.audio.param.amr.mode_set[iPos] != '\0')
        {
            char ch = sip_chan->remote_media.codec.ext_param.audio.param.amr.mode_set[iPos];
            if (ch <= '8' && ch >= '0')
            {
                audio->ext_toc_mode |= 1 << (ch - '0');
            }
            iPos++;
        }
    }

#if IF_FEATURE_LOOPBACK
    audio->loopback_type = sip_chan->local_media.looback_type;
    audio->loopback_role = sip_chan->local_media.loopback_role;
    audio->loopback_pkt_mode = sip_chan->local_media.loopback_pkt_mode;
    audio->loopback_pkt_dynamic_pl = sip_chan->local_media.loopback_pkt_dynamic_pl;
#endif

    audio->srtp_ext_param = sip_chan->remote_media.srtp_flag;

    /*
    * use security phone call
    */

    /* 优先取v20 */
    if (sip_chan_v2 && sip_chan_v2->local_media_ext.rtp_key[0] != '\0')
    {
        memcpy(audio->enc_key, sip_chan_v2->local_media_ext.rtp_key, IPVP_MAX_SRTPKEY);
        memcpy(audio->dec_key, sip_chan_v2->remote_media_ext.rtp_key, IPVP_MAX_SRTPKEY);
    }
    else if (sip_chan->local_media.srtp_key[0] != '\0')
    {
        memcpy(audio->enc_key, sip_chan->local_media.srtp_key, IPVP_MAX_SRTPKEY);
        memcpy(audio->dec_key, sip_chan->remote_media.srtp_key, IPVP_MAX_SRTPKEY);
    }

    /* RTCP key */
    if (sip_chan_v2 && sip_chan_v2->local_media_ext.rtcp_key[0] != '\0')
    {
        memcpy(audio->rtcp_enc_key, sip_chan_v2->local_media_ext.rtcp_key, IPVP_MAX_SRTPKEY);
        memcpy(audio->rtcp_dec_key, sip_chan_v2->remote_media_ext.rtcp_key, IPVP_MAX_SRTPKEY);
    }

    // sip：本地的codec当做解码
    AudioCodecFrom_sip_codec_t(&sip_chan->local_media.codec, &audio->codec_list[0], strUserAgent);
    audio->codec_list[0].codec_ptime = sip_chan->local_media.rtp_ptime;
    audio->codec_list[0].event_ptype = sip_chan->local_media.rtpev_payload;
    audio->codec_list[0].event_clock = 0;

    // http://10.2.1.199/Bug.php?BugID=103047
    GetH460KeepAlive(0, audio->rtp_ka_ptype, audio->rtp_ka_interval);

#if 1
    TALK_INFO("~~~~~~~~~~~audio param~~~~~~~~~~~");
    TALK_INFO("name[%s] ptype[%d] clock[%d] codec_rate[%d]", audio->codec_name, audio->codec_ptype,
              audio->codec_clock,
              audio->codec_rate);
    TALK_INFO("event_ptype[%d] frmrate[%d] ptime[%d]", audio->event_ptype, audio->event_clock,
              audio->ptime);
    TALK_INFO("enc_key[%s] dec_key[%s]", audio->enc_key, audio->dec_key);
    TALK_INFO("ext_toc_mode[%d] ext_octet_align[%d]", audio->ext_toc_mode, audio->ext_octet_align);
    TALK_INFO("keep type[%d] time[%d]", audio->rtp_ka_ptype, audio->rtp_ka_interval);
    TALK_INFO("sip_ext_audio.audio_codec_type  [%d]", sip_ext_audio.audio_codec_type);
    TALK_INFO("~~~~~~~~~~~audio param~~~~~~~~~~~");
#endif

    return 0;
}

#ifdef IF_FEATURE_PSTN
int Comm_VoiceParam_pstn2ipvp(int pstn_line_id, ipvp_speech_t* speech)
{
    TALK_INFO("Comm_VoiceParam_pstn2ipvp(%d, %p)", pstn_line_id, speech);
    ipvp_audio_t    audio;

    memset(&audio, 0, sizeof(ipvp_audio_t));
    audio.flag = IPVP_ADDR_FL_SENDRECV;

    /*address*/
    audio.addr_rtp.disable = 0;
    audio.addr_rtp.af_type = AF_INET;
    audio.addr_rtp.src.sa4.sin_family = AF_INET;
    audio.addr_rtp.sa_len = sizeof(struct sockaddr_in);

    audio.addr_rtp.src.sa4.sin_port = htons(35340 + 2 * pstn_line_id);
    inet_pton(AF_INET, "127.0.0.1", &audio.addr_rtp.src.sa4.sin_addr);
    audio.addr_rtp.dst.sa4.sin_family = AF_INET;
    audio.addr_rtp.dst.sa4.sin_port = htons(35440 + 2 * pstn_line_id);
    inet_pton(AF_INET, "127.0.0.1", &audio.addr_rtp.dst.sa4.sin_addr);

    audio.addr_rtcp.disable = 1;

    /*codec*/
    strncpy(audio.codec_name, "L8", IPVP_MAX_CODECNAME);
    audio.codec_ptype = 112;
    audio.codec_clock = 8000;
    audio.event_ptype = 101;
    audio.event_clock = 8000;
    audio.ptime = 10;

    speech->mode = IPVP_FL_VAM_AUDIO;
    memcpy(&speech->audio, &audio, sizeof(ipvp_audio_t));

    return 0;
}
#endif // IF_FEATURE_PSTN

bool VideoCodecFrom_sip_codec_t(const sip_codec_t& sipCodec, ipvp_vcodec_t& codec)
{
    if (strlen(sipCodec.codec_name) == 0)
    {
        TALK_INFO("Sip codec name is empty");
        return false;
    }

    strncpy(codec.codec_name, sipCodec.codec_name, IPVP_MAX_CODECNAME);

    codec.codec_ptype = sipCodec.payload;   // 负载
    // 帧速率
    codec.codec_frmrate = sipCodec.samplerate;

    codec.codec_fec = 0;   // TODO
    codec.codec_clock = 0; // TODO
    codec.event_ptype = 0; // TODO
    codec.event_clock = 0; // TODO
    codec.quality_policy = 0;
#if 0
    TALK_INFO("\nCode info begin:");
    TALK_INFO("codec_name [%s]", codec.codec_name);
    TALK_INFO("codec_ptype [%d]", codec.codec_ptype);
    TALK_INFO("codec_fec [%d]", codec.codec_fec);
    TALK_INFO("codec_bitrate [%d]", codec.codec_bitrate);
    TALK_INFO("codec_frmrate [%d]", codec.codec_frmrate);
    TALK_INFO("codec_profile [%d]", codec.codec_profile);
    TALK_INFO("codec_pmode [%d]", codec.codec_pmode);
    TALK_INFO("codec_clock [%d]", codec.codec_clock);
    TALK_INFO("event_ptype [%d]", codec.event_ptype);
    TALK_INFO("event_clock [%d]", codec.event_clock);
    TALK_INFO("win_width [%d]", codec.win_width);
    TALK_INFO("win_height [%d]", codec.win_height);
    TALK_INFO("Code info end");
#endif

    return true;
}

bool VideoCodecExtFrom_sip_codec_t(const sip_codec_t& sipCodec, VideoCodecExtParam& videoExt)
{
    if (strcmp("H263", sipCodec.codec_name) == 0)
    {
        const h263_fmtp_t& src = sipCodec.ext_param.video.param.h263;
        videoExt.m_iSQCIF = src.SQCIF;
        videoExt.m_iQCIF = src.QCIF;
        videoExt.m_iCIF = src.CIF;
        videoExt.m_iCIF4 = src.CIF4;
        TALK_INFO("H263 SQCIF[%d] QCIF[%d] CIF[%d] CIF4[%d]", videoExt.m_iSQCIF, videoExt.m_iQCIF,
                  videoExt.m_iCIF, videoExt.m_iCIF4);
    }
    else if (strcmp("H264", sipCodec.codec_name) == 0)
    {
        const h264_fmtp_t& src = sipCodec.ext_param.video.param.h264;
        videoExt.m_iProfileIDC = src.profile_idc;
        videoExt.m_iProfileIOP = src.profile_iop;
        videoExt.m_iLevel = src.level;
        videoExt.m_iPacketizationMode = src.packetization_mode;
        videoExt.m_iMaxMBPS = src.max_mbps;
        videoExt.m_iMaxFS = src.max_fs;
        videoExt.m_iMaxCPB = src.max_cpb;
        videoExt.m_iMaxDPB = src.max_dpb;
        videoExt.m_iMaxBR = src.max_br;

        TALK_INFO("H264 ProfileIDC[%d],IOP[%d],Level[%d],PacketMode[%d],MBPS[%d],MFS[%d],MCPB[%d],MDPB[%d],MBR[%d]",
                  videoExt.m_iProfileIDC, videoExt.m_iProfileIOP, videoExt.m_iLevel, videoExt.m_iPacketizationMode,
                  videoExt.m_iMaxMBPS, videoExt.m_iMaxFS,
                  videoExt.m_iMaxCPB, videoExt.m_iMaxDPB, videoExt.m_iMaxBR);
    }
    else
    {
        // 不支持
        TALK_INFO("Not support video code name [%s]", sipCodec.codec_name);
        return false;
    }

    return true;
}

bool VideoCodecFrom_sip_media_t(const sip_media_t& sipMedia, ipvp_vcodec_t& codec,
                                VideoCodecExtParam& codecExt)
{
    if (VideoCodecFrom_sip_codec_t(sipMedia.codec, codec))
    {
        VideoCodecExtFrom_sip_codec_t(sipMedia.codec, codecExt);
    }

    codec.codec_bitrate = sipMedia.bandwidth;

    VOICE_INFO("codec bittrate[%d]", sipMedia.bandwidth);
    return true;
}

int Comm_VideoParam_sip2ipvp(const sip_channel_t* sip_chan, ipvp_speech_t* speech,
                             VideoExtInfo* pVideoExt)
{
    if (sip_chan == NULL || speech == NULL)
    {
        return -1;
    }

    TALK_INFO("Map Video type [%d] local direction[%d]", sip_chan->channel_type,
              sip_chan->local_media.media_direction);
    ipvp_video_t* video;

    video = &speech->video;
    int iRet = Comm_VideoParam_sip2ipvp(sip_chan, video, pVideoExt);
    // 只保存Video
    if (sip_chan->channel_type == SIP_CALL_CHANNEL_TYPE_VIDEO
            || sip_chan->channel_type == SIP_CALL_CHANNEL_TYPE_EXTVIDEO)
    {
        speech->mode |= IPVP_FL_VAM_VIDEO;
    }
    else
    {
        iRet = -1;
    }

    return iRet;
}

int Comm_VideoParam_sip2ipvp(const sip_channel_t* sip_chan, ipvp_video_t* video,
                             VideoExtInfo* pVideoExt)
{
    if (NULL == sip_chan || NULL == video)
    {
        return -1;
    }

    memset(video, 0, sizeof(ipvp_video_t));
    // 1.设置标志
    switch (sip_chan->local_media.media_direction)
    {
    case SIP_CALL_SEND_RECV:
        {
            video->flags  =  IPVP_ADDR_FL_SENDRECV;
            break;
        }
    case SIP_CALL_SEND_ONLY:
        {
            video->flags  =  IPVP_ADDR_FL_SENDONLY;
            break;
        }
    case SIP_CALL_RECV_ONLY:
        {
            video->flags  =  IPVP_ADDR_FL_RECVONLY;
            break;
        }
    case SIP_CALL_INACTIVE:
    default:
        {
            video->flags  =  IPVP_ADDR_FL_INACTIVE;
            break;
        }
    }

    video->size = sizeof(ipvp_video_t);

    // 2. 设置RTP、RTCP：address, the rtp/rtcp , src/dst MUST work in the same socket family
    video->addr_rtp.af_type  = sip_chan->remote_media.rtp_addr.sa_family;
    video->addr_rtp.sa_len   = (video->addr_rtp.af_type == AF_INET6) ? sizeof(
                                   struct sockaddr_in6) : sizeof(struct sockaddr_in);

    // 本地IP可能是0.0.0.0
    memcpy(&video->addr_rtp.src, &sip_chan->local_media.rtp_addr, video->addr_rtp.sa_len);

    memcpy(&video->addr_rtp.dst, &sip_chan->remote_media.rtp_addr, video->addr_rtp.sa_len);

    video->addr_rtcp.af_type = video->addr_rtp.af_type;
    video->addr_rtcp.sa_len  = video->addr_rtp.sa_len;

    memcpy(&video->addr_rtcp.src, &sip_chan->local_media.rtcp_addr, video->addr_rtcp.sa_len);
    memcpy(&video->addr_rtcp.dst, &sip_chan->remote_media.rtcp_addr, video->addr_rtcp.sa_len);

    // 3.1 注意编码使用对方codec
    video->enc_max = 1;
    VideoCodecFrom_sip_media_t(sip_chan->remote_media, video->enc_list[0], pVideoExt->m_stRemote);

    // 3.2 解码使用Local
    video->dec_max = 1;
    VideoCodecFrom_sip_media_t(sip_chan->local_media, video->dec_list[0], pVideoExt->m_stLocal);

    // 4 设置SRTP秘钥
    if (sip_chan->local_media.srtp_key[0] != '\0')
    {
        memcpy(video->enc_key, sip_chan->local_media.srtp_key, IPVP_MAX_SRTPKEY);
        memcpy(video->dec_key, sip_chan->remote_media.srtp_key, IPVP_MAX_SRTPKEY);
    }

    // 帧同步方式2
    // 5 设置RTCP参数
    strncpy(video->RTCP_params, sip_chan->remote_media.rtcp_fb, chMIN(SIP_MAX_RTCP_FB_LEN,
            IPVP_MAX_CODEC_ATTR));

    // 6.H460
    //video->H460_params  h323
    // 6.H460 参数 需要根据网络穿透使用默认值
    yl::string strH460 = toH460KeepAlive(0);
    if (!strH460.empty())
    {
        strncpy(video->H460_params, strH460.c_str(), chMIN(IPVP_MAX_CODEC_ATTR - 1, strH460.size()));
    }

    // capset http://bugfree.yealink.com/Bug.php?BugID=82713
    if (strcasecmp(sip_chan->remote_media.codec.codec_name, "YL-FPR") == 0)
    {
        return -1;
    }

    video->rtp_ext_CVO_id = sip_chan->remote_media.cvo_identifier;

#if 0
    TALK_INFO("Video Param:");
    TALK_INFO("flags [%d]", video->flags);
    TALK_INFO("size [%d]", video->size);
    TALK_INFO("dec_max [%d]", video->dec_max);
    TALK_INFO("enc_max [%d]", video->enc_max);
    TALK_INFO("enc_key [%s]", video->enc_key);
    TALK_INFO("dec_key [%s]", video->dec_key);
    TALK_INFO("RTCP_params [%s]", video->RTCP_params);
    TALK_INFO("H460_params [%s]", video->H460_params);
    TALK_INFO("priv_capset [%s]", video->priv_capset);
#endif
    // 分辨率
    return 0;
}

int Comm_MediaParam_sip2ipvp(sip_channels_v20_t* sip_chan, const yl::string& strUserAgent,
                             ipvp_speech_t* speech,
                             VideoExtInfo* pVideoExt/* = NULL*/)
{
    TALK_INFO("Get Media Param start");
    if (NULL == speech)
    {
        return -1;
    }

    memset(speech, 0, sizeof(ipvp_speech_t));
    Comm_VoiceParam_sip2ipvp((sip_channel_t*)(sip_chan->audio.ptr), strUserAgent, speech, (sip_channel_ext_t*)(sip_chan->audio_ext.ptr));

    // FECC 远端摄像头控制，T49不支持
    // SIP_CALL_FLAG_HAVE_FECC Far end camera control
    TALK_INFO("Get Media Param end");

    return 0;
}

int Comm_RingbackParam_sip2ipvp(sip_call_ringback_info_t* sip_ringback, ipvp_speech_t* speech,
                                VideoExtInfo* pVideoExt)
{
    if (NULL == sip_ringback
            || NULL == speech
            || NULL == pVideoExt)
    {
        return -1;
    }

    TALK_INFO("Get Ringback Param Beg");

    memset(speech, 0, sizeof(ipvp_speech_t));

    Comm_VoiceParam_sip2ipvp(&sip_ringback->audio, "", speech);

    TALK_INFO("Get Ringback Param End");
    return 1;
}

int Comm_AudioParamArray_sip2ipvp(const sip_codec_array_t& src, AudioStreamInfo& audio,
                                  const yl::string& strUserAgent)
{
    TALK_INFO("Audio param array size[%d]", src.codec_cnt);
    // 清空数据
    audio.Reset();

    for (int i = 0; i < src.codec_cnt && i < SIP_MAX_CODEC_LIST; i++)
    {
        const sip_codec_t& sipRemote = src.remote[i];
        const sip_codec_t& sipLocal = src.local[i];

        // 底层的参数默认都设置为0
        ipvp_acodec_t codecInfo;
        memset(&codecInfo, 0, sizeof(ipvp_acodec_t));
        if (AudioCodecFrom_sip_codec_t(&sipRemote, &codecInfo, strUserAgent))
        {
            audio.listRemoteCodec.push_back(codecInfo);
        }

        if (AudioCodecFrom_sip_codec_t(&sipLocal, &codecInfo, strUserAgent))
        {
            audio.listLocalCodec.push_back(codecInfo);
        }

    }

    return 1;
}

int Comm_MediaParamArray_sip2ipvp(sip_media_codec_array_t& src, const yl::string& strUserAgent,
                                  MediaStreamInfo& media)
{
    // 此处保存SIP_CALL_CODEC_LIST消息的数据
    for (UINT i = 0; i < src.media_cnt; i++)
    {
        const sip_codec_array_t& codecArray = src.codec_array[i];
        switch (codecArray.codec_type)
        {
        case SIP_CALL_CHANNEL_TYPE_AUDIO:
            {
                // 音频
                Comm_AudioParamArray_sip2ipvp(codecArray, media.m_stAudio, strUserAgent);
            }
            break;
        default:
            COMMONUNITS_INFO("Unknow codec list[%d]", codecArray.codec_type);
        }
    }

    return 1;
}

// 从消息中获取sip数据
bool Comm_GetSipData(void* pDst, const void* pSrc, int iType)
{
    return 0 == sip_map_info_recover(pDst, (void*)pSrc, iType);
}

bool Comm_GetSipChannelsData(sip_channels_v20_t& dst, msgObject& msg)
{
    AUTO_SCOPED_TIMER;

    void* p = msg.GetExtraData();
    if (p == NULL)
    {
        return false;
    }

    memset(&dst, 0, sizeof(dst));

    if (Comm_GetSipData(&dst, p, SIP_MAP_METHOD_CHANNELS_V20))
    {
        return true;
    }

    /* 兼容sip旧版本，取不到v20情况下，再尝试取旧的 */
    return Comm_GetSipData(&dst, p, SIP_MAP_METHOD_CHANNELS);
}

// 转换sip的bla,blf状态
BLF_STATUS_CODE Comm_MapSipBLFStatus(int iStatus)
{
    BLF_STATUS_CODE eBLFStatus = CN_BLF_STATUS_IDLE;
    switch (iStatus)
    {
    case SIP_BLF_STATUS_GENBEN_UNKOWN:
    case SIP_BLF_STATUS_UNKOWN:
        eBLFStatus = CN_BLF_STATUS_UNKOWN;
        break;
    case SIP_BLF_STATUS_IDLE:
        eBLFStatus = CN_BLF_STATUS_IDLE;
        break;
    case SIP_BLF_STATUS_RINGING:
        eBLFStatus = CN_BLF_STATUS_RINGING;
        break;
    case SIP_BLF_STATUS_TALKING:
        eBLFStatus = CN_BLF_STATUS_TALKING;
        break;
    case SIP_BLF_STATUS_CALLOUT:
        eBLFStatus = CN_BLF_STATUS_CALLOUT;
        break;
    case SIP_BLF_STATUS_HOLD:
        eBLFStatus = CN_BLF_STATUS_HOLD;
        break;

    case SIP_BLF_STATUS_TALKING_ME:
        eBLFStatus = CN_BLF_STATUS_TALKING_ME;
        break;
    case SIP_BLF_STATUS_RINGING_ME:
        eBLFStatus = CN_BLF_STATUS_RINGING_ME;
        break;
    case SIP_BLF_STATUS_CALLING_ME:
        eBLFStatus = CN_BLF_STATUS_CALLING_ME;
        break;
    case SIP_BLF_STATUS_HELD_ME:
        eBLFStatus = CN_BLF_STATUS_HELD_ME;
        break;
    case SIP_BLF_STATUS_PARKED:
        eBLFStatus = CN_BLF_STATUS_PARKED;
        break;

    case SIP_BLA_STATUS_UNKOWN:
    case SIP_BLA_DRAFT_STATUS_UNKOWN:
        eBLFStatus = CN_BLA_STATUS_UNKOWN;
        break;
    case SIP_BLA_STATUS_IDLE:
    case SIP_BLA_DRAFT_STATUS_IDLE:
        eBLFStatus = CN_BLA_STATUS_IDLE;
        break;
    case SIP_BLA_STATUS_SEIZED:
    case SIP_BLA_DRAFT_STATUS_SEIZED:
        eBLFStatus = CN_BLA_STATUS_SEIZED;
        break;
    case SIP_BLA_STATUS_PROGRESSING:
    case SIP_BLA_DRAFT_STATUS_OUTGOING:
        eBLFStatus = CN_BLA_STATUS_PROGRESSING;
        break;
    case SIP_BLA_STATUS_ALERTING:
    case SIP_BLA_DRAFT_STATUS_RINGING:
        eBLFStatus = CN_BLA_STATUS_ALERTING;
        break;
    case SIP_BLA_STATUS_ACTIVE:
    case SIP_BLA_DRAFT_STATUS_TALKING:
        eBLFStatus = CN_BLA_STATUS_ACTIVE;
        break;
    case SIP_BLA_STATUS_HELD:
    case SIP_BLA_DRAFT_STATUS_HELD_PUBLIC:
        eBLFStatus = CN_BLA_STATUS_HELD;
        break;
    case SIP_BLA_STATUS_HELD_PRIVATE:
        eBLFStatus = CN_BLA_STATUS_HELD_PRIVATE;
        break;
    case SIP_BLA_STATUS_BRIDGE_ACITVE:
        eBLFStatus = CN_BLA_STATUS_BRIDGE_ACITVE;
        break;
    case SIP_BLA_STATUS_BRIDGE_HELD:
        eBLFStatus = CN_BLA_STATUS_BRIDGE_HELD;
        break;
    default:
        break;
    }

    return eBLFStatus;
}

BLF_STATUS_CODE Comm_MapBLF2BLAStatus(BLF_STATUS_CODE eStatus)
{
    switch (eStatus)
    {
    case CN_BLF_STATUS_CALLOUT:
        return CN_BLA_STATUS_PROGRESSING;
    case CN_BLF_STATUS_RINGING:
        return CN_BLA_STATUS_ALERTING;
    case CN_BLF_STATUS_TALKING:
        return CN_BLA_STATUS_ACTIVE;
    case CN_BLF_STATUS_HOLD:
        return CN_BLA_STATUS_HELD;
    case CN_BLF_STATUS_IDLE:
        return CN_BLA_STATUS_IDLE;
    case CN_BLF_STATUS_UNKOWN:
        return CN_BLA_STATUS_UNKOWN;
    default:
        return eStatus;
    }
}

bool Comm_UpdateSysTime(msgObject& objMsg)
{
    // timeout设置为0，rtServer将使用默认值（1000）
    return sys_sip_set_time(mkGetHandle(), objMsg.GetExtraData(), objMsg.GetExtraSize(), 0) == 0;
}

bool ParseString(YLVector<yl::string>& veclist, const yl::string& strSrc,
                 const yl::string& filterStr)
{
    if (!strSrc.empty())
    {
        size_t begpos = 0;
        size_t found = strSrc.find_first_of(filterStr, begpos);
        while (found != yl::string::npos)
        {
            if (begpos < found)
            {
                veclist.push_back(strSrc.substr(begpos, found - begpos));
            }

            begpos = found + 1;
            found = strSrc.find_first_of(filterStr, begpos);
        }

        if (begpos < strSrc.length())
        {
            // 剩余字符串
            veclist.push_back(strSrc.substr(begpos));
        }
    }

    return veclist.size() > 0;
}

bool GetDPickupCallOutNumber(int iDsskeyID, yl::string& strCallOutNumber,
                             const yl::string& strValue, const yl::string& strExtension, int iAccountID)
{
    // todo
    yl::string strURI = strValue;
    if (strURI.substr(0, 4) == "sip:")
    {
        strURI = strURI.substr(4, strURI.size() - 4);
    }

    if (blf_IsSupportAutopPickUpByDsskey(iDsskeyID)
            && configParser_GetCfgItemIntValue(kszAccountDialogPickUp, iAccountID) == 1)
    {
        if (1 == configParser_GetConfigInt(kszBlfCallOutByNumber))
        {
            yl::string strServerName;
            commonUnits_ParserDialURI(strURI, strCallOutNumber, strServerName);
        }
        else
        {
            strCallOutNumber = strURI;
        }
    }
    else
    {
        enum
        {
            DPICKUP_SEND_CODE_VALUE = 0,
            DPICKUP_SEND_CODE       = 1,
            DPICKUP_SEND_VALUE      = 2,
        };

        //http://192.168.1.99/Bug.php?BugID=22831
        int iSendMode = configParser_GetConfigInt(kszIsBLFPickupOnlySendCode);
        bool bBlflistType = (dsskey_GetDsskeyType(iDsskeyID) == DT_BLFLIST);

        yl::string strCode = "";
        if (iSendMode != DPICKUP_SEND_VALUE)
        {
            if (bBlflistType)
            {
                strCode = configParser_GetCfgItemStringValue(kszBLFListCode, iAccountID);
            }
            else
            {
                strCode = strExtension;
            }
            strCode.empty() ? strCode = acc_GetPickupCodeByType(iAccountID, PT_DIRECTED) : NULL;
        }

        yl::string strNumber = "";
        if (iSendMode != DPICKUP_SEND_CODE)
        {
            DialgInfo refData;
            blf_GetDialogInfoByDsskeyId(refData, iDsskeyID, BLF_Manager);
            if (!refData.m_sSipPickUpInfo.empty())
            {
                strNumber = refData.m_sSipPickUpInfo;
            }
            else
            {
                if (bBlflistType && !strExtension.empty())
                {
                    strNumber = strExtension;
                }
                else
                {
                    if (1 == configParser_GetConfigInt(kszBlfCallOutByNumber))
                    {
                        // 截取sip:11231@192.168.1.199 中的11231，只呼出11231
                        yl::string strServerName;
                        commonUnits_ParserDialURI(strURI, strNumber, strServerName);
                    }
                    else
                    {
                        strNumber = strURI;
                    }
                }
            }
        }

        strCallOutNumber = strCode + strNumber;
    }

    TALK_INFO("DPickupCallOutNumber [%s]", strCallOutNumber.c_str());

    return !strCallOutNumber.empty();
}

int Comm_AudioCodec_sip2ipvp(AudioStreamInfo& audioInfo, ipvp_speech_t* speech)
{
    // 因为第1个codec是通话建立的消息携带，目前codec从1开始
    int iCodec = 1;
    AudioCodecList::iterator it = audioInfo.listLocalCodec.begin();
    for (; iCodec < IPVP_MAX_CODEC && it != audioInfo.listLocalCodec.end(); ++it, ++iCodec)
    {
        // http://10.2.1.199/Bug.php?BugID=96159 sip协议：本端当做解码
        speech->audio.codec_list[iCodec] = *it;
    }

    if (iCodec != 1)
    {
        TALK_INFO("audio codec counts[%d]", iCodec);
        speech->audio.codec_list_cnt = iCodec;
    }

    return 1;
}

int Comm_ShareVideo_sip2ipvp(const sip_channel_t* sip_chan, bool& bHasShareVideo,
                             ipvp_video_t* videoInfo, VideoExtInfo* pVideoExt/* = NULL*/)
{
    if (NULL == sip_chan)
    {
        return -1;
    }

    bHasShareVideo = Comm_ShareData_sip2ipvp(sip_chan);

    //TALK_INFO("Comm_ShareVideo_sip2ipvp bHasShareVideo[%d]", bHasShareVideo);
    return Comm_VideoParam_sip2ipvp(sip_chan, videoInfo, pVideoExt);
}

bool Comm_ShareData_sip2ipvp(const sip_channel_t* sip_chan)
{
    if (NULL == sip_chan)
    {
        return false;
    }
    int nRemotertpPort = sip_chan->remote_media.rtp_addr.sa_family == PF_INET6 ? chBE2Host((
                             WORD)sip_chan->remote_media.rtp_addr.a6.sin6_port) : chBE2Host((WORD)
                                     sip_chan->remote_media.rtp_addr.a4.sin_port);
    int nLocalrtpPort = sip_chan->local_media.rtp_addr.sa_family == PF_INET6 ? chBE2Host((
                            WORD)sip_chan->local_media.rtp_addr.a6.sin6_port) : chBE2Host((WORD)
                                    sip_chan->local_media.rtp_addr.a4.sin_port);

    return (nLocalrtpPort != 0 || nRemotertpPort != 0);
}

void Comm_PrintMedia(ipvp_speech_t* speech)
{
    if (NULL == speech)
    {
        return;
    }

    VOICE_INFO("\n******************speech info*******************");
    VOICE_INFO("size[%d] mode[%d]", speech->size, speech->mode);
    ipvp_audio_t& stAudio = speech->audio;

    // 通话调试打印, 为减少重复编码，勿删
    VOICE_INFO("\n~~~~~~~~~~~~audio info~~~~~~~~~~~~~~");
    VOICE_INFO("flag[%d] size[%d] sizeof[%d]", stAudio.flag, stAudio.size, sizeof(ipvp_audio_t));

    VOICE_INFO("addr_rtp[%d] addr_rtp[%d] addr_rtp[%d]", stAudio.addr_rtp.disable,
               stAudio.addr_rtp.af_type, stAudio.addr_rtp.sa_len);
    VOICE_INFO("addr_rtcp[%d] addr_rtcp[%d] addr_rtcp[%d]", stAudio.addr_rtcp.disable,
               stAudio.addr_rtcp.af_type, stAudio.addr_rtcp.sa_len);

    VOICE_INFO("codec_name[%s]", stAudio.codec_name);
    VOICE_INFO("codec_ptype[%d]", stAudio.codec_ptype);
    VOICE_INFO("codec_clock[%d]", stAudio.codec_clock);
    VOICE_INFO("codec_rate[%d]", stAudio.codec_rate);
    VOICE_INFO("event_ptype[%d]", stAudio.event_ptype);
    VOICE_INFO("event_clock[%d]", stAudio.event_clock);
    VOICE_INFO("ptime[%d]", stAudio.ptime);

    VOICE_INFO("enc_key[%s]", stAudio.enc_key);
    VOICE_INFO("dec_key[%s]", stAudio.dec_key);

    VOICE_INFO("rtcp_enc_key[%s]", stAudio.rtcp_enc_key);
    VOICE_INFO("rtcp_dec_key[%s]", stAudio.rtcp_dec_key);

    VOICE_INFO("ext_octet_align [%d]", stAudio.ext_octet_align);
    VOICE_INFO("ext_toc_mode [%d]", stAudio.ext_toc_mode);
    VOICE_INFO("ext_annexa [%d]", stAudio.ext_annexa);

    for (int i = 0; i < stAudio.codec_list_cnt; i++)
    {
        PrintAudioCodec(stAudio.codec_list[i]);
    }

    VOICE_INFO("codec list[%d] codec_0[%s] codec_0[%s]", stAudio.codec_list_cnt,
               stAudio.codec_list[0].codec_name,
               stAudio.codec_list[1].codec_name);
    VOICE_INFO("rtp_ka_ptype[%d] rtp_ka_interval[%d]", stAudio.rtp_ka_ptype, stAudio.rtp_ka_interval);

    VOICE_INFO("rtp_multiplex_enable[%d] id[%d] rtcp_multiplex_enable[%d] id[%d]",
               stAudio.multiplex.rtp_multiplex_enable,
               stAudio.multiplex.rtp_multiplex_id, stAudio.multiplex.rtcp_multiplex_enable,
               stAudio.multiplex.rtcp_multiplex_id);

    VOICE_INFO("loopback_type[%d] loopback_pkt_mode[%d] loopback_role[%d] loopback_pkt_dynamic_pl[%d]",
               stAudio.loopback_type, stAudio.loopback_pkt_mode, stAudio.loopback_role,
               stAudio.loopback_pkt_dynamic_pl);

    VOICE_INFO("local maxplaybackrate[%d] maxcapturerate[%d] remote maxplaybackrate[%d] maxcapturerate[%d]",
               stAudio.local_maxplaybackrate, stAudio.local_maxcapturerate, stAudio.remote_maxplaybackrate,
               stAudio.remote_maxcapturerate);


    ipvp_video_t& stVideo = speech->video;
    VOICE_INFO("\n~~~~~~~~~~~~video info~~~~~~~~~~~~~~");
    VOICE_INFO("flags[%d] size[%d]", stVideo.flags, stVideo.size);

    VOICE_INFO("dec_max[%d] enc_max[%d]", stVideo.dec_max, stVideo.enc_max);
    VOICE_INFO("--------------enc list---------------");
    for (int i = 0; i < stVideo.enc_max; i++)
    {
        ipvp_vcodec_t& codec = stVideo.enc_list[i];
        VOICE_INFO("name[%s] ptype[%d] fec[%d] bitrate [%d]", codec.codec_name, codec.codec_ptype,
                   codec.codec_fec,
                   codec.codec_bitrate);
        VOICE_INFO("frmrate[%d] profile[%d] pmode[%d] quality_policy[%d]", codec.codec_frmrate,
                   codec.codec_profile, codec.codec_profile, codec.quality_policy);
        VOICE_INFO("clock[%d] event_ptype[%d] event_clock[%d] width[%d] height[%d] key_frame_interval[%d]",
                   codec.codec_clock, codec.event_ptype, codec.event_clock, codec.win_width, codec.win_height,
                   codec.key_frame_interval);
    }

    VOICE_INFO("--------------dec list---------------");

    for (int i = 0; i < stVideo.dec_max; i++)
    {
        ipvp_vcodec_t& codec = stVideo.dec_list[i];
        VOICE_INFO("name[%s] ptype[%d] fec[%d] bitrate [%d]", codec.codec_name, codec.codec_ptype,
                   codec.codec_fec,
                   codec.codec_bitrate);
        VOICE_INFO("frmrate[%d] profile[%d] pmode[%d] quality_policy[%d]", codec.codec_frmrate,
                   codec.codec_profile, codec.codec_profile, codec.quality_policy);
        VOICE_INFO("clock[%d] event_ptype[%d] event_clock[%d] width[%d] height[%d] key_frame_interval[%d]",
                   codec.codec_clock, codec.event_ptype, codec.event_clock, codec.win_width, codec.win_height,
                   codec.key_frame_interval);
    }

    VOICE_INFO("enc_key[%s]", stVideo.enc_key);
    VOICE_INFO("dec_key %s]", stVideo.dec_key);

    VOICE_INFO("RTCP_params [%s]", stVideo.RTCP_params);
    VOICE_INFO("H460_params [%s]", stVideo.H460_params);
    VOICE_INFO("priv_capset [%s]", stVideo.priv_capset);

    VOICE_INFO("rtp_ext_CVO_id[%d]", stVideo.rtp_ext_CVO_id);

    VOICE_INFO("\n******************speech info end*******************");

    return;
}

bool Comm_IsRoutineInherits(ROUTINE_STATE base, ROUTINE_STATE herit, ROUTINE_STATE except/* = ROUTINE_UNKNOWN*/)
{
    /* 相等或者有共同的base，且存在继承关系 */
    if (herit == except)
    {
        return false;
    }

    if (base == herit)
    {
        return true;
    }

    struct RoutineHeritPair
    {
        ROUTINE_STATE m_base;
        ROUTINE_STATE m_herit;
    };

    static const RoutineHeritPair s_cRoutineHeritTab[] =
    {
        /* dial */
        { ROUTINE_DIALING_CLS,      ROUTINE_DIALING           },
        { ROUTINE_DIALING_CLS,      ROUTINE_BTDIALING         },
        { ROUTINE_BTDIALING,        ROUTINE_BTPREDIAL         },
        { ROUTINE_DIALING,          ROUTINE_PREDIAL           },
        { ROUTINE_DIALING,          ROUTINE_PRETRAN           },
        { ROUTINE_DIALING,          ROUTINE_RETRIEVE_CALLPARK },


        /* talk */
        { ROUTINE_TALKING_CLS,      ROUTINE_TALKING           },
        { ROUTINE_TALKING_CLS,      ROUTINE_BTTALKING         },
        { ROUTINE_TALKING,          ROUTINE_NETWORKCONF       },
        { ROUTINE_TALKING,          ROUTINE_LINE_DEVICE       },


        /* connect */
        { ROUTINE_CONNECTING_CLS,   ROUTINE_CONNECTING          },
        { ROUTINE_CONNECTING_CLS,   ROUTINE_BTCONNECTING        },
        { ROUTINE_CONNECTING,       ROUTINE_RINGBACK            },
        { ROUTINE_CONNECTING,       ROUTINE_NETWORKCONF_CONNECT },
        { ROUTINE_CONNECTING,       ROUTINE_BLACONNECTING       },

        /* ring */
        { ROUTINE_RINGING_CLS,      ROUTINE_RINGING             },
        { ROUTINE_RINGING_CLS,      ROUTINE_BTRINGING           },
        { ROUTINE_RINGING,          ROUTINE_RECALL              },

        /* finish */
        { ROUTINE_FINISHED_CLS,     ROUTINE_FINISHED            },
        { ROUTINE_FINISHED_CLS,     ROUTINE_BTFINISHED          },
        { ROUTINE_FINISHED,         ROUTINE_AUTOREDIAL          },
    };

    const static int iTabSize = sizeof(s_cRoutineHeritTab) / sizeof(s_cRoutineHeritTab[0]);
    for (int i = 0; i < iTabSize; ++i)
    {
        if (s_cRoutineHeritTab[i].m_herit == herit)
        {
            return Comm_IsRoutineInherits(base, s_cRoutineHeritTab[i].m_base, except);
        }
    }

    return false;
}

bool Comm_IsSessionInherits(SESSION_STATE base, SESSION_STATE herit)
{
    return base == herit
           || Comm_IsRoutineInherits(Comm_SessionMap2Routine(base), Comm_SessionMap2Routine(herit));
}

ROUTINE_STATE Comm_SessionMap2Routine(SESSION_STATE eSession)
{
    switch (eSession)
    {
    case SESSION_DIALING:
        return ROUTINE_DIALING;
    case SESSION_PREDIAL:
        return ROUTINE_PREDIAL;
    case SESSION_PRETRAN:
        return ROUTINE_PRETRAN;
    case SESSION_PRE_RETRIEVEPARK:
        return ROUTINE_RETRIEVE_CALLPARK;
    case SESSION_CONNECTING:
    case SESSION_CONNECTING_TRAN:
        return ROUTINE_CONNECTING;
    case SESSION_RINGBACK:
    case SESSION_RINGBACK_TRAN:
        return ROUTINE_RINGBACK;
    case SESSION_BLACONNECTING:
        return ROUTINE_BLACONNECTING;
    case SESSION_NETWORKCONF_CONNECT:
        return ROUTINE_NETWORKCONF_CONNECT;
    case SESSION_RINGING:
        return ROUTINE_RINGING;
    case SESSION_RECALL:
        return ROUTINE_RECALL;
    case SESSION_NETWORKCONF:
        return ROUTINE_NETWORKCONF;
    case SESSION_TALKING:
    case SESSION_TRANSFERRED:
    case SESSION_AOC_TALKING:
        return ROUTINE_TALKING;
    case SESSION_LINEDEVICE:
        return ROUTINE_LINE_DEVICE;
    case SESSION_FINISHED:
        return ROUTINE_FINISHED;
    case SESSION_AUTO_REDIAL:
        return ROUTINE_AUTOREDIAL;
    case SESSION_PARK_NOTIFY:
        return ROUTINE_PARK_NOTIFY;
    case SESSION_AOC_FINISH:
        return ROUTINE_AOC_FINISH;
    case SESSION_LOCALCONF:
        break;
    case SESSION_BETRANSFERRED:
        break;
    case SESSION_BTTALKING:
        return ROUTINE_BTTALKING;
    case SESSION_BTDIALING:
        return ROUTINE_BTDIALING;
    case SESSION_BTCONNECTING:
        return ROUTINE_BTCONNECTING;
    case SESSION_BTFINISHED:
        return ROUTINE_BTFINISHED;
    case SESSION_BTRINGING:
        return ROUTINE_BTRINGING;
    case SESSION_ATSCONF:
        return ROUTINE_ATSCONF;
    default:
        break;
    }

    return ROUTINE_UNKNOWN;
}

ROUTINE_STATE Comm_GetInheritsRoutine(ROUTINE_STATE herit)
{
    switch (herit)
    {
    case ROUTINE_DIALING:
    case ROUTINE_PREDIAL:
    case ROUTINE_PRETRAN:
    case ROUTINE_RETRIEVE_CALLPARK:
        return ROUTINE_DIALING;
    case ROUTINE_CONNECTING:
    case ROUTINE_NETWORKCONF_CONNECT:
    case ROUTINE_BLACONNECTING:
    case ROUTINE_RINGBACK:
        return ROUTINE_CONNECTING;
    case ROUTINE_RINGING:
    case ROUTINE_RECALL:
        return ROUTINE_RINGING;
    case ROUTINE_TALKING:
    case ROUTINE_NETWORKCONF:
    case ROUTINE_LINE_DEVICE:
        return ROUTINE_TALKING;
    case ROUTINE_FINISHED:
    case ROUTINE_AUTOREDIAL:
        return ROUTINE_FINISHED;
#if 0
    /* BT 单独处理 */
    case ROUTINE_BTCONNECTING:
    case ROUTINE_BTDIALING:
    case ROUTINE_BTTALKING:
    case ROUTINE_BTRINGING:
    case ROUTINE_BTFINISHED:
#endif
    default:
        return herit;
    }
}


