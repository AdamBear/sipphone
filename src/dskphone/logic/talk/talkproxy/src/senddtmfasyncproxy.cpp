#include "senddtmfasyncproxy.h"
#include "keymap.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/callinfo.h"
#include "voip/modvoip.h"
#include "voice/include/modvoice.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "talk/talklistener/include/modtalklistener.h"

// 进入界面就需要发送DTMF延时
#define DELAY_DTMF_TIME     200
// 逗号延迟发送DTMF事件
#define COMMA_DELAY_TIME    2*1000

int CSendDTMFASyncProxy::GetDtmfKey(int iDtmf)
{
    if (iDtmf == '*')
    {
        return PHONE_KEY_STAR;
    }
    else if (iDtmf == '#')
    {
        return PHONE_KEY_POUND;
    }
    else if (iDtmf >= '0' && iDtmf <= '9')
    {
        return PHONE_KEY_0 + iDtmf - '0';
    }
#if IF_FEATURE_DTMF_FLASH
    else if (iDtmf >= 'A' && iDtmf <= 'E')
#else
    else if (iDtmf >= 'A' && iDtmf <= 'D')
#endif
    {
        return PHONE_KEY_A + iDtmf - 'A';
    }

    return -1;
}

bool CSendDTMFASyncProxy::SendDTMF(char ch, int interval)
{
    /* 客户特制确定等待时间，不用外部传递值 */
    bool bDelay = ch == ',';
    int iKey = GetDtmfKey(ch);

    if (bDelay)
    {
        interval = COMMA_DELAY_TIME;
    }
    else if (iKey < 0)
    {
        return false;
    }

    if (m_uInterval != interval)
    {
        SetTimer(interval);
    }

    return bDelay || SendDTMF(iKey);
}

bool CSendDTMFASyncProxy::SendDTMF(int nKey)
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoBySessionID(m_iSessionID);
    if (NULL == pCallInfo)
    {
        return false;
    }

#ifdef IF_SUPPORT_LINE_DEVICE
    if (pCallInfo->GetCallLineType() == CALL_LINE_DEVICE)
    {
        // 外设通话只本地播放tone
        voice_PlayKeyTone(nKey, true);
        return true;
    }
#endif // IF_SUPPORT_LINE_DEVICE

#ifdef IF_FEATURE_PSTN
    if (pCallInfo->GetCallLineType() == CALL_LINE_PSTN)
    {
        char szKey[2] = {0};
        snprintf(szKey, sizeof(szKey), "%c", nKey);
        return pstnadapter_Dial(acc_GetDeviceIDByAccountID(pCallInfo->GetAccountID()), szKey, pCallInfo->GetAccountID());
    }
#endif // IF_FEATURE_PSTN

    bool bSendInband = false;
    bool bSendInfo = false;
    bool bSend2833 = false;

    int iPayloadType = pCallInfo->m_tVPMParam.audio.event_ptype;
    int iAccountID = pCallInfo->GetAccountID();
    int iCallID = pCallInfo->GetCallID();

    DTMF_DIR eDtmfDir = DTMF_DIR_TOBOTH;
    if (configParser_GetConfigInt(kszLocalDtmfTone) == 0)
    {
        // 本地不播放声音
        eDtmfDir = DTMF_DIR_TONET;
    }

    if (iAccountID != INVALID_ACCOUNTID
            && configParser_GetCfgItemIntValue(kszAccountDTMFType, iAccountID) == DTMF_INBAND
            && acc_GetAccountProtocol(iAccountID) == PROTOCOL_SIP)
    {
        // todo
        eDtmfDir = DTMF_DIR_TONET;
    }

    TALK_INFO("payload type[%d]", iPayloadType);

    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        // 1/0 auto/inband
        if (configParser_GetConfigInt(kszVoipUserCapEnable) == 0)
        {
            bSendInband = true;
        }
        // auto
        else if (iPayloadType == 0)
        {
            bSendInband = true;
        }
        else if (iPayloadType == 128)
        {
            bSendInfo = true;
        }
        else
        {
            bSend2833 = true;
        }
    }
    else
    {
        int iDtmfType = configParser_GetCfgItemIntValue(kszAccountDTMFType, iAccountID);
        switch (iDtmfType)
        {
        case DTMF_INBAND:
            {
                bSendInband = true;
            }
            break;
        case DTMF_RFC2833:
            {
                bSend2833 = true;
            }
            break;
        case DTMF_SIPINFO:
            {
                bSendInfo = true;
            }
            break;
        case DTMF_AUTO_SIPINFO:
            {
                bSendInfo = true;
                bSendInband = true;
            }
            break;
        default:
            break;
        }
    }

    if (bSend2833 || bSendInband)
    {
        SignalParams sigp(m_iSessionID);
        sigp.wParam.iValue = iAccountID;
        sigp.lParam.iValue = nKey;
        modtklsn_emit(TALK_SIG_CALL_SEND_REMOTE_DTMF, sigp);

        // event_ptype ipvp会根据此字段决定：0 inband；非0 2833
        // 这里向VPM发送DTMF消息，同时带上CallId为-1标示为只是发DTMF音
#ifdef IF_BT_SUPPORT_PHONE
        if (talklogic_IsBluetoothSessionByCallID(iCallID))
        {
            Bluetooth_SendMobileDTMF(iCallID, nKey);
        }
        else
#endif
        {
            voice_SendRemoteDTMF(iCallID, nKey, eDtmfDir);
        }
    }

    if (bSendInfo)
    {
        //Sip Info Type
        modVoip_SendDtmf(iAccountID, iCallID, nKey);
    }

    return true;
}

bool CSendDTMFASyncProxy::SendOnce()
{
    LIST_DTMF::iterator it = m_listDTMF.begin();
    if (it == m_listDTMF.end())
    {
        return true;
    }

    CDTMFDetails& data = *it;
    yl::string& strDTMF = data.strValue;

    /* 为空但是时间不为0，则执行一个等待，浪费一个周期 */
    if (strDTMF.empty() && data.interval > 0)
    {
        SetTimer(data.interval);
    }

    /* 如果发送失败直接发下一个，直到发送成功才执行等待 */
    while (true)
    {
        if (strDTMF.empty())
        {
            break;
        }

        yl::string::iterator beg = strDTMF.begin();
        bool bSend = SendDTMF(*beg, data.interval);
        strDTMF.erase(beg);

        if (bSend)
        {
            break;
        }
    };

    if (strDTMF.empty())
    {
        it = m_listDTMF.erase(it);
    }

    /* 所有都发完了退出 */
    if (it == m_listDTMF.end())
    {
        Kill();

        modtklsn_emit(TALK_SIG_SESSION_DTMF_FINISH, m_iSessionID);
    }

    return true;
}

bool CSendDTMFASyncProxy::OnCallBack()
{
    return SendOnce();
}

bool CSendDTMFASyncProxy::Send(const yl::string& str, int interval/* = 0*/)
{
    /* 一定要有延时，低于1的使用默认值 */
    if (interval <= 0)
    {
        interval = DELAY_DTMF_TIME;

        using namespace yl;
        int iOnTime = configParser_GetConfigInt(kszToneDtmfOntime);
        iOnTime = max(iOnTime, LOCALDTMF_ONTIME_DEF);
        int iOffTime = configParser_GetConfigInt(kszToneDtmfOftime);
        iOffTime = max(iOffTime, LOCALDTMF_OFFTIME_DEF);

        // 默认值，使用默认延时
        if (iOffTime != LOCALDTMF_OFFTIME_DEF
                || iOnTime != LOCALDTMF_ONTIME_DEF)
        {
            interval = iOffTime + iOnTime;
        }
    }

    m_listDTMF.push_back(CDTMFDetails(interval, str));
    /* 立即开始发送 */
    return SendOnce();
}
